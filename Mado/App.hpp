
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
#include "Helpers.hpp"


#include "resource.h"


class App {

    using MadoState = Mado<4>;
    using uidx = typename MadoState::uidx;
    using sidx = typename MadoState::sidx;
    using hex = typename MadoState::hex;

    sf::CallbackAnimator m_animator;

    float m_window_width, m_window_height;
    sf::Vector2f m_center;
    float m_hori, m_vert;
    float m_circle_diameter, m_circle_radius, m_circle_radius_squared;

    sf::ContextSettings m_settings;
    sf::RenderWindow m_window;
    sf::FloatRect m_window_bounds;

    bool m_is_running = true;

    MadoState m_state;

    sf::Font m_font_regular, m_font_bold, m_font_mono, m_font_numbers;

    sf::Texture m_circles_texture;
    sf::Sprite m_circles;

    std::array<sf::Boxf, 6> m_tex_box;

    sf::Texture m_taskbar_texture;
    sf::Sprite m_taskbar;
    sf::IntRect m_taskbar_default, m_taskbar_minimize, m_taskbar_close;
    sf::FloatRect m_close_bounds, m_minimize_bounds;

    bool m_display_close = false, m_display_minimize = false, m_close = false, m_minimize = false, m_left_mousebutton_pressed = false;

    sf::Music m_music;

    MouseState m_mouse;

    int m_where = -1;
    display m_what = display::in_active_vacant;

    [[ nodiscard ]] sf::Quad makeVertex ( const sf::Vector2f & p_, const sf::Boxf & tb_ ) const noexcept;
    void makeVertexArray ( ) noexcept;

    template<typename SizeType>
    void setQuadTex ( SizeType i_, SizeType t_ ) noexcept;

    inline void activate ( ) noexcept {
        if ( static_cast<int> ( m_what ) < 3 ) {
            m_what = static_cast<display> ( static_cast<int> ( m_what ) + 3 );
            setQuadTex ( m_where, static_cast<int> ( m_what ) );
        }
    }
    inline void deactivate ( ) noexcept {
        if ( static_cast<int> ( m_what ) > 2 ) {
            m_what = static_cast<display> ( static_cast<int> ( m_what ) - 3 );
            setQuadTex ( m_where, static_cast<int> ( m_what ) );
        }
    }


    sf::VertexArray m_vertices;
    HexContainer<int, MadoState::radius ( )> m_vertex_indices;

    public:

    App ( );

private:

    [[ nodiscard ]] static constexpr float distance_squared ( const sf::Vector2f & p1_, const sf::Vector2f & p2_ ) noexcept {
        return ( ( p1_.x - p2_.x ) * ( p1_.x - p2_.x ) ) + ( ( p1_.y - p2_.y ) * ( p1_.y - p2_.y ) );
    }

    [[ nodiscard ]] inline int int_floorf ( float x ) const noexcept {
        return static_cast<int> ( x + 65'536.0f ) - 65'536;
    }

    [[ nodiscard ]] uidx pointToIdx ( const sf::Vector2f & p ) const noexcept;
    [[ nodiscard ]] hex pointToHex ( sf::Vector2f p_ ) const noexcept;

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

    sf::RectangleShape m_overlay;

    bool runStartupAnimation ( ) noexcept;
    void updateWindow ( ) noexcept;

private:

    // [[ nodiscard ]] bool doHumanMove ( const Point point_ ) noexcept;
    // void doAgentRandomMove ( ) noexcept;
    // void doAgentMctsMove ( ) noexcept;

public:

    void mouseEvents ( const sf::Event & event_ );
};
