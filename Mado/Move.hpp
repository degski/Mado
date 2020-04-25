
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
#include <cstring>

#include <sax/iostream.hpp>
#include <limits>
#include <type_traits>

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>

#include "../../compact_vector/include/compact_vector.hpp"

#include "Hexcontainer.hpp"

template<int R>
struct Move {

    using value_type = typename Hex<R, true>::IdxType;

    enum class Type : value_type { none = std::numeric_limits<value_type>::lowest ( ), place = 1, slide = 2 };

    value_type to, from;

    [[nodiscard]] bool operator< ( Move const & rhs_ ) const noexcept {
        return to < rhs_.to or ( to == rhs_.to and from < rhs_.from );
    }

    [[nodiscard]] bool operator== ( Move const & rhs_ ) const noexcept {
        using uint = std::conditional_t<sizeof ( value_type ) == 8, std::uint16_t,
                                        std::conditional_t<sizeof ( value_type ) == 16, std::uint32_t, std::uint64_t>>;
        uint l;
        std::memcpy ( std::addressof ( l ), this, sizeof ( uint ) );
        uint r;
        std::memcpy ( std::addressof ( r ), std::addressof ( rhs_ ), sizeof ( uint ) );
        return l == r;
    }
    [[nodiscard]] bool operator!= ( Move const & rhs_ ) const noexcept { return not operator== ( rhs_ ); }

    constexpr Move ( ) noexcept :
        to{ std::numeric_limits<value_type>::lowest ( ) }, from{ std::numeric_limits<value_type>::lowest ( ) } {}
    constexpr Move ( value_type const & to_ ) noexcept : to{ to_ }, from{ std::numeric_limits<value_type>::lowest ( ) } {}
    constexpr Move ( value_type && to_ ) noexcept : to{ std::move ( to_ ) }, from{ std::numeric_limits<value_type>::lowest ( ) } {}
    constexpr Move ( value_type const & from_, value_type const & to_ ) noexcept : to{ to_ }, from{ from_ } {}
    constexpr Move ( value_type && from_, value_type && to_ ) noexcept : to{ std::move ( to_ ) }, from{ std::move ( from_ ) } {}

    [[nodiscard]] constexpr bool is_placement ( ) const noexcept {
        return
            // std::numeric_limits<value_type>::lowest ( ) != to and
            std::numeric_limits<value_type>::lowest ( ) == from;
    }
    [[nodiscard]] constexpr bool is_slide ( ) const noexcept { return not is_placement ( ); }

    [[nodiscard]] constexpr bool is_invalid ( ) const noexcept { return std::numeric_limits<value_type>::lowest ( ) == to; }
    [[nodiscard]] constexpr bool is_valid ( ) const noexcept { return not is_invalid ( ); }

    void invalidate ( ) noexcept { to = std::numeric_limits<value_type>::lowest ( ); }

    template<typename Stream>
    [[maybe_unused]] friend Stream & operator<< ( Stream & out_, Move const & m_ ) noexcept {
        out_ << std::dec;
        if ( m_.is_invalid ( ) )
            if ( m_.is_placement ( ) )
                out_ << '<' << static_cast<std::uint64_t> ( m_.to ) << '>';
            else
                out_ << '<' << static_cast<std::uint64_t> ( m_.from ) << ' ' << static_cast<std::uint64_t> ( m_.to ) << '>';
        else
            out_ << "<*>";
        return out_;
    }

    private:
    friend class cereal::access;

    template<class Archive>
    void serialize ( Archive & ar_ ) {
        ar_ ( to, from );
    }
};

template<std::size_t R, std::size_t BoardSize>
// using Moves = std::vector<Move<R>>;
using Moves = sax::compact_vector<Move<R>, int, 2 * BoardSize, BoardSize / 2>;

// R Max Av. Moves
// 4     102
// 5     150
// 6     204
// 7     272
// 8     334

template<typename Moves>
void print_moves ( Moves const & moves_ ) noexcept {
    for ( auto const m : moves_ )
        std::cout << m;
    std::cout << nl;
}
