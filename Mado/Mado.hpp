
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
#include <iostream>
#include <optional>
#include <random>
#include <type_traits>
#include <vector>

#include <experimental/fixed_capacity_vector>

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>

#include "../../KD-Tree/KD-Tree/sorted_vector_set.hpp"
#include "../../MCTSSearchTree/include/flat_search_tree.hpp"

#ifndef nl
#define nl '\n'
#endif

#include "Types.hpp"
#include "Player.hpp"
#include "Move.hpp"


template<std::intptr_t R, bool zero_base = true>
struct Hex {

    static_assert ( R > 0, "the radius should be larger than 0" );

    using value_type = sidx<R>;

    [[ nodiscard ]] static constexpr value_type radius ( ) noexcept {
        return R;
    }

    value_type q = value_type { -Hex::radius ( ) - 1 }, r = value_type { -Hex::radius ( ) - 1 };

    void nil ( ) noexcept {
        q = value_type { -Hex::radius ( ) - 1 }; r = value_type { -Hex::radius ( ) - 1 };
    }

    [[ nodiscard ]] bool operator == ( const Hex & rhs_ ) const noexcept {
        return q == rhs_.q and r == rhs_.r;
    }
    [[ nodiscard ]] bool operator != ( const Hex & rhs_ ) const noexcept {
        return q != rhs_.q or r != rhs_.r;
    }

    [[ nodiscard ]] inline bool in_valid ( ) const noexcept {
        if constexpr ( zero_base ) {
            return std::abs ( q ) > radius ( ) or std::abs ( r ) > radius ( ) or std::abs ( -q - r ) > radius ( );
        }
        else {
            return std::abs ( q - radius ( ) ) > radius ( ) or std::abs ( r - radius ( ) ) > radius ( ) or std::abs ( -q - r + ( 2 * radius ( ) ) ) > radius ( );
        }
    }
    [[ nodiscard ]] static constexpr bool in_valid ( value_type q_, value_type r_ ) noexcept {
        if constexpr ( zero_base ) {
            return std::abs ( q_ ) > radius ( ) or std::abs ( r_ ) > radius ( ) or std::abs ( -q_ - r_ ) > radius ( );
        }
        else {
            return std::abs ( q_ - radius ( ) ) > radius ( ) or std::abs ( r_ - radius ( ) ) > radius ( ) or std::abs ( -q_ - r_ + ( 2 * radius ( ) ) ) > radius ( );
        }
    }
    [[ nodiscard ]] inline bool valid ( ) const noexcept {
        return not ( in_valid ( ) );
    }
    [[ nodiscard ]] static constexpr bool valid ( value_type q_, value_type r_ ) noexcept {
        return not ( in_valid ( q_, r_ ) );
    }

    template<typename Stream>
    [[ maybe_unused ]] friend Stream & operator << ( Stream & out_, const Hex & h_ ) noexcept {
        out_ << '<' << h_.q << ' ' << h_.r << '>';
        return out_;
    }
};



template<std::size_t R, typename T = std::int8_t>
struct Mado {

    static_assert ( std::conjunction<std::is_signed<T>, std::is_integral<T>>::value, "signed integer types only" );

    [[ nodiscard ]] static constexpr std::intptr_t radius ( ) noexcept {
        return static_cast<std::intptr_t> ( R );
    }

    [[ nodiscard ]] static constexpr std::size_t size ( ) noexcept {
        return static_cast<std::size_t> ( 1 + 3 * radius ( ) * ( radius ( ) + 1 ) );
    }

    [[ nodiscard ]] static constexpr std::intptr_t width ( ) noexcept {
        return static_cast<std::intptr_t> ( 2 * radius ( ) + 1 );
    }
    [[ nodiscard ]] static constexpr std::intptr_t height ( ) noexcept {
        return static_cast<std::intptr_t> ( 2 * radius ( ) + 1 );
    }

    [[ nodiscard ]] static constexpr std::intptr_t cols ( ) noexcept {
        return 2 * width ( ) + 1;
    }
    [[ nodiscard ]] static constexpr std::intptr_t rows ( ) noexcept {
        return height ( ) + 2;
    }

    using uidx = uidx<radius ( )>;
    using sidx = sidx<radius ( )>;

    [[ nodiscard ]] static constexpr uidx center ( ) noexcept {
        return static_cast<uidx> ( cols ( ) * ( radius ( ) + 1 ) + width ( ) );
    }

    using Hex = Hex<radius ( )>;

    // https://www.redblobgames.com/grids/hexagons/

    [[ nodiscard ]] static constexpr uidx hex_to_idx ( const sidx q_, const sidx r_ ) noexcept {
        return center ( ) + 2 * q_ + ( cols ( ) + 1 ) * r_;
    }
    [[ nodiscard ]] static constexpr uidx hex_to_idx ( const Hex & h_ ) noexcept {
        return center ( ) + 2 * h_.q + ( cols ( ) + 1 ) * h_.r;
    }
    [[ nodiscard ]] static constexpr Hex idx_to_hex ( const uidx i_ ) noexcept {
        Hex h { i_, i_ };
        h.r /= cols ( );
        h.q -= center ( );
        h.r -= rows ( ) / 2;
        h.q -= ( cols ( ) + 1 ) * h.r;
        h.q /= 2;
        return h;
    }


    using move = Move<radius ( )>;

    using value = typename Player<radius ( )>::Type;
    using value_type = Player<radius ( )>;
    using pointer = Player<radius ( )> * ;
    using const_pointer = Player<radius ( )> const *;

    using board = std::array<value_type, cols ( ) * rows ( )>;
    using indices = std::array<uidx, size ( )>;

    using zobrist_hash = std::uint64_t;

    using surrounded_vector = std::experimental::fixed_capacity_vector<sidx, 6>;

    [[ nodiscard ]] uidx north_east ( const uidx o_ ) const noexcept {
        static_assert ( cols ( ) > 0, "number of columns has to be larger than 0" );
        assert ( o_ >= ( cols ( ) - 1 ) );
        return o_ - uidx { cols ( ) - 1 };
    }
    [[ nodiscard ]] uidx east ( const uidx o_ ) const noexcept {
        return o_ + uidx { 2 };
    }
    [[ nodiscard ]] uidx south_east ( const uidx o_ ) const noexcept {
        return o_ + uidx { cols ( ) + 1 };
    }
    [[ nodiscard ]] uidx south_west ( const uidx o_ ) const noexcept {
        static_assert ( cols ( ) > 0, "number of columns has to be larger than 0" );
        return o_ + uidx { cols ( ) - 1 };
    }
    [[ nodiscard ]] uidx west ( const uidx o_ ) const noexcept {
        assert ( o_ >= 2 );
        return o_ - uidx { 2 };
    }
    [[ nodiscard ]] uidx north_west ( const uidx o_ ) const noexcept {
        assert ( o_ >= ( cols ( ) + 1 ) );
        return o_ - uidx { cols ( ) + 1 };
    }


    void init ( ) noexcept {
        uidx idx = center ( );
        m_board [ idx ] = value::vacant;
        std::size_t i = 0u;
        m_indcs [ i++ ] = idx;
        for ( int ring = 1; ring < int { radius ( ) + 1 }; ++ring ) {
            idx = east ( idx ); // move to next ring.
            for ( int j = 0; j < ring; ++j ) {
                idx = north_west ( idx );
                m_board [ idx ] = value::vacant;
                m_indcs [ i++ ] = idx;
            }
            for ( int j = 0; j < ring; ++j ) {
                idx = west ( idx );
                m_board [ idx ] = value::vacant;
                m_indcs [ i++ ] = idx;
            }
            for ( int j = 0; j < ring; ++j ) {
                idx = south_west ( idx );
                m_board [ idx ] = value::vacant;
                m_indcs [ i++ ] = idx;
            }
            for ( int j = 0; j < ring; ++j ) {
                idx = south_east ( idx );
                m_board [ idx ] = value::vacant;
                m_indcs [ i++ ] = idx;
            }
            for ( int j = 0; j < ring; ++j ) {
                idx = east ( idx );
                m_board [ idx ] = value::vacant;
                m_indcs [ i++ ] = idx;
            }
            for ( int j = 0; j < ring; ++j ) {
                idx = north_east ( idx );
                m_board [ idx ] = value::vacant;
                m_indcs [ i++ ] = idx;
            }
        }
        std::sort ( std::begin ( m_indcs ), std::end ( m_indcs ) );
    }


    board m_board;
    indices m_indcs;
    zobrist_hash m_zobrist_hash = 0xb735a0f5839e4e22; // Hash of the current m_board, some random initial value;

    int m_slides = 0;

    move m_last_move;

    value_type m_player_to_move = value::human; // value_type::random ( ),
    value_type m_winner = value::invalid;

    Mado ( ) noexcept {
        init ( );
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
        if ( std::numeric_limits<uidx>::max ( ) == move_.from ) { // Place.
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
        if ( std::numeric_limits<uidx>::max ( ) == move_.from ) { // Place.
            m_slides = 0;
        }
        else { // Slide.
            ++m_slides;
            m_board [ move_.from ] = value::vacant;
        }
        m_board [ move_.to ] = m_player_to_move;
    }

    private:

    [[ nodiscard ]] bool is_surrounded ( const uidx idx_ ) const noexcept {
        return
            m_board [ north_west ( idx_ ) ].is_not_vacant ( ) and
            m_board [ north_east ( idx_ ) ].is_not_vacant ( ) and
            m_board [       west ( idx_ ) ].is_not_vacant ( ) and
            m_board [       east ( idx_ ) ].is_not_vacant ( ) and
            m_board [ south_west ( idx_ ) ].is_not_vacant ( ) and
            m_board [ south_east ( idx_ ) ].is_not_vacant ( );
    }

    void emplace_back_surrounded ( surrounded_vector & s_, uidx && idx_ ) const noexcept {
        if ( m_board [ idx_ ].occupied ( ) and is_surrounded ( idx_ ) ) {
            s_.emplace_back ( std::move ( idx_ ) );
        }
    }

    public:

    // Return all the surrounded stones in s_ (after a place/slide).
    void find_surrounded ( surrounded_vector & s_, const uidx idx_ ) const noexcept {
        emplace_back_surrounded ( s_, north_west ( idx_ ) );
        emplace_back_surrounded ( s_, north_east ( idx_ ) );
        emplace_back_surrounded ( s_,       east ( idx_ ) );
        emplace_back_surrounded ( s_,       west ( idx_ ) );
        emplace_back_surrounded ( s_, south_west ( idx_ ) );
        emplace_back_surrounded ( s_, south_west ( idx_ ) );
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
        if ( surrounded.size ( ) ) {
            m_winner = std::end ( surrounded ) == std::find ( std::begin ( surrounded ), std::end ( surrounded ), m_player_to_move ) ? m_player_to_move : m_player_to_move.opponent ( );
        }
    }

    void move_hash ( const move & move_ ) noexcept {
        m_last_move = move_;
        move_hash_impl ( move_ );
        m_player_to_move.next ( );
        std::cout << *this << nl;
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
            for ( auto i : m_indcs ) {
                // Find places.
                if ( m_board [ i ].is_vacant ( ) ) {
                    moves_->emplace_back ( i );
                    continue;
                }
                // Find slides.
                if ( m_player_to_move == m_board [ i ] ) {
                    if ( m_board [ north_west ( i ) ].is_vacant ( ) )
                        moves_->emplace_back ( i, north_west ( i ) );
                    if ( m_board [ north_east ( i ) ].is_vacant ( ) )
                        moves_->emplace_back ( i, north_east ( i ) );
                    if ( m_board [ west ( i ) ].is_vacant ( ) )
                        moves_->emplace_back ( i, west ( i ) );
                    if ( m_board [ east ( i ) ].is_vacant ( ) )
                        moves_->emplace_back ( i, east ( i ) );
                    if ( m_board [ south_west ( i ) ].is_vacant ( ) )
                        moves_->emplace_back ( i, south_west ( i ) );
                    if ( m_board [ south_east ( i ) ].is_vacant ( ) )
                        moves_->emplace_back ( i, south_east ( i ) );
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
    void set ( Hex & h_, value_type v_ ) noexcept {
        m_board [ hex_to_idx ( h_ ) ] = v_;
    }
    void set ( sidx q_, sidx r_, value_type v_ ) noexcept {
        m_board [ hex_to_idx ( q_, r_ ) ] = v_;
    }

    template<typename Stream>
    [[ maybe_unused ]] friend Stream & operator << ( Stream & out_, const Mado & b_ ) noexcept {
        for ( std::size_t r = 0u; r < rows ( ); ++r ) {
            for ( std::size_t c = 0u; c < cols ( ); ++c ) {
                out_ << b_.m_board [ r * cols ( ) + c ] << ' ';
            }
            out_ << nl;
        }
        out_ << nl << "  hash 0x" << std::hex << b_.m_zobrist_hash << " slides " << b_.m_slides << nl;
        return out_;
    }
};
