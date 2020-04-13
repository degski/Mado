
// MIT License
//
// Copyright (c) 2018, 2019, 2020 degski
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

#include <cassert> // assert
#include <cstdint> // std::intptr_t
#include <cstddef> // std::size_t
#include <cstring> // std::memcpy

#include <array>
#include <type_traits>

namespace ma {

template<typename T, std::intptr_t I, std::intptr_t BaseI = 0,
         typename = std::enable_if_t<std::is_default_constructible<T>::value, T>>
class Vector {

    public:
    using value_type    = T;
    using pointer       = value_type *;
    using const_pointer = value_type const *;

    using reference       = value_type &;
    using const_reference = value_type const &;
    using rv_reference    = value_type &&;

    using size_type       = std::intptr_t;
    using difference_type = std::make_signed<size_type>;

    using iterator               = pointer;
    using const_iterator         = const_pointer;
    using reverse_iterator       = pointer;
    using const_reverse_iterator = const_pointer;

    private:
    T m_data[ I ];

    public:
    Vector ( ) : m_data{ T{} } {}
    Vector ( const Vector & v_ ) { std::memcpy ( m_data, v_.m_data, I * sizeof ( T ) ); }
    Vector ( Vector && v_ ) { std::memcpy ( m_data, v_.m_data, I * sizeof ( T ) ); }
    template<typename... Args>
    constexpr Vector ( Args... a_ ) : m_data{ a_... } {}

    typedef std::array<std::intptr_t, 1> extents_type;

    [[nodiscard]] const_iterator begin ( ) const noexcept { return const_iterator{ m_data }; }
    [[nodiscard]] const_iterator cbegin ( ) const noexcept { return begin ( ); }
    [[nodiscard]] iterator begin ( ) noexcept { return const_cast<iterator> ( std::as_const ( *this ).begin ( ) ); }

    [[nodiscard]] const_iterator end ( ) const noexcept { return const_iterator{ m_data + ( I ) }; }
    [[nodiscard]] const_iterator cend ( ) const noexcept { return end ( ); }
    [[nodiscard]] iterator end ( ) noexcept { return const_cast<iterator> ( std::as_const ( *this ).end ( ) ); }

    [[nodiscard]] const_iterator rbegin ( ) const noexcept { return const_iterator{ m_data + ( I - size_type{ 1 } ) }; }
    [[nodiscard]] const_iterator crbegin ( ) const noexcept { return rbegin ( ); }
    [[nodiscard]] iterator rbegin ( ) noexcept { return const_cast<iterator> ( std::as_const ( *this ).rbegin ( ) ); }

    [[nodiscard]] const_iterator rend ( ) const noexcept { return const_iterator{ m_data - size_type{ 1 } }; }
    [[nodiscard]] const_iterator crend ( ) const noexcept { return rend ( ); }
    [[nodiscard]] iterator rend ( ) noexcept { return const_cast<iterator> ( std::as_const ( *this ).rend ( ) ); }

    [[nodiscard]] value_type & operator( ) ( size_type i_ ) noexcept { return at ( i_ ); }
    [[nodiscard]] value_type operator( ) ( size_type i_ ) const noexcept { return at ( i_ ); }

    [[nodiscard]] T & at ( size_type const i_ ) noexcept {
        assert ( i_ >= BaseI );
        assert ( i_ < I + BaseI );
        return ( m_data - BaseI )[ i_ ];
    }

    [[nodiscard]] T at ( size_type const i_ ) const noexcept {
        assert ( i_ >= BaseI );
        assert ( i_ < I + BaseI );
        return ( m_data - BaseI )[ i_ ];
    }

    [[nodiscard]] T & at_r ( size_type const i_ ) noexcept {
        assert ( i_ >= BaseI );
        assert ( i_ < I + BaseI );
        return ( m_data + I - 1 + BaseI )[ -i_ ];
    }

    [[nodiscard]] T at_r ( size_type const i_ ) const noexcept {
        assert ( i_ >= BaseI );
        assert ( i_ < I + BaseI );
        return ( m_data + I - 1 + BaseI )[ -i_ ];
    }

    [[nodiscard]] pointer data ( ) noexcept { return m_data; }
    [[nodiscard]] const_pointer data ( ) const noexcept { return m_data; }

    [[nodiscard]] static constexpr std::size_t size ( ) noexcept { return I; }
    [[nodiscard]] static constexpr extents_type extents ( ) noexcept { return extents_type{ I }; }
};

template<typename T, std::intptr_t I, std::intptr_t J, std::intptr_t BaseI = 0, std::intptr_t BaseJ = 0,
         typename = std::enable_if_t<std::is_default_constructible<T>::value, T>>
class Matrix {

    public:
    using value_type    = T;
    using pointer       = value_type *;
    using const_pointer = value_type const *;

    using reference       = value_type &;
    using const_reference = value_type const &;
    using rv_reference    = value_type &&;

    using size_type       = std::intptr_t;
    using difference_type = std::make_signed<size_type>;

    using iterator               = pointer;
    using const_iterator         = const_pointer;
    using reverse_iterator       = pointer;
    using const_reverse_iterator = const_pointer;

    private:
    T m_data[ I * J ];

    public:
    Matrix ( ) : m_data{ T{} } {}
    Matrix ( const Matrix & m_ ) { std::memcpy ( m_data, m_.m_data, I * J * sizeof ( T ) ); }
    Matrix ( Matrix && m_ ) { std::memcpy ( m_data, m_.m_data, I * J * sizeof ( T ) ); }
    template<typename... Args>
    constexpr Matrix ( Args... a_ ) : m_data{ a_... } {}

    typedef std::array<std::intptr_t, 2> extents_type;

    [[nodiscard]] const_iterator begin ( ) const noexcept { return const_iterator{ m_data }; }
    [[nodiscard]] const_iterator cbegin ( ) const noexcept { return begin ( ); }
    [[nodiscard]] iterator begin ( ) noexcept { return const_cast<iterator> ( std::as_const ( *this ).begin ( ) ); }

    [[nodiscard]] const_iterator end ( ) const noexcept {
        assert ( m_data );
        return const_iterator{ m_data + ( I * J ) };
    }
    [[nodiscard]] const_iterator cend ( ) const noexcept { return end ( ); }
    [[nodiscard]] iterator end ( ) noexcept { return const_cast<iterator> ( std::as_const ( *this ).end ( ) ); }

    [[nodiscard]] const_iterator rbegin ( ) const noexcept {
        assert ( m_data );
        return const_iterator{ m_data + ( I * J - size_type{ 1 } ) };
    }
    [[nodiscard]] const_iterator crbegin ( ) const noexcept { return rbegin ( ); }
    [[nodiscard]] iterator rbegin ( ) noexcept { return const_cast<iterator> ( std::as_const ( *this ).rbegin ( ) ); }

    [[nodiscard]] const_iterator rend ( ) const noexcept {
        assert ( m_data );
        return const_iterator{ m_data - size_type{ 1 } };
    }
    [[nodiscard]] const_iterator crend ( ) const noexcept { return rend ( ); }
    [[nodiscard]] iterator rend ( ) noexcept { return const_cast<iterator> ( std::as_const ( *this ).rend ( ) ); }

    [[nodiscard]] value_type & operator( ) ( size_type i_, size_type j_ ) noexcept { return at ( i_, j_ ); }
    [[nodiscard]] value_type operator( ) ( size_type i_, size_type j_ ) const noexcept { return at ( i_, j_ ); }

    [[nodiscard]] T const & ref ( size_type const i_, size_type const j_ ) const noexcept {
        assert ( i_ >= BaseI );
        assert ( i_ < I + BaseI );
        assert ( j_ >= BaseJ );
        assert ( j_ < J + BaseJ );
        return ( m_data - BaseJ - BaseI * J )[ j_ + i_ * J ];
    }

    [[nodiscard]] T & at ( size_type const i_, size_type const j_ ) noexcept {
        assert ( i_ >= BaseI );
        assert ( i_ < I + BaseI );
        assert ( j_ >= BaseJ );
        assert ( j_ < J + BaseJ );
        return ( m_data - BaseJ - BaseI * J )[ j_ + i_ * J ];
    }

    [[nodiscard]] T at ( size_type const i_, size_type const j_ ) const noexcept {
        assert ( i_ >= BaseI );
        assert ( i_ < I + BaseI );
        assert ( j_ >= BaseJ );
        assert ( j_ < J + BaseJ );
        return ( m_data - BaseJ - BaseI * J )[ j_ + i_ * J ];
    }

    // Mirror the matrix coordinates.
    [[nodiscard]] T const & ref_r ( size_type const i_, size_type const j_ ) noexcept {
        assert ( i_ >= BaseI );
        assert ( i_ < I + BaseI );
        assert ( j_ >= BaseJ );
        assert ( j_ < J + BaseJ );
        return ( m_data + I * J - 1 + BaseJ + BaseI * J )[ -j_ - i_ * J ];
    }

    // Mirror the matrix coordinates.
    [[nodiscard]] T & at_r ( size_type const i_, size_type const j_ ) noexcept {
        assert ( i_ >= BaseI );
        assert ( i_ < I + BaseI );
        assert ( j_ >= BaseJ );
        assert ( j_ < J + BaseJ );
        return ( m_data + I * J - 1 + BaseJ + BaseI * J )[ -j_ - i_ * J ];
    }

    // Mirror the matrix coordinates.
    [[nodiscard]] T at_r ( size_type const i_, size_type const j_ ) const noexcept {
        assert ( i_ >= BaseI );
        assert ( i_ < I + BaseI );
        assert ( j_ >= BaseJ );
        assert ( j_ < J + BaseJ );
        return ( m_data + I * J - 1 + BaseJ + BaseI * J )[ -j_ - i_ * J ];
    }

    [[nodiscard]] pointer data ( ) noexcept { return m_data; }
    [[nodiscard]] const_pointer data ( ) const noexcept { return m_data; }

    [[nodiscard]] static constexpr std::size_t size ( ) noexcept { return I * J; }
    [[nodiscard]] static constexpr extents_type extents ( ) noexcept { return extents_type{ I, J }; }
};

template<typename T, std::intptr_t I, std::intptr_t J, std::intptr_t BaseI = 0, std::intptr_t BaseJ = 0,
         typename = std::enable_if_t<std::is_default_constructible<T>::value, T>>
using MatrixRM = Matrix<T, I, J, BaseI, BaseJ>;

template<typename T, std::intptr_t J, std::intptr_t I, std::intptr_t BaseJ = 0, std::intptr_t BaseI = 0,
         typename = std::enable_if_t<std::is_default_constructible<T>::value, T>>
using MatrixCM = Matrix<T, J, I, BaseJ, BaseI>;

template<typename T, std::intptr_t I, std::intptr_t J, std::intptr_t K, std::intptr_t BaseI = 0, std::intptr_t BaseJ = 0,
         std::intptr_t BaseK = 0, typename = std::enable_if_t<std::is_default_constructible<T>::value, T>>
class Cube {

    public:
    using value_type    = T;
    using pointer       = value_type *;
    using const_pointer = value_type const *;

    using reference       = value_type &;
    using const_reference = value_type const &;
    using rv_reference    = value_type &&;

    using size_type       = std::intptr_t;
    using difference_type = std::make_signed<size_type>;

    using iterator               = pointer;
    using const_iterator         = const_pointer;
    using reverse_iterator       = pointer;
    using const_reverse_iterator = const_pointer;

    private:
    T m_data[ I * J * K ];

    public:
    Cube ( ) : m_data{ T{} } {}
    Cube ( Cube const & c_ ) { std::memcpy ( m_data, c_.m_data, I * J * K * sizeof ( T ) ); }
    Cube ( Cube && c_ ) { std::memcpy ( m_data, c_.m_data, I * J * K * sizeof ( T ) ); }
    template<typename... Args>
    constexpr Cube ( Args... a_ ) : m_data{ a_... } {}

    typedef std::array<std::intptr_t, 3> extents_type;

    [[nodiscard]] const_iterator begin ( ) const noexcept {
        assert ( m_data );
        return const_iterator{ m_data };
    }
    [[nodiscard]] const_iterator cbegin ( ) const noexcept { return begin ( ); }
    [[nodiscard]] iterator begin ( ) noexcept { return const_cast<iterator> ( std::as_const ( *this ).begin ( ) ); }

    [[nodiscard]] const_iterator end ( ) const noexcept {
        assert ( m_data );
        return const_iterator{ m_data + ( I * J * K ) };
    }
    [[nodiscard]] const_iterator cend ( ) const noexcept { return end ( ); }
    [[nodiscard]] iterator end ( ) noexcept { return const_cast<iterator> ( std::as_const ( *this ).end ( ) ); }

    [[nodiscard]] const_iterator rbegin ( ) const noexcept {
        assert ( m_data );
        return const_iterator{ m_data + ( I * J * K - size_type{ 1 } ) };
    }
    [[nodiscard]] const_iterator crbegin ( ) const noexcept { return rbegin ( ); }
    [[nodiscard]] iterator rbegin ( ) noexcept { return const_cast<iterator> ( std::as_const ( *this ).rbegin ( ) ); }

    [[nodiscard]] const_iterator rend ( ) const noexcept {
        assert ( m_data );
        return const_iterator{ m_data - size_type{ 1 } };
    }
    [[nodiscard]] const_iterator crend ( ) const noexcept { return rend ( ); }
    [[nodiscard]] iterator rend ( ) noexcept { return const_cast<iterator> ( std::as_const ( *this ).rend ( ) ); }

    [[nodiscard]] value_type & operator( ) ( size_type i_, size_type j_, size_type k_ ) noexcept { return at ( i_, j_, k_ ); }
    [[nodiscard]] value_type operator( ) ( size_type i_, size_type j_, size_type k_ ) const noexcept { return at ( i_, j_, k_ ); }

    [[nodiscard]] T & at ( size_type const i_, size_type const j_, size_type const k_ ) noexcept {
        assert ( i_ >= BaseI && i_ < I + BaseI );
        assert ( j_ >= BaseJ && j_ < J + BaseJ );
        assert ( k_ >= BaseK && k_ < K + BaseK );
        return ( m_data + K * ( -BaseJ - BaseI * J ) - BaseK )[ K * ( j_ + i_ * J ) + k_ ];
    }

    [[nodiscard]] T at ( size_type const i_, size_type const j_, size_type const k_ ) const noexcept {
        assert ( i_ >= BaseI && i_ < I + BaseI );
        assert ( j_ >= BaseJ && j_ < J + BaseJ );
        assert ( k_ >= BaseK && k_ < K + BaseK );
        return ( m_data + K * ( -BaseJ - BaseI * J ) - BaseK )[ K * ( j_ + i_ * J ) + k_ ];
    }

    [[nodiscard]] T & at_r ( size_type const i_, size_type const j_, size_type const k_ ) noexcept {
        assert ( i_ >= BaseI && i_ < I + BaseI );
        assert ( j_ >= BaseJ && j_ < J + BaseJ );
        assert ( k_ >= BaseK && k_ < K + BaseK );
        return ( m_data + I * J * K - 1 + BaseJ * K + BaseI * J * K + BaseK )[ K * ( -j_ - i_ * J ) - k_ ];
    }

    [[nodiscard]] T at_r ( size_type const i_, size_type const j_, size_type const k_ ) const noexcept {
        assert ( i_ >= BaseI && i_ < I + BaseI );
        assert ( j_ >= BaseJ && j_ < J + BaseJ );
        assert ( k_ >= BaseK && k_ < K + BaseK );
        return ( m_data + I * J * K - 1 + BaseJ * K + BaseI * J * K + BaseK )[ K * ( -j_ - i_ * J ) - k_ ];
    }

    [[nodiscard]] pointer data ( ) noexcept { return m_data; }
    [[nodiscard]] const_pointer data ( ) const noexcept { return m_data; }

    [[nodiscard]] static constexpr std::size_t size ( ) noexcept { return I * J * K; }
    [[nodiscard]] static constexpr extents_type extents ( ) noexcept { return extents_type{ I, J, K }; }
};

template<typename T, std::intptr_t I, std::intptr_t J, std::intptr_t K, std::intptr_t L, std::intptr_t BaseI = 0,
         std::intptr_t BaseJ = 0, std::intptr_t BaseK = 0, std::intptr_t BaseL = 0,
         typename = std::enable_if_t<std::is_default_constructible<T>::value, T>>
class HyperCube {

    public:
    using value_type    = T;
    using pointer       = value_type *;
    using const_pointer = value_type const *;

    using reference       = value_type &;
    using const_reference = value_type const &;
    using rv_reference    = value_type &&;

    using size_type       = std::intptr_t;
    using difference_type = std::make_signed<size_type>;

    using iterator               = pointer;
    using const_iterator         = const_pointer;
    using reverse_iterator       = pointer;
    using const_reverse_iterator = const_pointer;

    private:
    T m_data[ I * J * K * L ];

    public:
    HyperCube ( ) noexcept : m_data{ T{} } {}
    HyperCube ( HyperCube const & h_ ) { std::memcpy ( m_data, h_.m_data, I * J * K * L * sizeof ( T ) ); }
    HyperCube ( HyperCube && h_ ) { std::memcpy ( m_data, h_.m_data, I * J * K * L * sizeof ( T ) ); }
    template<typename... Args>
    constexpr HyperCube ( Args... a_ ) : m_data{ a_... } {}

    typedef std::array<std::intptr_t, 4> extents_type;

    [[nodiscard]] const_iterator begin ( ) const noexcept {
        assert ( m_data );
        return const_iterator{ m_data };
    }
    [[nodiscard]] const_iterator cbegin ( ) const noexcept { return begin ( ); }
    [[nodiscard]] iterator begin ( ) noexcept { return const_cast<iterator> ( std::as_const ( *this ).begin ( ) ); }

    [[nodiscard]] const_iterator end ( ) const noexcept {
        assert ( m_data );
        return const_iterator{ m_data + ( I * J * K * L ) };
    }
    [[nodiscard]] const_iterator cend ( ) const noexcept { return end ( ); }
    [[nodiscard]] iterator end ( ) noexcept { return const_cast<iterator> ( std::as_const ( *this ).end ( ) ); }

    [[nodiscard]] const_iterator rbegin ( ) const noexcept {
        assert ( m_data );
        return const_iterator{ m_data + ( I * J * K * L - size_type{ 1 } ) };
    }
    [[nodiscard]] const_iterator crbegin ( ) const noexcept { return rbegin ( ); }
    [[nodiscard]] iterator rbegin ( ) noexcept { return const_cast<iterator> ( std::as_const ( *this ).rbegin ( ) ); }

    [[nodiscard]] const_iterator rend ( ) const noexcept {
        assert ( m_data );
        return const_iterator{ m_data - size_type{ 1 } };
    }
    [[nodiscard]] const_iterator crend ( ) const noexcept { return rend ( ); }
    [[nodiscard]] iterator rend ( ) noexcept { return const_cast<iterator> ( std::as_const ( *this ).rend ( ) ); }

    [[nodiscard]] value_type & operator( ) ( size_type i_, size_type j_, size_type k_, size_type l_ ) noexcept {
        return at ( i_, j_, k_, l_ );
    }
    [[nodiscard]] value_type operator( ) ( size_type i_, size_type j_, size_type k_, size_type l_ ) const noexcept {
        return at ( i_, j_, k_, l_ );
    }

    [[nodiscard]] T & at ( size_type const i_, size_type const j_, size_type const k_, size_type const l_ ) noexcept {
        assert ( i_ >= BaseI && i_ < I + BaseI );
        assert ( j_ >= BaseJ && j_ < J + BaseJ );
        assert ( k_ >= BaseK && k_ < K + BaseK );
        assert ( l_ >= BaseL && l_ < L + BaseL );
        return ( m_data + L * ( K * ( -BaseJ - BaseI * J ) - BaseK ) - BaseL )[ L * ( K * ( j_ + i_ * J ) + k_ ) + l_ ];
    }

    [[nodiscard]] T at ( size_type const i_, size_type const j_, size_type const k_, size_type const l_ ) const noexcept {
        assert ( i_ >= BaseI && i_ < I + BaseI );
        assert ( j_ >= BaseJ && j_ < J + BaseJ );
        assert ( k_ >= BaseK && k_ < K + BaseK );
        assert ( l_ >= BaseL && l_ < L + BaseL );
        return ( m_data + L * ( K * ( -BaseJ - BaseI * J ) - BaseK ) - BaseL )[ L * ( K * ( j_ + i_ * J ) + k_ ) + l_ ];
    }

    [[nodiscard]] pointer data ( ) noexcept { return m_data; }
    [[nodiscard]] const_pointer data ( ) const noexcept { return m_data; }

    [[nodiscard]] static constexpr std::size_t size ( ) noexcept { return I * J * K * L; }
    [[nodiscard]] static constexpr extents_type extents ( ) noexcept { return extents_type{ I, J, K, L }; }
};

}; // namespace ma
