
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

#include <array>
#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include <stlab/concurrency/default_executor.hpp>
#include <stlab/concurrency/future.hpp>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Extensions.hpp>

#include "Types.hpp"
#include "Globals.hpp"
#include "Hexcontainer.hpp"

#include "resource.h"


class spinlock_mutex {
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
    public:
    void lock ( ) noexcept {
        while ( flag.test_and_set ( std::memory_order_acquire ) );
    }
    void unlock ( ) noexcept {
        flag.clear ( std::memory_order_release );
    }
    [[ nodiscard ]] bool try_lock ( ) noexcept {
        return not ( flag.test_and_set ( std::memory_order_acquire ) );
    }
};


namespace sf {
using Boxf = Box<float>;

struct Quad {
    Vertex v0, v1, v2, v3;
};
}


struct MouseState {

    enum mouse_state : int { idle = 0, moved = 1, left_clicked = 2, moved_and_left_clicked = 3 };

    sf::RenderWindow * m_window_ptr = nullptr;
    int m_current = 0;
    mouse_state m_mouse_state = mouse_state::moved;
    sf::Vector2i m_data [ 2 ];
    sf::Vector2f m_position;

    MouseState ( ) noexcept { }

    void initialize ( sf::RenderWindow & w_ ) noexcept {
        m_window_ptr = & w_;
        m_data [ 0 ] = m_data [ 1 ] = sf::Mouse::getPosition ( *m_window_ptr );
        m_position = sf::castVector2f ( m_data [ 0 ] );
    }

    [[ nodiscard ]] const sf::Vector2f & operator ( ) ( ) const noexcept {
        return m_position;
    }

    [[ nodiscard ]] bool hadActivity ( ) const noexcept {
        return static_cast<int> ( m_mouse_state );
    }
    [[ nodiscard ]] bool hadNoActivity ( ) const noexcept {
        return not ( hadActivity ( ) );
    }

    [[ nodiscard ]] bool leftClicked ( ) const noexcept {
        return 1 < static_cast<int> ( m_mouse_state );
    }
    [[ nodiscard ]] bool hasMoved ( ) const noexcept {
        return static_cast<int> ( m_mouse_state ) & 1;
    }

    const sf::Vector2f & update ( ) noexcept {
        m_mouse_state = mouse_state::idle;
        if ( sf::Mouse::isButtonPressed ( sf::Mouse::Left ) )
            m_mouse_state = mouse_state::left_clicked;
        m_data [ m_current ^ 1 ] = sf::Mouse::getPosition ( * m_window_ptr );
        if ( m_data [ 0 ] != m_data [ 1 ] ) {
            m_current ^= 1;
            m_position = sf::castVector2f ( m_data [ m_current ] );
            m_mouse_state = mouse_state::left_clicked == m_mouse_state ? mouse_state::moved_and_left_clicked : mouse_state::moved;
        }
        return m_position;
    }
};




class Taskbar : public sf::Drawable {

    static constexpr float width = 135.0f, height = 30.0f;

    public:

    enum State : int { in_active = 0, minimize, maximize, close };

    private:

    [[ nodiscard ]] sf::Quad makeVertex ( const sf::Vector2f & p_ ) const noexcept {
        return {
            sf::Vertex { sf::Vector2f { p_.x, p_.y }, sf::Vector2f { 0.0f, 0.0f } },
            sf::Vertex { sf::Vector2f { p_.x + width, p_.y }, sf::Vector2f { width, 0.0f } },
            sf::Vertex { sf::Vector2f { p_.x + width, p_.y + height }, sf::Vector2f { width, height } },
            sf::Vertex { sf::Vector2f { p_.x, p_.y + height }, sf::Vector2f { 0.0f, height } }
        };
    }

    void setQuadTexture ( const State state_ ) noexcept {
        if ( state_ != state ( ) ) {
            sf::Quad & quads = *reinterpret_cast< sf::Quad* > ( &m_vertices [ 0 ] );
            const float left { state_ * width }, right { left + width };
            quads.v0.texCoords.x = left;
            quads.v1.texCoords.x = right;
            quads.v2.texCoords.x = right;
            quads.v3.texCoords.x = left;
        }
    }

    public:

    Taskbar ( const float window_width_ ) :
        m_minimize_bounds { window_width_ - width, 0.0f, width / 3.0f, height },
        m_close_bounds { window_width_ - width / 3.0f, 0.0f, width / 3.0f, height } {
        sf::loadFromResource ( m_texture, TASKBAR );
        m_texture.setSmooth ( true );
        m_vertices.setPrimitiveType ( sf::Quads );
        m_vertices.resize ( 4 );
        sf::Quad * quads = reinterpret_cast< sf::Quad* > ( &m_vertices [ 0 ] );
        quads [ 0 ] = makeVertex ( sf::Vector2f { window_width_ - width, 0.0f } );
    }

    virtual void draw ( sf::RenderTarget & target, sf::RenderStates states ) const {
        states.texture = &m_texture;
        target.draw ( m_vertices, states );
    }

    [[ nodiscard ]] State state ( ) const noexcept {
        return static_cast<State> ( static_cast< int > ( m_vertices [ 0 ].texCoords.x ) / static_cast< int > ( width ) );
    }

    void update ( const sf::Vector2f & p_ ) noexcept {
        setQuadTexture ( m_minimize_bounds.contains ( p_ ) ? State::minimize : m_close_bounds.contains ( p_ ) ? State::close : State::in_active );
    }

    void reset ( ) noexcept {
        setQuadTexture ( State::in_active );
    }

    private:

    sf::FloatRect m_minimize_bounds, m_close_bounds;

    sf::Texture m_texture;
    sf::VertexArray m_vertices;
};


struct DelayTimer {

    void set ( const int d_ ) {
        m_delay = std::chrono::milliseconds { 1'000 * d_ };
        expired = m_delay == std::chrono::milliseconds { 0 };
    }

    void restart ( const sf::HrClock::time_point now_ ) {
        if ( not ( expired = m_delay == std::chrono::milliseconds { 0 } ) ) {
            m_end_time = now_ + m_delay;
        }
    }

    [[ nodiscard ]] bool update ( const sf::HrClock::time_point now_ ) noexcept {
        return expired = now_ > m_end_time;
    }

    sf::HrClock::time_point m_end_time;
    std::chrono::milliseconds m_delay;
    bool expired = false;
};

struct GameClock : public sf::Drawable, public sf::Transformable {

    enum Player : int { agent, human };

    private:

    void init ( sf::Text & text_ ) const noexcept {
        text_.setFont ( m_font_numbers );
        text_.setCharacterSize ( 24 );
        text_.setString ( "00:00" );
        text_.setColor ( sf::Color { 178, 178, 178, 255 } );
        sf::centreOrigin ( text_ );
    }

    public:

    GameClock ( const float left_, const float right_, const float height_ ) noexcept {
        sf::loadFromResource ( m_font_numbers, __NUMBERS_FONT__ );
        init ( m_text [ Player::human ] );
        m_text [ Player::human ].setPosition ( left_, height_ );
        m_bounds [ Player::human ] = m_text [ Player::human ].getGlobalBounds ( );
        init ( m_text [ Player::agent ] );
        m_text [ Player::agent ].setPosition ( right_, height_ );
        m_bounds [ Player::agent ] = m_text [ Player::agent ].getGlobalBounds ( );
        set ( 5, 0, 2 );
        restart ( Player::human );
    }

    void set ( const int min_, const int sec_ = 0, const int delay_ = 0 ) noexcept {
        m_time [ Player::human ] = m_time [ Player::agent ] = sf::fminutes { min_ } +sf::fseconds { sec_ };
        m_delay_timer.set ( delay_ );
        char buf [ 6 ] = { 0 };
        std::snprintf ( buf, 6, "%0.2i:%0.2i", min_, sec_ );
        m_text [ Player::human ].setString ( buf );
        m_text [ Player::agent ].setString ( buf );
    }

    void update ( ) const noexcept {
        if ( m_is_running ) {
            const sf::HrClock::time_point now = m_clock.now ( );
            if ( m_delay_timer.expired ) {
                m_time [ m_player_to_move ] -= now - m_start;
                m_start = now;
            }
            else {
                if ( m_delay_timer.update ( now ) ) {
                    // Just expired, start timing.
                    m_start = now;
                }
                else {
                    return;
                }
            }
            char buf [ 6 ] = { 0 };
            int s = static_cast< int > ( m_time [ m_player_to_move ].count ( ) );
            if ( s < 0 )
                s = 0;
            std::snprintf ( buf, 6, "%0.2i:%0.2i", s / 60, s % 60 );
            m_text [ m_player_to_move ].setString ( buf );
        }
    }

    void restart ( const Player p_ ) noexcept {
        m_player_to_move = p_;
        m_start = m_clock.now ( ); // In case delay == 0 or very short.
        m_delay_timer.restart ( m_start );
        update ( );
    }

    void update_next ( ) noexcept {
        update ( );
        restart ( Player::agent == m_player_to_move ? Player::human : Player::agent );
    }

    virtual void draw ( sf::RenderTarget & target, sf::RenderStates states ) const {
        update ( );
        target.draw ( m_text [ Player::human ] );
        target.draw ( m_text [ Player::agent ] );
    }

    [[ nodiscard ]] bool isClicked ( const sf::Vector2f & mouse_position_ ) noexcept {
        const bool is_clicked = m_bounds [ m_player_to_move ].contains ( mouse_position_ );
        if ( is_clicked ) {
            update_next ( );
        }
        return is_clicked;
    }

    void pause ( ) noexcept {
        m_is_running = false;
    }

    void resume ( ) noexcept {
        m_is_running = true;
        m_start = m_clock.now ( );
    }

    private:

    // The clock.

    mutable sf::HrClock::time_point m_start;
    sf::HrClock m_clock;
    mutable std::array<sf::fseconds, 2> m_time;
    mutable DelayTimer m_delay_timer;
    Player m_player_to_move = Player::human;

    // Stuff to draw it.

    sf::Font m_font_numbers;

    mutable sf::Text m_text [ 2 ];
    sf::FloatRect m_bounds [ 2 ];

    bool m_is_running = true;
};


template<typename State>
struct PlayArea : public sf::Drawable, public sf::Transformable {

    using board = typename State::board;
    using state_move = typename State::move;
    using state_reference = State &;
    using clock_reference = GameClock &;

    using size_type = typename board::size_type;

    using play_area_lock = spinlock_mutex;
    using future_state_move = stlab::future<state_move>;

    static constexpr int not_set = -1;

    enum DisplayType : int { vacant = 0, red, green };
    enum DisplayValue : int { in_active_vacant = 0, in_active_red, in_active_green, active_vacant, active_red, active_green, selected_vacant, selected_red, selected_green };

    // sf::VertexArray m_vertices; is an array of quads. The x coordinate of the texCoords of the
    // first sf::Vertex of the sf::Quad is a multiple of the DisplayValue, as per above, i.e. the
    // texCoords store the information as to which sub-texture is referenced.

    using sidx = typename State::sidx;
    using hex = typename State::hex;

    [[ nodiscard ]] sf::Quad makeVertex ( const sf::Vector2f & p_ ) const noexcept;
    void init ( const sf::Vector2f & center_ ) noexcept;

    PlayArea ( State & state_, GameClock & clock_, const sf::Vector2f & center_, float hori_, float vert_, float circle_diameter_ );

    ~PlayArea ( ) {
        // Wait for the agent to return it's move. This
        // is the only time this lock might spin.
        m_agent_move_lock.lock ( );
    }

    private:

    void setQuadTexture ( sf::Quad & quad_, size_type i_ ) noexcept {
        quad_.v0.texCoords.x = i_ * m_circle_diameter;
        quad_.v1.texCoords.x = quad_.v0.texCoords.x + m_circle_diameter;
        quad_.v2.texCoords.x = quad_.v1.texCoords.x;
        quad_.v3.texCoords.x = quad_.v0.texCoords.x;
    }
    void setQuadTexture ( sf::Quad * quad_, size_type i_ ) noexcept {
        setQuadTexture ( * quad_, i_ );
    }

    void setQuadAlpha ( sf::Quad & quad_, const float alpha_ ) noexcept {
        quad_.v0.color.a = static_cast< sf::Uint8 > ( alpha_ );
        quad_.v1.color.a = static_cast< sf::Uint8 > ( alpha_ );
        quad_.v2.color.a = static_cast< sf::Uint8 > ( alpha_ );
        quad_.v3.color.a = static_cast< sf::Uint8 > ( alpha_ );
    }
    void setQuadAlpha ( sf::Quad * quad_, const float alpha_ ) noexcept {
        setQuadAlpha ( * quad_, alpha_ );
    }

    [[ nodiscard ]] DisplayType display_type ( DisplayValue d_ ) const noexcept {
        return static_cast<DisplayType> ( static_cast<int> ( d_ ) % 3 );
    }

    [[ nodiscard ]] size_type what ( size_type i_ ) const noexcept {
        return static_cast<size_type> ( m_quads [ i_ ].v0.texCoords.x ) % static_cast<size_type> ( m_circle_radius );
    }
    [[ nodiscard ]] DisplayValue what_value ( size_type i_ ) const noexcept {
        return static_cast<DisplayValue> ( what ( i_ ) );
    }
    [[ nodiscard ]] DisplayType what_type ( size_type i_ ) const noexcept {
        return static_cast<DisplayType> ( what ( i_ ) % 3 );
    }

    [[ nodiscard ]] bool are_neighbors ( const hex a_, const hex b_ ) const noexcept {
        if ( a_ != b_ ) {
            const typename hex::value_type dq = a_.q - b_.q, dr = a_.r - b_.r;
            return std::abs ( dq ) + std::abs ( dr ) + std::abs ( -dq - dr ) == typename hex::value_type { 2 };
        }
        return false;
    }

    public:

    void make_agent_move ( const DisplayValue d_ = DisplayValue::in_active_green ) noexcept {
        m_agent_move_lock.lock ( );
        agent_is_making_move = true;
        m_move_future = std::move ( stlab::async ( stlab::default_executor, [ & ] ( ) noexcept { return m_state.get_random_move ( ); } )
            .then ( [ this, d_ ] ( state_move m ) noexcept {
            if ( m.is_slide ( ) ) {
                m_lock.lock ( );
                setQuadTexture ( m_quads [ m.from ], DisplayValue::in_active_vacant );
                setQuadTexture ( m_quads [ m.to ], d_ );
                m_lock.unlock ( );
            }
            else {
                m_lock.lock ( );
                setQuadTexture ( m_quads [ m.to ], d_ );
                m_lock.unlock ( );
            }
            m_state.move_hash_winner ( m );
            std::cout << m_state << nl;
            m_clock.update_next ( );
            agent_is_making_move = false;
            m_agent_move_lock.unlock ( );
        } )
        );
    }

    [[ nodiscard ]] bool equal ( const hex & i_, const DisplayValue d_ ) noexcept {
        const size_type i = board::index ( i_.q, i_.r ), w = what_type ( i );
        if ( display_type ( d_ ) == w ) {
            m_lock.lock ( );
            setQuadTexture ( m_quads [ i ], w + 6 );
            m_lock.unlock ( );
            m_last = i;
            return true;
        }
        return false;
    }
    [[ nodiscard ]] bool place ( const hex & t_, const DisplayValue d_ ) noexcept {
        const size_type t = board::index ( t_.q, t_.r );
        if ( DisplayType::vacant == what_type ( t ) ) {
            m_lock.lock ( );
            setQuadTexture ( m_quads [ t ], d_ );
            m_lock.unlock ( );
            m_last = t;
            m_state.move_hash_winner ( state_move { t } );
            std::cout << m_state << nl;
            m_clock.update_next ( );
            make_agent_move ( );
            return true;
        }
        return false;
    }

    [[ nodiscard ]] bool move ( const hex & f_, const hex & t_, const DisplayValue d_ ) noexcept {
        if ( are_neighbors ( f_, t_ ) ) {
            const size_type f = board::index ( f_.q, f_.r ), t = board::index ( t_.q, t_.r );
            if ( display_type ( d_ ) == what_type ( f ) and DisplayValue::active_vacant == what_value ( t ) ) {
                m_lock.lock ( );
                // * m_from_quad = m_quads [ f ];
                // setQuadTexture ( * m_from_quad, what_type ( f ) );
                // m_animator.emplace ( LAMBDA_EASING_START_END_DURATION ( [ & ] ( const float v ) noexcept { setQuadAlpha ( m_from_quad, v ); }, sf::easing::exponentialInEasing, 255.0f, 0.0f, 500 ) );
                // m_animator.emplace ( LAMBDA_DELAY ( [ & ] ( const float ) noexcept { std::memset ( m_from_quad, 0, sizeof ( sf::Quad ) ); }, 500 ) );
                setQuadTexture ( m_quads [ f ], DisplayValue::in_active_vacant );
                setQuadTexture ( m_quads [ t ], d_ );
                // * m_to_quad = m_quads [ t ];
                // setQuadTexture ( * m_to_quad, d_ );
                // m_animator.emplace ( LAMBDA_EASING_START_END_DURATION ( [ & ] ( const float v ) noexcept { setQuadAlpha ( m_to_quad, v ); }, sf::easing::exponentialInEasing, 0.0f, 255.0f, 1500 ) );
                // m_animator.emplace ( LAMBDA_DELAY ( ( [ &, t ] ( const float ) noexcept {
                //    m_quads [ t ] = * m_to_quad;
                //    std::memset ( m_to_quad, 0, sizeof ( sf::Quad ) );
                // } ), 1500 ) );
                m_lock.unlock ( );
                m_last = t;
                m_state.move_hash_winner ( state_move { f, t } );
                std::cout << m_state << nl;
                m_clock.update_next ( );
                make_agent_move ( );
                return true;
            }
        }
        const size_type f = board::index ( f_.q, f_.r );
        m_lock.lock ( );
        setQuadTexture ( m_quads [ f ], what_type ( f ) );
        m_lock.unlock ( );
        return false;
    }

    void make_active ( const hex & i_ ) noexcept {
        const size_type i = board::index ( i_.q, i_.r ), w = what ( i );
        if ( w < 3 ) {
            reset ( );
            m_last = i;
            m_lock.lock ( );
            setQuadTexture ( m_quads [ i ], w + 3 );
            m_lock.unlock ( );
        }
    }

    void unselect ( ) noexcept {
        if ( not_set != m_last ) {
            m_lock.lock ( );
            setQuadTexture ( m_quads [ m_last ], what_type ( m_last ) );
            m_lock.unlock ( );
            m_last = not_set;
        }
    }

    void reset ( ) noexcept {
        if ( not_set != m_last ) {
            const size_type l = what ( m_last );
            if ( l - 2 < ( 6 - 2 ) ) { // 2 < l < 6
                m_lock.lock ( );
                setQuadTexture ( m_quads [ m_last ], l % 3 );
                m_lock.unlock ( );
                m_last = not_set;
            }
        }
    }

    virtual void draw ( sf::RenderTarget & target, sf::RenderStates states ) const {
        // Apply the entity's transform -- combine it with the one that was passed by the caller.
        // states.transform *= getTransform ( ); // getTransform() is defined by sf::Transformable.
        // Apply the texture.
        // m_animator.run ( );
        m_lock.lock ( );
        states.texture = & m_texture;
        // You may also override states.shader or states.blendMode if you want.
        // Draw the vertex array.
        target.draw ( m_vertices, states );
        m_lock.unlock ( );
    }

    mutable play_area_lock m_agent_move_lock;

    const float m_hori, m_vert, m_circle_diameter, m_circle_radius;

    size_type m_last;

    bool agent_is_making_move;

    public:

    private:

    sf::Texture m_texture;

    state_reference m_state;
    clock_reference m_clock;
    mutable sf::CallbackAnimator m_animator;

    mutable play_area_lock m_lock;
    stlab::future<void> m_move_future;
    sf::VertexArray m_vertices;
    sf::Quad * m_quads, * m_to_quad, * m_from_quad;

    void resetToFromQuads ( ) noexcept {
        std::memset ( m_to_quad, 0, 2 * sizeof ( sf::Quad ) );
    }
    void resetFromQuad ( ) noexcept {
        std::memset ( m_from_quad, 0, sizeof ( sf::Quad ) );
    }
};


template<typename State>
PlayArea<State>::PlayArea ( State & state_, GameClock & clock_, const sf::Vector2f & center_, float hori_, float vert_, float circle_diameter_ ) :
    // Parameters.
    m_hori { hori_ },
    m_vert { vert_ },
    m_circle_diameter { circle_diameter_ },
    m_circle_radius { std::floorf ( m_circle_diameter * 0.5f ) },
    m_last { not_set },
    agent_is_making_move { false },
    m_state { state_ },
    m_clock { clock_ } {
    // Load play area graphics.
    sf::loadFromResource ( m_texture, CIRCLES_LARGE );
    m_texture.setSmooth ( true );
    // Init data structures.
    init ( center_ );
}


template<typename State>
[[ nodiscard ]] sf::Quad PlayArea<State>::makeVertex ( const sf::Vector2f & p_ ) const noexcept {
    return {
        sf::Vertex { sf::Vector2f { p_.x, p_.y }, sf::Vector2f { 0.0f, 0.0f } },
        sf::Vertex { sf::Vector2f { p_.x + m_circle_diameter, p_.y }, sf::Vector2f { m_circle_diameter, 0.0f } },
        sf::Vertex { sf::Vector2f { p_.x + m_circle_diameter, p_.y + m_circle_diameter }, sf::Vector2f { m_circle_diameter, m_circle_diameter } },
        sf::Vertex { sf::Vector2f { p_.x, p_.y + m_circle_diameter }, sf::Vector2f { 0.0f, m_circle_diameter } }
    };
}

template<typename State>
void PlayArea<State>::init ( const sf::Vector2f & center_ ) noexcept {
    // Construct vertices.
    m_vertices.setPrimitiveType ( sf::Quads );
    m_vertices.resize ( 4 * ( 2 * board::size ( ) + 2 ) );
    m_quads = reinterpret_cast<sf::Quad*> ( & m_vertices [ 0 ] );
    size_type i = 0;
    sf::Vector2f p = center_ - sf::Vector2f { m_circle_radius, m_circle_radius };
    m_quads [ i ] = makeVertex ( p );
    for ( size_type ring = 1; ring <= board::radius ( ); ++ring ) {
        p.x += m_hori; // Move east.
        for ( size_type j = 0; j < ring; ++j ) { // nw.
            p.x -= m_hori / 2; p.y -= m_vert;
            m_quads [ ++i ] = makeVertex ( p );
        }
        for ( size_type j = 0; j < ring; ++j ) { // w.
            p.x -= m_hori;
            m_quads [ ++i ] = makeVertex ( p );
        }
        for ( size_type j = 0; j < ring; ++j ) { // sw.
            p.x -= m_hori / 2; p.y += m_vert;
            m_quads [ ++i ] = makeVertex ( p );
        }
        for ( size_type j = 0; j < ring; ++j ) { // se.
            p.x += m_hori / 2; p.y += m_vert;
            m_quads [ ++i ] = makeVertex ( p );
        }
        for ( size_type j = 0; j < ring; ++j ) { // e.
            p.x += m_hori;
            m_quads [ ++i ] = makeVertex ( p );
        }
        for ( size_type j = 0; j < ring; ++j ) { // ne.
            p.x += m_hori / 2; p.y -= m_vert;
            m_quads [ ++i ] = makeVertex ( p );
        }
    }
    auto const quad_size = ( m_vertices.getVertexCount ( ) - 2 ) / 8;
    m_to_quad = m_quads + quad_size;
    // Sort m_quads lambda.
    auto quads_less = [ ] ( const auto & a, const auto & b ) {
        return ( a.v0.position.y < b.v0.position.y ) or ( a.v0.position.y == b.v0.position.y and a.v0.position.x < b.v0.position.x );
    };
    std::sort ( m_quads, m_to_quad, quads_less );
    std::memcpy ( m_to_quad, m_quads, quad_size * sizeof ( sf::Quad ) );
    m_quads = m_to_quad;
    m_to_quad += quad_size;
    m_from_quad = m_to_quad + 1;
}
