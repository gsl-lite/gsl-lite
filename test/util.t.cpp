//
// gsl-lite is based on GSL: Guidelines Support Library.
// For more information see https://github.com/martinmoene/gsl-lite
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

#include "gsl-lite.t.hpp"
#include <functional>

#define gsl_CPP11_OR_GREATER_WRT_FINAL ( gsl_CPP11_OR_GREATER || gsl_COMPILER_MSVC_VERSION >= 110 )

using namespace gsl;

CASE( "finally: Allows to run lambda on leaving scope" )
{
#if gsl_CPP11_OR_GREATER_WRT_FINAL
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
#if gsl_CPP11_OR_GREATER_WRT_FINAL
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

#if gsl_CPP11_OR_GREATER_WRT_FINAL

    g_i = 0;
    {
        auto _ = finally( &F::incr );
        EXPECT( g_i == 0 );
    }
    EXPECT( g_i == 1 );
#else
    g_i = 0;
    {
        final_action _ = finally( &F::incr );
        EXPECT( g_i == 0 );
    }
    EXPECT( g_i == 1 );
#endif
}

CASE( "finally: Allows to move final_action" )
{
#if gsl_CPP11_OR_GREATER_WRT_FINAL
    struct F { static void incr( int & i ) { i += 1; } };

    int i = 0;
    {
        auto _1 = finally( [&]() { F::incr( i ); } );
        {
            auto _2 = std::move( _1 );
            EXPECT( i == 0 );
        }
        EXPECT( i == 1 );
        {
            auto _2 = std::move( _1 );
            EXPECT( i == 1 );
        }
        EXPECT( i == 1 );
    }
    EXPECT( i == 1 );
#else
    struct F { static void incr() { g_i += 1; } };

    g_i = 0;
    {
        final_action _1 = finally( &F::incr );
        {
            final_action _2 = _1;
            EXPECT( g_i == 0 );
        }
        EXPECT( g_i == 1 );
        {
            final_action _2 = _1;
            EXPECT( g_i == 1 );
        }
        EXPECT( g_i == 1 );
    }
    EXPECT( g_i == 1 );
#endif
}

CASE( "finally: Allows moving final_action to throw" "[.]")
{
#if gsl_CPP11_OR_GREATER_WRT_FINAL
    struct action
    {
        int & i_;
        void operator()(){ i_ += 1; }     
        action( int & i ) : i_( i ) {}
        action( action && other ) : i_( other.i_) { throw std::runtime_error("action move-ctor"); }   
    };

    int i = 0;
    {
        {
            EXPECT_THROWS( finally( action( i ) ) );
        }
        EXPECT( i == 1 );
    }

    // ... 
#else
    EXPECT( !!"lambda is not available (no C++11)" );
#endif
}

CASE( "on_return: Allows to perform action on leaving scope without exception (gsl_FEATURE_EXPERIMENTAL_RETURN_GUARD)" )
{
#if gsl_FEATURE( EXPERIMENTAL_RETURN_GUARD )
#if gsl_CPP11_OR_GREATER_WRT_FINAL
    struct F { 
        static void incr() { g_i += 1; }
        static void pass() { try { auto _ = on_return( &F::incr ); /*throw std::exception();*/ } catch (...) {} }
        static void fail() { try { auto _ = on_return( &F::incr );   throw std::exception();   } catch (...) {} }
    };
#else
    struct F { 
        static void incr() { g_i += 1; }
        static void pass() { try { final_action_return _ = on_return( &F::incr ); /*throw std::exception();*/ } catch (...) {} }
        static void fail() { try { final_action_return _ = on_return( &F::incr );   throw std::exception();   } catch (...) {} }
    };
#endif
    struct G {
        ~G() { F::pass(); }
    };
    { g_i = 0; F::pass(); EXPECT( g_i == 1 ); }
    { g_i = 0; F::fail(); EXPECT( g_i == 0 ); }
    { g_i = 0; try { G g; throw std::exception(); } catch (...) {}; EXPECT( g_i == 1 ); }
#else
    EXPECT( !!"on_return not available (no gsl_FEATURE_EXPERIMENTAL_RETURN_GUARD)" );
#endif
}

CASE( "on_error: Allows to perform action on leaving scope via an exception (gsl_FEATURE_EXPERIMENTAL_RETURN_GUARD)" )
{
#if gsl_FEATURE( EXPERIMENTAL_RETURN_GUARD )
#if gsl_CPP11_OR_GREATER_WRT_FINAL
    struct F { 
        static void incr() { g_i += 1; }
        static void pass() { try { auto _ = on_error( &F::incr ); /*throw std::exception();*/ } catch (...) {} }
        static void fail() { try { auto _ = on_error( &F::incr );   throw std::exception();   } catch (...) {} }
    };
#else
    struct F { 
        static void incr() { g_i += 1; }
        static void pass() { try { final_action_error _ = on_error( &F::incr ); /*throw std::exception();*/ } catch (...) {} }
        static void fail() { try { final_action_error _ = on_error( &F::incr );   throw std::exception();   } catch (...) {} }
    };
#endif
    struct G {
        ~G() { F::pass(); }
    };
    { g_i = 0; F::pass(); EXPECT( g_i == 0 ); }
    { g_i = 0; F::fail(); EXPECT( g_i == 1 ); }
    { g_i = 0; try { G g; throw std::exception(); } catch (...) {}; EXPECT( g_i == 0 ); }
#else
    EXPECT( !!"on_error not available (no gsl_FEATURE_EXPERIMENTAL_RETURN_GUARD)" );
#endif
}

CASE( "narrow_cast<>: Allows narrowing without value loss" )
{
    EXPECT( narrow_cast<char>( 120 ) == 120 );
}

CASE( "narrow_cast<>: Allows narrowing with value loss" )
{
    EXPECT( narrow_cast<unsigned char>( 300 ) == 44 );
}

CASE( "narrow<>(): Allows narrowing without value loss" )
{
    EXPECT( narrow<char>( 120 ) == 120 );
}

CASE( "narrow<>(): Terminates when narrowing with value loss" )
{
    EXPECT_THROWS_AS( narrow<char>( 300 ), narrowing_error );
}

CASE( "narrow<>(): Terminates when narrowing with sign loss" )
{
    EXPECT_THROWS_AS( narrow<unsigned>( -42 ), narrowing_error );
}

// end of file
