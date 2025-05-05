//
// For more information see https://github.com/gsl-lite/gsl-lite
//
// gsl-lite is originally based on Microsoft GSL, which is an implementation of the C++ Core Guidelines Support Library:
// https://github.com/microsoft/GSL
//
// Copyright (c) 2015-2019 Martin Moene
// Copyright (c) 2019-2025 Moritz Beutel
// Copyright (c) 2015-2018 Microsoft Corporation. All rights reserved.
//
// This code is licensed under the MIT License (MIT).
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef GSL_GSL_LITE_HPP_INCLUDED
#define GSL_GSL_LITE_HPP_INCLUDED

#include <gsl-lite/gsl-lite.hpp>

#if ! gsl_FEATURE( GSL_COMPATIBILITY_MODE )

# pragma message (__FILE__ "(" gsl_STRINGIFY( __LINE__ ) "): warning: The legacy header file <gsl/gsl-lite.hpp> implicitly enables gsl-lite's GSL compatibility mode, making namespace gsl and the Expects() and Ensures() macros available for backward compatibility. If possible, modernize your code by including the <gsl-lite/gsl-lite.hpp> header file, referring to namespace gsl_lite rather than namespace gsl, and using the prefixed contract checking macros gsl_Expects() and gsl_Ensures() instead of the unprefixed Expects() and Ensures(). If you wish to explicitly retain the GSL compatibility mode, define the macro gsl_FEATURE_GSL_COMPATIBILITY_MODE=1 in the build options.")

// Implicitly enable the GSL compatbility mode.
# undef gsl_FEATURE_GSL_COMPATIBILITY_MODE
# define gsl_FEATURE_GSL_COMPATIBILITY_MODE  1

namespace gsl = ::gsl_lite;

//# if gsl_HAVE( C99_PREPROCESSOR )
//#  define gsl_DEPRECATED_PRAGMA_EXPECTS_MSG_  _Pragma("message(\"warning: gsl-lite defines the Expects() macro in GSL compatibility mode only; please use the prefixed macro gsl_Expects() instead or enable GSL compatibility mode by defining the macro gsl_FEATURE_GSL_COMPATIBILITY_MODE=1 in the build options\")")
//#  define gsl_DEPRECATED_PRAGMA_ENSURES_MSG_  _Pragma("message(\"warning: gsl-lite defines the Ensures() macro in GSL compatibility mode only; please use the prefixed macro gsl_Ensures() instead or enable GSL compatibility mode by defining the macro gsl_FEATURE_GSL_COMPATIBILITY_MODE=1 in the build options\")")
//# elif defined( _MSC_VER )
//#  define gsl_DEPRECATED_PRAGMA_EXPECTS_MSG_  __pragma("message(\"warning: gsl-lite defines the Expects() macro in GSL compatibility mode only; please use the prefixed macro gsl_Expects() instead or enable GSL compatibility mode by defining the macro gsl_FEATURE_GSL_COMPATIBILITY_MODE=1 in the build options\")")
//#  define gsl_DEPRECATED_PRAGMA_ENSURES_MSG_  __pragma("message(\"warning: gsl-lite defines the Ensures() macro in GSL compatibility mode only; please use the prefixed macro gsl_Ensures() instead or enable GSL compatibility mode by defining the macro gsl_FEATURE_GSL_COMPATIBILITY_MODE=1 in the build options\")")
//# else
//#  define gsl_DEPRECATED_PRAGMA_EXPECTS_MSG_
//#  define gsl_DEPRECATED_PRAGMA_ENSURES_MSG_
//# endif 
//
//# define  Expects( x )           gsl_DEPRECATED_PRAGMA_EXPECTS_MSG_ gsl_Expects( x )
//# define  Ensures( x )           gsl_DEPRECATED_PRAGMA_ENSURES_MSG_ gsl_Ensures( x )

# define  Expects( x )           gsl_Expects( x )
# define  Ensures( x )           gsl_Ensures( x )

#endif // ! gsl_FEATURE( GSL_COMPATIBILITY_MODE )

#endif // GSL_GSL_LITE_HPP_INCLUDED

// end of file
