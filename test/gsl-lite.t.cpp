// Copyright (c) 2015-2019 Martin Moene
// Copyright (c) 2019-2021 Moritz Beutel
// Copyright (c) 2015-2018 Microsoft Corporation. All rights reserved.
//
// gsl-lite is based on GSL: Guidelines Support Library.
// For more information see https://github.com/gsl-lite/gsl-lite
//
// This code is licensed under the MIT License (MIT).
//

#include "gsl-lite.t.hpp"

#define gsl_PRINT_NUMERIC( x ) \
    std::cout << #x << ": " << x << "\n"

#define gsl_PRINT_BOOLEAN( x ) \
    std::cout << #x << ": " << x << "\n"

#define gsl_PRINT_TOKEN( x ) \
    std::cout << #x << ": " << gsl_STRINGIFY(x) << "\n"

#define gsl_PRESENT( x ) \
    std::cout << #x << ": (defined)" << "\n"

#define gsl_ABSENT( x ) \
    std::cout << #x << ": (undefined)\n"

#define gsl_PRINT_NO_VALUE( x ) \
    std::cout << #x << ": (no value)\n"

lest::tests & specification()
{
    static lest::tests tests;
    return tests;
}

CASE( "gsl-lite version" "[.version]" )
{
    gsl_PRINT_NUMERIC( gsl_lite_VERSION );
}

CASE( "C++ compiler: compiler version" "[.compiler]" )
{
    gsl_PRINT_NUMERIC( gsl_COMPILER_APPLECLANG_VERSION );
    gsl_PRINT_NUMERIC( gsl_COMPILER_CLANG_VERSION );
    gsl_PRINT_NUMERIC( gsl_COMPILER_GNUC_VERSION );
    gsl_PRINT_NUMERIC( gsl_COMPILER_MSVC_VERSION );
}

CASE( "__cplusplus" "[.stdc++]" )
{
    gsl_PRINT_NUMERIC( __cplusplus );
    gsl_PRINT_NUMERIC( gsl_CPLUSPLUS );
}

CASE( "_MSVC_LANG" "[.stdc++]" )
{
#ifdef _MSVC_LANG
    gsl_PRINT_NUMERIC( _MSVC_LANG );
#else
    gsl_ABSENT(        _MSVC_LANG );
#endif
}

CASE( "gsl_CPP11_OR_GREATER" "[.stdc++]" )
{
    gsl_PRINT_BOOLEAN( gsl_CPP11_OR_GREATER );
}

CASE( "gsl_CPP14_OR_GREATER" "[.stdc++]" )
{
    gsl_PRINT_BOOLEAN( gsl_CPP14_OR_GREATER );
}

CASE( "gsl_CPP17_OR_GREATER" "[.stdc++]" )
{
    gsl_PRINT_BOOLEAN( gsl_CPP17_OR_GREATER );
}

CASE( "gsl_CPP20_OR_GREATER" "[.stdc++]" )
{
    gsl_PRINT_BOOLEAN( gsl_CPP20_OR_GREATER );
}

CASE( "gsl_STDLIB_CPP11_OR_GREATER" "[.stdc++]" )
{
    gsl_PRINT_BOOLEAN( gsl_STDLIB_CPP11_OR_GREATER );
}

CASE( "gsl_STDLIB_CPP14_OR_GREATER" "[.stdc++]" )
{
    gsl_PRINT_BOOLEAN( gsl_STDLIB_CPP14_OR_GREATER );
}

CASE( "gsl_STDLIB_CPP17_OR_GREATER" "[.stdc++]" )
{
    gsl_PRINT_BOOLEAN( gsl_STDLIB_CPP17_OR_GREATER );
}

CASE( "gsl_STDLIB_CPP20_OR_GREATER" "[.stdc++]" )
{
    gsl_PRINT_BOOLEAN( gsl_STDLIB_CPP20_OR_GREATER );
}

CASE( "Presence of C++ language features" "[.stdlanguage]" )
{
    gsl_PRINT_BOOLEAN( gsl_HAVE( ALIAS_TEMPLATE ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( AUTO ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( CONSTEXPR_11 ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( CONSTEXPR_14 ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( CONSTEXPR_17 ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( CONSTEXPR_20 ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( DECLTYPE_AUTO ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( DEDUCTION_GUIDES ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( DEFAULT_FUNCTION_TEMPLATE_ARG ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( DEPRECATED ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( ENUM_CLASS ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( ENUM_CLASS_CONSTRUCTION_FROM_UNDERLYING_TYPE ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( EXCEPTIONS ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( EXPLICIT ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( EXPRESSION_SFINAE ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( FUNCTION_REF_QUALIFIER ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( INITIALIZER_LIST ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( IS_DEFAULT ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( IS_DELETE ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( NODISCARD ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( NOEXCEPT ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( NORETURN ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( NULLPTR ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( RVALUE_REFERENCE ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( VARIADIC_TEMPLATE ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( WCHAR ) );
}

CASE( "Presence of C++ library features" "[.stdlibrary]" )
{
    gsl_PRINT_BOOLEAN( gsl_HAVE( ADD_CONST ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( ADDRESSOF ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( ARRAY ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( CONTAINER_DATA_METHOD ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( INTEGRAL_CONSTANT ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( HASH ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( MAKE_SHARED ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( MAKE_UNIQUE ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( MOVE_FORWARD ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( REMOVE_CONST ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( REMOVE_CVREF ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( REMOVE_REFERENCE ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( SHARED_PTR ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( SIZED_TYPES ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( STD_DATA ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( TR1_ADD_CONST ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( TR1_INTEGRAL_CONSTANT ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( TR1_REMOVE_CONST ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( TR1_REMOVE_REFERENCE ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( TR1_TYPE_TRAITS ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( TYPE_TRAITS ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( UNCAUGHT_EXCEPTIONS ) );
    gsl_PRINT_BOOLEAN( gsl_HAVE( UNIQUE_PTR ) );

#ifdef _HAS_CPP0X
    gsl_PRINT_BOOLEAN( _HAS_CPP0X );
#else
    gsl_ABSENT(  _HAS_CPP0X );
#endif
}

CASE( "gsl-lite configuration" "[.gsl-configuration]" )
{
//#if gsl_CHECK_CFG_NO_VALUE_( gsl_api )
//    gsl_PRINT_NO_VALUE( gsl_api );
//#else
//    gsl_PRINT_TOKEN( gsl_api );
//#endif

#ifdef gsl_CONFIG_CONTRACT_CHECKING_AUDIT
    gsl_PRESENT( gsl_CONFIG_CONTRACT_CHECKING_AUDIT );
#else
    gsl_ABSENT(  gsl_CONFIG_CONTRACT_CHECKING_AUDIT );
#endif

#ifdef gsl_CONFIG_CONTRACT_CHECKING_ON
    gsl_PRESENT( gsl_CONFIG_CONTRACT_CHECKING_ON );
#else
    gsl_ABSENT(  gsl_CONFIG_CONTRACT_CHECKING_ON );
#endif

#ifdef gsl_CONFIG_CONTRACT_CHECKING_OFF
    gsl_PRESENT( gsl_CONFIG_CONTRACT_CHECKING_OFF );
#else
    gsl_ABSENT(  gsl_CONFIG_CONTRACT_CHECKING_OFF );
#endif

#ifdef gsl_CONFIG_CONTRACT_CHECKING_ENSURES_OFF
    gsl_PRESENT( gsl_CONFIG_CONTRACT_CHECKING_ENSURES_OFF );
#else
    gsl_ABSENT(  gsl_CONFIG_CONTRACT_CHECKING_ENSURES_OFF );
#endif

#ifdef gsl_CONFIG_CONTRACT_CHECKING_EXPECTS_OFF
    gsl_PRESENT( gsl_CONFIG_CONTRACT_CHECKING_EXPECTS_OFF );
#else
    gsl_ABSENT(  gsl_CONFIG_CONTRACT_CHECKING_EXPECTS_OFF );
#endif

#ifdef gsl_CONFIG_UNENFORCED_CONTRACTS_ASSUME
    gsl_PRESENT( gsl_CONFIG_UNENFORCED_CONTRACTS_ASSUME );
#else
    gsl_ABSENT(  gsl_CONFIG_UNENFORCED_CONTRACTS_ASSUME );
#endif

#ifdef gsl_CONFIG_UNENFORCED_CONTRACTS_ELIDE
    gsl_PRESENT( gsl_CONFIG_UNENFORCED_CONTRACTS_ELIDE );
#else
    gsl_ABSENT(  gsl_CONFIG_UNENFORCED_CONTRACTS_ELIDE );
#endif

#ifdef gsl_CONFIG_CONTRACT_VIOLATION_TERMINATES
    gsl_PRESENT( gsl_CONFIG_CONTRACT_VIOLATION_TERMINATES );
#else
    gsl_ABSENT(  gsl_CONFIG_CONTRACT_VIOLATION_TERMINATES );
#endif

#ifdef gsl_CONFIG_CONTRACT_VIOLATION_THROWS
    gsl_PRESENT( gsl_CONFIG_CONTRACT_VIOLATION_THROWS );
#else
    gsl_ABSENT(  gsl_CONFIG_CONTRACT_VIOLATION_THROWS );
#endif

#ifdef gsl_CONFIG_CONTRACT_VIOLATION_CALLS_HANDLER
    gsl_PRESENT( gsl_CONFIG_CONTRACT_VIOLATION_CALLS_HANDLER );
#else
    gsl_ABSENT(  gsl_CONFIG_CONTRACT_VIOLATION_CALLS_HANDLER );
#endif

    gsl_PRINT_BOOLEAN( gsl_CONFIG( TRANSPARENT_NOT_NULL ) );
    gsl_PRINT_BOOLEAN( gsl_CONFIG_DEPRECATE_TO_LEVEL );
    gsl_PRINT_TOKEN( gsl_CONFIG( SPAN_INDEX_TYPE ) );
    gsl_PRINT_TOKEN( gsl_CONFIG( INDEX_TYPE ) );
    gsl_PRINT_BOOLEAN( gsl_CONFIG( NOT_NULL_EXPLICIT_CTOR ) );
    gsl_PRINT_BOOLEAN( gsl_CONFIG( NOT_NULL_GET_BY_CONST_REF ) );
    gsl_PRINT_BOOLEAN( gsl_CONFIG( CONFIRMS_COMPILATION_ERRORS ) );
    gsl_PRINT_BOOLEAN( gsl_CONFIG( ALLOWS_SPAN_COMPARISON ) );
    gsl_PRINT_BOOLEAN( gsl_CONFIG( ALLOWS_NONSTRICT_SPAN_COMPARISON ) );
    gsl_PRINT_BOOLEAN( gsl_CONFIG( ALLOWS_UNCONSTRAINED_SPAN_CONTAINER_CTOR ) );
    gsl_PRINT_BOOLEAN( gsl_CONFIG( NARROW_THROWS_ON_TRUNCATION ) );
}

CASE( "gsl-lite features" "[.gsl-features]" )
{
    gsl_PRINT_BOOLEAN( gsl_FEATURE( WITH_CONTAINER_TO_STD ) );
    gsl_PRINT_BOOLEAN( gsl_FEATURE( MAKE_SPAN_TO_STD ) );
    gsl_PRINT_BOOLEAN( gsl_FEATURE( BYTE_SPAN_TO_STD ) );
    gsl_PRINT_BOOLEAN( gsl_FEATURE( IMPLICIT_MACRO ) );
    gsl_PRINT_BOOLEAN( gsl_FEATURE( OWNER_MACRO ) );
    gsl_PRINT_BOOLEAN( gsl_FEATURE( EXPERIMENTAL_RETURN_GUARD ) );
    gsl_PRINT_BOOLEAN( gsl_FEATURE( GSL_LITE_NAMESPACE ) );
}

int main( int argc, char * argv[] )
{
        // We only run tests for targets with exceptions enabled. lest has been modified to permit compilation without exceptions
        // so we can test compiling gsl-lite without exceptions, but the no-exception tests will not run correctly because lest
        // relies on exceptions for running tests and therefore cannot function correctly without.
    if ( ! gsl_HAVE( EXCEPTIONS ) )
    {
        std::cerr << "Cannot run test suite with exceptions disabled; exiting.\n";
        return 1;
    }

    return lest::run( specification(), argc, argv );
}

#if defined( gsl_CONFIG_CONTRACT_VIOLATION_CALLS_HANDLER )
namespace gsl {

gsl_api void fail_fast_assert_handler( char const * const expression, char const * const message, char const * const file, int line )
{
    std::cerr << message << ": " << expression << " at " << file << ":" << line << "\n";
}

} // namespace gsl
#endif

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
