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
#include <cstddef>
#include <complex>
#include <functional>

#if gsl_STDLIB_CPP11_OR_GREATER
# include <limits>
# include <cstdint>
#endif // gsl_STDLIB_CPP11_OR_GREATER

#define gsl_STDLIB_CPP11_OR_GREATER_WRT_FINAL ( gsl_STDLIB_CPP11_OR_GREATER || gsl_COMPILER_MSVC_VERSION >= 110 )

using namespace gsl_lite;

CASE( "finally: Allows to run lambda on leaving scope" )
{
#if gsl_STDLIB_CPP11_OR_GREATER_WRT_FINAL
    struct F { static void incr( int & i ) { i += 1; } };

    int i = 0;
    {
        auto _ = finally( [&]() { F::incr( i ); } );
        EXPECT( i == 0 );
    }
    EXPECT( i == 1 );
#else
    EXPECT( !!"lambda is not available (no C++11)" );
#endif
}

CASE( "finally: Allows to run function (bind) on leaving scope" )
{
#if gsl_STDLIB_CPP11_OR_GREATER_WRT_FINAL
    struct F { static void incr( int & i ) { i += 1; } };

    int i = 0;
    {
        auto _ = finally( std::bind( &F::incr, std::ref( i ) ) );
        EXPECT( i == 0 );
    }
    EXPECT( i == 1 );
#else
    EXPECT( !!"auto and std::ref perhaps not available (no C++11)" );
#endif
}

namespace{ int g_i = 0; }

CASE( "finally: Allows to run function (pointer) on leaving scope" )
{
    struct F { static void incr() { g_i += 1; } };

#if gsl_STDLIB_CPP11_OR_GREATER_WRT_FINAL

    g_i = 0;
    {
        auto _ = finally( &F::incr );
        EXPECT( g_i == 0 );
    }
    EXPECT( g_i == 1 );
#endif
}

CASE( "on_return: Allows to perform action on leaving scope without exception (gsl_FEATURE_EXPERIMENTAL_RETURN_GUARD)" )
{
#if gsl_HAVE( EXCEPTIONS )
# if gsl_FEATURE( EXPERIMENTAL_RETURN_GUARD )
#  if gsl_STDLIB_CPP11_OR_GREATER_WRT_FINAL
    struct F { 
        static void incr() { g_i += 1; }
        static void pass() { try { auto _ = on_return( &F::incr ); /*throw std::exception();*/ } catch (...) {} }
        static void fail() { try { auto _ = on_return( &F::incr );   throw std::exception();   } catch (...) {} }
    };
    struct G {
        ~G() { F::pass(); }
    };
    { g_i = 0; F::pass(); EXPECT( g_i == 1 ); }
    { g_i = 0; F::fail(); EXPECT( g_i == 0 ); }
    { g_i = 0; try { G g; throw std::exception(); } catch (...) {}; EXPECT( g_i == 1 ); }
#  endif
# else
    EXPECT( !!"on_return not available (no gsl_FEATURE_EXPERIMENTAL_RETURN_GUARD)" );
# endif
#endif // gsl_HAVE( EXCEPTIONS )
}

CASE( "on_error: Allows to perform action on leaving scope via an exception (gsl_FEATURE_EXPERIMENTAL_RETURN_GUARD)" )
{
#if gsl_HAVE( EXCEPTIONS )
# if gsl_FEATURE( EXPERIMENTAL_RETURN_GUARD )
#  if gsl_STDLIB_CPP11_OR_GREATER_WRT_FINAL
    struct F { 
        static void incr() { g_i += 1; }
        static void pass() { try { auto _ = on_error( &F::incr ); /*throw std::exception();*/ } catch (...) {} }
        static void fail() { try { auto _ = on_error( &F::incr );   throw std::exception();   } catch (...) {} }
    };
    struct G {
        ~G() { F::pass(); }
    };
    { g_i = 0; F::pass(); EXPECT( g_i == 0 ); }
    { g_i = 0; F::fail(); EXPECT( g_i == 1 ); }
    { g_i = 0; try { G g; throw std::exception(); } catch (...) {}; EXPECT( g_i == 0 ); }
#  endif
# else
    EXPECT( !!"on_error not available (no gsl_FEATURE_EXPERIMENTAL_RETURN_GUARD)" );
# endif
#endif // gsl_HAVE( EXCEPTIONS )
}

CASE( "narrow_cast<>: Allows narrowing without value loss" )
{
    EXPECT( narrow_cast<char>( 120 ) == 120 );
}

CASE( "narrow_cast<>: Allows narrowing with value loss" )
{
    EXPECT( narrow_cast<unsigned char>( 300 ) == 44 );
}

#if gsl_STDLIB_CPP11_OR_GREATER
const  std::uint8_t  u8  =  std::uint8_t((std::numeric_limits< std::uint8_t>::max)() - 1);
const std::uint16_t u16  = std::uint16_t((std::numeric_limits<std::uint16_t>::max)() - 1);
const   std::int8_t  i8n =   std::int8_t((std::numeric_limits<  std::int8_t>::min)() + 1);
const  std::int16_t i16n =  std::int16_t((std::numeric_limits< std::int16_t>::min)() + 1);
const   std::int8_t  i8p =   std::int8_t((std::numeric_limits<  std::int8_t>::max)() - 1);
const  std::int16_t i16p =  std::int16_t((std::numeric_limits< std::int16_t>::max)() - 1);
#endif // gsl_CPP11_OR_GREATER

CASE( "narrow<>(): Allows narrowing without value loss" )
{
#if gsl_HAVE( EXCEPTIONS )
    EXPECT( narrow<char>( 120 ) == 120 );

# if gsl_STDLIB_CPP11_OR_GREATER
    std::uint8_t lu8;
    std::uint16_t lu16;
    std::int8_t li8;
    std::int16_t li16;

    // uint <-> uint
    EXPECT_NO_THROW((lu16 = narrow<std::uint16_t>( std::uint8_t( u8)))); EXPECT(lu16 == u8);
    EXPECT_NO_THROW(( lu8 = narrow< std::uint8_t>( std::uint8_t( u8)))); EXPECT( lu8 == u8);
    EXPECT_NO_THROW(( lu8 = narrow< std::uint8_t>(std::uint16_t( u8)))); EXPECT( lu8 == u8);

    // int <-> int
    EXPECT_NO_THROW((li16 = narrow< std::int16_t>(  std::int8_t(i8n)))); EXPECT(li16 == i8n);
    EXPECT_NO_THROW((li16 = narrow< std::int16_t>(  std::int8_t(i8p)))); EXPECT(li16 == i8p);
    EXPECT_NO_THROW(( li8 = narrow<  std::int8_t>(  std::int8_t(i8n)))); EXPECT( li8 == i8n);
    EXPECT_NO_THROW(( li8 = narrow<  std::int8_t>(  std::int8_t(i8p)))); EXPECT( li8 == i8p);
    EXPECT_NO_THROW(( li8 = narrow<  std::int8_t>( std::int16_t(i8n)))); EXPECT( li8 == i8n);
    EXPECT_NO_THROW(( li8 = narrow<  std::int8_t>( std::int16_t(i8p)))); EXPECT( li8 == i8p);

    // uint <-> int
    EXPECT_NO_THROW((lu16 = narrow<std::uint16_t>(  std::int8_t(i8p)))); EXPECT(lu16 == i8p);
    EXPECT_NO_THROW((li16 = narrow< std::int16_t>( std::uint8_t(i8p)))); EXPECT(li16 == i8p);
    EXPECT_NO_THROW(( lu8 = narrow< std::uint8_t>(  std::int8_t(i8p)))); EXPECT( lu8 == i8p);
    EXPECT_NO_THROW(( li8 = narrow<  std::int8_t>( std::uint8_t(i8p)))); EXPECT( li8 == i8p);
    EXPECT_NO_THROW(( lu8 = narrow< std::uint8_t>( std::int16_t(i8p)))); EXPECT( lu8 == i8p);
    EXPECT_NO_THROW(( li8 = narrow<  std::int8_t>(std::uint16_t(i8p)))); EXPECT( li8 == i8p);
# endif // gsl_STDLIB_CPP11_OR_GREATER
#endif // gsl_HAVE( EXCEPTIONS )
}

CASE( "narrow<>(): Allows narrowing unordered type without precision loss" )
{
#if gsl_HAVE( EXCEPTIONS ) && gsl_HAVE( TYPE_TRAITS ) && gsl_HAVE( DEFAULT_FUNCTION_TEMPLATE_ARG )
    std::complex<float> cf;
    EXPECT_NO_THROW( cf = narrow<std::complex<float>>( std::complex<double>( 4, 2 ) ) );
    EXPECT( narrow<std::complex<float>>( std::complex<double>( 4, 2 ) ) == std::complex<float>( 4, 2 ) );
#endif // gsl_HAVE( EXCEPTIONS ) && gsl_HAVE( TYPE_TRAITS ) && gsl_HAVE( DEFAULT_FUNCTION_TEMPLATE_ARG )
}

CASE( "narrow<>(): Throws when narrowing with value loss" )
{
#if gsl_HAVE( EXCEPTIONS ) && gsl_CONFIG( NARROW_THROWS_ON_TRUNCATION )
    EXPECT_THROWS_AS( (void) narrow<char>( 300 ), narrowing_error );

# if gsl_STDLIB_CPP11_OR_GREATER
    // uint <-> uint
    EXPECT_THROWS_AS( (void) narrow< std::uint8_t>( std::uint16_t( u16) ), narrowing_error );

    // int <-> int
    EXPECT_THROWS_AS( (void) narrow<  std::int8_t>(  std::int16_t(i16n) ), narrowing_error );
    EXPECT_THROWS_AS( (void) narrow<  std::int8_t>(  std::int16_t(i16p) ), narrowing_error );

    // uint <-> int
    EXPECT_THROWS_AS( (void) narrow<  std::int8_t>(  std::uint8_t( u8 ) ), narrowing_error );
    EXPECT_THROWS_AS( (void) narrow< std::uint8_t>(  std::int16_t(i16p) ), narrowing_error );
    EXPECT_THROWS_AS( (void) narrow<  std::int8_t>( std::uint16_t( u8 ) ), narrowing_error );
    EXPECT_THROWS_AS( (void) narrow<  std::int8_t>( std::uint16_t(u16 ) ), narrowing_error );
# endif // gsl_STDLIB_CPP11_OR_GREATER
#endif // gsl_HAVE( EXCEPTIONS ) && gsl_CONFIG( NARROW_THROWS_ON_TRUNCATION )
}

CASE( "narrow<>(): Throws when narrowing with sign loss" )
{
#if gsl_HAVE( EXCEPTIONS ) && gsl_CONFIG( NARROW_THROWS_ON_TRUNCATION )
    EXPECT_THROWS_AS( (void) narrow<unsigned>( -42 ), narrowing_error );

# if gsl_STDLIB_CPP11_OR_GREATER
    // uint <-> int
    EXPECT_THROWS_AS( (void) narrow<std::uint16_t>(   std::int8_t( i8n) ), narrowing_error );
    EXPECT_THROWS_AS( (void) narrow< std::uint8_t>(   std::int8_t( i8n) ), narrowing_error );
    EXPECT_THROWS_AS( (void) narrow< std::uint8_t>(  std::int16_t( i8n) ), narrowing_error );
    EXPECT_THROWS_AS( (void) narrow< std::uint8_t>(  std::int16_t(i16n) ), narrowing_error );
# endif // gsl_STDLIB_CPP11_OR_GREATER
#endif // gsl_HAVE( EXCEPTIONS ) && gsl_CONFIG( NARROW_THROWS_ON_TRUNCATION )
}

CASE( "narrow<>(): Throws when narrowing unordered type with precision loss" )
{
#if gsl_HAVE( TYPE_TRAITS ) && gsl_HAVE( DEFAULT_FUNCTION_TEMPLATE_ARG ) && gsl_HAVE( EXCEPTIONS ) && gsl_CONFIG( NARROW_THROWS_ON_TRUNCATION )
    EXPECT_THROWS_AS( (void) narrow<std::complex<float>>( std::complex<double>( 4.2 ) ), narrowing_error );
#endif // gsl_HAVE( TYPE_TRAITS ) && gsl_HAVE( DEFAULT_FUNCTION_TEMPLATE_ARG ) && gsl_HAVE( EXCEPTIONS ) && gsl_CONFIG( NARROW_THROWS_ON_TRUNCATION )
}

CASE( "narrow_failfast<>(): Allows narrowing without value loss" )
{
    EXPECT( narrow_failfast<char>( 120 ) == 120 );

#if gsl_STDLIB_CPP11_OR_GREATER
    std::uint8_t lu8;
    std::uint16_t lu16;
    std::int8_t li8;
    std::int16_t li16;

    // uint <-> uint
    EXPECT_NO_THROW((lu16 = narrow_failfast<std::uint16_t>( std::uint8_t( u8)))); EXPECT(lu16 == u8);
    EXPECT_NO_THROW(( lu8 = narrow_failfast< std::uint8_t>( std::uint8_t( u8)))); EXPECT( lu8 == u8);
    EXPECT_NO_THROW(( lu8 = narrow_failfast< std::uint8_t>(std::uint16_t( u8)))); EXPECT( lu8 == u8);

    // int <-> int
    EXPECT_NO_THROW((li16 = narrow_failfast< std::int16_t>(  std::int8_t(i8n)))); EXPECT(li16 == i8n);
    EXPECT_NO_THROW((li16 = narrow_failfast< std::int16_t>(  std::int8_t(i8p)))); EXPECT(li16 == i8p);
    EXPECT_NO_THROW(( li8 = narrow_failfast<  std::int8_t>(  std::int8_t(i8n)))); EXPECT( li8 == i8n);
    EXPECT_NO_THROW(( li8 = narrow_failfast<  std::int8_t>(  std::int8_t(i8p)))); EXPECT( li8 == i8p);
    EXPECT_NO_THROW(( li8 = narrow_failfast<  std::int8_t>( std::int16_t(i8n)))); EXPECT( li8 == i8n);
    EXPECT_NO_THROW(( li8 = narrow_failfast<  std::int8_t>( std::int16_t(i8p)))); EXPECT( li8 == i8p);

    // uint <-> int
    EXPECT_NO_THROW((lu16 = narrow_failfast<std::uint16_t>(  std::int8_t(i8p)))); EXPECT(lu16 == i8p);
    EXPECT_NO_THROW((li16 = narrow_failfast< std::int16_t>( std::uint8_t(i8p)))); EXPECT(li16 == i8p);
    EXPECT_NO_THROW(( lu8 = narrow_failfast< std::uint8_t>(  std::int8_t(i8p)))); EXPECT( lu8 == i8p);
    EXPECT_NO_THROW(( li8 = narrow_failfast<  std::int8_t>( std::uint8_t(i8p)))); EXPECT( li8 == i8p);
    EXPECT_NO_THROW(( lu8 = narrow_failfast< std::uint8_t>( std::int16_t(i8p)))); EXPECT( lu8 == i8p);
    EXPECT_NO_THROW(( li8 = narrow_failfast<  std::int8_t>(std::uint16_t(i8p)))); EXPECT( li8 == i8p);
#endif // gsl_STDLIB_CPP11_OR_GREATER
}

CASE( "narrow_failfast<>(): Allows narrowing unordered type without precision loss" )
{
#if gsl_HAVE( TYPE_TRAITS ) && gsl_HAVE( DEFAULT_FUNCTION_TEMPLATE_ARG )
    std::complex<float> cf;
    EXPECT_NO_THROW( cf = narrow_failfast<std::complex<float>>( std::complex<double>( 4, 2 ) ) );
    EXPECT( narrow_failfast<std::complex<float>>( std::complex<double>( 4, 2 ) ) == std::complex<float>( 4, 2 ) );
#endif // gsl_HAVE( TYPE_TRAITS ) && gsl_HAVE( DEFAULT_FUNCTION_TEMPLATE_ARG )
}

CASE( "narrow_failfast<>(): Fails when narrowing with value loss" )
{
    EXPECT_THROWS_AS( (void) narrow_failfast<char>( 300 ), fail_fast );

#if gsl_STDLIB_CPP11_OR_GREATER
    // uint <-> uint
    EXPECT_THROWS_AS( (void) narrow_failfast< std::uint8_t>( std::uint16_t( u16) ), fail_fast );

    // int <-> int
    EXPECT_THROWS_AS( (void) narrow_failfast<  std::int8_t>(  std::int16_t(i16n) ), fail_fast );
    EXPECT_THROWS_AS( (void) narrow_failfast<  std::int8_t>(  std::int16_t(i16p) ), fail_fast );

    // uint <-> int
    EXPECT_THROWS_AS( (void) narrow_failfast<  std::int8_t>(  std::uint8_t( u8 ) ), fail_fast );
    EXPECT_THROWS_AS( (void) narrow_failfast< std::uint8_t>(  std::int16_t(i16p) ), fail_fast );
    EXPECT_THROWS_AS( (void) narrow_failfast<  std::int8_t>( std::uint16_t( u8 ) ), fail_fast );
    EXPECT_THROWS_AS( (void) narrow_failfast<  std::int8_t>( std::uint16_t(u16 ) ), fail_fast );
#endif // gsl_STDLIB_CPP11_OR_GREATER
}

CASE( "narrow_failfast<>(): Fails when narrowing with sign loss" )
{
    EXPECT_THROWS_AS( (void) narrow_failfast<unsigned>( -42 ), fail_fast );

#if gsl_STDLIB_CPP11_OR_GREATER
    // uint <-> int
    EXPECT_THROWS_AS( (void) narrow_failfast<std::uint16_t>(   std::int8_t( i8n) ), fail_fast );
    EXPECT_THROWS_AS( (void) narrow_failfast< std::uint8_t>(   std::int8_t( i8n) ), fail_fast );
    EXPECT_THROWS_AS( (void) narrow_failfast< std::uint8_t>(  std::int16_t( i8n) ), fail_fast );
    EXPECT_THROWS_AS( (void) narrow_failfast< std::uint8_t>(  std::int16_t(i16n) ), fail_fast );
#endif // gsl_STDLIB_CPP11_OR_GREATER
}

CASE( "narrow_failfast<>(): Fails when narrowing unordered type with precision loss" )
{
#if gsl_HAVE( TYPE_TRAITS ) && gsl_HAVE( DEFAULT_FUNCTION_TEMPLATE_ARG )
    EXPECT_THROWS_AS( (void) narrow_failfast<std::complex<float>>( std::complex<double>( 4.2 ) ), fail_fast );
#endif // gsl_HAVE( TYPE_TRAITS ) && gsl_HAVE( DEFAULT_FUNCTION_TEMPLATE_ARG )
}

#if gsl_CPP20_OR_GREATER && ( ! defined( _MSC_VER ) || gsl_COMPILER_MSVC_VERSION >= 1929 || gsl_COMPILER_CLANG_VERSION >= 1800 )
struct Empty { };
struct NoEmptyMember
{
    int nonEmpty;
};
struct OneEmptyMember
{
    gsl_NO_UNIQUE_ADDRESS Empty empty;
    int nonEmpty;
};

CASE( "gsl_NO_UNIQUE_ADDRESS: Class layout is compressed if attribute is used on empty members" )
{
    EXPECT( sizeof( NoEmptyMember ) == sizeof( int ) );
    EXPECT( sizeof( OneEmptyMember ) == sizeof( NoEmptyMember ) );
    EXPECT( offsetof( OneEmptyMember, nonEmpty ) == offsetof( NoEmptyMember, nonEmpty ) );
}
#endif // gsl_CPP20_OR_GREATER && ( ! defined( _MSC_VER ) || gsl_COMPILER_MSVC_VERSION >= 1929 || gsl_COMPILER_CLANG_VERSION >= 1800 )

// end of file
