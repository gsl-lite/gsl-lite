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
g++ -std=c++98 -I../include/ -Dgsl_THROW_ON_FAILURE -o gsl-lite.t.exe gsl-lite.t.cpp owner.t.cpp not_null.t.cpp && gsl-lite.t.exe --pass
g++ -std=c++03 -I../include/ -Dgsl_THROW_ON_FAILURE -o gsl-lite.t.exe gsl-lite.t.cpp owner.t.cpp not_null.t.cpp && gsl-lite.t.exe --pass
g++ -std=c++0x -I../include/ -Dgsl_THROW_ON_FAILURE -o gsl-lite.t.exe gsl-lite.t.cpp owner.t.cpp not_null.t.cpp && gsl-lite.t.exe --pass
g++ -std=c++11 -I../include/ -Dgsl_THROW_ON_FAILURE -o gsl-lite.t.exe gsl-lite.t.cpp owner.t.cpp not_null.t.cpp && gsl-lite.t.exe --pass
g++ -std=c++14 -I../include/ -Dgsl_THROW_ON_FAILURE -o gsl-lite.t.exe gsl-lite.t.cpp owner.t.cpp not_null.t.cpp && gsl-lite.t.exe --pass

cl -EHsc -I../include/ -Dgsl_THROW_ON_FAILURE gsl-lite.t.cpp owner.t.cpp not_null.t.cpp && gsl-lite.t.exe --pass

cl -EHsc -I../include/ -Dgsl_THROW_ON_FAILURE -Dgsl_CONFIRM_COMPILATION_ERRORS gsl-lite.t.cpp owner.t.cpp not_null.t.cpp && gsl-lite.t.exe --pass
#endif

// end of file
