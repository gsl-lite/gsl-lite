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

using namespace gsl;

// Use gsl::byte instead of plain byte to prevent collisions with
// other byte declarations, such as in rpcndr.h (Windows kit).

// We have a chicken & egg problem here:
//   verifying operations via to_integer() that has yet to verified itself...

CASE( "byte: Allows to construct from integral via static cast (C++17)" )
{
#if gsl_HAVE( ENUM_CLASS_CONSTRUCTION_FROM_UNDERLYING_TYPE )
    gsl::byte b = static_cast<gsl::byte>( 4 );
    EXPECT( static_cast<unsigned char>(b) == 4 );
    EXPECT( to_integer<int>( b ) == 4 );
#else
    EXPECT( !!"enum class is not constructible from underlying type (no C++17)" );
#endif
}

CASE( "byte: Allows to construct from integral via byte() (C++17)" )
{
#if gsl_HAVE( ENUM_CLASS_CONSTRUCTION_FROM_UNDERLYING_TYPE )
    gsl::byte b = gsl::byte( 4 );
    EXPECT( to_integer<int>( b ) == 4 );
#else
    EXPECT( !!"enum class is not constructible from underlying type (no C++17)" );
#endif
}

CASE( "byte: Allows to construct from integral via to_byte()" )
{
    gsl::byte b = to_byte( 4 );
    EXPECT( to_integer<int>( b ) == 4 );
}

CASE( "byte: Allows to convert to integral via to_integer()" )
{
    gsl::byte b = to_byte( 4 );
    EXPECT( to_integer<int>( b ) == 4 );
}

CASE( "byte: Allows comparison operations" )
{
    gsl::byte a = to_byte( 3 );
    gsl::byte b = to_byte( 7 );

    EXPECT(     a == a );
    EXPECT(     a != b );

    EXPECT(     a <  b );
    EXPECT(     a <= a );
    EXPECT(     a <= b );

    EXPECT(     b >  a );
    EXPECT(     b >= a );
    EXPECT(     b >= b );

    EXPECT_NOT( a == b );
    EXPECT_NOT( a != a );
    EXPECT_NOT( b <  a );
    EXPECT_NOT( a >  b );
}

CASE( "byte: Allows bitwise or operation" )
{
    gsl::byte const b = to_byte( 0xa5 );

    EXPECT( ( b | b ) == b );
    EXPECT( ( b | to_byte( 0x00 ) ) == b );
    EXPECT( ( b | to_byte( 0xff ) ) == to_byte( 0xff ) );
}

CASE( "byte: Allows bitwise and operation" )
{
    gsl::byte const b = to_byte( 0xa5 );

    EXPECT( ( b & b ) == b );
    EXPECT( ( b & to_byte( 0xff ) ) == b );
    EXPECT( ( b & to_byte( 0x00 ) ) == to_byte( 0x00 ) );
}

CASE( "byte: Allows bitwise x-or operation" )
{
    gsl::byte const b = to_byte( 0xa5 );

    EXPECT( ( b ^ b ) == to_byte( 0 ) );
    EXPECT( ( b ^ to_byte( 0x00 ) ) ==  b );
    EXPECT( ( b ^ to_byte( 0xff ) ) == ~b );
}

CASE( "byte: Allows bitwise or assignment" )
{
    SETUP("") {

    gsl::byte const b_org = to_byte( 0xa5 );
    gsl::byte       b     = b_org;

    SECTION("Identity") { EXPECT( ( b |= b               ) == b_org           ); }
    SECTION("Identity") { EXPECT( ( b |= to_byte( 0x00 ) ) == b_org           ); }
    SECTION("Saturate") { EXPECT( ( b |= to_byte( 0xff ) ) == to_byte( 0xff ) ); }
    }
}

CASE( "byte: Allows bitwise and assignment" )
{
    SETUP("") {

    gsl::byte const b_org = to_byte( 0xa5 );
    gsl::byte       b     = b_org;

    SECTION("Identity") { EXPECT( ( b &= b               ) == b_org           ); }
    SECTION("Identity") { EXPECT( ( b &= to_byte( 0xff ) ) == b_org           ); }
    SECTION("Clear"   ) { EXPECT( ( b &= to_byte( 0x00 ) ) == to_byte( 0x00 ) ); }
    }
}

CASE( "byte: Allows bitwise x-or assignment" )
{
    SETUP("") {

    gsl::byte const b_org = to_byte( 0xa5 );
    gsl::byte       b     = b_org;

    SECTION("Identity") { EXPECT( ( b ^= b               ) == to_byte( 0 ) ); }
    SECTION("Identity") { EXPECT( ( b ^= to_byte( 0x00 ) ) ==  b_org       ); }
    SECTION("Invert"  ) { EXPECT( ( b ^= to_byte( 0xff ) ) == ~b_org       ); }
    }
}

CASE( "byte: Allows shift-left operation" )
{
    gsl::byte const b = to_byte( 0xa5 );

    EXPECT( ( b << 3 ) == to_byte( to_uchar( b ) << 3 ) );
}

CASE( "byte: Allows shift-right operation" )
{
    gsl::byte const b = to_byte( 0xa5 );

    EXPECT( ( b >> 3 ) == to_byte( to_uchar( b ) >> 3 ) );
}

CASE( "byte: Allows shift-left assignment" )
{
    gsl::byte const b_org = to_byte( 0xa5 );
    gsl::byte       b     = b_org;

    EXPECT( ( b <<= 3 ) == to_byte( to_uchar( b_org ) << 3 ) );
}

CASE( "byte: Allows shift-right assignment" )
{
    gsl::byte const b_org = to_byte( 0xa5 );
    gsl::byte       b     = b_org;

    EXPECT( ( b >>= 3 ) == to_byte( to_uchar( b_org ) >> 3 ) );
}

CASE( "byte: Provides constexpr non-assignment operations (C++11)" )
{
#if gsl_HAVE( CONSTEXPR_11 )
    static_assert( to_byte( 0xa5 ) == to_byte( 0xa5 )                      , "" );
    static_assert(          0xa5   == to_integer<int>( to_byte( 0xa5 )    ), "" );

    static_assert( to_byte( 0x02 ) == ( to_byte( 0x01 ) << 1              ), "" );
    static_assert( to_byte( 0x01 ) == ( to_byte( 0x02 ) >> 1              ), "" );

    static_assert( to_byte( 0x01 ) == ( to_byte( 0x03 ) & to_byte( 0x01 ) ), "" );
    static_assert( to_byte( 0x01 ) == ( to_byte( 0x00 ) | to_byte( 0x01 ) ), "" );
    static_assert( to_byte( 0x00 ) == ( to_byte( 0x01 ) ^ to_byte( 0x01 ) ), "" );

    static_assert( to_byte( 0xff ) ==  ~to_byte( 0x00 ), "" );
#endif
}

CASE( "byte: Provides constexpr assignment operations (C++14)" )
{
#if gsl_HAVE( CONSTEXPR_14 )
//  ...
#endif
}

CASE( "byte: Provides hash support (C++11)" )
{
#if gsl_CPP11_OR_GREATER || gsl_COMPILER_MSVC_VERSION >= 120
    EXPECT_NO_THROW( std::hash<gsl::byte>{}( to_byte( 42 ) ) );
#else
    EXPECT( !!"hash support is not available (no C++11)" );
#endif
}

// end of file
