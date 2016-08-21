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

// Use gsl::byte instead of plain byte to prevent collisions with 
// other byte declarations, such as in rpcndr.h (Windows kit).

// We have a chicken & egg problem here: 
//   verifying operations via to_integer() that has yet to verified itself...

CASE( "byte: Allows construction from integral via static cast (C++11)" )
{
#if gsl_HAVE_ENUM_CLASS
    gsl::byte b = static_cast<gsl::byte>( 4 );
    EXPECT( static_cast<unsigned char>(b) == 4 );
    EXPECT( to_integer<int>( b ) == 4 );
#else
    EXPECT( !!"enum class is not available (no C++11)" );
#endif
}

CASE( "byte: Allows construction from integral via byte() (C++11)" )
{
#if gsl_HAVE_ENUM_CLASS
    gsl::byte b = gsl::byte( 4 );
    EXPECT( to_integer<int>( b ) == 4 );
#else
    EXPECT( !!"enum class is not available (no C++11)" );
#endif
}

CASE( "byte: Allows construction from integral via to_byte()" )
{
    gsl::byte b = to_byte( 4 );
    EXPECT( to_integer<int>( b ) == 4 );
}

CASE( "byte: Allows conversion to integral via to_integer()" )
{
    gsl::byte b = to_byte( 4 );
    EXPECT( to_integer<int>( b ) == 4 );
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

// end of file
