
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

#include "../../MCTSSearchTree/include/flat_search_tree.hpp"
#include "MonteCarlo.hpp"

#if 1

#    include "Application.hpp"

void handleEptr ( std::exception_ptr eptr ) { // Passing by value is ok.
    try {
        if ( eptr )
            std::rethrow_exception ( eptr );
    }
    catch ( const std::exception & e ) {
        std::cout << "Caught exception \"" << e.what ( ) << "\"\n";
    }
}

[[nodiscard]] bool isEscapePressed ( sf::Event const & event_ ) noexcept {
    return sf::Event::KeyPressed == event_.type and sf::Keyboard::Escape == event_.key.code;
}

[[nodiscard]] int isCtrlShiftNumPressed ( sf::Event const & event_ ) noexcept {
    if ( event_.key.control and event_.key.shift and sf::Event::KeyPressed == event_.type ) {
        if ( sf::Keyboard::Num4 == event_.key.code )
            return 4;
        if ( sf::Keyboard::Num5 == event_.key.code )
            return 5;
        if ( sf::Keyboard::Num6 == event_.key.code )
            return 6;
        if ( sf::Keyboard::Num7 == event_.key.code )
            return 7;
        if ( sf::Keyboard::Num8 == event_.key.code )
            return 8;
    }
    return 0;
}

int main65675 ( ) {
    std::exception_ptr eptr;
    try {
        // Application app;
        A app;
        sf::Pacer keep ( 60 );
        sf::Event event;
        // Startup animation.
        while ( app.runStartupAnimation ( ) ) {
            while ( app.pollWindowEvent ( event ) ) {
                if ( isEscapePressed ( event ) ) {
                    app.closeWindow ( );
                    return EXIT_SUCCESS;
                }
            }
            keep.pace ( );
        }
        // Regular game loop.
        keep.reset ( 20 );
        while ( app.isWindowOpen ( ) ) {
            while ( app.pollWindowEvent ( event ) ) {
                if ( sf::Event::LostFocus == event.type ) {
                    app.pause ( );
                    break;
                }
                if ( sf::Event::GainedFocus == event.type ) {
                    app.resume ( );
                }
                if ( sf::Event::Closed == event.type or isEscapePressed ( event ) ) {
                    app.closeWindow ( );
                    return EXIT_SUCCESS;
                }
                if ( int const n = isCtrlShiftNumPressed ( event ) ) {
                    std::cout << "c+s+" << n << nl;
                    A::resize ( app, n );
                }
                app.mouseEvents ( event );
            }
            if ( app.isRunning ( ) ) {
                app.updateWindow ( );
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

int main ( ) {
    sax::enable_virtual_terminal_sequences ( );
    std::ios_base::sync_with_stdio ( false );
    using State = Mado<3>;
    State state;
    using Player = typename State::value_type;
    Player winner;
    std::uint32_t matches = 0u, agent_wins = 0u, human_wins = 0u;
    putchar ( '\n' );
    sf::HrClock::duration elapsed;
    sf::HrTimePoint match_start;
    for ( int i = 0; i < 100; ++i ) {
        {
            State state;
            match_start = Clock::instance ( ).now ( );
            do {
                state.moveWinner ( Mcts::compute_move ( state, Mcts::ComputeOptions{ } ) );
                std::cout << nl << state << nl;
            } while ( state.nonterminal ( ) );
            winner = state.winner ( );
        }
        elapsed += since ( match_start );
        ++matches;
        switch ( winner.as_index ( ) ) {
            case ( int ) Player::Type::agent: ++agent_wins; break;
            case ( int ) Player::Type::human: ++human_wins; break;
        }
        float a = ( 1000.0f * agent_wins ) / float ( agent_wins + human_wins );
        a       = ( ( int ) a ) / 10.0f;
        float h = ( 1000.0f * human_wins ) / float ( agent_wins + human_wins );
        h       = ( ( int ) h ) / 10.0f;
        printf ( "\r Match %i: Agent%6.1f%% - Human%6.1f%% (%.1f Sec./Match - %.1f Sec.)\n", matches, a, h,
                 std::chrono::duration_cast<std::chrono::milliseconds> ( elapsed ).count ( ) / ( ( float ) matches * 1'000.0f ),
                 std::chrono::duration_cast<std::chrono::milliseconds> ( elapsed ).count ( ) / 1'000.0f );
    }

    return EXIT_SUCCESS;
}

// Possible additional flags Clang/LLVM:
// -fsanitize=address
// -Xclang -fconstexpr-steps -Xclang 10000000

int main67567 ( ) {

    sax::enable_virtual_terminal_sequences ( );

    Mado<4> mado;

    int c[ 3 ]{ };

    plf::nanotimer timer;

    timer.start ( );

    for ( int i = 0; i < 1; ++i ) {
        ( ( c + 1 )[ mado.simulate ( ).as_index ( ) ] ) += 1;
        mado.reset ( );
    }

    std::cout << ( ( int ) ( timer.get_elapsed_us ( ) ) / 1'000'000 ) << " usecs/sim" << nl;
    std::cout << c[ 0 ] << " " << c[ 1 ] << " " << c[ 2 ] << nl;

    return EXIT_SUCCESS;
}

int main786786 ( ) {

    sax::enable_virtual_terminal_sequences ( );

    Mado<4> mado0;

    mado0.moveHashWinner ( Move<4>{ std::int8_t{ 22 } } );
    mado0.moveHashWinner ( Move<4>{ std::int8_t{ 38 } } );

    Mado<4> mado = mado0;

    mado.moveHashWinner ( Move<4>{ std::int8_t{ 27 } } );
    mado.moveHashWinner ( Move<4>{ std::int8_t{ 34 } } );

    std::cout << mado << nl;

    Moves<4, Mado<4>::Board::size ( )> moves;

    bool N = mado.availableMoves ( moves );

    print_moves ( moves );

    return EXIT_SUCCESS;
}

#else

#    include "Hexcontainer.hpp"

using board_type = HexContainer<int, 2, true>;

int main7686787 ( ) {

    board_type Board;

    std::cout << sizeof ( Board ) << nl;

    std::cout << board_type::index ( 0, 0 ) << nl;

    for ( auto const n : board_type::neighbors ) {
        for ( auto const v : n ) {
            std::cout << static_cast<int> ( v ) << sp;
        }
        std::cout << nl;
    }
    std::cout << nl;

    for ( auto & v : Board )
        v = 1;

    std::cout << Board << nl;

    return EXIT_SUCCESS;
}

#    include "Mado.hpp"

struct Hash64 {
    size_t operator( ) ( uint64_t k ) const { return ( k ^ 14695981039346656037ULL ) * 1099511628211ULL; }
};

struct Hash32 {
    size_t operator( ) ( uint32_t k ) const { return ( k ^ 2166136261U ) * 16777619UL; }
};

// 3 >  68 - 37, 1.84
// 4 > 107 - 61, 1.75
// 5 > 155 - 91, 1.70

// -fsanitize=address

int main ( ) {

    Mado<4> state;

    plf::nanotimer timer;

    timer.start ( );

    for ( int i = 0; i < 10'000; ++i ) {
        while ( true ) {
            auto const m = state.randomMove ( );
            if ( state.terminal ( ) )
                break;
            state.moveWinner ( m );
        }
        state.reset ( );
    }

    std::cout << ( int ) ( timer.get_elapsed_ms ( ) ) << " msecs" << nl;

    return EXIT_SUCCESS;
}

int main8768678 ( ) {

    std::vector<int> a{ 1, 2, 3, 4, 5, 6, 7, 8 };

    sf::saveToFileLZ4 ( a, "y://tmp//", "csv_test", true );

    std::vector<int> b{ 11, 12, 13, 14, 15, 16, 17, 18 };

    sf::saveToFileLZ4 ( b, "y://tmp//", "csv_test", true );

    std::vector<int> c;

    std::fpos_t pos = sf::loadFromFileLZ4 ( c, "y://tmp//", "csv_test" );

    for ( auto & i : c )
        std::cout << i << ' ';
    std::cout << nl;

    std::vector<int> d;

    sf::loadFromFileLZ4 ( d, "y://tmp//", "csv_test", pos );

    for ( auto & i : d )
        std::cout << i << ' ';
    std::cout << nl;

    return EXIT_SUCCESS;
}

// LZ4 API example : Dictionary Random Access

#    if defined( _MSC_VER ) && ( _MSC_VER <= 1800 ) /* Visual Studio <= 2013 */
#        define _CRT_SECURE_NO_WARNINGS
#        define snprintf sprintf_s
#    endif
#    include <lz4.h>
#    include <lz4frame.h>

#    include <cstdio>
#    include <cstdint>
#    include <cstdlib>
#    include <cstring>

#    define MIN( x, y ) ( ( x ) < ( y ) ? ( x ) : ( y ) )

enum {
    BLOCK_BYTES      = 1024, /* 1 KiB of uncompressed data in a block */
    DICTIONARY_BYTES = 1024, /* Load a 1 KiB dictionary */
    MAX_BLOCKS       = 1024  /* For simplicity of implementation */
};

/**
 * Magic bytes for this test case.
 * This is not a great magic number because it is a common word in ASCII.
 * However, it is important to have some versioning system in your format.
 */
const char kTestMagic[] = { 'T', 'E', 'S', 'T' };

void write_int ( FILE * fp, int i ) {
    size_t written = fwrite ( &i, sizeof ( i ), 1, fp );
    if ( written != 1 ) {
        exit ( 10 );
    }
}

void write_bin ( FILE * fp, const void * array, size_t arrayBytes ) {
    size_t written = fwrite ( array, 1, arrayBytes, fp );
    if ( written != arrayBytes ) {
        exit ( 11 );
    }
}

void read_int ( FILE * fp, int * i ) {
    size_t read = fread ( i, sizeof ( *i ), 1, fp );
    if ( read != 1 ) {
        exit ( 12 );
    }
}

size_t read_bin ( FILE * fp, void * array, size_t arrayBytes ) {
    size_t read = fread ( array, 1, arrayBytes, fp );
    if ( ferror ( fp ) ) {
        exit ( 12 );
    }
    return read;
}

void seek_bin ( FILE * fp, long offset, int origin ) {
    if ( fseek ( fp, offset, origin ) ) {
        exit ( 14 );
    }
}

void test_compress ( FILE * outFp, FILE * inpFp, const char * dict, int dictSize ) {
    LZ4_stream_t lz4Stream_body;
    LZ4_stream_t * lz4Stream = &lz4Stream_body;

    char inpBuf[ BLOCK_BYTES ];
    int offsets[ MAX_BLOCKS ];
    int * offsetsEnd = offsets;

    LZ4_initStream ( lz4Stream, sizeof ( *lz4Stream ) );

    /* Write header magic */
    write_bin ( outFp, kTestMagic, sizeof ( kTestMagic ) );

    *offsetsEnd++ = sizeof ( kTestMagic );
    /* Write compressed data blocks.  Each block contains BLOCK_BYTES of plain
       data except possibly the last. */
    for ( ;; ) {
        int const inpBytes = ( int ) read_bin ( inpFp, inpBuf, BLOCK_BYTES );
        if ( 0 == inpBytes ) {
            break;
        }

        /* Forget previously compressed data and load the dictionary */
        LZ4_loadDict ( lz4Stream, dict, dictSize );
        {
            char cmpBuf[ LZ4_COMPRESSBOUND ( BLOCK_BYTES ) ];
            int const cmpBytes = LZ4_compress_fast_continue ( lz4Stream, inpBuf, cmpBuf, inpBytes, sizeof ( cmpBuf ), 1 );
            if ( cmpBytes <= 0 ) {
                exit ( 1 );
            }
            write_bin ( outFp, cmpBuf, ( size_t ) cmpBytes );
            /* Keep track of the offsets */
            *offsetsEnd = *( offsetsEnd - 1 ) + cmpBytes;
            ++offsetsEnd;
        }
        if ( offsetsEnd - offsets > MAX_BLOCKS ) {
            exit ( 2 );
        }
    }
    /* Write the tailing jump table */
    {
        int * ptr = offsets;
        while ( ptr != offsetsEnd ) {
            write_int ( outFp, *ptr++ );
        }
        write_int ( outFp, offsetsEnd - offsets );
    }
}

void test_decompress ( FILE * outFp, FILE * inpFp, const char * dict, int dictSize, int offset, int length ) {
    LZ4_streamDecode_t lz4StreamDecode_body;
    LZ4_streamDecode_t * lz4StreamDecode = &lz4StreamDecode_body;

    /* The blocks [currentBlock, endBlock) contain the data we want */
    int currentBlock = offset / BLOCK_BYTES;
    int endBlock     = ( ( offset + length - 1 ) / BLOCK_BYTES ) + 1;

    char decBuf[ BLOCK_BYTES ];
    int offsets[ MAX_BLOCKS ];

    /* Special cases */
    if ( length == 0 ) {
        return;
    }

    /* Read the magic bytes */
    {
        char magic[ sizeof ( kTestMagic ) ];
        size_t read = read_bin ( inpFp, magic, sizeof ( magic ) );
        if ( read != sizeof ( magic ) ) {
            exit ( 1 );
        }
        if ( memcmp ( kTestMagic, magic, sizeof ( magic ) ) ) {
            exit ( 2 );
        }
    }

    /* Read the offsets tail */
    {
        int numOffsets;
        int block;
        int * offsetsPtr = offsets;
        seek_bin ( inpFp, -4, SEEK_END );
        read_int ( inpFp, &numOffsets );
        if ( numOffsets <= endBlock ) {
            exit ( 3 );
        }
        seek_bin ( inpFp, -4 * ( numOffsets + 1 ), SEEK_END );
        for ( block = 0; block <= endBlock; ++block ) {
            read_int ( inpFp, offsetsPtr++ );
        }
    }
    /* Seek to the first block to read */
    seek_bin ( inpFp, offsets[ currentBlock ], SEEK_SET );
    offset = offset % BLOCK_BYTES;

    /* Start decoding */
    for ( ; currentBlock < endBlock; ++currentBlock ) {
        char cmpBuf[ LZ4_COMPRESSBOUND ( BLOCK_BYTES ) ];
        /* The difference in offsets is the size of the block */
        int cmpBytes = offsets[ currentBlock + 1 ] - offsets[ currentBlock ];
        {
            const size_t read = read_bin ( inpFp, cmpBuf, ( size_t ) cmpBytes );
            if ( read != ( size_t ) cmpBytes ) {
                exit ( 4 );
            }
        }

        /* Load the dictionary */
        LZ4_setStreamDecode ( lz4StreamDecode, dict, dictSize );
        {
            int const decBytes = LZ4_decompress_safe_continue ( lz4StreamDecode, cmpBuf, decBuf, cmpBytes, BLOCK_BYTES );
            if ( decBytes <= 0 ) {
                exit ( 5 );
            }
            {
                /* Write out the part of the data we care about */
                int blockLength = MIN ( length, ( decBytes - offset ) );
                write_bin ( outFp, decBuf + offset, ( size_t ) blockLength );
                offset = 0;
                length -= blockLength;
            }
        }
    }
}

int compare ( FILE * fp0, FILE * fp1, int length ) {
    int result = 0;

    while ( 0 == result ) {
        char b0[ 4096 ];
        char b1[ 4096 ];
        const size_t r0 = read_bin ( fp0, b0, MIN ( length, ( int ) sizeof ( b0 ) ) );
        const size_t r1 = read_bin ( fp1, b1, MIN ( length, ( int ) sizeof ( b1 ) ) );

        result = ( int ) r0 - ( int ) r1;

        if ( 0 == r0 || 0 == r1 ) {
            break;
        }
        if ( 0 == result ) {
            result = memcmp ( b0, b1, r0 );
        }
        length -= r0;
    }

    return result;
}

int main869689 ( int argc, char * argv[] ) {
    char inpFilename[ 256 ]  = { 0 };
    char lz4Filename[ 256 ]  = { 0 };
    char decFilename[ 256 ]  = { 0 };
    char dictFilename[ 256 ] = { 0 };
    int offset;
    int length;
    char dict[ DICTIONARY_BYTES ];
    int dictSize;

    if ( argc < 5 ) {
        printf ( "Usage: %s input dictionary offset length", argv[ 0 ] );
        return 0;
    }

    snprintf ( inpFilename, 256, "%s", argv[ 1 ] );
    snprintf ( lz4Filename, 256, "%s.lz4s-%d", argv[ 1 ], BLOCK_BYTES );
    snprintf ( decFilename, 256, "%s.lz4s-%d.dec", argv[ 1 ], BLOCK_BYTES );
    snprintf ( dictFilename, 256, "%s", argv[ 2 ] );
    offset = atoi ( argv[ 3 ] );
    length = atoi ( argv[ 4 ] );

    printf ( "inp    = [%s]\n", inpFilename );
    printf ( "lz4    = [%s]\n", lz4Filename );
    printf ( "dec    = [%s]\n", decFilename );
    printf ( "dict   = [%s]\n", dictFilename );
    printf ( "offset = [%d]\n", offset );
    printf ( "length = [%d]\n", length );

    /* Load dictionary */
    {
        FILE * dictFp = fopen ( dictFilename, "rb" );
        dictSize      = ( int ) read_bin ( dictFp, dict, DICTIONARY_BYTES );
        fclose ( dictFp );
    }

    /* compress */
    {
        FILE * inpFp = fopen ( inpFilename, "rb" );
        FILE * outFp = fopen ( lz4Filename, "wb" );

        printf ( "compress : %s -> %s\n", inpFilename, lz4Filename );
        test_compress ( outFp, inpFp, dict, dictSize );
        printf ( "compress : done\n" );

        fclose ( outFp );
        fclose ( inpFp );
    }

    /* decompress */
    {
        FILE * inpFp = fopen ( lz4Filename, "rb" );
        FILE * outFp = fopen ( decFilename, "wb" );

        printf ( "decompress : %s -> %s\n", lz4Filename, decFilename );
        test_decompress ( outFp, inpFp, dict, DICTIONARY_BYTES, offset, length );
        printf ( "decompress : done\n" );

        fclose ( outFp );
        fclose ( inpFp );
    }

    /* verify */
    {
        FILE * inpFp = fopen ( inpFilename, "rb" );
        FILE * decFp = fopen ( decFilename, "rb" );
        seek_bin ( inpFp, offset, SEEK_SET );

        printf ( "verify : %s <-> %s\n", inpFilename, decFilename );
        int const cmp = compare ( inpFp, decFp, length );
        if ( 0 == cmp ) {
            printf ( "verify : OK\n" );
        }
        else {
            printf ( "verify : NG\n" );
        }

        fclose ( decFp );
        fclose ( inpFp );
    }

    return 0;
}

#endif

/*

template <typename T, typename Hash>
static T* hashLookup2(T* table, size_t buckets, const Hash& hash, T const& key, T const& empty)
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

#    include <iostream>
#    include <vector>
#    include <utility>
#    include <algorithm>

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
void print_bits ( T const n ) noexcept {
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

    float_as_bits ( float const & v_ ) : value { v_ } { };
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
    using const_pointer = T const *;
    using reference = T & ;
    using const_reference = T const &;

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
    // Simulate both copy-assign and Move-assign
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
            int const total_construct = num_construct + num_copy + num_move;
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
