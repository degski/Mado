
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

#include <cstdint>
#include <cstdlib>
#include <cmath>

#include <sax/iostream.hpp>
#include <random>
#include <unordered_map>
#include <vector>

#include <boost/container/deque.hpp>
#include <boost/container/static_vector.hpp>
#include <boost/dynamic_bitset.hpp>

#include <cereal/cereal.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/archives/binary.hpp>

#include <sax/owningptr.hpp>
#include <sax/memorypool.hpp>

#include "Globals.hpp"

#include "../../MCTSSearchTree/include/flat_search_tree.hpp"


namespace mcts {

    template<typename T, int N = 128>
    class Stack {

        std::vector<T> m_data;

    public:

        Stack ( const T v_ ) noexcept {
            m_data.reserve ( N );
            m_data.push_back ( v_ );
        }

        [[ nodiscard ]] const T pop ( ) noexcept {
            return m_data.pop ( );
        }

        void push ( const T v_ ) noexcept {
            m_data.push_back ( v_ );
        }

        [[ nodiscard ]] bool not_empty ( ) const noexcept {
            return m_data.size ( );
        }
    };


    template<typename T>
    class Queue {

        boost::container::deque<T> m_data;

    public:

        Queue ( const T v_ ) noexcept {
            m_data.push_back ( v_ );
        }

        [[ nodiscard ]] const T pop ( ) noexcept {
            const T v = m_data.front ( );
            m_data.pop_front ( );
            return v;
        }

        void push ( const T v_ ) noexcept {
            m_data.push_back ( v_ );
        }

        [[ nodiscard ]] bool not_empty ( ) const noexcept {
            return m_data.size ( );
        }
    };


    template<typename State>
    struct ArcData { // 1 bytes.

        using Move = typename State::Move;

        // float m_score = 0.0f; // 4 bytes.
        // std::int32_t m_visits = 0; // 4 bytes.

        Move m_move; // 2 bytes.

        // Constructors.

        ArcData ( ) noexcept {
            // std::cout << "arcdata default constructed\n";
        }

        ArcData ( const State & state_ ) noexcept {
            // std::cout << "arcdata constructed from state\n";
            m_move = state_.lastMove ( );
        }

        ArcData ( const ArcData & ad_ ) noexcept {
            // std::cout << "arcdata copy constructed\n";
            // m_score = nd_.m_score;
            // m_visits = nd_.m_visits;
            m_move = ad_.m_move;
        }

        ArcData ( ArcData && ad_ ) noexcept {
            // std::cout << "arcdata Move constructed\n";
            // m_score = std::move ( ad_.m_score );
            // m_visits = std::move ( ad_.m_visits );
            m_move = std::move ( ad_.m_move );
        }

        ~ArcData ( ) noexcept {
        }

        [[ maybe_unused ]] ArcData & operator += ( const ArcData & rhs_ ) noexcept {
            // m_score += rhs_.m_score;
            // m_visits += rhs_.m_visits;
            return * this;
        }

        [[ maybe_unused ]] ArcData & operator = ( const ArcData & ad_ ) noexcept {
            // std::cout << "arcdata copy assigned\n";
            // m_score = nd_.m_score;
            // m_visits = nd_.m_visits;
            m_move = ad_.m_move;
            return *this;
        }

        [[ maybe_unused ]] ArcData & operator = ( ArcData && ad_ ) noexcept {
            // std::cout << "arcdata Move assigned\n";
            // m_score = std::move ( ad_.m_score );
            // m_visits = std::move ( ad_.m_visits );
            m_move = std::move ( ad_.m_move );
            return *this;
        }

    private:

        friend class cereal::access;

        template<class Archive>
        void serialize ( Archive & ar_ ) { ar_ ( m_move ); }
    };


    template<typename State>
    struct NodeData { // 17 bytes.

        using Move = typename State::Move;
        using Moves = std::vector<Move>;
        using MovesPool = sax::MemoryPool<Moves, 65536>;
        using MovesPoolPtr = sax::owningptr<MovesPool>;

        using PlayerValue = typename State::value;
        using Player = typename State::value_type;

        Moves * m_moves = nullptr;  // 8 bytes.

        float m_score = 0.0f; // 4 bytes.
        std::int32_t m_visits = 0; // 4 bytes.

        Player m_player_just_moved = PlayerValue::invalid; // 1 byte.

        // Constructors.

        NodeData ( ) noexcept {
            // std::cout << "nodedata default constructed\n";
        }

        NodeData ( const State & state_ ) noexcept {
            // std::cout << "nodedata constructed from state\n";
            m_moves = new ( m_moves_pool->allocate ( ) ) Moves ( );
            if ( not ( state_.moves ( m_moves ) ) ) {
                m_moves_pool->deallocate ( m_moves );
                m_moves = nullptr;
            }
            m_player_just_moved = state_.playerJustMoved ( );
        }

        NodeData ( const NodeData & nd_ ) noexcept {

            // std::cout << "nodedata copy constructed\n";

            if ( nd_.m_moves != nullptr ) {

                m_moves = new ( m_moves_pool->allocate ( ) ) Moves ( );

                * m_moves = * nd_.m_moves;
            }

            m_score = nd_.m_score;
            m_visits = nd_.m_visits;
            m_player_just_moved = nd_.m_player_just_moved;
        }

        NodeData ( NodeData && nd_ ) noexcept {

            // std::cout << "nodedata Move constructed\n";

            std::swap ( m_moves, nd_.m_moves );

            m_score = std::move ( nd_.m_score );
            m_visits = std::move ( nd_.m_visits );
            m_player_just_moved = std::move ( nd_.m_player_just_moved );
        }

        ~NodeData ( ) noexcept {

            if ( m_moves != nullptr ) {

                // std::cout << "moves deallocated\n";

                m_moves_pool->deallocate ( m_moves );
            }
        }

        [[ nodiscard ]] Move getUntriedMove ( ) noexcept {
            if ( m_moves->size ( ) == 1 ) {
                const Move Move = m_moves->front ( );
                m_moves_pool->deallocate ( m_moves );
                m_moves = nullptr;
                return Move;
            }
            return m_moves->draw ( );
        }

        [[ nodiscard ]] NodeData & operator += ( const NodeData & rhs_ ) noexcept {
            m_score += rhs_.m_score;
            m_visits += rhs_.m_visits;
            return * this;
        }


        [[ nodiscard ]] NodeData & operator = ( const NodeData & nd_ ) noexcept {
            // std::cout << "nodedata copy assigned\n";
            if ( nd_.m_moves != nullptr ) {
                m_moves = new ( m_moves_pool->allocate ( ) ) Moves ( );
                * m_moves = * nd_.m_moves;
            }
            m_score = nd_.m_score;
            m_visits = nd_.m_visits;
            m_player_just_moved = nd_.m_player_just_moved;
            return * this;
        }

        [[ nodiscard ]] NodeData & operator = ( NodeData && nd_ ) noexcept {
            // std::cout << "nodedata Move assigned\n";
            std::swap ( m_moves, nd_.m_moves );
            m_score = std::move ( nd_.m_score );
            m_visits = std::move ( nd_.m_visits );
            m_player_just_moved = std::move ( nd_.m_player_just_moved );
            return * this;
        }

        static MovesPoolPtr m_moves_pool;

    private:

        friend class cereal::access;

        template<class Archive>
        void save ( Archive & ar_ ) const noexcept {
            if ( m_moves != nullptr ) {
                const std::int8_t tmp = 2;
                ar_ ( tmp );
                m_moves->serialize ( ar_ );
            }
            else {
                const std::int8_t tmp = 1;
                ar_ ( tmp );
            }
            ar_ ( m_score, m_visits, m_player_just_moved );
        }

        template<class Archive>
        void load ( Archive & ar_ ) noexcept {
            std::int8_t tmp = -1;
            ar_ ( tmp );
            if ( tmp == 2 ) {
                m_moves = new ( m_moves_pool->allocate ( ) ) Moves ( );
                m_moves->serialize ( ar_ );
            }
            ar_ ( m_score, m_visits, m_player_just_moved );
        }
    };

    template<typename State>
    typename NodeData<State>::MovesPoolPtr NodeData<State>::m_moves_pool ( new MovesPool ( ) );


    template<typename State>
    using Tree = fst::SearchTree<ArcData<State>, NodeData<State>>;

    template<typename State>
    using Node = typename Tree<State>::Node;

    template<typename State>
    using Arc = typename Tree<State>::Arc;


    template<typename State>
    class Mcts {

    public:
        using Tree = Tree<State>;
        using Arc = typename Tree::Arc;
        using Node = typename Tree::Node;
        using ArcData = ArcData<State>;
        using NodeData = NodeData<State>;
        using InIt = typename Tree::in_iterator;
        using OutIt = typename Tree::out_iterator;

        using Move = typename State::Move;
        using Moves = std::vector<Move>;

        using Link = typename Tree::Link;
        using Path = typename Tree::Path;

        using Generator = sax::Rng &;
        using ZobristHash = typename State::ZobristHash;

        typedef std::unordered_map<ZobristHash, Node> TranspositionTable;
        typedef std::vector<ZobristHash> InverseTranspositionTable;
        typedef sax::owningptr<TranspositionTable> TranspositionTablePtr;

        // The data.

        Tree m_tree;
        TranspositionTablePtr m_transposition_table;
        Generator m_generator;

        bool m_not_initialized = true;

        // The purpose of the m_path is to maintain the path with updates
        // (of the visits/scores) all the way to the original root (start
        // of the game). It's also used as a scrath-pad for the updates
        // after play-out. Each Move and players' Move gets added to this
        // path.

        Path m_path;
        int m_path_size;

        Mcts () noexcept : m_generator ( Rng::generator ( ) ) {}

        // Init.

        void initialize ( const State & state_ ) noexcept {
            if ( m_transposition_table.get ( ) == nullptr ) {
                m_transposition_table.reset ( new TranspositionTable ( ) );
            }
            // Set root_arc and root_node data.
            m_tree [ m_tree.root_arc  ] =  ArcData ( state_ );
            m_tree [ m_tree.root_node ] = NodeData ( state_ );
            // Add root_node to transposition_table.
            m_transposition_table->emplace ( state_.zobrist ( ) [ 0 ], m_tree.root_node );
            // Has been initialized.
            m_not_initialized = false;
            m_path.reset ( m_tree.root_arc, m_tree.root_node );
            m_path_size = 1;
        }


        [[ nodiscard ]] Link addArc ( const Node parent_, const Node child_, const State & state_ ) noexcept {
            return m_tree.addArc ( parent_, child_, state_ );
        }

        [[ nodiscard ]] Link addNode ( const Node parent_, const State & state_ ) noexcept {
            const Link child = m_tree.addNode ( parent_, state_ );
            m_transposition_table->emplace ( state_.zobrist ( ) [ 0 ], child.target );
            return child;
        }


        void printMoves ( const Node n_ ) const noexcept {
            std::cout << "moves of " << ( int ) n_ << ": ";
            for ( OutIt a ( m_tree, n_ ); a != OutIt::end ( ); ++a ) {
                std::cout << "[" << ( int ) a.get ( ) << ", " << ( int ) m_tree [ a ].m_move.m_loc << "]";
            }
            putchar ( '\n' );
        }


        [[ nodiscard ]] Move getMove ( const Arc arc_ ) const noexcept {
            return m_tree [ arc_ ].m_move;
        }


        [[ nodiscard ]] Node getNode ( const ZobristHash zobrist_ ) const noexcept {
            const auto it = m_transposition_table->find ( zobrist_ );
            return it == m_transposition_table->cend ( ) ? Tree::invalid_node : it->second;
        }


        [[ nodiscard ]] bool hasChildren ( const Node node_ ) const noexcept {
            return m_tree.isInternal ( node_ );
        }


        // Moves.

        [ [ nodiscard ] ] bool hasNoUntriedMoves ( const Node node_ ) const noexcept {
            return m_tree [ node_ ].m_moves == nullptr;
        }


        [[ nodiscard ]] bool hasUntriedMoves ( const Node node_ ) const noexcept {
            return m_tree [ node_ ].m_moves != nullptr;
        }


        [[ nodiscard ]] Move getUntriedMove ( const Node node_ ) noexcept {
            return m_tree [ node_ ].getUntriedMove ( );
        }


        // Data.

        [[ nodiscard ]] std::int32_t getVisits ( const Node node_ ) const noexcept {
            std::int32_t visits = 0L;
            for ( InIt a ( m_tree, node_ ); a != InIt::end ( ); ++a ) {
                visits += m_tree [ a ].m_visits;
            }
            return visits;
        }


        [[ nodiscard ]] float getUCTFromArcs ( const Node parent_, const Node child_ ) const noexcept {
            ArcData child_data;
            for ( InIt a ( m_tree, child_ ); a != InIt::end ( ); ++a ) {
                child_data += m_tree [ a ];
            }
            std::int32_t visits = 1L;
            for ( InIt a ( m_tree, parent_ ); a != InIt::end ( ); ++a ) {
                visits += m_tree [ a ].m_visits;
            }
            //                              Exploitation                                                             Exploration
            // Exploitation is the task to select the Move that leads to the best results so far.
            // Exploration deals with less promising moves that still have to be examined, due to the uncertainty of the evaluation.
            return ( float ) child_data.m_score / ( float ) child_data.m_visits + sqrtf ( 3.0f * logf ( ( float ) visits ) / ( float ) child_data.m_visits );
        }


        [[ nodiscard ]] float getUCTFromNode ( const Node parent_, const Node child_ ) const noexcept {
            //                              Exploitation                                                             Exploration
            // Exploitation is the task to select the Move that leads to the best results so far.
            // Exploration deals with less promising moves that still have to be examined, due to the uncertainty of the evaluation.
            return ( float ) m_tree [ child_ ].m_score / ( float ) m_tree [ child_ ].m_visits + sqrtf ( 4.0f * logf ( ( float ) ( m_tree [ parent_ ].m_visits + 1 ) ) / ( float ) m_tree [ child_ ].m_visits );
        }


        [[ nodiscard ]] Link selectChildRandom ( const Node parent_ ) const noexcept {
            boost::container::static_vector < Link, State::max_no_moves > children;
            for ( OutIt a ( m_tree, parent_ ); a != OutIt::end ( ); ++a ) {
                children.emplace_back ( m_tree.link ( a ) );
            }
            return children [ std::uniform_int_distribution < ptrdiff_t > ( 0, children.size ( ) - 1 ) ( m_generator ) ];
        }


        [[ nodiscard ]] Link selectChildUCT ( const Node parent_ ) const noexcept {
            OutIt a ( m_tree, parent_ );
            boost::container::static_vector < Link, State::max_no_moves > best_children ( 1, m_tree.link ( a ) );
            float best_UCT_score = getUCTFromNode ( parent_, best_children.back ( ).target );
            ++a;
            for ( ; a != OutIt::end ( ); ++a ) {
                const Link child = m_tree.link ( a );
                const float UCT_score = getUCTFromNode ( parent_, child.target );
                if ( UCT_score > best_UCT_score ) {
                    best_children.resize ( 1 );
                    best_children.back ( ) = child;
                    best_UCT_score = UCT_score;
                }
                else if ( UCT_score == best_UCT_score ) {
                    best_children.push_back ( child );
                }
            }
            // Ties are broken by fair coin flips.
            return best_children.size ( ) == 1 ? best_children.back ( ) : best_children [ std::uniform_int_distribution < ptrdiff_t > ( 0, best_children.size ( ) - 1 ) ( m_generator ) ];
        }


        [[ nodiscard ]] Link addChild ( const Node parent_, const State & state_ ) noexcept {
            // State is updated to reflect Move.
            const Node child = getNode ( state_.zobrist ( ) [ 0 ] );
            return child == Tree::invalid_node ? addNode ( parent_, state_ ) : addArc ( parent_, child, state_ );
        }


        void updateData ( Link && link_, const State & state_ ) noexcept {
            const float result = state_.result ( m_tree [ link_.target ].m_player_just_moved );
            // ++m_tree [ link_.arc ].m_visits;
            // m_tree [ link_.arc ].m_score += result;
            ++m_tree [ link_.target ].m_visits;
            m_tree [ link_.target ].m_score += result;
        }


        [[ nodiscard ]] Move getBestMove ( ) noexcept {
            // Find the node (the most robust) with the most visits.
            std::int32_t best_child_visits = INT_MIN;
            Move best_child_move = State::Move::none;
            m_path.push ( Tree::invalid_arc, Tree::invalid_node );
            ++m_path_size;
            for ( OutIt a ( m_tree, m_tree.root_node ); a != OutIt::end ( ); ++a ) {
                const Link child ( m_tree.link ( a ) );
                const std::int32_t child_visits ( m_tree [ child.target ].m_visits );
                if ( child_visits > best_child_visits ) {
                    best_child_visits = child_visits;
                    best_child_move = m_tree [ child.arc ].m_move;
                    m_path.back ( ) = child;
                }
            }
            return best_child_move;
        }


        void connectStatesPath ( const State & state_ ) noexcept {
            // Adding the Move of the opponent to the path (and possibly to the tree).
            const Node parent = m_path.back ( ).target; Node child = getNode ( state_.zobrist ( ) [ 0 ] );
            if ( child == Tree::invalid_node ) {
                child = addNode ( parent, state_ ).target;
            }
            m_path.push ( m_tree.link ( parent, child ) );
            ++m_path_size;
        }


        [[ nodiscard ]] Move compute ( const State & state_, int max_iterations_ = 100'000 ) noexcept {
            // constexpr std::int32_t threshold = 5;
            if ( m_not_initialized ) {
                initialize ( state_ );
            }
            else {
                connectStatesPath ( state_ );
            }
            const Player player = state_.playerToMove ( );
            if ( player == Player::Type::agent ) {
                // m_path.print ( );
            }
            // max_iterations_ -= m_tree.nodeNum ( );
            while ( max_iterations_-- > 0 ) {
                Node node = m_tree.root_node;
                State state ( state_ );
                // Select a path through the tree to a leaf node.
                while ( hasNoUntriedMoves ( node ) and hasChildren ( node ) ) {
                    // UCT is only applied in nodes of which the visit count
                    // is higher than a certain threshold T
                    // Link child = player == Player::Type::agent and m_tree [ node ].m_visits < threshold ? selectChildRandom ( node ) :
                    Link child = selectChildUCT ( node );
                    state.moveHash ( m_tree [ child.arc ].m_move );
                    m_path.push ( child );
                    node = child.target;
                }
                /*

                static int cnt = 0;

                if ( state != m_tree [ node ].m_state ) {

                    state.print ( );
                    m_tree [ node ].m_state.print ( );

                    ++cnt;

                    if ( cnt == 100 ) exit ( 0 );
                }

                */

                // If we are not already at the final state, expand the tree with a new
                // node and Move there.

                // In addition to expanding one node per simulated game, we also expand all the
                // children of a node when a node's visit count equals T

                if ( hasUntriedMoves ( node ) ) {
                    //if ( player == Player::Type::agent and m_tree [ node ].m_visits < threshold )
                    state.moveHashWinner ( getUntriedMove ( node ) ); // State update.
                    m_path.push ( addChild ( node, state ) );
                }

                // The player in back of path is player ( the player to Move ).We now play
                // randomly until the game ends.

                if ( player == Player::Type::human ) {
                    state.simulate ( );
                    for ( Link link : m_path ) {
                        // We have now reached a final state. Backpropagate the result up the
                        // tree to the root node.
                        updateData ( std::move ( link ), state );
                    }
                }

                else {
                    for ( int i = 0; i < 10; ++i ) {
                        State sim_state ( state );
                        sim_state.simulate ( );
                        // We have now reached a final state. Backpropagate the result up the
                        // tree to the root node.
                        for ( Link link : m_path ) {
                            updateData ( std::move ( link ), sim_state );
                        }
                    }
                }
                m_path.resize ( m_path_size );
            }
            return getBestMove ( );
        }


        void prune_ ( Mcts * new_mcts_, const State & state_ ) noexcept {

            // at::AutoTimer t ( at::milliseconds );

            typedef std::vector < Node > Visited; // New m_nodes by old_index.
            typedef Queue < Node > Queue;

            // Prune Tree.

            const Node old_node = getNode ( state_.zobrist ( ) [ 0 ] );

            Visited visited ( m_tree.nodeNum ( ), Tree::invalid_node );

            visited [ old_node ] = m_tree.root_node;

            Queue queue ( old_node );

            Tree & new_tree = new_mcts_->m_tree;

            new_tree [ m_tree.root_node ] = std::move ( m_tree [ old_node ] );

            while ( queue.not_empty ( ) ) {

                const Node parent = queue.pop ( );

                for ( OutIt a ( m_tree, parent ); a != OutIt::end ( ); ++a ) {

                    const Node child = m_tree.target ( a );

                    if ( visited [ child ] == Tree::invalid_node ) { // Not visited yet.

                        const Link link = new_tree.addNodeUnsafe ( visited [ parent ] );

                        visited [ child ] = link.target;
                        queue.push ( child );

                        new_tree [ link.arc ] = std::move ( m_tree [ a ] );
                        new_tree [ link.target ] = std::move ( m_tree [ child ] );
                    }

                    else {

                        new_tree [ new_tree.addArcUnsafe ( visited [ parent ], visited [ child ] ).arc ] = std::move ( m_tree [ a ] );
                    }
                }
            }

            // Purge TransitionTable.

            auto it = m_transposition_table->begin ( );
            const auto it_cend = m_transposition_table->cend ( );

            while ( it != it_cend ) {

                const auto tmp_it = it;

                ++it;

                const Node new_node = visited [ tmp_it->second ];

                if ( new_node == Tree::invalid_node ) {

                    m_transposition_table->erase ( tmp_it );
                }

                else {

                    tmp_it->second = new_node;
                }
            }

            // Transfer TranspositionTable.

            new_mcts_->m_transposition_table.reset ( m_transposition_table.take ( ) );

            // Has been initialized.

            new_mcts_->m_not_initialized = false;

            // Reset path.

            new_mcts_->m_path.reset ( new_tree.root_arc, new_tree.root_node );
            new_mcts_->m_path_size = 1;
        }


        static void prune ( Mcts * & old_mcts_, const State & state_ ) noexcept {

            if ( not ( old_mcts_->m_not_initialized ) and old_mcts_->getNode ( state_.zobrist ( ) [ 0 ] ) != Mcts::Tree::invalid_node ) {

                // The state exists in the tree and it's not the current root_node. i.e. now prune.

                Mcts * new_mcts = new Mcts ( );

                old_mcts_->prune_ ( new_mcts, state_ );

                std::swap ( old_mcts_, new_mcts );

                delete new_mcts;
            }
        }


        static void reset ( Mcts * & mcts_, const State & state_, const Player player_ ) noexcept {

            if ( not ( mcts_->m_not_initialized ) ) {

                const Mcts::Node new_root_node = mcts_->getNode ( state_.zobrist ( ) [ 0 ] );

                if ( new_root_node != Mcts::Tree::invalid_node ) {

                    // The state exists in the tree and it's not the current root_node. i.e. re-hang the tree.

                    mcts_->m_tree.setRoot ( new_root_node );

                    if ( player_ == Player::Type::agent ) {

                        // std::cout << "new root node " << ( int ) new_root_node << ", node num " << mcts_->m_tree.nodeNum ( ) << "\n";
                    }
                }

                else {

                    std::cout << "new tree\n";

                    Mcts * new_mcts = new Mcts ( );

                    new_mcts->initialize ( state_ );

                    std::swap ( mcts_, new_mcts );

                    delete new_mcts;
                }
            }
        }


        InverseTranspositionTable invertTranspositionTable ( ) const noexcept {

            InverseTranspositionTable itt ( m_transposition_table->size ( ) );

            for ( auto & e : * m_transposition_table ) {

                itt [ e.second ] = e.first;
            }

            return itt;
        }


        static void merge ( Mcts * & t_mcts_, Mcts * & s_mcts_ ) {

            // Same pointer, do nothing.

            if ( t_mcts_ == s_mcts_ ) {

                return;
            }

            // t_mcts_ (target) becomes the largest tree, we're merging the smaller tree (source) into the larger tree.

            if ( t_mcts_->m_tree.nodeNum ( ) < s_mcts_->m_tree.nodeNum ( ) ) {

                std::swap ( t_mcts_, s_mcts_ );
            }

            // Avoid some levels of indirection and make things clearer.

            Tree & t_t = t_mcts_->m_tree, & s_t = s_mcts_->m_tree; // target tree, source tree.
            TranspositionTable & t_tt = * t_mcts_->m_transposition_table; // target transposition table.
            InverseTranspositionTable s_itt ( std::move ( s_mcts_->invertTranspositionTable ( ) ) ); // source inverse transposition table.

            // bfs help structures.

            typedef boost::dynamic_bitset < > Visited;
            typedef Queue < Node > Queue;

            Visited s_visited ( s_t.nodeNum ( ) );
            Queue s_queue ( s_t.root_node );

            s_visited [ s_t.root_node ] = true;

            // Walk the tree, breadth first.

            while ( s_queue.not_empty ( ) ) {

                // The t_source (target parent) does always exist, as we are going at it breadth first.

                const Node s_source = s_queue.pop ( ), t_source = t_tt.find ( s_itt [ s_source ] )->second;

                // Iterate over children (targets) of the parent (source).

                for ( OutIt soi ( s_t, s_source ); soi != OutIt::end ( ); ++soi ) { // Source Out Iterator (soi).

                    const Link s_link = s_t.link ( soi );

                    if ( not ( s_visited [ s_link.target ] ) ) {

                        s_visited [ s_link.target ] = true;
                        s_queue.push ( s_link.target );

                        // Now do something. If child in s_mcts_ doesn't exist in t_mcts_, add child.

                        const auto t_it = t_tt.find ( s_itt [ s_link.target ] );

                        if ( t_it != t_tt.cend ( ) ) { // Child exists. The arc does or does not exist.

                            // Node t_it->second corresponds to Node target child.

                            const Link t_link ( t_t.link ( t_source, t_it->second ) );

                            if ( t_link.arc != Tree::invalid_arc ) { // The arc does exist.

                                t_t [ t_link.arc ] += s_t [ s_link.arc ];
                            }

                            else { // The arc does not exist.

                                t_t [ t_t.addArcUnsafe ( t_source, t_link.target ).arc ] = std::move ( s_t [ s_link.arc ] );
                            }

                            // Update the values of the target.

                            t_t [ t_link.target ] += s_t [ s_link.target ];
                        }

                        else { // Child does not exist.

                            const Link t_link = t_t.addNodeUnsafe ( t_source );

                            // m_tree.

                            t_t [ t_link.arc    ] = std::move ( s_t [ s_link.arc    ] );
                            t_t [ t_link.target ] = std::move ( s_t [ s_link.target ] );

                            // m_transposition_table.

                            t_tt.emplace ( s_itt [ s_link.target ], t_link.target );
                        }
                    }
                }
            }

            t_mcts_->m_path.resize ( 1 );
            t_mcts_->m_path_size = 1;

            delete s_mcts_; // Destruct the smaller tree.

            s_mcts_ = nullptr;
        }


        std::size_t numTranspositions ( ) const noexcept {

            std::size_t nt = 0;

            typedef boost::dynamic_bitset < > Visited;
            typedef Stack < Node > Stack;

            Visited visited ( m_tree.nodeNum ( ) );
            Stack stack ( m_tree.root_node );

            visited [ m_tree.root_node ] = true;

            while ( stack.not_empty ( ) ) {

                const Node parent = stack.pop ( );

                for ( OutIt a ( m_tree, parent ); a != OutIt::end ( ); ++a ) {

                    const Node child = m_tree.target ( a );

                    if ( visited [ child ] == false ) {

                        visited [ child ] = true;
                        stack.push ( child );

                        if ( m_tree.inArcNum ( child ) > 1 ) {

                            ++nt;
                        }
                    }
                }
            }

            return nt;
        }

    private:

        friend class cereal::access;

        template<class Archive>
        void save ( Archive & ar_ ) const noexcept {

            ar_ ( m_tree, * m_transposition_table, m_not_initialized );
        }

        template<class Archive>
        void load ( Archive & ar_ ) noexcept {

            m_tree.clearUnsafe ( );

            if ( m_transposition_table.get ( ) == nullptr ) {

                m_transposition_table.reset ( new TranspositionTable ( ) );
            }

            else {

                m_transposition_table->clear ( );
            }

            ar_ ( m_tree, * m_transposition_table, m_not_initialized );

            m_path.reset ( m_tree.root_arc, m_tree.root_node );
            m_path_size = 1;
        }
    };


    template<typename State>
    void computeMove ( State & state_, int max_iterations_ = 100'000 ) noexcept {
        Mcts<State> * mcts = new Mcts<State> ( );
        state_.moveHashWinner ( mcts->compute ( state_, max_iterations_ ) );
        delete mcts;
    }
}
