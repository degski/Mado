
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
#include <type_traits>
#include <vector>

#include <experimental/fixed_capacity_vector>

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>

#include "Types.hpp"
#include "Player.hpp"
#include "Hexcontainer.hpp"
#include "Move.hpp"


template<int R>
struct Mado {

    using hex = Hex<R, true>;

    using uidx = uidx<R>;
    using sidx = sidx<R>;

    using move = Move<R>;

    using value = typename Player<R>::Type;
    using value_type = Player<R>;
    using pointer = Player<R> * ;
    using const_pointer = Player<R> const *;

    using board = HexContainer<Player<R>, R, true>;

    using zobrist_hash = std::uint64_t;

    using surrounded_vector = std::experimental::fixed_capacity_vector<value_type, 6>;

    board m_board;
    zobrist_hash m_zobrist_hash = 0xb735a0f5839e4e22; // Hash of the current m_board, some random initial value;

    int m_slides = 0;

    move m_last_move;

    value_type m_player_to_move = value::human; // value_type::random ( ),
    value_type m_winner = value::invalid;


    Mado ( ) noexcept {
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

    [[ nodiscard ]] static constexpr zobrist_hash hash ( value_type p_, uidx i_ ) noexcept {
        return mm_mix64 ( static_cast<std::uint64_t> ( p_.as_index ( ) ) ^ static_cast<std::uint64_t> ( i_ ) );
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
        static constexpr zobrist_hash hash_slide [ 7 ] { 0x0000000000000000, 0x2c507643dcca6c7f, 0x82701a7a226fb2db, 0xe2b504a067cdb385, 0x4c696ea2a6481a72, 0x18dfdafd5c21cf7b, 0xef66e9ab2c19a2d6 };
        if ( move_.is_placement ( ) ) { // Place.
            if ( m_slides )
                m_zobrist_hash ^= hash_slide [ m_slides ];
            m_slides = 0;
        }
        else { // Slide.
            if ( m_slides )
                m_zobrist_hash ^= hash_slide [ m_slides ];
            m_zobrist_hash ^= hash_slide [ ++m_slides ];
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
    [[ nodiscard ]] bool is_surrounded ( const IdxType idx_ ) const noexcept {
        for ( const auto neighbor_of_idx : board::neighbors [ idx_ ] )
            if ( m_board [ neighbor_of_idx ].is_vacant ( ) )
                return false;
        return true;
    }
    template<typename IdxType>
    void emplace_back_surrounded ( surrounded_vector & s_, IdxType && idx_ ) const noexcept {
        if ( m_board [ idx_ ].occupied ( ) and is_surrounded ( idx_ ) )
            s_.push_back ( m_board [ idx_ ] );
    }

    public:

    // Return all the surrounded stones in s_ (after a place/slide).
    template<typename IdxType>
    void find_surrounded ( surrounded_vector & s_, const IdxType idx_ ) const noexcept {
        for ( auto && i : board::neighbors [ idx_ ] )
            emplace_back_surrounded ( s_, i );
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
            return;
        }
        surrounded_vector surrounded;
        find_surrounded ( surrounded, m_last_move.to );
        if ( surrounded.size ( ) )
            m_winner = ( std::end ( surrounded ) == std::find ( std::begin ( surrounded ), std::end ( surrounded ), m_player_to_move ) ? m_player_to_move : m_player_to_move.opponent ( ) );
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

    [[ nodiscard ]] bool moves ( std::vector<move> * moves_ ) const noexcept {
        // Mcts class takes has ownership.
        if ( nonterminal ( ) ) {
            moves_->clear ( );
            for ( typename move::value_type i = 0; i < static_cast<typename move::value_type> ( board::size ( ) ); ++i ) {
                // Find places.
                if ( m_board [ i ].is_vacant ( ) ) {
                    moves_->emplace_back ( i );
                    continue;
                }
                // Find slides.
                if ( m_player_to_move == m_board [ i ] ) {
                    for ( typename move::value_type to : board::neighbors [ i ] )
                        if ( m_board [ to ].is_vacant ( ) )
                            moves_->emplace_back ( i, to );
                }
            }
            return moves_->size ( );
        }
        return false;
    }

    [[ nodiscard ]] std::optional<value_type> ended ( ) const noexcept {
        return m_winner.value == value::invalid ? std::optional<value_type> ( ) : std::optional<value_type> ( m_winner );
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

    // Input - output, just for dev.

    void set ( uidx x_, value_type v_ ) noexcept {
        m_board [ x_ ] = v_;
    }
    void set ( hex & h_, value_type v_ ) noexcept {
        m_board [ hex_to_idx ( h_ ) ] = v_;
    }
    void set ( sidx q_, sidx r_, value_type v_ ) noexcept {
        m_board [ hex_to_idx ( q_, r_ ) ] = v_;
    }

    template<typename Stream>
    [[ maybe_unused ]] friend Stream & operator << ( Stream & out_, const Mado & b_ ) noexcept {
        /*
        for ( std::size_t r = 0u; r < rows ( ); ++r ) {
            for ( std::size_t c = 0u; c < cols ( ); ++c ) {
                out_ << b_.m_board [ r * cols ( ) + c ] << ' ';
            }
            out_ << nl;
        }
        out_ << nl << "  hash 0x" << std::hex << b_.m_zobrist_hash << " slides " << b_.m_slides << nl;
        */
        return out_;
    }
};
