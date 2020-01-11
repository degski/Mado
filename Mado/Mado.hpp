
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
#include <charconv>
#include <sax/iostream.hpp>
#include <optional>
#include <random>
#include <string_view>
#include <thread>
#include <type_traits>
#include <vector>

#include <SFML/Extensions.hpp>

#include <experimental/fixed_capacity_vector>

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>

#include <sax/singleton.hpp>
#include <sax/stl.hpp>
#include <sax/srwlock.hpp>

#include "Player.hpp"
#include "Hexcontainer.hpp"
#include "Move.hpp"

template<int R>
struct PositionData {

    using value_type = Player<R>;
    using Board      = HexContainer<value_type, R, true>;

    Board m_board;
    std::int8_t m_slides;
    value_type m_player_to_move; // = value_type::random ( );

    PositionData ( ) noexcept                      = default;
    PositionData ( PositionData const & ) noexcept = default;
    PositionData ( PositionData && ) noexcept      = default;

    ~PositionData ( ) noexcept = default;

    [[maybe_unused]] PositionData & operator= ( const PositionData & ) noexcept = default;
    [[maybe_unused]] PositionData & operator= ( PositionData && ) noexcept = default;

    private:
    friend class cereal::access;

    template<class Archive>
    void serialize ( Archive & ar_ ) {
        ar_ ( m_board );
        ar_ ( m_slides );
        ar_ ( m_player_to_move );
    }
};

template<int R>
using PositionDataVector = sax::singleton<std::vector<PositionData<R>>>;

template<int R>
class Mado {

    public:
    using Hex     = Hex<R, true>;
    using IdxType = typename Hex::IdxType;

    using Move  = Move<R>;
    using Moves = Moves<R>;

    using value      = typename Player<R>::Type;
    using value_type = Player<R>;

    using PositionData = PositionData<R>;
    using PDV          = PositionDataVector<R>;
    using Board        = typename PositionData::Board;
    using size_type    = typename Board::size_type;

    using Generator   = sax::Rng &;
    using ZobristHash = std::uint64_t;

    using SurroundedPlayerVector = std::experimental::fixed_capacity_vector<value_type, 6>;
    using MoveLock               = sax::SRWLock;

    private:
    PositionData m_pos;
    value_type m_winner;
    Generator m_generator;
    ZobristHash m_zobrist_hash; // Hash of the current m_board, some random initial value;
    Move m_last_move;

    public:
    static constexpr int const max_no_moves                 = 4096;
    static constexpr ZobristHash const zobrist_hash_default = 0xb735a0f5839e4e22;
    int move_no                                             = 0;

    static int max_moves_size;

    Mado ( ) noexcept : m_generator ( Rng::generator ( ) ) { reset ( ); }
    Mado ( Mado const & m_ ) noexcept :
        m_pos ( m_.m_pos ), m_winner ( m_.m_winner ), m_generator ( Rng::generator ( ) ), m_zobrist_hash ( m_.m_zobrist_hash ),
        m_last_move ( m_.m_last_move ), move_no ( m_.move_no ) {}
    Mado ( Mado && m_ ) noexcept = delete;

    ~Mado ( ) noexcept {}

    [[nodiscard]] Mado & operator= ( Mado const & m_ ) noexcept {
        m_pos          = m_.m_pos;
        m_winner       = m_.m_winner;
        m_zobrist_hash = m_.m_zobrist_hash;
        m_last_move    = m_.m_last_move;
        move_no        = m_.move_no;
    }
    [[nodiscard]] Mado & operator= ( Mado && m_ ) noexcept = delete;

    void reset ( ) noexcept {
        m_pos.m_board.reset ( );
        m_pos.m_slides         = 0;
        m_pos.m_player_to_move = value::human;
        m_winner               = value::invalid;
        m_zobrist_hash         = zobrist_hash_default;
        move_no                = 0;
    }

    [[nodiscard]] static constexpr ZobristHash hash ( value_type p_, IdxType const i_ ) noexcept {
        return iu_mix64 ( static_cast<std::uint64_t> ( p_.as_index ( ) ) ^ static_cast<std::uint64_t> ( i_ ) );
    }

    [[nodiscard]] ZobristHash zobrist ( ) const noexcept { return m_zobrist_hash; }

    [[nodiscard]] value_type playerToMove ( ) const noexcept { return m_pos.m_player_to_move; }
    [[nodiscard]] value_type playerJustMoved ( ) const noexcept { return m_pos.m_player_to_move.opponent ( ); }

    void addPositionData ( ) {
        if ( std::bernoulli_distribution ( 0.0025 ) ( m_generator ) )
            PDV::instance ( ).push_back ( m_pos );
    }

    void writePositionData ( ) {
        if ( std::bernoulli_distribution ( 0.0025 ) ( m_generator ) ) {
            static std::array<char, 21> str;
            if ( auto [ p, ec ] = std::to_chars ( str.data ( ), str.data ( ) + str.size ( ),
                                                  sax::uniform_int_distribution<std::uint64_t> ( ) ( m_generator ) );
                 ec == std::errc ( ) )
                saveToFileBin ( m_pos, "y://dict//", std::string_view ( str.data ( ), p - str.data ( ) ) );
        }
    }

    void moveHash ( Move const move_ ) noexcept {
        m_last_move = move_;
        moveHashImpl ( move_ );
        // std::cout << *this << nl;
        // writePositionData ( );
        m_pos.m_player_to_move.next ( );
    }

    void moveWinner ( Move const move_ ) noexcept {
        m_last_move = move_;
        moveImpl ( move_ );
        checkForWinner ( );
        // writePositionData ( );
        m_pos.m_player_to_move.next ( );
    }

    void moveHashWinner ( Move const move_ ) noexcept {
        if ( m_pos.m_board[ move_.to ] != value::vacant ) {
            std::cout << " to not valid " << move_ << nl;
        }
        m_last_move = move_;
        moveHashImpl ( move_ );
        checkForWinner ( );
        // writePositionData ( );
        m_pos.m_player_to_move.next ( );
    }

    template<typename MovesContainerPtr>
    [[nodiscard]] int availableMoves ( MovesContainerPtr moves_ ) const noexcept {
        // Mcts class takes/has ownership.
        for ( int s = static_cast<int> ( Board::size ( ) ), i = 0; i < s; ++i ) {
            // Find placements.
            if ( m_pos.m_board[ i ].vacant ( ) ) {
                moves_->emplace_back ( i );
                continue;
            }
            // Find slides.
            if ( m_pos.m_player_to_move == m_pos.m_board[ i ] ) {
                for ( auto const to : Board::neighbors[ i ] )
                    if ( m_pos.m_board[ to ].vacant ( ) )
                        moves_->emplace_back ( i, to );
            }
        }
        return static_cast<int> ( moves_->size ( ) );
    }

    [[nodiscard]] Move randomMove ( ) noexcept {
        std::experimental::fixed_capacity_vector<Move, std::size_t{ Board::size ( ) } * std::size_t{ 2 }> available_moves;
        return nonterminal ( ) and availableMoves ( &available_moves )
                   ? available_moves[ bounded_integer ( available_moves.size ( ) ) ]
                   : Move{ };
    }

    [[nodiscard]] Move randomMoveDelayed ( ) noexcept {
        sf::sleep ( sf::milliseconds ( bounded_integer ( std::size_t{ 500 }, std::size_t{ 1'500 } ) ) );
        return randomMove ( );
    }

    [[maybe_unused]] value_type simulate ( ) noexcept {
        std::experimental::fixed_capacity_vector<Move, std::size_t{ Board::size ( ) } * std::size_t{ 2 }> available_moves;
        while ( nonterminal ( ) and availableMoves ( &available_moves ) ) {
            // std::cout << *this << nl;
            auto const s = available_moves.size ( );
            moveWinner ( available_moves[ bounded_integer ( s ) ] );
            if ( max_moves_size < s )
                max_moves_size = s;
            available_moves.clear ( );
        }
        // std::cout << *this << nl;
        return m_winner;
    }

    [[nodiscard]] std::optional<value_type> ended ( ) const noexcept {
        return m_winner.invalid ( ) ? std::optional<value_type>{ } : std::optional<value_type>{ m_winner };
    }

    [[nodiscard]] float result ( ) const noexcept {
        // Determine result: last player of path is the player to Move. The role of the minus 1? By now the player to make
        // a move has be updated (has moved, the state is ready for the next move, including the player to move).
        return static_cast<float> ( static_cast<int> ( m_winner.as_index ( ) ) * ( -1 ) );
    }
    [[nodiscard]] float result ( value_type const player_just_moved_ ) const noexcept {
        // Determine result: last player of path is the player to Move.
        return m_winner.vacant ( ) ? 0.0f : ( m_winner == player_just_moved_ ? 1.0f : -1.0f );
    }

    [[nodiscard]] bool terminal ( ) const noexcept { return m_winner.valid ( ); }
    [[nodiscard]] bool nonterminal ( ) const noexcept { return m_winner.invalid ( ); }

    [[nodiscard]] value_type winner ( ) const noexcept { return m_winner; }

    [[nodiscard]] Move lastMove ( ) const noexcept { return m_last_move; }

    template<typename Stream>
    [[maybe_unused]] friend Stream & operator<< ( Stream & out_, Mado const & b_ ) noexcept {
        out_ << b_.m_pos.m_board << "  move " << b_.move_no << " hash 0x" << std::hex << b_.m_zobrist_hash << " slides "
             << static_cast<int> ( b_.m_pos.m_slides ) << " last move " << b_.m_last_move << nl;
        if ( b_.terminal ( ) )
            out_ << "  winner: " << b_.winner ( ) << nl;
        out_ << nl;
        return out_;
    }

    private:
    // From SplitMix64, the mixer.
    [[nodiscard]] static constexpr std::uint64_t sm_mix64 ( std::uint64_t k_ ) noexcept {
        k_ = ( k_ ^ ( k_ >> 30 ) ) * std::uint64_t{ 0xbf58476d1ce4e5b9 };
        k_ = ( k_ ^ ( k_ >> 27 ) ) * std::uint64_t{ 0x94d049bb133111eb };
        return k_ ^ ( k_ >> 31 );
    }
    // My mixer.
    [[nodiscard]] static constexpr std::uint64_t iu_mix64 ( std::uint64_t k_ ) noexcept {
        k_ = ( k_ ^ ( k_ >> 32 ) ) * std::uint64_t{ 0xd6e8feb86659fd93 };
        return k_ ^ ( k_ >> 32 );
    }
    // From MurMurHash, the mixer.
    [[nodiscard]] static constexpr std::uint64_t mm_mix64 ( std::uint64_t k_ ) noexcept {
        k_ = ( k_ ^ ( k_ >> 33 ) ) * std::uint64_t{ 0xff51afd7ed558ccd };
        // k_ = ( k_ ^ ( k_ >> 33 ) ) * std::uint64_t { 0xc4ceb9fe1a85ec53 };
        return k_ ^ ( k_ >> 33 );
    }

    // To be called before the player swap, but after m_player_to_move made his Move.
    void checkForWinner ( ) noexcept {
        // If both players slide three turns in a row (three slides for each player makes
        // six total), the game is a draw.
        if ( 6 == m_pos.m_slides ) {
            m_winner = value::vacant;
            return;
        }
        // The object of the game is to surround any one of your opponent's stones. You
        // surround a stone by filling in the spaces around it - a stone can be surrounded
        // by any combination of your stones, your opponent's stones and the edge of the
        // board. But be careful; if one of your stones is surrounded on your own turn
        // (even if you surround one of your opponent's stones at the same time), you lose
        // the game!
        if ( isSurrounded ( m_last_move.to ) ) {
            m_winner = m_pos.m_player_to_move.opponent ( );
            return;
        }
        for ( auto const neighbor : Board::neighbors[ m_last_move.to ] ) {
            if ( m_pos.m_board[ neighbor ].occupied ( ) and isSurrounded ( neighbor ) ) {
                if ( m_pos.m_player_to_move == m_pos.m_board[ neighbor ] ) {
                    m_winner = m_pos.m_player_to_move.opponent ( );
                    return;
                }
                // Continue to verify that the current player did not surround himself.
                m_winner = m_pos.m_player_to_move;
            }
        }
    }

    // Move and update zobrist-hash.
    void moveHashImpl ( Move const move_ ) noexcept {
        if ( move_.is_placement ( ) ) { // Place.
            if ( m_pos.m_slides )
                m_zobrist_hash ^= mm_mix64 ( static_cast<std::uint64_t> ( m_pos.m_slides ) );
            m_pos.m_slides = 0;
        }
        else { // Slide.
            if ( m_pos.m_slides )
                m_zobrist_hash ^= mm_mix64 ( static_cast<std::uint64_t> ( m_pos.m_slides ) );
            m_zobrist_hash ^= mm_mix64 ( static_cast<std::uint64_t> ( ++m_pos.m_slides ) );
            m_zobrist_hash ^= hash ( m_pos.m_player_to_move, move_.from );
            m_pos.m_board[ move_.from ] = value::vacant;
        }
        m_pos.m_board[ move_.to ] = m_pos.m_player_to_move;
        m_zobrist_hash ^= hash ( m_pos.m_player_to_move, move_.to );
        // Alternatingly hash-in and hash-out this value,
        // to add-in the current player.
        m_zobrist_hash ^= 0xa9063818575b53b7;
        ++move_no;
    }

    // Move (no update zobrist-hash).
    void moveImpl ( Move const move_ ) noexcept {
        if ( move_.is_placement ( ) ) { // Place.
            m_pos.m_slides = 0;
        }
        else { // Slide.
            ++m_pos.m_slides;
            m_pos.m_board[ move_.from ] = value::vacant;
        }
        m_pos.m_board[ move_.to ] = m_pos.m_player_to_move;
        ++move_no;
    }

    template<typename IdxType>
    [[nodiscard]] inline bool isSurrounded ( IdxType const idx_ ) const noexcept {
        for ( auto const neighbor : Board::neighbors[ idx_ ] )
            if ( m_pos.m_board[ neighbor ].vacant ( ) )
                return false;
        return true;
    }

    template<typename T>
    [[nodiscard]] inline T bounded_integer ( T const u_ ) const noexcept {
        return sax::uniform_int_distribution<T> ( 0, u_ - T{ 1 } ) ( m_generator );
    }
    template<typename T>
    [[nodiscard]] inline T bounded_integer ( T const l_, T const u_ ) const noexcept {
        return sax::uniform_int_distribution<T> ( l_, u_ - T{ 1 } ) ( m_generator );
    }

    template<typename T>
    void saveToFileBin ( T const & t_, sf::Path && path_, std::string_view && file_name_, bool const append_ = false ) noexcept {
        std::ofstream ostream ( path_ / file_name_,
                                append_ ? std::ios::binary | std::ios::app | std::ios::out : std::ios::binary | std::ios::out );
        {
            cereal::BinaryOutputArchive archive ( ostream );
            archive ( t_ );
        }
        ostream.flush ( );
        ostream.close ( );
    }

    friend class cereal::access;

    template<class Archive>
    void serialize ( Archive & ar_ ) {
        ar_ ( m_pos );
    }
};

template<int R>
int Mado<R>::max_moves_size = 0;
