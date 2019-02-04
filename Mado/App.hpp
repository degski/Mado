
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

#include <array>
#include <vector>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Extensions.hpp>

#include <fsmlite/fsm.hpp>

#include "Types.hpp"
#include "Globals.hpp"
#include "Mado.hpp"
#include "Drawables.hpp"


#include "resource.h"



struct GameClock {




    sf::Clock m_agent_clock, m_human_clock;
};



template<typename GameState, typename Position>
struct NextMove {

    enum State : int { none = 0, place, move };

    using position = Position;

    void reset ( ) noexcept {
        std::memset ( this, 0, sizeof ( NextMove ) );
    }

    [[ nodiscard ]] const position & from ( ) const noexcept {
        return m_from;
    }
    void from ( const position & f_ ) noexcept {
        m_from = f_;
        m_state = State::move;
        sf::sleepForMilliseconds ( 150 ); // To deal with (in-voluntary) double-clicking, TODO: fix that in MouseState class.
    }

    [[ nodiscard ]] const position & to ( ) const noexcept {
        return m_to;
    }
    void to ( const position & t_ ) noexcept {
        m_to = t_;
        m_state = State::none;
        sf::sleepForMilliseconds ( 150 );
    }

    [[ nodiscard ]] State state ( ) const noexcept {
        return m_state;
    }
    void state ( const State & s_ ) noexcept {
        m_state = s_;
    }

    private:

    position m_from, m_to;
    State m_state = State::none;
};



class App {

    using MadoState = Mado<4>;
    using uidx = typename MadoState::uidx;
    using sidx = typename MadoState::sidx;
    using hex = typename MadoState::hex;
    using PlayArea = PlayArea<MadoState>;
    using NextMove = NextMove<MadoState, hex>;

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
    PlayArea m_play_area;
    NextMove m_human_move;

    sf::CallbackAnimator m_animator;
    sf::Music m_music;

    struct Fsm : public fsmlite::fsm<Fsm> {
        friend class fsm; // Base class needs access to transition_table.

        enum states { s_waiting, s_run, s_pause, };

        Fsm ( App & app_, state_type init_state = s_waiting ) :
            fsm ( init_state ),
            app { app_ } {
        }
        /*
        // Events.

        struct mouse_active { };

        // Actions.

        void handle_ ( const mouse_active & ) {
            app.runStartupAnimation ( );
        }


        using transition_table = table<
        //              Start    Event        Target   Action
        //  -----------+--------+------------+--------+------------------------+--
            mem_fn_row<s_waiting, left_mouse_click, s_run, & Fsm::run_startup>,


            //  -----------+--------+------------+--------+------------------------+--
        >;
        */
        App & app;
    };

    friend struct Fsm;

    public:

    App ( );

private:

    [[ nodiscard ]] static constexpr float distance_squared ( const sf::Vector2f & p1_, const sf::Vector2f & p2_ ) noexcept {
        return ( ( p1_.x - p2_.x ) * ( p1_.x - p2_.x ) ) + ( ( p1_.y - p2_.y ) * ( p1_.y - p2_.y ) );
    }

    [[ nodiscard ]] inline int int_floorf ( float x ) const noexcept {
        return static_cast<int> ( x + 4'194'304.0f ) - 4'194'304;
    }

    [[ nodiscard ]] uidx pointToIdx ( const sf::Vector2f & p ) const noexcept;
    [[ nodiscard ]] std::pair<hex, bool> pointToHex ( sf::Vector2f p_ ) const noexcept;
    [[ nodiscard ]] bool playAreaContains ( sf::Vector2f p_ ) const noexcept;

    [[ nodiscard ]] inline bool in_valid ( const sidx q_, const sidx r_ ) const noexcept {
        return std::abs ( q_ ) > MadoState::radius ( ) or std::abs ( r_ ) > MadoState::radius ( ) or std::abs ( -q_ - r_ ) > MadoState::radius ( );
    }
    [[ nodiscard ]] bool in_valid ( const hex h_ ) const noexcept {
        return in_valid ( h_.q, h_.r );
    }
    [[ nodiscard ]] bool valid ( const sidx q_, const sidx r_ ) const noexcept {
        return not ( in_valid ( q_, r_ ) );
    }
    [[ nodiscard ]] bool valid ( const hex h_ ) const noexcept {
        return not ( in_valid ( h_.q, h_.r ) );
    }

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
