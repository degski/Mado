
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
#include <limits>

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>

#include "Hexcontainer.hpp"

template<int R>
struct Move {

    using value_type = typename Hex<R, true>::IdxType;

    enum class Type : value_type { none = std::numeric_limits<value_type>::lowest ( ), place = 1, slide = 2 };

    value_type from, to;

    [[nodiscard]] bool operator< ( Move const & rhs_ ) const noexcept {
        return from < rhs_.from or ( from == rhs_.from and to < rhs_.to );
    }
    [[nodiscard]] bool operator== ( Move const & rhs_ ) const noexcept { return from == rhs_.from and to == rhs_.to; }
    [[nodiscard]] bool operator!= ( Move const & rhs_ ) const noexcept { return not operator== ( rhs_ ); }

    constexpr Move ( ) noexcept :
        from{ std::numeric_limits<value_type>::lowest ( ) }, to{ std::numeric_limits<value_type>::lowest ( ) } {}
    constexpr Move ( value_type const & to_ ) noexcept : from{ std::numeric_limits<value_type>::lowest ( ) }, to{ to_ } {}
    constexpr Move ( value_type && to_ ) noexcept : from{ std::numeric_limits<value_type>::lowest ( ) }, to{ std::move ( to_ ) } {}
    constexpr Move ( value_type const & from_, value_type const & to_ ) noexcept : from{ from_ }, to{ to_ } {}
    constexpr Move ( value_type && from_, value_type && to_ ) noexcept : from{ std::move ( from_ ) }, to{ std::move ( to_ ) } {}

    [[nodiscard]] constexpr bool is_placement ( ) const noexcept {
        return std::numeric_limits<value_type>::lowest ( ) != to and std::numeric_limits<value_type>::lowest ( ) == from;
    }
    [[nodiscard]] constexpr bool is_slide ( ) const noexcept {
        return std::numeric_limits<value_type>::lowest ( ) != to and std::numeric_limits<value_type>::lowest ( ) != from;
    }
    [[nodiscard]] constexpr bool is_valid ( ) const noexcept { return std::numeric_limits<value_type>::lowest ( ) != to; }
    [[nodiscard]] constexpr bool is_invalid ( ) const noexcept { return std::numeric_limits<value_type>::lowest ( ) == to; }

    void invalidate ( ) noexcept { to = std::numeric_limits<value_type>::lowest ( ); }

    template<typename Stream>
    [[maybe_unused]] friend Stream & operator<< ( Stream & out_, Move const & m_ ) noexcept {
        if ( std::numeric_limits<value_type>::lowest ( ) == m_.from )
            std::cout << std::dec << '<' << static_cast<std::uint64_t> ( m_.to ) << '>';
        else
            std::cout << std::dec << '<' << static_cast<std::uint64_t> ( m_.from ) << ' ' << static_cast<std::uint64_t> ( m_.to )
                      << '>';
        return out_;
    }

    private:
    friend class cereal::access;

    template<class Archive>
    void serialize ( Archive & ar_ ) {
        ar_ ( from );
        ar_ ( to );
    }
};

template<std::size_t R>
using Moves = std::vector<Move<R>>;

template<typename Moves>
void print_moves ( Moves const & moves_ ) noexcept {
    for ( auto const m : moves_ )
        std::cout << m;
    std::cout << nl;
}
