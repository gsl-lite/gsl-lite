//
// gsl-lite is based on GSL: Guidelines Support Library.
// For more information see https://github.com/gsl-lite/gsl-lite
//
// Copyright (c) 2015-2019 Martin Moene
// Copyright (c) 2019-2021 Moritz Beutel
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


#include "gsl-lite.t.hpp"

using namespace gsl;


#if gsl_HAVE( TYPE_TRAITS ) && gsl_HAVE( VARIADIC_TEMPLATE )
template <int> struct True : std::true_type { };
template <int> struct False : std::false_type { };

struct Incomplete;
#endif // gsl_HAVE( TYPE_TRAITS ) && gsl_HAVE( VARIADIC_TEMPLATE )


CASE( "equal()" )
{
    char const* arg1 = "foo";
    char const* arg2 = "fou";
    EXPECT(   detail::equal( arg1, arg1 + std::strlen( arg1 ), arg1 ) );
    EXPECT( ! detail::equal( arg1, arg1 + std::strlen( arg1 ), arg2 ) );
    EXPECT( ! detail::equal( arg2, arg2 + std::strlen( arg2 ), arg1 ) );

    std::istringstream sstr1a1( "foo" ), sstr1a2( "foo" ), sstr1b( "foo" ), sstr1c( "foo" );
    std::istringstream sstr2b( "fou" ), sstr2c( "fou" );
    EXPECT(   detail::equal( std::istreambuf_iterator<char>( sstr1a1 ), std::istreambuf_iterator<char>(), std::istreambuf_iterator<char>( sstr1a2 ) ) );
    EXPECT( ! detail::equal( std::istreambuf_iterator<char>( sstr1b ), std::istreambuf_iterator<char>(), std::istreambuf_iterator<char>( sstr2b ) ) );
    EXPECT( ! detail::equal( std::istreambuf_iterator<char>( sstr2c ), std::istreambuf_iterator<char>(), std::istreambuf_iterator<char>( sstr1c ) ) );
}

template < typename T, std::size_t N >
std::size_t arraySize( T (&)[N] )
{
    return N;
}

CASE( "lexicographical_compare()" )
{
    std::pair<char const*, char const*> less[] = {
        std::make_pair("", "foo"),
        std::make_pair("fa", "foo"),
        std::make_pair("foe", "foo"),
        std::make_pair("fond", "foo")
    };
    for ( std::size_t i = 0, n = arraySize( less ); i != n; ++i )
    {
        EXPECT( detail::lexicographical_compare( less[i].first, less[i].first + std::strlen( less[i].first ), less[i].second, less[i].second + std::strlen( less[i].second ) ));

        std::istringstream sstr1( less[i].first );
        std::istringstream sstr2( less[i].second );
        std::istreambuf_iterator<char> it1( sstr1 ), it1End;
        std::istreambuf_iterator<char> it2( sstr2 ), it2End;
        EXPECT( detail::lexicographical_compare( it1, it1End, it2, it2End ));
    }

    std::pair<char const*, char const*> notLess[] = {
        std::make_pair("g", "foo"),
        std::make_pair("fu", "foo"),
        std::make_pair("foo", "foo"),
        std::make_pair("fou", "foo"),
        std::make_pair("fool", "foo"),
        std::make_pair("foul", "foo")
    };
    for ( std::size_t i = 0, n = arraySize( less ); i != n; ++i )
    {
        EXPECT( ! detail::lexicographical_compare( notLess[i].first, notLess[i].first + std::strlen( notLess[i].first ), notLess[i].second, notLess[i].second + std::strlen( notLess[i].second ) ));

        std::istringstream sstr1( notLess[i].first );
        std::istringstream sstr2( notLess[i].second );
        std::istreambuf_iterator<char> it1( sstr1 ), it1End;
        std::istreambuf_iterator<char> it2( sstr2 ), it2End;
        EXPECT( ! detail::lexicographical_compare( it1, it1End, it2, it2End ));
    }
}

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
