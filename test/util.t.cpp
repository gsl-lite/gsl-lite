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

#include "gsl-lite.t.h"
#include <functional>

namespace {

CASE( "finally: Allows lambda to run" )
{
#if gsl_CPP11_OR_GREATER
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

CASE( "finally: Allows function with bind" )
{
#if gsl_CPP11_OR_GREATER
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

int g_i = 0;

CASE( "finally: Allows pointer to function" )
{
    struct F { static void incr() { g_i += 1; } };

#if gsl_CPP11_OR_GREATER

    g_i = 0;
    {
        auto _ = finally( &F::incr );
        EXPECT( g_i == 0 );
    }
    EXPECT( g_i == 1 );
#else
    g_i = 0;
    {
        Final_act _ = finally( &F::incr );
        EXPECT( g_i == 0 );
    }
    EXPECT( g_i == 1 );
#endif
}

CASE( "narrow_cast<>: Allows narrowing without loss" )
{
    EXPECT( narrow_cast<char>( 120 ) == 120 );
}

CASE( "narrow_cast<>: Allows narrowing with loss" )
{
    EXPECT( narrow_cast<unsigned char>( 300 ) == 44 );
}

CASE( "narrow<>(): Allows narrowing without loss" )
{
    EXPECT( narrow<char>( 120 ) == 120 );
}

CASE( "narrow<>(): Terminates when narrowing with loss" )
{
    EXPECT_THROWS_AS( narrow<char>( 300 ), narrowing_error );
}

}

// end of file
