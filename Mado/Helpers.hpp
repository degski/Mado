
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
#include <vector>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Extensions.hpp>

#include "Types.hpp"
#include "Globals.hpp"
#include "Mado.hpp"


namespace sf {
using Boxf = Box<float>;

struct Quad {
    Vertex v0, v1, v2, v3;
};
}

enum class display : int { in_active_vacant = 0, in_active_red, in_active_green, active_vacant, active_red, active_green };


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
        return m_data [ r_ ] [ q_ - std::max ( size_type { 0 }, r_ - radius ( ) ) ];
    }
    [[ nodiscard ]] T at ( const size_type q_, const size_type r_ ) const noexcept {
        return m_data [ r_ ] [ q_ - std::max ( size_type { 0 }, r_ - radius ( ) ) ];
    }
    [[ nodiscard ]] T & at ( const hex<R> & h_ ) noexcept {
        return m_data [ static_cast<size_type> ( h_.r ) ] [ static_cast<size_type> ( h_.q ) - std::max ( size_type { 0 }, static_cast<size_type> ( h_.r ) - radius ( ) ) ];
    }
    [[ nodiscard ]] T at ( const hex<R> & h_ ) const noexcept {
        return m_data [ static_cast<size_type> ( h_.r ) ] [ static_cast<size_type> ( h_.q ) - std::max ( size_type { 0 }, static_cast<size_type> ( h_.r ) - radius ( ) ) ];
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
