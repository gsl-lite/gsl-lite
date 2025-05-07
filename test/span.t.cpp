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

using namespace gsl_lite;

typedef span<int>::size_type size_type;
typedef std::ptrdiff_t       difference_type;


static std::vector<int> vector_iota( int n )
{
  std::vector<int> result;

  for ( int i = 0; i < n; ++i )
    result.push_back( i );

  return result;
}

CASE( "span<>: Disallows construction from a temporary value (C++11) (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)" )
{
#if gsl_CONFIG( CONFIRMS_COMPILATION_ERRORS )
# if gsl_HAVE( IS_DELETE )
    span<int> v = 42;
# endif
#endif
}

CASE( "span<>: Disallows construction from a C-array of incompatible type (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)" )
{
#if gsl_CONFIG( CONFIRMS_COMPILATION_ERRORS )
    short arr[] = { 1, 2, 3, };
    span<int> v = arr;
#endif
}

CASE( "span<>: Disallows construction from a std::array of incompatible type (C++11) (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)" )
{
#if gsl_CONFIG( CONFIRMS_COMPILATION_ERRORS )
# if gsl_HAVE( ARRAY )
    std::array<long,3> arr = {{ 1L, 2L, 3L, }};
    span<int> v( arr);
# endif
#endif
}

CASE( "span<>: Terminates construction with a mismatching size" )
{
    struct F { static void blow() { int a[2]; span<int, 3> v( a, 2 ); (void) v.size(); } };
    struct G { static void blow() { int a[2]; span<int, 3> v( a, a + 2 ); (void) v.size(); } };

    EXPECT_THROWS( F::blow() );
    EXPECT_THROWS( G::blow() );
}

CASE( "span<>: Terminates construction from a nullptr and a non-zero size (C++11)" )
{
#if gsl_HAVE( NULLPTR )
    struct F { static void blow() { span<int> v( nullptr, 42 ); (void) v.size(); } };
    struct G { static void blow() { span<int, 42> v( nullptr, 42 ); (void) v.size(); } };

    EXPECT_THROWS( F::blow() );
    EXPECT_THROWS( G::blow() );
#else
    EXPECT( !!"nullptr is not available (no C++11)" );
#endif
}

CASE( "span<>: Terminates construction from two pointers in the wrong order" )
{
    struct F { static void blow() { int a[2]; span<int> v( &a[1], &a[0] ); (void) v.size(); } };
    struct G { static void blow() { int a[2]; span<int, 2> v( &a[1], &a[0] ); (void) v.size(); } };

    EXPECT_THROWS( F::blow() );
    EXPECT_THROWS( G::blow() );
}

CASE( "span<>: Terminates construction from a null pointer and a non-zero size" )
{
    struct F { static void blow() { int * p = gsl_nullptr; span<int> v( p, 42 ); (void) v.size(); } };
    struct G { static void blow() { int * p = gsl_nullptr; span<int, 42> v( p, 42 ); (void) v.size(); } };

    EXPECT_THROWS( F::blow() );
    EXPECT_THROWS( G::blow() );
}

CASE( "span<>: Terminates creation of a sub span of the first n elements for n exceeding the span" )
{
    struct F {
    static void blow()
    {
        int arr[] = { 1, 2, 3, };
        span<int> v( arr );

        (void) v.first( 4 );
    }};
    struct G {
    static void blow()
    {
        int arr[] = { 1, 2, 3, };
        span<int, 3> v( arr );

        (void) v.first( 4 );
    }};

    EXPECT_THROWS( F::blow() );
    EXPECT_THROWS( G::blow() );
}

CASE( "span<>: Terminates creation of a sub span of the last n elements for n exceeding the span" )
{
    struct F {
    static void blow()
    {
        int arr[] = { 1, 2, 3, };
        span<int> v( arr );

        (void) v.last( 4 );
    }};
    struct G {
    static void blow()
    {
        int arr[] = { 1, 2, 3, };
        span<int, 3> v( arr );

        (void) v.last( 4 );
    }};

    EXPECT_THROWS( F::blow() );
    EXPECT_THROWS( G::blow() );
}

CASE( "span<>: Terminates creation of a sub span outside the span" )
{
    struct F {
    static void blow_offset()
    {
        int arr[] = { 1, 2, 3, };
        span<int> v( arr );

        (void) v.subspan( 4 );
    }
    static void blow_count()
    {
        int arr[] = { 1, 2, 3, };
        span<int> v( arr );

        (void) v.subspan( 1, 3 );
    }};
    struct G {
    static void blow_offset()
    {
        int arr[] = { 1, 2, 3, };
        span<int, 3> v( arr );

        (void) v.subspan( 4 );
    }
    static void blow_count()
    {
        int arr[] = { 1, 2, 3, };
        span<int, 3> v( arr );

        (void) v.subspan( 1, 3 );
    }};

    EXPECT_THROWS( F::blow_offset() );
    EXPECT_THROWS( F::blow_count()  );
    EXPECT_THROWS( G::blow_offset() );
    EXPECT_THROWS( G::blow_count()  );
}

CASE( "span<>: Terminates access outside the span" )
{
    struct F {
        static void blow_ix(size_t i) { int arr[] = { 1, 2, 3, }; span<int> v( arr ); (void) v[i]; }
    };
    struct G {
        static void blow_ix(size_t i) { int arr[] = { 1, 2, 3, }; span<int, 3> v( arr ); (void) v[i]; }
    };

    EXPECT_NO_THROW( F::blow_ix(2) );
    EXPECT_THROWS(   F::blow_ix(3) );
    EXPECT_NO_THROW( G::blow_ix(2) );
    EXPECT_THROWS(   G::blow_ix(3) );
}

CASE( "span<>: Terminates access with front() and back() on empty span" )
{
    EXPECT_THROWS( (void) span<int>().front() );
    EXPECT_THROWS( (void) span<int>().back() );
    EXPECT_THROWS(( (void) span<int, 0>().front() ));
    EXPECT_THROWS(( (void) span<int, 0>().back() ));
}

CASE( "span<>: Allows to default-construct" )
{
    span<int> v;
    span<int, 0> w;

    EXPECT( v.size() == size_type( 0 ) );
    EXPECT( w.size() == size_type( 0 ) );
}

CASE( "span<>: Allows to construct from a nullptr and a zero size (C++11)" )
{
#if  gsl_HAVE( NULLPTR )
    span<      int>    v ( nullptr, size_type( 0 ) );
    span<const int>    cv( nullptr, size_type( 0 ) );
    span<      int, 0> w ( nullptr, size_type( 0 ) );
    span<const int, 0> cw( nullptr, size_type( 0 ) );

    EXPECT( v.size()  == size_type( 0 ) );
    EXPECT( cv.size() == size_type( 0 ) );
    EXPECT( w.size()  == size_type( 0 ) );
    EXPECT( cw.size() == size_type( 0 ) );
#else
    EXPECT( !!"nullptr is not available (no C++11)" );
#endif
}

CASE( "span<>: Allows to construct from two pointers" )
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<      int>    v ( arr, arr + gsl_DIMENSION_OF( arr ) );
    span<const int>    cv( arr, arr + gsl_DIMENSION_OF( arr ) );
    span<      int, 9> w ( arr, arr + gsl_DIMENSION_OF( arr ) );
    span<const int, 9> cw( arr, arr + gsl_DIMENSION_OF( arr ) );

    EXPECT( std::equal( v.begin(),  v.end(),  arr ) );
    EXPECT( std::equal( cv.begin(), cv.end(), arr ) );
    EXPECT( std::equal( w.begin(),  w.end(),  arr ) );
    EXPECT( std::equal( cw.begin(), cw.end(), arr ) );
}

CASE( "span<>: Allows to construct from two pointers to const" )
{
    const int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<const int>    cv( arr, arr + gsl_DIMENSION_OF( arr ) );
    span<const int, 9> cw( arr, arr + gsl_DIMENSION_OF( arr ) );

    EXPECT( std::equal( cv.begin(), cv.end(), arr ) );
    EXPECT( std::equal( cw.begin(), cw.end(), arr ) );
}

CASE( "span<>: Allows to construct from a non-null pointer and a size" )
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<      int>    v ( arr, gsl_DIMENSION_OF( arr ) );
    span<const int>    cv( arr, gsl_DIMENSION_OF( arr ) );
    span<      int, 9> w ( arr, gsl_DIMENSION_OF( arr ) );
    span<const int, 9> cw( arr, gsl_DIMENSION_OF( arr ) );

    EXPECT( std::equal( v.begin(),  v.end(),  arr ) );
    EXPECT( std::equal( cv.begin(), cv.end(), arr ) );
    EXPECT( std::equal( w.begin(),  w.end(),  arr ) );
    EXPECT( std::equal( cw.begin(), cw.end(), arr ) );
}

CASE( "span<>: Allows to construct from a non-null pointer to const and a size" )
{
    const int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<const int>    cv( arr, gsl_DIMENSION_OF( arr ) );
    span<const int, 9> cw( arr, gsl_DIMENSION_OF( arr ) );

    EXPECT( std::equal( cv.begin(), cv.end(), arr ) );
    EXPECT( std::equal( cw.begin(), cw.end(), arr ) );
}

CASE( "span<>: Allows to construct from a temporary pointer and a size" )
{
    int x = 42;

    span<      int>    v ( &x, 1 );
    span<const int>    cv( &x, 1 );
    span<      int, 1> w ( &x, 1 );
    span<const int, 1> cw( &x, 1 );

    EXPECT( std::equal( v.begin(),  v.end(),  &x ) );
    EXPECT( std::equal( cv.begin(), cv.end(), &x ) );
    EXPECT( std::equal( w.begin(),  w.end(),  &x ) );
    EXPECT( std::equal( cw.begin(), cw.end(), &x ) );
}

CASE( "span<>: Allows to construct from a temporary pointer to const and a size" )
{
    const int x = 42;

    span<const int>    cv( &x, 1 );
    span<const int, 1> cw( &x, 1 );

    EXPECT( std::equal( cv.begin(), cv.end(), &x ) );
    EXPECT( std::equal( cw.begin(), cw.end(), &x ) );
}

CASE( "span<>: Allows to construct from any pointer and a zero size" )
{
    struct F {
        static void null() {
            int * p = gsl_nullptr; span<int> v( p, size_type( 0 ) );
        }
        static void nonnull() {
            int i = 7; int * p = &i; span<int> v( p, size_type( 0 ) );
        }
    };

    EXPECT_NO_THROW( F::null() );
    EXPECT_NO_THROW( F::nonnull() );
}

CASE( "span<>: Allows to construct from a C-array" )
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<      int>    v ( arr );
    span<const int>    cv( arr );
    span<      int, 9> w ( arr );
    span<const int, 9> cw( arr );

    EXPECT( std::equal( v.begin(),  v.end(),  arr ) );
    EXPECT( std::equal( cv.begin(), cv.end(), arr ) );
    EXPECT( std::equal( w.begin(),  w.end(),  arr ) );
    EXPECT( std::equal( cw.begin(), cw.end(), arr ) );
}

CASE( "span<>: Allows to construct from a const C-array" )
{
    const int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<const int>    cv( arr );
    span<const int, 9> cw( arr );

    EXPECT( std::equal( cv.begin(), cv.end(), arr ) );
    EXPECT( std::equal( cw.begin(), cw.end(), arr ) );
}

CASE( "span<>: Allows to construct from a C-array with size via decay to pointer (potentially dangerous)" )
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    {
    span<      int> v( arr, gsl_DIMENSION_OF(arr) );
    span<const int> w( arr, gsl_DIMENSION_OF(arr) );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
    EXPECT( std::equal( w.begin(), w.end(), arr ) );
    }

#if gsl_CPP14_OR_GREATER
    {
    span<      int> v( arr, 3 );
    span<const int> w( arr, 3 );

    EXPECT( std::equal( v.begin(), v.end(), arr, arr + 3 ) );
    EXPECT( std::equal( w.begin(), w.end(), arr, arr + 3 ) );
    }
#endif
}

CASE( "span<>: Allows to construct from a const C-array with size via decay to pointer (potentially dangerous)" )
{
    const int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    {
    span<const int> v( arr, gsl_DIMENSION_OF(arr) );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
    }

#if gsl_CPP14_OR_GREATER
    {
    span<const int> w( arr, 3 );

    EXPECT( std::equal( w.begin(), w.end(), arr, arr + 3 ) );
    }
#endif
}

CASE( "span<>: Allows to construct from a std::initializer_list<> (C++11)" )
{
#if gsl_HAVE( INITIALIZER_LIST )
    auto il = { 1, 2, 3, 4, 5, };

    span<int const> v( il );

    EXPECT( std::equal( v.begin(), v.end(), il.begin() ) );
#else
    EXPECT( !!"std::initializer_list<> is not available (no C++11)" );
#endif
}

CASE( "span<>: Allows to construct from a std::array<> (C++11)" )
{
# if gsl_HAVE( ARRAY )
    std::array<int,9> arr = {{ 1, 2, 3, 4, 5, 6, 7, 8, 9, }};

    span<int> v( arr );
    span<int, 9> w( arr );

    EXPECT( std::equal( v.begin(), v.end(), arr.begin() ) );
    EXPECT( std::equal( w.begin(), w.end(), arr.begin() ) );

    span<const int> cv( arr );
    span<const int, 9> cw( arr );

    EXPECT( std::equal( cv.begin(), cv.end(), arr.begin() ) );
    EXPECT( std::equal( cw.begin(), cw.end(), arr.begin() ) );

#else
    EXPECT( !!"std::array<> is not available (no C++11)" );
#endif
}

gsl_constexpr14 int use_span_with_array( int const (&array)[3], span<int const> sp )
{
    gsl_Assert( sp.size() == 3 );
    gsl_Assert( sp.front() == 0 );
    gsl_Assert( sp[1] == array[1] );
    gsl_Assert( sp.back() == 2 );
    gsl_Assert( sp.first( 0 ).size() == 0 );
    gsl_Assert( sp.last( 1 ).size() == 1 );
    gsl_Assert( sp.subspan( 1, 2 ).size() == 2 );
    gsl_Assert( ! sp.empty() );

    int array2[3] = { 0, 0, 0 };
    span<int> sp2 = span<int>( array2 );
    sp = sp2;
#if gsl_CPP17_OR_GREATER
    array2[0] = 1;
    gsl_Assert( sp2[0] == 1 );
#endif
    sp2[1] = 2;
    gsl_Assert( array2[1] == 2 );
    return 0;
}

gsl_constexpr14 int use_fixed_span_with_array( int const (&array)[3], span<int const, 3> sp )
{
    gsl_Assert( sp.size() == 3 );
    gsl_Assert( sp.front() == 0 );
    gsl_Assert( sp[1] == array[1] );
    gsl_Assert( sp.back() == 2 );
    gsl_Assert( sp.first( 0 ).size() == 0 );
    gsl_Assert( sp.last( 1 ).size() == 1 );
    gsl_Assert( sp.subspan( 1, 2 ).size() == 2 );
    gsl_Assert( ! sp.empty() );

    int array2[3] = { 0, 0, 0 };
    span<int, 3> sp2 = span<int, 3>( array2 );
    sp = sp2;
#if gsl_CPP17_OR_GREATER
    array2[0] = 1;
    gsl_Assert( sp2[0] == 1 );
#endif
    sp2[1] = 2;
    gsl_Assert( array2[1] == 2 );
    return 0;
}

CASE("span<>: Allows constexpr use (C++14)")
{
#if gsl_HAVE( CONSTEXPR_14 ) && ! gsl_BETWEEN( gsl_COMPILER_GNUC_VERSION, 1, 700 )
    constexpr int array[3] = { 0, 1, 2 };
# if gsl_FEATURE_TO_STD( MAKE_SPAN )
    constexpr int r1 = use_span_with_array( array, make_span( array ) );
    constexpr int r2 = use_span_with_array( array, make_span( array, array + 3 ) );
    constexpr int r3 = use_span_with_array( array, make_span( array, 3 ) );
    constexpr int s1 = use_fixed_span_with_array( array, make_span( array ) );
# endif
    constexpr int r4 = use_span_with_array( array, span<int const>( array ) );
    constexpr int r5 = use_span_with_array( array, span<int const>( array, array + 3 ) );
    constexpr int r6 = use_span_with_array( array, span<int const>( array, 3 ) );
    constexpr int s2 = use_fixed_span_with_array( array, span<int const, 3>( array ) );
    EXPECT( (r1 | r2 | r3 | r4 | r5 | r6) == 0 );
    EXPECT( (s1 | s2) == 0 );
# if gsl_HAVE( DEDUCTION_GUIDES )
    constexpr int r7 = use_span_with_array( array, span( array ) );
    constexpr int r8 = use_span_with_array( array, span( array, array + 3 ) );
    constexpr int r9 = use_span_with_array( array, span( array, 3 ) );
    constexpr int s3 = use_fixed_span_with_array( array, span( array ) );
    EXPECT( (r7 | r8 | r9) == 0 );
    EXPECT( s3 == 0 );
# endif
#endif
}

CASE( "span<>: Allows to construct from a std::array<> with const data (C++11)" )
{
#if  gsl_HAVE( ARRAY )
    std::array<const int,9> arr = {{ 1, 2, 3, 4, 5, 6, 7, 8, 9, }};

    span<const int> v( arr );

    EXPECT( std::equal( v.begin(), v.end(), arr.begin() ) );
#else
    EXPECT( !!"std::array<> is not available (no C++11)" );
#endif
}

CASE( "span<>: Allows to construct from a container (std::vector<>)" )
{
    std::vector<int> vec = vector_iota(10);

#if gsl_HAVE( CONSTRAINED_SPAN_CONTAINER_CTOR ) || gsl_HAVE( UNCONSTRAINED_SPAN_CONTAINER_CTOR )
    span<      int    > v ( vec );
    span<const int    > cv( vec );
    span<      int, 10> w ( vec );
    span<const int, 10> cw( vec );

    EXPECT( std::equal( v.begin(),  v.end(),  vec.begin() ) );
    EXPECT( std::equal( cv.begin(), cv.end(), vec.begin() ) );
    EXPECT( std::equal( w.begin(),  w.end(),  vec.begin() ) );
    EXPECT( std::equal( cw.begin(), cw.end(), vec.begin() ) );
#else
    EXPECT( !!"(un)constrained construction from container is not available" );
#endif
}

CASE( "span<>: Allows to construct from a temporary container (potentially dangerous)" )
{
#if gsl_HAVE( CONSTRAINED_SPAN_CONTAINER_CTOR ) || gsl_HAVE( UNCONSTRAINED_SPAN_CONTAINER_CTOR )
    std::vector<int> vec = vector_iota( 10 );

    EXPECT( std::equal( vec.begin(), vec.end(), span<const int>( vector_iota(10) ).begin() ) );
    EXPECT( std::equal( vec.begin(), vec.end(), span<const int, 10>( vector_iota(10) ).begin() ) );
#else
    EXPECT( !!"(un)constrained construction from container is not available" );
#endif
}

CASE( "span<>: Allows to tag-construct from a container (std::vector<>)" )
{
#if gsl_FEATURE_TO_STD( WITH_CONTAINER )
    std::vector<int> vec = vector_iota(10);
    span<      int>     v ( with_container, vec );
    span<const int>     cv( with_container, vec );
    span<      int, 10> w ( with_container, vec );
    span<const int, 10> cw( with_container, vec );

    EXPECT( std::equal( v.begin(),  v.end(),  vec.begin() ) );
    EXPECT( std::equal( cv.begin(), cv.end(), vec.begin() ) );
    EXPECT( std::equal( w.begin(),  w.end(),  vec.begin() ) );
    EXPECT( std::equal( cw.begin(), cw.end(), vec.begin() ) );
#else
    EXPECT( !!"with_container is not available (gsl_FEATURE_WITH_CONTAINER_TO_STD)" );
#endif
}

CASE( "span<>: Allows to tag-construct from a temporary container (potentially dangerous)" )
{
#if gsl_FEATURE_TO_STD( WITH_CONTAINER )
    std::vector<int> vec = vector_iota(10);

    EXPECT( std::equal( vec.begin(), vec.end(), span<const int>( with_container, vector_iota( 10 ) ).begin() ) );
    EXPECT( std::equal( vec.begin(), vec.end(), span<const int, 10>( with_container, vector_iota( 10 ) ).begin() ) );
#else
    EXPECT( !!"with_container is not available (gsl_FEATURE_WITH_CONTAINER_TO_STD)" );
#endif
}

CASE( "span<>: Allows to default construct in a constexpr context")
{
#if gsl_HAVE( CONSTEXPR_11 )
    constexpr auto spn = gsl_lite::span<int>{ };
    constexpr auto spn2 = gsl_lite::span<int, 0>{ };
    EXPECT( spn.size() == size_type( 0 ) );
    EXPECT( spn2.size() == size_type( 0 ) );
#else
    EXPECT( !!"construction in a constexpr context is not available (C++11)");
#endif
}

CASE( "span<>: Allows to copy-construct from another span of the same type" )
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
    span<      int>    v ( arr );
    span<const int>    cv( arr );
    span<      int, 9> w ( arr );
    span<const int, 9> cw( arr );

    span<      int>    x ( v );
    span<const int>    cx( w );
    span<      int, 9> y ( v );
    span<const int, 9> cy( w );

    EXPECT( std::equal( x.begin(),  x.end(),  arr ) );
    EXPECT( std::equal( cx.begin(), cx.end(), arr ) );
    EXPECT( std::equal( y.begin(),  y.end(),  arr ) );
    EXPECT( std::equal( cy.begin(), cy.end(), arr ) );
}

CASE( "span<>: Allows to copy-construct from another span of a compatible type" )
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
    span<      int>    v ( arr );
    span<const int>    cv( arr );
    span<      int, 9> w ( arr );
    span<const int, 9> cw( arr );

    span<const volatile int>    x1( v );
    span<const volatile int>    x2( cv );
    span<const volatile int>    x3( w );
    span<const volatile int>    x4( cw );
    span<const volatile int, 9> y1( v );
    span<const volatile int, 9> y2( cv );
    span<const volatile int, 9> y3( v );
    span<const volatile int, 9> y4( cv );

#if !gsl_BETWEEN( gsl_COMPILER_APPLECLANG_VERSION, 1, 1500 )
    EXPECT( std::equal( x1.begin(), x1.end(), arr ) );
    EXPECT( std::equal( x2.begin(), x2.end(), arr ) );
    EXPECT( std::equal( x3.begin(), x3.end(), arr ) );
    EXPECT( std::equal( x4.begin(), x4.end(), arr ) );
    EXPECT( std::equal( y1.begin(), y1.end(), arr ) );
    EXPECT( std::equal( y2.begin(), y2.end(), arr ) );
    EXPECT( std::equal( y3.begin(), y3.end(), arr ) );
    EXPECT( std::equal( y4.begin(), y4.end(), arr ) );
#endif // !gsl_BETWEEN( gsl_COMPILER_APPLECLANG_VERSION, 1, 1500 )
}

CASE( "span<>: Allows to move-construct from another span of the same type (C++11)" )
{
#if gsl_STDLIB_CPP11_OR_GREATER
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<      int>    v (( span<      int>   ( arr ) ));
//  span<      int>    w (( span<const int>   ( arr ) ));
    span<const int>    cv(( span<      int>   ( arr ) ));
    span<const int>    cw(( span<const int>   ( arr ) ));
    span<      int, 9> x (( span<      int, 9>( arr ) ));
//  span<      int, 9> y (( span<const int, 9>( arr ) ));
    span<const int, 9> cx(( span<      int, 9>( arr ) ));
    span<const int, 9> cy(( span<const int, 9>( arr ) ));

    EXPECT( std::equal( v.begin(),  v.end(),  arr ) );
    EXPECT( std::equal( cv.begin(), cv.end(), arr ) );
    EXPECT( std::equal( cw.begin(), cw.end(), arr ) );
    EXPECT( std::equal( x.begin(),  x.end(),  arr ) );
    EXPECT( std::equal( cx.begin(), cx.end(), arr ) );
    EXPECT( std::equal( cy.begin(), cy.end(), arr ) );
#else
    EXPECT( !!"move-semantics are not available (no C++11)" );
#endif
}

CASE( "span<>: Allows to copy-assign from another span of the same type" )
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
    int arr2[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, };
    span<      int>    x( arr );
    span<      int, 9> y( arr );
    span<      int>    s( arr2 );
    span<const int>    t( arr2 );
    span<      int, 9> u( arr2 );
    span<const int, 9> v( arr2 );

    s = x;
    t = x;
    u = y;
    v = y;

    EXPECT( std::equal( s.begin(), s.end(), arr ) );
    EXPECT( std::equal( t.begin(), t.end(), arr ) );
    EXPECT( std::equal( u.begin(), u.end(), arr ) );
    EXPECT( std::equal( v.begin(), v.end(), arr ) );
}

CASE( "span<>: Allows to move-assign from another span of the same type (C++11)" )
{
#if gsl_STDLIB_CPP11_OR_GREATER
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
    int arr2[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, };
    span<      int> s( arr2 );
    span<const int> t( arr2 );
    span<      int, 9> u( arr2 );
    span<const int, 9> v( arr2 );

    s = span<int>( arr );
    t = span<const int>( arr );
    u = span<int, 9>( arr );
    v = span<const int, 9>( arr );

    EXPECT( std::equal( s.begin(), s.end(), arr ) );
    EXPECT( std::equal( t.begin(), t.end(), arr ) );
    EXPECT( std::equal( u.begin(), u.end(), arr ) );
    EXPECT( std::equal( v.begin(), v.end(), arr ) );
#else
    EXPECT( !!"move-semantics are not available (no C++11)" );
#endif
}

CASE( "span<>: Allows to create a sub span of the first n elements" )
{
    int arr[] = { 1, 2, 3, 4, 5, };
    span<int> v( arr );
    span<int, 5> w( arr );
    size_type count = 3;

    span<      int> s1 = v.first( count );
    span<const int> t1 = v.first( count );
    span<      int> s2 = w.first( count );
    span<const int> t2 = w.first( count );
    span<      int, 3> s3 = v.first<3>( );
    span<const int, 3> t3 = v.first<3>( );
    span<      int, 3> s4 = w.first<3>( );
    span<const int, 3> t4 = w.first<3>( );

    EXPECT( s1.size() == count );
    EXPECT( t1.size() == count );
    EXPECT( s2.size() == count );
    EXPECT( t2.size() == count );
    EXPECT( s3.size() == count );
    EXPECT( t3.size() == count );
    EXPECT( s4.size() == count );
    EXPECT( t4.size() == count );
    EXPECT( std::equal( s1.begin(), s1.end(), arr ) );
    EXPECT( std::equal( t1.begin(), t1.end(), arr ) );
    EXPECT( std::equal( s2.begin(), s2.end(), arr ) );
    EXPECT( std::equal( t2.begin(), t2.end(), arr ) );
    EXPECT( std::equal( s3.begin(), s3.end(), arr ) );
    EXPECT( std::equal( t3.begin(), t3.end(), arr ) );
    EXPECT( std::equal( s4.begin(), s4.end(), arr ) );
    EXPECT( std::equal( t4.begin(), t4.end(), arr ) );
}

CASE( "span<>: Allows to create a sub span of the last n elements" )
{
    int arr[] = { 1, 2, 3, 4, 5, };
    span<int> v( arr );
    span<int, 5> w( arr );
    size_type count = 3;

    span<      int> s1 = v.last( count );
    span<const int> t1 = v.last( count );
    span<      int> s2 = w.last( count );
    span<const int> t2 = w.last( count );
    span<      int, 3> s3 = v.last<3>( );
    span<const int, 3> t3 = v.last<3>( );
    span<      int, 3> s4 = w.last<3>( );
    span<const int, 3> t4 = w.last<3>( );

    EXPECT( s1.size() == count );
    EXPECT( t1.size() == count );
    EXPECT( s2.size() == count );
    EXPECT( t2.size() == count );
    EXPECT( s3.size() == count );
    EXPECT( t3.size() == count );
    EXPECT( s4.size() == count );
    EXPECT( t4.size() == count );
    EXPECT( std::equal( s1.begin(), s1.end(), arr + v.size() - count ) );
    EXPECT( std::equal( t1.begin(), t1.end(), arr + v.size() - count ) );
    EXPECT( std::equal( s2.begin(), s2.end(), arr + v.size() - count ) );
    EXPECT( std::equal( t2.begin(), t2.end(), arr + v.size() - count ) );
    EXPECT( std::equal( s3.begin(), s3.end(), arr + v.size() - count ) );
    EXPECT( std::equal( t3.begin(), t3.end(), arr + v.size() - count ) );
    EXPECT( std::equal( s4.begin(), s4.end(), arr + v.size() - count ) );
    EXPECT( std::equal( t4.begin(), t4.end(), arr + v.size() - count ) );
}

CASE( "span<>: Allows to create a sub span starting at a given offset" )
{
    int arr[] = { 1, 2, 3, };
    span<int> v( arr );
    span<int, 3> w( arr );
    size_type offset = 1;

    span<      int> s1 = v.subspan( offset );
    span<const int> t1 = v.subspan( offset );
    span<      int> s2 = w.subspan( offset );
    span<const int> t2 = w.subspan( offset );
    span<      int> s3 = v.subspan<1>( );
    span<const int> t3 = v.subspan<1>( );
    span<      int, 2> s4 = w.subspan<1>( );
    span<const int, 2> t4 = w.subspan<1>( );

    EXPECT( s1.size() == v.size() - offset );
    EXPECT( t1.size() == v.size() - offset );
    EXPECT( s2.size() == v.size() - offset );
    EXPECT( t2.size() == v.size() - offset );
    EXPECT( s3.size() == v.size() - offset );
    EXPECT( t3.size() == v.size() - offset );
    EXPECT( s4.size() == v.size() - offset );
    EXPECT( t4.size() == v.size() - offset );
    EXPECT( std::equal( s1.begin(), s1.end(), arr + offset ) );
    EXPECT( std::equal( t1.begin(), t1.end(), arr + offset ) );
    EXPECT( std::equal( s2.begin(), s2.end(), arr + offset ) );
    EXPECT( std::equal( t2.begin(), t2.end(), arr + offset ) );
    EXPECT( std::equal( s3.begin(), s3.end(), arr + offset ) );
    EXPECT( std::equal( t3.begin(), t3.end(), arr + offset ) );
    EXPECT( std::equal( s4.begin(), s4.end(), arr + offset ) );
    EXPECT( std::equal( t4.begin(), t4.end(), arr + offset ) );
}

CASE( "span<>: Allows to create a sub span starting at a given offset with a given length" )
{
    int arr[] = { 1, 2, 3, };
    span<int> v( arr );
    span<int, 3> w( arr );
    size_type offset = 1;
    size_type length = 1;

    span<      int> s1 = v.subspan( offset, length );
    span<const int> t1 = v.subspan( offset, length );
    span<      int> s2 = w.subspan( offset, length );
    span<const int> t2 = w.subspan( offset, length );
    span<      int, 1> s3 = v.subspan<1, 1>( );
    span<const int, 1> t3 = v.subspan<1, 1>( );
    span<      int, 1> s4 = w.subspan<1, 1>( );
    span<const int, 1> t4 = w.subspan<1, 1>( );

    EXPECT( s1.size() == length );
    EXPECT( t1.size() == length );
    EXPECT( s2.size() == length );
    EXPECT( t2.size() == length );
    EXPECT( s3.size() == length );
    EXPECT( t3.size() == length );
    EXPECT( s4.size() == length );
    EXPECT( t4.size() == length );
    EXPECT( std::equal( s1.begin(), s1.end(), arr + offset ) );
    EXPECT( std::equal( t1.begin(), t1.end(), arr + offset ) );
    EXPECT( std::equal( s2.begin(), s2.end(), arr + offset ) );
    EXPECT( std::equal( t2.begin(), t2.end(), arr + offset ) );
    EXPECT( std::equal( s3.begin(), s3.end(), arr + offset ) );
    EXPECT( std::equal( t3.begin(), t3.end(), arr + offset ) );
    EXPECT( std::equal( s4.begin(), s4.end(), arr + offset ) );
    EXPECT( std::equal( t4.begin(), t4.end(), arr + offset ) );
}

CASE( "span<>: Allows to create an empty sub span at full offset" )
{
    int arr[] = { 1, 2, 3, };
    span<int> v( arr );
    span<int, 3> w( arr );
    size_type offset = v.size();

    span<      int> s1 = v.subspan( offset );
    span<const int> t1 = v.subspan( offset );
    span<      int> s2 = w.subspan( offset );
    span<const int> t2 = w.subspan( offset );
    span<      int> s3 = v.subspan<3>( );
    span<const int> t3 = v.subspan<3>( );
    span<      int, 0> s4 = w.subspan<3>( );
    span<const int, 0> t4 = w.subspan<3>( );

    EXPECT( s1.empty() );
    EXPECT( t1.empty() );
    EXPECT( s2.empty() );
    EXPECT( t2.empty() );
    EXPECT( s3.empty() );
    EXPECT( t3.empty() );
    EXPECT( s4.empty() );
    EXPECT( t4.empty() );
}

CASE( "span<>: Allows to create an empty sub span at full offset with zero length" )
{
    int arr[] = { 1, 2, 3, };
    span<int> v( arr );
    span<int, 3> w( arr );
    size_type offset = v.size();
    size_type length = 0;

    span<      int> s1 = v.subspan( offset, length );
    span<const int> t1 = v.subspan( offset, length );
    span<      int> s2 = w.subspan( offset, length );
    span<const int> t2 = w.subspan( offset, length );
    span<      int> s3 = v.subspan<3, 0>( );
    span<const int> t3 = v.subspan<3, 0>( );
    span<      int, 0> s4 = w.subspan<3, 0>( );
    span<const int, 0> t4 = w.subspan<3, 0>( );

    EXPECT( s1.empty() );
    EXPECT( t1.empty() );
    EXPECT( s2.empty() );
    EXPECT( t2.empty() );
    EXPECT( s3.empty() );
    EXPECT( t3.empty() );
    EXPECT( s4.empty() );
    EXPECT( t4.empty() );
}

CASE( "span<>: Allows forward iteration" )
{
    int arr[] = { 1, 2, 3, };
    span<int> v( arr );
    span<int, 3> w( arr );

    for ( span<int>::iterator pos = v.begin(); pos != v.end(); ++pos )
    {
        EXPECT( *pos == arr[ std::distance( v.begin(), pos )] );
    }
    for ( span<int, 3>::iterator pos = w.begin(); pos != w.end(); ++pos )
    {
        EXPECT( *pos == arr[ std::distance( w.begin(), pos )] );
    }
}

CASE( "span<>: Allows const forward iteration" )
{
    int arr[] = { 1, 2, 3, };
    span<int> v( arr );
    span<int, 3> w( arr );

    for ( span<int>::const_iterator pos = v.cbegin(); pos != v.cend(); ++pos )
    {
        EXPECT( *pos == arr[ std::distance( v.cbegin(), pos )] );
    }
    for ( span<int, 3>::const_iterator pos = w.cbegin(); pos != w.cend(); ++pos )
    {
        EXPECT( *pos == arr[ std::distance( w.cbegin(), pos )] );
    }
}

CASE( "span<>: Allows reverse iteration" )
{
    int arr[] = { 1, 2, 3, };
    span<int> v( arr );
    span<int, 3> w( arr );

    for ( span<int>::reverse_iterator pos = v.rbegin(); pos != v.rend(); ++pos )
    {
        size_t dist = narrow_failfast<size_t>( std::distance(v.rbegin(), pos) );
        EXPECT( *pos == arr[ v.size() - 1 - dist ] );
    }
    for ( span<int, 3>::reverse_iterator pos = w.rbegin(); pos != w.rend(); ++pos )
    {
        size_t dist = narrow_failfast<size_t>( std::distance(v.rbegin(), pos) );
        EXPECT( *pos == arr[ w.size() - 1 - dist ] );
    }
}

CASE( "span<>: Allows const reverse iteration" )
{
    int arr[] = { 1, 2, 3, };
    const span<int> v( arr );
    const span<int, 3> w( arr );

    for ( span<int>::const_reverse_iterator pos = v.crbegin(); pos != v.crend(); ++pos )
    {
        size_t dist = narrow_failfast<size_t>( std::distance(v.crbegin(), pos) );
        EXPECT( *pos == arr[ v.size() - 1 - dist ] );
    }
    for ( span<int>::const_reverse_iterator pos = w.crbegin(); pos != w.crend(); ++pos )
    {
        size_t dist = narrow_failfast<size_t>( std::distance(v.crbegin(), pos) );
        EXPECT( *pos == arr[ w.size() - 1 - dist ] );
    }
}

CASE( "span<>: Allows to observe an element via array indexing" )
{
    int arr[] = { 1, 2, 3, };
    span<int>          v ( arr );
    span<int> const    cv( arr );
    span<int, 3>       w ( arr );
    span<int, 3> const cw( arr );

    for ( size_type i = 0; i < v.size(); ++i )
    {
        EXPECT(   v[i] ==  arr[i] );
        EXPECT(  cv[i] ==  arr[i] );
        EXPECT(  &v[i] == &arr[i] );
        EXPECT( &cv[i] == &arr[i] );
        EXPECT(   w[i] ==  arr[i] );
        EXPECT(  cw[i] ==  arr[i] );
        EXPECT(  &w[i] == &arr[i] );
        EXPECT( &cw[i] == &arr[i] );
    }
}

CASE( "span<>: Allows to observe an element via front() and back()" )
{
    int arr[] = { 1, 2, 3, };
    span<int>          v ( arr );
    span<int> const    cv( arr );
    span<int, 3>       w ( arr );
    span<int, 3> const cw( arr );


    EXPECT(   v.front() ==  arr[0] );
    EXPECT(  &v.front() == &arr[0] );
    EXPECT(  cv.front() ==  arr[0] );
    EXPECT( &cv.front() == &arr[0] );
    EXPECT(   w.back()  ==  arr[2] );
    EXPECT(  &w.back()  == &arr[2] );
    EXPECT(  cw.back()  ==  arr[2] );
    EXPECT( &cw.back()  == &arr[2] );
}

CASE( "span<>: Allows to observe an element via data()" )
{
    int arr[] = { 1, 2, 3, };
    span<int>          v ( arr );
    span<int> const    cv( arr );
    span<int, 3>       w ( arr );
    span<int, 3> const cw( arr );

    EXPECT(  *v.data() ==  *v.begin() );
    EXPECT(   v.data() == &*v.begin() );
    EXPECT( *cv.data() ==  *v.begin() );
    EXPECT(  cv.data() == &*v.begin() );
    EXPECT(  *w.data() ==  *v.begin() );
    EXPECT(   w.data() == &*v.begin() );
    EXPECT( *cw.data() ==  *v.begin() );
    EXPECT(  cw.data() == &*v.begin() );

    for ( size_type i = 0; i < v.size(); ++i )
    {
        EXPECT(  v.data()[i] == arr[i] );
        EXPECT( cv.data()[i] == arr[i] );
        EXPECT(  w.data()[i] == arr[i] );
        EXPECT( cw.data()[i] == arr[i] );
    }
}

CASE( "span<>: Allows to change an element via array indexing" )
{
    int arr[] = { 1, 2, 3, 4, 5, };
    span<int>          v(  arr );
    span<int> const    cv( arr );
    span<int, 5>       w(  arr );
    span<int, 5> const cw( arr );

    v[1] = 22;
    cv[2] = 33;
    w[3] = 44;
    cw[4] = 55;

    EXPECT( 22 == arr[1] );
    EXPECT( 33 == arr[2] );
    EXPECT( 44 == arr[3] );
    EXPECT( 55 == arr[4] );
}

CASE( "span<>: Allows to change an element via front() and back()" )
{
    int arr[]  = { 1, 2, 3, };
    int arr2[] = { 1, 2, 3, };
    int arr3[] = { 1, 2, 3, };
    int arr4[] = { 1, 2, 3, };
    span<int>          v(  arr );
    span<int> const    cv( arr2 );
    span<int, 3>       w(  arr3 );
    span<int, 3> const cw( arr4 );

    v.front() = 11;
    v.back() = 33;
    cv.front() = 11;
    cv.back() = 33;
    w.front() = 11;
    w.back() = 33;
    cw.front() = 11;
    cw.back() = 33;

    EXPECT( 11 == arr[0] );
    EXPECT( 33 == arr[2] );
    EXPECT( 11 == arr2[0] );
    EXPECT( 33 == arr2[2] );
    EXPECT( 11 == arr3[0] );
    EXPECT( 33 == arr3[2] );
    EXPECT( 11 == arr4[0] );
    EXPECT( 33 == arr4[2] );
}

CASE( "span<>: Allows to change an element via data()" )
{
    int arr[] = { 1, 2, 3, };
    span<int>          v(  arr );
    span<int> const    cv( arr );
    span<int, 3>       w(  arr );
    span<int, 3> const cw( arr );

    *v.data() = 22;
    EXPECT( 22 == *cv.data() );
    *cv.data() = 33;
    EXPECT( 33 == *v.data() );
    *w.data() = 44;
    EXPECT( 44 == *cw.data() );
    *cw.data() = 55;
    EXPECT( 55 == *w.data() );
}

#if gsl_CONFIG( ALLOWS_SPAN_COMPARISON )
CASE( "span<>: Allows to compare equal to another span of the same type" )
{
    int a[] = { 1 }, b[] = { 2 }, c[] = { 1, 2 };
    span<int>    va( a );
    span<int>    vb( b );
    span<int>    vc( c );
    span<int, 1> wa( a );
    span<int, 1> wb( b );
    span<int, 2> wc( c );

    EXPECT(     va == va );
    EXPECT_NOT( vb == va );
    EXPECT_NOT( vc == va );
    EXPECT(     wa == wa );
    EXPECT(     va == wa );
    EXPECT_NOT( vb == wa );
    EXPECT_NOT( vc == wa );
}

CASE( "span<>: Allows to compare unequal to another span of the same type" )
{
    int a[] = { 1 }, b[] = { 2 }, c[] = { 1, 2 };
    span<int>    va( a );
    span<int>    vb( b );
    span<int>    vc( c );
    span<int, 1> wa( a );
    span<int, 1> wb( b );
    span<int, 2> wc( c );

    EXPECT_NOT( va != va );
    EXPECT(     vb != va );
    EXPECT(     vc != va );
    EXPECT(     vc != va );
    EXPECT_NOT( wa != wa );
    EXPECT(     wa != wb );
    EXPECT_NOT( va != wa );
    EXPECT(     vb != wa );
    EXPECT(     vc != wa );
}

CASE( "span<>: Allows to compare less than another span of the same type" )
{
    int a[] = { 1 }, b[] = { 2 }, c[] = { 1, 2 };
    span<int>    va( a );
    span<int>    vb( b );
    span<int>    vc( c );
    span<int, 1> wa( a );
    span<int, 1> wb( b );
    span<int, 2> wc( c );

    EXPECT_NOT( va < va );
    EXPECT(     va < vb );
    EXPECT(     va < vc );
    EXPECT_NOT( wa < wa );
    EXPECT(     wa < wb );
    EXPECT(     wa < wc );
    EXPECT_NOT( va < wa );
    EXPECT(     va < wb );
    EXPECT(     va < wc );
}

CASE( "span<>: Allows to compare less than or equal to another span of the same type" )
{
    int a[] = { 1 }, b[] = { 2 }, c[] = { 1, 2 };
    span<int>    va( a );
    span<int>    vb( b );
    span<int>    vc( c );
    span<int, 1> wa( a );
    span<int, 1> wb( b );
    span<int, 2> wc( c );

    EXPECT_NOT( vb <= va );
    EXPECT(     va <= vb );
    EXPECT(     va <= vc );
    EXPECT_NOT( wb <= wa );
    EXPECT(     wa <= wb );
    EXPECT(     wa <= wc );
    EXPECT_NOT( vb <= wa );
    EXPECT(     va <= wb );
    EXPECT(     va <= wc );
}

CASE( "span<>: Allows to compare greater than another span of the same type" )
{
    int a[] = { 1 }, b[] = { 2 }, c[] = { 1, 2 };
    span<int>    va( a );
    span<int>    vb( b );
    span<int>    vc( c );
    span<int, 1> wa( a );
    span<int, 1> wb( b );
    span<int, 2> wc( c );

    EXPECT_NOT( va > va );
    EXPECT(     vb > va );
    EXPECT(     vc > va );
    EXPECT_NOT( wa > wa );
    EXPECT(     wb > wa );
    EXPECT(     wc > wa );
    EXPECT_NOT( va > wa );
    EXPECT(     vb > wa );
    EXPECT(     vc > wa );
}

CASE( "span<>: Allows to compare greater than or equal to another span of the same type" )
{
    int a[] = { 1 }, b[] = { 2 }, c[] = { 1, 2 };
    span<int>    va( a );
    span<int>    vb( b );
    span<int>    vc( c );
    span<int, 1> wa( a );
    span<int, 1> wb( b );
    span<int, 2> wc( c );

    EXPECT_NOT( va >= vb );
    EXPECT(     vb >= va );
    EXPECT(     vc >= va );
    EXPECT_NOT( wa >= wb );
    EXPECT(     wb >= wa );
    EXPECT(     wc >= wa );
    EXPECT_NOT( va >= wb );
    EXPECT(     vb >= wa );
    EXPECT(     vc >= wa );
}

CASE( "span<>: Allows to compare to another span of the same type and different cv-ness (non-standard)" )
{
#if gsl_CONFIG( ALLOWS_NONSTRICT_SPAN_COMPARISON )
    int aa[] = { 1 }, bb[] = { 2 };
    span<         int>     a(  aa );
    span<   const int>    ca(  aa );
    span<volatile int>    va(  aa );
    span<         int>     b(  bb );
    span<   const int>    cb(  bb );
    span<         int, 1>  af( aa );
    span<   const int, 1> caf( aa );
    span<volatile int, 1> vaf( aa );
    span<         int, 1>  bf( bb );
    span<   const int, 1> cbf( bb );

    EXPECT(  va == ca );
    EXPECT(   a == va );
    EXPECT( vaf == caf );
    EXPECT(  af == vaf );
    EXPECT(  va == caf );
    EXPECT(   a == vaf );

    EXPECT(  a  == ca );
    EXPECT(  a  != cb );
    EXPECT(  a  <= cb );
    EXPECT(  a  <  cb );
    EXPECT(  b  >= ca );
    EXPECT(  b  >  ca );
    EXPECT(  af == ca );
    EXPECT(  af != cb );
    EXPECT(  af <= cb );
    EXPECT(  af <  cb );
    EXPECT(  bf >= ca );
    EXPECT(  bf >  ca );
#else
    EXPECT( !!"span<>: cannot compare different types (gsl_CONFIG_ALLOWS_NONSTRICT_SPAN_COMPARISON=0)" );
#endif
}

CASE( "span<>: Allows to compare empty spans as equal" )
{
    int a;

    span<int> p;
    span<int> q;
    span<int> r( &a, size_type( 0 ) );
    span<int, 0> s;
    span<int, 0> t( &a, size_type( 0 ) );

    EXPECT( p == q );
    EXPECT( p == r );
    EXPECT( p == s );
    EXPECT( p == t );
    EXPECT( s == t );
}
#endif // gsl_CONFIG( ALLOWS_SPAN_COMPARISON )

CASE( "span<>: Allows to test for empty span via empty(), empty case" )
{
    span<int> v;
    span<int, 0> w;

    EXPECT( v.empty() );
    EXPECT( w.empty() );
}

CASE( "span<>: Allows to test for empty span via empty(), non-empty case" )
{
    int a[] = { 1 };
    span<int> v( a );
    span<int, 1> w( a );

    EXPECT_NOT( v.empty() );
    EXPECT_NOT( w.empty() );
}

CASE( "span<>: Allows to obtain the number of elements via size(), as configured" )
{
    int a[] = { 1, 2, 3, };
    int b[] = { 1, 2, 3, 4, 5, };

    span<int> z;
    span<int> va( a );
    span<int> vb( b );
    span<int, 0> y;
    span<int, 3> wa( a );
    span<int, 5> wb( b );

    EXPECT( va.size() == size_type( gsl_DIMENSION_OF( a ) ) );
    EXPECT( vb.size() == size_type( gsl_DIMENSION_OF( b ) ) );
    EXPECT( wa.size() == size_type( gsl_DIMENSION_OF( a ) ) );
    EXPECT( wb.size() == size_type( gsl_DIMENSION_OF( b ) ) );
    EXPECT(  z.size() == size_type( 0 ) );
    EXPECT(  y.size() == size_type( 0 ) );
}

CASE( "span<>: Allows to obtain the number of elements via ssize(), signed" )
{
    int a[] = { 1, 2, 3, };
    int b[] = { 1, 2, 3, 4, 5, };

    span<int> z;
    span<int> va( a );
    span<int> vb( b );
    span<int, 0> y;
    span<int, 3> wa( a );
    span<int, 5> wb( b );

    EXPECT( va.ssize() == difference_type( gsl_DIMENSION_OF( a ) ) );
    EXPECT( vb.ssize() == difference_type( gsl_DIMENSION_OF( b ) ) );
    EXPECT( wa.ssize() == difference_type( gsl_DIMENSION_OF( a ) ) );
    EXPECT( wb.ssize() == difference_type( gsl_DIMENSION_OF( b ) ) );
    EXPECT(  z.ssize() == difference_type( 0 ) );
    EXPECT(  y.ssize() == difference_type( 0 ) );
}

CASE( "span<>: Allows to obtain the number of bytes via size_bytes()" )
{
    int a[] = { 1, 2, 3, };
    int b[] = { 1, 2, 3, 4, 5, };

    span<int> z;
    span<int> va( a );
    span<int> vb( b );
    span<int, 0> y;
    span<int, 3> wa( a );
    span<int, 5> wb( b );

    EXPECT( va.size_bytes() == size_type( gsl_DIMENSION_OF( a ) * sizeof(int) ) );
    EXPECT( vb.size_bytes() == size_type( gsl_DIMENSION_OF( b ) * sizeof(int) ) );
    EXPECT( wa.size_bytes() == size_type( gsl_DIMENSION_OF( a ) * sizeof(int) ) );
    EXPECT( wb.size_bytes() == size_type( gsl_DIMENSION_OF( b ) * sizeof(int) ) );
    EXPECT(  z.size_bytes() == size_type( 0 ) );
    EXPECT(  y.size_bytes() == size_type( 0 ) );
}

CASE( "span<>: Allows to swap with another span of the same type" )
{
    int a[] = { 1, 2, 3, };
    int b[] = { 1, 2, 3, 4, 5, };
    int c[] = { 4, 5, 6, };

    span<int> va0( a );
    span<int> vb0( b );
    span<int> va( a );
    span<int> vb( b );
    span<int, 3> wa0( a );
    span<int, 3> wc0( c );
    span<int, 3> wa( a );
    span<int, 3> wc( c );

    va.swap( vb );
    wa.swap( wc );

    EXPECT( va.data() == vb0.data() ); EXPECT( va.size() == vb0.size() );
    EXPECT( vb.data() == va0.data() ); EXPECT( vb.size() == va0.size() );
    EXPECT( wa.data() == wc0.data() ); EXPECT( wa.size() == wc0.size() );
    EXPECT( wc.data() == wa0.data() ); EXPECT( wc.size() == wa0.size() );
}

bool is_little_endian()
{
    union U
    {
        U() : i(1) {}
        int i;
        unsigned char c[ sizeof(int) ];
    };
    return 1 != U().c[ sizeof(int) - 1 ];
}

CASE( "span<>: Allows to view the elements as read-only bytes" )
{
#if gsl_FEATURE( BYTE )
# if gsl_HAVE( SIZED_TYPES )
    typedef int32_t type;
# else
    typedef int type;
# endif
    typedef gsl_lite::byte gyte;

    EXPECT( sizeof( type ) == size_t( 4 ) );

    type  a[] = { 0x12345678, };
    gyte be[] = { to_byte(0x12), to_byte(0x34), to_byte(0x56), to_byte(0x78), };
    gyte le[] = { to_byte(0x78), to_byte(0x56), to_byte(0x34), to_byte(0x12), };

    gyte * b = is_little_endian() ? le : be;

    span<type> va( a );
    span<const gyte> vc( as_bytes( va ) );

    EXPECT( vc[0] == b[0] );
    EXPECT( vc[1] == b[1] );
    EXPECT( vc[2] == b[2] );
    EXPECT( vc[3] == b[3] );

#endif // gsl_FEATURE( BYTE )
}

CASE( "span<>: Allows to view and change the elements as writable bytes" )
{
#if gsl_FEATURE( BYTE )
# if gsl_HAVE( SIZED_TYPES )
    typedef int32_t type;
# else
    typedef int type;
# endif
    typedef gsl_lite::byte gyte;

    EXPECT( sizeof(type) == size_t( 4 ) );

    {
        type  a[] = { 0x0, };
        span<type> va( a );
        span<gyte> vc( as_writable_bytes(va) );

        for ( size_t i = 0; i < sizeof(type); ++i )
        {
            EXPECT( vc[i] == to_byte(0) );
        }

        vc[0] = to_byte(0x42);

        EXPECT( vc[0] == to_byte(0x42) );
        for ( size_t i = 1; i < sizeof(type); ++i )
        {
            EXPECT( vc[i] == to_byte(0) );
        }
    }

#endif // gsl_FEATURE( BYTE )
}

CASE( "span<>: Allows to view the elements as a span of another type" )
{
#if ! gsl_DEPRECATE_TO_LEVEL( 9 )
# if gsl_HAVE( SIZED_TYPES )
    typedef int32_t type1;
    typedef int16_t type2;
# else
    typedef int   type1;
    typedef short type2;
# endif
    EXPECT( sizeof( type1 ) == size_t( 4 ) );
    EXPECT( sizeof( type2 ) == size_t( 2 ) );

    type1  a[] = { 0x12345678, };
    type2 be[] = { type2(0x1234), type2(0x5678), };
    type2 le[] = { type2(0x5678), type2(0x1234), };

    type2 * b = is_little_endian() ? le : be;

    span<type1> va( a );
    span<type2> vb( va.as_span<type2>() );

    EXPECT( vb[0] == b[0] );
    EXPECT( vb[1] == b[1] );
#endif // ! gsl_DEPRECATE_TO_LEVEL( 9 )
}

CASE( "span<>: Allows to change the elements from a span of another type" )
{
#if ! gsl_DEPRECATE_TO_LEVEL( 9 )
# if gsl_HAVE( SIZED_TYPES )
    typedef int32_t type1;
    typedef int16_t type2;
# else
    typedef int   type1;
    typedef short type2;
# endif
    EXPECT( sizeof( type1 ) == size_t( 4 ) );
    EXPECT( sizeof( type2 ) == size_t( 2 ) );

    type1  a[] = { 0x0, };

    span<type1> va( a );
    span<type2> vb( va.as_span<type2>() );

    {for ( size_t i = 0; i < sizeof(type2); ++i ) EXPECT( vb[i] == type2(0) ); }

    vb[0] = 0x42;

    EXPECT( vb[0] == type2(0x42) );
    {for ( size_t i = 1; i < sizeof(type2); ++i ) EXPECT( vb[i] == type2(0) ); }
#endif // ! gsl_DEPRECATE_TO_LEVEL( 9 )
}

CASE( "span<>: Allows default construction of iterators" )
{
    span<int>::iterator it1;
    span<int>::iterator it2;
    EXPECT(it1 == it2);
}

CASE( "span<>: Allows comparison of iterators from same span" )
{
    int a[] = {1, 2, 3, 4};

    span<int> s = a;
    span<int>::iterator it = s.begin();
    span<int>::iterator it2 = it + 1;

    EXPECT(it == it);
    EXPECT(it == s.begin());
    EXPECT(s.begin() == it);

    EXPECT(it != it2);
    EXPECT(it2 != it);
    EXPECT(it != s.end());
    EXPECT(it2 != s.end());
    EXPECT(s.end() != it);

    EXPECT(it < it2);
    EXPECT(it <= it2);
    EXPECT(it2 <= s.end());
    EXPECT(it < s.end());

    EXPECT(it2 > it);
    EXPECT(it2 >= it);
    EXPECT(s.end() > it2);
    EXPECT(s.end() >= it2);
}

CASE( "span<>: Terminates comparison of iterators from different spans" )
{
    struct F {
    static void blow()
    {
        int a[] = {1, 2, 3, 4};
        int b[] = {1, 2, 3, 4};
        span<int> s( a );
        span<int> s2( b );

        (void) ( s.begin() == s2.begin() );
    }};
    struct G {
    static void blow()
    {
        int a[] = {1, 2, 3, 4};
        int b[] = {1, 2, 3, 4};
        span<int> s( a );
        span<int> s2( b );

        (void) ( s.begin() <= s2.begin() );
    }};

    EXPECT_THROWS( F::blow() );
    EXPECT_THROWS( G::blow() );
}

CASE( "span<>: Allows dereferencing iterators" )
{
    {
        int a[] = {1, 2, 3, 4};
        span<int> s = a;

        span<int>::iterator it = s.begin();
#if gsl_STDLIB_CPP11_OR_GREATER
        span<int>::iterator it2 = std::begin(s);
#else // ! gsl_STDLIB_CPP11_OR_GREATER
        span<int>::iterator it2 = s.begin();
#endif // gsl_STDLIB_CPP11_OR_GREATER
        EXPECT(it == it2);

        it = s.end();
#if gsl_STDLIB_CPP11_OR_GREATER
        it2 = std::end(s);
#else // ! gsl_STDLIB_CPP11_OR_GREATER
        it2 = s.end();
#endif // gsl_STDLIB_CPP11_OR_GREATER
        EXPECT(it == it2);
    }

    {
        int a[] = {1, 2, 3, 4};
        span<int> s = a;

        span<int>::iterator it = s.begin();
        span<int>::iterator first = it;
        EXPECT(it == first);
        EXPECT(*it == 1);

        span<int>::iterator beyond = s.end();
        EXPECT(it != beyond);

        EXPECT(beyond - first == difference_type( 4 ));
        EXPECT(first - first == difference_type( 0 ));
        EXPECT(beyond - beyond == difference_type( 0 ));

        ++it;
        EXPECT(it - first == difference_type( 1 ));
        EXPECT(*it == 2);
        *it = 22;
        EXPECT(*it == 22);
        EXPECT(beyond - it == difference_type( 3 ));

        it = first;
        EXPECT(it == first);
        while (it != s.end())
        {
            *it = 5;
            ++it;
        }

        EXPECT(it == beyond);
        EXPECT(it - beyond == difference_type( 0 ));

        for (span<int>::const_iterator pos = s.begin(); pos != s.end(); ++pos)
        {
            EXPECT(*pos == 5);
        }
    }
}

CASE( "span<>: Terminates dereferencing past-the-end iterators" )
{
    int a[] = {1, 2, 3, 4};
    span<int> s = a;

    span<int>::iterator beyond = s.end();
    EXPECT_THROWS(*beyond);
}

CASE( "span<>: Allows dereferencing reverse iterators" )
{
    int a[] = {1, 2, 3, 4};
    span<int> s = a;

    span<int>::reverse_iterator it = s.rbegin();
    span<int>::reverse_iterator first = it;
    EXPECT(it == first);
    EXPECT(*it == 4);

    span<int>::reverse_iterator beyond = s.rend();
    EXPECT(it != beyond);

    EXPECT(beyond - first == difference_type( 4 ));
    EXPECT(first - first == difference_type( 0 ));
    EXPECT(beyond - beyond == difference_type( 0 ));

    ++it;
    EXPECT(it - s.rbegin() == difference_type( 1 ));
    EXPECT(*it == 3);
    *it = 22;
    EXPECT(*it == 22);
    EXPECT(beyond - it == difference_type( 3 ));

    it = first;
    EXPECT(it == first);
    while (it != s.rend())
    {
        *it = 5;
        ++it;
    }

    EXPECT(it == beyond);
    EXPECT(it - beyond == difference_type( 0 ));

    for (span<int>::const_reverse_iterator pos = s.rbegin(); pos != s.rend(); ++pos)
    {
        EXPECT(*pos == 5);
    }
}

CASE( "span<>: Terminates dereferencing past-the-end reverse iterators" )
{
    struct F {
    static void blow()
    {
        int a[] = {1, 2, 3, 4};
        span<int> s = a;

        span<int>::reverse_iterator beyond = s.rend();
        (void) *beyond;
    }};

    EXPECT_THROWS(F::blow());
}

CASE( "span<>: Allows appropriate fixed-size conversions" )
{
    int arr[] = {1, 2, 3, 4};

    // converting to a span from an equal size array is ok
    span<int, 4> s4 = arr;
    EXPECT(s4.size() == size_type( 4 ));

    // converting to dynamic_range is always ok
    {
        span<int> s = s4;
        EXPECT(s.size() == s4.size());
        (void) s;
    }

#if gsl_HAVE( TYPE_TRAITS )
    // initialization or assignment to static span is NOT ok
    static_assert( !std::is_convertible< int[4], span<int, 2> >::value, "static assertion failed" );
    static_assert( !std::is_convertible< span<int, 4>, span<int, 2> >::value, "static assertion failed" );
    static_assert( !std::is_convertible< int[2], span<int, 4> >::value, "static assertion failed" );
    static_assert( !std::is_convertible< span<int, 2>, span<int, 4> >::value, "static assertion failed" );

    // even when done dynamically
    static_assert( !std::is_convertible< span<int>, span<int, 2> >::value, "static assertion failed" );

    // but doing so explicitly is ok
    static_assert(  std::is_constructible< span<int>, span<int, 2> >::value, "static assertion failed" );
#endif

    // you can convert statically
    {
        const span<int, 2> s2( &arr[0], 2 );
        (void) s2;
    }
    {
        const span<int, 1> s1 = s4.first<1>();
        (void) s1;
    }
}

CASE( "span<>: Terminates inappropriate fixed-size conversions" )
{
    struct F {
    static void blow()
    {
        int arr2[2] = {1, 2};
        const span<int, 4> s4(arr2, 2);
        (void) s4;
    }};

    EXPECT_THROWS( F::blow() );
}

CASE( "copy(): Allows to copy a span to another span of the same element type" )
{
    int a[] = { 1, 2, 3,       };
    int b[] = { 0, 0, 0, 0, 0, };
    int c[] = { 0, 0, 0, 0, 0, };
    int d[] = { 0, 0, 0, 0, 0, };
    int e[] = { 0, 0, 0, 0, 0, };

    span<int>    src1( a );
    span<int, 3> src2( a );
    span<int>    dst1( b );
    span<int, 5> dst2( c );
    span<int>    dst3( d );
    span<int, 5> dst4( e );

    copy( src1, dst1 );
    copy( src1, dst2 );
    copy( src2, dst3 );
    copy( src2, dst4 );

    EXPECT( std::equal(src1.begin(), src1.end(), dst1.begin()) );
    EXPECT( std::equal(src1.begin(), src1.end(), dst2.begin()) );
    EXPECT( std::equal(src1.begin(), src1.end(), dst3.begin()) );
    EXPECT( std::equal(src1.begin(), src1.end(), dst4.begin()) );
}

CASE( "copy(): Allows to copy a span to another span of a different element type" )
{
    char a[] = { 'a', 'b', 'c',       };
    int  b[] = {  0 ,  0 ,  0 , 0, 0, };
    int  c[] = {  0 ,  0 ,  0 , 0, 0, };
    int  d[] = {  0 ,  0 ,  0 , 0, 0, };
    int  e[] = {  0 ,  0 ,  0 , 0, 0, };

    span<char>    src1( a );
    span<char, 3> src2( a );
    span<int>     dst1( b );
    span<int, 5>  dst2( c );
    span<int>     dst3( d );
    span<int, 5>  dst4( e );

    copy( src1, dst1 );
    copy( src1, dst2 );
    copy( src2, dst3 );
    copy( src2, dst4 );

    for ( span<int>::size_type i = 0; i < src1.size(); ++i )
    {
        EXPECT( src1[i] == dst1[i] );
        EXPECT( src1[i] == dst2[i] );
        EXPECT( src1[i] == dst3[i] );
        EXPECT( src1[i] == dst4[i] );
    }
}

CASE( "size(): Allows to obtain the number of elements in span via size(span), unsigned" )
{
    int a[] = { 1, 2, 3, };
    int b[] = { 1, 2, 3, 4, 5, };

    span<int> z;
    span<int> va( a );
    span<int> vb( b );
    span<int, 0> y;
    span<int, 3> wa( a );
    span<int, 5> wb( b );

    EXPECT( size( va ) == size_type( gsl_DIMENSION_OF( a ) ) );
    EXPECT( size( vb ) == size_type( gsl_DIMENSION_OF( b ) ) );
    EXPECT( size( wa ) == size_type( gsl_DIMENSION_OF( a ) ) );
    EXPECT( size( wb ) == size_type( gsl_DIMENSION_OF( b ) ) );
    EXPECT( size(  z ) == size_type( 0 ) );
    EXPECT( size(  y ) == size_type( 0 ) );
}

CASE( "ssize(): Allows to obtain the number of elements in span via ssize(span), signed" )
{
    int a[] = { 1, 2, 3, };
    int b[] = { 1, 2, 3, 4, 5, };

    span<int> z;
    span<int> va( a );
    span<int> vb( b );
    span<int, 0> y;
    span<int, 3> wa( a );
    span<int, 5> wb( b );

    EXPECT( ssize( va ) == difference_type( gsl_DIMENSION_OF( a ) ) );
    EXPECT( ssize( vb ) == difference_type( gsl_DIMENSION_OF( b ) ) );
    EXPECT( ssize( wa ) == difference_type( gsl_DIMENSION_OF( a ) ) );
    EXPECT( ssize( wb ) == difference_type( gsl_DIMENSION_OF( b ) ) );
    EXPECT( ssize(  z ) == difference_type( 0 ) );
    EXPECT( ssize(  y ) == difference_type( 0 ) );
}

#if ! gsl_FEATURE_TO_STD( MAKE_SPAN )

CASE( "make_span(): unavailable (gsl_FEATURE_MAKE_SPAN=0)" )
{
    EXPECT( !!"(avoid warning)" );  // suppress: unused parameter 'lest_env' [-Wunused-parameter]
}

#else

CASE( "make_span(): (gsl_FEATURE_MAKE_SPAN=1)" )
{
    EXPECT( !!"(avoid warning)" );  // suppress: unused parameter 'lest_env' [-Wunused-parameter]
}

CASE( "make_span(): Allows to build from two pointers" )
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<int> v = make_span( arr, arr + gsl_DIMENSION_OF( arr ) );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
}

CASE( "make_span(): Allows to build from two const pointers" )
{
    const int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<const int> v = make_span( arr, arr + gsl_DIMENSION_OF( arr ) );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
}

CASE( "make_span(): Allows to build from a non-null pointer and a size" )
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<int> v = make_span( arr, gsl_DIMENSION_OF( arr ) );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
}

CASE( "make_span(): Allows to build from a non-null const pointer and a size" )
{
    const int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<const int> v = make_span( arr, gsl_DIMENSION_OF( arr ) );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
}

CASE( "make_span(): Allows to build from a C-array" )
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<int> v = make_span( arr );
    span<int, 9> w = make_span( arr );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
    EXPECT( std::equal( w.begin(), w.end(), arr ) );
}

CASE( "make_span(): Allows to build from a const C-array" )
{
    const int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<const int> v = make_span( arr );
    span<const int, 9> w = make_span( arr );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
    EXPECT( std::equal( w.begin(), w.end(), arr ) );
}

CASE( "make_span(): Allows building from a std::initializer_list<> (C++11)" )
{
#if gsl_HAVE( INITIALIZER_LIST )
    auto il = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<int const> v = make_span( il );

    EXPECT( std::equal( v.begin(), v.end(), il.begin() ) );
#else
    EXPECT( !!"std::initializer_list<> is not available (no C++11)" );
#endif
}

CASE( "make_span(): Allows to build from a std::array<> (C++11)" )
{
#if gsl_HAVE( ARRAY )
    std::array<int,9> arr = {{ 1, 2, 3, 4, 5, 6, 7, 8, 9, }};

    span<int> v = make_span( arr );
    span<int, 9> w = make_span( arr );

    EXPECT( std::equal( v.begin(), v.end(), arr.begin() ) );
    EXPECT( std::equal( w.begin(), w.end(), arr.begin() ) );
#else
    EXPECT( !!"std::array<> is not available (no C++11)" );
#endif
}

CASE( "make_span(): Allows to build from a const std::array<> (C++11)" )
{
#if gsl_HAVE( ARRAY )
    const std::array<int,9> arr = {{ 1, 2, 3, 4, 5, 6, 7, 8, 9, }};

    span<const int> v = make_span( arr );
    span<const int, 9> w = make_span( arr );

    EXPECT( std::equal( v.begin(), v.end(), arr.begin() ) );
    EXPECT( std::equal( w.begin(), w.end(), arr.begin() ) );
#else
    EXPECT( !!"std::array<> is not available (no C++11)" );
#endif
}

CASE( "make_span(): Allows to build from a container (std::vector<>)" )
{
    std::vector<int> vec = vector_iota(10);
    span<int> v = make_span( vec );

    EXPECT( std::equal( v.begin(), v.end(), vec.begin() ) );
}

CASE( "make_span(): Allows to build from a const container (std::vector<>)" )
{
    const std::vector<int> vec = vector_iota(10);
    span<const int> v = make_span( vec );

    EXPECT( std::equal( v.begin(), v.end(), vec.begin() ) );
}

CASE( "make_span(): Allows to build from a temporary container (potentially dangerous)" )
{
    std::vector<int> vec = vector_iota(10);

    EXPECT( std::equal( vec.begin(), vec.end(), make_span( vector_iota( 10 ) ).begin() ) );
}

CASE( "make_span(): Allows to tag-build from a container (std::vector<>)" )
{
#if gsl_FEATURE_TO_STD( WITH_CONTAINER )
    std::vector<int> vec = vector_iota(10);
    span<      int> v = make_span( with_container, vec );
    span<const int> w = make_span( with_container, vec );

    EXPECT( std::equal( v.begin(), v.end(), vec.begin() ) );
    EXPECT( std::equal( w.begin(), w.end(), vec.begin() ) );
#else
    EXPECT( !!"with_container is not available (gsl_FEATURE_WITH_CONTAINER_TO_STD)" );
#endif
}

CASE( "make_span(): Allows to tag-build from a temporary container (potentially dangerous)" )
{
#if gsl_FEATURE_TO_STD( WITH_CONTAINER )
    std::vector<int> vec = vector_iota(10);

    EXPECT( std::equal( vec.begin(), vec.end(), make_span( with_container, vector_iota( 10 ) ).begin() ) );
#else
    EXPECT( !!"with_container is not available (gsl_FEATURE_WITH_CONTAINER_TO_STD)" );
#endif
}

#endif // gsl_FEATURE( MAKE_SPAN )

#if ! gsl_FEATURE( BYTE ) || ! gsl_FEATURE_TO_STD( BYTE_SPAN )

CASE( "byte_span(): unavailable (gsl_FEATURE_BYTE_SPAN=0)" )
{
    EXPECT( !!"(avoid warning)" );  // suppress: unused parameter 'lest_env' [-Wunused-parameter]
}

#else

CASE( "byte_span() (gsl_FEATURE_BYTE_SPAN=1)" )
{
    EXPECT( !!"(avoid warning)" );  // suppress: unused parameter 'lest_env' [-Wunused-parameter]
}

# if gsl_COMPILER_MSVC_VERSION
#  pragma warning( push )
#  pragma warning( disable : 4127 ) // conditional expression is constant
# endif

CASE( "byte_span(): Allows to build a span of gsl_lite::byte from a single object" )
{
# if gsl_HAVE( ENUM_CLASS )
    int x = (std::numeric_limits<int>::max)();

    span<gsl_lite::byte> spn = byte_span( x );

    EXPECT( spn.size() == size_type( sizeof x ) );
    if ( sizeof x > 1 )
    {
        if ( std20::endian::native == std20::endian::little )
        {
            EXPECT( spn[0]            == to_byte( 0xff ) );
            EXPECT( spn[sizeof x - 1] == to_byte( 0x7f ) );
        }
        else if ( std20::endian::native == std20::endian::big )
        {
            EXPECT( spn[sizeof x - 1] == to_byte( 0xff ) );
            EXPECT( spn[0]            == to_byte( 0x7f ) );
        }
    }
# endif // gsl_HAVE( ENUM_CLASS )
}

CASE( "byte_span(): Allows to build a span of const gsl_lite::byte from a single const object" )
{
# if gsl_HAVE( ENUM_CLASS )
    const int x = (std::numeric_limits<int>::max)();

    span<const gsl_lite::byte> spn = byte_span( x );

    EXPECT( spn.size() == size_type( sizeof x ) );
    if ( sizeof x > 1 )
    {
        if ( std20::endian::native == std20::endian::little )
        {
            EXPECT( spn[0]            == to_byte( 0xff ) );
            EXPECT( spn[sizeof x - 1] == to_byte( 0x7f ) );
        }
        else if ( std20::endian::native == std20::endian::big )
        {
            EXPECT( spn[sizeof x - 1] == to_byte( 0xff ) );
            EXPECT( spn[0]            == to_byte( 0x7f ) );
        }
    }
# endif // gsl_HAVE( ENUM_CLASS )
}

# if gsl_COMPILER_MSVC_VERSION
#  pragma warning( pop )
# endif

#endif // span_PROVIDE( BYTE_SPAN )


// end of file
