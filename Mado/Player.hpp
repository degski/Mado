
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

#include <iostream>
#include <random>

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>

#ifndef nl
#define nl '\n'
#endif

#include "Types.hpp"
#include "Globals.hpp"


template<std::size_t S>
struct Player {

    private:

    using underlying_t = std::int8_t;
    using sidx = sidx<S>;

    public:

    enum class Type : underlying_t { invalid = -2, agent = -1, vacant = 0, human = 1 }; // Keep numbering this way, vacant **has** to be 0.

    Type value = Type::invalid;

    Player ( ) noexcept {
    }
    Player ( const Type & p_ ) noexcept :
        value ( p_ ) {
    }
    Player ( Type && p_ ) noexcept :
        value ( std::move ( p_ ) ) {
    }

    [[ nodiscard ]] bool is_vacant ( ) const noexcept {
        return not ( static_cast<bool> ( value ) );
    }
    [[ nodiscard ]] bool is_not_vacant ( ) const noexcept {
        return static_cast<bool> ( value );
    }

    [[ nodiscard ]] Type opponent ( ) const noexcept {
        return static_cast<Type> ( -static_cast<underlying_t> ( value ) );
    }

    void next ( ) noexcept {
        value = opponent ( );
    }

    [[ nodiscard ]] Type get ( ) const noexcept {
        return value;
    }

    [[ nodiscard ]] sidx as_index ( ) const noexcept {
        return static_cast<sidx> ( value );
    }

    [[ nodiscard ]] sidx as_01index ( ) const noexcept {
        return ( as_index ( ) + 1 ) / 2;
    }

    template<typename Rng>
    [[ nodiscard ]] static Type random ( ) noexcept {
        return bernoulli ( ) ? Type::agent : Type::human;
    }

    [[ nodiscard ]] bool vacant ( ) const noexcept {
        return Type::vacant == value;
    }
    [[ nodiscard ]] bool occupied ( ) const noexcept {
        return static_cast<int> ( value ) & 1;
    }

    template<typename Stream>
    [[ maybe_unused ]] friend Stream & operator << ( Stream & out_, const Player & p_ ) noexcept {
        constexpr char name [ 4 ] { ' ', 'A', '*', 'H' };
        out_ << name [ static_cast<int> ( p_.value ) + 2 ];
        return out_;
    }

    [[ nodiscard ]] bool operator == ( const Player p_ ) const noexcept {
        return value == p_.value;
    }
    [[ nodiscard ]] bool operator != ( const Player p_ ) const noexcept {
        return value != p_.value;
    }

    private:

    friend class cereal::access;

    template<class Archive>
    void serialize ( Archive & ar_ ) {
        ar_ ( value );
    }
};
