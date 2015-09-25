// Copyright 2015 by Martin Moene
//
// gsl-lite is based on GSL: Guidelines Support Library,
// https://github.com/microsoft/gsl
//
// This code is licensed under the MIT License (MIT). 
//

#pragma once

#if defined(_MSC_VER)
# define gsl_COMPILER_MSVC_VERSION   ((_MSC_VER - 600 ) / 100)
#else
# define gsl_COMPILER_MSVC_VERSION   0
# define gsl_COMPILER_NON_MSVC       1
#endif

#if ( __cplusplus >= 201103L ) || gsl_COMPILER_MSVC_VERSION >= 12
# define gsl_CPP11_OR_GREATER  1
#endif

// half-open range [lo..hi):
#define gsl_BETWEEN( v, lo, hi ) ( lo <= v && v < hi )

#if gsl_CPP11_OR_GREATER
#define gsl_HAVE_DEFAULT_FUNCTION_TEMPLATE_ARG  1
#endif

#if gsl_CPP11_OR_GREATER
# define gsl_HAVE_SHARED_PTR  1
# define gsl_HAVE_UNIQUE_PTR  1
#endif

#if gsl_CPP11_OR_GREATER || gsl_COMPILER_MSVC_VERSION >= 10
# define gsl_HAVE_NULLPTR  1
#endif

#if gsl_COMPILER_MSVC_VERSION == 6
# define gsl_QUAL_NS_STD(name) name
#else
# define gsl_QUAL_NS_STD(name) std::name
#endif

// end of file
