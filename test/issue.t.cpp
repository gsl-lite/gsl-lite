﻿//
// gsl-lite is based on GSL: Guidelines Support Library.
// For more information see https://github.com/gsl-lite/gsl-lite
//
// Copyright (c) 2015-2019 Martin Moene
// Copyright (c) 2019-2021 Moritz Beutel
// Copyright (c) 2015-2018 Microsoft Corporation. All rights reserved.
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

#include "gsl-lite.t.hpp"

using namespace gsl_lite;

CASE( "span<>: free comparation functions fail for different const-ness [issue #32]" )
{
#if gsl_FEATURE( STRING_SPAN )
# if gsl_FEATURE_TO_STD( MAKE_SPAN )
#  if gsl_CONFIG( ALLOWS_NONSTRICT_SPAN_COMPARISON )
    char data[] = { 'a', 'b' };
    string_span  a = make_span( data );
    cstring_span b = make_span( data ).last( 1 );

    // worked as expected before 0.9.0, in 0.9.[0,1] converts to bool and compares equal!

    EXPECT( a != b );
#  else
    EXPECT( !!"span<>: cannot compare different types (gsl_CONFIG_ALLOWS_NONSTRICT_SPAN_COMPARISON=0)" );
#  endif
# else
    EXPECT( !!"span<>: make_span() is not available (gsl_FEATURE_MAKE_SPAN_TO_STD)" );
# endif
#endif // gsl_FEATURE( STRING_SPAN )
}

CASE( "span<>: constrained container constructor suffers hard failure for arguments with reference-returning data() function [issue #242]" )
{
#if gsl_FEATURE( BYTE )&& gsl_HAVE( CONSTRAINED_SPAN_CONTAINER_CTOR ) && gsl_HAVE( TYPE_TRAITS )
    struct S
    {
        int data_{ };

        explicit S( gsl_lite::span<gsl_lite::byte const> ) { }
        int const & data() const { return data_; }
    };

    // S is not a `contiguous_range`, hence the range constructor should not be instantiable, but this needs to be a substitution
    // failure, not a hard error.
    EXPECT( !(std::is_constructible< gsl_lite::span< gsl_lite::byte const >, S >::value) );
#else
    EXPECT( !!"span<>: constrained container constructor is not available (gsl_HAVE_CONSTRAINED_SPAN_CONTAINER_CTOR=0)" );
#endif
}

#if gsl_COMPILER_MSVC_VERSION
# pragma warning( push )
# pragma warning( disable : 4127 ) // conditional expression is constant
#endif

CASE( "byte: aliasing rules lead to undefined behaviour when using enum class [issue #34](GSL issue #313, PR #390)" )
{
#if gsl_FEATURE( BYTE ) && gsl_HAVE( ENUM_CLASS )
    struct F {
        static int f( int & i, gsl_lite::byte & r )
        {
           i = 7;
           r <<= 1;
           return i;
        }
    };

    int i = 0;
    if ( std20::endian::native == std20::endian::little )
    {
        EXPECT( 14 == F::f( i, reinterpret_cast<gsl_lite::byte*>( &i )[0] ) );
    }
    else if ( std20::endian::native == std20::endian::big )
    {
        EXPECT( 14 == F::f( i, reinterpret_cast<gsl_lite::byte*>( &i )[sizeof i - 1] ) );
    }
#endif // gsl_FEATURE( BYTE ) && gsl_HAVE( ENUM_CLASS )
}

#if gsl_COMPILER_MSVC_VERSION
# pragma warning( pop )
#endif

CASE( "string_span<>: must not include terminating '\\0' [issue #53]" )
{
#if gsl_FEATURE( STRING_SPAN )
    char const data[] = "ab";
    char const * text = "ab";
    cstring_span span = "ab";

    cstring_span a( data );
    cstring_span b( ensure_z( text ) );

    // may include terminating '\0'
    // works differently from M-GSL and basic_string_span proposal:

    EXPECT( span.length() == 2u );
    EXPECT(    a.length() == 2u );
    EXPECT(    b.length() == 2u );

    EXPECT( a == span );
    EXPECT( a == b );
#endif // gsl_FEATURE( STRING_SPAN )
}

CASE( "string_span<>: to_string triggers SFINAE errors on basic_string_span's move & copy constructor with Clang-3.9 (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS) [issue #53a]" )
{
# if gsl_FEATURE( STRING_SPAN ) && gsl_CONFIG( CONFIRMS_COMPILATION_ERRORS )
    cstring_span span = "Hello world";
    std::string str = to_string( span );
# endif
}

CASE( "narrow<>(): Allows narrowing double to float without MSVC level 4 warning C4127: conditional expression is constant [issue #115]" )
{
# if gsl_HAVE( EXCEPTIONS )
    try { (void) narrow<float>( 1.0 ); } catch(...) {}
# endif // gsl_HAVE( EXCEPTIONS )
}

CASE( "detail::is_compatible_container<>: Not a proper type trait [PR #238]" )
{
#if gsl_HAVE( TYPE_TRAITS ) && ! gsl_BETWEEN( gsl_COMPILER_MSVC_VERSION, 1, 140 )
    static_assert( std::is_base_of<gsl_lite::std11::false_type, gsl_lite::detail::is_compatible_container< int, int > >::value, "static assertion failed" );
#endif
}

// end of file
