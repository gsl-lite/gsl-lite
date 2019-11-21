// Copyright 2015 by Martin Moene
//
// gsl-lite is based on GSL: Guidelines Support Library.
// For more information see https://github.com/martinmoene/gsl-lite
//
// This code is licensed under the MIT License (MIT).
//

#include "gsl-lite.t.hpp"

#define gsl_PRESENT( x ) \
    std::cout << #x << ": " << x << "\n"

#define gsl_PRESENT_VALUELESS( x ) \
    std::cout << #x << ": (defined)" << "\n"

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

CASE( "C++ compiler: compiler version" "[.compiler]" )
{
#ifdef gsl_COMPILER_APPLECLANG_VERSION
    gsl_PRESENT( gsl_COMPILER_APPLECLANG_VERSION );
#else
    gsl_ABSENT(  gsl_COMPILER_APPLECLANG_VERSION );
#endif

#ifdef gsl_COMPILER_CLANG_VERSION
    gsl_PRESENT( gsl_COMPILER_CLANG_VERSION );
#else
    gsl_ABSENT(  gsl_COMPILER_CLANG_VERSION );
#endif

#ifdef gsl_COMPILER_GNUC_VERSION
    gsl_PRESENT( gsl_COMPILER_GNUC_VERSION );
#else
    gsl_ABSENT(  gsl_COMPILER_GNUC_VERSION );
#endif

#ifdef gsl_COMPILER_MSVC_VERSION
    gsl_PRESENT( gsl_COMPILER_MSVC_VERSION );
#else
    gsl_ABSENT(  gsl_COMPILER_MSVC_VERSION );
#endif
}

CASE( "__cplusplus" "[.stdc++]" )
{
    gsl_PRESENT( __cplusplus );
    gsl_PRESENT( gsl_CPLUSPLUS );
}

CASE( "_MSVC_LANG" "[.stdc++]" )
{
#ifdef _MSVC_LANG
    gsl_PRESENT( _MSVC_LANG );
#else
    gsl_ABSENT(  _MSVC_LANG );
#endif
}

CASE( "gsl_CPP11_OR_GREATER" "[.stdc++]" )
{
    gsl_PRESENT( gsl_CPP11_OR_GREATER );
}

CASE( "gsl_CPP14_OR_GREATER" "[.stdc++]" )
{
    gsl_PRESENT( gsl_CPP14_OR_GREATER );
}

CASE( "gsl_CPP17_OR_GREATER" "[.stdc++]" )
{
    gsl_PRESENT( gsl_CPP17_OR_GREATER );
}

CASE( "Presence of C++ language features" "[.stdlanguage]" )
{
    gsl_PRESENT( gsl_HAVE_ALIAS_TEMPLATE );
    gsl_PRESENT( gsl_HAVE_AUTO );
    gsl_PRESENT( gsl_HAVE_CONSTEXPR_11 );
    gsl_PRESENT( gsl_HAVE_CONSTEXPR_14 );
    gsl_PRESENT( gsl_HAVE_DECLTYPE_AUTO );
    gsl_PRESENT( gsl_HAVE_DEDUCTION_GUIDES );
    gsl_PRESENT( gsl_HAVE_DEFAULT_FUNCTION_TEMPLATE_ARG );
    gsl_PRESENT( gsl_HAVE_ENUM_CLASS );
    gsl_PRESENT( gsl_HAVE_ENUM_CLASS_CONSTRUCTION_FROM_UNDERLYING_TYPE );
    gsl_PRESENT( gsl_HAVE_EXPLICIT );
    gsl_PRESENT( gsl_HAVE_INITIALIZER_LIST );
    gsl_PRESENT( gsl_HAVE_IS_DEFAULT );
    gsl_PRESENT( gsl_HAVE_IS_DELETE );
    gsl_PRESENT( gsl_HAVE_NOEXCEPT );
    gsl_PRESENT( gsl_HAVE_NULLPTR );
    gsl_PRESENT( gsl_HAVE_RVALUE_REFERENCE );
    gsl_PRESENT( gsl_HAVE_WCHAR );
}

CASE( "Presence of C++ library features" "[.stdlibrary]" )
{
    gsl_PRESENT( gsl_HAVE_ADD_CONST );
    gsl_PRESENT( gsl_HAVE_ADDRESSOF );
    gsl_PRESENT( gsl_HAVE_ARRAY );
    gsl_PRESENT( gsl_HAVE_CONTAINER_DATA_METHOD );
    gsl_PRESENT( gsl_HAVE_STD_DATA );
    gsl_PRESENT( gsl_HAVE_MAKE_SHARED );
    gsl_PRESENT( gsl_HAVE_MAKE_UNIQUE );
    gsl_PRESENT( gsl_HAVE_REMOVE_CONST );
    gsl_PRESENT( gsl_HAVE_SHARED_PTR );
    gsl_PRESENT( gsl_HAVE_SIZED_TYPES );
    gsl_PRESENT( gsl_HAVE_TR1_ADD_CONST );
    gsl_PRESENT( gsl_HAVE_TYPE_TRAITS );
    gsl_PRESENT( gsl_HAVE_UNIQUE_PTR );

#ifdef _HAS_CPP0X
    gsl_PRESENT( _HAS_CPP0X );
#else
    gsl_ABSENT(  _HAS_CPP0X );
#endif
}

CASE( "gsl-lite configuration" "[.gsl-configuration]" )
{
//#ifdef gsl_api
//    gsl_PRESENT( gsl_api );
//#else
//    gsl_ABSENT(  gsl_api );
//#endif

#ifdef gsl_CONFIG_CONTRACT_CHECKING_AUDIT
    gsl_PRESENT_VALUELESS( gsl_CONFIG_CONTRACT_CHECKING_AUDIT );
#else
    gsl_ABSENT(            gsl_CONFIG_CONTRACT_CHECKING_AUDIT );
#endif

#ifdef gsl_CONFIG_CONTRACT_CHECKING_ON
    gsl_PRESENT_VALUELESS( gsl_CONFIG_CONTRACT_CHECKING_ON );
#else
    gsl_ABSENT(            gsl_CONFIG_CONTRACT_CHECKING_ON );
#endif

#ifdef gsl_CONFIG_CONTRACT_CHECKING_OFF
    gsl_PRESENT_VALUELESS( gsl_CONFIG_CONTRACT_CHECKING_OFF );
#else
    gsl_ABSENT(            gsl_CONFIG_CONTRACT_CHECKING_OFF );
#endif

#ifdef gsl_CONFIG_CONTRACT_CHECKING_ENSURES_OFF
    gsl_PRESENT_VALUELESS( gsl_CONFIG_CONTRACT_CHECKING_ENSURES_OFF );
#else
    gsl_ABSENT(            gsl_CONFIG_CONTRACT_CHECKING_ENSURES_OFF );
#endif

#ifdef gsl_CONFIG_CONTRACT_CHECKING_EXPECTS_OFF
    gsl_PRESENT_VALUELESS( gsl_CONFIG_CONTRACT_CHECKING_EXPECTS_OFF );
#else
    gsl_ABSENT(            gsl_CONFIG_CONTRACT_CHECKING_EXPECTS_OFF );
#endif

#ifdef gsl_CONFIG_UNENFORCED_CONTRACTS_ASSUME
    gsl_PRESENT_VALUELESS( gsl_CONFIG_UNENFORCED_CONTRACTS_ASSUME );
#else
    gsl_ABSENT(            gsl_CONFIG_UNENFORCED_CONTRACTS_ASSUME );
#endif

#ifdef gsl_CONFIG_UNENFORCED_CONTRACTS_ELIDE
    gsl_PRESENT_VALUELESS( gsl_CONFIG_UNENFORCED_CONTRACTS_ELIDE );
#else
    gsl_ABSENT(            gsl_CONFIG_UNENFORCED_CONTRACTS_ELIDE );
#endif

#ifdef gsl_CONFIG_CONTRACT_VIOLATION_TERMINATES
    gsl_PRESENT_VALUELESS( gsl_CONFIG_CONTRACT_VIOLATION_TERMINATES );
#else
    gsl_ABSENT(            gsl_CONFIG_CONTRACT_VIOLATION_TERMINATES );
#endif

#ifdef gsl_CONFIG_CONTRACT_VIOLATION_THROWS
    gsl_PRESENT_VALUELESS( gsl_CONFIG_CONTRACT_VIOLATION_THROWS );
#else
    gsl_ABSENT(            gsl_CONFIG_CONTRACT_VIOLATION_THROWS );
#endif

#ifdef gsl_CONFIG_CONTRACT_VIOLATION_CALLS_HANDLER
    gsl_PRESENT_VALUELESS( gsl_CONFIG_CONTRACT_VIOLATION_CALLS_HANDLER );
#else
    gsl_ABSENT(            gsl_CONFIG_CONTRACT_VIOLATION_CALLS_HANDLER );
#endif
}

CASE( "gsl-lite features" "[.gsl-features]" )
{
#ifdef gsl_FEATURE_WITH_CONTAINER_TO_STD
    gsl_PRESENT( gsl_FEATURE_WITH_CONTAINER_TO_STD );
#else
    gsl_ABSENT(  gsl_FEATURE_WITH_CONTAINER_TO_STD );
#endif

#ifdef gsl_FEATURE_MAKE_SPAN_TO_STD
    gsl_PRESENT( gsl_FEATURE_MAKE_SPAN_TO_STD );
#else
    gsl_ABSENT(  gsl_FEATURE_MAKE_SPAN_TO_STD );
#endif

#ifdef gsl_FEATURE_BYTE_SPAN_TO_STD
    gsl_PRESENT( gsl_FEATURE_BYTE_SPAN_TO_STD );
#else
    gsl_ABSENT(  gsl_FEATURE_BYTE_SPAN_TO_STD );
#endif

#ifdef gsl_FEATURE_IMPLICIT_MACRO
    gsl_PRESENT( gsl_FEATURE_IMPLICIT_MACRO );
#else
    gsl_ABSENT(  gsl_FEATURE_IMPLICIT_MACRO );
#endif

#ifdef gsl_FEATURE_OWNER_MACRO
    gsl_PRESENT( gsl_FEATURE_OWNER_MACRO );
#else
    gsl_ABSENT(  gsl_FEATURE_OWNER_MACRO );
#endif

#ifdef gsl_FEATURE_EXPERIMENTAL_RETURN_GUARD
    gsl_PRESENT( gsl_FEATURE_EXPERIMENTAL_RETURN_GUARD );
#else
    gsl_ABSENT(  gsl_FEATURE_EXPERIMENTAL_RETURN_GUARD );
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
