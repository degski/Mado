
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

#include "../../KD-Tree/KD-Tree/ikdtree.hpp"
#include "multi_array.hpp"

#include "Types.hpp"
#include "Globals.hpp"
#include "Mado.hpp"


class App {

    using state = Mado<9>;
    using uidx = typename state::uidx;
    using sidx = typename state::sidx;
    using hex = typename state::hex;

    enum class display : int { in_active_vacant = 0, in_active_red, in_active_green, active_vacant, active_red, active_green };

    struct position {

        sf::Vector2f where;
        display what = display::in_active_vacant;

        position ( ) = default;
        position ( const position & ) noexcept = default;
        position ( position && ) noexcept = default;
        position ( const sf::Vector2f & where_, const display & what_ ) noexcept :
            where { where_ },
            what { what_ } {
        }
        position ( sf::Vector2f && where_, display && what_ ) noexcept :
            where { std::move ( where_ ) },
            what { std::move ( what_ ) } {
        }

        [[ nodiscard ]] inline bool is_active ( ) const noexcept {
            return static_cast<int> ( what ) > 2;
        }
        [[ nodiscard ]] inline bool is_inactive ( ) const noexcept {
            return static_cast<int> ( what ) < 3;
        }

        void activate ( ) noexcept {
            if ( is_inactive ( ) )
                what = static_cast<display> ( static_cast<int> ( what ) + 3 );
        }
        void deactivate ( ) noexcept {
            if ( is_active ( ) )
                what = static_cast<display> ( static_cast<int> ( what ) - 3 );
        }
    };


    struct mouse_status {

        enum State : int { none = 0, moved = 1, left_clicked = 2, moved_and_left_clicked = 3 };

        sf::RenderWindow * window = nullptr;
        bool current = false;
        State state = State::moved;
        sf::Vector2i data [ 2 ];
        sf::Vector2f position;

        mouse_status ( ) noexcept { }

        void initialize ( sf::RenderWindow & w_ ) noexcept {
            window = &w_;
            data [ 0 ] = data [ 1 ] = sf::Mouse::getPosition ( *window );
            position = sf::castVector2f ( data [ 0 ] );
        }

        [[ nodiscard ]] const sf::Vector2f & operator ( ) ( ) const noexcept {
            return position;
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
            state = State::none;
            if ( sf::Mouse::isButtonPressed ( sf::Mouse::Left ) ) {
                state = State::left_clicked;
            }
            data [ not ( current ) ] = sf::Mouse::getPosition ( *window );
            if ( data [ 0 ] != data [ 1 ] ) {
                current ^= true;
                position = sf::castVector2f ( data [ current ] );
                state = State::left_clicked == state ? State::moved_and_left_clicked : State::moved;
            }
            return position;
        }
    };

    using kdtree = kd::Tree2D<float, sf::Vector2<float>, kd::array_tag_t, state::size ( )>;
    using hextable = ma::Matrix<sf::Vector2f, static_cast<int>( state::width ( ) ), static_cast<int>( state::width ( ) ), -( static_cast<int>( state::width ( ) ) / 2 ), -(static_cast<int>( state::width ( ) )/2)>;
    using positions = std::array<position, kdtree::size ( )>;
    using indices = std::array<uidx, kdtree::size ( )>;

    public:

    float m_window_width, m_window_height;
    sf::Vector2f m_center;
    float m_hori, m_vert;
    sf::ContextSettings m_settings;

    sf::RenderWindow m_window;
    sf::FloatRect m_window_bounds;

    bool m_is_running = true;

    state m_state;

    kdtree m_kdtree;
    hextable m_hex;
    positions m_positions;
    indices m_indices;

    sf::Font m_font_regular, m_font_bold, m_font_mono, m_font_numbers;

    sf::Texture m_circles_texture;
    sf::Sprite m_circles;

    std::array<sf::IntRect, 6> m_display_rect;

    float m_circle_radius_squared;

    sf::Texture m_taskbar_texture;
    sf::Sprite m_taskbar;
    sf::IntRect m_taskbar_default, m_taskbar_minimize, m_taskbar_close;
    sf::FloatRect m_close_bounds, m_minimize_bounds;

    bool m_display_close = false, m_display_minimize = false, m_close = false, m_minimize = false, m_left_mousebutton_pressed = false;

    sf::Music m_music;

    mouse_status m_mouse;
    std::ptrdiff_t m_idx = -1;

    App ( );

private:


    template<typename T, std::intptr_t W, typename = std::enable_if_t<std::is_default_constructible_v<T>, T>>
    struct HexStorage {

        [[ nodiscard ]] static constexpr std::intptr_t radius ( ) noexcept {
            return W / 2;
        }
        [[ nodiscard ]] static constexpr std::intptr_t width ( ) noexcept {
            return W;
        }
        [[ nodiscard ]] static constexpr std::intptr_t size ( ) noexcept {
            return width ( ) * width ( );
        }

        T m_data [ width ( ) ] [ width ( ) ];

        HexStorage ( ) noexcept : m_data { { T ( ) } } { }

        [[ nodiscard ]] T & at ( const std::intptr_t q_, const std::intptr_t r_ ) noexcept {
            return m_data [ r_ ] [ q_ - std::max ( std::intptr_t { 0 }, r_ - radius ( ) ) ];
        }
        [[ nodiscard ]] T at ( const std::intptr_t q_, const std::intptr_t r_ ) const noexcept {
            return m_data [ r_ ] [ q_ - std::max ( std::intptr_t { 0 }, r_ - radius ( ) ) ];
        }

        [[ nodiscard ]] T * data ( ) noexcept {
            return &m_data [ 0 ] [ 0 ];
        }
        [[ nodiscard ]] const T * data ( ) const noexcept {
            return &m_data [ 0 ] [ 0 ];
        }
    };


    [[ nodiscard ]] static constexpr float distance_squared ( const sf::Vector2f & p1_, const sf::Vector2f & p2_ ) noexcept {
        return ( ( p1_.x - p2_.x ) * ( p1_.x - p2_.x ) ) + ( ( p1_.y - p2_.y ) * ( p1_.y - p2_.y ) );
    }

    [[ nodiscard ]] uidx pointToIdx ( sf::Vector2f p ) const noexcept;
    [[ nodiscard ]] hex pointToHex ( sf::Vector2f p_ ) const noexcept;
    [[ nodiscard ]] uidx pointToCoord ( sf::Vector2f p ) const noexcept;

    [[ nodiscard ]] std::vector<sf::Vector2f> positionData ( ) const noexcept;
    [[ nodiscard ]] std::vector<std::pair<sf::Vector2f, uidx>> positionIdxData ( ) const noexcept;
    void hexData ( ) noexcept;

    [[ nodiscard ]] bool playAreaContains ( sf::Vector2f p_ ) const noexcept;

    void setIcon ( ) noexcept;

public:

    [[ nodiscard ]] inline bool isWindowOpen ( ) const {
        return m_window.isOpen ( );
    }

    [[ nodiscard ]] inline bool pollWindowEvent ( sf::Event &event_ ) {
        return m_window.pollEvent ( event_ );
    }

    inline void closeWindow ( ) noexcept {
        std::cout << "close\n";
        m_window.close ( );
    }

    inline void minimizeWindow ( ) noexcept {
        std::cout << "minimize\n";
        pause ( );
        ShowWindow ( m_window.getSystemHandle ( ), SW_MINIMIZE );
    }

    inline void pause ( ) noexcept {
        std::cout << "pause\n";
        m_music.pause ( );
    }

    inline void resume ( ) noexcept {
        std::cout << "resume\n";
        m_music.play ( );
        m_is_running = true;
    }

    [[ nodiscard ]] inline bool isPaused ( ) const noexcept {
        return not ( m_is_running );
    }
    [[ nodiscard ]] inline bool isRunning ( ) const noexcept {
        return m_is_running;
    }

    void updateWindow ( ) noexcept;

private:

    // [[ nodiscard ]] bool doHumanMove ( const Point point_ ) noexcept;
    // void doAgentRandomMove ( ) noexcept;
    // void doAgentMctsMove ( ) noexcept;

public:

    void mouseEvents ( const sf::Event & event_ );
};
