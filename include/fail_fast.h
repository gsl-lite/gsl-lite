// Copyright 2015 by Martin Moene
//
// gsl-lite is based on GSL: Guidelines Support Library,
// https://github.com/microsoft/gsl
//
// This code is licensed under the MIT License (MIT). 
//

#pragma once

#include "config.h"
#include <exception>
#include <stdexcept>

namespace Guide
{

//
// Having "fail fast" result in an exception makes unit testing
// the GSL classes that rely upon it much simpler. 
//
#if defined(gsl_THROW_ON_FAILURE)

struct fail_fast : public std::runtime_error 
{
	fail_fast() : std::runtime_error("GSL failure") {}
	explicit fail_fast(char const* const message) : std::runtime_error(message) {}
};

inline void fail_fast_assert(bool cond) { if (!cond) throw fail_fast(); }
inline void fail_fast_assert(bool cond, const char* const message) { if (!cond) throw fail_fast(message); }

#else

inline void fail_fast_assert(bool cond) { if (!cond) gsl_QUAL_NS_STD(terminate)(); }
inline void fail_fast_assert(bool cond, const char* const) { if (!cond) gsl_QUAL_NS_STD(terminate)(); }

#endif // gsl_THROW_ON_FAILURE

}

// end of file
