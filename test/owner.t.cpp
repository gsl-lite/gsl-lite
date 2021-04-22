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

using namespace gsl;

CASE( "owner<>: Disallows construction from a non-pointer type (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)" )
{
#if  gsl_CONFIG( CONFIRMS_COMPILATION_ERRORS )
# if gsl_HAVE( TYPE_TRAITS )
    owner<int> p = gsl_nullptr;
# else
    EXPECT( !!"owner<> alias template restricted to pointer type is not available." );
# endif
#endif
}

CASE( "owner<>: Allows its use as the (pointer) type it stands for" )
{
#if gsl_HAVE( OWNER_TEMPLATE )
    struct F { static void incr( int * i ) { *i += 1; } };

    owner<int*> p = new int( 120 );

    EXPECT( (p != NULL) );
    EXPECT(  p != nullptr_void() );
# if gsl_HAVE( NULLPTR )
    EXPECT(  p != nullptr );
# endif
    EXPECT( *p == 120 );

    F::incr( p );

    EXPECT( *p == 121 );
    delete p;

#if gsl_FEATURE( GSL_LITE_NAMESPACE )
    // Also try to consume `owner<>` from the `gsl_lite` namespace
    gsl_lite::owner<int*> p2 = new int( 120 );
    EXPECT( ( p2 != NULL ) );
    EXPECT( *p2 == 120 );
    F::incr( p2 );
    EXPECT( *p2 == 121 );
    delete p2;
#endif
#else
    EXPECT( !!"owner<> alias template is not available." );
#endif
}

CASE( "Owner(): Allows its use as the (pointer) type it stands for" )
{
#if gsl_FEATURE( OWNER_MACRO )
    struct F { static void incr( int * i ) { *i += 1; } };

    Owner(int*) p = new int( 120 );

    EXPECT( (p != NULL) );
    EXPECT(  p != nullptr_void() );
# if gsl_HAVE( NULLPTR )
    EXPECT(  p != nullptr );
# endif
    EXPECT( *p == 120 );

    F::incr( p );

    EXPECT( *p == 121 );
    delete p;
#else
    EXPECT( !!"Owner() macro is not available." );
#endif
}

// end of file
