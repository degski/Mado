
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
#include <iostream>
#include <vector>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Extensions.hpp>

#include "Types.hpp"
#include "Globals.hpp"


#include "resource.h"


namespace sf {
using Boxf = Box<float>;

struct Quad {
    Vertex v0, v1, v2, v3;
};
}



template<typename T, std::size_t R, bool zero_base = true, typename SizeType = int, typename = std::enable_if_t<std::is_default_constructible_v<T>, T>>
struct HexContainer {

    using size_type = SizeType;
    using value_type = T;
    using pointer = T * ;
    using const_pointer = const T * ;
    using reference = T & ;
    using const_reference = const T &;

    [[ nodiscard ]] static constexpr size_type radius ( ) noexcept {
        return static_cast< std::size_t > ( R );
    }
    [[ nodiscard ]] static constexpr size_type width ( ) noexcept {
        return 2 * radius ( ) + 1;
    }
    [[ nodiscard ]] static constexpr size_type height ( ) noexcept {
        return 2 * radius ( ) + 1;
    }
    [[ nodiscard ]] static constexpr size_type size ( ) noexcept {
        return width ( ) * height ( );
    }

    T m_data [ 2 * R + 1 ] [ 2 * R + 1 ];

    HexContainer ( ) noexcept : m_data { { T ( ) } } { }

    [[ nodiscard ]] reference at ( const size_type q_, const size_type r_ ) noexcept {
        if constexpr ( zero_base ) {
            // Center at { 0, 0 }.
            return m_data [ r_ + radius ( ) ] [ q_ + std::max ( radius ( ), r_ ) ];
        }
        else {
            // Center at { radius, radius }.
            return m_data [ r_ ] [ q_ + std::max ( size_type { 0 }, r_ - 2 * radius ( ) ) ];
        }
    }
    [[ nodiscard ]] const_reference at ( const size_type q_, const size_type r_ ) const noexcept {
        return at ( q_, r_ );
    }
    [[ nodiscard ]] reference at ( const Hex<R> & h_ ) noexcept {
        return at ( h_.q, h_.r );
    }
    [[ nodiscard ]] const_reference at ( const Hex<R> & h_ ) const noexcept {
        return at ( h_.q, h_.r );
    }

    [[ nodiscard ]] reference operator [ ] ( const Hex<R> & h_ ) noexcept {
        return at ( h_ );
    }
    [[ nodiscard ]] const_reference operator [ ] ( const Hex<R> & h_ ) const noexcept {
        return at ( h_ );
    }

    [[ nodiscard ]] pointer data ( ) noexcept {
        return & m_data [ 0 ] [ 0 ];
    }
    [[ nodiscard ]] const_pointer data ( ) const noexcept {
        return & m_data [ 0 ] [ 0 ];
    }

    template<typename Stream>
    [[ maybe_unused ]] friend Stream & operator << ( Stream & out_, const HexContainer & h_ ) noexcept {
        for ( int r = 0; r < height ( ); ++r ) {
            for ( int q = 0; q < width ( ); ++q ) {
                out_ << std::setw ( 3 ) << h_.m_data [ r ] [ q ];
            }
            out_ << nl;
        }
        return out_;
    }
};

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
        if ( sf::Mouse::isButtonPressed ( sf::Mouse::Left ) ) {
            m_mouse_state = mouse_state::left_clicked;
        }
        m_data [ m_current ^ 1 ] = sf::Mouse::getPosition ( * m_window_ptr );
        if ( m_data [ 0 ] != m_data [ 1 ] ) {
            m_current ^= 1;
            m_position = sf::castVector2f ( m_data [ m_current ] );
            m_mouse_state = mouse_state::left_clicked == m_mouse_state ? mouse_state::moved_and_left_clicked : mouse_state::moved;
        }
        return m_position;
    }
};


template<typename State>
struct PlayArea : public sf::Drawable, public sf::Transformable {

    static constexpr int not_set = -1;

    enum DisplayType : int { vacant = 0, red, green };
    enum DisplayValue : int { in_active_vacant = 0, in_active_red, in_active_green, active_vacant, active_red, active_green, selected_vacant, selected_red, selected_green };

    // sf::VertexArray m_vertices; is an array of quads. The x coordinate of the texCoords of the
    // first sf::Vertex of the sf::Quad is a multiple of the DisplayValue, as per above, i.e. the
    // texCoords store the information as to which sub-texture is referenced.

    using sidx = typename State::sidx;
    using Hex = typename State::Hex;

    [[ nodiscard ]] sf::Quad makeVertex ( const sf::Vector2f & p_ ) const noexcept;
    void init ( const sf::Vector2f & center_ ) noexcept;

    PlayArea ( const sf::Vector2f & center_, float hori_, float vert_, float circle_diameter_ );

    private:

    void setTexture ( int v_, int i_ ) noexcept {
        v_ *= 4;
        const float left { i_ * m_circle_diameter }, right { left + m_circle_diameter };
        sf::Quad & quad = *reinterpret_cast<sf::Quad*> ( & m_vertices [ v_ ] );
        quad.v0.texCoords.x = left;
        quad.v1.texCoords.x = right;
        quad.v2.texCoords.x = right;
        quad.v3.texCoords.x = left;
    }

    [[ nodiscard ]] DisplayType display_type ( DisplayValue d_ ) const noexcept {
        return static_cast<DisplayType> ( static_cast<int> ( d_ ) % 3 );
    }

    [[ nodiscard ]] int what ( int i_ ) const noexcept {
        static const int radius = static_cast<int> ( m_circle_radius );
        return static_cast<int> ( m_vertices [ 4 * i_ ].texCoords.x ) % radius;
    }
    [[ nodiscard ]] DisplayValue what_value ( int i_ ) const noexcept {
        return static_cast<DisplayValue> ( what ( i_ ) );
    }
    [[ nodiscard ]] DisplayType what_type ( int i_ ) const noexcept {
        return static_cast<DisplayType> ( what ( i_ ) % 3 );
    }

    [[ nodiscard ]] bool are_neighbors ( const Hex a_, const Hex b_ ) const noexcept {
        if ( a_ != b_ ) {
            const typename Hex::value_type dq = a_.q - b_.q, dr = a_.r - b_.r;
            return std::abs ( dq ) + std::abs ( dr ) + std::abs ( -dq - dr ) == typename Hex::value_type { 2 };
        }
        return false;
    }

    public:

    [[ nodiscard ]] bool equal ( const Hex & i_, const DisplayValue d_ ) noexcept {
        const int i = m_vertex_indices [ i_ ], w = what_type ( i );
        if ( display_type ( d_ ) == w ) {
            setTexture ( i, w + 6 );
            return true;
        }
        return false;
    }
    [[ nodiscard ]] bool place ( const Hex & i_, const DisplayValue d_ ) noexcept {
        const int i = m_vertex_indices [ i_ ];
        if ( DisplayType::vacant == what_type ( i ) ) {
            setTexture ( i, d_ );
            m_last = i;
            return true;
        }
        return false;
    }
    [[ nodiscard ]] bool move ( const Hex & f_, const Hex & t_, const DisplayValue d_ ) noexcept {
        if ( are_neighbors ( f_, t_ ) ) {
            const int f = m_vertex_indices [ f_ ], t = m_vertex_indices [ t_ ];
            if ( display_type ( d_ ) == what_type ( f ) and DisplayValue::active_vacant == what_value ( t ) ) {
                setTexture ( f, DisplayValue::in_active_vacant );
                setTexture ( t, d_ );
                m_last = t;
                return true;
            }
        }
        const int f = m_vertex_indices [ f_ ];
        setTexture ( f, what_type ( f ) );
        return false;
    }

    void make_active ( const Hex & i_ ) noexcept {
        const int i = m_vertex_indices [ i_ ], w = what ( i );
        if ( w < 3 ) {
            reset ( );
            m_last = i;
            setTexture ( i, w + 3 );
        }
    }

    void reset ( ) noexcept {
        if ( not_set != m_last ) {
            const int l = what ( m_last );
            if ( 2 < l and l < 6 ) {
                setTexture ( m_last, l % 3 );
            }
            m_last = not_set;
        }
    }

    virtual void draw ( sf::RenderTarget & target, sf::RenderStates states ) const {
        // Apply the entity's transform -- combine it with the one that was passed by the caller.
        // states.transform *= getTransform ( ); // getTransform() is defined by sf::Transformable.
        // Apply the texture.
        states.texture = & m_texture;
        // You may also override states.shader or states.blendMode if you want.
        // Draw the vertex array.
        target.draw ( m_vertices, states );
    }

    const float m_hori, m_vert, m_circle_diameter, m_circle_radius;

    int m_last = not_set;

    sf::Texture m_texture;

    HexContainer<sidx, State::radius ( )> m_vertex_indices;
    sf::VertexArray m_vertices;
};


template<typename State>
PlayArea<State>::PlayArea ( const sf::Vector2f & center_, float hori_, float vert_, float circle_diameter_ ) :
    // Parameters.
    m_hori { hori_ },
    m_vert { vert_ },
    m_circle_diameter { circle_diameter_ },
    m_circle_radius { std::floorf ( m_circle_diameter * 0.5f ) } {
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
    m_vertices.setPrimitiveType ( sf::Quads );
    m_vertices.resize ( 4 * State::size ( ) );
    sf::Quad * quads = reinterpret_cast<sf::Quad*> ( & m_vertices [ 0 ] );
    int i = 0;
    sf::Vector2f p = center_ - sf::Vector2f { m_circle_radius, m_circle_radius };
    quads [ i ] = makeVertex ( p );
    for ( int ring = 1; ring <= int { State::radius ( ) }; ++ring ) {
        p.x += m_hori; // Move east.
        for ( int j = 0; j < ring; ++j ) { // nw.
            p.x -= m_hori / 2; p.y -= m_vert;
            quads [ ++i ] = makeVertex ( p );
        }
        for ( int j = 0; j < ring; ++j ) { // w.
            p.x -= m_hori;
            quads [ ++i ] = makeVertex ( p );
        }
        for ( int j = 0; j < ring; ++j ) { // sw.
            p.x -= m_hori / 2; p.y += m_vert;
            quads [ ++i ] = makeVertex ( p );
        }
        for ( int j = 0; j < ring; ++j ) { // se.
            p.x += m_hori / 2; p.y += m_vert;
            quads [ ++i ] = makeVertex ( p );
        }
        for ( int j = 0; j < ring; ++j ) { // e.
            p.x += m_hori;
            quads [ ++i ] = makeVertex ( p );
        }
        for ( int j = 0; j < ring; ++j ) { // ne.
            p.x += m_hori / 2; p.y -= m_vert;
            quads [ ++i ] = makeVertex ( p );
        }
    }
    // Create m_vertex_indices.
    int c = 0, z = 0;
    typename HexContainer<sidx, State::radius ( )>::pointer beg = m_vertex_indices.data ( );
    for ( int s = State::radius ( ); s > 0; --s ) {
        const int n = State::width ( ) - s;
        beg += z + s;
        std::iota ( beg, beg + n, c );
        c += n;
        z = n;
    }
    for ( int s = 0; s <= State::radius ( ); ++s ) {
        const int n = State::width ( ) - s;
        beg += z;
        std::iota ( beg, beg + n, c );
        beg += s;
        c += n;
        z = n;
    }
    // Sort quads lambda.
    auto quads_less = [ ] ( const auto & a, const auto & b ) {
        return ( a.v0.position.y < b.v0.position.y ) or ( a.v0.position.y == b.v0.position.y and a.v0.position.x < b.v0.position.x );
    };
    std::sort ( quads, quads + m_vertices.getVertexCount ( ) / 4, quads_less );
}


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

    void setTexture ( const State state_ ) noexcept {
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
        sf::Quad * quads = reinterpret_cast<sf::Quad*> ( & m_vertices [ 0 ] );
        quads [ 0 ] = makeVertex ( sf::Vector2f { window_width_ - width, 0.0f } );
    }

    virtual void draw ( sf::RenderTarget & target, sf::RenderStates states ) const {
        states.texture = & m_texture;
        target.draw ( m_vertices, states );
    }

    [[ nodiscard ]] State state ( ) const noexcept {
        return static_cast<State> ( static_cast<int> ( m_vertices [ 0 ].texCoords.x ) / static_cast<int> ( width ) );
    }

    void update ( const sf::Vector2f & p_ ) noexcept {
        setTexture ( m_minimize_bounds.contains ( p_ ) ? State::minimize : m_close_bounds.contains ( p_ ) ? State::close : State::in_active );
    }

    void reset ( ) noexcept {
        setTexture ( State::in_active );
    }

    private:

    sf::FloatRect m_minimize_bounds, m_close_bounds;

    sf::Texture m_texture;
    sf::VertexArray m_vertices;
};



struct PlayerTime {
    int min, sec;
};

struct GameClockTimes {
    PlayerTime agent, human;
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

    GameClock ( ) noexcept { }

    void set ( const int min_, const int sec_ = 0, const int delay_ = 0 ) noexcept {
        m_time [ Player::agent ] = m_time [ Player::human ] = sf::fminutes { min_ } +sf::fseconds { sec_ };
        m_delay_timer.set ( delay_ );
    }

    void restart ( const Player p_ ) noexcept {
        m_player = p_;
        m_start = m_clock.now ( ); // In case delay == 0 or very short.
        m_delay_timer.restart ( m_start );
    }

    [[ nodiscard ]] GameClockTimes update ( ) noexcept {
        const sf::HrClock::time_point now = m_clock.now ( );
        if ( m_delay_timer.expired ) {
            m_time [ m_player ] -= now - m_start;
            m_start = now;
        }
        else {
            if ( m_delay_timer.update ( now ) ) {
                // Just expired, start timing.
                m_start = now;
            }
        }
        // TODO: there's probably a better way.
        const int agent_time = static_cast<int> ( m_time [ Player::agent ].count ( ) ), human_time = static_cast<int> ( m_time [ Player::human ].count ( ) );
        return { { agent_time / 60'000, agent_time % 60'000 }, { human_time / 60'000, human_time % 60'000 } };
    }

    [[ maybe_unused ]] GameClockTimes update_next ( ) noexcept {
        const GameClockTimes t = update ( );
        restart ( Player::agent == m_player ? Player::human : Player::agent );
        return t;
    }

    private:

    // The clock.

    sf::HrClock::time_point m_start;
    sf::HrClock m_clock;
    std::array<sf::fmilliseconds, 2> m_time;
    DelayTimer m_delay_timer;
    Player m_player = Player::human;

    // Stuff to draw it.

    virtual void draw ( sf::RenderTarget & target, sf::RenderStates states ) const {
        states.texture = & m_texture;
        target.draw ( m_vertices, states );
    }

    void update ( const sf::Vector2f & p_ ) noexcept {
        // setTexture ( m_minimize_bounds.contains ( p_ ) ? State::minimize : m_close_bounds.contains ( p_ ) ? State::close : State::in_active );
    }

    void reset ( ) noexcept {
        // setTexture ( State::in_active );
    }

    private:

    sf::FloatRect m_left_bounds, m_right_bounds;

    sf::Texture m_texture;
    sf::VertexArray m_vertices;
};
