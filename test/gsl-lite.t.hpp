// Copyright 2015 by Martin Moene
//
// gsl-lite is based on GSL: Guidelines Support Library.
// For more information see https://github.com/martinmoene/gsl-lite
//
// This code is licensed under the MIT License (MIT).
//

#pragma once

#ifndef GSL_TEST_GSL_LITE_HPP_INCLUDED
#define GSL_TEST_GSL_LITE_HPP_INCLUDED

// Select GSL Lite for VC6 if compiling with VC6:

#if defined(_MSC_VER) && _MSC_VER < 1300
# include "gsl/gsl-lite-vc6.hpp"
#else
# include "gsl/gsl-lite.hpp"
#endif

// Limit C++ Core Guidelines checking to GSL Lite:

#if defined(_MSC_VER) && _MSC_VER >= 1910
# include <CppCoreCheck/warnings.h>
# pragma warning(disable: ALL_CPPCORECHECK_WARNINGS)
#endif

// Compiler warning suppression for usage of lest:

#ifdef __clang__
# pragma clang diagnostic ignored "-Wstring-conversion"
# pragma clang diagnostic ignored "-Wunused-parameter"
# pragma clang diagnostic ignored "-Wunused-template"
# pragma clang diagnostic ignored "-Wunused-function"
# pragma clang diagnostic ignored "-Wunused-member-function"
#elif defined __GNUC__
# pragma GCC   diagnostic ignored "-Wunused-parameter"
# pragma GCC   diagnostic ignored "-Wunused-function"
#endif

#include "lest_cpp03.hpp"

#define CASE( name ) lest_CASE( specification(), name )

extern lest::tests & specification();

#if ! gsl_BETWEEN( gsl_COMPILER_MSVC_VERSION, 60, 70 )
namespace std {
#else
namespace lest {
#endif

#if gsl_HAVE( ARRAY )
template< typename T, std::size_t N >
inline std::ostream & operator<<( std::ostream & os, std::array<T,N> const & a )
{
    return os << std::hex << "[std::array[" << N << "]";
}
#endif

#if gsl_HAVE( WCHAR )
inline std::ostream & operator<<( std::ostream & os, std::wstring const & text )
{
#if ! gsl_BETWEEN( gsl_COMPILER_MSVC_VERSION, 60, 70 )
    return os << std::string( text.begin(), text.end() );
#else
    std::copy( text.begin(), text.end(), std::ostream_iterator<char>(os) ); return os;
#endif
}
#endif // gsl_HAVE( WCHAR )

#if ! gsl_BETWEEN( gsl_COMPILER_MSVC_VERSION, 60, 70 )
} // namespace std
#else
} // namespace lest
#endif

namespace gsl {

inline const void * nullptr_void() { return gsl_nullptr; }

// use oparator<< instead of to_string() overload;
// see  http://stackoverflow.com/a/10651752/437272

inline std::ostream & operator<<( std::ostream & os, byte b )
{
    return os << std::hex << "0x" << to_integer<int>(b);
}

template< typename T >
inline std::ostream & operator<<( std::ostream & os, span<T> s )
{
    return os << "[", std::copy( s.begin(), s.end(), std::ostream_iterator<T>(os, ",") ), os << "]";
}

} // namespace gsl

inline void suppress_warning_unused_template_ensure_sentinel()
{
    (void) gsl::ensure_z( "zero-terminated" );
}

#endif // GSL_TEST_GSL_LITE_HPP_INCLUDED

// end of file
