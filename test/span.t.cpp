//
// gsl-lite is based on GSL: Guideline Support Library.
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

#include "gsl-lite.t.h"

typedef span<int>::index_type index_type;

CASE( "span<>: Disallows construction from a temporary value (C++11) (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)" )
{
#if gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS
# if gsl_HAVE_IS_DELETE
    span<int> v = 42;
# endif
#endif
}

CASE( "span<>: Disallows construction from a C-array of incompatible type (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)" )
{
#if gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS
    short arr[] = { 1, 2, 3, };
    span<int> v = arr;
#endif
}

CASE( "span<>: Disallows construction from a std::array of incompatible type (C++11) (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)" )
{
#if gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS
# if gsl_HAVE_ARRAY
    std::array<long,3> arr = {{ 1L, 2L, 3L, }};
    span<int> v( arr);
# endif
#endif
}

CASE( "span<>: Terminates construction from a nullptr and a non-zero size (C++11)" )
{
#if gsl_HAVE_NULLPTR
    struct F { static void blow() { span<int> v( nullptr, 42 ); } };

    EXPECT_THROWS( F::blow() );
#else
    EXPECT( !!"nullptr is not available (no C++11)" );
#endif
}

CASE( "span<>: Terminates construction from two pointers in the wrong order" )
{
    struct F { static void blow() { int a[2]; span<int> v( &a[1], &a[0] ); } };

    EXPECT_THROWS( F::blow() );
}

CASE( "span<>: Terminates construction from a null pointer and a non-zero size" )
{
    struct F { static void blow() { int * p = NULL; span<int> v( p, 42 ); } };

    EXPECT_THROWS( F::blow() );
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

    EXPECT_THROWS( F::blow() );
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

    EXPECT_THROWS( F::blow() );
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

    EXPECT_THROWS( F::blow_offset() );
    EXPECT_THROWS( F::blow_count()  );
}

CASE( "span<>: Terminates access outside the span" )
{
    struct F {
        static void blow_ix(int i) { int arr[] = { 1, 2, 3, }; span<int> v( arr ); (void) v[i]; }
        static void blow_iv(int i) { int arr[] = { 1, 2, 3, }; span<int> v( arr ); (void) v(i); }
        static void blow_at(int i) { int arr[] = { 1, 2, 3, }; span<int> v( arr ); (void) v.at(i); }
    };

    EXPECT_NO_THROW( F::blow_ix(2) );
    EXPECT_NO_THROW( F::blow_iv(2) );
    EXPECT_NO_THROW( F::blow_at(2) );
    EXPECT_THROWS(   F::blow_ix(3) );
    EXPECT_THROWS(   F::blow_iv(3) );
    EXPECT_THROWS(   F::blow_at(3) );
}

CASE( "span<>: Allows to default-construct" )
{
    span<int> v;

    EXPECT( v.size() == index_type( 0 ) );
}

CASE( "span<>: Allows to construct from a nullptr and a zero size (C++11)" )
{
#if gsl_HAVE_NULLPTR
    span<      int> v( nullptr, 0 );
    span<const int> w( nullptr, 0 );

    EXPECT( v.size() == index_type( 0 ) );
    EXPECT( w.size() == index_type( 0 ) );
#else
    EXPECT( !!"nullptr is not available (no C++11)" );
#endif
}

CASE( "span<>: Allows to construct from a l-value (C++11)" )
{
#if gsl_HAVE_IS_DELETE
    int x = 0;

    span<      int> v( x );
    span<const int> w( x );

    EXPECT( v.size() == index_type( 1 ) );
    EXPECT( w.size() == index_type( 1 ) );
#else
    EXPECT( !!"=delete is not available (no C++11)" );
#endif
}

CASE( "span<>: Allows to construct from a const l-value (C++11)" )
{
#if gsl_HAVE_IS_DELETE
    const int x = 0;

    span<const int> v( x );

    EXPECT( v.size() == index_type( 1 ) );
#else
    EXPECT( !!"=delete is not available (no C++11)" );
#endif
}

CASE( "span<>: Allows to construct from two pointers" )
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<      int> v( arr, arr + gsl_DIMENSION_OF( arr ) );
    span<const int> w( arr, arr + gsl_DIMENSION_OF( arr ) );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
    EXPECT( std::equal( w.begin(), w.end(), arr ) );
}

CASE( "span<>: Allows to construct from two pointers to const" )
{
    const int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<const int> v( arr, arr + gsl_DIMENSION_OF( arr ) );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
}

CASE( "span<>: Allows to construct from a non-null pointer and a size" )
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<      int> v( arr, gsl_DIMENSION_OF( arr ) );
    span<const int> w( arr, gsl_DIMENSION_OF( arr ) );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
    EXPECT( std::equal( w.begin(), w.end(), arr ) );
}

CASE( "span<>: Allows to construct from a non-null pointer to const and a size" )
{
    const int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<const int> v( arr, gsl_DIMENSION_OF( arr ) );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
}

CASE( "span<>: Allows to construct from a temporary pointer and a size" )
{
    int x = 42;

    span<      int> v( &x, 1 );
    span<const int> w( &x, 1 );

    EXPECT( std::equal( v.begin(), v.end(), &x ) );
    EXPECT( std::equal( w.begin(), w.end(), &x ) );
}

CASE( "span<>: Allows to construct from a temporary pointer to const and a size" )
{
    const int x = 42;

    span<const int> v( &x, 1 );

    EXPECT( std::equal( v.begin(), v.end(), &x ) );
}

CASE( "span<>: Allows to construct from any pointer and a zero size" )
{
    struct F {
        static void null() {
            int * p = NULL; span<int> v( p, index_type( 0 ) );
        }
        static void nonnull() {
            int i = 7; int * p = &i; span<int> v( p, index_type( 0 ) );
        }
    };

    EXPECT_NO_THROW( F::null() );
    EXPECT_NO_THROW( F::nonnull() );
}

CASE( "span<>: Allows to construct from a C-array" )
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<      int> v( arr );
    span<const int> w( arr );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
    EXPECT( std::equal( w.begin(), w.end(), arr ) );
}

CASE( "span<>: Allows to construct from a const C-array" )
{
    const int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<const int> v( arr );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
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

CASE( "span<>: Allows to construct from a std::array<> (C++11)" )
{
# if gsl_HAVE_ARRAY
    std::array<int,9> arr = {{ 1, 2, 3, 4, 5, 6, 7, 8, 9, }};

    span<      int> v( arr );
    span<const int> w( arr );

    EXPECT( std::equal( v.begin(), v.end(), arr.begin() ) );
    EXPECT( std::equal( w.begin(), w.end(), arr.begin() ) );
#else
    EXPECT( !!"std::array<> is not available (no C++11)" );
#endif
}

CASE( "span<>: Allows to construct from a std::array<> with const data (C++11)" )
{
# if gsl_HAVE_ARRAY
    std::array<const int,9> arr = {{ 1, 2, 3, 4, 5, 6, 7, 8, 9, }};

    span<const int> v( arr );

    EXPECT( std::equal( v.begin(), v.end(), arr.begin() ) );
#else
    EXPECT( !!"std::array<> is not available (no C++11)" );
#endif
}

CASE( "span<>: Allows to construct from a container (std::vector<>)" )
{
# if gsl_HAVE_INITIALIZER_LIST
    std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
#else
    std::vector<int> vec; {for ( int i = 1; i < 10; ++i ) vec.push_back(i); }
#endif

#if gsl_HAVE_CONSTRAINED_SPAN_CONTAINER_CTOR || gsl_HAVE_UNCONSTRAINED_SPAN_CONTAINER_CTOR
    span<      int> v( vec );
    span<const int> w( vec );

    EXPECT( std::equal( v.begin(), v.end(), vec.begin() ) );
    EXPECT( std::equal( w.begin(), w.end(), vec.begin() ) );
#else
    EXPECT( !!"(un)constrained construction from container is not available" );
#endif
}

CASE( "span<>: Allows to tag-construct from a container (std::vector<>)" )
{
# if gsl_HAVE_INITIALIZER_LIST
    std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
#else
    std::vector<int> vec; {for ( int i = 1; i < 10; ++i ) vec.push_back(i); }
#endif
    span<      int> v( with_container, vec );
    span<const int> w( with_container, vec );

    EXPECT( std::equal( v.begin(), v.end(), vec.begin() ) );
    EXPECT( std::equal( w.begin(), w.end(), vec.begin() ) );
}

CASE( "span<>: Allows to construct from an empty gsl::shared_ptr (C++11)" )
{
#if gsl_HAVE_SHARED_PTR
    span<int> ptr = gsl::shared_ptr<int>( gsl_nullptr );

    span<int> s( ptr );

    EXPECT( s.length() == index_type( 0  ) );
    EXPECT( s.data()   == gsl_nullptr      );
#else
    EXPECT( !!"gsl::shared_ptr is not available" );
#endif
}

CASE( "span<>: Allows to construct from an empty gsl::unique_ptr (C++11)" )
{
#if gsl_HAVE_UNIQUE_PTR
    gsl::unique_ptr<int> ptr = std::unique_ptr<int>( gsl_nullptr );

    span<int> s( ptr );

    EXPECT( s.length() == index_type( 0 ) );
    EXPECT( s.data()   == gsl_nullptr     );
#else
    EXPECT( !!"gsl::unique_ptr is not available" );
#endif
}

CASE( "span<>: Allows to construct from an empty gsl::unique_ptr (array, C++11)" )
{
#if gsl_HAVE_UNIQUE_PTR
    gsl::unique_ptr<int[]> ptr = unique_ptr<int[]>( gsl_nullptr );

    span<int> s( ptr, 0 );

    EXPECT( s.length() == index_type( 0 ) );
    EXPECT( s.data()   == gsl_nullptr     );
#else
    EXPECT( !!"gsl::unique_ptr is not available" );
#endif
}

CASE( "span<>: Allows to construct from a non-empty gsl::shared_ptr (C++11)" )
{
#if gsl_HAVE_SHARED_PTR
    shared_ptr<int> ptr = gsl::make_shared<int>( 4 );

    span<int> s( ptr );

    EXPECT( s.length() == index_type( 1 ) );
    EXPECT( s.data()   == ptr.get()       );
    EXPECT( s[0]       == 4               );
#else
    EXPECT( !!"gsl::shared_ptr is not available" );
#endif
}

CASE( "span<>: Allows to construct from a non-empty gsl::unique_ptr (C++11)" )
{
#if gsl_HAVE_UNIQUE_PTR
# if gsl_HAVE_MAKE_UNIQUE
    gsl::unique_ptr<int> ptr = std::make_unique<int>( 4 );
#else
    gsl::unique_ptr<int> ptr = unique_ptr<int>( new int( 4 ) );
#endif

    span<int> s( ptr );

    EXPECT( s.length() == index_type( 1 ) );
    EXPECT( s.data()   == ptr.get()       );
    EXPECT( s[0]       == 4               );
#else
    EXPECT( !!"gsl::unique_ptr is not available" );
#endif
}

CASE( "span<>: Allows to construct from a non-empty gsl::unique_ptr (array, C++11)" )
{
#if gsl_HAVE_UNIQUE_PTR
# if gsl_HAVE_MAKE_UNIQUE
    gsl::unique_ptr<int[]> arr = make_unique<int[]>( 4 );
#else
    gsl::unique_ptr<int[]> arr = unique_ptr<int[]>( new int[4] );
#endif

    for ( int i = 0; i < 4; i++ )
        arr[i] = i + 1;

    span<int> s( arr, 4 );

    EXPECT( s.length() == index_type( 4 ) );
    EXPECT( s.data()   == arr.get()       );
    EXPECT( s[0]       == 1               );
    EXPECT( s[1]       == 2               );
#else
    EXPECT( !!"gsl::unique_ptr is not available" );
#endif
}

CASE( "span<>: Allows to copy-construct from another span of the same type" )
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
    span<      int> v( arr );
    span<const int> w( arr );

    span<      int> x( v );
    span<const int> y( w );

    EXPECT( std::equal( x.begin(), x.end(), arr ) );
    EXPECT( std::equal( y.begin(), y.end(), arr ) );
}

CASE( "span<>: Allows to copy-construct from another span of a compatible type" )
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
    span<      int> v( arr );
    span<const int> w( arr );

    span<const volatile int> x( v );
    span<const volatile int> y( v );

    EXPECT( std::equal( x.begin(), x.end(), arr ) );
    EXPECT( std::equal( y.begin(), y.end(), arr ) );
}

CASE( "span<>: Allows to move-construct from another span of the same type (C++11)" )
{
#if gsl_CPP11_OR_GREATER
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<      int> v(( span<      int>( arr ) ));
//  span<      int> w(( span<const int>( arr ) ));
    span<const int> x(( span<      int>( arr ) ));
    span<const int> y(( span<const int>( arr ) ));

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
    EXPECT( std::equal( x.begin(), x.end(), arr ) );
    EXPECT( std::equal( y.begin(), y.end(), arr ) );
#else
    EXPECT( !!"move-semantics are not available (no C++11)" );
#endif
}

CASE( "span<>: Allows to copy-assign from another span of the same type" )
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
    span<      int> v( arr );
    span<      int> s;
    span<const int> t;

    s = v;
    t = v;

    EXPECT( std::equal( s.begin(), s.end(), arr ) );
    EXPECT( std::equal( t.begin(), t.end(), arr ) );
}

CASE( "span<>: Allows to move-assign from another span of the same type (C++11)" )
{
#if gsl_CPP11_OR_GREATER
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
    span<      int> v;
    span<const int> w;

    v = span<int>( arr );
    w = span<int>( arr );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
    EXPECT( std::equal( w.begin(), w.end(), arr ) );
#else
    EXPECT( !!"move-semantics are not available (no C++11)" );
#endif
}

CASE( "span<>: Allows to create a sub span of the first n elements" )
{
    int arr[] = { 1, 2, 3, 4, 5, };
    span<int> v( arr );
    index_type count = 3;

    span<      int> s = v.first( count );
    span<const int> t = v.first( count );

    EXPECT( s.size() == count );
    EXPECT( t.size() == count );
    EXPECT( std::equal( s.begin(), s.end(), arr ) );
    EXPECT( std::equal( t.begin(), t.end(), arr ) );
}

CASE( "span<>: Allows to create a sub span of the last n elements" )
{
    int arr[] = { 1, 2, 3, 4, 5, };
    span<int> v( arr );
    index_type count = 3;

    span<      int> s = v.last( count );
    span<const int> t = v.last( count );

    EXPECT( s.size() == count );
    EXPECT( t.size() == count );
    EXPECT( std::equal( s.begin(), s.end(), arr + v.size() - count ) );
    EXPECT( std::equal( t.begin(), t.end(), arr + v.size() - count ) );
}

CASE( "span<>: Allows to create a sub span starting at a given offset" )
{
    int arr[] = { 1, 2, 3, };
    span<int> v( arr );
    index_type offset = 1;

    span<      int> s = v.subspan( offset );
    span<const int> t = v.subspan( offset );

    EXPECT( s.size() == v.size() - offset );
    EXPECT( t.size() == v.size() - offset );
    EXPECT( std::equal( s.begin(), s.end(), arr + offset ) );
    EXPECT( std::equal( t.begin(), t.end(), arr + offset ) );
}

CASE( "span<>: Allows to create a sub span starting at a given offset with a given length" )
{
    int arr[] = { 1, 2, 3, };
    span<int> v( arr );
    index_type offset = 1;
    index_type length = 1;

    span<      int> s = v.subspan( offset, length );
    span<const int> t = v.subspan( offset, length );

    EXPECT( s.size() == length );
    EXPECT( t.size() == length );
    EXPECT( std::equal( s.begin(), s.end(), arr + offset ) );
    EXPECT( std::equal( t.begin(), t.end(), arr + offset ) );
}

CASE( "span<>: Allows to create an empty sub span at full offset" )
{
    int arr[] = { 1, 2, 3, };
    span<int> v( arr );
    index_type offset = v.size();

    span<      int> s = v.subspan( offset );
    span<const int> t = v.subspan( offset );

    EXPECT( s.empty() );
    EXPECT( t.empty() );
}

CASE( "span<>: Allows to create an empty sub span at full offset with zero length" )
{
    int arr[] = { 1, 2, 3, };
    span<int> v( arr );
    index_type offset = v.size();
    index_type length = 0;

    span<      int> s = v.subspan( offset, length );
    span<const int> t = v.subspan( offset, length );

    EXPECT( s.empty() );
    EXPECT( t.empty() );
}

CASE( "span<>: Allows forward iteration" )
{
    int arr[] = { 1, 2, 3, };
    span<int> v( arr );

    for ( span<int>::iterator pos = v.begin(); pos != v.end(); ++pos )
    {
        EXPECT( *pos == arr[ std::distance( v.begin(), pos )] );
    }
}

CASE( "span<>: Allows const forward iteration" )
{
    int arr[] = { 1, 2, 3, };
    span<int> v( arr );

    for ( span<int>::const_iterator pos = v.cbegin(); pos != v.cend(); ++pos )
    {
        EXPECT( *pos == arr[ std::distance( v.cbegin(), pos )] );
    }
}

CASE( "span<>: Allows reverse iteration" )
{
    int arr[] = { 1, 2, 3, };
    span<int> v( arr );

    for ( span<int>::reverse_iterator pos = v.rbegin(); pos != v.rend(); ++pos )
    {
        EXPECT( *pos == arr[ v.size() - 1 - std::distance(v.rbegin(), pos)] );
    }
}

CASE( "span<>: Allows const reverse iteration" )
{
    int arr[] = { 1, 2, 3, };
    const span<int> v( arr );

    for ( span<int>::const_reverse_iterator pos = v.crbegin(); pos != v.crend(); ++pos )
    {
        EXPECT( *pos == arr[ v.size() - 1 - std::distance(v.crbegin(), pos)] );
    }
}

CASE( "span<>: Allows to observe an element via array indexing" )
{
    int arr[] = { 1, 2, 3, };
    span<int>       v( arr );
    span<int> const w( arr );

    for ( index_type i = 0; i < v.size(); ++i )
    {
        EXPECT( v[i] == arr[i] );
        EXPECT( w[i] == arr[i] );
    }
}

CASE( "span<>: Allows to observe an element via call indexing" )
{
    int arr[] = { 1, 2, 3, };
    span<int>       v( arr );
    span<int> const w( arr );

    for ( index_type i = 0; i < v.size(); ++i )
    {
        EXPECT( v(i) == arr[i] );
        EXPECT( w(i) == arr[i] );
    }
}

CASE( "span<>: Allows to observe an element via at()" )
{
    int arr[] = { 1, 2, 3, };
    span<int>       v( arr );
    span<int> const w( arr );

    for ( index_type i = 0; i < v.size(); ++i )
    {
        EXPECT( v.at(i) == arr[i] );
        EXPECT( w.at(i) == arr[i] );
    }
}

CASE( "span<>: Allows to observe an element via data()" )
{
    int arr[] = { 1, 2, 3, };
    span<int>       v( arr );
    span<int> const w( arr );

    EXPECT( *v.data() == *v.begin() );
    EXPECT( *w.data() == *v.begin() );

    for ( index_type i = 0; i < v.size(); ++i )
    {
        EXPECT( v.data()[i] == arr[i] );
        EXPECT( w.data()[i] == arr[i] );
    }
}

CASE( "span<>: Allows to change an element via array indexing" )
{
    int arr[] = { 1, 2, 3, };
    span<int>       v( arr );
    span<int> const w( arr );

    v[1] = 22;
    w[2] = 33;

    EXPECT( 22 == arr[1] );
    EXPECT( 33 == arr[2] );
}

CASE( "span<>: Allows to change an element via call indexing" )
{
    int arr[] = { 1, 2, 3, };
    span<int>       v( arr );
    span<int> const w( arr );

    v(1) = 22;
    w(2) = 33;

    EXPECT( 22 == arr[1] );
    EXPECT( 33 == arr[2] );
}

CASE( "span<>: Allows to change an element via at()" )
{
    int arr[] = { 1, 2, 3, };
    span<int>       v( arr );
    span<int> const w( arr );

    v.at(1) = 22;
    w.at(2) = 33;

    EXPECT( 22 == arr[1] );
    EXPECT( 33 == arr[2] );
}

CASE( "span<>: Allows to change an element via data()" )
{
    int arr[] = { 1, 2, 3, };

    span<int> v( arr );
    span<int> const w( arr );

    *v.data() = 22;
    EXPECT( 22 == *v.data() );

    *w.data() = 33;
    EXPECT( 33 == *w.data() );
}

CASE( "span<>: Allows to compare equal to another span of the same type" )
{
    int a[] = { 1 }, b[] = { 2 }, c[] = { 1, 2 };
    span<int> va( a );
    span<int> vb( b );
    span<int> vc( c );

    EXPECT(     va == va );
    EXPECT_NOT( vb == va );
    EXPECT_NOT( vc == va );
}

CASE( "span<>: Allows to compare unequal to another span of the same type" )
{
    int a[] = { 1 }, b[] = { 2 }, c[] = { 1, 2 };
    span<int> va( a );
    span<int> vb( b );
    span<int> vc( c );

    EXPECT_NOT( va != va );
    EXPECT(     vb != va );
    EXPECT(     vc != va );
}

CASE( "span<>: Allows to compare less than another span of the same type" )
{
    int a[] = { 1 }, b[] = { 2 }, c[] = { 1, 2 };
    span<int> va( a );
    span<int> vb( b );
    span<int> vc( c );

    EXPECT_NOT( va < va );
    EXPECT(     va < vb );
    EXPECT(     va < vc );
}

CASE( "span<>: Allows to compare less than or equal to another span of the same type" )
{
    int a[] = { 1 }, b[] = { 2 }, c[] = { 1, 2 };
    span<int> va( a );
    span<int> vb( b );
    span<int> vc( c );

    EXPECT_NOT( vb <= va );
    EXPECT(     va <= vb );
    EXPECT(     va <= vc );
}

CASE( "span<>: Allows to compare greater than another span of the same type" )
{
    int a[] = { 1 }, b[] = { 2 }, c[] = { 1, 2 };
    span<int> va( a );
    span<int> vb( b );
    span<int> vc( c );

    EXPECT_NOT( va > va );
    EXPECT(     vb > va );
    EXPECT(     vc > va );
}

CASE( "span<>: Allows to compare greater than or equal to another span of the same type" )
{
    int a[] = { 1 }, b[] = { 2 }, c[] = { 1, 2 };
    span<int> va( a );
    span<int> vb( b );
    span<int> vc( c );

    EXPECT_NOT( va >= vb );
    EXPECT(     vb >= va );
    EXPECT(     vc >= va );
}

CASE( "span<>: Allows to compare to another span of the same type and different cv-ness (non-standard)" )
{
#if gsl_CONFIG_ALLOWS_NONSTRICT_SPAN_COMPARISON
    int aa[] = { 1 }, bb[] = { 2 };
    span<         int>  a( aa );
    span<   const int> ca( aa );
    span<volatile int> va( aa );
    span<         int>  b( bb );
    span<   const int> cb( bb );

    EXPECT( va == ca );
    EXPECT(  a == va );

    EXPECT(  a == ca );
    EXPECT(  a != cb );
    EXPECT(  a <= cb );
    EXPECT(  a <  cb );
    EXPECT(  b >= ca );
    EXPECT(  b >  ca );
#else
    EXPECT( !!"span<>: cannot compare different types (gsl_CONFIG_ALLOWS_NONSTRICT_SPAN_COMPARISON=0)" );
#endif
}

CASE( "span<>: Allows to compare empty spans as equal" )
{
    int a;

    span<int> p;
    span<int> q;
    span<int> r( &a, index_type( 0 ) );

    EXPECT( p == q );
    EXPECT( p == r );

#if gsl_HAVE_NULLPTR
    span<int> s( nullptr, 0 );
    span<int> t( nullptr, 0 );

    EXPECT( s == p );
    EXPECT( s == r );
    EXPECT( s == t );
#endif
}

CASE( "span<>: Allows to test for empty span via empty(), empty case" )
{
    span<int> v;

    EXPECT( v.empty() );
}

CASE( "span<>: Allows to test for empty span via empty(), non-empty case" )
{
    int a[] = { 1 };
    span<int> v( a );

    EXPECT_NOT( v.empty() );
}

CASE( "span<>: Allows to obtain the number of elements via size()" )
{
    int a[] = { 1, 2, 3, };
    int b[] = { 1, 2, 3, 4, 5, };

    span<int> z;
    span<int> va( a );
    span<int> vb( b );

    EXPECT( va.size() == index_type( gsl_DIMENSION_OF( a ) ) );
    EXPECT( vb.size() == index_type( gsl_DIMENSION_OF( b ) ) );
    EXPECT(  z.size() == index_type( 0 ) );
}

CASE( "span<>: Allows to obtain the number of elements via length()" )
{
    int a[] = { 1, 2, 3, };
    int b[] = { 1, 2, 3, 4, 5, };

    span<int> z;
    span<int> va( a );
    span<int> vb( b );

    EXPECT( va.length() == index_type( gsl_DIMENSION_OF( a ) ) );
    EXPECT( vb.length() == index_type( gsl_DIMENSION_OF( b ) ) );
    EXPECT(  z.length() == index_type( 0 ) );
}

CASE( "span<>: Allows to obtain the number of bytes via size_bytes()" )
{
    int a[] = { 1, 2, 3, };
    int b[] = { 1, 2, 3, 4, 5, };

    span<int> z;
    span<int> va( a );
    span<int> vb( b );

    EXPECT( va.size_bytes() == index_type( gsl_DIMENSION_OF( a ) * sizeof(int) ) );
    EXPECT( vb.size_bytes() == index_type( gsl_DIMENSION_OF( b ) * sizeof(int) ) );
    EXPECT(  z.size_bytes() == index_type( 0 * sizeof(int) ) );
}

CASE( "span<>: Allows to obtain the number of bytes via length_bytes()" )
{
    int a[] = { 1, 2, 3, };
    int b[] = { 1, 2, 3, 4, 5, };

    span<int> z;
    span<int> va( a );
    span<int> vb( b );

    EXPECT( va.length_bytes() == index_type( gsl_DIMENSION_OF( a ) * sizeof(int) ) );
    EXPECT( vb.length_bytes() == index_type( gsl_DIMENSION_OF( b ) * sizeof(int) ) );
    EXPECT(  z.length_bytes() == index_type( 0 * sizeof(int) ) );
}

CASE( "span<>: Allows to swap with another span of the same type" )
{
    int a[] = { 1, 2, 3, };
    int b[] = { 1, 2, 3, 4, 5, };

    span<int> va0( a );
    span<int> vb0( b );
    span<int> va( a );
    span<int> vb( b );

    va.swap( vb );

    EXPECT( va == vb0 );
    EXPECT( vb == va0 );
}

static bool is_little_endian()
{
    union U
    {
        U() : i(1) {}
        int i;
        char c[ sizeof(int) ];
    };
    return 1 != U().c[ sizeof(int) - 1 ];
}

CASE( "span<>: Allows to view the elements as read-only bytes" )
{
#if gsl_HAVE_SIZED_TYPES
    typedef int32_t type;
#else
    typedef int type;
#endif
    typedef gsl::byte gyte;

    EXPECT( sizeof( type ) == size_t( 4 ) );

    type  a[] = { 0x12345678, };
    gyte be[] = { to_byte(0x12), to_byte(0x34), to_byte(0x56), to_byte(0x78), };
    gyte le[] = { to_byte(0x78), to_byte(0x56), to_byte(0x34), to_byte(0x12), };

    gyte * b = is_little_endian() ? le : be;

    span<int> va( a );
    span<const gyte> vb( va.as_bytes()  );
    span<const gyte> vc( as_bytes( va ) );

    EXPECT( vb[0] == b[0] );
    EXPECT( vc[0] == b[0] );
    EXPECT( vb[1] == b[1] );
    EXPECT( vc[1] == b[1] );
    EXPECT( vb[2] == b[2] );
    EXPECT( vc[2] == b[2] );
    EXPECT( vb[3] == b[3] );
    EXPECT( vc[3] == b[3] );
}

CASE( "span<>: Allows to view and change the elements as writable bytes" )
{
#if gsl_HAVE_SIZED_TYPES
    typedef int32_t type;
#else
    typedef int type;
#endif
    typedef gsl::byte gyte;

    EXPECT( sizeof(type) == size_t( 4 ) );

    type  a[] = { 0x0, };
    span<int> va( a );
    span<gyte> vb( va.as_writeable_bytes() );
    span<gyte> vc( as_writeable_bytes(va) );

    for ( size_t i = 0; i < sizeof(type); ++i )
    {
        EXPECT( vb[i] == to_byte(0) );
        EXPECT( vc[i] == to_byte(0) );
    }

    vb[0] = to_byte(0x42);

    EXPECT( vb[0] == to_byte(0x42) );
    for ( size_t i = 1; i < sizeof(type); ++i )
    {
        EXPECT( vb[i] == to_byte(0) );
        EXPECT( vc[i] == to_byte(0) );
    }
}

CASE( "span<>: Allows to view the elements as a span of another type" )
{
#if gsl_HAVE_SIZED_TYPES
    typedef int32_t type1;
    typedef int16_t type2;
#else
    typedef int   type1;
    typedef short type2;
#endif
    EXPECT( sizeof( type1 ) == size_t( 4 ) );
    EXPECT( sizeof( type2 ) == size_t( 2 ) );

    type1  a[] = { 0x12345678, };
    type2 be[] = { type2(0x1234), type2(0x5678), };
    type2 le[] = { type2(0x5678), type2(0x1234), };

    type2 * b = is_little_endian() ? le : be;

    span<type1> va( a );
#if gsl_COMPILER_MSVC_VERSION == 6
    span<type2> vb( va.as_span( type2() ) );
#else
    span<type2> vb( va.as_span<type2>() );
#endif

    EXPECT( vb[0] == b[0] );
    EXPECT( vb[1] == b[1] );
}

CASE( "span<>: Allows to change the elements from a span of another type" )
{
#if gsl_HAVE_SIZED_TYPES
    typedef int32_t type1;
    typedef int16_t type2;
#else
    typedef int   type1;
    typedef short type2;
#endif
    EXPECT( sizeof( type1 ) == size_t( 4 ) );
    EXPECT( sizeof( type2 ) == size_t( 2 ) );

    type1  a[] = { 0x0, };

    span<type1> va( a );
#if gsl_COMPILER_MSVC_VERSION == 6
    span<type2> vb( va.as_span( type2() ) );
#else
    span<type2> vb( va.as_span<type2>() );
#endif

    {for ( size_t i = 0; i < sizeof(type2); ++i ) EXPECT( vb[i] == type2(0) ); }

    vb[0] = 0x42;

    EXPECT( vb[0] == type2(0x42) );
    {for ( size_t i = 1; i < sizeof(type2); ++i ) EXPECT( vb[i] == type2(0) ); }
}

CASE( "span<>: Allows to copy a span to another span of the same element type" )
{
    int a[] = { 1, 2, 3,       };
    int b[] = { 0, 0, 0, 0, 0, };

    span<int> src( a );
    span<int> dst( b );

    copy( src, dst );

    EXPECT( src == dst.subspan( 0, src.size() ) );
}

CASE( "span<>: Allows to copy a span to another span of a different element type" )
{
    char a[] = { 'a', 'b', 'c',       };
    int  b[] = {  0 ,  0 ,  0 , 0, 0, };

    span<char> src( a );
    span<int > dst( b );

    copy( src, dst );

    for ( span<int>::index_type i = 0; i < src.size(); ++i )
    {
        EXPECT( src[i] == dst[i] );
    }
}

CASE( "make_span(): Allows building from two pointers" )
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<int> v = make_span( arr, arr + gsl_DIMENSION_OF( arr ) );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
}

CASE( "make_span(): Allows building from two const pointers" )
{
    const int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<const int> v = make_span( arr, arr + gsl_DIMENSION_OF( arr ) );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
}

CASE( "make_span(): Allows building from a non-null pointer and a size" )
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<int> v = make_span( arr, gsl_DIMENSION_OF( arr ) );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
}

CASE( "make_span(): Allows building from a non-null const pointer and a size" )
{
    const int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<const int> v = make_span( arr, gsl_DIMENSION_OF( arr ) );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
}

CASE( "make_span(): Allows building from a C-array" )
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<int> v = make_span( arr );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
}

CASE( "make_span(): Allows building from a const C-array" )
{
    const int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<const int> v = make_span( arr );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
}

CASE( "make_span(): Allows building from a std::array<> (C++11)" )
{
# if gsl_HAVE_ARRAY
    std::array<int,9> arr = {{ 1, 2, 3, 4, 5, 6, 7, 8, 9, }};

    span<int> v = make_span( arr );

    EXPECT( std::equal( v.begin(), v.end(), arr.begin() ) );
#else
    EXPECT( !!"std::array<> is not available (no C++11)" );
#endif
}

CASE( "make_span(): Allows building from a const std::array<> (C++11)" )
{
# if gsl_HAVE_ARRAY
    const std::array<int,9> arr = {{ 1, 2, 3, 4, 5, 6, 7, 8, 9, }};

    span<const int> v = make_span( arr );

    EXPECT( std::equal( v.begin(), v.end(), arr.begin() ) );
#else
    EXPECT( !!"std::array<> is not available (no C++11)" );
#endif
}

CASE( "make_span(): Allows building from a container (std::vector<>)" )
{
# if gsl_HAVE_INITIALIZER_LIST
    std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
#else
    std::vector<int> vec; {for ( int i = 1; i < 10; ++i ) vec.push_back(i); }
#endif
    span<int> v = make_span( vec );

    EXPECT( std::equal( v.begin(), v.end(), vec.begin() ) );
}

CASE( "make_span(): Allows building from a const container (std::vector<>)" )
{
# if gsl_HAVE_INITIALIZER_LIST
    const std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
#else
    const std::vector<int> vec( 10, 42 );
#endif
    span<const int> v = make_span( vec );

    EXPECT( std::equal( v.begin(), v.end(), vec.begin() ) );
}

CASE( "make_span(): Allows building from an empty gsl::shared_ptr (C++11)" )
{
# if gsl_HAVE_SHARED_PTR
    auto ptr = gsl::shared_ptr<int>( gsl_nullptr );

    auto s = make_span( ptr );

    EXPECT( s.length() == index_type( 0) );
    EXPECT( s.data()   == gsl_nullptr    );
#else
    EXPECT( !!"gsl::shared_ptr<> is not available (no C++11)" );
#endif
}

CASE( "make_span(): Allows building from an empty gsl::unique_ptr (C++11)" )
{
# if gsl_HAVE_UNIQUE_PTR
    auto ptr = gsl::unique_ptr<int>( gsl_nullptr );

    auto s = make_span( ptr );

    EXPECT( s.length() == index_type( 0 ) );
    EXPECT( s.data()   == gsl_nullptr     );
#else
    EXPECT( !!"gsl::unique_ptr<> is not available (no C++11)" );
#endif
}

CASE( "make_span(): Allows building from an empty gsl::unique_ptr (array, C++11)" )
{
# if gsl_HAVE_UNIQUE_PTR
    auto arr = std::unique_ptr<int[]>( gsl_nullptr );

    auto s = make_span( arr, 0 );

    EXPECT( s.length() == index_type( 0 ) );
    EXPECT( s.data()   == gsl_nullptr     );
#else
    EXPECT( !!"gsl::unique_ptr<> is not available (no C++11)" );
#endif
}

CASE( "make_span(): Allows building from a non-empty gsl::shared_ptr (C++11)" )
{
# if gsl_HAVE_SHARED_PTR
    auto ptr = gsl::make_shared<int>( 4 );

    auto s = make_span( ptr );

    EXPECT( s.length() == index_type( 1 ) );
    EXPECT( s.data()   == ptr.get()       );
    EXPECT( s[0]       == 4               );
#else
    EXPECT( !!"gsl::shared_ptr<> is not available (no C++11)" );
#endif
}

CASE( "make_span(): Allows building from a non-empty gsl::unique_ptr (C++11)" )
{
# if gsl_HAVE_UNIQUE_PTR
# if gsl_HAVE_MAKE_UNIQUE
    auto ptr = gsl::make_unique<int>( 4 );
#else
    auto ptr = gsl::unique_ptr<int>( new int(4) );
#endif

    auto s = make_span( ptr );

    EXPECT( s.length() == index_type( 1 ) );
    EXPECT( s.data()   == ptr.get()       );
    EXPECT( s[0]       == 4               );
#else
    EXPECT( !!"gsl::unique_ptr<> is not available (no C++11)" );
#endif
}

CASE( "make_span(): Allows building from a non-empty gsl::unique_ptr (array, C++11)" )
{
# if gsl_HAVE_UNIQUE_PTR
# if gsl_HAVE_MAKE_SHARED
    auto arr = std::make_unique<int[]>(4);
#else
    auto arr = std::unique_ptr<int[]>( new int[4] );
#endif
    for ( int i = 0; i < 4; i++ )
        arr[i] = i + 1;

    auto s = make_span( arr, 4 );

    EXPECT( s.length() == index_type( 4 ) );
    EXPECT( s.data()   == arr.get()       );
    EXPECT( s[0]       == 1               );
    EXPECT( s[1]       == 2               );
#else
    EXPECT( !!"gsl::unique_ptr<> is not available (no C++11)" );
#endif
}

// end of file
