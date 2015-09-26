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
// not_null
//
// Restricts a pointer or smart pointer to only hold non-null values.
// 
// Has zero size overhead over T.
//
// If T is a pointer (i.e. T == U*) then 
// - allow construction from U* or U& 
// - disallow construction from nullptr_t
// - disallow default construction
// - ensure construction from U* fails with nullptr
// - allow implicit conversion to U*
//
template<class T>
class not_null
{
public:
    not_null( T t ) : ptr_( t ) { ensure_invariant(); }

    not_null( not_null const & other ) : ptr_(other.ptr_) {}

#if gsl_HAVE_DEFAULT_FUNCTION_TEMPLATE_ARG
    template< typename U, typename Dummy = 
        typename std::enable_if<std::is_convertible<U, T>::value, void>::type > 
    not_null( not_null<U> const & other ) : ptr_( other.get() ) {}
#elif gsl_COMPILER_MSVC_VERSION != 6
    template< typename U > 
    not_null( not_null<U> const & other ) : ptr_( other.get() ) {}
#else
    // VC6 accepts it anyway.
#endif

private:
    // Prevent compilation when initialized with a nullptr or literal 0:
#if gsl_HAVE_NULLPTR
    not_null( std::nullptr_t );
#endif
    not_null( int );

public: 
    not_null<T> & operator=( T const & t ) { ptr_ = t; ensure_invariant(); return *this; }

#if gsl_HAVE_DEFAULT_FUNCTION_TEMPLATE_ARG
    template< typename U, typename Dummy = 
        typename std::enable_if<std::is_convertible<U, T>::value, void>::type > 
    not_null<T> & operator=( not_null<U> const & other ) { ptr_ = other.get(); ensure_invariant(); return *this; }
#elif gsl_COMPILER_MSVC_VERSION != 6
    template< typename U > 
    not_null<T> & operator=( not_null<U> const & other ) { ptr_ = other.get(); ensure_invariant(); return *this; }
#else
    // VC6 accepts it anyway.
#endif

private: 
    // Prevent compilation when someone attempts to assign a nullptr:
#if gsl_HAVE_NULLPTR
    not_null<T> & operator=( std::nullptr_t );
#endif
	not_null<T> & operator=( int );

public:
    T get() const 
    {
#if gsl_HAVE_NULLPTR && gsl_COMPILER_MSVC_VERSION > 0
        // The assume() should help the optimizer:
        __assume( ptr_ != nullptr );
#endif
        return ptr_;
    } 
    
    operator  T () const { return get(); }
    T operator->() const { return get(); }

	bool operator==(T const & rhs) const { return    ptr_ == rhs; }
	bool operator!=(T const & rhs) const { return !(*this == rhs); }

private:
    T ptr_;

    // We assume that the compiler can hoist/prove away most of the checks inlined 
    // from this function if not, we could make them optional via conditional compilation.
    void ensure_invariant() const { fail_fast_assert(ptr_ != NULL); }

    // Unwanted operators: pointers only point to single objects!
    // TODO ensure all arithmetic ops on this type are unavailable.
    not_null<T> & operator++();
    not_null<T> & operator--();
    not_null<T>   operator++( int );
    not_null<T>   operator--( int );
    not_null<T> & operator+ ( size_t );
    not_null<T> & operator+=( size_t );
    not_null<T> & operator- ( size_t );
    not_null<T> & operator-=( size_t );
};

} // namespace gsl

#endif // GSL_GSL_LITE_H_INCLUDED

// end of file
