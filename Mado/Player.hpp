
// MIT License
//
// Copyright (c) 2019, 2020 degski
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
#include <string>
#include <random>
#include <utility>

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>

#include "Globals.hpp"
#include "Hexcontainer.hpp"

template<int R>
struct Player {

    private:
    using value_type = typename Hex<R, true>::IdxType;

    public:
    enum class Type : value_type {
        invalid = -2,
        agent   = -1,
        vacant  = 0,
        human   = 1
    }; // Keep numbering this way, vacant **has** to be 0.

    Type value = Type::vacant;

    Player ( ) noexcept                = default;
    Player ( Player const & ) noexcept = default;
    Player ( Player && ) noexcept      = default;

    [[maybe_unused]] Player & operator= ( Player const & ) noexcept = default;
    [[maybe_unused]] Player & operator= ( Player && ) noexcept = default;

    Player ( Type const & p_ ) noexcept : value ( p_ ) {}
    Player ( Type && p_ ) noexcept : value ( std::move ( p_ ) ) {}

    ~Player ( ) = default;

    [[nodiscard]] Type opponent ( ) const noexcept { return static_cast<Type> ( -static_cast<value_type> ( value ) ); }

    Type next ( ) noexcept { return std::exchange ( value, opponent ( ) ); }

    [[nodiscard]] Type get ( ) const noexcept { return value; }

    [[nodiscard]] value_type as_index ( ) const noexcept { return static_cast<value_type> ( value ); }

    [[nodiscard]] value_type as_01index ( ) const noexcept { return ( as_index ( ) + 1 ) / 2; }

    template<typename Rng>
    [[nodiscard]] static Type random ( ) noexcept {
        return Rng::bernoulli ( ) ? Type::agent : Type::human;
    }

    [[nodiscard]] bool valid ( ) const noexcept { return Type::invalid != value; }
    [[nodiscard]] bool invalid ( ) const noexcept { return Type::invalid == value; }
    [[nodiscard]] bool vacant ( ) const noexcept { return Type::vacant == value; }
    [[nodiscard]] bool occupied ( ) const noexcept { return static_cast<int> ( value ) & 1; }

    [[nodiscard]] bool agent ( ) const noexcept { return Type::agent == value; }
    [[nodiscard]] bool human ( ) const noexcept { return Type::human == value; }

    [[nodiscard]] std::string str ( ) const noexcept {
        constexpr char name[ 4 ]{ ' ', 'A', '*', 'H' };
        return { name[ static_cast<int> ( value ) + 2 ] };
    }

    template<typename Stream>
    [[maybe_unused]] friend Stream & operator<< ( Stream & out_, Player const & p_ ) noexcept {
        constexpr char name[ 4 ]{ ' ', 'A', '*', 'H' };
        out_ << name[ static_cast<int> ( p_.value ) + 2 ];
        return out_;
    }

    [[nodiscard]] bool operator== ( Player const p_ ) const noexcept { return value == p_.value; }
    [[nodiscard]] bool operator!= ( Player const p_ ) const noexcept { return value != p_.value; }

    private:
    friend class cereal::access;

    template<class Archive>
    void serialize ( Archive & ar_ ) {
        ar_ ( value );
    }
};
