
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

// #include <lz4frame.h>

#include <sax/singleton.hpp>

#include "Globals.hpp"
#include "Mado.hpp"
#include "Drawables.hpp"

#include "resource.h"

template<typename GameState, typename Position>
struct NextMove {

    enum State : int { select = 0, place, Move };

    void reset ( ) noexcept {
        m_from  = Position{ };
        m_to    = Position{ };
        m_state = State::select;
    }

    [[nodiscard]] Position const & from ( ) const noexcept { return m_from; }
    void from ( Position const f_ ) noexcept {
        m_from  = f_;
        m_to    = Position{ };
        m_state = State::Move;
    }

    [[nodiscard]] Position const & to ( ) const noexcept { return m_to; }
    void to ( Position const t_ ) noexcept {
        m_to    = t_;
        m_state = State::select;
    }

    [[nodiscard]] State state ( ) const noexcept { return m_state; }
    void state ( State const s_ ) noexcept { m_state = std::move ( s_ ); }

    template<typename Stream>
    [[maybe_unused]] friend Stream & operator<< ( Stream & out_, NextMove const & nm_ ) noexcept {
        switch ( static_cast<int> ( nm_.m_state ) ) {
            case 0: out_ << "select <" << nm_.m_from << nm_.m_to << ">"; break;
            case 1: out_ << "place  <" << nm_.m_from << nm_.m_to << ">"; break;
            case 2: out_ << "Move   <" << nm_.m_from << nm_.m_to << ">"; break;
        }
        return out_;
    }

    private:
    Position m_from, m_to;
    State m_state = State::select;
};

// Default- and move-constructible and move-assignable.
template<int R>
class AppImpl {

    using MadoState = Mado<R>;

    using IdxType  = typename MadoState::IdxType;
    using Hex      = typename MadoState::Hex;
    using PlayArea = PlayArea<MadoState>;
    using NextMove = NextMove<MadoState, Hex>;
    using Player   = typename MadoState::value_type;

    MadoState m_state;

    // sf::LZ4Dictionary m_dict;

    float m_hori, m_vert, m_window_width, m_window_height;
    sf::Vector2f m_center;

    sf::ContextSettings m_settings;
    sf::RenderWindow & m_window;
    sf::FloatRect m_window_bounds;

    sf::Font m_font_regular, m_font_bold, m_font_mono, m_font_numbers, m_font_dottie;

    bool m_is_running = true, m_minimize = false;

    MouseState m_mouse;

    Taskbar m_taskbar;
    GameClock m_game_clock;
    PlayArea m_play_area;

    NextMove m_human_move;

    sf::Music m_music;

    public:
    static constexpr int const radius = R;

    AppImpl ( );
    AppImpl ( AppImpl const & )        = delete;
    AppImpl ( AppImpl && a_ ) noexcept = delete;

    ~AppImpl ( ) noexcept { closeWindow ( ); }

    [[nodiscard]] AppImpl & operator= ( AppImpl const & a_ ) = delete;
    [[nodiscard]] AppImpl & operator= ( AppImpl && a_ ) noexcept = delete;

    private:
    [[nodiscard]] static inline float distance_squared ( sf::Vector2f const p1_, sf::Vector2f const p2_ ) noexcept {
        return ( ( p1_.x - p2_.x ) * ( p1_.x - p2_.x ) ) + ( ( p1_.y - p2_.y ) * ( p1_.y - p2_.y ) );
    }

    template<typename T>
    [[nodiscard]] inline T floorf ( float const x ) const noexcept {
        return static_cast<T> ( static_cast<int> ( x - std::numeric_limits<IdxType>::min ( ) ) +
                                std::numeric_limits<IdxType>::min ( ) );
    }

    [[nodiscard]] Hex pointToHex ( sf::Vector2f p_ ) const noexcept;
    [[nodiscard]] bool playAreaContains ( sf::Vector2f p_ ) const noexcept;

    void setIcon ( ) noexcept;

    public:
    [[nodiscard]] inline bool isWindowOpen ( ) const { return m_window.isOpen ( ); }

    [[nodiscard]] inline bool pollWindowEvent ( sf::Event & event_ ) { return m_window.pollEvent ( event_ ); }

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

    [[nodiscard]] inline bool isPaused ( ) const noexcept { return not( m_is_running ); }
    [[nodiscard]] inline bool isRunning ( ) const noexcept { return m_is_running; }

    sf::RectangleShape m_overlay;
    sf::Text m_overlay_text;
    sf::Texture m_name_texture;
    sf::Sprite m_sprite;

    void setupStartupAnimation ( ) noexcept;
    bool runStartupAnimation ( ) noexcept;
    void updateWindow ( ) noexcept;

    void mouseEvents ( const sf::Event & event_ );
};

template<int R>
[[nodiscard]] typename AppImpl<R>::Hex AppImpl<R>::pointToHex ( sf::Vector2f p_ ) const noexcept {
    using value_type = typename Hex::value_type;
    // https://www.redblobgames.com/grids/hexagons/#comment-1063818420
    static float const radius{ m_hori * 0.5773502588f };
    static sf::Vector2f const center{ m_center.x, m_center.y - radius };
    p_ -= center;
    p_.x /= m_hori;
    p_.y /= radius;
    Hex h;
    h.q = floorf<value_type> ( p_.y + p_.x );
    h.r = floorf<value_type> ( ( floorf<value_type> ( p_.y - p_.x ) + h.q ) * 0.3333333433f );
    h.q = floorf<value_type> ( ( floorf<value_type> ( 2.0f * p_.x + 1.0f ) + h.q ) * 0.3333333433f ) - h.r;
    return h;
}

template<int R>
[[nodiscard]] bool AppImpl<R>::playAreaContains ( sf::Vector2f p_ ) const noexcept {
    // http://www.playchilla.com/how-to-check-if-a-point-is-inside-a-hexagon
    static float const hori{ MadoState::Board::width ( ) * 0.5f * m_vert }, vert{ hori * 0.5773502588f }, vert_2{ 2.0f * vert },
        hori_vert_2{ hori * vert_2 };
    p_ -= m_center;
    p_.x = std::abs ( p_.x );
    p_.y = std::abs ( p_.y );
    // x- and y-coordinates swapped (for flat-topped hexagon).
    if ( p_.y > hori or p_.x > vert_2 )
        return false;
    return ( hori_vert_2 - vert * p_.y - hori * p_.x ) > 0.0f;
}

template<int R>
AppImpl<R>::AppImpl ( ) :
    // Setup parameters.
    m_state{ }, m_hori{ 74.0f }, m_vert{ 64.0f }, m_window_width{ MadoState::Board::width ( ) * m_hori + m_vert + 1.0f },
    m_window_height{ MadoState::Board::height ( ) * m_vert + m_vert + 1.0f + 12.0f }, m_center{ sf::Vector2f{
                                                                                          m_window_width * 0.5f,
                                                                                          m_window_height * 0.5f + 6.0f } },
    m_window{ Window::instance ( ) }, m_taskbar{ m_window_width },
    m_game_clock{ std::floorf ( ( m_window_width - ( MadoState::Board::radius ( ) + 1 ) * m_hori ) / 4 ),
                  m_window_width - std::floorf ( ( m_window_width - ( MadoState::Board::radius ( ) + 1 ) * m_hori ) / 4 ), 76.5f },
    m_play_area{ m_state, m_game_clock, m_center, m_hori, m_vert, 67.0f } {
    m_settings.antialiasingLevel = 8u;
    // Create the m_window.
    m_window.create (
        sf::VideoMode ( static_cast<std::uint32_t> ( m_window_width ), static_cast<std::uint32_t> ( m_window_height ) ), L"Mado",
        sf::Style::None, m_settings );
    m_window_bounds = sf::FloatRect{ 0.0f, 0.0f, m_window_width, m_window_height };
    // Set icon.
    setIcon ( );
    // Load fonts.
    sf::loadFromResource ( m_font_regular, __REGULAR_FONT__ );
    sf::loadFromResource ( m_font_bold, __BOLD_FONT__ );
    sf::loadFromResource ( m_font_mono, __MONO_FONT__ );
    sf::loadFromResource ( m_font_numbers, __NUMBERS_FONT__ );
    sf::loadFromResource ( m_font_dottie, __DOTTIE_FONT__ );
    // Load sound.
    // sf::loadFromResource ( m_music, MUSIC );
    // m_music.setVolume ( 10.0f );
    // m_music.setLoop ( true );
    sf::Music::TimeSpan loop;
    loop.offset = sf::seconds ( 3.0f );
    loop.length = sf::seconds ( 192.0f );
    m_music.setLoopPoints ( loop );
    // Load Name Image.
    sf::loadFromResource ( m_name_texture, NAME );
    m_name_texture.setSmooth ( true );
    m_sprite.setTexture ( m_name_texture );
    // Load dictionary.
    // m_dict.loadFromResource ( DICT );
    // Start.
    m_music.play ( );
    // Player to Move.
    // m_player_to_move.what = display::in_active_green;
    // Ge started.
    m_mouse.initialize ( m_window );
    Animator::instance ( ).reserve ( 32 );
    setupStartupAnimation ( );
    m_window.requestFocus ( );
}

template<int R>
void AppImpl<R>::setIcon ( ) noexcept {
    HICON hIcon = LoadIcon ( GetModuleHandle ( NULL ), MAKEINTRESOURCE ( IDI_ICON1 ) );
    if ( hIcon ) {
        SendMessage ( m_window.getSystemHandle ( ), WM_SETICON, ICON_BIG, ( LPARAM ) hIcon );
    }
}

template<int R>
void AppImpl<R>::setupStartupAnimation ( ) noexcept {
    m_overlay.setSize ( sf::Vector2f{ m_window_width, m_window_height } );
    // Text.
    sf::centreOrigin ( m_sprite );
    m_sprite.setScale ( sf::Vector2f{ 0.01f, 0.01f } );
    m_sprite.setPosition ( sf::Vector2f{ m_window_width / 2.0f, m_window_height * 0.90f } );
    m_sprite.setColor ( sf::Color{ 255u, 255u, 255u, 0u } );
    // Callbacks.
    auto update_overlay_alpha = [ & ] ( float const v ) {
        m_overlay.setFillColor ( sf::Color{ 10u, 10u, 10u, static_cast<sf::Uint8> ( v ) } );
    };
    auto update_overlay_text_alpha = [ & ] ( float const v ) {
        m_sprite.setColor ( sf::Color{ 255u, 255u, 255u, static_cast<sf::Uint8> ( v ) } );
    };
    auto update_overlay_text_position = [ & ] ( float const v ) {
        m_sprite.setPosition ( sf::Vector2f{ m_window_width / 2.0f, m_window_height * v } );
    };
    auto update_overlay_text_rotate = [ & ] ( float const v ) { m_sprite.setRotation ( v ); };
    auto update_overlay_text_scale  = [ & ] ( float const v ) { m_sprite.setScale ( v, v ); };
    // Start animation.
    Animator::instance ( ).emplace (
        LAMBDA_EASING_START_END_DURATION ( update_overlay_alpha, sf::easing::exponentialInEasing, 255.0f, 0.0f, 4'000 ) );
    Animator::instance ( ).emplace (
        LAMBDA_EASING_START_END_DURATION ( update_overlay_text_alpha, sf::easing::exponentialInEasing, 0.0f, 255.0f, 1'300 ) );
    Animator::instance ( ).emplace ( LAMBDA_EASING_START_END_DURATION_DELAY (
        update_overlay_text_alpha, sf::easing::exponentialInEasing, 255.0f, 0.0f, 1'050, 1'100 ) );
    Animator::instance ( ).emplace (
        LAMBDA_EASING_START_END_DURATION ( update_overlay_text_position, sf::easing::exponentialInEasing, 0.90f, 0.40f, 1'300 ) );
    Animator::instance ( ).emplace (
        LAMBDA_EASING_START_END_DURATION ( update_overlay_text_scale, sf::easing::exponentialInEasing, 0.01f, 1.0f, 1'300 ) );
    Animator::instance ( ).emplace (
        LAMBDA_EASING_START_END_DURATION ( update_overlay_text_rotate, sf::easing::exponentialInEasing, 0.0f, 360.0f, 1'300 ) );
    Animator::instance ( ).emplace ( LAMBDA_EASING_START_END_DURATION_DELAY (
        update_overlay_text_position, sf::easing::exponentialInEasing, 0.40f, 0.0f, 750, 1'500 ) );
}

template<int R>
bool AppImpl<R>::runStartupAnimation ( ) noexcept {
    Animator::instance ( ).run ( );
    m_window.clear ( sf::Color{ 10u, 10u, 10u, 255u } );
    m_window.draw ( m_play_area );
    m_window.draw ( m_overlay );
    m_window.draw ( m_sprite );
    m_window.display ( );
    return Animator::instance ( ).size ( );
}

template<int R>
void AppImpl<R>::updateWindow ( ) noexcept {
    m_play_area.update ( );
    m_window.clear ( sf::Color{ 10u, 10u, 10u, 255u } );
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

template<int R>
void AppImpl<R>::mouseEvents ( sf::Event const & event_ ) {
    // Update mouse state.
    sf::Vector2f const & mouse_position = m_mouse.update ( );
    if ( m_window_bounds.contains ( mouse_position ) ) {
        // In window.
        Hex const hex_position = pointToHex ( mouse_position );
        // if ( hex_position.valid ( ) ) {
        if ( Hex::is_valid ( hex_position.q, hex_position.r ) ) {
            if ( not( m_play_area.agent_is_making_move ) and sf::Mouse::isButtonPressed ( sf::Mouse::Left ) ) {
                // Selected a circle.
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
                    case NextMove::State::Move:
                        if ( ( no_reset =
                                   m_play_area.Move ( m_human_move.from ( ), hex_position, PlayArea::DisplayValue::active_red ) ) )
                            m_human_move.to ( hex_position );
                        break;
                }
                if ( not( no_reset ) )
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
                        if ( not( m_play_area.agent_is_making_move ) ) {
                            // Clicked the new area, after selecting where to Move from.
                            if ( NextMove::State::Move == m_human_move.state ( ) ) {
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

class Application {

    public:
    using AppType = std::variant<AppImpl<4>, AppImpl<5>, AppImpl<6>, AppImpl<7>, AppImpl<8>>;

    [[nodiscard]] inline bool isWindowOpen ( ) const {
        return std::visit ( [] ( auto const & inst ) noexcept { return inst.isWindowOpen ( ); }, m_instance );
    }

    [[nodiscard]] inline bool pollWindowEvent ( sf::Event & event_ ) {
        return std::visit ( [ &event_ ] ( auto & inst ) noexcept { return inst.pollWindowEvent ( event_ ); }, m_instance );
    }

    inline void closeWindow ( ) noexcept {
        return std::visit ( [] ( auto & inst ) noexcept { return inst.closeWindow ( ); }, m_instance );
    }

    inline void minimizeWindow ( ) noexcept {
        return std::visit ( [] ( auto & inst ) noexcept { return inst.minimizeWindow ( ); }, m_instance );
    }

    inline void pause ( ) noexcept {
        return std::visit ( [] ( auto & inst ) noexcept { return inst.pause ( ); }, m_instance );
    }

    inline void resume ( ) noexcept {
        return std::visit ( [] ( auto & inst ) noexcept { return inst.resume ( ); }, m_instance );
    }

    [[nodiscard]] inline bool isPaused ( ) const noexcept {
        return std::visit ( [] ( auto const & inst ) noexcept { return inst.isPaused ( ); }, m_instance );
    }
    [[nodiscard]] inline bool isRunning ( ) const noexcept {
        return std::visit ( [] ( auto const & inst ) noexcept { return inst.isRunning ( ); }, m_instance );
    }

    void setupStartupAnimation ( ) noexcept {
        return std::visit ( [] ( auto & inst ) noexcept { return inst.setupStartupAnimation ( ); }, m_instance );
    }
    bool runStartupAnimation ( ) noexcept {
        return std::visit ( [] ( auto & inst ) noexcept { return inst.runStartupAnimation ( ); }, m_instance );
    }
    void updateWindow ( ) noexcept {
        return std::visit ( [] ( auto & inst ) noexcept { return inst.updateWindow ( ); }, m_instance );
    }

    void mouseEvents ( sf::Event const & event_ ) {
        return std::visit ( [ &event_ ] ( auto & inst ) noexcept { return inst.mouseEvents ( event_ ); }, m_instance );
    }

    static void resize ( Application & a_, int const r_ ) noexcept {
        static sf::HrClock & clock{ Clock::instance ( ) };
        static DelayTimer dt ( 4 );
        if ( r_ == a_.m_radius or r_ < 4 or r_ > 8 )
            return;
        if ( dt.expired or dt.update ( clock.now ( ) ) ) {
            a_.m_radius = r_;
            switch ( r_ ) {
                case 4: a_.m_instance.emplace<AppImpl<4>> ( ); break;
                case 5: a_.m_instance.emplace<AppImpl<5>> ( ); break;
                case 6: a_.m_instance.emplace<AppImpl<6>> ( ); break;
                case 7: a_.m_instance.emplace<AppImpl<7>> ( ); break;
                case 8: a_.m_instance.emplace<AppImpl<8>> ( ); break;
            }
            dt.restart ( clock.now ( ) );
        }
    }

    private:
    AppType m_instance;
    int m_radius = std::visit ( [] ( auto & inst ) noexcept { return inst.radius; }, m_instance );
};

#define VISIT_0( F )                                                                                                               \
    switch ( radius ) {                                                                                                            \
        case 4: return reinterpret_cast<AppImpl<4> *> ( mem )->F ( );                                                              \
        case 5: return reinterpret_cast<AppImpl<5> *> ( mem )->F ( );                                                              \
        case 6: return reinterpret_cast<AppImpl<6> *> ( mem )->F ( );                                                              \
        case 7: return reinterpret_cast<AppImpl<7> *> ( mem )->F ( );                                                              \
        case 8: return reinterpret_cast<AppImpl<8> *> ( mem )->F ( );                                                              \
    }
#define VISIT_1( F, A )                                                                                                            \
    switch ( radius ) {                                                                                                            \
        case 4: return reinterpret_cast<AppImpl<4> *> ( mem )->F ( A );                                                            \
        case 5: return reinterpret_cast<AppImpl<5> *> ( mem )->F ( A );                                                            \
        case 6: return reinterpret_cast<AppImpl<6> *> ( mem )->F ( A );                                                            \
        case 7: return reinterpret_cast<AppImpl<7> *> ( mem )->F ( A );                                                            \
        case 8: return reinterpret_cast<AppImpl<8> *> ( mem )->F ( A );                                                            \
    }

struct A {

    alignas ( 4096 ) char mem[ sizeof ( AppImpl<8> ) ];
    int radius = 0;

    A ( int const r_ = 4 ) { construct ( r_ ); }
    ~A ( ) noexcept { destruct ( ); }

    [[nodiscard]] inline bool isWindowOpen ( ) { VISIT_0 ( isWindowOpen ) return false; }
    [[nodiscard]] inline bool pollWindowEvent ( sf::Event & event_ ) { VISIT_1 ( pollWindowEvent, event_ ) return false; }
    inline void closeWindow ( ) noexcept { VISIT_0 ( closeWindow ) }
    inline void minimizeWindow ( ) noexcept { VISIT_0 ( minimizeWindow ) }
    inline void pause ( ) noexcept { VISIT_0 ( pause ) }
    inline void resume ( ) noexcept { VISIT_0 ( resume ) }
    [[nodiscard]] inline bool isPaused ( ) noexcept { VISIT_0 ( isPaused ) return false; }
    [[nodiscard]] inline bool isRunning ( ) noexcept { VISIT_0 ( isRunning ) return false; }
    void setupStartupAnimation ( ) noexcept { VISIT_0 ( setupStartupAnimation ) }
    bool runStartupAnimation ( ) noexcept { VISIT_0 ( runStartupAnimation ) return false; }
    void updateWindow ( ) noexcept { VISIT_0 ( updateWindow ) }
    void mouseEvents ( sf::Event const & event_ ) { VISIT_1 ( mouseEvents, event_ ) }

    static void resize ( A & a_, int const r_ ) {
        static sf::HrClock & clock{ Clock::instance ( ) };
        static DelayTimer dt ( 4 );
        if ( r_ == a_.radius or r_ < 4 or r_ > 8 )
            return;
        if ( dt.expired or dt.update ( clock.now ( ) ) ) {
            a_.destruct ( );
            a_.construct ( r_ );
            dt.restart ( clock.now ( ) );
        }
    }

    private:
    void construct ( int const r_ ) {
        switch ( r_ ) {
            case 4: new ( mem ) AppImpl<4> ( ); break;
            case 5: new ( mem ) AppImpl<5> ( ); break;
            case 6: new ( mem ) AppImpl<6> ( ); break;
            case 7: new ( mem ) AppImpl<7> ( ); break;
            case 8: new ( mem ) AppImpl<8> ( ); break;
        }
        radius = r_;
    }

    void destruct ( ) noexcept {
        switch ( radius ) {
            case 4: reinterpret_cast<AppImpl<4> *> ( mem )->~AppImpl<4> ( ); break;
            case 5: reinterpret_cast<AppImpl<5> *> ( mem )->~AppImpl<5> ( ); break;
            case 6: reinterpret_cast<AppImpl<6> *> ( mem )->~AppImpl<6> ( ); break;
            case 7: reinterpret_cast<AppImpl<7> *> ( mem )->~AppImpl<7> ( ); break;
            case 8: reinterpret_cast<AppImpl<8> *> ( mem )->~AppImpl<8> ( ); break;
        }
        radius = 0;
    }
};

#undef VISIT_1
#undef VISIT_0
