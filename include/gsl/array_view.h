//
// gsl-lite is based on GSL: Guidelines Support Library,
// https://github.com/microsoft/gsl
//
// Copyright (c) 2015 Martin Moene
// Copyright (c) 2015 Microsoft Corporation. All rights reserved. 
// 
// This code is licensed under the MIT License (MIT). 
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
// THE SOFTWARE. 

#pragma once

#ifndef GSL_ARRAY_VIEW_H_INCLUDED
#define GSL_ARRAY_VIEW_H_INCLUDED

#include <iterator>
#include <utility>

namespace gsl {

#if gsl_HAVE_ENUM_CLASS
# include <cstdint>
  enum class byte : std::uint8_t {};
#elif gsl_HAVE_SIZED_TYPES
# include <cstdint>
  typedef std::uint8_t byte;
#else
  typedef unsigned char byte;
#endif

template< class T >
class array_view
{
public:
    typedef size_t size_type;

    typedef T value_type;
    typedef T & reference;
    typedef T * pointer;
    typedef T const * const_pointer;

    typedef pointer       iterator;
    typedef const_pointer const_iterator;
    
#if gsl_BETWEEN( gsl_COMPILER_MSVC_VERSION, 6, 7 )
    typedef std::reverse_iterator< iterator, T >             reverse_iterator;
    typedef std::reverse_iterator< const_iterator, const T > const_reverse_iterator;
#else
    typedef std::reverse_iterator< iterator >                reverse_iterator;
    typedef std::reverse_iterator< const_iterator >          const_reverse_iterator;
#endif

#if gsl_COMPILER_MSVC_VERSION == 6
    // Todo
#else
    typedef typename std::iterator_traits< iterator >::difference_type difference_type;    
#endif

    gsl_constexpr14 array_view()
        : begin_( NULL )
        , end_  ( NULL )
    {
        Expects( size() == 0 );
    }

#if gsl_HAVE_NULLPTR
    gsl_constexpr14 array_view( std::nullptr_t, size_type size )
        : begin_( nullptr )
        , end_  ( nullptr )
    {
        Expects( size == 0 );
    }
#endif    

    gsl_constexpr14 array_view( pointer begin, pointer end )
        : begin_( begin )
        , end_  ( end )
    {
        Expects( begin <= end );
    }

    gsl_constexpr14 array_view( pointer begin, size_type size )
        : begin_( begin )
        , end_  ( begin + size )
    {
        Expects( size == 0 || ( size > 0 && begin != NULL ) );
    }

#if gsl_COMPILER_MSVC_VERSION != 6

    template< size_t N >
    gsl_constexpr14 array_view( T (&arr)[N] )
        : begin_( arr )
        , end_  ( arr + N )
    {}

# if gsl_HAVE_ARRAY
    template< size_t N >
    gsl_constexpr14 array_view( std::array< T, N > & arr ) 
        : begin_( arr.data() )
        , end_  ( arr.data() + N )
    {}
# endif

    template< class Cont >
    gsl_constexpr14 array_view( Cont & cont ) 
#if gsl_HAVE_CONTAINER_DATA_METHOD
        : begin_( cont.data() )
        , end_  ( cont.data() + cont.size() )
#else
        : begin_( &cont[0] )
        , end_  ( &cont[0] + cont.size() )
#endif
    {}

#else // gsl_COMPILER_MSVC_VERSION != 6

private:
    struct lower_precedence {};
    struct higher_precedence : lower_precedence {};
    
    template< class Array >
    array_view create( Array & arr, T*, lower_precedence const & ) const
    { 
        return array_view( arr, gsl_DIMENSION_OF( arr ) );
    }

    array_view create( std::vector<T> & cont, T*, higher_precedence const & ) const
    {
        return array_view( &cont[0], cont.size() );
    }

public:    
    template< class Cont >
    array_view( Cont & cont )
    { 
        *this = create( cont, &cont[0], higher_precedence() );
    }
#endif // gsl_COMPILER_MSVC_VERSION != 6


#if gsl_HAVE_IS_DEFAULT_CTOR
    gsl_constexpr14 array_view( array_view const & ) = default;
#else
    // default
#endif

#if gsl_COMPILER_MSVC_VERSION != 6
    array_view & operator=( array_view other )
    {
        other.swap( *this ); 
        return *this;
    }
#else
    array_view & operator=( array_view const & other )
    {
        // VC6 balks at copy-swap implementation (here),
        // so we do it the simple way:
        begin_ = other.begin_;
        end_   = other.end_;
        return *this;
    }
#endif

#if 0
    // Converting from other array_view ?    
    template< typename U > operator=();
#endif

    gsl_constexpr14 iterator begin() const
    {
        return iterator( begin_ );
    }

    gsl_constexpr14 iterator end() const
    {
        return iterator( end_ );
    }

    gsl_constexpr14 const_iterator cbegin() const
    {
        return const_iterator( begin() );
    }

    gsl_constexpr14 const_iterator cend() const
    {
        return const_iterator( end() );
    }

    gsl_constexpr14 reverse_iterator rbegin() const
    {
        return reverse_iterator( end() );
    }

    gsl_constexpr14 reverse_iterator rend() const
    {
        return reverse_iterator( begin() );
    }

    gsl_constexpr14 const_reverse_iterator crbegin() const
    {
        return const_reverse_iterator( cend() );
    }

    gsl_constexpr14 const_reverse_iterator crend() const
    {
        return const_reverse_iterator( cbegin() );
    }

    gsl_constexpr14 operator bool () const gsl_noexcept
    {
        return begin_ != NULL;
    }

    gsl_constexpr14 reference operator[]( size_t index )
    {
        return at( index );
    }

    gsl_constexpr14 bool operator==( array_view const & other ) const
    {
        return  size() == other.size() 
            && (begin_ == other.begin_ || std::equal( this->begin(), this->end(), other.begin() ) );	    
    }

    gsl_constexpr14 bool operator!=( array_view const & other ) const gsl_noexcept 
    { 
        return !( *this == other ); 
    }

    gsl_constexpr14 bool operator< ( array_view const & other ) const gsl_noexcept
    { 
        return std::lexicographical_compare( this->begin(), this->end(), other.begin(), other.end() ); 
    }

    gsl_constexpr14 bool operator<=( array_view const & other ) const gsl_noexcept
    { 
        return !( other < *this ); 
    }

    gsl_constexpr14 bool operator> ( array_view const & other ) const gsl_noexcept
    { 
        return ( other < *this ); 
    }

    gsl_constexpr14 bool operator>=( array_view const & other ) const gsl_noexcept
    {
        return !( *this < other );
    }

    gsl_constexpr14 reference at( size_t index )
    {
        Expects( index >= 0 && index < size());
        return begin_[ index ];
    }

    gsl_constexpr14 pointer data() const gsl_noexcept
    {
        return begin_;
    }

    gsl_constexpr14 bool empty() const gsl_noexcept
    {
        return size() == 0;
    }

    gsl_constexpr14 size_type size() const gsl_noexcept
    {
        return std::distance( begin_, end_ );
    }

    gsl_constexpr14 size_type length() const gsl_noexcept
    {
        return size();
    }

    gsl_constexpr14 size_type used_length() const gsl_noexcept
    {
        return length();
    }

    gsl_constexpr14 size_type bytes() const gsl_noexcept
    {
        return sizeof( value_type ) * size();
    }

    gsl_constexpr14 size_type used_bytes() const gsl_noexcept
    {
        return bytes();
    }

    void swap( array_view & other ) gsl_noexcept
    {
        using std::swap;
        swap( begin_, other.begin_ );
        swap( end_  , other.end_   );
    }

    array_view< const byte > as_bytes() const gsl_noexcept
    {
        return array_view< const byte >( reinterpret_cast<const byte *>( data() ), bytes() );
    }

    array_view< byte > as_writeable_bytes() const gsl_noexcept
    {
        return array_view< byte >( reinterpret_cast<byte *>( data() ), bytes() );
    }

    template< typename U >
#if gsl_COMPILER_MSVC_VERSION == 6
    array_view< U > as_array_view( U u = U() ) const gsl_noexcept
#else
    array_view< U > as_array_view() const gsl_noexcept
#endif
    {
        return array_view< U >( reinterpret_cast<U *>( this->data() ), this->bytes() / sizeof( U ) );
    }

private:
    pointer begin_;
    pointer end_;
};

// make functions (see ctors)
# if gsl_HAVE_NULLPTR
template< typename T >
gsl_constexpr14 array_view<T> as_array_view( std::nullptr_t, size_t size )
{
    return array_view<T>( nullptr, size );
}
#endif

template< typename T >
gsl_constexpr14 array_view<T> as_array_view( T * begin, T * end )
{
    return array_view<T>( begin, end );
}

template< typename T >
gsl_constexpr14 array_view<T> as_array_view( T * begin, size_t size )
{
    return array_view<T>( begin, size );
}

#if gsl_COMPILER_MSVC_VERSION != 6

template< typename T, size_t N >
gsl_constexpr14 array_view<T> as_array_view( T (&arr)[N] )
{ 
    return array_view<T>( arr, N );
}

# if gsl_HAVE_ARRAY
template< typename T, size_t N >
gsl_constexpr14 array_view<T> as_array_view( std::array<T,N> & arr )
{
    return array_view<T>( arr );
}
# endif

# if gsl_HAVE_AUTO
template< class Cont >
auto as_array_view( Cont & cont ) -> gsl_constexpr14 array_view< typename Cont::value_type > 
{ 
    return array_view< typename Cont::value_type >( cont );
}
# else
template< class T >
array_view<T> as_array_view( std::vector<T> & cont )
{ 
    return array_view<T>( cont );
}
#endif

#else // gsl_COMPILER_MSVC_VERSION != 6

template< class T >
array_view<T> as_array_view( std::vector<T> & cont )
{ 
    return array_view<T>( cont );
}

#endif // gsl_COMPILER_MSVC_VERSION != 6

} // namespace gsl

#endif // GSL_ARRAY_VIEW_H_INCLUDED

// end of file
