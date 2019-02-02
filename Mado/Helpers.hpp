
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


template<typename T, std::size_t R, typename SizeType = int, typename = std::enable_if_t<std::is_default_constructible_v<T>, T>>
struct HexContainer {

    using size_type = SizeType;

    [[ nodiscard ]] static constexpr size_type radius ( ) noexcept {
        return static_cast<std::size_t> ( R );
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
        return m_data [ r_ ] [ q_ - std::max ( size_type { 0 }, radius ( ) - r_ ) ];
    }
    [[ nodiscard ]] T at ( const size_type q_, const size_type r_ ) const noexcept {
        return m_data [ r_ ] [ q_ - std::max ( size_type { 0 }, radius ( ) - r_ ) ];
    }
    [[ nodiscard ]] T & at ( const hex<R> & h_ ) noexcept {
        return m_data [ static_cast<size_type> ( h_.r ) ] [ static_cast<size_type> ( h_.q ) - std::max ( size_type { 0 }, radius ( ) - static_cast<size_type> ( h_.r ) ) ];
    }
    [[ nodiscard ]] T at ( const hex<R> & h_ ) const noexcept {
        return m_data [ static_cast<size_type> ( h_.r ) ] [ static_cast<size_type> ( h_.q ) - std::max ( size_type { 0 }, radius ( ) - static_cast<size_type> ( h_.r ) ) ];
    }

    [[ nodiscard ]] T & operator [ ] ( const hex<R> & h_ ) noexcept {
        return at ( h_ );
    }
    [[ nodiscard ]] T operator [ ] ( const hex<R> & h_ ) const noexcept {
        return at ( h_ );
    }

    [[ nodiscard ]] T * data ( ) noexcept {
        return & m_data [ 0 ] [ 0 ];
    }
    [[ nodiscard ]] const T * data ( ) const noexcept {
        return & m_data [ 0 ] [ 0 ];
    }

    void print ( ) {
        for ( int j = 0; j < 2 * R + 1; ++j ) {
            for ( int i = 0; i < 2 * R + 1; ++i ) {
                std::cout << std::setw ( 3 ) << m_data [ i ] [ j ];
            }
            std::cout << nl;
        }
    }
};


enum mouse_state : int { none = 0, moved = 1, left_clicked = 2, moved_and_left_clicked = 3 };


struct MouseState {

    sf::RenderWindow * window = nullptr;
    int current = 0;
    mouse_state state = mouse_state::moved;
    sf::Vector2i data [ 2 ];
    sf::Vector2f Position;

    MouseState ( ) noexcept { }

    void initialize ( sf::RenderWindow & w_ ) noexcept {
        window = & w_;
        data [ 0 ] = data [ 1 ] = sf::Mouse::getPosition ( *window );
        Position = sf::castVector2f ( data [ 0 ] );
    }

    [[ nodiscard ]] const sf::Vector2f & operator ( ) ( ) const noexcept {
        return Position;
    }

    [[ nodiscard ]] bool hadActivity ( ) const noexcept {
        return static_cast<int> ( state );
    }
    [[ nodiscard ]] bool hadNoActivity ( ) const noexcept {
        return not ( hadActivity ( ) );
    }

    [[ nodiscard ]] bool leftClicked ( ) const noexcept {
        return 1 < static_cast<int> ( state );
    }
    [[ nodiscard ]] bool hasMoved ( ) const noexcept {
        return static_cast<int> ( state ) & 1;
    }

    const sf::Vector2f & update ( ) noexcept {
        state = mouse_state::none;
        if ( sf::Mouse::isButtonPressed ( sf::Mouse::Left ) ) {
            state = mouse_state::left_clicked;
        }
        data [ current ^ 1 ] = sf::Mouse::getPosition ( * window );
        if ( data [ 0 ] != data [ 1 ] ) {
            current ^= 1;
            Position = sf::castVector2f ( data [ current ] );
            state = mouse_state::left_clicked == state ? mouse_state::moved_and_left_clicked : mouse_state::moved;
        }
        return Position;
    }
};


template<typename State>
struct PlayArea : public sf::Drawable, public sf::Transformable {

    static constexpr int not_set = -1;

    using display_type = std::int8_t;

    enum class display : display_type { in_active_vacant = 0, in_active_red, in_active_green, active_vacant, active_red, active_green };

    using sidx = typename State::sidx;
    using hex = typename State::hex;

    [[ nodiscard ]] sf::Quad makeVertex ( const sf::Vector2f & p_, const sf::Boxf & tb_ ) const noexcept;
    void init ( ) noexcept;

    PlayArea ( const sf::Vector2f & center_, float hori_, float vert_, float circle_diameter_ );

    [[ nodiscard ]] const sf::Boxf & getQuadTex ( display d_ ) const noexcept {
        return m_tex_box [ static_cast<int> ( d_ ) ];
    }
    void setQuadTex ( int i_, display d_ ) noexcept;

    [[ nodiscard ]] display make_active ( const int a_ ) noexcept {
        return m_what [ m_active ] = static_cast<display> ( static_cast<int> ( m_what [ m_active ] ) + 3 );
    }
    [[ nodiscard ]] display make_in_active ( const int a_ ) noexcept {
        return m_what [ m_active ] = static_cast<display> ( static_cast<int> ( m_what [ m_active ] ) - 3 );
    }

    void activate ( const hex & h_ ) noexcept {
        const int active = m_vertex_indices [ h_ ];
        if ( active != m_active ) {
            if ( not_set != m_active ) {
                setQuadTex ( m_active, make_in_active ( m_active ) );
            }
            m_active = active;
            setQuadTex ( m_active, make_active ( m_active ) );
        }
    }
    void de_activate ( ) noexcept {
        if ( not_set != m_active ) {
            setQuadTex ( m_active, make_in_active ( m_active ) );
            m_active = not_set;
        }
    }

    virtual void draw ( sf::RenderTarget & target, sf::RenderStates states ) const {
        // Apply the entity's transform -- combine it with the one that was passed by the caller.
        // states.transform *= getTransform ( ); // getTransform() is defined by sf::Transformable.
        // Apply the texture.
        states.texture = & m_circles_texture;
        // You may also override states.shader or states.blendMode if you want.
        // Draw the vertex array.
        target.draw ( m_vertices, states );
    }

    const sf::Vector2f & m_center;
    const float m_hori, m_vert, m_circle_diameter, m_circle_radius;

    int m_active = not_set;

    const std::array<sf::Boxf, 6> m_tex_box;

    sf::Texture m_circles_texture;

    HexContainer<int, State::radius ( )> m_vertex_indices;
    std::array<display, State::size ( )> m_what;
    sf::VertexArray m_vertices;
};




template<typename State>
PlayArea<State>::PlayArea ( const sf::Vector2f & center_, float hori_, float vert_, float circle_diameter_ ) :
    // Parameters.
    m_center { center_ },
    m_hori { hori_ },
    m_vert { vert_ },
    m_circle_diameter { circle_diameter_ },
    m_circle_radius { std::floorf ( m_circle_diameter * 0.5f ) },
    m_tex_box  {
        {
            { 0.0f, 0.0f, m_circle_diameter, m_circle_diameter }, { m_circle_diameter, 0.0f, 2.0f * m_circle_diameter, m_circle_diameter }, { 2.0f * m_circle_diameter, 0.0f, 3.0f * m_circle_diameter, m_circle_diameter },
            { 0.0f, m_circle_diameter, m_circle_diameter, 2.0f * m_circle_diameter }, { m_circle_diameter, m_circle_diameter, 2.0f * m_circle_diameter, 2.0f * m_circle_diameter }, { 2.0f * m_circle_diameter, m_circle_diameter, 3.0f * m_circle_diameter, 2.0f * m_circle_diameter }
        }
    } {
    // Load play area graphics.
    sf::loadFromResource ( m_circles_texture, CIRCLES_LARGE );
    m_circles_texture.setSmooth ( true );
    // Init data structures.
    init ( );
}


template<typename State>
[ [ nodiscard ] ] sf::Quad PlayArea<State>::makeVertex ( const sf::Vector2f & p_, const sf::Boxf & tb_ ) const noexcept {
    return {
        sf::Vertex { sf::Vector2f { p_.x, p_.y }, sf::Vector2f { tb_.left, tb_.top } },
        sf::Vertex { sf::Vector2f { p_.x + m_circle_diameter, p_.y }, sf::Vector2f { tb_.right, tb_.top } },
        sf::Vertex { sf::Vector2f { p_.x + m_circle_diameter, p_.y + m_circle_diameter }, sf::Vector2f { tb_.right, tb_.bottom } },
        sf::Vertex { sf::Vector2f { p_.x, p_.y + m_circle_diameter }, sf::Vector2f { tb_.left, tb_.bottom } }
    };
}

template<typename State>
void PlayArea<State>::init ( ) noexcept {
    m_vertices.setPrimitiveType ( sf::Quads );
    m_vertices.resize ( 4 * State::size ( ) );
    const sf::Boxf & tex_box = getQuadTex ( display::in_active_vacant );
    sf::Quad * quads = reinterpret_cast< sf::Quad* > ( & m_vertices [ 0 ] );
    int i = 0;
    sf::Vector2f p = m_center - sf::Vector2f { m_circle_radius, m_circle_radius };
    hex ax { static_cast<sidx> ( State::radius ( ) ), static_cast<sidx> ( State::radius ( ) ) };
    quads [ i ] = makeVertex ( p, tex_box );
    m_vertex_indices.at ( ax ) = i;
    for ( int ring = 1; ring <= int { State::radius ( ) }; ++ring ) {
        p.x += m_hori; // Move east.
        ++ax.q;
        for ( int j = 0; j < ring; ++j ) { // nw.
            p.x -= m_hori / 2; p.y -= m_vert;
            --ax.r;
            quads [ ++i ] = makeVertex ( p, tex_box );
            m_vertex_indices.at ( ax ) = i;
        }
        for ( int j = 0; j < ring; ++j ) { // w.
            p.x -= m_hori;
            --ax.q;
            quads [ ++i ] = makeVertex ( p, tex_box );
            m_vertex_indices.at ( ax ) = i;
        }
        for ( int j = 0; j < ring; ++j ) { // sw.
            p.x -= m_hori / 2; p.y += m_vert;
            --ax.q; ++ax.r;
            quads [ ++i ] = makeVertex ( p, tex_box );
            m_vertex_indices.at ( ax ) = i;
        }
        for ( int j = 0; j < ring; ++j ) { // se.
            p.x += m_hori / 2; p.y += m_vert;
            ++ax.r;
            quads [ ++i ] = makeVertex ( p, tex_box );
            m_vertex_indices.at ( ax ) = i;
        }
        for ( int j = 0; j < ring; ++j ) { // e.
            p.x += m_hori;
            ++ax.q;
            quads [ ++i ] = makeVertex ( p, tex_box );
            m_vertex_indices.at ( ax ) = i;
        }
        for ( int j = 0; j < ring; ++j ) { // ne.
            p.x += m_hori / 2; p.y -= m_vert;
            ++ax.q; --ax.r;
            quads [ ++i ] = makeVertex ( p, tex_box );
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
    ax = { static_cast<sidx> ( State::radius ( ) ), static_cast<sidx> ( State::radius ( ) ) };
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
    // Fill the initial displays.
    std::fill ( std::begin ( m_what ), std::end ( m_what ), display::in_active_vacant );
    // Finally, sort the vertices.
    std::sort ( quads, quads + m_vertices.getVertexCount ( ) / 4, quads_less );
}


template<typename State>
void PlayArea<State>::setQuadTex ( int i_, display d_ ) noexcept {
    i_ *= 4;
    const sf::Boxf & tex_box = getQuadTex ( d_ );
    sf::Quad & quads = *reinterpret_cast<sf::Quad*> ( & m_vertices [ i_ ] );
    quads.v0.texCoords = sf::Vector2f { tex_box.left, tex_box.top };
    quads.v1.texCoords = sf::Vector2f { tex_box.right, tex_box.top };
    quads.v2.texCoords = sf::Vector2f { tex_box.right, tex_box.bottom };
    quads.v3.texCoords = sf::Vector2f { tex_box.left, tex_box.bottom };
}
