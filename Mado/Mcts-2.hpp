
// MIT License
//
// Copyright (c) 2019, 2020 degski
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

#include <cstdint>
#include <cstdlib>
#include <cmath>

#include <sax/iostream.hpp>
#include <random>
#include <unordered_map>
#include <vector>

#include <experimental/fixed_capacity_vector>

#include <boost/container/deque.hpp>
#include <boost/dynamic_bitset.hpp>

#include <cereal/cereal.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/archives/binary.hpp>

#include <sax/uniform_int_distribution.hpp>
#include <sax/memorypool.hpp>

#include "Globals.hpp"

#include "../../compact_vector/include/compact_vector.hpp"
#include "../../MCTSSearchTree/include/flat_search_tree.hpp"

namespace mcts {

template<typename T, int N = 128>
class Stack {

    std::vector<T> m_data;

    public:
    Stack ( T const & v_ ) noexcept {
        m_data.reserve ( N );
        m_data.push_back ( v_ );
    }
    Stack ( T && v_ ) noexcept {
        m_data.reserve ( N );
        m_data.emplace_back ( v_ );
    }

    [[nodiscard]] T const pop ( ) noexcept { return m_data.pop ( ); }

    void push ( T const v_ ) noexcept { m_data.push_back ( v_ ); }

    [[nodiscard]] bool not_empty ( ) const noexcept { return m_data.size ( ); }
};

template<typename T>
class Queue {

    boost::container::deque<T> m_data;

    public:
    Queue ( T const & v_ ) noexcept { m_data.push_back ( v_ ); }
    Queue ( T && v_ ) noexcept { m_data.emplace_back ( v_ ); }

    [[nodiscard]] T pop ( ) noexcept {
        T const v = m_data.front ( );
        m_data.pop_front ( );
        return v;
    }

    void push ( T const v_ ) noexcept { m_data.push_back ( v_ ); }

    [[nodiscard]] bool not_empty ( ) const noexcept { return m_data.size ( ); }
};

template<typename State>
struct ArcData { // 2 bytes

    using Move = typename State::Move;

    Move m_move; // 2 bytes

    ArcData ( ) noexcept                 = default;
    ArcData ( ArcData const & ) noexcept = default;
    ArcData ( ArcData && ) noexcept      = default;
    ArcData ( State const & state_ ) noexcept : m_move{ state_.lastMove ( ) } {}

    ~ArcData ( ) noexcept = default;

    [[maybe_unused]] ArcData & operator+= ( ArcData const & rhs_ ) noexcept { return *this; }

    [[maybe_unused]] ArcData & operator= ( ArcData const & ad_ ) noexcept {
        m_move = ad_.m_move;
        return *this;
    }
    [[maybe_unused]] ArcData & operator= ( ArcData && ad_ ) noexcept {
        m_move = std::move ( ad_.m_move );
        return *this;
    }

    private:
    friend class cereal::access;

    template<class Archive>
    void serialize ( Archive & ar_ ) {
        ar_ ( m_move );
    }
};

template<typename State>
struct NodeData { // 24 bytes

    using Move        = typename State::Move;
    using Moves       = typename State::Moves;
    using ZobristHash = typename State::ZobristHash;
    using PlayerValue = typename State::value;
    using Player      = typename State::value_type;

    Moves m_moves;                                            // 8 bytes
    ZobristHash m_zobrist_hash = State::zobrist_hash_default; // 8 bytes
    float m_score              = 0.0f;                        // 4 bytes
    int m_visits               = 0;                           // 4 bytes
    Player m_player_just_moved = PlayerValue::invalid;        // 1 byte

    NodeData ( ) noexcept                  = default;
    NodeData ( NodeData const & ) noexcept = default;
    NodeData ( NodeData && ) noexcept      = default;
    NodeData ( State const & state_ ) noexcept :
        m_moves{ state_.availableMoves ( ) }, m_zobrist_hash{ state_.zobrist ( ) }, m_player_just_moved{
            state_.playerJustMoved ( )
        } {}

    ~NodeData ( ) noexcept = default;

    [[nodiscard]] Move getUntriedMove ( ) noexcept {
        if ( 1 == m_moves.size ( ) ) {
            Move m = m_moves.front ( );
            m_moves.reset ( );
            return m;
        }
        // Random draw.
        int const i              = sax::uniform_int_distribution<int> ( 0, m_moves.size ( ) - 1 ) ( Rng::generator ( ) );
        Move const m             = m_moves.operator[] ( i );
        m_moves.operator[] ( i ) = m_moves.back ( );
        m_moves.pop_back ( );
        return m;
    }

    void removeMove ( Move const m_ ) noexcept {
        m_moves.unordered_erase_v ( m_ );
        if ( m_moves.empty ( ) )
            m_moves.reset ( );
    }

    [[maybe_unused]] NodeData & operator= ( NodeData const & ) noexcept = default;
    [[maybe_unused]] NodeData & operator= ( NodeData && ) noexcept = default;

    [[nodiscard]] NodeData & operator+= ( NodeData const & rhs_ ) noexcept {
        m_score += rhs_.m_score;
        m_visits += rhs_.m_visits;
        return *this;
    }

    private:
    friend class cereal::access;

    template<class Archive>
    void serialize ( Archive & ar_ ) noexcept { // m_moves not serialized
        ar_ ( m_zobrist_hash, m_score, m_visits, m_player_just_moved );
    }
};

template<typename State>
using Tree = fst::SearchTree<ArcData<State>, NodeData<State>>;
template<typename State>
using NodeID = typename Tree<State>::NodeID;
template<typename State>
using ArcID = typename Tree<State>::ArcID;

template<typename State>
class Mcts {

    public:
    using Tree     = Tree<State>;
    using ArcID    = typename Tree::ArcID;
    using NodeID   = typename Tree::NodeID;
    using ArcData  = ArcData<State>;
    using NodeData = NodeData<State>;

    using InIt  = typename Tree::in_iterator;
    using OutIt = typename Tree::out_iterator;

    using CInIt  = typename Tree::const_in_iterator;
    using COutIt = typename Tree::const_out_iterator;

    using PlayerValue = typename State::value;
    using Player      = typename State::value_type;

    using Move  = typename State::Move;
    using Moves = std::vector<Move>;

    using Link = typename Tree::Link;
    using Path = typename Tree::Path;

    using ZobristHash = typename State::ZobristHash;

    using TranspositionTable = std::unordered_map<ZobristHash, NodeID>;

    // The data.

    Tree m_tree;
    TranspositionTable m_transposition_table;

    bool m_not_initialized = true;

    private:
    void initialize ( State const & state_ ) noexcept {
        // Set root_arc and root_node data.
        m_tree[ m_tree.root_arc ]  = ArcData ( state_ );
        m_tree[ m_tree.root_node ] = NodeData ( state_ );
        // Add root_node to transposition_table.
        m_transposition_table.emplace ( state_.zobrist ( ), m_tree.root_node );
        // Has been initialized.
        m_not_initialized = false;
    }

    public:
    [[nodiscard]] Link addArc ( NodeID const parent_, NodeID const child_, State const & state_ ) noexcept {
        return { m_tree.addArc ( parent_, child_, state_ ), child_ };
    }

    [[nodiscard]] Link addNode ( NodeID const parent_, State const & state_ ) noexcept {
        NodeID target = m_tree.addNode ( state_ );
        m_transposition_table.emplace ( state_.zobrist ( ), target );
        ArcID arc = m_tree.addArc ( parent_, target, state_ );
        return { std::move ( arc ), std::move ( target ) };
    }

    void printMoves ( NodeID const n_ ) const noexcept {
        std::cout << "moves of " << ( int ) n_ << ": ";
        for ( OutIt a ( m_tree, n_ ); a != OutIt::end ( ); ++a )
            std::cout << "[" << ( int ) a.get ( ) << ", " << ( int ) m_tree[ a ].m_move.m_loc << "]";
        putchar ( '\n' );
    }

    [[nodiscard]] Move getMove ( const ArcID arc_ ) const noexcept { return m_tree[ arc_ ].m_move; }

    [[nodiscard]] NodeID getNode ( const ZobristHash zobrist_ ) const noexcept {
        auto const it = m_transposition_table.find ( zobrist_ );
        return it == m_transposition_table.cend ( ) ? Tree::NodeID::invalid ( ) : it->second;
    }

    [[nodiscard]] bool hasChildren ( NodeID const node_ ) const noexcept { return m_tree.isInternal ( node_ ); }

    // Moves.

    [[nodiscard]] bool hasNoUntriedMoves ( NodeID const node_ ) const noexcept { return m_tree[ node_ ].m_moves.is_released ( ); }
    [[nodiscard]] bool hasUntriedMoves ( NodeID const node_ ) const noexcept { return not hasNoUntriedMoves ( node_ ); }
    [[nodiscard]] Move getUntriedMove ( NodeID const node_ ) noexcept { return m_tree[ node_ ].getUntriedMove ( ); }

    // Data.

    [[nodiscard]] int getVisits ( NodeID const node_ ) const noexcept {
        int visits = 0;
        for ( InIt a ( m_tree, node_ ); InIt::end ( ) != a; ++a )
            visits += m_tree[ a ].m_visits;
        return visits;
    }

    [[nodiscard]] float getUCTFromNode ( NodeID const parent_, NodeID const child_ ) const noexcept {
        //                              Exploitation                                                             Exploration
        // Exploitation is the task to select the Move that leads to the best results so far.
        // Exploration deals with less promising moves that still have to be examined, due to the uncertainty of the evaluation.
        return ( float ) m_tree[ child_ ].m_score / ( float ) m_tree[ child_ ].m_visits +
               std::sqrtf ( 2.0f * std::logf ( ( float ) ( m_tree[ parent_ ].m_visits ) ) / ( float ) m_tree[ child_ ].m_visits );
    }

    [[nodiscard]] Link selectChildUCT ( NodeID const parent_ ) const noexcept {
        COutIt a ( m_tree, parent_ );
        using children_static_vector = std::experimental::fixed_capacity_vector<Link, State::max_no_moves>;
        children_static_vector best_children ( 1, m_tree.link ( a ) );
        float best_UCT_score = getUCTFromNode ( parent_, best_children.back ( ).target );
        ++a;
        for ( ; a.is_valid ( ); ++a ) {
            Link const child      = m_tree.link ( a );
            float const UCT_score = getUCTFromNode ( parent_, child.target );
            if ( UCT_score > best_UCT_score ) {
                best_children.resize ( 1 );
                best_children.back ( ) = child;
                best_UCT_score         = UCT_score;
            }
            else if ( UCT_score == best_UCT_score ) {
                best_children.push_back ( child );
            }
        }
        // Ties are broken by fair coin flips.
        return best_children.size ( ) == 1 ? best_children.back ( )
                                           : best_children[ std::uniform_int_distribution<ptrdiff_t> (
                                                 0, best_children.size ( ) - 1 ) ( Rng::generator ( ) ) ];
    }

    [[nodiscard]] Link addChild ( NodeID const parent_, State const & state_ ) noexcept {
        // State is updated to reflect Move.
        NodeID const child = getNode ( state_.zobrist ( ) );
        return child == Tree::NodeID::invalid ( ) ? addNode ( parent_, state_ ) : addArc ( parent_, child, state_ );
    }

    void updateData ( Link && link_, State const & state_ ) noexcept {
        ++m_tree[ link_.target ].m_visits;
        m_tree[ link_.target ].m_score += state_.result ( m_tree[ link_.target ].m_player_just_moved );
    }

    [[nodiscard]] Move getBestMove ( ) noexcept {
        // Find the node (the most robust) with the most visits.
        int best_child_visits = INT_MIN;
        Move best_child_move;
        for ( OutIt a ( m_tree, m_tree.root_node ); a.is_valid ( ); ++a ) {
            Link child       = m_tree.link ( a );
            int child_visits = m_tree[ child.target ].m_visits;
            if ( child_visits > best_child_visits ) {
                best_child_visits = child_visits;
                best_child_move   = m_tree[ child.arc ].m_move;
            }
        }
        return best_child_move;
    }

    [[nodiscard]] Move compute ( State const & state_, int max_iterations_ = 100'000 ) noexcept {
        initialize ( state_ );
        Path path ( m_tree.root_arc, m_tree.root_node );
        while ( max_iterations_-- > 0 ) {
            NodeID node = m_tree.root_node;
            State state ( state_ );
            while ( hasNoUntriedMoves ( node ) and hasChildren ( node ) ) {
                Link child = selectChildUCT ( node );
                node       = child.target;
                state.moveHash ( m_tree[ child.arc ].m_move );
                path.emplace ( std::move ( child ) );
            }
            if ( hasUntriedMoves ( node ) ) {
                state.moveHashWinner ( getUntriedMove ( node ) );
                path.emplace ( addChild ( node, state ) );
            }
            for ( int i = 0; i < 1; ++i ) {
                State monte_carlo_state ( state );
                monte_carlo_state.simulate ( );
                for ( Link link : path )
                    updateData ( std::move ( link ), monte_carlo_state );
            }
            path.resize ( 1u );
        }
        return getBestMove ( );
    }

    private:
    friend class cereal::access;

    template<class Archive>
    void serialize ( Archive & ar_ ) const noexcept {
        ar_ ( m_tree, m_transposition_table, m_not_initialized );
    }
};

template<typename State>
void computeMove ( State & state_, int max_iterations_ = 100'000 ) noexcept {
    Mcts<State> * mcts = new Mcts<State> ( );
    state_.moveHashWinner ( mcts->compute ( state_, max_iterations_ ) );
    delete mcts;
}

} // namespace mcts
