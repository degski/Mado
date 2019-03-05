
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

#include <filesystem>
#include <random>

#include <sax/prng.hpp>


// C++ global constants have static linkage. This is different from C. If you try to use a global
// constant in C++ in multiple files you get an unresolved external error. The compiler optimizes
// global constants out, leaving no space reserved for the variable. One way to resolve this error
// is to include the const initializations in a header file and include that header in your CPP files
// when necessary, just as if it was function prototype. Another possibility is to make the variable
// non-constant and use a constant reference when assessing it.


#if defined ( _DEBUG )
#define RANDOM 0
#else
#define RANDOM 1
#endif

struct Rng {

    static thread_local sax::Rng gen;

    static void seed ( const std::uint64_t s_ = 0u ) noexcept {
        Rng::gen.seed ( s_ ? s_ : sax::os_seed ( ) );
    }

    [[ nodiscard ]] static bool bernoulli ( ) noexcept {
        static std::bernoulli_distribution g_bernoulli_distribution;
        return g_bernoulli_distribution ( Rng::gen );
    }
};

thread_local sax::Rng Rng::gen ( RANDOM ? sax::os_seed ( ) : sax::fixed_seed ( ) );

#undef RANDOM


namespace fs = std::filesystem;

extern fs::path const & g_app_data_path;
extern fs::path const & g_app_path;

[[ nodiscard ]] std::int32_t getNumberOfProcessors ( ) noexcept;
