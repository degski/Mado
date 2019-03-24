
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
#include <future>
#include <sax/iostream.hpp>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <random>
#include <string>
#include <thread>
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


#if 1

#include "App.hpp"


#include <lemon/smart_graph.h>

template<typename RadBase>
struct HexGrid : public RadBase {

    using rad = RadBase;
    using size_type = typename rad::size_type;

    lemon::SmartDigraph m_grid;

    void add_valid_neighbor_arc ( const size_type i_, const size_type q_, const size_type r_ ) noexcept {
        if ( rad::is_invalid ( q_, r_ ) )
            return;
        m_grid.addArc ( m_grid.nodeFromId ( rad::index ( q_, r_ ) ), m_grid.nodeFromId ( i_ ) );
    }
    void add_neighbor_arcs ( const size_type q_, const size_type r_ ) noexcept {
        const size_type i = rad::index ( q_, r_ );
        add_valid_neighbor_arc ( i, q_    , r_ - 1 );
        add_valid_neighbor_arc ( i, q_ + 1, r_ - 1 );
        add_valid_neighbor_arc ( i, q_ - 1, r_     );
        add_valid_neighbor_arc ( i, q_ + 1, r_     );
        add_valid_neighbor_arc ( i, q_ - 1, r_ + 1 );
        add_valid_neighbor_arc ( i, q_    , r_ + 1 );
    }

    HexGrid ( ) : RadBase { } {

        // Add nodes.
        for ( size_type i = 0; i < rad::size ( ); ++i )
            m_grid.addNode ( );
        // Add arcs.
        size_type q = rad::centre_idx ( ), r = rad::centre_idx ( );
        add_neighbor_arcs ( q, r );
        for ( size_type ring = 1; ring <= rad::radius ( ); ++ring ) {
            ++q; // move to next ring, east.
            for ( size_type j = 0; j < ring; ++j ) // nw.
                add_neighbor_arcs ( q, --r );
            for ( size_type j = 0; j < ring; ++j ) // w.
                add_neighbor_arcs ( --q, r );
            for ( size_type j = 0; j < ring; ++j ) // sw.
                add_neighbor_arcs ( --q, ++r );
            for ( size_type j = 0; j < ring; ++j ) // se.
                add_neighbor_arcs ( q, ++r );
            for ( size_type j = 0; j < ring; ++j ) // e.
                add_neighbor_arcs ( ++q, r );
            for ( size_type j = 0; j < ring; ++j ) // ne.
                add_neighbor_arcs ( ++q, --r );
        }
    }

    [[ nodiscard ]] lemon::SmartDigraph & grid ( ) noexcept {
        return m_grid;
    }

};


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


int main ( ) {
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
        keep.reset ( 12 );
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

#else

#include "Hexcontainer.hpp"

using board_type = HexContainer<int, 2, true>;


int main7686787 ( ) {

    board_type board;

    std::cout << sizeof ( board ) << nl;

    std::cout << board_type::index ( 0, 0 ) << nl;

    for ( const auto n : board_type::neighbors ) {
        for ( const auto v : n ) {
            std::cout << static_cast< int > ( v ) << sp;
        }
        std::cout << nl;
    }
    std::cout << nl;

    for ( auto & v : board )
        v = 1;

    std::cout << board << nl;

    return EXIT_SUCCESS;
}

#endif

/*

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

#if 0

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
    [ [ maybe_unused ] ] friend Stream & operator << ( Stream & out_, const float_as_bits & v_ ) noexcept {
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
        return *this;
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

#endif
