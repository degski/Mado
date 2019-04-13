
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

#include <sax/iostream.hpp>
#include <random>

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>

#include "Globals.hpp"
#include "Hexcontainer.hpp"


template<int R>
struct Player {

    private:

    using IdxType = typename Hex<R, true>::IdxType;

    public:

    enum class Type : IdxType { invalid = -2, agent = -1, vacant = 0, human = 1 }; // Keep numbering this way, vacant **has** to be 0.

    Type value = Type::vacant;

    Player ( ) noexcept {
    }
    Player ( const Type & p_ ) noexcept :
        value ( p_ ) {
    }
    Player ( Type && p_ ) noexcept :
        value ( std::move ( p_ ) ) {
    }

    [[ nodiscard ]] Type opponent ( ) const noexcept {
        return static_cast<Type> ( -static_cast<IdxType> ( value ) );
    }

    void next ( ) noexcept {
        value = opponent ( );
    }

    [[ nodiscard ]] Type get ( ) const noexcept {
        return value;
    }

    [[ nodiscard ]] IdxType as_index ( ) const noexcept {
        return static_cast<IdxType> ( value );
    }

    [[ nodiscard ]] IdxType as_01index ( ) const noexcept {
        return ( as_index ( ) + 1 ) / 2;
    }

    template<typename Rng>
    [[ nodiscard ]] static Type random ( ) noexcept {
        return Rng::bernoulli ( ) ? Type::agent : Type::human;
    }

    [[ nodiscard ]] bool valid ( ) const noexcept {
        return Type::invalid != value;
    }
    [[ nodiscard ]] bool invalid ( ) const noexcept {
        return Type::invalid == value;
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
