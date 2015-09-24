// Copyright 2015 by Martin Moene
//
// gsl-lite is based on GSL: Guidelines Support Library,
// https://github.com/microsoft/gsl
//
// This code is licensed under the MIT License (MIT). 
//

#include "gsl-lite.t.h"

namespace {

CASE( "owner(): Can use as the pointer it stands for" )
{
    struct F { static void incr( int * i ) { *i += 1; } };
    Owner(int*) p = new int( 120 );

    EXPECT( (p != NULL) );
    EXPECT(  p != (void*)0 );
#if gsl_HAVE_NULLPTR
    EXPECT(  p != nullptr );
#endif 
    EXPECT( *p == 120 );

    F::incr(p);

    EXPECT( *p == 121 );
}

}

// end of file
