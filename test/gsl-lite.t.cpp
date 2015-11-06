// Copyright 2015 by Martin Moene
//
// gsl-lite is based on GSL: Guidelines Support Library,
// https://github.com/microsoft/gsl
//
// This code is licensed under the MIT License (MIT). 
//

#include "gsl-lite.t.h"

lest::tests & specification() 
{ 
    static lest::tests tests; 
    return tests; 
}

namespace {

CASE( "__cplusplus" )
{
    EXPECT( __cplusplus > 0L );
}

}

int main( int argc, char * argv[] )
{
    return lest::run( specification(), argc, argv );
}

#if 0
g++            -I../include/gsl -Dgsl_CONFIG_THROWS_FOR_TESTING=1 -o gsl-lite.t.exe gsl-lite.t.cpp assert.t.cpp at.t.cpp not_null.t.cpp owner.t.cpp span.t.cpp string_span.t.cpp util.t.cpp && gsl-lite.t.exe --pass
g++ -std=c++98 -I../include/gsl -Dgsl_CONFIG_THROWS_FOR_TESTING=1 -o gsl-lite.t.exe gsl-lite.t.cpp assert.t.cpp at.t.cpp not_null.t.cpp owner.t.cpp span.t.cpp string_span.t.cpp util.t.cpp && gsl-lite.t.exe --pass
g++ -std=c++03 -I../include/gsl -Dgsl_CONFIG_THROWS_FOR_TESTING=1 -o gsl-lite.t.exe gsl-lite.t.cpp assert.t.cpp at.t.cpp not_null.t.cpp owner.t.cpp span.t.cpp string_span.t.cpp util.t.cpp && gsl-lite.t.exe --pass
g++ -std=c++0x -I../include/gsl -Dgsl_CONFIG_THROWS_FOR_TESTING=1 -o gsl-lite.t.exe gsl-lite.t.cpp assert.t.cpp at.t.cpp not_null.t.cpp owner.t.cpp span.t.cpp string_span.t.cpp util.t.cpp && gsl-lite.t.exe --pass
g++ -std=c++11 -I../include/gsl -Dgsl_CONFIG_THROWS_FOR_TESTING=1 -o gsl-lite.t.exe gsl-lite.t.cpp assert.t.cpp at.t.cpp not_null.t.cpp owner.t.cpp span.t.cpp string_span.t.cpp util.t.cpp && gsl-lite.t.exe --pass
g++ -std=c++14 -I../include/gsl -Dgsl_CONFIG_THROWS_FOR_TESTING=1 -o gsl-lite.t.exe gsl-lite.t.cpp assert.t.cpp at.t.cpp not_null.t.cpp owner.t.cpp span.t.cpp string_span.t.cpp util.t.cpp && gsl-lite.t.exe --pass

cl -EHsc -I../include/gsl -Dgsl_CONFIG_THROWS_FOR_TESTING=1 gsl-lite.t.cpp assert.t.cpp at.t.cpp not_null.t.cpp owner.t.cpp span.t.cpp string_span.t.cpp util.t.cpp && gsl-lite.t.exe --pass

cl -EHsc -I../include/gsl -Dgsl_CONFIG_THROWS_FOR_TESTING=1 -Dgsl_CONFIG_CONFIRMS_COMPILATION_ERRORS gsl-lite.t.cpp assert.t.cpp at.t.cpp not_null.t.cpp owner.t.cpp span.t.cpp string_span.t.cpp util.t.cpp && gsl-lite.t.exe --pass
#endif

// end of file
