
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

    [[ nodiscard ]] T & at ( const size_type q_, const size_type r_ ) noexcept {
        if constexpr ( zero_base ) {
            // Center at { 0, 0 }.
            return m_data [ q_ + std::max ( radius ( ), r_ ) ] [ r_ + radius ( ) ];
        }
        else {
            // Center at { radius, radius }.
            return m_data [ q_ + std::max ( size_type { 0 }, r_ - 2 * radius ( ) ) ] [ r_ ];
        }
    }
    [[ nodiscard ]] T at ( const size_type q_, const size_type r_ ) const noexcept {
        return at ( q_, r_ );
    }
    [[ nodiscard ]] T & at ( const Hex<R> & h_ ) noexcept {
        return at ( h_.q, h_.r );
    }
    [[ nodiscard ]] T at ( const Hex<R> & h_ ) const noexcept {
        return at ( h_.q, h_.r );
    }

    [[ nodiscard ]] T & operator [ ] ( const Hex<R> & h_ ) noexcept {
        return at ( h_ );
    }
    [[ nodiscard ]] T operator [ ] ( const Hex<R> & h_ ) const noexcept {
        return at ( h_ );
    }

    [[ nodiscard ]] T * data ( ) noexcept {
        return &m_data [ 0 ] [ 0 ];
    }
    [[ nodiscard ]] const T * data ( ) const noexcept {
        return &m_data [ 0 ] [ 0 ];
    }

    void print ( ) {
        for ( int r = 0; r < height ( ); ++r ) {
            for ( int q = 0; q < width ( ); ++q ) {
                std::cout << std::setw ( 3 ) << m_data [ q ] [ r ];
            }
            std::cout << nl;
        }
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
    Hex ax { 0 , 0 };
    quads [ i ] = makeVertex ( p );
    m_vertex_indices.at ( ax ) = i;
    for ( int ring = 1; ring <= int { State::radius ( ) }; ++ring ) {
        p.x += m_hori; // Move east.
        ++ax.q;
        for ( int j = 0; j < ring; ++j ) { // nw.
            p.x -= m_hori / 2; p.y -= m_vert;
            --ax.r;
            quads [ ++i ] = makeVertex ( p );
            m_vertex_indices.at ( ax ) = i;
        }
        for ( int j = 0; j < ring; ++j ) { // w.
            p.x -= m_hori;
            --ax.q;
            quads [ ++i ] = makeVertex ( p );
            m_vertex_indices.at ( ax ) = i;
        }
        for ( int j = 0; j < ring; ++j ) { // sw.
            p.x -= m_hori / 2; p.y += m_vert;
            --ax.q; ++ax.r;
            quads [ ++i ] = makeVertex ( p );
            m_vertex_indices.at ( ax ) = i;
        }
        for ( int j = 0; j < ring; ++j ) { // se.
            p.x += m_hori / 2; p.y += m_vert;
            ++ax.r;
            quads [ ++i ] = makeVertex ( p );
            m_vertex_indices.at ( ax ) = i;
        }
        for ( int j = 0; j < ring; ++j ) { // e.
            p.x += m_hori;
            ++ax.q;
            quads [ ++i ] = makeVertex ( p );
            m_vertex_indices.at ( ax ) = i;
        }
        for ( int j = 0; j < ring; ++j ) { // ne.
            p.x += m_hori / 2; p.y -= m_vert;
            ++ax.q; --ax.r;
            quads [ ++i ] = makeVertex ( p );
            m_vertex_indices.at ( ax ) = i;
        }
    }
    // Sort quads lambda.
    auto quads_less = [ ] ( const auto & a, const auto & b ) {
        return ( a.v0.position.y < b.v0.position.y ) or ( a.v0.position.y == b.v0.position.y and a.v0.position.x < b.v0.position.x );
    };
    // Establish the new order of the vertices by index.
    std::array<int, State::size ( )> sorted_index;
    std::iota ( std::begin ( sorted_index ), std::end ( sorted_index ), 0 );
    std::sort ( std::begin ( sorted_index ), std::end ( sorted_index ), [ quads, quads_less ] ( int i, int j ) { return quads_less ( quads [ i ], quads [ j ] ); } );
    // Invert the lookup.
    std::array<int, State::size ( )> inverted;
    std::iota ( std::begin ( inverted ), std::end ( inverted ), 0 );
    std::sort ( std::begin ( inverted ), std::end ( inverted ), [ &sorted_index ] ( int i, int j ) { return sorted_index [ i ] < sorted_index [ j ]; } );
    // Replace the old index with the new index.
    ax = { 0, 0 };
    m_vertex_indices.at ( ax ) = inverted [ m_vertex_indices.at ( ax ) ];
    for ( int ring = 1; ring <= int { State::radius ( ) }; ++ring ) {
        ++ax.q;
        for ( int j = 0; j < ring; ++j ) { // nw.
            --ax.r;
            m_vertex_indices.at ( ax ) = inverted [ m_vertex_indices.at ( ax ) ];
        }
        for ( int j = 0; j < ring; ++j ) { // w.
            --ax.q;
            m_vertex_indices.at ( ax ) = inverted [ m_vertex_indices.at ( ax ) ];
        }
        for ( int j = 0; j < ring; ++j ) { // sw.
            --ax.q, ++ax.r;
            m_vertex_indices.at ( ax ) = inverted [ m_vertex_indices.at ( ax ) ];
        }
        for ( int j = 0; j < ring; ++j ) { // se.
            ++ax.r;
            m_vertex_indices.at ( ax ) = inverted [ m_vertex_indices.at ( ax ) ];
        }
        for ( int j = 0; j < ring; ++j ) { // e.
            ++ax.q;
            m_vertex_indices.at ( ax ) = inverted [ m_vertex_indices.at ( ax ) ];
        }
        for ( int j = 0; j < ring; ++j ) { // ne.
            ++ax.q; --ax.r;
            m_vertex_indices.at ( ax ) = inverted [ m_vertex_indices.at ( ax ) ];
        }
    }
    //
    m_vertex_indices.print ( );

    HexContainer<sidx, State::radius ( )> vertex_indices;

    int s = State::radius ( ), c = 0, idx = 0;

    for ( ; s > -1; --s ) {

        std::cout << s << ' ' << ( State::width ( ) - s ) << nl;
    }
    ++s;
    for ( ; s < State::radius ( ); ++s ) {

        std::cout << s << ' ' << ( State::width ( ) - 1 - s ) << nl;
    }

    std::cout << nl;

    vertex_indices.print ( );

    std::cout << nl;

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
