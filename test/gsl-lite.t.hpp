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

// GSL-Lite only depends on <ios>, but we're instantiating templates using streams, so we need <ostream>
#include <ostream>

namespace lest {

// These functions cannot be found via ADL, so we have to define them before including lest.

#if gsl_HAVE( ARRAY )
template< typename T, std::size_t N >
inline std::ostream & operator<<( std::ostream & os, std::array<T,N> const & a )
{
    return os << std::hex << "[std::array[" << N << "]";
}
#endif

#if gsl_HAVE( WCHAR )
// We do this with a loop and explicit casts to avoid warnings about implicit narrowing casts (which we don't care about because we don't have to handle non-ASCII strings in the tests).
inline std::string narrowString( std::wstring const & str )
{
    std::string result(str.size(), '\0');
    for (std::size_t i = 0, n = str.size(); i != n; ++i)
    {
        result[i] = static_cast<char>(str[i]);
    }
    return result;
}

inline std::ostream & operator<<( std::ostream & os, std::wstring const & text )
{
    return os << narrowString( text );
}
#endif // gsl_HAVE( WCHAR )

} // namespace lest

#include "lest_cpp03.hpp"

#define CASE( name ) lest_CASE( specification(), name )

extern lest::tests & specification();

namespace gsl {

inline const void * nullptr_void() { return gsl_nullptr; }

// use operator<< instead of to_string() overload;
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
