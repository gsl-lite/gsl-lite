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

#ifndef GSL_GSL_LITE_H_INCLUDED
#define GSL_GSL_LITE_H_INCLUDED

//#include "array_view.h"     // array_view, strided_array_view...
//#include "string_view.h"    // zstring, string_view, zstring_builder...
#include "fail_fast.h"
#include <memory>

#if gsl_HAVE_ARRAY
# include <array>
#endif

namespace gsl {

//
// GSL.owner: ownership pointers 
//
#if gsl_HAVE_SHARED_PTR
  using std::unique_ptr;
  using std::shared_ptr;
#endif

#if gsl_HAVE_ALIAS_TEMPLATE
  template< class T > using owner = T;
#else
  template< class T > struct owner { typedef T type; };
#endif

#define gsl_HAVE_OWNER_TEMPLATE  gsl_HAVE_ALIAS_TEMPLATE

#if gsl_FEATURE_HAVE_OWNER_MACRO
# if gsl_HAVE_OWNER_TEMPLATE 
#  define Owner(t)  ::gsl::owner<t>
# else
#  define Owner(t)  ::gsl::owner<t>::type
# endif
#endif 

//
// GSL.assert: assertions
//
#define Expects(x)  ::gsl::fail_fast_assert((x))
#define Ensures(x)  ::gsl::fail_fast_assert((x))

//
// GSL.util: utilities
//

#if gsl_CPP11_OR_GREATER

template< class Fn >
class Final_act
{
public:
    explicit Final_act( Fn action ) 
    : action_( std::move( action ) ) {}
    
    ~Final_act() { action_(); }

private:
    Fn action_;
};

template< class Fn >
Final_act<Fn> finally( Fn const & action ) 
{ 
    return Final_act<Fn>( action ); 
}

template< class Fn >
Final_act<Fn> finally( Fn && action )
{ 
    return Final_act<Fn>( std::forward<Fn>( action ) ); 
}

#else // gsl_CPP11_OR_GREATER

class Final_act
{
public:
    typedef void (*Action)();

    Final_act( Action action ) 
    : action_( action ) {}

    ~Final_act() 
    {
        action_();
    }

private:    
    Action action_;
};

template< class Fn >
Final_act finally( Fn const & f )
{
    return Final_act(( f ));
}

#endif // gsl_CPP11_OR_GREATER

template< class T, class U >
T narrow_cast( U u ) 
{ 
    return static_cast<T>( u ); 
}

struct narrowing_error : public std::exception {};

template< class T, class U >
T narrow( U u ) 
{ 
    T t = narrow_cast<T>( u ); 
    
    if ( static_cast<U>( t ) != u ) 
    {
        throw narrowing_error(); 
    }
    return t; 
}

//
// GSL.views: views
//

typedef char * zstring;
typedef wchar_t * zwstring;
typedef const char * czstring;
typedef const wchar_t * cwzstring;

#if gsl_COMPILER_MSVC_VERSION != 6

template< class T, size_t N >
T & at( T(&arr)[N], size_t index ) 
{ 
    fail_fast_assert( index < N ); 
    return arr[index]; 
}

# if gsl_HAVE_ARRAY

template< class T, size_t N >
T & at( std::array<T, N> & arr, size_t index ) 
{ 
    fail_fast_assert( index < N ); 
    return arr[index]; 
}
# endif

template< class Cont >
typename Cont::value_type & at( Cont & cont, size_t index ) 
{ 
    fail_fast_assert( index < cont.size() ); 
    return cont[index]; 
}

#else // gsl_COMPILER_MSVC_VERSION != 6

namespace detail {

struct lower_precedence {};
struct higher_precedence : lower_precedence {};

template< class Array, class T >
T & at( Array & arr, size_t index, T*, lower_precedence const & )
{ 
    fail_fast_assert( index < gsl_DIMENSION_OF( arr ) ); 
    return arr[index]; 
}

# define gsl_MK_AT( Cont ) \
    namespace gsl { namespace detail { \
    template< class T > \
    inline T & at( Cont<T> & cont, size_t index, T*, higher_precedence const & ) \
    { \
        fail_fast_assert( index < cont.size() ); \
        return cont[index]; \
    } }} 

} // namespace detail

template< class Cont >
int & at( Cont & cont, size_t index )
{ 
    return detail::at( cont, index, &cont[0], detail::higher_precedence() );
}

#endif // gsl_COMPILER_MSVC_VERSION != 6

template<class T>
class not_null
{
public:
    not_null(             T t         ) : ptr_ ( t ){ Expects( ptr_ != NULL ); }
    not_null & operator=( T const & t ) { ptr_ = t ;  Expects( ptr_ != NULL ); return *this; }

    not_null(             not_null const & other ) : ptr_ ( other.ptr_  ) {}
    not_null & operator=( not_null const & other ) { ptr_ = other.ptr_; }

#if gsl_HAVE_DEFAULT_FUNCTION_TEMPLATE_ARG
    template< typename U, typename Dummy = 
        typename std::enable_if<std::is_convertible<U, T>::value, void>::type > 
    not_null( not_null<U> const & other ) : ptr_( other.get() ) {}

    template< typename U, typename Dummy = 
        typename std::enable_if<std::is_convertible<U, T>::value, void>::type > 
    not_null & operator=( not_null<U> const & other ) 
    { 
        ptr_ = other.get(); 
        return *this; 
    }
#elif gsl_COMPILER_MSVC_VERSION != 6
    template< typename U > 
    not_null( not_null<U> const & other ) : ptr_( other.get() ) {}

    template< typename U > 
    not_null & operator=( not_null<U> const & other ) 
    { 
        ptr_ = other.get(); 
        return *this; 
    }
#else
    // VC6 accepts it anyway.
#endif

private:
    // Prevent compilation when initialized with a nullptr or literal 0:
#if gsl_HAVE_NULLPTR
    not_null(             std::nullptr_t );
    not_null & operator=( std::nullptr_t );
#endif
    not_null(             int );
	not_null & operator=( int );

public:
    T get() const 
    {
#if gsl_HAVE_NULLPTR && gsl_COMPILER_MSVC_VERSION > 0
        // The assume() should help the optimizer:
        __assume( ptr_ != nullptr );
#endif
        return ptr_;
    } 
    
    gsl_explicit operator T() const { return get(); }
    T            operator->() const { return get(); }

	bool operator==(T const & rhs) const { return    ptr_ == rhs; }
	bool operator!=(T const & rhs) const { return !(*this == rhs); }

private:
    T ptr_;

    not_null & operator++();
    not_null & operator--();
    not_null   operator++( int );
    not_null   operator--( int );
    not_null & operator+ ( size_t );
    not_null & operator+=( size_t );
    not_null & operator- ( size_t );
    not_null & operator-=( size_t );
};

} // namespace gsl

#endif // GSL_GSL_LITE_H_INCLUDED

// end of file
