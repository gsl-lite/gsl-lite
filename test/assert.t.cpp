//
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

#include <vector>

namespace {

bool expects( bool x ) { gsl_Expects( x ); return x; }
bool ensures( bool x ) { gsl_Ensures( x ); return x; }
bool assert_( bool x ) { gsl_Assert( x ); return x; }
void failFast() { gsl_FailFast(); }
bool expectsDebug( bool x ) { gsl_ExpectsDebug( x ); return x; }
bool ensuresDebug( bool x ) { gsl_EnsuresDebug( x ); return x; }
bool assertDebug( bool x ) { gsl_AssertDebug( x ); return x; }
bool expectsAudit( bool x ) { gsl_ExpectsAudit( x ); return x; }
bool ensuresAudit( bool x ) { gsl_EnsuresAudit( x ); return x; }
bool assertAudit( bool x ) { gsl_AssertAudit( x ); return x; }

enum Color
#if gsl_CPP11_OR_GREATER
: int
#endif
{ red, green, blue };

#if gsl_COMPILER_NVHPC_VERSION && !gsl_CPP11_OR_GREATER
# pragma diag_suppress 941 // Suppress:  missing return statement at end of non-void function "..."
#endif
std::string colorToString( Color color )
{
    switch (color)
    {
    case red: return "red";
    case green: return "green";
    case blue: return "blue";
    }
    gsl_FailFast();  // this should keep the compiler from issuing a warning about not returning a value
}
#if gsl_COMPILER_NVHPC_VERSION && !gsl_CPP11_OR_GREATER
# pragma diag_default 941
#endif


struct ConvertibleToBool
{
#if gsl_CPP11_OR_GREATER
    explicit
#endif
    operator bool(void) const gsl_noexcept { return true; }
};


} // anonymous namespace

CASE( "gsl_Expects(): Allows a true expression" )
{
    EXPECT_NO_THROW( expects( true  ) );
}

CASE( "gsl_Ensures(): Allows a true expression" )
{
    EXPECT_NO_THROW( ensures( true  ) );
}

CASE( "gsl_Assert(): Allows a true expression" )
{
    EXPECT_NO_THROW( assert_( true  ) );
}

CASE( "gsl_Expects(): Terminates on a false expression" )
{
    EXPECT_THROWS( expects( false ) );
}

CASE( "gsl_Ensures(): Terminates on a false expression" )
{
    EXPECT_THROWS( ensures( false ) );
}

CASE( "gsl_Assert(): Terminates on a false expression" )
{
    EXPECT_THROWS( assert_( false ) );
}

CASE( "gsl_FailFast(): Suppresses compiler warning about missing return value" )
{
    EXPECT( colorToString(red) == "red" );
}

CASE( "gsl_FailFast(): Terminates" )
{
    EXPECT_THROWS( failFast() );
#if gsl_CPP11_OR_GREATER
    EXPECT_THROWS( colorToString( Color( 42 ) ) );
#endif
}

CASE( "gsl_ExpectsDebug(): Allows a true expression" )
{
    EXPECT_NO_THROW( expectsDebug( true  ) );
}

CASE( "gsl_EnsuresDebug(): Allows a true expression" )
{
    EXPECT_NO_THROW( ensuresDebug( true  ) );
}

CASE( "gsl_AssertDebug(): Allows a true expression" )
{
    EXPECT_NO_THROW( assertDebug( true  ) );
}

CASE( "gsl_ExpectsDebug(): Terminates on a false expression in debug build or AUDIT mode" )
{
#if !defined( NDEBUG ) || defined( gsl_CONFIG_CONTRACT_CHECKING_AUDIT )
    EXPECT_THROWS( expectsDebug( false ) );
#else
    EXPECT_NO_THROW( expectsDebug( false ) );
#endif
}

CASE( "gsl_EnsuresAudit(): Terminates on a false expression in debug build or AUDIT mode" )
{
#if !defined( NDEBUG ) || defined( gsl_CONFIG_CONTRACT_CHECKING_AUDIT )
    EXPECT_THROWS( ensuresDebug( false ) );
#else
    EXPECT_NO_THROW( ensuresDebug( false ) );
#endif
}

CASE( "gsl_AssertAudit(): Terminates on a false expression in debug build or AUDIT mode" )
{
#if !defined( NDEBUG ) || defined( gsl_CONFIG_CONTRACT_CHECKING_AUDIT )
    EXPECT_THROWS( assertDebug( false ) );
#else
    EXPECT_NO_THROW( assertDebug( false ) );
#endif
}

CASE( "gsl_ExpectsAudit(): Allows a true expression" )
{
    EXPECT_NO_THROW( expectsAudit( true  ) );
}

CASE( "gsl_EnsuresAudit(): Allows a true expression" )
{
    EXPECT_NO_THROW( ensuresAudit( true  ) );
}

CASE( "gsl_AssertAudit(): Allows a true expression" )
{
    EXPECT_NO_THROW( assertAudit( true  ) );
}

CASE( "gsl_ExpectsAudit(): Terminates on a false expression in AUDIT mode" )
{
#if defined( gsl_CONFIG_CONTRACT_CHECKING_AUDIT )
    EXPECT_THROWS( expectsAudit( false ) );
#else
    EXPECT_NO_THROW( expectsAudit( false ) );
#endif
}

CASE( "gsl_EnsuresAudit(): Terminates on a false expression in AUDIT mode" )
{
#if defined( gsl_CONFIG_CONTRACT_CHECKING_AUDIT )
    EXPECT_THROWS( ensuresAudit( false ) );
#else
    EXPECT_NO_THROW( ensuresAudit( false ) );
#endif
}

CASE( "gsl_AssertAudit(): Terminates on a false expression in AUDIT mode" )
{
#if defined( gsl_CONFIG_CONTRACT_CHECKING_AUDIT )
    EXPECT_THROWS( assertAudit( false ) );
#else
    EXPECT_NO_THROW( assertAudit( false ) );
#endif
}

int myAt( int i, std::vector<int> const& v )
{
    // The arguments to `__assume( x )` (MSVC) and `__builtin_assume( x )` (Clang) are never evaluated, so they cannot incur side-effects. We would like to implement
    // `gsl_ASSUME_()` in terms of these. However, Clang always emits a diagnostic if a potential side-effect is discarded, and every call to a function not annotated
    // `__attribute__ ((pure))` or `__attribute__ ((const))` is considered a potential side-effect (e.g. the call to `v.size()` below). In many cases Clang is capable
    // of inlining the expression and find it free of side-effects, cf. https://gcc.godbolt.org/z/ZcKfbp, but the warning is produced anyway.
    //
    // To avoid littering user code with warnings, we instead define `gsl_ASSUME_()` in terms of `__builtin_unreachable()`. The following `gsl_ASSUME_()` statement
    // should thus compile without any warnings.

    gsl_Expects( i >= 0 && static_cast<std::size_t>(i) < v.size() );
    return v.at( static_cast<std::size_t>(i) );
}

CASE( "gsl_Expects(): No warnings produced for function calls in precondition checks" )
{
    std::vector<int> v;
    v.push_back( 42 );
    EXPECT_NO_THROW( myAt( 0, v ) );
    EXPECT_THROWS( myAt( 1, v ) );
}

CASE( "gsl_Expects(): Supports explicit conversions to bool" )
{
    // `gsl_Expects()` should be compatible with explicit conversions to bool.
    gsl_Expects( ConvertibleToBool() );

    if ( ConvertibleToBool() ) { } // to get rid of weird NVCC warning about never-referenced conversion operator
}


// end of file
