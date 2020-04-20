
// MIT License
//
// Petter Strandmark 2013
// petter.strandmark@gmail.com
//
// Copyright (c) 2020 degski
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

// Monte Carlo Tree Search for finite games.
//
// Originally based on Python code at http://mcts.ai/code/python.html
//
// Uses the "root parallelization" technique [1].
//
// This game engine can play any game defined by a state like this:
//
// class GameState {
//
// public:
//     typedef int Move;
//     static Move const no_move = ...
//
//     void do_move(Move move);
//     template<typename RandomEngine>
//     void do_random_move(*engine);
//     bool has_moves() const;
//     std::std::vector<Move> get_moves() const;
//
//     // Returns a value in {0, 0.5, 1}.
//     // This should not be an evaluation function, because it will only be
//     // called for finished games. Return 0.5 to indicate a draw.
//     float get_result(int current_player_to_move) const;
//
//     int player_to_move;
//
//     // ...
// private:
//     // ...
// };
//
// See the examples for more details. Given a suitable State, the
// following function (tries to) compute the best move for the
// player to move.
//
//
// [1] Chaslot, G. M. B., Winands, M. H., & van Den Herik, H. J. (2008).
//     Parallel monte-carlo tree search. In Computers and Games (pp.
//     60-71). Springer Berlin Heidelberg.

#pragma once

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <future>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include <sax/prng_sfc.hpp>
#include <sax/uniform_int_distribution.hpp>

#include "../../compact_vector/include/compact_vector.hpp"

namespace Mcts {

struct ComputeOptions {

    int number_of_threads;
    int max_iterations;
    float max_time;
    bool verbose;

    ComputeOptions ( ) :
        number_of_threads ( 4 ), max_iterations ( 1'000'000 ), max_time ( -1.0 ), // default is no time limit.
        verbose ( true ) {}
};

template<typename State>
typename State::Move compute_move ( State const root_state, ComputeOptions const options );

static void check ( bool expr, char const * message );
static void assertion_failed ( char const * expr, char const * file, int line );

#define attest( expr )                                                                                                             \
    if ( not( expr ) ) {                                                                                                           \
        ::Mcts::assertion_failed ( #expr, __FILE__, __LINE__ );                                                                    \
    }
#ifndef NDEBUG
#    define dattest( expr )                                                                                                        \
        if ( not( expr ) ) {                                                                                                       \
            ::Mcts::assertion_failed ( #expr, __FILE__, __LINE__ );                                                                \
        }
#else
#    define dattest( expr ) ( ( void ) 0 )
#endif

// This class is used to build the game tree. The root is created by the users and
// the rest of the tree is created by add_node.
template<typename State>
struct Node {

    using Move            = typename State::Move;
    using Moves           = typename State::Moves;
    using Player          = typename State::value_type;
    using moves_size_type = typename Moves::size_type;
    using Children        = sax::compact_vector<std::unique_ptr<Node>>;
    using ZobristHash     = typename State::ZobristHash;

    Node ( State const & state ) :
        parent ( nullptr ), player_to_move ( state.playerToMove ( ) ), visits ( 0 ), wins ( 0.0f ), moves ( state.get_moves ( ) ),
        UCT_score ( 0.0f ), hash ( state.zobrist ( ) ), move ( State::no_move ) {}

    private:
    Node ( State const & state, Move const & move_, Node * parent_ ) :
        parent ( parent_ ), player_to_move ( state.playerToMove ( ) ), visits ( 0 ), wins ( 0.0f ), moves ( state.get_moves ( ) ),
        UCT_score ( 0.0f ), hash ( state.zobrist ( ) ), move ( move_ ) {}

    public:
    Node ( Node const & )     = default;
    Node ( Node && ) noexcept = default;
    ~Node ( ) noexcept        = default;

    [[maybe_unused]] Node & operator= ( Node const & ) = default;
    [[maybe_unused]] Node & operator= ( Node && ) noexcept = default;

    [[nodiscard]] bool has_untried_moves ( ) const noexcept { return not moves.is_released ( ); }

    template<typename RandomEngine>
    [[nodiscard]] Move get_untried_move ( RandomEngine * engine ) noexcept { // removes move
        attest ( not moves.empty ( ) );
        if ( 1 == moves.size ( ) ) {
            Move m = moves.front ( );
            moves.reset ( );
            return m;
        }
        return moves.unordered_erase ( sax::uniform_int_distribution<moves_size_type> ( 0, moves.size ( ) - 1 ) ( *engine ) );
    }

    [[nodiscard]] Node * best_child ( ) const noexcept {
        attest ( moves.empty ( ) );
        attest ( not children.empty ( ) );
        return std::max_element ( children.begin ( ), children.end ( ),
                                  [] ( auto & a, auto & b ) noexcept { return a->visits < b->visits; } )
            ->get ( );
    }

    [[nodiscard]] Node * select_child_UCT ( ) const noexcept {
        attest ( not children.empty ( ) );
        for ( auto & child : children )
            child->UCT_score =
                ( static_cast<double> ( child->wins ) / 2.0 ) / static_cast<double> ( child->visits ) +
                std::sqrt ( 2.0 * std::log ( static_cast<double> ( this->visits ) ) / static_cast<double> ( child->visits ) );
        return std::max_element ( children.begin ( ), children.end ( ),
                                  [] ( auto & a, auto & b ) { return a->UCT_score < b->UCT_score; } )
            ->get ( );
    }

    [[nodiscard]] bool has_children ( ) const noexcept { return not children.empty ( ); }

    [[nodiscard]] Node * add_child ( Move const & move, State const & state ) {
        return children.emplace_back ( new Node{ state, move, this } ).get ( );
    }

    void update ( float result ) {
        visits++;
        wins += result;
    }

    std::string to_string ( ) const;
    std::string tree_to_string ( int max_depth = 1'000'000, int indent = 0 ) const;

    Node * parent;         // 8
    Player player_to_move; // 12
    int visits;            // 16
    float wins;            // 20
    Moves moves;           // 28
    Children children;     // 36

    private:
    std::string indent_string ( int indent ) const;

    float UCT_score; // 40

    public:
    /*
        [[nodiscard]] static void * operator new ( std::size_t n_size_ ) {
            if ( auto ptr = mi_malloc ( n_size_ ) )
                return ptr;
            else {
                std::cout << "malloc err\n";
                throw std::bad_alloc{ };
            }
        }

        static void operator delete ( void * ptr_ ) noexcept { mi_free ( ptr_ ); }
    */
    ZobristHash hash; // 48
    Move move;        // 50
};

template<typename State>
std::string Node<State>::to_string ( ) const {
    std::stringstream ss;
    ss << "["
       << "P" << 3 - player_to_move << " "
       << "M:" << move << " "
       << "W/V: " << ( static_cast<double> ( wins ) / 2.0 ) << "/" << visits << " "
       << "U: " << moves.size ( ) << "]\n";
    return ss.str ( );
}

template<typename State>
std::string Node<State>::tree_to_string ( int max_depth, int indent ) const {
    if ( indent >= max_depth )
        return "";
    std::string s = indent_string ( indent ) + to_string ( );
    for ( auto child : children )
        s += child->tree_to_string ( max_depth, indent + 1 );
    return s;
}

template<typename State>
std::string Node<State>::indent_string ( int indent ) const {
    std::string s = "";
    for ( int i = 1; i <= indent; ++i )
        s += "| ";
    return s;
}

// Walltime.
inline double wall_time ( ) noexcept {
    using Clock = std::chrono::high_resolution_clock;
    return double ( Clock::now ( ).time_since_epoch ( ).count ( ) ) * double ( Clock::period::num ) / double ( Clock::period::den );
}

template<typename State>
std::unique_ptr<Node<State>> compute_tree ( State const root_state, ComputeOptions const options, sax::Rng::result_type seed_ ) {
    static_assert ( std::is_copy_assignable<Node<State>>::value, "Node<State> is not copy-assignable" );
    static_assert ( std::is_move_assignable<Node<State>>::value, "Node<State> is not move-assignable" );
    sax::Rng random_engine ( seed_ );
    attest ( options.max_iterations >= 0 or options.max_time >= 0 );
    auto root         = std::unique_ptr<Node<State>> ( new Node<State> ( root_state ) );
    double start_time = wall_time ( );
    double print_time = start_time;
    for ( int iter = 1; iter <= options.max_iterations or options.max_iterations < 0; ++iter ) {
        auto node   = root.get ( );
        State state = root_state;
        while ( not node->has_untried_moves ( ) and node->has_children ( ) ) {
            node = node->select_child_UCT ( );
            state.do_move ( node->move );
        }
        if ( node->has_untried_moves ( ) ) {
            auto move = node->get_untried_move ( &random_engine );
            state.do_move ( move );
            node = node->add_child ( move, state );
        }
        state.simulate ( );
        while ( node ) {
            node->update ( state.get_result ( node->player_to_move ) );
            node = node->parent;
        }
        if ( options.verbose or options.max_time >= 0 ) {
            double time = wall_time ( );
            if ( options.verbose && ( time - print_time >= 1.0 or iter == options.max_iterations ) ) {
                std::cerr << iter << " games played (" << double ( iter ) / ( time - start_time ) << " / second)." << std::endl;
                print_time = time;
            }

            if ( time - start_time >= options.max_time )
                break;
        }
    }
    return root;
}

template<typename State>
typename State::Move compute_move ( State const root_state, ComputeOptions const options ) {
    auto moves = root_state.get_moves ( );
    attest ( moves.size ( ) > 0 );
    if ( moves.size ( ) == 1 )
        return moves[ 0 ];
    double start_time = wall_time ( );
    // Start all jobs to compute trees.
    std::vector<std::future<std::unique_ptr<Node<State>>>> root_futures;
    ComputeOptions job_options = options;
    job_options.verbose        = false;
    for ( int t = 0; t < options.number_of_threads; ++t ) {
        auto func = [ t, &root_state, &job_options ] ( ) -> std::unique_ptr<Node<State>> {
            return compute_tree ( root_state, job_options, 18'446'744'073'709'551'557ull * t + 0x0fce58188743146dull );
        };
        root_futures.push_back ( std::async ( std::launch::async, func ) );
    }
    // Collect the results.
    std::vector<std::unique_ptr<Node<State>>> roots;
    for ( int t = 0; t < options.number_of_threads; ++t )
        roots.push_back ( std::move ( root_futures[ t ].get ( ) ) );
    // Merge the children of all root nodes.
    std::map<typename State::Move, int> visits;
    std::map<typename State::Move, int> wins;
    std::int64_t games_played = 0;
    for ( int t = 0; t < options.number_of_threads; ++t ) {
        auto root = roots[ t ].get ( );
        games_played += root->visits;
        for ( auto child = root->children.cbegin ( ); child != root->children.cend ( ); ++child ) {
            visits[ ( *child )->move ] += ( *child )->visits;
            wins[ ( *child )->move ] += ( *child )->wins;
        }
    }
    // Find the node with the highest score.
    float best_score               = -1;
    typename State::Move best_move = typename State::Move ( );
    for ( auto itr : visits ) {
        auto move = itr.first;
        float v   = itr.second;
        float w   = wins[ move ];
        // Expected success rate assuming a uniform prior (Beta(1, 1)).
        // https://en.wikipedia.org/wiki/Beta_distribution
        float expected_success_rate = ( w + 1 ) / ( v + 2 );
        if ( expected_success_rate > best_score ) {
            best_move  = move;
            best_score = expected_success_rate;
        }
        if ( options.verbose ) {
            std::cerr << "Move: " << itr.first << " (" << std::setw ( 2 ) << std::right
                      << int ( 100.0 * v / float ( games_played ) + 0.5 ) << "% visits)"
                      << " (" << std::setw ( 2 ) << std::right << int ( 100.0 * w / v + 0.5 ) << "% wins)" << std::endl;
        }
    }
    if ( options.verbose ) {
        auto best_wins   = wins[ best_move ];
        auto best_visits = visits[ best_move ];
        std::cerr << "----" << std::endl;
        std::cerr << "Best: " << best_move << " (" << 100.0 * best_visits / float ( games_played ) << "% visits)"
                  << " (" << 100.0 * best_wins / best_visits << "% wins)" << std::endl;
    }
    if ( options.verbose ) {
        float time = wall_time ( );
        std::cerr << games_played << " games played in " << float ( time - start_time ) << " s. "
                  << "(" << float ( games_played ) / ( time - start_time ) << " / second, " << options.number_of_threads
                  << " parallel jobs)." << std::endl;
    }
    return best_move;
}

inline void check ( bool expr, char const * message ) {
    if ( not expr )
        throw std::invalid_argument ( message );
}

inline void assertion_failed ( char const * expr, char const * file_cstr, int line ) {
    // Extract the file name only.
    std::string file ( file_cstr );
    auto pos = file.find_last_of ( "/\\" );
    if ( pos == std::string::npos )
        pos = 0;
    file = file.substr ( pos + 1 ); // Returns empty string if pos + 1 == length.
    std::stringstream ss;
    ss << "Assertion failed: " << expr << " in " << file << ":" << line << ".";
    throw std::runtime_error ( ss.str ( ).c_str ( ) );
}

} // namespace Mcts
