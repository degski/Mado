
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
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include <filesystem>
#include <limits>
#include <random>

#include <SFML/Extensions.hpp>

#include <sax/prng_sfc.hpp>
#include <sax/singleton.hpp>
#include <sax/uniform_int_distribution.hpp>


// C++ global constants have static linkage. This is different from C.
// If you try to use a global constant in C++ in multiple files you get
// an unresolved external error. The compiler optimizes global constants
// out, leaving no space reserved for the variable. One way to resolve
// this error is to include the const initializations in a header file
// and include that header in your CPP files when necessary, just as if
// it was function prototype. Another possibility is to make the variable
// non-constant and use a constant reference when assessing it.


#if defined ( _DEBUG )
#define RANDOM 0
#else
#define RANDOM 1
#endif


struct Rng final {

    Rng ( Rng && ) = delete;
    Rng ( Rng const & ) = delete;

    Rng & operator = ( Rng && ) = delete;
    Rng & operator = ( Rng const & ) = delete;


    // A pareto-variate, the defaults give the 'famous' 80/20 distribution.
    template<typename T = float>
    [[ nodiscard ]] static T pareto_variate ( T const min_ = T { 1 }, T const alpha_ = { std::log ( T { 5 } ) / std::log ( T { 4 } ) } ) noexcept {
        assert ( min_ > T { 0 } ); assert ( alpha_ > T { 0 } );
        static std::uniform_real_distribution<T> dis ( std::numeric_limits<T>::min ( ), T { 1 } );
        return min_ / std::pow ( dis ( Rng::generator ( ) ), T { 1 } / alpha_ );
    }

    [[ nodiscard ]] static bool bernoulli ( double p_ = 0.5 ) noexcept {
        return std::bernoulli_distribution ( p_ ) ( Rng::generator ( ) );
    }

    static void seed ( std::uint64_t const s_ = 0u ) noexcept {
        Rng::generator ( ).seed ( s_ ? s_ : sax::os_seed ( ) );
    }

    [[nodiscard]] static sax::Rng& generator ( ) noexcept {
        if constexpr ( RANDOM ) {
            static thread_local sax::Rng generator ( sax::os_seed ( ), sax::os_seed ( ), sax::os_seed ( ), sax::os_seed ( ) );
            return generator;
        }
        else {
            static thread_local sax::Rng generator ( sax::fixed_seed ( ) );
            return generator;
        }
    }
};

#undef RANDOM


namespace fs = std::filesystem;

extern fs::path const & g_app_data_path;
extern fs::path const & g_app_path;

[[ nodiscard ]] inline std::uint32_t getNumberOfProcessors ( ) noexcept {
    return std::thread::hardware_concurrency ( );
}

using Animator = sax::singleton<sf::CallbackAnimator>;
