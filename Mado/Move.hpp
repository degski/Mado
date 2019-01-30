
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
#include <limits>

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>

#ifndef nl
#define nl '\n'
#endif

#include "Types.hpp"


template<std::size_t S>
struct Move {

    using uidx = uidx<S>;

    enum class Type : uidx { place = 0, slide = 1 };

    uidx from, to;

    Move ( ) noexcept :
        from { std::move ( std::numeric_limits<uidx>::max ( ) ) },
        to { std::move ( std::numeric_limits<uidx>::max ( ) ) } {
    }
    Move ( const uidx & to_ ) noexcept :
        from { std::move ( std::numeric_limits<uidx>::max ( ) ) },
        to { to_ } {
    }
    Move ( uidx && to_ ) noexcept :
        from { std::move ( std::numeric_limits<uidx>::max ( ) ) },
        to { std::move ( to_ ) } {
    }
    Move ( const uidx & from_, const uidx & to_ ) noexcept :
        from { from_ },
        to { to_ } {
    }
    Move ( uidx && from_, uidx && to_ ) noexcept :
        from { std::move ( from_ ) },
        to { std::move ( to_ ) } {
    }

    template<typename Stream>
    [[ maybe_unused ]] friend Stream & operator << ( Stream & out_, const Move & m_ ) noexcept {
        if ( std::numeric_limits<uidx>::max ( ) == m_.from )
            std::cout << '<' << static_cast<std::uint64_t> ( m_.to ) << '>';
        else
            std::cout << '<' << static_cast<std::uint64_t> ( m_.from ) << ' ' << static_cast<std::uint64_t> ( m_.to ) << '>';
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

template<std::size_t S>
using Moves = std::vector<Move<S>>;
