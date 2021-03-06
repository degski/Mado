
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
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include <array>
#include <execution>
#include <sax/iostream.hpp>
#include <iterator>
#include <type_traits>

#include <cereal/cereal.hpp>
#include <cereal/types/array.hpp>
#include <cereal/archives/binary.hpp>

#include <experimental/fixed_capacity_vector>

template<int R, bool zero_base = true>
struct RadiusBase {

    static_assert ( R > 1 and R < 105, "radius (R) must be on interval [ 2, 104 ]" );

    using IdxType =
        std::conditional_t<( 1 + 3 * R * ( R + 1 ) ) < std::numeric_limits<std::int8_t>::max ( ), std::int8_t, std::int16_t>;

    using index_array       = std::experimental::fixed_capacity_vector<IdxType, ( 1 + 2 * R )>;
    using const_index_array = index_array const;

    using size_type       = int;
    using difference_type = size_type;

    [[nodiscard]] static constexpr size_type radius ( ) noexcept { return R; }

    [[nodiscard]] static constexpr size_type size ( ) noexcept {
        return static_cast<size_type> ( 1 + 3 * radius ( ) * ( radius ( ) + 1 ) );
    }

    [[nodiscard]] static constexpr size_type width ( ) noexcept { return static_cast<size_type> ( 2 * radius ( ) + 1 ); }
    [[nodiscard]] static constexpr size_type height ( ) noexcept { return static_cast<size_type> ( 2 * radius ( ) + 1 ); }

    [[nodiscard]] static constexpr size_type dw_cols ( ) noexcept { return static_cast<size_type> ( 2 * width ( ) + 1 ); }
    [[nodiscard]] static constexpr size_type dw_rows ( ) noexcept { return static_cast<size_type> ( height ( ) + 2 ); }

    [[nodiscard]] static constexpr size_type center_idx ( ) noexcept {
        return static_cast<size_type> ( zero_base ? 0 : radius ( ) );
    }

    RadiusBase ( ) noexcept                    = default;
    RadiusBase ( RadiusBase const & ) noexcept = default;
    RadiusBase ( RadiusBase && ) noexcept      = default;

    RadiusBase & operator= ( RadiusBase const & ) noexcept = default;
    RadiusBase & operator= ( RadiusBase && ) noexcept = default;

    ~RadiusBase ( ) noexcept = default;

    // Compile-time function.
    template<size_type Start>
    [[nodiscard]] static constexpr const_index_array make_index_array ( ) noexcept {
        index_array a ( 1, static_cast<IdxType> ( Start - radius ( ) ) );
        size_type i = radius ( ) + 1;
        for ( ; i < ( 1 + 2 * radius ( ) ); ++i )
            a.emplace_back ( static_cast<IdxType> ( a.back ( ) + i + 1 ) );
        for ( ; i > ( 1 + radius ( ) ); --i )
            a.emplace_back ( static_cast<IdxType> ( a.back ( ) + i ) );
        return a;
    }

    [[nodiscard]] static constexpr size_type index ( size_type const q_, size_type const r_ ) noexcept {
        assert ( not( is_invalid ( q_, r_ ) ) );
        constexpr const_index_array idx = make_index_array < zero_base ? R : 0 > ( );
        return ( idx.data ( ) + ( zero_base ? R : 0 ) )[ r_ ] + q_;
    }

    [[nodiscard]] static constexpr bool is_invalid ( size_type const q_, size_type const r_ ) noexcept {
        auto abs = [] ( auto const a ) noexcept { return a > decltype ( a ){ 0 } ? a : -a; };
        return abs ( q_ - ( zero_base ? 0 : radius ( ) ) ) > radius ( ) or
               abs ( r_ - ( zero_base ? 0 : radius ( ) ) ) > radius ( ) or
               abs ( -q_ - r_ + ( 2 * ( zero_base ? 0 : radius ( ) ) ) ) > radius ( );
    }
    [[nodiscard]] static constexpr bool is_valid ( size_type const q_, size_type const r_ ) noexcept {
        return not( is_invalid ( q_, r_ ) );
    }
};

template<int R, bool zero_base>
struct Hex : public RadiusBase<R, zero_base> {

    using rad        = RadiusBase<R, zero_base>;
    using IdxType    = typename rad::IdxType;
    using value_type = IdxType;
    using size_type  = typename rad::size_type;

    using rad::center_idx;
    using rad::dw_cols;
    using rad::dw_rows;
    using rad::height;
    using rad::index;
    using rad::is_invalid;
    using rad::is_valid;
    using rad::radius;
    using rad::size;
    using rad::width;

    value_type q = value_type{ -radius ( ) - 1 }, r = value_type{ -radius ( ) - 1 };

    Hex ( ) noexcept             = default;
    Hex ( Hex const & ) noexcept = default;
    Hex ( Hex && ) noexcept      = default;

    Hex & operator= ( Hex const & ) noexcept = default;
    Hex & operator= ( Hex && ) noexcept = default;

    ~Hex ( ) noexcept = default;

    void nil ( ) noexcept {
        q = value_type{ -radius ( ) - 1 };
        r = value_type{ -radius ( ) - 1 };
    }

    [[nodiscard]] constexpr bool operator== ( Hex const & rhs_ ) const noexcept { return q == rhs_.q and r == rhs_.r; }
    [[nodiscard]] constexpr bool operator!= ( Hex const & rhs_ ) const noexcept { return q != rhs_.q or r != rhs_.r; }

    template<typename Stream>
    [[maybe_unused]] friend Stream & operator<< ( Stream & out_, Hex const & h_ ) noexcept {
        if ( value_type{ -Hex::radius ( ) - 1 } == h_.q )
            out_ << std::dec << "<* *>";
        else
            out_ << std::dec << '<' << std::setw ( 2 ) << static_cast<int> ( h_.q ) << ' ' << std::setw ( 2 )
                 << static_cast<int> ( h_.r ) << '>';
        return out_;
    }

    private:
    friend class cereal::access;

    template<class Archive>
    void serialize ( Archive & ar_ ) {
        ar_ ( q );
        ar_ ( r );
    }
};

template<int R, bool zero_base>
struct HexBase : public RadiusBase<R, zero_base> {

    using rad             = RadiusBase<R, zero_base>;
    using IdxType         = typename rad::IdxType;
    using size_type       = typename rad::size_type;
    using const_pointer   = IdxType const *;
    using const_reference = IdxType const &;

    using neighbors_type       = std::experimental::fixed_capacity_vector<IdxType, 6>;
    using neighbors_type_array = std::array<neighbors_type, rad::size ( )>;

    using const_iterator = typename neighbors_type::const_iterator;

    using rad::center_idx;
    using rad::dw_cols;
    using rad::dw_rows;
    using rad::height;
    using rad::index;
    using rad::is_invalid;
    using rad::is_valid;
    using rad::radius;
    using rad::size;
    using rad::width;

    HexBase ( ) noexcept                 = default;
    HexBase ( const HexBase & ) noexcept = default;
    HexBase ( HexBase && ) noexcept      = default;

    HexBase & operator= ( const HexBase & ) noexcept = default;
    HexBase & operator= ( HexBase && ) noexcept = default;

    ~HexBase ( ) noexcept = default;

    private:
    // From https://stackoverflow.com/a/40030044/646940, and fixed the missing constexpr std::swap (in C++17).

    static constexpr void emplace_valid_neighbor ( neighbors_type & n_, size_type const q_, size_type const r_ ) noexcept {
        if ( is_valid ( q_, r_ ) )
            n_.emplace_back ( index ( q_, r_ ) );
    }

    static constexpr void emplace_neighbors ( neighbors_type_array & na_, size_type const q_, size_type const r_ ) noexcept {
        neighbors_type & n = na_[ index ( q_, r_ ) ];
        emplace_valid_neighbor ( n, q_, r_ - 1 );
        emplace_valid_neighbor ( n, q_ + 1, r_ - 1 );
        emplace_valid_neighbor ( n, q_ - 1, r_ );
        emplace_valid_neighbor ( n, q_ + 1, r_ );
        emplace_valid_neighbor ( n, q_ - 1, r_ + 1 );
        emplace_valid_neighbor ( n, q_, r_ + 1 );
    }

    [[nodiscard]] static constexpr neighbors_type_array const make_neighbors_array ( ) noexcept {
        neighbors_type_array na;
        size_type q = center_idx ( ), r = center_idx ( );
        emplace_neighbors ( na, q, r );
        for ( size_type ring = 1; ring <= radius ( ); ++ring ) {
            ++q;                                   // Move to next ring, east.
            for ( size_type j = 0; j < ring; ++j ) // nw.
                emplace_neighbors ( na, q, --r );
            for ( size_type j = 0; j < ring; ++j ) // w.
                emplace_neighbors ( na, --q, r );
            for ( size_type j = 0; j < ring; ++j ) // sw.
                emplace_neighbors ( na, --q, ++r );
            for ( size_type j = 0; j < ring; ++j ) // se.
                emplace_neighbors ( na, q, ++r );
            for ( size_type j = 0; j < ring; ++j ) // e.
                emplace_neighbors ( na, ++q, r );
            for ( size_type j = 0; j < ring; ++j ) // ne.
                emplace_neighbors ( na, ++q, --r );
        }
        return na;
    }

    public:
    static constexpr neighbors_type_array const neighbors = make_neighbors_array ( );
};

template<typename Type, int R, bool zero_base>
struct HexContainer : public HexBase<R, zero_base> {

    using rad      = RadiusBase<R, zero_base>;
    using IdxType  = typename rad::IdxType;
    using Hex      = Hex<R, zero_base>;
    using hex_base = HexBase<R, zero_base>;

    using rad::center_idx;
    using rad::dw_cols;
    using rad::dw_rows;
    using rad::height;
    using rad::index;
    using rad::is_invalid;
    using rad::is_valid;
    using rad::radius;
    using rad::size;
    using rad::width;

    using hex_base::neighbors;

    using size_type     = typename rad::size_type;
    using value_type    = Type;
    using pointer       = value_type *;
    using const_pointer = value_type const *;

    using reference       = value_type &;
    using const_reference = value_type const &;
    using rv_reference    = value_type &&;

    using data_array  = std::array<value_type, rad::size ( )>;
    using color_array = std::array<sax::string_literal_t, size ( )>;

    using iterator       = typename data_array::iterator;
    using const_iterator = typename data_array::const_iterator;

    using const_neighbor_iterator = typename hex_base::const_iterator;

    static color_array color_codes; // Allows for thread_local.

    HexContainer ( ) noexcept                      = default;
    HexContainer ( HexContainer const & ) noexcept = default;
    HexContainer ( HexContainer && ) noexcept      = default;

    HexContainer & operator= ( HexContainer const & ) noexcept = default;
    HexContainer & operator= ( HexContainer && ) noexcept = default;

    ~HexContainer ( ) noexcept = default;

    void reset ( ) noexcept { std::fill ( std::execution::par_unseq, begin ( ), end ( ), value_type{ } ); }

    // Data access.

    [[nodiscard]] constexpr const_reference at ( size_type const q_, size_type const r_ ) const noexcept {
        return m_data[ index ( q_, r_ ) ];
    }
    [[nodiscard]] constexpr reference at ( size_type const q_, size_type const r_ ) noexcept {
        return const_cast<reference> ( std::as_const ( *this ).at ( q_, r_ ) );
    }

    [[nodiscard]] pointer data ( ) noexcept { return m_data.data ( ); }
    [[nodiscard]] const_pointer data ( ) const noexcept { return m_data.data ( ); }

    [[nodiscard]] iterator begin ( ) noexcept { return std::begin ( m_data ); }
    [[nodiscard]] const_iterator begin ( ) const noexcept { return std::cbegin ( m_data ); }
    [[nodiscard]] const_iterator cbegin ( ) const noexcept { return std::cbegin ( m_data ); }

    [[nodiscard]] iterator end ( ) noexcept { return std::end ( m_data ); }
    [[nodiscard]] const_iterator end ( ) const noexcept { return std::cend ( m_data ); }
    [[nodiscard]] const_iterator cend ( ) const noexcept { return std::cend ( m_data ); }

    [[nodiscard]] inline const_reference operator[] ( Hex const h_ ) const noexcept { return at ( h_ ); }
    [[nodiscard]] inline reference operator[] ( Hex const h_ ) noexcept {
        return const_cast<reference> ( std::as_const ( *this ).operator[] ( h_ ) );
    }

    [[nodiscard]] const_reference operator[] ( size_type const i_ ) const noexcept { return m_data[ i_ ]; }
    [[nodiscard]] reference operator[] ( size_type const i_ ) noexcept {
        return const_cast<reference> ( std::as_const ( *this ).operator[] ( i_ ) );
    }

    // Neighbors.

    [[nodiscard]] const_neighbor_iterator begin ( size_type const i_ ) const noexcept { return std::cbegin ( neighbors[ i_ ] ); }
    [[nodiscard]] const_neighbor_iterator end ( size_type const i_ ) const noexcept { return std::cend ( neighbors[ i_ ] ); }
    [[nodiscard]] const_neighbor_iterator begin ( size_type const q_, size_type const r_ ) const noexcept {
        return begin ( index ( q_, r_ ) );
    }
    [[nodiscard]] const_neighbor_iterator end ( size_type const q_, size_type const r_ ) const noexcept {
        return end ( index ( q_, r_ ) );
    }

    // Output.

    template<typename Stream>
    [[maybe_unused]] friend Stream & operator<< ( Stream & out_, HexContainer const & hc_ ) noexcept {
        constexpr int width = 1;
        size_type r         = 0;
        size_type i         = 0;
        const_pointer d     = hc_.m_data.data ( );
        for ( ; r <= radius ( ); ++r ) {
            out_ << std::setw ( width ) << ' ';
            for ( size_type s = 0; s < radius ( ) - r; ++s )
                out_ << std::setw ( width ) << ' ';
            for ( size_type q = 0; q <= radius ( ) + r; ++q, ++i ) {
                if ( color_codes[ i ] )
                    out_ << std::setw ( width ) << color_codes[ i ] << d[ i ] << sax::reset << ' ';
                else
                    out_ << std::setw ( width ) << d[ i ] << ' ';
            }
            out_ << nl;
        }
        --r;
        for ( ; r > size_type{ 0 }; --r ) {
            out_ << std::setw ( width ) << ' ';
            for ( size_type s = 0; s < radius ( ) - r + 1; ++s )
                out_ << std::setw ( width ) << ' ';
            for ( size_type q = 0; q < radius ( ) + r; ++q, ++i ) {
                if ( color_codes[ i ] )
                    out_ << std::setw ( width ) << color_codes[ i ] << d[ i ] << sax::reset << ' ';
                else
                    out_ << std::setw ( width ) << d[ i ] << ' ';
            }
            out_ << nl;
        }
        out_ << nl;
        return out_;
    }

    private:
    friend class cereal::access;

    template<class Archive>
    void serialize ( Archive & ar_ ) {
        ar_ ( m_data );
    }

    data_array m_data = { }; // value initialized, default is indeterminate.
};

template<typename Type, int R, bool zero_base>
typename HexContainer<Type, R, zero_base>::color_array HexContainer<Type, R, zero_base>::color_codes = { };
