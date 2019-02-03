
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

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Extensions.hpp>

#include "App.hpp"

#include "resource.h"


#ifndef nl
#define nl '\n'
#endif


//  https://stackoverflow.com/questions/22128872/simple-c-sfml-program-high-cpu-usage


[[ nodiscard ]] App::uidx App::pointToIdx ( const sf::Vector2f & p_ ) const noexcept {
    return MadoState::hex_to_idx ( pointToHex ( p_ ) );
}
[[ nodiscard ]] App::hex App::pointToHex ( sf::Vector2f p_ ) const noexcept {
    // https://www.redblobgames.com/grids/hexagons/#comment-1063818420
    static const float radius { m_hori * 0.5773502588f };
    static const sf::Vector2f center { m_center.x, m_center.y - radius };
    p_ -= center;
    p_.x /= m_hori; p_.y /= radius;
    int q = int_floorf ( p_.y + p_.x ), r = int_floorf ( ( int_floorf ( p_.y - p_.x ) + q ) * 0.3333333433f );
    q = int_floorf ( ( int_floorf ( 2.0f * p_.x + 1.0f ) + q ) * 0.3333333433f ) - r;
    q += static_cast<int> ( MadoState::radius ( ) );
    r += static_cast<int> ( MadoState::radius ( ) );
    return { static_cast<sidx> ( std::clamp ( q, 0, 2 * static_cast<int> ( MadoState::radius ( ) ) ) ), static_cast<sidx> ( std::clamp ( r, 0, 2 * static_cast<int> ( MadoState::radius ( ) ) ) ) };
}


[[ nodiscard ]] bool App::playAreaContains ( sf::Vector2f p_ ) const noexcept {
    // http://www.playchilla.com/how-to-check-if-a-point-is-inside-a-hexagon
    static const float hori { MadoState::width ( ) * 0.5f * m_vert }, vert { hori * 0.5773502588f }, vert_2 { 2.0f * vert }, hori_vert_2 { hori * vert_2 };
    p_ -= m_center;
    p_.x = std::abs ( p_.x ); p_.y = std::abs ( p_.y );
    // x- and y-coordinates swapped (for flat-topped hexagon).
    if ( p_.y > hori or p_.x > vert_2 )
        return false;
    return ( hori_vert_2 - vert * p_.y - hori * p_.x ) >= 0.0f;
}


App::App ( ) :
    // Setup parameters.
    m_state { },
    m_hori { 74.0f },
    m_vert { 64.0f },
    m_window_width { MadoState::width ( ) * m_hori + m_vert + 1.0f },
    m_window_height { MadoState::height ( ) * m_vert + m_vert + 1.0f + 12.0f },
    m_center { sf::Vector2f { m_window_width * 0.5f, m_window_height * 0.5f + 6.0f } },
    m_taskbar { m_window_width },
    m_play_area { m_center, m_hori, m_vert, 67.0f } {
    m_settings.antialiasingLevel = 8u;
    // Create the m_window.
    m_window.create ( sf::VideoMode ( static_cast<std::uint32_t> ( m_window_width ), static_cast<std::uint32_t> ( m_window_height ) ), L"Mado", sf::Style::None, m_settings );
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
    m_animator.reserve ( 32 );
    m_overlay.setSize ( sf::Vector2f { m_window_width, m_window_height } );
    auto update_overlay_alpha = [ this ] ( const float v ) { m_overlay.setFillColor ( sf::Color { 10u, 10u, 10u, static_cast<sf::Uint8> ( v ) } ); };
    m_animator.emplace ( LAMBDA_EASING_START_END_DURATION ( update_overlay_alpha, sf::easing::exponentialInEasing, 255.0f, 0.0f, 600 ) );
    m_window.requestFocus ( );
}


void App::setIcon ( ) noexcept {
    HICON hIcon = LoadIcon ( GetModuleHandle ( NULL ), MAKEINTRESOURCE ( IDI_ICON1 ) );
    if ( hIcon ) {
        SendMessage ( m_window.getSystemHandle ( ), WM_SETICON, ICON_BIG, ( LPARAM ) hIcon );
    }
}


bool App::runStartupAnimation ( ) noexcept {
    m_animator.run ( );
    m_window.clear ( sf::Color { 10u, 10u, 10u, 255u } );
    m_window.draw ( m_play_area );
    m_window.draw ( m_overlay );
    m_window.display ( );
    return m_animator.size ( );
}


void App::updateWindow ( ) noexcept {
    m_window.clear ( sf::Color { 10u, 10u, 10u, 255u } );
    m_window.draw ( m_taskbar );
    m_window.draw ( m_play_area );
    m_window.display ( );
    // Minimize if required (after updating above).
    if ( m_minimize ) {
        m_minimize = false;
        minimizeWindow ( );
    }
}


// https://en.sfml-dev.org/forums/index.php?topic=9829.0

void App::mouseEvents ( const sf::Event & event_ ) {
    const sf::Vector2f & mouse_position = m_mouse.update ( );
    if ( m_window_bounds.contains ( mouse_position ) ) {
        m_taskbar.update ( mouse_position );
        if ( Taskbar::State::in_active == m_taskbar.state ) {
            if ( playAreaContains ( mouse_position ) ) {
                m_play_area.activate ( pointToHex ( mouse_position ) );
            }
            else {
                m_play_area.reset ( );
            }
        }
        else {
            if ( sf::Mouse::isButtonPressed ( sf::Mouse::Left ) ) {
                if ( Taskbar::State::close == m_taskbar.state ) {
                    closeWindow ( );
                    return;
                }
                else if ( Taskbar::State::minimize == m_taskbar.state ) {
                    m_taskbar.reset ( );
                    m_play_area.reset ( );
                    m_minimize = true;
                }
            }
        }
    }
    else {
        m_taskbar.reset ( );
        m_play_area.reset ( );
    }
}


#if 0

bool App::doHumanMove ( const Point point_ ) noexcept {

    if ( m_state.getHexRefFromID ( m_human_id_to ).isClose ( point_ ) ) {

        const Move human_move = m_state.humanMove ( m_human_id_from, m_human_id_to );

        if ( human_move == Move::none ) {

            m_state.getHexRefFromID ( m_human_id_to ).setOffsetFromPoint ( point_ );

            return false;
        }

        if ( human_move == Move::invalid ) {

            return false;
        }

        if ( human_move.isCapture ( ) ) {

            m_human_stone_captor.start ( m_state.other ( human_move.captured ( ) ), 0.4f, sf::milliseconds ( 100 ) );
        }

        m_state.doMove ( human_move );
        m_state.getHexRefFromID ( m_human_id_to ).setOffsetFromPoint ( point_ );

        return true;
    }

    return false;
}


void App::doAgentRandomMove ( ) noexcept {

    const Move agent_move = m_state.randomMove ( );

    if ( agent_move not_eq Move::invalid ) {

        if ( agent_move.isCapture ( ) ) {

            m_agent_stone_captor.start ( agent_move.captured ( ), 0.4f, sf::milliseconds ( 350 ) );
        }

        m_agent_stone_mover.start ( agent_move, 0.25f );
        m_state.doMove ( agent_move );

        return;
    }

    std::cout << "Invalid random move.\n";
    exit ( 0 );
}


void App::doAgentMctsMove ( ) noexcept {

    const Move agent_move = mcts::Mcts<os::OskaState>::compute ( m_state );

    if ( agent_move not_eq Move::invalid ) {

        if ( agent_move.isCapture ( ) ) {

            m_agent_stone_captor.start ( agent_move.captured ( ), 0.4f, sf::milliseconds ( 350 ) );
        }

        m_agent_stone_mover.start ( agent_move, 0.25f );
        m_state.doMove ( agent_move );

        return;
    }

    std::cout << "Invalid mcts move.\n";
    exit ( EXIT_FAILURE );
}

#endif
