//
// gsl-lite is based on GSL: Guidelines Support Library.
// For more information see https://github.com/martinmoene/gsl-lite
//
// Copyright (c) 2015 Martin Moene
// Copyright (c) 2015 Microsoft Corporation. All rights reserved.
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

#include "gsl-lite.t.hpp"

using namespace gsl;


#if gsl_HAVE( TYPE_TRAITS ) && gsl_HAVE( VARIADIC_TEMPLATE )
template <int> struct True : std::true_type { };
template <int> struct False : std::false_type { };

struct Incomplete;
#endif // gsl_HAVE( TYPE_TRAITS ) && gsl_HAVE( VARIADIC_TEMPLATE )


CASE( "conjunction<> and disjunction<>: Short-circuiting is handled correctly" )
{
#if gsl_HAVE( TYPE_TRAITS ) && gsl_HAVE( VARIADIC_TEMPLATE )
    static_assert(std::is_base_of<True<0>, std17::disjunction<True<0>, Incomplete>>::value, "static assertion failed");
    static_assert(std::is_base_of<False<0>, std17::conjunction<False<0>, Incomplete>>::value, "static assertion failed");
#endif // gsl_HAVE( TYPE_TRAITS ) && gsl_HAVE( VARIADIC_TEMPLATE )
}

CASE( "conjunction<> and disjunction<>: First suitable type is chosen as base" )
{
#if gsl_HAVE( TYPE_TRAITS ) && gsl_HAVE( VARIADIC_TEMPLATE )
    static_assert(std::is_base_of<std::false_type, std17::disjunction<>>::value, "static assertion failed");
    static_assert(std::is_base_of<True<0>, std17::disjunction<True<0>>>::value, "static assertion failed");
    static_assert(std::is_base_of<False<0>, std17::disjunction<False<0>>>::value, "static assertion failed");
    static_assert(std::is_base_of<True<0>, std17::disjunction<True<0>, True<1>>>::value, "static assertion failed");
    static_assert(std::is_base_of<True<0>, std17::disjunction<True<0>, True<1>, True<2>>>::value, "static assertion failed");
    static_assert(std::is_base_of<True<1>, std17::disjunction<False<0>, True<1>, True<2>>>::value, "static assertion failed");
    static_assert(std::is_base_of<False<2>, std17::disjunction<False<0>, False<1>, False<2>>>::value, "static assertion failed");
    static_assert(std::is_base_of<std::true_type, std17::conjunction<>>::value, "static assertion failed");
    static_assert(std::is_base_of<True<0>, std17::conjunction<True<0>>>::value, "static assertion failed");
    static_assert(std::is_base_of<False<0>, std17::conjunction<False<0>>>::value, "static assertion failed");
    static_assert(std::is_base_of<True<1>, std17::conjunction<True<0>, True<1>>>::value, "static assertion failed");
    static_assert(std::is_base_of<True<2>, std17::conjunction<True<0>, True<1>, True<2>>>::value, "static assertion failed");
    static_assert(std::is_base_of<False<1>, std17::conjunction<True<0>, False<1>, False<2>>>::value, "static assertion failed");
    static_assert(std::is_base_of<False<0>, std17::conjunction<False<0>, False<1>, False<2>>>::value, "static assertion failed");
#endif // gsl_HAVE( TYPE_TRAITS ) && gsl_HAVE( VARIADIC_TEMPLATE )
}
