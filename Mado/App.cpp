
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


[[ nodiscard ]] App::uidx App::pointToIdx ( sf::Vector2f p_ ) const noexcept {
    // https://www.redblobgames.com/grids/hexagons/#comment-1063818420
    static const float radius { m_hori / 1.732050807568877193f };
    static const sf::Vector2f center { m_center.x, m_center.y - radius };
    p_ -= center;
    p_.x /= m_hori; p_.y /= radius;
    const float t = std::floorf ( p_.y + p_.x ), r = std::floorf ( ( std::floorf ( p_.y - p_.x ) + t ) / 3.0f );
    return state::hex_to_idx ( std::floorf ( ( std::floorf ( 2.0f * p_.x + 1.0f ) + t ) / 3.0f ) - r, r );
}

[[ nodiscard ]] App::hex App::pointToHex ( sf::Vector2f p_ ) const noexcept {
    // https://www.redblobgames.com/grids/hexagons/#comment-1063818420
    static const float radius { m_hori / 1.732050807568877193f };
    static const sf::Vector2f center { m_center.x, m_center.y - radius };
    p_ -= center;
    p_.x /= m_hori; p_.y /= radius;
    const float t = std::floorf ( p_.y + p_.x ), r = std::floorf ( ( std::floorf ( p_.y - p_.x ) + t ) / 3.0f );
    return { static_cast<sidx> ( std::floorf ( ( std::floorf ( 2.0f * p_.x + 1.0f ) + t ) / 3.0f ) - r ), static_cast<sidx> ( r ) };
}

template<typename T>
[[ nodiscard ]] inline bool is_even ( const T v_ ) noexcept {
    return v_ & T { 1 };
}

[[ nodiscard ]] App::uidx App::pointToCoord ( sf::Vector2f p_ ) const noexcept {
    static const float radius { m_hori / 1.732050807568877193f };
    static const float hm1 = m_center.x - ( ( state::width ( ) * m_hori ) / 2.0f ), hm2 = hm1 + 0.5f * m_hori, vm = m_center.y - ( ( state::width ( ) * m_vert ) / 2.0f );
    const uidx row = ( p_.y - vm ) / m_vert + 1;
    return is_even ( row ) ?
        static_cast<uidx> ( ( p_.x - hm1 ) / m_hori ) * 2 + 1 + row * state::cols ( ) :
        static_cast<uidx> ( ( p_.x - hm2 ) / m_hori ) * 2 + 2 + row * state::cols ( );
}


[[ nodiscard ]] std::vector<sf::Vector2f> App::positionData ( ) const noexcept {
    std::vector<sf::Vector2f> pos;
    pos.reserve ( state::size ( ) );
    sf::Vector2f p = m_center;
    pos.push_back ( p );
    for ( int ring = 1; ring <= int { state::radius ( ) }; ++ring ) {
        p.x += m_hori; // Move east.
        for ( int j = 0; j < ring; ++j ) { // nw.
            p.x -= m_hori / 2; p.y -= m_vert;
            pos.push_back ( p );
        }
        for ( int j = 0; j < ring; ++j ) { // w.
            p.x -= m_hori;
            pos.push_back ( p );
        }
        for ( int j = 0; j < ring; ++j ) { // sw.
            p.x -= m_hori / 2; p.y += m_vert;
            pos.push_back ( p );
        }
        for ( int j = 0; j < ring; ++j ) { // se.
            p.x += m_hori / 2; p.y += m_vert;
            pos.push_back ( p );
        }
        for ( int j = 0; j < ring; ++j ) { // e.
            p.x += m_hori;
            pos.push_back ( p );
        }
        for ( int j = 0; j < ring; ++j ) { // ne.
            p.x += m_hori / 2; p.y -= m_vert;
            pos.push_back ( p );
        }
    }
    return pos;
}


[[ nodiscard ]] std::vector<std::pair<sf::Vector2f, App::uidx>> App::positionIdxData ( ) const noexcept {
    std::vector<std::pair<sf::Vector2f, uidx>> pos;
    pos.reserve ( state::size ( ) );
    sf::Vector2f p = m_center;
    hex ax { 0, 0 };
    pos.emplace_back ( p, state::hex_to_idx ( ax ) );
    for ( int ring = 1; ring <= int { state::radius ( ) }; ++ring ) {
        p.x += m_hori; // Move east.
        ++ax.q;
        for ( int j = 0; j < ring; ++j ) { // nw.
            p.x -= m_hori / 2; p.y -= m_vert;
            --ax.r;
            pos.emplace_back ( p, state::hex_to_idx ( ax ) );
        }
        for ( int j = 0; j < ring; ++j ) { // w.
            p.x -= m_hori;
            --ax.q;
            pos.emplace_back ( p, state::hex_to_idx ( ax ) );
        }
        for ( int j = 0; j < ring; ++j ) { // sw.
            p.x -= m_hori / 2; p.y += m_vert;
            --ax.q; ++ax.r;
            pos.emplace_back ( p, state::hex_to_idx ( ax ) );
        }
        for ( int j = 0; j < ring; ++j ) { // se.
            p.x += m_hori / 2; p.y += m_vert;
            ++ax.r;
            pos.emplace_back ( p, state::hex_to_idx ( ax ) );
        }
        for ( int j = 0; j < ring; ++j ) { // e.
            p.x += m_hori;
            ++ax.q;
            pos.emplace_back ( p, state::hex_to_idx ( ax ) );
        }
        for ( int j = 0; j < ring; ++j ) { // ne.
            p.x += m_hori / 2; p.y -= m_vert;
            ++ax.q; --ax.r;
            pos.emplace_back ( p, state::hex_to_idx ( ax ) );
        }
    }
    return pos;
}

void App::hexData ( ) noexcept {
    sf::Vector2f p = m_center;
    hex ax { 0, 0 };
    m_hex.at ( ax.q, ax.r ) = p;
    for ( int ring = 1; ring <= int { state::radius ( ) }; ++ring ) {
        p.x += m_hori; // Move east.
        ++ax.q;
        for ( int j = 0; j < ring; ++j ) { // nw.
            p.x -= m_hori / 2; p.y -= m_vert;
            m_hex.at ( ax.q, --ax.r ) = p;
        }
        for ( int j = 0; j < ring; ++j ) { // w.
            p.x -= m_hori;
            m_hex.at ( --ax.q, ax.r ) = p;
        }
        for ( int j = 0; j < ring; ++j ) { // sw.
            p.x -= m_hori / 2; p.y += m_vert;
            m_hex.at ( --ax.q, ++ax.r ) = p;
        }
        for ( int j = 0; j < ring; ++j ) { // se.
            p.x += m_hori / 2; p.y += m_vert;
            m_hex.at ( ax.q, ++ax.r ) = p;
        }
        for ( int j = 0; j < ring; ++j ) { // e.
            p.x += m_hori;
            m_hex.at ( ++ax.q, ax.r ) = p;
        }
        for ( int j = 0; j < ring; ++j ) { // ne.
            p.x += m_hori / 2; p.y -= m_vert;
            m_hex.at ( ++ax.q, --ax.r ) = p;
        }
    }
}


void App::loadVertexArray ( ) noexcept {

    m_vertices.setPrimitiveType ( sf::Quads );
    m_vertices.resize ( 4 * state::size ( ) );

    sf::Box<float> & tb = m_tex_box [ 0 ];

    struct quad {
        sf::Vertex v0, v1, v2, v3;
    };

    quad * quads = reinterpret_cast<quad*> ( & m_vertices [ 0 ] );

    int i = 0;

    sf::Vector2f p = m_center - sf::Vector2f { m_circle_radius, m_circle_radius };
    quads [ i ] = {
        sf::Vertex { sf::Vector2f { p.x, p.y }, sf::Vector2f { tb.left, tb.top } },
        sf::Vertex { sf::Vector2f { p.x + m_circle_diameter, p.y }, sf::Vector2f { tb.right, tb.top } },
        sf::Vertex { sf::Vector2f { p.x + m_circle_diameter, p.y + m_circle_diameter }, sf::Vector2f { tb.right, tb.bottom } },
        sf::Vertex { sf::Vector2f { p.x, p.y + m_circle_diameter }, sf::Vector2f { tb.left, tb.bottom } }
    };
    for ( int ring = 1; ring <= int { state::radius ( ) }; ++ring ) {
        p.x += m_hori; // Move east.
        for ( int j = 0; j < ring; ++j ) { // nw.
            p.x -= m_hori / 2; p.y -= m_vert;
            quads [ ++i ] = {
                sf::Vertex { sf::Vector2f { p.x, p.y }, sf::Vector2f { tb.left, tb.top } },
                sf::Vertex { sf::Vector2f { p.x + m_circle_diameter, p.y }, sf::Vector2f { tb.right, tb.top } },
                sf::Vertex { sf::Vector2f { p.x + m_circle_diameter, p.y + m_circle_diameter }, sf::Vector2f { tb.right, tb.bottom } },
                sf::Vertex { sf::Vector2f { p.x, p.y + m_circle_diameter }, sf::Vector2f { tb.left, tb.bottom } }
            };
        }
        for ( int j = 0; j < ring; ++j ) { // w.
            p.x -= m_hori;
            quads [ ++i ] = {
                sf::Vertex { sf::Vector2f { p.x, p.y }, sf::Vector2f { tb.left, tb.top } },
                sf::Vertex { sf::Vector2f { p.x + m_circle_diameter, p.y }, sf::Vector2f { tb.right, tb.top } },
                sf::Vertex { sf::Vector2f { p.x + m_circle_diameter, p.y + m_circle_diameter }, sf::Vector2f { tb.right, tb.bottom } },
                sf::Vertex { sf::Vector2f { p.x, p.y + m_circle_diameter }, sf::Vector2f { tb.left, tb.bottom } }
            };
        }
        for ( int j = 0; j < ring; ++j ) { // sw.
            p.x -= m_hori / 2; p.y += m_vert;
            quads [ ++i ] = {
                sf::Vertex { sf::Vector2f { p.x, p.y }, sf::Vector2f { tb.left, tb.top } },
                sf::Vertex { sf::Vector2f { p.x + m_circle_diameter, p.y }, sf::Vector2f { tb.right, tb.top } },
                sf::Vertex { sf::Vector2f { p.x + m_circle_diameter, p.y + m_circle_diameter }, sf::Vector2f { tb.right, tb.bottom } },
                sf::Vertex { sf::Vector2f { p.x, p.y + m_circle_diameter }, sf::Vector2f { tb.left, tb.bottom } }
            };
        }
        for ( int j = 0; j < ring; ++j ) { // se.
            p.x += m_hori / 2; p.y += m_vert;
            quads [ ++i ] = {
                sf::Vertex { sf::Vector2f { p.x, p.y }, sf::Vector2f { tb.left, tb.top } },
                sf::Vertex { sf::Vector2f { p.x + m_circle_diameter, p.y }, sf::Vector2f { tb.right, tb.top } },
                sf::Vertex { sf::Vector2f { p.x + m_circle_diameter, p.y + m_circle_diameter }, sf::Vector2f { tb.right, tb.bottom } },
                sf::Vertex { sf::Vector2f { p.x, p.y + m_circle_diameter }, sf::Vector2f { tb.left, tb.bottom } }
            };
        }
        for ( int j = 0; j < ring; ++j ) { // e.
            p.x += m_hori;
            quads [ ++i ] = {
                sf::Vertex { sf::Vector2f { p.x, p.y }, sf::Vector2f { tb.left, tb.top } },
                sf::Vertex { sf::Vector2f { p.x + m_circle_diameter, p.y }, sf::Vector2f { tb.right, tb.top } },
                sf::Vertex { sf::Vector2f { p.x + m_circle_diameter, p.y + m_circle_diameter }, sf::Vector2f { tb.right, tb.bottom } },
                sf::Vertex { sf::Vector2f { p.x, p.y + m_circle_diameter }, sf::Vector2f { tb.left, tb.bottom } }
            };
        }
        for ( int j = 0; j < ring; ++j ) { // ne.
            p.x += m_hori / 2; p.y -= m_vert;
            quads [ ++i ] = {
                sf::Vertex { sf::Vector2f { p.x, p.y }, sf::Vector2f { tb.left, tb.top } },
                sf::Vertex { sf::Vector2f { p.x + m_circle_diameter, p.y }, sf::Vector2f { tb.right, tb.top } },
                sf::Vertex { sf::Vector2f { p.x + m_circle_diameter, p.y + m_circle_diameter }, sf::Vector2f { tb.right, tb.bottom } },
                sf::Vertex { sf::Vector2f { p.x, p.y + m_circle_diameter }, sf::Vector2f { tb.left, tb.bottom } }
            };
        }
    }

    std::sort ( quads, quads + m_vertices.getVertexCount ( ) / 4,
        [ ] ( const auto & a, const auto & b ) {
            return ( a.v0.position.y < b.v0.position.y ) or ( a.v0.position.y == b.v0.position.y and a.v0.position.x < b.v0.position.x );
        }
    );
}


[[ nodiscard ]] bool App::playAreaContains ( sf::Vector2f p_ ) const noexcept {
    // http://www.playchilla.com/how-to-check-if-a-point-is-inside-a-hexagon
    static const float hori { state::width ( ) * 0.5f * m_vert }, vert { hori / 1.732050807568877193f }, vert_2 { 2.0f * vert }, hori_vert_2 { hori * vert_2 };
    p_ -= m_center;
    p_.x = std::abs ( p_.x ); p_.y = std::abs ( p_.y );
    // x- and y-coordinates swapped (for flat-topped hexagon).
    if ( p_.y > hori or p_.x > vert_2 )
        return false;
    return ( hori_vert_2 - vert * p_.y - hori * p_.x ) >= 0.0f;
}


float sqr ( const float x_ ) noexcept {
    return x_ * x_;
}

App::App ( ) {
    // Setup parameters.
    m_window_width = 701.0f;
    m_window_height = 647.0f;
    m_center = sf::Vector2f { m_window_width / 2.0f, m_window_height / 2.0f /*+ 12.0f*/ };
    m_hori = 74.0f; // Horizontal displacement.
    m_vert = 64.0f; // Vertical displacement.
    m_circle_diameter = 67.0f;
    m_circle_radius = std::floorf ( m_circle_diameter / 2.0f );
    m_circle_radius_squared = sqr ( m_circle_radius );
    m_settings.antialiasingLevel = 8u;
    // Create the m_window.
    m_window.create ( sf::VideoMode ( static_cast<std::uint32_t> ( m_window_width ), static_cast<std::uint32_t> ( m_window_height ) ), L"Mado", sf::Style::None, m_settings );
    m_window_bounds = sf::FloatRect { 0.0f, 0.0f, m_window_width, m_window_height };
    // Set icon.
    setIcon ( );
    // Create data structures.
    {
        std::vector<sf::Vector2f> tmp = positionData ( );
        m_kdtree.initialize ( std::begin ( tmp ), std::end ( tmp ) );
    }
    hexData ( );
    int i = 0;
    for ( const auto & p : m_kdtree ) {
        m_positions [ i ].where = p;
        if ( kdtree::is_valid ( p ) )
            m_indices [ i ] = pointToIdx ( p );
        ++i;
    }
    m_tex_box = std::array<sf::Box<float>, 6> {
        {
            { 0.0f, 0.0f, m_circle_diameter, m_circle_diameter }, { m_circle_diameter, 0.0f, 2.0f * m_circle_diameter, m_circle_diameter }, { 2.0f * m_circle_diameter, 0.0f, 3.0f * m_circle_diameter, m_circle_diameter },
            { 0.0f, m_circle_diameter, m_circle_diameter, 2.0f * m_circle_diameter }, { m_circle_diameter, m_circle_diameter, 2.0f * m_circle_diameter, 2.0f * m_circle_diameter }, { 2.0f * m_circle_diameter, m_circle_diameter, 3.0f * m_circle_diameter, 2.0f * m_circle_diameter }
        }
    };
    loadVertexArray ( );
    // Load fonts.
    sf::loadFromResource ( m_font_regular, __REGULAR_FONT__ );
    sf::loadFromResource ( m_font_bold, __BOLD_FONT__ );
    sf::loadFromResource ( m_font_mono, __MONO_FONT__ );
    sf::loadFromResource ( m_font_numbers, __NUMBERS_FONT__ );
    // Load play area graphics.
    sf::loadFromResource ( m_circles_texture, CIRCLES_LARGE );
    m_circles_texture.setSmooth ( true );
    m_circles.setTexture ( m_circles_texture );
    m_circles.setOrigin ( m_circle_radius, m_circle_radius );
    // Load taskbar graphics.
    sf::loadFromResource ( m_taskbar_texture, TASKBAR );
    m_taskbar_texture.setSmooth ( true );
    m_taskbar.setTexture ( m_taskbar_texture );
    m_taskbar.setOrigin ( static_cast<float> ( m_taskbar_texture.getSize ( ).x ), 0.0f );
    m_taskbar.setPosition ( m_window_width, 0.0f );
    m_taskbar_default = sf::IntRect { 0,  0, 135, 30 };
    m_taskbar_minimize = sf::IntRect { 0, 30, 135, 30 };
    m_taskbar_close = sf::IntRect { 0, 90, 135, 30 };
    m_minimize_bounds = sf::FloatRect { m_window_width - m_taskbar_texture.getSize ( ).x, 0.0f, m_taskbar_texture.getSize ( ).x / 3.0f, m_taskbar_texture.getSize ( ).y / 4.0f };
    m_close_bounds = sf::FloatRect { m_window_width - m_taskbar_texture.getSize ( ).x / 3.0f, 0.0f, m_taskbar_texture.getSize ( ).x / 3.0f, m_taskbar_texture.getSize ( ).y / 4.0f };
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
    m_player_to_move.what = display::in_active_green;
    m_player_to_move.where.x = m_hex.at ( -state::width ( ) / 2, 0 ).x;
    m_player_to_move.where.y = m_hex.at ( 0, -state::width ( ) / 2 ).y;
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
    // Clear.
    m_window.clear ( sf::Color { 10u, 10u, 10u, 255u } );
    // Draw play area.
    m_window.draw ( m_vertices, & m_circles_texture );
    m_animator.run ( );
    m_window.draw ( m_overlay );
    // Display window.
    m_window.display ( );
    return m_animator.size ( );
}


void App::updateWindow ( ) noexcept {
    // Clear.
    m_window.clear ( sf::Color { 10u, 10u, 10u, 255u } );
    // Draw taskbar.
    m_taskbar.setTextureRect ( m_display_close ? m_taskbar_close : m_display_minimize ? m_taskbar_minimize : m_taskbar_default );
    m_window.draw ( m_taskbar );
    // Draw play area.
    setQuadTex ( 30, 5 );
    m_window.draw ( m_vertices, & m_circles_texture );
    // Display window.
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
    if ( m_idx >= 0 )
        m_positions [ m_idx ].deactivate ( );
    std::ptrdiff_t oi = m_idx;
    m_idx = -1;
    if ( m_window_bounds.contains ( mouse_position ) ) {
        m_display_close = m_close_bounds.contains ( mouse_position );
        m_display_minimize = m_minimize_bounds.contains ( mouse_position );
        if ( not ( m_display_close or m_display_minimize ) ) {
            if ( playAreaContains ( mouse_position ) ) {
                const auto [ idx, distance ] = m_kdtree.nn_index_distance ( mouse_position );
                if ( distance < m_circle_radius_squared )
                    m_idx = idx, m_positions [ idx ].activate ( );
            }
        }
        else {
            if ( sf::Mouse::isButtonPressed ( sf::Mouse::Left ) ) {
                if ( m_display_close ) {
                    closeWindow ( );
                    return;
                }
                if ( m_display_minimize ) {
                    m_display_minimize = false;
                    m_minimize = true;
                    return;
                }
            }
        }
    }
    else {
        m_display_close = m_display_minimize = false;
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
