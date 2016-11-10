// Copyright 2015 by Martin Moene
//
// gsl-lite is based on GSL: Guideline Support Library.
// For more information see https://github.com/martinmoene/gsl-lite
//
// This code is licensed under the MIT License (MIT). 
//

#include "gsl-lite.t.h"

#define gsl_PRESENT( x ) \
    std::cout << #x << ": " << x << "\n"

#define gsl_ABSENT( x ) \
    std::cout << #x << ": (undefined)\n"

lest::tests & specification() 
{ 
    static lest::tests tests; 
    return tests; 
}

CASE( "gsl-lite version" "[.version]" )
{
    gsl_PRESENT( gsl_lite_VERSION );
}

CASE( "__cplusplus" "[.stdc++]" )
{
    gsl_PRESENT( __cplusplus );
}

CASE( "Presence of C++ language features" "[.stdlanguage]" )
{
#if gsl_HAVE_AUTO 
    gsl_PRESENT( gsl_HAVE_AUTO );
#else    
    gsl_ABSENT(  gsl_HAVE_AUTO );
#endif

#if gsl_HAVE_NULLPTR 
    gsl_PRESENT( gsl_HAVE_NULLPTR );
#else    
    gsl_ABSENT(  gsl_HAVE_NULLPTR );
#endif

#if gsl_HAVE_DEFAULT_FUNCTION_TEMPLATE_ARG 
    gsl_PRESENT( gsl_HAVE_DEFAULT_FUNCTION_TEMPLATE_ARG );
#else    
    gsl_ABSENT(  gsl_HAVE_DEFAULT_FUNCTION_TEMPLATE_ARG );
#endif

#if gsl_HAVE_ALIAS_TEMPLATE 
    gsl_PRESENT( gsl_HAVE_ALIAS_TEMPLATE );
#else    
    gsl_ABSENT(  gsl_HAVE_ALIAS_TEMPLATE );
#endif

#if gsl_HAVE_CONSTEXPR_11 
    gsl_PRESENT( gsl_HAVE_CONSTEXPR_11 );
#else    
    gsl_ABSENT(  gsl_HAVE_CONSTEXPR_11 );
#endif

#if gsl_HAVE_CONSTEXPR_14
    gsl_PRESENT( gsl_HAVE_CONSTEXPR_14 );
#else    
    gsl_ABSENT(  gsl_HAVE_CONSTEXPR_14 );
#endif

#if gsl_HAVE_ENUM_CLASS 
    gsl_PRESENT( gsl_HAVE_ENUM_CLASS );
#else    
    gsl_ABSENT(  gsl_HAVE_ENUM_CLASS );
#endif

#if gsl_HAVE_ENUM_CLASS_CONSTRUCTION_FROM_UNDERLYING_TYPE 
    gsl_PRESENT( gsl_HAVE_ENUM_CLASS_CONSTRUCTION_FROM_UNDERLYING_TYPE );
#else    
    gsl_ABSENT(  gsl_HAVE_ENUM_CLASS_CONSTRUCTION_FROM_UNDERLYING_TYPE );
#endif

#if gsl_HAVE_EXPLICIT_CONVERSION 
    gsl_PRESENT( gsl_HAVE_EXPLICIT_CONVERSION );
#else    
    gsl_ABSENT(  gsl_HAVE_EXPLICIT_CONVERSION );
#endif

#if gsl_HAVE_INITIALIZER_LIST 
    gsl_PRESENT( gsl_HAVE_INITIALIZER_LIST );
#else    
    gsl_ABSENT(  gsl_HAVE_INITIALIZER_LIST );
#endif

#if gsl_HAVE_IS_DEFAULT 
    gsl_PRESENT( gsl_HAVE_IS_DEFAULT );
#else    
    gsl_ABSENT(  gsl_HAVE_IS_DEFAULT );
#endif

#if gsl_HAVE_IS_DELETE 
    gsl_PRESENT( gsl_HAVE_IS_DELETE );
#else    
    gsl_ABSENT(  gsl_HAVE_IS_DELETE );
#endif

#if gsl_HAVE_NOEXCEPT 
    gsl_PRESENT( gsl_HAVE_NOEXCEPT );
#else    
    gsl_ABSENT(  gsl_HAVE_NOEXCEPT );
#endif
}

CASE( "Presence of C++ library features" "[.stdlibrary]" )
{
#if gsl_HAVE_ARRAY
    gsl_PRESENT( gsl_HAVE_ARRAY );
#else    
    gsl_ABSENT(  gsl_HAVE_ARRAY );
#endif

#if gsl_HAVE_CONTAINER_DATA_METHOD
    gsl_PRESENT( gsl_HAVE_CONTAINER_DATA_METHOD );
#else    
    gsl_ABSENT(  gsl_HAVE_CONTAINER_DATA_METHOD );
#endif

#if gsl_HAVE_SIZED_TYPES
    gsl_PRESENT( gsl_HAVE_SIZED_TYPES );
#else    
    gsl_ABSENT(  gsl_HAVE_SIZED_TYPES );
#endif

#if gsl_HAVE_SHARED_PTR
    gsl_PRESENT( gsl_HAVE_SHARED_PTR );
#else    
    gsl_ABSENT(  gsl_HAVE_SHARED_PTR );
#endif

#if gsl_HAVE_UNIQUE_PTR
    gsl_PRESENT( gsl_HAVE_UNIQUE_PTR );
#else    
    gsl_ABSENT(  gsl_HAVE_UNIQUE_PTR );
#endif

#if gsl_HAVE_TYPE_TRAITS
    gsl_PRESENT( gsl_HAVE_TYPE_TRAITS );
#else    
    gsl_ABSENT(  gsl_HAVE_TYPE_TRAITS );
#endif

#if _HAS_CPP0X
    gsl_PRESENT( _HAS_CPP0X );
#else    
    gsl_ABSENT(  _HAS_CPP0X );
#endif
}

int main( int argc, char * argv[] )
{
    return lest::run( specification(), argc, argv );
}

#if 0
g++            -I../include/gsl -Dgsl_CONFIG_CONTRACT_VIOLATION_THROWS -o gsl-lite.t.exe gsl-lite.t.cpp assert.t.cpp at.t.cpp byte.t.cpp issue.t.cpp not_null.t.cpp owner.t.cpp span.t.cpp string_span.t.cpp util.t.cpp && gsl-lite.t.exe --pass
g++ -std=c++98 -I../include/gsl -Dgsl_CONFIG_CONTRACT_VIOLATION_THROWS -o gsl-lite.t.exe gsl-lite.t.cpp assert.t.cpp at.t.cpp byte.t.cpp issue.t.cpp not_null.t.cpp owner.t.cpp span.t.cpp string_span.t.cpp util.t.cpp && gsl-lite.t.exe --pass
g++ -std=c++03 -I../include/gsl -Dgsl_CONFIG_CONTRACT_VIOLATION_THROWS -o gsl-lite.t.exe gsl-lite.t.cpp assert.t.cpp at.t.cpp byte.t.cpp issue.t.cpp not_null.t.cpp owner.t.cpp span.t.cpp string_span.t.cpp util.t.cpp && gsl-lite.t.exe --pass
g++ -std=c++0x -I../include/gsl -Dgsl_CONFIG_CONTRACT_VIOLATION_THROWS -o gsl-lite.t.exe gsl-lite.t.cpp assert.t.cpp at.t.cpp byte.t.cpp issue.t.cpp not_null.t.cpp owner.t.cpp span.t.cpp string_span.t.cpp util.t.cpp && gsl-lite.t.exe --pass
g++ -std=c++11 -I../include/gsl -Dgsl_CONFIG_CONTRACT_VIOLATION_THROWS -o gsl-lite.t.exe gsl-lite.t.cpp assert.t.cpp at.t.cpp byte.t.cpp issue.t.cpp not_null.t.cpp owner.t.cpp span.t.cpp string_span.t.cpp util.t.cpp && gsl-lite.t.exe --pass
g++ -std=c++14 -I../include/gsl -Dgsl_CONFIG_CONTRACT_VIOLATION_THROWS -o gsl-lite.t.exe gsl-lite.t.cpp assert.t.cpp at.t.cpp byte.t.cpp issue.t.cpp not_null.t.cpp owner.t.cpp span.t.cpp string_span.t.cpp util.t.cpp && gsl-lite.t.exe --pass

cl -EHsc -I../include/gsl -Dgsl_CONFIG_CONTRACT_VIOLATION_THROWS gsl-lite.t.cpp assert.t.cpp at.t.cpp byte.t.cpp issue.t.cpp not_null.t.cpp owner.t.cpp span.t.cpp string_span.t.cpp util.t.cpp && gsl-lite.t.exe --pass

cl -EHsc -I../include/gsl -Dgsl_CONFIG_CONTRACT_VIOLATION_THROWS -Dgsl_CONFIG_CONFIRMS_COMPILATION_ERRORS gsl-lite.t.cpp assert.t.cpp at.t.cpp byte.t.cpp issue.t.cpp not_null.t.cpp owner.t.cpp span.t.cpp string_span.t.cpp util.t.cpp && gsl-lite.t.exe --pass
#endif

// end of file
