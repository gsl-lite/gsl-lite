// Copyright 2015 by Martin Moene
//
// gsl-lite is based on GSL: Guideline Support Library.
// For more information see https://github.com/martinmoene/gsl-lite
//
// This code is licensed under the MIT License (MIT).
//

#pragma once

#ifndef GSL_TEST_GSL_LITE_H_INCLUDED
#define GSL_TEST_GSL_LITE_H_INCLUDED

#if defined(_MSC_VER) && _MSC_VER < 1300
# include "gsl-lite-vc6.h"
#else
# include "gsl-lite.h"
#endif

#include "lest_cpp03.hpp"

using namespace gsl;

#define CASE( name ) lest_CASE( specification(), name )

extern lest::tests & specification();

#if ! gsl_BETWEEN( gsl_COMPILER_MSVC_VERSION, 6, 7 )
namespace std {
#else
namespace lest {
#endif

#if gsl_HAVE_ARRAY
template< typename T, std::size_t N >
inline std::ostream & operator<<( std::ostream & os, std::array<T,N> const & a )
{
    return os << std::hex << "[std::array[" << N << "]";
}
#endif

inline std::ostream & operator<<( std::ostream & os, std::wstring const & text )
{
#if ! gsl_BETWEEN( gsl_COMPILER_MSVC_VERSION, 6, 7 )
    return os << std::string( text.begin(), text.end() );
#else
    std::copy( text.begin(), text.end(), std::ostream_iterator<char>(os) ); return os;
#endif
}

#if ! gsl_BETWEEN( gsl_COMPILER_MSVC_VERSION, 6, 7 )
} // namespace std
#else
} // namespace lest
#endif

namespace gsl {
    
inline const void * nullptr_void() { return 0; }

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

template< typename T >
inline std::ostream & operator<<( std::ostream & os, basic_string_span<T> s )
{
    return os << "[", std::copy( s.begin(), s.end(), std::ostream_iterator<T>(os, ",") ), os << "]";
}

} // namespace gsl

#endif // GSL_TEST_GSL_LITE_H_INCLUDED

// end of file
