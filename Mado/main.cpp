
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

#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include <algorithm>
#include <array>
#include <exception>
#include <filesystem>
#include <fstream>
#include <sax/iostream.hpp>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <random>
#include <string>
#include <type_traits>
#include <vector>

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Extensions.hpp>

#include <plf/plf_nanotimer.h>

// #include "../../KD-Tree/KD-Tree/sorted_vector_set.hpp"
// #include "../../MCTSSearchTree/include/flat_search_tree.hpp"



#include "App.hpp"


template<typename T, std::size_t R, bool zero_base = true>
class hb { };

template<typename T, bool zero_base>
class hb<T, 4, zero_base> {

    static constexpr int radius ( ) noexcept {
        return 4;
    }

    T r0 [ 5 ] { 0 };
    T r1 [ 6 ] { 0 };
    T r2 [ 7 ] { 0 };
    T r3 [ 8 ] { 0 };
    T r4 [ 9 ] { 0 };
    T r5 [ 8 ] { 0 };
    T r6 [ 7 ] { 0 };
    T r7 [ 6 ] { 0 };
    T r8 [ 5 ] { 0 };

    public:

    constexpr hb ( ) noexcept { }

    constexpr T const & at ( int q_, int r_ ) const noexcept {
        if constexpr ( zero_base ) {
            q_ += radius ( ); r_ += radius ( );
        }
        assert ( not ( is_invalid ( q_, r_ ) ) );
        switch ( r_ ) {
        case 0: return r0 [ q_ ];
        case 1: return r1 [ q_ ];
        case 2: return r2 [ q_ ];
        case 3: return r3 [ q_ ];
        case 4: return r4 [ q_ ];
        case 5: return r5 [ q_ ];
        case 6: return r6 [ q_ ];
        case 7: return r7 [ q_ ];
        case 8: [[ fallthrough ]];
        default: return r8 [ q_ ];
        }
    }

    constexpr T & at ( int q_, int r_ ) noexcept {
        return const_cast<T&> ( std::as_const ( *this ).at ( q_, r_ ) );
    }

    constexpr bool is_invalid ( const int q_, const int r_ ) const noexcept {
        if constexpr ( zero_base ) {
            return std::abs ( q_ ) > radius ( ) or std::abs ( r_ ) > radius ( ) or std::abs ( -q_ - r_ ) > radius ( );
        }
        else {
            return std::abs ( q_ - radius ( ) ) > radius ( ) or std::abs ( r_ - radius ( ) ) > radius ( ) or std::abs ( -q_ - r_ + ( 2 * radius ( ) ) ) > radius ( );
        }
    }
};


template<typename T, bool zero_base>
class hb<T, 5, zero_base> {

    static constexpr int radius ( ) noexcept {
        return 5;
    }

    T  r0 [  6 ] { 0 };
    T  r1 [  7 ] { 0 };
    T  r2 [  8 ] { 0 };
    T  r3 [  9 ] { 0 };
    T  r4 [ 10 ] { 0 };
    T  r5 [ 11 ] { 0 };
    T  r6 [ 10 ] { 0 };
    T  r7 [  9 ] { 0 };
    T  r8 [  8 ] { 0 };
    T  r9 [  7 ] { 0 };
    T r10 [  6 ] { 0 };

    public:

    constexpr hb ( ) noexcept { }

    constexpr T const & at ( int q_, int r_ ) const noexcept {
        if constexpr ( zero_base ) {
            q_ += radius ( ); r_ += radius ( );
        }
        assert ( not ( is_invalid ( q_, r_ ) ) );
        switch ( r_ ) {
        case 0: return r0 [ q_ ];
        case 1: return r1 [ q_ ];
        case 2: return r2 [ q_ ];
        case 3: return r3 [ q_ ];
        case 4: return r4 [ q_ ];
        case 5: return r5 [ q_ ];
        case 6: return r6 [ q_ ];
        case 7: return r7 [ q_ ];
        case 8: return r8 [ q_ ];
        case 9: return r9 [ q_ ];
        case 10: [[ fallthrough ]];
        default: return r10 [ q_ ];
        }
    }

    constexpr T & at ( int q_, int r_ ) noexcept {
        return const_cast< T& > ( std::as_const ( *this ).at ( q_, r_ ) );
    }

    constexpr bool is_invalid ( const int q_, const int r_ ) const noexcept {
        if constexpr ( zero_base ) {
            return std::abs ( q_ ) > radius ( ) or std::abs ( r_ ) > radius ( ) or std::abs ( -q_ - r_ ) > radius ( );
        }
        else {
            return std::abs ( q_ - radius ( ) ) > radius ( ) or std::abs ( r_ - radius ( ) ) > radius ( ) or std::abs ( -q_ - r_ + ( 2 * radius ( ) ) ) > radius ( );
        }
    }
};



template<typename T, bool zero_base>
class hb<T, 6, zero_base> {

    static constexpr int radius ( ) noexcept {
        return 6;
    }

    T  r0 [  7 ] { 0 };
    T  r1 [  8 ] { 0 };
    T  r2 [  9 ] { 0 };
    T  r3 [ 10 ] { 0 };
    T  r4 [ 11 ] { 0 };
    T  r5 [ 12 ] { 0 };
    T  r6 [ 13 ] { 0 };
    T  r7 [ 12 ] { 0 };
    T  r8 [ 11 ] { 0 };
    T  r9 [ 10 ] { 0 };
    T r10 [  9 ] { 0 };
    T r11 [  8 ] { 0 };
    T r12 [  7 ] { 0 };

    public:

    constexpr hb ( ) noexcept { }

    constexpr T const & at ( int q_, int r_ ) const noexcept {
        if constexpr ( zero_base ) {
            q_ += radius ( ); r_ += radius ( );
        }
        assert ( not ( is_invalid ( q_, r_ ) ) );
        switch ( r_ ) {
        case  0: return  r0 [ q_ ];
        case  1: return  r1 [ q_ ];
        case  2: return  r2 [ q_ ];
        case  3: return  r3 [ q_ ];
        case  4: return  r4 [ q_ ];
        case  5: return  r5 [ q_ ];
        case  6: return  r6 [ q_ ];
        case  7: return  r7 [ q_ ];
        case  8: return  r8 [ q_ ];
        case  9: return  r9 [ q_ ];
        case 10: return r10 [ q_ ];
        case 11: return r11 [ q_ ];
        case 12: [[ fallthrough ]];
        default: return r12 [ q_ ];
        }
    }

    constexpr T & at ( int q_, int r_ ) noexcept {
        return const_cast< T& > ( std::as_const ( *this ).at ( q_, r_ ) );
    }

    constexpr bool is_invalid ( const int q_, const int r_ ) const noexcept {
        if constexpr ( zero_base ) {
            return std::abs ( q_ ) > radius ( ) or std::abs ( r_ ) > radius ( ) or std::abs ( -q_ - r_ ) > radius ( );
        }
        else {
            return std::abs ( q_ - radius ( ) ) > radius ( ) or std::abs ( r_ - radius ( ) ) > radius ( ) or std::abs ( -q_ - r_ + ( 2 * radius ( ) ) ) > radius ( );
        }
    }
};


int main ( ) {

    hb<char, 4, false> h;

    h.at ( 0, 4 ) = char { 1 };

    std::cout << sizeof ( h ) << nl;

    return EXIT_SUCCESS;
}

namespace exp = std::experimental;



template<typename T, typename = std::enable_if_t<std::conjunction_v<std::is_integral<T>, std::is_unsigned<T>>>>
void print_bits ( const T n ) noexcept {
    int c = 0;
    T i = T ( 1 ) << ( sizeof ( T ) * 8 - 1 );
    while ( i ) {
        putchar ( int ( ( n & i ) > 0 ) + int ( 48 ) );
        if ( 0 == c or 8 == c ) {
            putchar ( 32 );
        }
        i >>= 1;
        ++c;
    }
}


struct float_as_bits {

    float_as_bits ( const float & v_ ) : value { v_ } { };
    float_as_bits ( float && v_ ) : value { std::move ( v_ ) } { };

    template<typename Stream>
    [[ maybe_unused ]] friend Stream & operator << ( Stream & out_, const float_as_bits & v_ ) noexcept {
        std::uint32_t v;
        std::memcpy ( &v, &v_.value, 4 );
        int c = 0;
        std::uint32_t i = std::uint32_t { 1 } << 31;
        while ( i ) {
            putchar ( int ( ( v & i ) > 0 ) + int ( 48 ) );
            if ( 0 == c or 8 == c ) {
                putchar ( 32 );
            }
            i >>= 1;
            ++c;
        }
        return out_;
    }

    float value;
};


struct State {

    enum DisplayValue : int { in_active_vacant = 0, in_active_red, in_active_green, active_vacant, active_red, active_green, selected_vacant, selected_red, selected_green };


    float state = 0.0f;
};


template<typename T, std::intptr_t I, std::intptr_t BaseI = 0, typename = std::enable_if_t<std::is_default_constructible<T>::value, T>>
class Vector {

    public:

    using size_type = std::intptr_t;
    using value_type = T;
    using pointer = T * ;
    using const_pointer = const T *;
    using reference = T & ;
    using const_reference = const T &;

    private:

    value_type m_data [ I ];

    [[ nodiscard ]] constexpr const_pointer base ( ) const noexcept {
        return m_data - BaseI;
    }

    public:

    constexpr Vector ( ) : m_data { T ( ) } { }

    [[ nodiscard ]] reference at ( const std::intptr_t i_ ) noexcept {
        assert ( i_ >= BaseI && i_ < I + BaseI );
        return base ( ) [ i_ ];
    }
    [[ nodiscard ]] const_reference at ( const std::intptr_t i_ ) const noexcept {
        assert ( i_ >= BaseI && i_ < I + BaseI );
        return base ( ) [ i_ ];
    }

    [[ nodiscard ]] pointer data ( ) noexcept {
        return m_data;
    }
    [[ nodiscard ]] const_pointer data ( ) const noexcept {
        return m_data;
    }
};

namespace fcv {

template<typename T, std::size_t S>
using vector = std::experimental::fixed_capacity_vector<T, S>;
}

namespace tcb {

namespace detail {

template <typename T, typename...>
struct vec_type_helper {
    using type = T;
};

template <typename... Args>
struct vec_type_helper<void, Args...> {
    using type = typename std::common_type<Args...>::type;
};

template <typename T, typename... Args>
using vec_type_helper_t = typename vec_type_helper<T, Args...>::type;

template <typename, typename...>
struct all_constructible_and_convertible
    : std::true_type { };

template <typename T, typename First, typename... Rest>
struct all_constructible_and_convertible<T, First, Rest...>
    : std::conditional<
    std::is_constructible<T, First>::value &&
    std::is_convertible<First, T>::value,
    all_constructible_and_convertible<T, Rest...>,
    std::false_type>::type { };

template <std::size_t S, typename T, typename... Args, typename std::enable_if<
    !std::is_trivially_copyable<T>::value, int>::type = 0>
    constexpr fcv::vector<T, S> make_vector_impl ( Args&&... args ) {
    fcv::vector<T, S> vec;
    using arr_t = int [ ];
    ( void ) arr_t {
        0, ( vec.emplace_back ( std::forward<Args> ( args ) ), 0 )...
    };
    return vec;
}

template <std::size_t S, typename T, typename... Args, typename std::enable_if<
    std::is_trivially_copyable<T>::value, int>::type = 0>
    constexpr fcv::vector<T, S> make_vector_impl ( Args&&... args ) {
    return fcv::vector<T, S>{std::forward<Args> ( args )...};
}

} // namespace detail


template <typename T = void, std::size_t S, typename... Args,
    typename V = detail::vec_type_helper_t<T, Args...>,
    typename std::enable_if<
    detail::all_constructible_and_convertible<V, Args...>::value, int>::type = 0>
    constexpr fcv::vector<T, S> make_vector ( Args&&... args ) {
    return detail::make_vector_impl<S, V> ( std::forward<Args> ( args )... );
}

} // namespace tcb


template <typename T>
struct instance_counter {

    instance_counter ( ) noexcept { ++icounter.num_construct; }
    instance_counter ( const instance_counter& ) noexcept { ++icounter.num_copy; }
    instance_counter ( instance_counter&& ) noexcept { ++icounter.num_move; }
    // Simulate both copy-assign and move-assign
    instance_counter& operator=( instance_counter ) noexcept {
        return * this;
    }
    ~instance_counter ( ) { ++icounter.num_destruct; }

    private:
    static struct counter {
        int num_construct = 0;
        int num_copy = 0;
        int num_move = 0;
        int num_destruct = 0;

        ~counter ( ) {
            std::printf ( "%i direct constructions\n", num_construct );
            std::printf ( "%i copies\n", num_copy );
            std::printf ( "%i moves\n", num_move );
            const int total_construct = num_construct + num_copy + num_move;
            std::printf ( "%i total constructions\n", total_construct );
            std::printf ( "%i destructions ", num_destruct );
            if ( total_construct == num_destruct ) {
                std::printf ( "(no leaks)\n" );
            }
            else {
                std::printf ( "WARNING: potential leak" );
            }
        }
    } icounter;
};

template <typename T>
typename instance_counter<T>::counter instance_counter<T>::icounter {};

template <typename T>
struct counted : T, private instance_counter<T> {
    using T::T;
};


template<typename T, std::size_t R, bool zero_base = false, typename SizeType = int, typename = std::enable_if_t<std::is_default_constructible_v<T>, T>>
struct HC3 {

    using size_type = SizeType;
    using value_type = T;
    using pointer = T * ;
    using const_pointer = const T *;
    using reference = T & ;
    using const_reference = const T &;
    using neighbors_type = std::experimental::fixed_capacity_vector<T, 6>;

    struct element {
        neighbors_type neighbors;
    };

    using indx_type = value_type [ 2 * R + 1 ] [ 2 * R + 1 ];
    using data_type = std::array<element, 1 + 3 * R * ( R + 1 )>;
    using play_type = std::array<value_type, 1 + 3 * R * ( R + 1 )>;

    [[ nodiscard ]] static constexpr size_type radius ( ) noexcept {
        return static_cast< std::size_t > ( R );
    }
    [[ nodiscard ]] static constexpr size_type width ( ) noexcept {
        return 2 * radius ( ) + 1;
    }
    [[ nodiscard ]] static constexpr size_type height ( ) noexcept {
        return 2 * radius ( ) + 1;
    }
    [[ nodiscard ]] static constexpr size_type size ( ) noexcept {
        return width ( ) * height ( );
    }

    [[ nodiscard ]] static constexpr value_type centre ( ) noexcept {
        if constexpr ( zero_base ) {
            return static_cast<value_type> ( 0 );
        }
        else {
            return static_cast<value_type> ( R );
        }
    }

    indx_type m_index;
    data_type m_data;
    play_type m_value;

    constexpr bool is_invalid ( const value_type q_, const value_type r_ ) const noexcept {
        if constexpr ( zero_base ) {
            return std::abs ( q_ ) > radius ( ) or std::abs ( r_ ) > radius ( ) or std::abs ( -q_ - r_ ) > radius ( );
        }
        else {
            return std::abs ( q_ - radius ( ) ) > radius ( ) or std::abs ( r_ - radius ( ) ) > radius ( ) or std::abs ( -q_ - r_ + ( 2 * radius ( ) ) ) > radius ( );
        }
    }

    constexpr void emplace_valid_neighbor ( const value_type i_, const value_type q_, const value_type r_ ) noexcept {
        if ( is_invalid ( q_, r_ ) )
            return;
        m_data [ i_ ].neighbors.emplace_back ( ati ( q_, r_ ) );
    }
    constexpr void emplace_neighbors ( const value_type q_, const value_type r_ ) noexcept {
        const value_type i = ati ( q_, r_ );
        emplace_valid_neighbor ( i, q_    , r_ - 1 );
        emplace_valid_neighbor ( i, q_ + 1, r_ - 1 );
        emplace_valid_neighbor ( i, q_ - 1, r_     );
        emplace_valid_neighbor ( i, q_ + 1, r_     );
        emplace_valid_neighbor ( i, q_ - 1, r_ + 1 );
        emplace_valid_neighbor ( i, q_    , r_ + 1 );
    }

    HC3 ( ) noexcept {
        // Construct indices.
        value_type index = 0;
        pointer ptr = & m_index [ 0 ] [ 0 ];
        for ( int skip = radius ( ); skip > 0; --skip ) {
            const pointer skip_end = ptr + skip;
            while ( ptr != skip_end )
                *ptr++ = -1;
            const pointer end = ptr + width ( ) - skip;
            while ( ptr != end )
                *ptr++ = index++;
        }
        for ( int skip = 0; skip <= radius ( ); ++skip ) {
            const pointer end = ptr - skip + width ( );
            while ( ptr != end )
                *ptr++ = index++;
            const pointer skip_end = ptr + skip;
            while ( ptr != skip_end )
                *ptr++ = -1;
        }
        // Construct neighbors.
        value_type q = centre ( ), r = centre ( );
        emplace_neighbors ( q, r );
        for ( int ring = 1; ring <= static_cast<int> ( radius ( ) ); ++ring ) {
            ++q; // move to next ring, east.
            for ( int j = 0; j < ring; ++j ) // nw.
                emplace_neighbors (   q, --r );
            for ( int j = 0; j < ring; ++j ) // w.
                emplace_neighbors ( --q,   r );
            for ( int j = 0; j < ring; ++j ) // sw.
                emplace_neighbors ( --q, ++r );
            for ( int j = 0; j < ring; ++j ) // se.
                emplace_neighbors (   q, ++r );
            for ( int j = 0; j < ring; ++j ) // e.
                emplace_neighbors ( ++q,   r );
            for ( int j = 0; j < ring; ++j ) // ne.
                emplace_neighbors ( ++q, --r );
        }
        // Clear play area.
        std::fill ( std::begin ( m_value ), std::end ( m_value ), value_type ( 0 ) );
    }


    [[ nodiscard ]] const_reference ati ( const size_type q_, const size_type r_ ) const noexcept {
        if constexpr ( zero_base ) { // Center at { 0, 0 }.
            return m_index [ r_ + radius ( ) ] [ q_ + std::max ( radius ( ), r_ ) ];
        }
        else { // Center at { radius, radius }.
            return m_index [ r_ ] [ q_ ];
        }
    }
    [[ nodiscard ]] reference ati ( const size_type q_, const size_type r_ ) noexcept {
        return const_cast<reference> ( std::as_const ( * this ).ati ( q_, r_ ) );
    }

    [[ nodiscard ]] const_reference at ( const size_type q_, const size_type r_ ) const noexcept {
        return m_value [ ati ( q_, r_ ) ];
    }
    [[ nodiscard ]] reference at ( const size_type q_, const size_type r_ ) noexcept {
        return const_cast<reference> ( std::as_const ( * this ).at ( q_, r_ ) );
    }

    [[ nodiscard ]] const_reference at ( const Hex<R> & h_ ) const noexcept {
        return at ( h_.q, h_.r );
    }
    [[ nodiscard ]] reference at ( const Hex<R> & h_ ) noexcept {
        return const_cast<reference> ( std::as_const ( * this ).at ( h_.q, h_.r ) );
    }

    [[ nodiscard ]] const_reference operator [ ] ( const Hex<R> & h_ ) const noexcept {
        return at ( h_ );
    }
    [[ nodiscard ]] reference operator [ ] ( const Hex<R> & h_ ) noexcept {
        return const_cast<reference> ( std::as_const ( * this ).operator [ ] ( h_ ) );
    }

    [[ nodiscard ]] constexpr const_pointer data ( ) const noexcept {
        return & m_value [ 0 ];
    }
    [[ nodiscard ]] constexpr pointer data ( ) noexcept {
        return const_cast<pointer> ( std::as_const ( * this ).data ( ) );
    }

    template<typename Stream>
    [[ maybe_unused ]] friend Stream & operator << ( Stream & out_, const HC3 & h_ ) noexcept {
        for ( int r = 0; r < height ( ); ++r ) {
            for ( int q = 0; q < width ( ); ++q ) {
                out_ << std::setw ( 3 ) << static_cast<int> ( h_.m_index [ r ] [ q ] );
            }
            out_ << nl;
        }
        std::cout << nl;
        for ( auto & vec : h_.m_data ) {
            for ( auto v : vec.neighbors ) {
                std::cout << std::setw ( 3 ) << static_cast<int> ( v );
            }
            std::cout << nl;
        }
        std::cout << nl;
        for ( const auto & v : h_.m_value )
            std::cout << std::setw ( 3 ) << static_cast<int> ( v );
        std::cout << nl;
        return out_;
    }
};


int main7686786 ( ) {

    HC3<char, 3, true> hc1;

    // std::cout << hc1 << nl;

    std::cout << ( int ) hc1.ati ( 0, -3 ) << nl;
    std::cout << ( int ) hc1.ati ( 1, -3 ) << nl;

    //hc1.at ( 0, -3 ) = 1;
    //hc1.at ( 1, -3 ) = 2;

    //std::cout << hc1 << nl;

    HC3<char, 3, false> hc2;

    // std::cout << hc2 << nl;

    std::cout << ( int ) hc2.ati ( 3, 0 ) << nl;
    std::cout << ( int ) hc2.ati ( 4, 0 ) << nl;

    // hc2.at ( 3, 0 ) = 1;
    // hc2.at ( 4, 0 ) = 2;
    // hc2.at ( 3, 3 ) = 18;

    // std::cout << hc2 << nl;

    return EXIT_SUCCESS;
}


void handleEptr ( std::exception_ptr eptr ) { // Passing by value is ok.
    try {
        if ( eptr )
            std::rethrow_exception ( eptr );
    }
    catch ( const std::exception & e ) {
        std::cout << "Caught exception \"" << e.what ( ) << "\"\n";
    }
}


[[ nodiscard ]] bool isEscapePressed ( const sf::Event & event_ ) noexcept {
    return sf::Event::KeyPressed == event_.type and sf::Keyboard::Escape == event_.key.code;
}


int main8078078 ( ) {
    std::exception_ptr eptr;
    try {
        std::unique_ptr<App> app_uptr = std::make_unique<App> ( );
        sf::Pacer keep ( 30 );
        sf::Event event;
        // Startup animation.
        while ( app_uptr->runStartupAnimation ( ) ) {
            while ( app_uptr->pollWindowEvent ( event ) ) {
                if ( isEscapePressed ( event ) ) {
                    app_uptr->closeWindow ( );
                    return EXIT_SUCCESS;
                }
            }
            keep.pace ( );
        }
        // Regular game loop.
        keep.reset ( 10 );
        while ( app_uptr->isWindowOpen ( ) ) {
            while ( app_uptr->pollWindowEvent ( event ) ) {
                if ( sf::Event::LostFocus == event.type ) {
                    app_uptr->pause ( );
                    break;
                }
                if ( sf::Event::GainedFocus == event.type ) {
                    app_uptr->resume ( );
                }
                if ( sf::Event::Closed == event.type or isEscapePressed ( event ) ) {
                    app_uptr->closeWindow ( );
                    return EXIT_SUCCESS;
                }
                app_uptr->mouseEvents ( event );
            }
            if ( app_uptr->isRunning ( ) ) {
                app_uptr->updateWindow ( );
                keep.pace ( );
            }
            else {
                sf::sleep ( sf::milliseconds ( 250 ) );
            }
        }
    }
    catch ( ... ) {
        eptr = std::current_exception ( ); // Capture.
    }
    handleEptr ( eptr );
    return EXIT_SUCCESS;
}



int main97979 ( ) {
    /*
    using board = Mado<9>;
    using move = typename board::move;

    board b;

    std::cout << b << nl;

    b.move_hash ( move { 104 } );
    b.move_hash ( move { 24 } );
    b.move_hash ( move { 104, 106 } );
    b.move_hash ( move { 28 } );
    b.move_hash ( move { 106, 104 } );
    b.move_hash ( move { 28, 26 } );
    */
    return EXIT_SUCCESS;
}

/* some (932) random hashes

0xfde23d0fb7a78033, 0xce802dbc60696dd2, 0xd49b74083f9d4d69, 0x370c5a7d12b0d58f,
0xe82bf57ae8b586ca, 0xfebd3ce41c098d8e, 0x2ac675a90629e5d9, 0xffba6c35e63b2b6a,
0xcf6ca22d97aac1c8, 0x33f0c72f5e1b152f, 0x8834db871e3fd62f, 0x27cab67b9d6b9463,
0x25570eec39033fc2, 0xf0f072fc08fa386f, 0xbf7d0db1570600dc, 0x27456ab4cdf36e44,
0xbf377f6aba7dd39c, 0xcee6a4edebcc0b28, 0x5193c0cd5c6c02c4, 0x6044914b58dd089a,
0x64d4bf12b94aedff, 0xf6bb1ac8a74f8fd7, 0xa7bfd7be28e32904, 0x5fe95cfe18464014,
0x5890ccfb753b878d, 0x40d318158f843719, 0xd9bfe314dd547866, 0x4d5628b5b17cced3,
0x69be0053456a7a07, 0x10a107d58a8a3267, 0x9ed7f12746746f26, 0xa0eaad811eb3988a,
0x5531105ba4038abf, 0x235400ad6094d000, 0x162b0c8a8e2250ad, 0x32e01e159e71b350,
0xe33b3ede120bb1c0, 0xe20dbbe903fea4d8, 0x5234f8b1db56ae1e, 0xe8f74cdac6d9a5df,
0x95704a5083f6a7bc, 0x96da139940c3aa22, 0x21174671201c5418, 0xd8ccdb41869ac610,
0x7a0de23d86d78784, 0x22f4f1c39ae7e8e0, 0x272142109c38a89d, 0xf86e174de0bfb550,
0x78b68983d0c5d6e7, 0xfb2c812255bef6e9, 0x6d3b6807a7784fa1, 0xcb01888f4f558791,
0xf60c9be7cb6bc7ad, 0x530ac260f739bc75, 0xd7644946ed195d41, 0xa427f97f3d3a9407,
0xc8a92f92d85e75b6, 0x76f750abd019064c, 0x904e7273a430ba26, 0x4d4a0217db783992,
0xda70a496b4f61676, 0x5e8f590d3ca1e2b4, 0x2e700e563b0f4845, 0xe7b64249e34c929c,
0x8c64a07c354908eb, 0xe9c1f6fb06b9ab01, 0xc723ce4f2f494afc, 0x35636eb522ea2f24,
0xb5b3e222685970f6, 0x2cb9de18df3f7bdd, 0x681532988937cb8a, 0x948c74634458c078,
0xc41f6c7c3eee5a54, 0xacc9f90baf500c62, 0xab5a4841fc857d10, 0x8b98782e3e21f0f0,
0x3e941cfa0d00dbf6, 0x60e2086b8e51a833, 0x18bf6b7156f44cda, 0x47a0de4cadc50338,
0xc8ee31a427866f11, 0x3041f924402c7b05, 0x9e8c2568198e0db8, 0x82356d1718ad6a83,
0xe310b98e53b3087d, 0xf150555d208ab9e9, 0x286972828c939903, 0xcd9adc36f5046916,
0x33ff6424426df46d, 0xaf4fa2e4642b0938, 0xdf09a6163c1bd17c, 0x95f5ef231a446f7e,
0x27dfd517bb6dd756, 0x5d1acfccdce799cd, 0x68eea93e34217d22, 0xdb5b01f2bfe27c30,
0xb5fd5d179d2a7311, 0xb84b3ad6fead2e1f, 0x6b90273a04bbb2c0, 0x590e3f4a08ba133a,
0x3db085c9d5932f9a, 0xa8fb8d29116f79be, 0x2997fd57bb5493eb, 0xa55e9ef4da3e848f,
0xcee73d02184d3c69, 0x735b5d788b62d5aa, 0x39b392f129d92f46, 0x3143abf724cc1e6b,
0xce726c71a13b5dd5, 0xc2b4d4f2b091b7d5, 0xaa5523b9bf713163, 0x5601c60dc92b0099,
0xe8b35d96d3a7d218, 0xeffdcf1a3e74bf21, 0x8414fbdc8555f919, 0x1093277c3c34be0d,
0x48db9e49a0676c8b, 0x94d0c2d5bd401c8d, 0x2533d5a4d421a387, 0x3fb82f4f3152bf2d,
0xcf336e0964aca9c3, 0x7972de2f9d113922, 0xc90474e89b5601a9, 0xa8773edc2312359b,
0x8886343d88a1b01d, 0xb40a032640e7a14d, 0xb0f70d0644698740, 0x713e397585ee4092,
0xb864e59c40562f7e, 0xc43f401280183f6d, 0x984e13803b2295d3, 0x3bef17b774939b6e,
0x6da39276b75dc5cc, 0x9616daddfca05789, 0x2d7dead304afe9d8, 0x9359863381233ef8,
0x3982a144e8e341f9, 0x81eb5270d0075687, 0x55136fcd83d5b30a, 0x15779d674f6a661f,
0xf1ec7266aea0bab8, 0xc8cf8ad60fbfe556, 0x94bd412d25ac9b7f, 0x561d9c4884370b9a,
0xaecd61638ebf240e, 0x2d7b062342b8a208, 0x3266ed32d4756c44, 0xd7e2e8edf2a0df9b,
0x48237e6c94690773, 0x55a011dfe1651b47, 0x2fc68fd889d278fd, 0x95dc5dac91357253,
0xf78e001ea5134614, 0xa0d0ff8e76eb700e, 0x34b7036be9f105b1, 0x10c91eb8a584b4b1,
0x38a5b2e56fba72ed, 0xf7e89f0ef0305897, 0x76edc5b11efc05bf, 0x5a8541a2b1dbfa04,
0x473d3f4599860ac2, 0x840393ead40c7dbb, 0xf39adfaf934bb56b, 0x7053bd16baed9b9a,
0x75130be44a100a2e, 0xaf54ab30e66d2929, 0x6a187702e579c814, 0x922d4d22cb3fdffc,
0x7536ae35c4e438fb, 0xf55294e8edf59a0f, 0x3638ecf1df450d36, 0x6d4b777141f2edbf,
0x2478a23ee4f0aa00, 0x75aaf139940ae5da, 0xa4d5cacb39ef7626, 0xa41b40ec2a4db358,
0x3d83df105f1d71fa, 0xf4d5d2c17b0b205d, 0x630d3a95b2471988, 0xa3e8151a087b14cb,
0xc91145a0e7831659, 0x9bb28ad71564faa0, 0x99a2c1eec1177558, 0x6ee1e5c9e362cedc,
0xa6b35ce050b968ad, 0xd8e4a785901198bd, 0x541cbc8df1493f73, 0x3b71acafdd28549d,
0xa8ef0e0ec38f06ef, 0x4cc7d42da23c4485, 0x29a8c6d233e8f76f, 0x947e27cd176b486d,
0x6a37b4c46796a316, 0x98a9104c4b75cee0, 0xa374cf3abce5d2cb, 0x34d3b979a95ac585,
0x93a53d19fc56a84b, 0x978c4061b87564c9, 0x20631db0abb3a23d, 0xa41d4b94637f3b88,
0xa56ccf5844f7f6e5, 0xdce5f953ea4dd800, 0x3bc49888781fe177, 0xa4460898e4b37a4f,
0xaada7a7b8a3c47c8, 0xb9dbb24a260e1312, 0x5fb247a3c52eed85, 0xb88cfa70f7602298,
0x951146c6ace01492, 0xb57f7e691be395a9, 0xc3e6412dc947b911, 0x1e0dc060842d2d25,
0xc0227f09f55c82d5, 0x48644c8622b74e68, 0x24deb7b3576fb39a, 0x8ac070fc0afc6d03,
0x2b45dbd5291bc973, 0xe81b84342ef3866f, 0xee958557a1c070f0, 0xbc8763f3a59f77ec,
0xb59ef7eb29306266, 0xd01daa80a220520a, 0x80e018d41784946b, 0x66be98d60c020673,
0x7d5488a4199320d7, 0x85efa6394b5519f9, 0x16a2cdc4e82e5fa8, 0xc811cac03ba8963b,
0x508d5861c8c3f1e5, 0xb563552391ad2996, 0x2d61483d438770a2, 0xc71b8497e4577a41,
0xeb311944285c033a, 0x2df1a35f60175320, 0xb2e01c332936e7fd, 0xfe577197f9dbb4a0,
0x45b96e2ecf9531f8, 0xe042f0bb78ce6f96, 0xde990708562cc4f3, 0xd90019b53e79976f,
0x3932a02065b9a328, 0x3489f608700d763a, 0xc9d42158d735c7b2, 0x4a96230f47ce6c7d,
0x37519e0a6d354bf9, 0xe04504e92c2e3245, 0x4aa499bef221d41f, 0xdbad71a0268856f0,
0xf20e7f242714a437, 0x4cbdc2a35f6b3575, 0xcf37ff1f67c9f59d, 0x5ecfb0a7b79a90bf,
0x52513c1d7a1f3ce8, 0x821dbc2fb348575c, 0xf3106f56afcb3136, 0x59fda8b62e96227f,
0xdd4a0bcdfbf550d8, 0x2dd8c9bcb9add17e, 0xfd8b2d552b8526c7, 0xcf682a46714627ec,
0x2f07b76d380dfe18, 0x8f86e38814ad8789, 0x218a7b9ffd63b1f5, 0x9812ffc6b41ec03d,
0x3c7f94e9956e06ca, 0x42391b24e76c7113, 0x2d2ecca5d0e2a253, 0x22355cf2546feb73,
0xa416c551ad28c23d, 0xd8c395d79fd597bb, 0x1a575e04c3a2975b, 0x1ba9f38e3d23e65c,
0xd466880423f18a4e, 0x5fce801bd1c73d16, 0x50e963379937707f, 0xa3e219e22b54104a,
0x973ec3fcf385bbec, 0xfc4911ca2ee55ea8, 0x20b8a0f0b4fd165d, 0x9cefa4f26f22402d,
0xff1be5bf4c953bb5, 0xbd4e606e9d294601, 0x62ee4ae1f4ef175a, 0x33447fba65b58fe5,
0x65ac92c12163b0c5, 0xf442613de27cfe85, 0x54a5daa6a1b7371f, 0x34e22204f4936af3,
0x3ad3d98ee9d675ad, 0x84cb40d9bf568300, 0xeb19b594ca057054, 0x6d5e142249655a5e,
0x7753e3e39457620d, 0xab3b45002eae9aa2, 0xec2166f474ceccfe, 0xf0157c655f1fadb0,
0xfd97466d367542f1, 0x835ed623a1943430, 0xffd81c18d7d22614, 0x3c0682ee27e4f5a0,
0xfae01a35003f3ec8, 0xc545bc71aa90b95e, 0x85830f7d5eebaa00, 0x29ec70e64a3b0cad,
0x4c2a5b151ff9e14d, 0x6f076e70137ca068, 0x24a718202d3ace23, 0x1868eea6aa990a5c,
0x668df2c574823d14, 0x8ebde4c9cc5fc0eb, 0x85342b2a529fe3ed, 0x1102fee1f72fb05d,
0xf122e57fd344eda1, 0x19efb1fa8b023990, 0x9e449936d62a80e2, 0x83334a93628e9946,
0xa2ce9526e68fe333, 0xc7beb720474da445, 0xbb7e8dd297ddc433, 0x6db7629846fe3519,
0xb1b49016ae3d899f, 0xb13ddbbc0a29a58f, 0x6f6391dcbcc2060a, 0xd5e67570d7fec062,
0x9903982711ab92df, 0x23910fecaca30af4, 0x288dd7276be0ad94, 0xdc557d8e7c38a3cf,
0x3132684aa1a57cc7, 0x2f91bea8806e3420, 0xeb50f832a6a8bad1, 0xbe3e0c70db1ac0b3,
0x9496ff52d028a5da, 0x1ed58f7bb724831b, 0x4866ad17026c3270, 0x732df30d812114da,
0xc61d3196735fdffb, 0x191f33551d1b7128, 0xb96aa7c6fa0877e0, 0x6552c3d62b8ebf72,
0xc80022a53413bd1d, 0x3098cb878551901f, 0x53ee42c7f6168195, 0xa4999f8914f7a019,
0xdbe1cb88e634b483, 0xefcc750a66053380, 0x6c1af35d27217dfe, 0xb075e710207f5e99,
0xb7cd1b7892d28b2d, 0x61796823917927ea, 0x4d593abd76e07747, 0x764abf77a5d3de87,
0xf604aa1427122449, 0x4b6cd71ccdd23430, 0xf7abbe8716a590fc, 0x9910b78d1c8229c2,
0xe820b975d1987f31, 0x78e2d66923a91710, 0xf54e56cea32d03b4, 0x4f477eb5a5a90714,
0x64e73697507b6303, 0xea7116ebc2efe1eb, 0xfec298d86c00a2f9, 0xde95a3b74ef1120f,
0x1069525194d2d890, 0x7ce9c4fdc2d1e264, 0xeac173b35f28a42d, 0xc90f85bb94533bf2,
0x9807a508efb138b7, 0xdfb0544e4b279f80, 0xd0dc370b35866b69, 0x1e9c3890ce3f5c35,
0x3d70ce2fbc19f568, 0xde04deb2f72579cb, 0xa41ec457a90c78f5, 0x208290bfb1686cf8,
0x4af70a556477db97, 0x765d174f71095d85, 0xc2acc2b6f47da4cd, 0x24606a537b255706,
0x7c398bf264aaa09b, 0x1fc544ca2a20495c, 0x766df0d454daf44e, 0xe0948a252b350ee0,
0x5be91f742a764192, 0x53c145506dd4d32e, 0xdfa36c411bc9b68a, 0x1c9a9dd31410b098,
0xa6930e0e09af9e50, 0x19904e555eed17af, 0xda8a14a01d99d5de, 0x16b80b34b6828d21,
0x214203b7299cb583, 0x6e86355bca239d9e, 0xb1366c41c8cbfbf9, 0xe05d33332dbec515,
0xad1a2887e7d31380, 0x17b8a3108eaf6eb9, 0x30f07df4ed85160f, 0xab168ac2a5e78ed8,
0x7089d8b08adaee0c, 0x35c8228e6e1ccf39, 0x815e90549e5f73d2, 0x4a7a4d9fa6407796,
0x662a5a1e79abd247, 0x8b08d1dd8ecda2c0, 0xedf028f1ae029159, 0xb1644b2f6e71ade2,
0x470c39486a032808, 0x4711252bccb41458, 0xf298f4671e112bbc, 0x68f354c7e319f70e,
0x28fc9a2c91e58e3e, 0x2b4f9563f4288a66, 0x2217f84a53d6f03a, 0xc2ce9acc5cd076cb,
0xfcfed5910e45bb42, 0x72dfdc8545bd7bc7, 0x918342e13f49b01b, 0x7eca52bca25eeb32,
0x576bc5205e74a7ed, 0x6694fa390880afd5, 0x3b076ac21359c0cf, 0x332f151ded95f2bf,
0x9198aec69d5b5c9d, 0x30b83cdd17d1ad93, 0x850744ba3ba4ed5a, 0x5c43a0a9548e3ffe,
0xc574fe14b8f1de95, 0x697045734b6b3e66, 0xee4d4a71aed83c62, 0xe14f7ca47804dba1,
0x9887e62338c5c27a, 0x6a3048b6572b6882, 0x12aec8a26d112b98, 0x28d3bebf73bf6c21,
0xe62a8ceb002e2be0, 0x942a6f58ab0ad181, 0x937f5080e8942c47, 0xf7da259099fb1da4,
0x4ca810cd3c8dd58b, 0x84471f745c5df5cf, 0x999fff2860551789, 0x23ba481e84639407,
0x4d3e96b3cb0d51cd, 0xb468fe3c98e7c0b0, 0x41c53fe2068b1ded, 0xb3682fd6b9b6d7d2,
0x6d09c0598794e5c9, 0x5fad65f6a39e545d, 0xa9399bf28bf4be3c, 0x3421842e669e183f,
0x767615bea374e57f, 0x10df0179c1745aef, 0xb24f68e56470ef52, 0x8238762045436775,
0xeb463a41296311cd, 0xb5539ed16934d534, 0x60e56c1418764a61, 0xdee9f30dee8649ce,
0xab256a21bb18a0b6, 0xa8b9f07b495ac5e6, 0xb756a384c18414fa, 0x7cdfe7aef67e5d8d,
0x371f446e00332bb5, 0xf533404906e894c1, 0xfe1ecf8d5a2dce30, 0x5ab915da18c92677,
0xff70dc29742bedda, 0xdc1f25888796f808, 0xe8d9945b23558eb6, 0xcf20b588e7ff54e3,
0xe7430655ff6784f8, 0x5db9b013ee488b0a, 0x6fbbc56372895753, 0x5711463c1f949e5c,
0xf7e2200ffa69c251, 0x6c1eee766a44235e, 0xb914cdb15f72d797, 0x1218e4450588648d,
0xc398d4400b0134e0, 0x5a87a899534d81fb, 0x386f67d4ea2fdedb, 0xd65235684b6a5aa3,
0xd2e06a61d377bb9d, 0x5e4f63fa75833da2, 0x95e628b6e4b38242, 0x5a219940a3d50773,
0x2b74d463b85104f8, 0x254c9fe673a173bb, 0xffe84a1a3f9a2292, 0x909f2fed66f320ce,
0xddefbf96f8e19be5, 0xb10567f2f6fe5607, 0x9aece12265eb3698, 0xb642e4bf893eddcf,
0x1661f985ce9f9cd3, 0x4cc02c51f3a18f03, 0x7582a18d566afe2d, 0xe014acf9e93d00e6,
0x8892b7e24b1538c5, 0xd397ca6872f19e6d, 0x43b51cd07b8b22f6, 0xa71f917542aa8625,
0x2ed77bf5c91a6490, 0x3ea6cbca9fbd46e1, 0xf81ba7c532316352, 0x10d875c1907f79bc,
0x2064160149ed7ab1, 0xc7c87585a9f42626, 0xb34348468962f2f4, 0xcfc5fd09725ed854,
0xfae98127a8453696, 0x48f6c0c1e5b2c841, 0x244c61074b5ab5c2, 0x6c698c9d1cb5ab81,
0x70918314ac07ce75, 0x36fb32bfa1527192, 0x7806a8732e7945da, 0xafb4e57ed1b2a970,
0x72785805ae5918c2, 0x917f501f4720fc08, 0xdb6ab32e89ef83cb, 0xcf6b1e402170afd5,
0x126b0b221c8b3ec8, 0xbc8a8a7517d36de1, 0x93a5fa5333a8d621, 0x41a768c616583d69,
0xa7b8c17e3ea75d41, 0xc8772586c80d29eb, 0x24fbdaee4263d583, 0x2b7481ed8e63ed1a,
0x748c65191e8b6d5c, 0x68a7b1962aa542da, 0x6203547e2ec6fb51, 0x613c117eb2f86996,
0x1682f918ea840a34, 0x2f8cfb053db45c63, 0x44cc01cd26ba0c1e, 0xcf5012ef66ea49aa,
0xf4aaa63aefa5b601, 0x7a3cd8091c5b252f, 0xdc9a00e1ebda756d, 0xafd10166a4100a3c,
0x59f17d405a5748c4, 0xd342824b320cdd9f, 0xde686d7d86e503d3, 0x44349b87ec7d7527,
0x6d20bcf625609dd7, 0xd3686a42cdbc37ea, 0x8425aff1a5fbd0bc, 0xcd679ee854369bea,
0x676a1d7032ac0a53, 0xd2320fbd6ac4501a, 0xb38b37dfd458f286, 0xa888cf943032df11,
0x2c295f3640d27ff1, 0xfc9fec73c836e4b5, 0xec5a8135dd7c5514, 0xb9b9be5d2dbb4db7,
0x4fe9b5de38a9621f, 0x2895f9700c30b627, 0x27ef947f17250d71, 0x8fdee5b59bf62a32,
0x2643040e1b8138f0, 0x235a819df03b68c3, 0xb0db21cbe3ace14e, 0x9cae0e6da75deb31,
0xdfddfd9d6202960e, 0x656522a10a541645, 0x9f92b51d6e6c58a8, 0xd716eacef77b8f01,
0x139862125e070350, 0x404b3ba3eaf6d85e, 0x4a0890d0a548e449, 0x903e7e3823885db2,
0xc8c8854f5cdcb82d, 0xe071b908826df495, 0x53638cb1af68211c, 0x5f8f52dab13eb751,
0x518b1434b90b0b35, 0xd82b55d898be6a60, 0xdcc620528d6125ab, 0x89f81b3f33b2efd8,
0x313b9c06923667e8, 0x1f0ab42314f52abb, 0xdb94a79abc5f1e59, 0x2f1c82b8782c7912,
0xc5de7db15f411d47, 0x4dbac0beb1c0d3ab, 0xcaf0456639e95376, 0x256708d666ac3ee6,
0xfefa919889d37e97, 0x5ed0589b5a62aa53, 0x2002765d7a79b4b3, 0xe21af890bd2845d9,
0xcf82f6de943f36a5, 0x93cb2c4c8b23c610, 0x4cded8427fa3a620, 0x62cea8293ac63f0e,
0x177cfcc4b01d0c4d, 0xd0817326bddabfc8, 0xf883583a41a2d33c, 0x28d37eaced6f282d,
0x421c7c736852e31d, 0x9e24f4c0a0f80781, 0x2976ca5a7f71fef8, 0xf458594dabd3dfdb,
0x459c4c2d9c8e0b44, 0xf5086e2c62231f37, 0xc117ea38cba17f09, 0xd877d10cc2a26eef,
0xcb6b70e63a18bffd, 0x637c9b7df894ea71, 0x31efdf0c72a0f1bb, 0x4366e8abe71d97c2,
0x69c412cf8019e8c4, 0x8b0410b19bf32080, 0x10598d10be4feaca, 0xbdbd9bbc76d89dad,
0x2049839c1587c132, 0x209d3f0787d7c41b, 0xc3076266958c4e73, 0x41b0e95dfa566376,
0xe20a500e3d9dba49, 0x47664ed2d8564a68, 0xd9ffd83b2ed917eb, 0x5d62dd501adbf016,
0x31c0e027c312469e, 0x58e37fb1d3ab6abc, 0x7e651a2ba372621c, 0xfacd38249273a6d7,
0xf704f2bb32e55b0f, 0x15d619aa5f51508c, 0xcca1723dda5bb07b, 0x2062910d7c6db496,
0xaa149ff655a43db6, 0xf9e90347d626c5ad, 0x1c3ae03fb1151de5, 0x9d4f63319c368dec,
0x5293c48a3c23131b, 0x1e7ee9fcf4233c19, 0x425cf209cd69ad4a, 0x61930b5364b8f7b4,
0x29e1a6772d3bfe15, 0x9cb0c93cc673418d, 0x8983b8d60bf34101, 0xa6aa973571033737,
0x8f96316b089f9b4d, 0xf7d34fe68629d322, 0x6d92807d1f7d2819, 0x60cfeb7daf9035a4,
0xf46681d1e253a15d, 0x1fc2b64a3113bbe0, 0x30960407641d2a37, 0x9ad95453ba7cfbfd,
0x678e71ca02f9c608, 0xc39a38b454a007b2, 0xfd223c70d34f1db8, 0x19b79269fd0bde30,
0x69cc18835741991b, 0x43311d79f6a1b0b1, 0xc12fb32381c8b852, 0x2d90e8910851a99f,
0x105d558eb430d051, 0x87a444e7eff75a1e, 0x93936f9f43e416f3, 0x12662915b26d7e25,
0x1c07a6bc17f4973c, 0x3d2724cb4fd9cc14, 0xf6d6fc22b95acb93, 0xed2b901eada05dac,
0x853bd296ff91f095, 0xf11bf2e3409197f9, 0xd65bcbcc745ae259, 0xb4cc6a1be8f16f68,
0x824080ecb5b1d6c8, 0xe1da5f5d6b691629, 0xeb89f6e21f5ce6ac, 0xea797e9a28385025,
0xc2765f6b16256164, 0x3b8f5153e3b4bd02, 0xbcbe7d744b9e13b7, 0x26738fbc65177827,
0xef3e724a625e5646, 0x5ef6746f7ea1b453, 0x556ae349bed29f3e, 0xafd56949614ce269,
0x6275df8e0978592f, 0x39cb884b191237e7, 0x7fc1ef4855ca0e5d, 0xdb733bfbba27bbd2,
0xd5780d4a42067977, 0xf1e6190223f072b8, 0x3418720ffa62eefa, 0x2e1afcf0cca20eb1,
0xd89e1e1bbcc98711, 0x602980a3fab0a6e3, 0x81a2ba1c19aaf8fd, 0x900ec40732aa005d,
0x5f268f90687019e3, 0x9fa5330d2286ed76, 0x1538f5bf127b52d8, 0xabe97171d7cffaec,
0x65e83c1625940199, 0x6e7525ef5129dd11, 0xa308e0bba1e3fdcf, 0x76fad582f986d0a2,
0xe603b38c0b0b8b40, 0xa2eb8d07b73aaa92, 0x2b49f9d01589da28, 0xd31b0f3cf39685b4,
0x1902009d4f6c2760, 0x5c9989f70c2e4596, 0xf9ca3e49844cd386, 0x2df8fcd71ed6ea28,
0xf5de122a0d3d083c, 0x607c2b066ccb1097, 0x77921186e0e07199, 0x1bf33ce5a3e3eb59,
0x61f70f56b9866aad, 0x9b8a3b30e2f82487, 0x8ffc9c35b35d5bb1, 0xf19bdf1c3f142c82,
0x9187f1f1b441e260, 0x83f3de0a9c4a11c2, 0xa1d33e23f707a4f1, 0xb2424377c227c117,
0x4727c9ee634cc37e, 0x3ecf83e4827a5132, 0xd994800698658353, 0x1885012aca553008,
0x3122072c6f6c9093, 0x4e7c845af023a34b, 0x801560ba012c151c, 0x851f344cb9d9ae90,
0x8c13eaa95f89bafc, 0xb72e2c10eb4a521b, 0x6245f9296dd24ee4, 0xfe53156944196525,
0x155630cafc65cd48, 0xf80e59f1878e4b09, 0xc2dd73b26b9e7e7c, 0xab23e9542e5cc437,
0x423bc96207d020b5, 0x2945fe0e0fcdd28d, 0x66ef34aaf151bd7b, 0x731df0be9f508a77,
0x5fc9f76a90595636, 0x1cb2d7808ca493be, 0xcd4320090744b5fb, 0xf030ab8f24e45ef9,
0xbd83ea2bf0c61145, 0x79c58301f996997b, 0xd125e8677462dd0b, 0x392fe4956a23e8a9,
0xa2ac91c919c7a78d, 0x534844914c61bb73, 0x3b481445248e645e, 0xd3bde3fc992e3d72,
0x167d51dde82d93fe, 0x96e30295f96c71f2, 0x593afa5ad60441db, 0xc56161bb1a6dcd1c,
0xa1fecf9e19a372ce, 0x5b10daa4444d8bbb, 0x3df2bd36496eb015, 0x4acdcac4cf2bba06,
0x303d0a8bd45d87c4, 0xea6bbfb7fc7f1366, 0x74370037f4b25d54, 0x5e8f5de53ab419ae,
0x79a03b9a965111f7, 0x7745053b8f00731e, 0xd0e6c082ab3bfaf3, 0x1161547582a77a75,
0x1addf3af2da4788a, 0x27b298a9cc9d317e, 0xe119f4893e54c663, 0x88a5acdf258e9f08,
0xce04753443a81b48, 0x1f13f36e149b8ce3, 0x49c878fc3bfa6aa0, 0x5051b10e77c42582,
0xcd4d69f2390a5854, 0xb2ff6906dd316433, 0x7ecad3f561df2395, 0x97fd7eeefed61c11,
0xc413a0f8ab30a847, 0x2283789415dcd294, 0x899d2dba8a86993c, 0x1f36b55a961b778a,
0x1ed906c51d8f0e29, 0x8e7c331c08088c6f, 0x2bdc4c12ab59cc85, 0xbfc9e4cc0fbcc2c5,
0xf956044490546b84, 0xf9e69b3f506c1902, 0x687125f52f43310c, 0xccbdba7b4c7da7c9,
0x2e5a74b4b6b418a5, 0xfdc47bd404e7957c, 0xd1e77f458a2ceab0, 0x9cfb7523996f8b41,
0xe3706fba66bbbfd4, 0x36b6a0ac8c39858f, 0xf3c195fa73194d4b, 0x4cfc73636cc16e3a,
0xb24fb812e8efbe78, 0x44ddb7ff4fe0fc7c, 0x74672a9d31c1d864, 0xc973aac0c741dd81,
0x617e33532795987b, 0x82259a4e19dd0990, 0x88638ac3fac893bb, 0x753234a6712aa71d,
0xd24ab80309a659d0, 0xf4dfccc0bd1be987, 0xd36f464c7f8f678d, 0xe9f4af08123bcec5,
0xdec3b02a6728a5ca, 0x37dec4a929401e00, 0x360145cf9c78ee8b, 0xb19ba21a3798b755,
0x8da3d933aea0d3b3, 0x424133d2200458f0, 0x6eded42399736107, 0x163c6066fe5818fd,
0x40ec8a365b4fc844, 0xf26905b7931db6d7, 0x42a0628b882cccbc, 0xb1ca60c4db04e752,
0xe1ab663d438a9c8d, 0x209225a6d8730c1b, 0x86ee5d084e437b63, 0xdbe371e3aaa92f31,
0x7c97ce35b6680bd1, 0x1a75d9220e2e6c77, 0x4f3a7e4dcf45d3ab, 0xfb29f1c8aae5a76b,
0xe700aa76f04e289e, 0xa3ff03491e201010, 0x631d40530cdf852a, 0xdbe507e41e817a5c,
0xce6a1b2e57a91ce2, 0xa324c6b22a7ab924, 0x97b6747e2870a49c, 0xfe22dfc28608686a,
0x3a15a8492e7615f2, 0x36d971781b3a4d6f, 0xb2a2256dc930f26f, 0xd6f03ec26a6b60e8,
0xfaa9af4d812a797e, 0xc539dc7bb6d3436d, 0x57b08a4e30d13347, 0x8a8526c64257284a,
0x576b83091f8418cc, 0x7daf31318abfb2e7, 0xccd71feabc460f96, 0xdad40612721f9c14,
0xb08b07a0282fcbb6, 0x4dca00f41fe72337, 0xc238205271d26f44, 0xa7f5ba96741b7378,
0x30120448121d5007, 0x78bd8fab59ee7945, 0x4da0a762c18b7f58, 0xd2b04be7e2cf9a76,
0x9fc61f24d1eaea11, 0x3b2dc310e15986e1, 0x679a42b44f04afaa, 0x26138b174d00f9f0,
0xb25509bbb5ee515c, 0xaf44c4161e109e2c, 0xf7cd2e5945bdf262, 0x5fdd9b8030a72a34,
0x4dd6210e83d9238a, 0xf23a26b7b33c2445, 0xba6cfe9689b79137, 0x4329211c496aaf64,
0x92f51587ad94181f, 0xb012239cd1e477f0, 0x71a93bd9c5aaa104, 0x4a01f11ca2a1f1e4,
0xbf926fdee98a3017, 0x75c2e80072caa48f, 0x7f59c4c2e03d0c1b, 0xa8983e2903d5cf8f,
0xd0f30c8ca0b9ed0d, 0xa026c63346652d58, 0x3a17db137d25ed5c, 0x5361eb6c14c20b33,
0x82a4cefbd3c085d6, 0xa51dbcddb4b17eb6, 0x61bf599191df4ad1, 0x16c91f9f7aa9586b,
0xfed82a73d5117216, 0xf302a571e9cdbefe, 0xd08e43df9c0ec848, 0x1b753149e8810bf4,
0x9163fad58921033f, 0x1d73b95282218653, 0xe9bfbfde273ba2c8, 0xaf6ef86b6c6435f7,
0x524cb03c2883ed73, 0x39288e4449207cb1, 0xee90e2353573ed98, 0x55f76a40b312f485,
0xde3dd6bb188c85c0, 0xf6e7700cfdb7be1b, 0x53993bcf4bfa0a7e, 0x36ccbafe69ee781f,
0x68e4abe4513a4093, 0xce9351526542cd54, 0x8f4362960a9d1d5b, 0x939cd71822e487a2,
0x799172e3f06ba5fd, 0x723cc2d4b8a75308, 0x536317b11771f49b, 0x4392f5cf4d689e57,
0xe52e7001dc2dbe15, 0x7d503d72cbd73464, 0xcce847fd06abd81f, 0x529c5fc940d77f4d,
0x49bd223af7f268b2, 0xef98b0cee64d7f3e, 0x84f563f28b1bd672, 0xfa94e77bc1a2692b,
0x9ac6cc497f37b0dd


template <typename T, typename Hash>
static T* hashLookup2(T* table, size_t buckets, const Hash& hash, const T& key, const T& empty)
{
    assert(buckets > 0);
    assert((buckets & (buckets - 1)) == 0);

    size_t hashmod = buckets - 1;
    size_t bucket = hash.hash(key) & hashmod;

    for (size_t probe = 0; probe <= hashmod; ++probe)
    {
        T& item = table[bucket];

        if (item == empty)
            return &item;

        if (hash.equal(item, key))
            return &item;

        // hash collision, quadratic probing
        bucket = (bucket + probe + 1) & hashmod;
    }

    assert(false && "Hash table is full");
    return 0;
}

*/



#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>

template<typename TFuncSignature>
class Callback;

template<typename R, typename... Args>
class Callback<R(Args...)> {
public:
    typedef R(*TFunc)(void*, Args...);

    Callback() : obj(0), func(0) {}
    Callback(void* o, TFunc f) : obj(o), func(f) {}

    R operator()(Args... a) const {
        return (*func)(obj, std::forward<Args>(a)...);
    }
    typedef void* Callback::*SafeBoolType;
    operator SafeBoolType() const {
        return func? &Callback::obj : 0;
    }
    bool operator!() const {
        return func == 0;
    }
    bool operator== (const Callback<R (Args...)>& right) const {
        return obj == right.obj && func == right.func;
    }
    bool operator!= (const Callback<R (Args...)>& right) const {
        return obj != right.obj || func != right.func;
    }
private:
    void* obj;
    TFunc func;
};

namespace detail {
    template<typename R, class T, typename... Args>
    struct DeduceConstMemCallback {
        template<R(T::*Func)(Args...) const> inline static Callback<R(Args...)> Bind(T* o) {
            struct _ { static R wrapper(void* o, Args... a) { return (static_cast<T*>(o)->*Func)(std::forward<Args>(a)...); } };
            return Callback<R(Args...)>(o, (R(*)(void*, Args...)) _::wrapper);
        }
    };

    template<typename R, class T, typename... Args>
    struct DeduceMemCallback {
        template<R(T::*Func)(Args...)> inline static Callback<R(Args...)> Bind(T* o) {
            struct _ { static R wrapper(void* o, Args... a) { return (static_cast<T*>(o)->*Func)(std::forward<Args>(a)...); } };
            return Callback<R(Args...)>(o, (R(*)(void*, Args...)) _::wrapper);
        }
    };

    template<typename R, typename... Args>
    struct DeduceStaticCallback {
        template<R(*Func)(Args...)> inline static Callback<R(Args...)> Bind() {
            struct _ { static R wrapper(void*, Args... a) { return (*Func)(std::forward<Args>(a)...); } };
            return Callback<R(Args...)>(0, (R(*)(void*, Args...)) _::wrapper);
        }
    };
}

template<typename R, class T, typename... Args>
detail::DeduceConstMemCallback<R, T, Args...> DeduceCallback(R(T::*)(Args...) const) {
    return detail::DeduceConstMemCallback<R, T, Args...>();
}

template<typename R, class T, typename... Args>
detail::DeduceMemCallback<R, T, Args...> DeduceCallback(R(T::*)(Args...)) {
    return detail::DeduceMemCallback<R, T, Args...>();
}

template<typename R, typename... Args>
detail::DeduceStaticCallback<R, Args...> DeduceCallback(R(*)(Args...)) {
    return detail::DeduceStaticCallback<R, Args...>();
}

template <typename... T1> class Event {
public:
    typedef void(*TSignature)(T1...);
    typedef Callback<void(T1...)> TCallback;
    typedef std::vector<TCallback> InvocationTable;

protected:
    InvocationTable invocations;

public:
    const static int ExpectedFunctorCount = 2;

    Event() : invocations() {
        invocations.reserve(ExpectedFunctorCount);
    }

    template <void (* TFunc)(T1...)> void Add() {
        TCallback c = DeduceCallback(TFunc).template Bind<TFunc>();
        invocations.push_back(c);
    }

    template <typename T, void (T::* TFunc)(T1...)> void Add(T& object) {
        Add<T, TFunc>(&object);
    }

    template <typename T, void (T::* TFunc)(T1...)> void Add(T* object) {
        TCallback c = DeduceCallback(TFunc).template Bind<TFunc>(object);
        invocations.push_back(c);
    }

    template <typename T, void (T::* TFunc)(T1...) const> void Add(T& object) {
        Add<T, TFunc>(&object);
    }

    template <typename T, void (T::* TFunc)(T1...) const> void Add(T* object) {
        TCallback c = DeduceCallback(TFunc).template Bind<TFunc>(object);
        invocations.push_back(c);
    }

    void Invoke(T1... t1) {
        for(size_t i = 0; i < invocations.size() ; ++i) invocations[i](std::forward<T1>(t1)...);
    }

    void operator()(T1... t1) {
        Invoke(std::forward<T1>(t1)...);
    }

    size_t InvocationCount() { return invocations.size(); }

    template <void (* TFunc)(T1...)> bool Remove ()
    { return Remove (DeduceCallback(TFunc).template Bind<TFunc>()); }
    template <typename T, void (T::* TFunc)(T1...)> bool Remove (T& object)
    { return Remove <T, TFunc>(&object); }
    template <typename T, void (T::* TFunc)(T1...)> bool Remove (T* object)
    { return Remove (DeduceCallback(TFunc).template Bind<TFunc>(object)); }
    template <typename T, void (T::* TFunc)(T1...) const> bool Remove (T& object)
    { return Remove <T, TFunc>(&object); }
    template <typename T, void (T::* TFunc)(T1...) const> bool Remove (T* object)
    { return Remove (DeduceCallback(TFunc).template Bind<TFunc>(object)); }

protected:
    bool Remove( TCallback const& target ) {
        auto it = std::find(invocations.begin(), invocations.end(), target);
        if (it == invocations.end())
            return false;
        invocations.erase(it);
        return true;
    }
};

namespace IntStatic {
        void VoidTest ( ) {
                std::cout << "INTO THE VOID"         << std::endl;
        }
        void IntTest ( int num ) {
                std::cout << "Got myself a "         << num  << " !"    << std::endl;
        }
        void IntTest2 ( int num ) {
                std::cout << "Now _I_ Got myself a " << num  << " !"    << std::endl;
        }
}

struct Integer {
    void Test ( int num ) {
            std::cout << num  << " on the inside of a class... ?"   << std::endl;
    }
    void Test2 ( int num ) {
            std::cout << num  << " on the inside of a struct, yo !" << std::endl;
    }
    static void Test3 ( int snum ) {
            std::cout << snum << " on the inside of a STATIC method?!"   << std::endl;
    }
};

struct Multi {
    void Test ( std::string woo, std::string yeah ) {
            std::cout << "Dat Multi Member Function: ";
            std::cout << woo << " | " << yeah  << std::endl;
    }

    void static Test2 ( std::string woo, std::string yeah ) {
            std::cout << "Dat Static Multi Function: ";
            std::cout << woo << " | " << yeah  << std::endl;
    }
};

int main7687867 ( int argc, char* argv[] ) {

    Event<int> intev;
    Integer i;
    Multi m;

    intev.Add<Integer, &Integer::Test>(i);
    intev.Add<&IntStatic::IntTest>();
    intev.Add<&IntStatic::IntTest2>();
    intev(20);
    intev.Remove<&IntStatic::IntTest>();
    intev.Remove<&IntStatic::IntTest>();
    intev.Remove<Integer, &Integer::Test>(i);
    intev(20);

    Event<std::string, std::string> doublestringev;

    doublestringev.Add<Multi, &Multi::Test>( m );
    doublestringev( "Bark Bark", "Meow Meow" );
    doublestringev.Remove<Multi, &Multi::Test>( m );

    doublestringev.Add<&Multi::Test2>( );
    doublestringev( "Bahh Bahh", "Moo Moo" );

    return EXIT_SUCCESS;
}



#include <fsmlite/fsm.hpp>


class player : public fsmlite::fsm<player> {
    friend class fsm;  // base class needs access to transition_table
    public:
    enum states { Stopped, Open, Empty, Playing, Paused };

    player ( state_type init_state = Empty ) : fsm ( init_state ) { }

    struct play { };
    struct open_close { };
    struct cd_detected {
        cd_detected ( const char* s = "" ) : title ( s ) { }
        std::string title;
    };
    struct stop { };
    struct pause { };

    private:
    void start_playback ( const play& );
    void open_drawer ( const open_close& );
    void close_drawer ( const open_close& );
    void store_cd_info ( const cd_detected& cd );
    void stop_playback ( const stop& );
    void pause_playback ( const pause& );
    void stop_and_open ( const open_close& );
    void resume_playback ( const play& );

    private:
    using transition_table = table<
    //              Start    Event        Target   Action
    //  -----------+--------+------------+--------+------------------------+-
    mem_fn_row< Stopped, play, Playing, &player::start_playback  >,
    mem_fn_row< Stopped, open_close, Open, &player::open_drawer     >,
    mem_fn_row< Open, open_close, Empty, &player::close_drawer    >,
    mem_fn_row< Empty, open_close, Open, &player::open_drawer     >,
    mem_fn_row< Empty, cd_detected, Stopped, &player::store_cd_info   >,
    mem_fn_row< Playing, stop, Stopped, &player::stop_playback   >,
    mem_fn_row< Playing, pause, Paused, &player::pause_playback  >,
    mem_fn_row< Playing, open_close, Open, &player::stop_and_open   >,
    mem_fn_row< Paused, play, Playing, &player::resume_playback >,
    mem_fn_row< Paused, stop, Stopped, &player::stop_playback   >,
    mem_fn_row< Paused, open_close, Open, &player::stop_and_open   >
    //  -----------+--------+------------+--------+------------------------+-
    >;
};

void player::start_playback ( const play& ) {
    std::cout << "Starting playback\n";
}

void player::open_drawer ( const open_close& ) {
    std::cout << "Opening drawer\n";
}

void player::close_drawer ( const open_close& ) {
    std::cout << "Closing drawer\n";
}

void player::store_cd_info ( const cd_detected& cd ) {
    std::cout << "Detected CD '" << cd.title << "'\n";
}

void player::stop_playback ( const stop& ) {
    std::cout << "Stopping playback\n";
}

void player::pause_playback ( const pause& ) {
    std::cout << "Pausing playback\n";
}

void player::stop_and_open ( const open_close& ) {
    std::cout << "Stopping and opening drawer\n";
}

void player::resume_playback ( const play& ) {
    std::cout << "Resuming playback\n";
}

int main6786556 ( ) {
    player p;

    p.process_event ( player::open_close ( ) );
    assert ( p.current_state ( ) == player::Open );
    p.process_event ( player::open_close ( ) );
    assert ( p.current_state ( ) == player::Empty );
    p.process_event ( player::cd_detected ( "Rubber Soul" ) );
    assert ( p.current_state ( ) == player::Stopped );
    p.process_event ( player::play ( ) );
    assert ( p.current_state ( ) == player::Playing );
    p.process_event ( player::pause ( ) );
    assert ( p.current_state ( ) == player::Paused );
    p.process_event ( player::open_close ( ) );
    assert ( p.current_state ( ) == player::Open );
    p.process_event ( player::open_close ( ) );
    assert ( p.current_state ( ) == player::Empty );

    return EXIT_SUCCESS;
}
