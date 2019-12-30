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

#include <vector>

namespace {

bool expects( bool x ) { gsl_Expects( x ); return x; } 
bool ensures( bool x ) { gsl_Ensures( x ); return x; }
bool expectsAudit( bool x ) { gsl_ExpectsAudit( x ); return x; } 
bool ensuresAudit( bool x ) { gsl_EnsuresAudit( x ); return x; }

struct ConvertibleToBool
{
#if gsl_CPP11_OR_GREATER
    explicit
#endif
    operator bool(void) const gsl_noexcept { return true; }
};


}

CASE( "gsl_Expects(): Allows a true expression" )
{
    EXPECT_NO_THROW( expects( true  ) );
}

CASE( "gsl_Ensures(): Allows a true expression" )
{
    EXPECT_NO_THROW( ensures( true  ) );
}

CASE( "gsl_Expects(): Terminates on a false expression" )
{
    EXPECT_THROWS( expects( false ) );
}

CASE( "gsl_Ensures(): Terminates on a false expression" )
{
    EXPECT_THROWS( ensures( false ) );
}

CASE( "gsl_ExpectsAudit(): Allows a true expression" )
{
    EXPECT_NO_THROW( expectsAudit( true  ) );
}

CASE( "gsl_EnsuresAudit(): Allows a true expression" )
{
    EXPECT_NO_THROW( ensuresAudit( true  ) );
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

int testAssume( int i, std::vector<int> const& v )
{
    // The arguments to `__assume(x)` (MSVC) and `__builtin_assume(x)` (Clang) are never evaluated, so they cannot incur side-effects. We would like to implement
    // `gsl_ASSUME()` in terms of these. However, Clang always emits a diagnostic if a potential side-effect is discarded, and every call to a function not annotated
    // `__attribute__ ((pure))` or `__attribute__ ((const))` is considered a potential side-effect (e.g. the call to `v.size()` below). In many cases Clang is capable
    // of inlining the expression and find it free of side-effects, cf. https://gcc.godbolt.org/z/ZcKfbp, but the warning is produced anyway.
    //
    // To avoid littering user code with warnings, we instead define `gsl_ASSUME()` in terms of `__builtin_unreachable()`. The following `gsl_ASSUME()` statement
    // should thus compile without any warnings.

    gsl_ASSUME( i >= 0 && static_cast<std::size_t>(i) < v.size() );
    return v.at( static_cast<std::size_t>(i) );
}

void testConvertibleToBool()
{
    // `gsl_Expects()` should be compatible with explicit conversions to bool.
    gsl_Expects( ConvertibleToBool() );
}

// end of file
