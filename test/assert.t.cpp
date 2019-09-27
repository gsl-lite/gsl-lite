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

bool expects( bool x ) { Expects( x ); return x; } 
bool ensures( bool x ) { Ensures( x ); return x; }
bool expectsAudit( bool x ) { ExpectsAudit( x ); return x; } 
bool ensuresAudit( bool x ) { EnsuresAudit( x ); return x; }

}

CASE( "Expects(): Allows a true expression" )
{
    EXPECT_NO_THROW( expects( true  ) );
}

CASE( "Ensures(): Allows a true expression" )
{
    EXPECT_NO_THROW( ensures( true  ) );
}

CASE( "Expects(): Terminates on a false expression" )
{
    EXPECT_THROWS( expects( false ) );
}

CASE( "Ensures(): Terminates on a false expression" )
{
    EXPECT_THROWS( ensures( false ) );
}

CASE( "ExpectsAudit(): Allows a true expression" )
{
    EXPECT_NO_THROW( expectsAudit( true  ) );
}

CASE( "EnsuresAudit(): Allows a true expression" )
{
    EXPECT_NO_THROW( ensuresAudit( true  ) );
}

CASE( "ExpectsAudit(): Terminates on a false expression in AUDIT mode" )
{
#if defined( gsl_CONFIG_CONTRACT_LEVEL_AUDIT )
    EXPECT_THROWS( expectsAudit( false ) );
#else
    EXPECT_NO_THROW( expectsAudit( false ) );
#endif
}

CASE( "EnsuresAudit(): Terminates on a false expression in AUDIT mode" )
{
#if defined( gsl_CONFIG_CONTRACT_LEVEL_AUDIT )
    EXPECT_THROWS( ensuresAudit( false ) );
#else
    EXPECT_NO_THROW( ensuresAudit( false ) );
#endif
}

int testAssume( int i, std::vector<int> const& v )
{
    // This should compile without warnings.
    gsl_ASSUME( i >= 0 && static_cast<std::size_t>(i) < v.size() );
    return v.at( static_cast<std::size_t>(i) );
}

// end of file
