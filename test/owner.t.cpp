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

namespace {

CASE( "owner<>: Allows its use as the (pointer) type it stands for" )
{
#if gsl_HAVE_OWNER_TEMPLATE
    struct F { static void incr( int * i ) { *i += 1; } };
    
    owner<int*> p = new int( 120 );

    EXPECT( (p != NULL) );
    EXPECT(  p != (void*)0 );
# if gsl_HAVE_NULLPTR
    EXPECT(  p != nullptr );
# endif 
    EXPECT( *p == 120 );

    F::incr( p );

    EXPECT( *p == 121 );
#else
    EXPECT( !!"owner<> alias template is not available." );
#endif
}

CASE( "Owner(): Allows its use as the (pointer) type it stands for" )
{
#if gsl_FEATURE_HAVE_OWNER_MACRO
    struct F { static void incr( int * i ) { *i += 1; } };
    
    Owner(int*) p = new int( 120 );

    EXPECT( (p != NULL) );
    EXPECT(  p != (void*)0 );
# if gsl_HAVE_NULLPTR
    EXPECT(  p != nullptr );
# endif 
    EXPECT( *p == 120 );

    F::incr( p );

    EXPECT( *p == 121 );
#else
    EXPECT( !!"Owner() macro is not available." );
#endif
}

}

// end of file
