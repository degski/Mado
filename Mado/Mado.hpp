
// MIT License
//
// Copyright (c) 2019 degski
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <algorithm>
#include <array>
#include <sax/iostream.hpp>
#include <optional>
#include <random>
#include <thread>
#include <type_traits>
#include <vector>

#include <SFML/Extensions.hpp>

#include <experimental/fixed_capacity_vector>

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>

#include <sax/stl.hpp>

#include "Player.hpp"
#include "Hexcontainer.hpp"
#include "Move.hpp"


template<int R>
struct Mado {

    using hex = Hex<R, true>;
    using idx_type = typename hex::idx_type;

    using move = Move<R>;

    using value = typename Player<R>::Type;
    using value_type = Player<R>;
    using pointer = value_type * ;
    using const_pointer = value_type const *;

    using board = HexContainer<value_type, R, true>;
    using size_type = typename board::size_type;

    using zobrist_hash = std::uint64_t;

    using surrounded_player_vector = std::experimental::fixed_capacity_vector<value_type, 6>;

    board m_board;
    zobrist_hash m_zobrist_hash = 0xb735a0f5839e4e22; // Hash of the current m_board, some random initial value;

    int m_slides = 0;

    move m_last_move;

    value_type m_player_to_move = value::human; // value_type::random ( ),
    value_type m_winner = value::invalid;


    Mado ( ) noexcept {
    }

    void reset ( ) noexcept {
        m_board.reset ( );
        m_zobrist_hash = 0xb735a0f5839e4e22;
        m_slides = 0;
        m_player_to_move = value::human;
        m_winner = value::invalid;
    }

    // From SplitMix64, the mixer.
    [[ nodiscard ]] static constexpr std::uint64_t sm_mix64 ( std::uint64_t k_ ) noexcept {
        k_ = ( k_ ^ ( k_ >> 30 ) ) * std::uint64_t { 0xbf58476d1ce4e5b9 };
        k_ = ( k_ ^ ( k_ >> 27 ) ) * std::uint64_t { 0x94d049bb133111eb };
        return k_ ^ ( k_ >> 31 );
    }
    [[ nodiscard ]] static constexpr std::uint64_t iu_mix64 ( std::uint64_t k_  ) noexcept {
        k_ = ( k_ ^ ( k_ >> 32 ) ) * std::uint64_t { 0xd6e8feb86659fd93 };
        return k_ ^ ( k_ >> 32 );
    }
    // From MurMurHash, the mixer.
    [[ nodiscard ]] static constexpr std::uint64_t mm_mix64 ( std::uint64_t k_ ) noexcept {
        k_ = ( k_ ^ ( k_ >> 33 ) ) * std::uint64_t { 0xff51afd7ed558ccd };
        // k_ = ( k_ ^ ( k_ >> 33 ) ) * std::uint64_t { 0xc4ceb9fe1a85ec53 };
        return k_ ^ ( k_ >> 33 );
    }

    [[ nodiscard ]] static constexpr zobrist_hash hash ( value_type p_, const idx_type i_ ) noexcept {
        return iu_mix64 ( static_cast<std::uint64_t> ( p_.as_index ( ) ) ^ static_cast<std::uint64_t> ( i_ ) );
    }

    [[ nodiscard ]] zobrist_hash zobrist ( ) const noexcept {
        return m_zobrist_hash;
    }

    [[ nodiscard ]] value_type playerToMove ( ) const noexcept {
        return m_player_to_move;
    }
    [[ nodiscard ]] value_type playerJustMoved ( ) const noexcept {
        return m_player_to_move.opponent ( );
    }


    void move_hash_impl ( const move & move_ ) noexcept {
        if ( move_.is_placement ( ) ) { // Place.
            if ( m_slides )
                m_zobrist_hash ^= mm_mix64 ( static_cast<std::uint64_t> ( m_slides ) );
            m_slides = 0;
        }
        else { // Slide.
            if ( m_slides )
                m_zobrist_hash ^= mm_mix64 ( static_cast<std::uint64_t> ( m_slides ) );
            m_zobrist_hash ^= mm_mix64 ( static_cast<std::uint64_t> ( ++m_slides ) );
            m_zobrist_hash ^= hash ( m_player_to_move, move_.from );
            m_board [ move_.from ] = value::vacant;
        }
        m_board [ move_.to ] = m_player_to_move;
        m_zobrist_hash ^= hash ( m_player_to_move, move_.to );
        // Alternatingly hash-in and hash-out this value,
        // to add-in the current player.
        m_zobrist_hash ^= 0xa9063818575b53b7;
    }

    void move_impl ( const move & move_ ) noexcept {
        if ( move_.is_placement ( ) ) { // Place.
            m_slides = 0;
        }
        else { // Slide.
            ++m_slides;
            m_board [ move_.from ] = value::vacant;
        }
        m_board [ move_.to ] = m_player_to_move;
    }

    private:

    template<typename IdxType>
    [[ nodiscard ]] inline bool is_surrounded ( const IdxType idx_ ) const noexcept {
        for ( auto const neighbor_of_idx : board::neighbors [ idx_ ] )
            if ( m_board [ neighbor_of_idx ].vacant ( ) )
                return false;
        return true;
    }

    public:

    // Return all the surrounded stones in s_ (after a place/slide).
    template<typename IdxType>
    void find_surrounded_neighbors ( surrounded_player_vector & s_, const IdxType idx_ ) const noexcept {
        for ( auto const idx : board::neighbors [ idx_ ] )
            if ( m_board [ idx ].occupied ( ) and is_surrounded ( idx ) )
                s_.push_back ( m_board [ idx ] );
    }

    void winner ( ) noexcept {
        // To be called before the player swap, but after m_player_to_move made his move.
        // If both players slide three turns in a row (three slides for each player makes
        // six total), the game is a draw.
        if ( 6 == m_slides ) {
            m_winner = value::vacant;
            return;
        }
        // The object of the game is to surround any ONE of your opponent's stones.  You
        // surround a stone by filling in the spaces around it - a stone can be surrounded
        // by any combination of your stones, your opponent's stones and the edge of the
        // board.But be careful; if one of your stones is surrounded on your own turn
        // ( even if you surround one of your opponent's stones at the same time), you lose
        // the game!
        if ( is_surrounded ( m_last_move.to ) ) {
            m_winner = m_player_to_move.opponent ( );
            std::cout << "winner 1 " << m_winner << nl;
            return;
        }

        for ( auto const idx : board::neighbors [ m_last_move.to ] ) {
            if ( m_board [ idx ].occupied ( ) and is_surrounded ( idx ) ) {
                if ( m_player_to_move == m_board [ idx ] ) {
                    m_winner = m_player_to_move.opponent ( );
                    std::cout << "winner 2 " << m_winner << nl;
                    return;
                }
                // Continue to verify that the curent player did not surround himself,
                // in which case he loses, even if the current player surrounded the
                // opponent at the same time.
                m_winner = m_player_to_move;
                std::cout << "winner 3 " << m_winner << nl;
            }
        }

        /*

        surrounded_player_vector surrounded_players;
        find_surrounded_neighbors ( surrounded_players, m_last_move.to );
        if ( surrounded_players.size ( ) )
            // Iff one location is surrounded, iff it's not the current player, the current player wins.
            m_winner = ( std::end ( surrounded_players ) == std::find ( std::begin ( surrounded_players ), std::end ( surrounded_players ), m_player_to_move ) ? m_player_to_move : m_player_to_move.opponent ( ) );

        */
    }

    void move_hash ( const move & move_ ) noexcept {
        m_last_move = move_;
        move_hash_impl ( move_ );
        m_player_to_move.next ( );
        std::cout << * this << nl;
    }

    void move_winner ( const move & move_ ) noexcept {
        m_last_move = move_;
        move_impl ( move_ );
        winner ( );
        m_player_to_move.next ( );
    }

    void move_hash_winner ( const move & move_ ) noexcept {
        m_last_move = move_;
        move_hash_impl ( move_ );
        winner ( );
        m_player_to_move.next ( );
    }

    template<typename MovesContainerPtr>
    [[ nodiscard ]] bool availableMoves ( MovesContainerPtr moves_ ) const noexcept {
        // Mcts class takes/has ownership.
        for ( int i = 0; i < static_cast<int> ( board::size ( ) ); ++i ) {
            // Find places.
            if ( m_board [ i ].vacant ( ) ) {
                moves_->emplace_back ( i );
                continue;
            }
            // Find slides.
            if ( m_player_to_move == m_board [ i ] ) {
                for ( auto const to : board::neighbors [ i ] )
                    if ( m_board [ to ].vacant ( ) )
                        moves_->emplace_back ( i, to );
            }
        }
        return moves_->size ( );
    }


    [[ nodiscard ]] move get_random_move ( ) noexcept {
        sf::sleep ( sf::milliseconds ( sax::uniform_int_distribution<size_type> ( 500, 1'500 ) ( Rng::gen ( ) ) ) );
        static std::experimental::fixed_capacity_vector<move, board::size ( ) * 2> available_moves;
        available_moves.clear ( );
        if ( nonterminal ( ) and availableMoves ( & available_moves ) ) {
            return available_moves [ sax::uniform_int_distribution<size_type> ( 0, available_moves.size ( ) - 1 ) ( Rng::gen ( ) ) ];
        }
        else {
            std::cout << "game ended" << nl << nl;
            std::exit ( EXIT_SUCCESS );
            return move { };
        }
    }

    [[ nodiscard ]] std::optional<value_type> ended ( ) const noexcept {
        return m_winner.value == value::invalid ? std::optional<value_type> { } : std::optional<value_type> { m_winner };
    }

    [[ nodiscard ]] float result ( const value_type player_just_moved_ ) const noexcept {
        // Determine result: last player of path is the player to move.
        return m_winner.vacant ( ) ? 0.0f : ( m_winner == player_just_moved_ ? 1.0f : -1.0f );
    }

    [[ nodiscard ]] bool terminal ( ) const noexcept {
        return value::invalid != m_winner.value;
    }
    [[ nodiscard ]] bool nonterminal ( ) const noexcept {
        return value::invalid == m_winner.value;
    }

    [[ nodiscard ]] move lastMove ( ) const noexcept {
        return m_last_move;
    }

    template<typename Stream>
    [[ maybe_unused ]] friend Stream & operator << ( Stream & out_, const Mado & b_ ) noexcept {
        out_ << b_.m_board << nl;
        out_ << nl << "  hash 0x" << std::hex << b_.m_zobrist_hash << " slides " << b_.m_slides << nl;
        return out_;
    }
};
