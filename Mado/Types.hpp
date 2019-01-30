
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

#include <limits>
#include <type_traits>


template<std::size_t S>
using rows_size = std::integral_constant<std::size_t, S + 2>;
template<std::size_t S>
using cols_size = std::integral_constant<std::size_t, S * 2 + 1>;
template<std::size_t S>
using data_size = std::integral_constant<std::size_t, rows_size<S>::value * cols_size<S>::value>;

template<std::size_t S>
using uidx = std::conditional_t<data_size<S>::value < std::numeric_limits<std::uint8_t>::max ( ), std::uint8_t, std::uint16_t>;
template<std::size_t S>
using sidx = std::conditional_t<data_size<S>::value < std::numeric_limits<std::int8_t >::max ( ), std::int8_t , std::int16_t >;
