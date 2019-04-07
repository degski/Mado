
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
#include <cstring>

#include <algorithm>
#include <array>
#include <vector>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Extensions.hpp>

#include <fsmlite/fsm.hpp>

#include <sax/singleton.hpp>

#include "Globals.hpp"
#include "Mado.hpp"
#include "Drawables.hpp"


#include "resource.h"



template<typename GameState, typename Position>
struct NextMove {

    enum State : int { select = 0, place, move };

    using position = Position;

    void reset ( ) noexcept {
        m_from = position { };
        m_to = position { };
        m_state = State::select;
    }

    [[ nodiscard ]] const position & from ( ) const noexcept {
        return m_from;
    }
    void from ( const position & f_ ) noexcept {
        m_from = f_;
        m_to = position { };
        m_state = State::move;
    }

    [[ nodiscard ]] const position & to ( ) const noexcept {
        return m_to;
    }
    void to ( const position & t_ ) noexcept {
        m_to = t_;
        m_state = State::select;
    }

    [[ nodiscard ]] State state ( ) const noexcept {
        return m_state;
    }
    void state ( const State & s_ ) noexcept {
        m_state = s_;
    }

    template<typename Stream>
    [[ maybe_unused ]] friend Stream & operator << ( Stream & out_, const NextMove & nm_ ) noexcept {
        switch ( static_cast<int> ( nm_.m_state ) ) {
        case 0:
            out_ << "select <" << nm_.m_from << nm_.m_to << ">";
            break;
        case 1:
            out_ << "place  <" << nm_.m_from << nm_.m_to << ">";
            break;
        case 2:
            out_ << "move   <" << nm_.m_from << nm_.m_to << ">";
            break;
        }
        return out_;
    }

    private:

    position m_from, m_to;
    State m_state = State::select;
};



class App {

    using MadoState = Mado<4>;

    using idx_type = typename MadoState::idx_type;
    using hex = typename MadoState::hex;
    using PlayArea = PlayArea<MadoState>;
    using NextMove = NextMove<MadoState, hex>;
    using Player = typename MadoState::value_type;

    MadoState m_state;

    float m_hori, m_vert, m_window_width, m_window_height;
    sf::Vector2f m_center;

    sf::ContextSettings m_settings;
    sf::RenderWindow m_window;
    sf::FloatRect m_window_bounds;

    sf::Font m_font_regular, m_font_bold, m_font_mono, m_font_numbers;

    bool m_is_running = true, m_minimize = false;

    MouseState m_mouse;

    Taskbar m_taskbar;
    GameClock m_game_clock;
    PlayArea m_play_area;

    NextMove m_human_move;

    sf::Music m_music;

    public:

    App ( );

private:

    [[ nodiscard ]] static constexpr float distance_squared ( const sf::Vector2f & p1_, const sf::Vector2f & p2_ ) noexcept {
        return ( ( p1_.x - p2_.x ) * ( p1_.x - p2_.x ) ) + ( ( p1_.y - p2_.y ) * ( p1_.y - p2_.y ) );
    }

    template<typename T>
    [[ nodiscard ]] inline T floorf ( float x ) const noexcept {
        return static_cast<T> ( static_cast<int> ( x - std::numeric_limits<idx_type>::min ( ) ) + std::numeric_limits<idx_type>::min ( ) );
    }

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
        m_game_clock.pause ( );
        m_music.pause ( );
    }

    inline void resume ( ) noexcept {
        std::cout << "resume\n";
        m_game_clock.resume ( );
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
    sf::Text m_overlay_text;

    void setupStartupAnimation ( ) noexcept;
    bool runStartupAnimation ( ) noexcept;
    void updateWindow ( ) noexcept;

    void mouseEvents ( const sf::Event & event_ );
};



[[ nodiscard] ] App::hex App::pointToHex ( sf::Vector2f p_ ) const noexcept {
    using value_type = typename hex::value_type;
    // https://www.redblobgames.com/grids/hexagons/#comment-1063818420
    static const float radius { m_hori * 0.5773502588f };
    static const sf::Vector2f center { m_center.x, m_center.y - radius };
    p_ -= center;
    p_.x /= m_hori; p_.y /= radius;
    hex h;
    h.q = floorf<value_type> ( p_.y + p_.x );
    h.r = floorf<value_type> ( ( floorf<value_type> ( p_.y - p_.x ) + h.q ) * 0.3333333433f );
    h.q = floorf<value_type> ( ( floorf<value_type> ( 2.0f * p_.x + 1.0f ) + h.q ) * 0.3333333433f ) - h.r;
    return h;
}


[[ nodiscard ]] bool App::playAreaContains ( sf::Vector2f p_ ) const noexcept {
    // http://www.playchilla.com/how-to-check-if-a-point-is-inside-a-hexagon
    static const float hori { MadoState::board::width ( ) * 0.5f * m_vert }, vert { hori * 0.5773502588f }, vert_2 { 2.0f * vert }, hori_vert_2 { hori * vert_2 };
    p_ -= m_center;
    p_.x = std::abs ( p_.x ); p_.y = std::abs ( p_.y );
    // x- and y-coordinates swapped (for flat-topped hexagon).
    if ( p_.y > hori or p_.x > vert_2 )
        return false;
    return ( hori_vert_2 - vert * p_.y - hori * p_.x ) > 0.0f;
}


App::App ( ) :
    // Setup parameters.
    m_state { },
    m_hori { 74.0f },
    m_vert { 64.0f },
    m_window_width { MadoState::board::width ( ) * m_hori + m_vert + 1.0f },
    m_window_height { MadoState::board::height ( ) * m_vert + m_vert + 1.0f + 12.0f },
    m_center { sf::Vector2f { m_window_width * 0.5f, m_window_height * 0.5f + 6.0f } },
    m_taskbar { m_window_width },
    m_game_clock { std::floorf ( ( m_window_width - ( MadoState::board::radius ( ) + 1 ) * m_hori ) / 4 ), m_window_width - std::floorf ( ( m_window_width - ( MadoState::board::radius ( ) + 1 ) * m_hori ) / 4 ), 76.5f },
    m_play_area { m_state, m_game_clock, m_center, m_hori, m_vert, 67.0f } {
    m_settings.antialiasingLevel = 8u;
    // Create the m_window.
    m_window.create ( sf::VideoMode ( static_cast< std::uint32_t > ( m_window_width ), static_cast< std::uint32_t > ( m_window_height ) ), L"Mado", sf::Style::None, m_settings );
    m_window_bounds = sf::FloatRect { 0.0f, 0.0f, m_window_width, m_window_height };
    // Set icon.
    setIcon ( );
     // Load fonts.
    sf::loadFromResource ( m_font_regular, __REGULAR_FONT__ );
    sf::loadFromResource ( m_font_bold, __BOLD_FONT__ );
    sf::loadFromResource ( m_font_mono, __MONO_FONT__ );
    sf::loadFromResource ( m_font_numbers, __NUMBERS_FONT__ );
    // Load sound.
    sf::loadFromResource ( m_music, MUSIC );
    m_music.setVolume ( 10.0f );
    m_music.setLoop ( true );
    sf::Music::TimeSpan loop;
    loop.offset = sf::seconds ( 3.0f );
    loop.length = sf::seconds ( 192.0f );
    m_music.setLoopPoints ( loop );
    m_music.play ( );
    // Player to move.
    // m_player_to_move.what = display::in_active_green;
    // Ge started.
    m_mouse.initialize ( m_window );
    Animator::instance ( ).reserve ( 32 );
    setupStartupAnimation ( );
    m_window.requestFocus ( );
}


void App::setIcon ( ) noexcept {
    HICON hIcon = LoadIcon ( GetModuleHandle ( NULL ), MAKEINTRESOURCE ( IDI_ICON1 ) );
    if ( hIcon ) {
        SendMessage ( m_window.getSystemHandle ( ), WM_SETICON, ICON_BIG, ( LPARAM ) hIcon );
    }
}


void App::setupStartupAnimation ( ) noexcept {
    m_overlay.setSize ( sf::Vector2f { m_window_width, m_window_height } );
    // Text.
    m_overlay_text.setFont ( m_font_mono );
    m_overlay_text.setCharacterSize ( 72u );
    m_overlay_text.setString ( "m a d o" );
    m_overlay_text.setColor ( sf::Color { 178u, 178u, 178u, 255u } );
    sf::centreOrigin ( m_overlay_text );
    m_overlay_text.setScale ( 0.01f, 0.01f );
    m_overlay_text.setPosition ( sf::Vector2f { m_window_width / 2.0f, m_window_height * 0.66f } );
    // Callbacks.
    auto update_overlay_alpha = [ & ] ( const float v ) {
        m_overlay.setFillColor ( sf::Color { 10u, 10u, 10u, static_cast< sf::Uint8 > ( v ) } );
    };
    auto update_overlay_text_alpha = [ & ] ( const float v ) {
        m_overlay_text.setFillColor ( sf::Color { 178u, 178u, 178u, static_cast< sf::Uint8 > ( v ) } );
    };
    auto update_overlay_text_position = [ & ] ( const float v ) {
        m_overlay_text.setPosition ( sf::Vector2f { m_window_width / 2.0f, m_window_height * v } );
    };
    auto update_overlay_text_rotate = [ & ] ( const float v ) {
        m_overlay_text.setRotation ( v );
    };
    auto update_overlay_text_scale = [ & ] ( const float v ) {
        m_overlay_text.setScale ( v, v );
    };
    // Start animation.
    Animator::instance ( ).emplace ( LAMBDA_EASING_START_END_DURATION ( update_overlay_alpha, sf::easing::exponentialInEasing, 255.0f, 0.0f, 3'000 ) );
    Animator::instance ( ).emplace ( LAMBDA_EASING_START_END_DURATION_DELAY ( update_overlay_text_alpha, sf::easing::exponentialInEasing, 255.0f, 0.0f, 750, 750 ) );
    Animator::instance ( ).emplace ( LAMBDA_EASING_START_END_DURATION ( update_overlay_text_position, sf::easing::exponentialInEasing, 0.5f, 0.33f, 1'000 ) );
    Animator::instance ( ).emplace ( LAMBDA_EASING_START_END_DURATION ( update_overlay_text_scale, sf::easing::exponentialInEasing, 0.01f, 1.0f, 1'000 ) );
    Animator::instance ( ).emplace ( LAMBDA_EASING_START_END_DURATION ( update_overlay_text_rotate, sf::easing::exponentialInEasing, 0.0f, 360.0f, 1'000 ) );
    Animator::instance ( ).emplace ( LAMBDA_EASING_START_END_DURATION_DELAY ( update_overlay_text_position, sf::easing::exponentialInEasing, 0.33f, 0.0f, 550, 1'000 ) );
}


bool App::runStartupAnimation ( ) noexcept {
    Animator::instance ( ).run ( );
    m_window.clear ( sf::Color { 10u, 10u, 10u, 255u } );
    m_window.draw ( m_play_area );
    m_window.draw ( m_overlay );
    m_window.draw ( m_overlay_text );
    m_window.display ( );
    return Animator::instance ( ).size ( );
}


void App::updateWindow ( ) noexcept {
    m_play_area.update ( );
    m_window.clear ( sf::Color { 10u, 10u, 10u, 255u } );
    m_window.draw ( m_taskbar );
    m_window.draw ( m_play_area );
    m_window.draw ( m_game_clock );
    m_window.display ( );
    // Minimize if required (after updating above).
    if ( m_minimize ) {
        m_minimize = false;
        minimizeWindow ( );
    }
}


// https://en.sfml-dev.org/forums/index.php?topic=9829.0

void App::mouseEvents ( const sf::Event & event_ ) {
    // Update mouse state.
    const sf::Vector2f & mouse_position = m_mouse.update ( );
    if ( m_window_bounds.contains ( mouse_position ) ) {
        // In window.
        const hex hex_position = pointToHex ( mouse_position );
        // if ( hex_position.valid ( ) ) {
        if ( hex::is_valid ( hex_position.q, hex_position.r ) ) {
            if ( not ( m_play_area.agent_is_making_move ) and sf::Mouse::isButtonPressed ( sf::Mouse::Left ) ) {
                // Selected a cicle.
                bool no_reset = false;
                switch ( m_human_move.state ( ) ) {
                case NextMove::State::select:
                    if ( ( no_reset = m_play_area.select ( hex_position, PlayArea::DisplayValue::active_red ) ) )
                        m_human_move.from ( hex_position );
                    break;
                case NextMove::State::place:
                    if ( ( no_reset = m_play_area.place ( hex_position, PlayArea::DisplayValue::active_red ) ) )
                        m_human_move.to ( hex_position );
                    break;
                case NextMove::State::move:
                    if ( ( no_reset = m_play_area.move ( m_human_move.from ( ), hex_position, PlayArea::DisplayValue::active_red ) ) )
                        m_human_move.to ( hex_position );
                    break;
                }
                if ( not ( no_reset ) )
                    m_human_move.reset ( );
            }
            else {
                // Just hovering in play area.
                m_play_area.make_active ( hex_position );
            }
        }
        else {
            // Not in play area.
            m_taskbar.update ( mouse_position );
            if ( Taskbar::State::in_active == m_taskbar.state ( ) ) {
                // In new area.
                if ( sf::Mouse::isButtonPressed ( sf::Mouse::Left ) ) {
                    if ( m_game_clock.isClicked ( mouse_position ) ) {
                        std::cout << "clock clicked\n";
                    }
                    else {
                        if ( not ( m_play_area.agent_is_making_move ) ) {
                            // Clicked the new area, after selecting where to move from.
                            if ( NextMove::State::move == m_human_move.state ( ) ) {
                                m_human_move.reset ( );
                                m_play_area.unselect ( );
                            }
                            // Requested placement.
                            else {
                                m_human_move.state ( NextMove::State::place );
                            }
                        }
                    }
                }
                m_play_area.make_inactive ( );
            }
            else {
                // In taskbar area.
                if ( sf::Mouse::isButtonPressed ( sf::Mouse::Left ) ) {
                    if ( Taskbar::State::close == m_taskbar.state ( ) ) {
                        closeWindow ( );
                        return;
                    }
                    else if ( Taskbar::State::minimize == m_taskbar.state ( ) ) {
                        m_taskbar.reset ( );
                        m_play_area.make_inactive ( );
                        m_minimize = true;
                        m_human_move.reset ( );
                    }
                }
                // Just hovering in taskbar area.
            }
        }
    }
    else {
        // Outside window.
        m_taskbar.reset ( );
        m_play_area.make_inactive ( );
        m_human_move.reset ( );
    }
}
