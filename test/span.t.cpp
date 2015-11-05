//
// gsl-lite is based on GSL: Guidelines Support Library,
// https://github.com/microsoft/gsl
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

namespace {

CASE( "span<>: Disallows construction from a C-array of incompatible type (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)" )
{
#if gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS
    short arr[] = { 1, 2, 3, };
    span<int> v = arr;
#endif
}

CASE( "span<>: Disallows construction from a std::array of incompatible type (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)" )
{
#if gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS
# if gsl_HAVE_ARRAY
    std::array<long,3> arr = {{ 1L, 2L, 3L, }};
    span<int> v( arr);
# endif
#endif
}

CASE( "span<>: Terminates construction from a nullptr and a non-zero size" )
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

CASE( "span<>: Terminates construction from a C-array with size exceeding array length" )
{
#if gsl_COMPILER_MSVC_VERSION != 6
    struct F { static void blow() { int arr[] = { 1, 2, 3, }; span<int> v( arr, 7 ); } };

    EXPECT_THROWS( F::blow() );
#else
    EXPECT( !!"not available for VC6" );
#endif
}

CASE( "span<>: Allows default construction" )
{
    span<int> v;
    EXPECT( v.size() == size_t( 0 ) );
}

CASE( "span<>: Allows construction from a nullptr and a zero size" )
{
#if gsl_HAVE_NULLPTR
    span<int> v( nullptr, 0 );
    EXPECT( v.size() == size_t( 0 ) );
#else
    EXPECT( !!"nullptr is not available (no C++11)" );
#endif
}

CASE( "span<>: Allows construction from two pointers" )
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
    
    span<int> v( arr, arr + gsl_DIMENSION_OF( arr ) );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
}

CASE( "span<>: Allows construction from a non-null pointer and a size" )
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
    
    span<int> v( arr, gsl_DIMENSION_OF( arr ) );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
}

CASE( "span<>: Allows construction from a any pointer and a zero size" )
{
    struct F { 
        typedef span<int>::size_type size_type;
        static void null() { 
            int * p = NULL; span<int> v( p, (size_type)0 ); 
        } 
        static void nonnull() { 
            int i = 7; int * p = &i; span<int> v( p, (size_type)0 ); 
        } 
    };
    
    EXPECT_NO_THROW( F::null() );
    EXPECT_NO_THROW( F::nonnull() );
}

CASE( "span<>: Allows construction from a C-array" )
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
    span<int> v( arr );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
}

CASE( "span<>: Allows construction from a C-array with size" )
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<int> v( arr, gsl_DIMENSION_OF(arr) );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );

#if gsl_CPP14_OR_GREATER
    span<int> w( arr, 3 );

    EXPECT( std::equal( w.begin(), w.end(), arr, arr + 3 ) );
#endif
}

CASE( "span<>: Allows construction from a std::array<>" )
{
# if gsl_HAVE_ARRAY
    std::array<int,9> arr = {{ 1, 2, 3, 4, 5, 6, 7, 8, 9, }};
    span<int> v( arr );

    EXPECT( std::equal( v.begin(), v.end(), arr.begin() ) );
#else
    EXPECT( !!"std::array<> is not available (no C++11)" );
#endif
}

CASE( "span<>: Allows construction from a container (std::vector<>)" )
{
# if gsl_HAVE_INITIALIZER_LIST
    std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
#else
    std::vector<int> vec; {for ( int i = 1; i < 10; ++i ) vec.push_back(i); }
#endif
    span<int> v( vec );

    EXPECT( std::equal( v.begin(), v.end(), vec.begin() ) );
}

CASE( "span<>: Allows construction from another view of the same type" )
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
    span<int> v( arr );

    span<int> w( v );

    EXPECT( std::equal( w.begin(), w.end(), arr ) );
}

CASE( "span<>: Allows assignment from another view of the same type" )
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
    span<int> v( arr );
    span<int> w;

    w = v;

    EXPECT( std::equal( w.begin(), w.end(), arr ) );
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
    span<int> v( arr );
    
    for ( span<int>::const_reverse_iterator pos = v.crbegin(); pos != v.crend(); ++pos )
    {
        EXPECT( *pos == arr[ v.size() - 1 - std::distance(v.crbegin(), pos)] );
    }
}

CASE( "span<>: Allows conversion to bool (true if non-empty)" )
{
    int arr[] = { 1, 2, 3, };
    span<int> v( arr );
    
    EXPECT( !!v );
}

CASE( "span<>: Allows element access via array indexing" )
{
    int arr[] = { 1, 2, 3, };
    span<int> v( arr );
    
    for ( size_t i = 0; i < v.size(); ++i )
    {
        EXPECT( v[i] == arr[i] );
    }
}

CASE( "span<>: Allows element access via at()" ) 
{
    int arr[] = { 1, 2, 3, };
    span<int> v( arr );
    
    for ( size_t i = 0; i < v.size(); ++i )
    {
        EXPECT( v.at(i) == arr[i] );
    }
}

CASE( "span<>: Allows element access via data()" ) 
{
    int arr[] = { 1, 2, 3, };
    span<int> v( arr );
    
    EXPECT( *v.data() == *v.begin() );

    for ( size_t i = 0; i < v.size(); ++i )
    {
        EXPECT( v.data()[i] == arr[i] );
    }
}

CASE( "span<>: Allows to compare equal to another view of the same type" )
{
    int a[] = { 1 }, b[] = { 2 }, c[] = { 1, 2 };
    span<int> va( a );
    span<int> vb( b );
    span<int> vc( c );
    
    EXPECT(     (va == va) );
    EXPECT_NOT( (vb == va) );
    EXPECT_NOT( (vc == va) );
}

CASE( "span<>: Allows to compare unequal to another view of the same type" )
{
    int a[] = { 1 }, b[] = { 2 }, c[] = { 1, 2 };
    span<int> va( a );
    span<int> vb( b );
    span<int> vc( c );
    
    EXPECT_NOT( (va != va) );
    EXPECT(     (vb != va) );
    EXPECT(     (vc != va) );
}

CASE( "span<>: Allows to compare less than another view of the same type" )
{
    int a[] = { 1 }, b[] = { 2 }, c[] = { 1, 2 };
    span<int> va( a );
    span<int> vb( b );
    span<int> vc( c );
    
    EXPECT_NOT( (va < va) );
    EXPECT(     (va < vb) );
    EXPECT(     (va < vc) );
}

CASE( "span<>: Allows to compare less than or equal to another view of the same type" )
{
    int a[] = { 1 }, b[] = { 2 }, c[] = { 1, 2 };
    span<int> va( a );
    span<int> vb( b );
    span<int> vc( c );
    
    EXPECT_NOT( (vb <= va) );
    EXPECT(     (va <= vb) );
    EXPECT(     (va <= vc) );
}

CASE( "span<>: Allows to compare greater than another view of the same type" )
{
    int a[] = { 1 }, b[] = { 2 }, c[] = { 1, 2 };
    span<int> va( a );
    span<int> vb( b );
    span<int> vc( c );
    
    EXPECT_NOT( (va > va) );
    EXPECT(     (vb > va) );
    EXPECT(     (vc > va) );
}

CASE( "span<>: Allows to compare greater than or equal to another view of the same type" )
{
    int a[] = { 1 }, b[] = { 2 }, c[] = { 1, 2 };
    span<int> va( a );
    span<int> vb( b );
    span<int> vc( c );
    
    EXPECT_NOT( (va >= vb) );
    EXPECT(     (vb >= va) );
    EXPECT(     (vc >= va) );
}

CASE( "span<>: Allows to test for empty view via empty(), empty case" ) 
{
    span<int> v;

    EXPECT( v.empty() );
}

CASE( "span<>: Allows to test for empty view via empty(), non-empty case" ) 
{
    int a[] = { 1 };
    span<int> v( a );

    EXPECT_NOT( v.empty() );
}

CASE( "span<>: Allows to obtain number of elements via size()" ) 
{
    int a[] = { 1, 2, 3, };
    int b[] = { 1, 2, 3, 4, 5, };

    span<int> z;
    span<int> va( a );
    span<int> vb( b );

    EXPECT( va.size() == gsl_DIMENSION_OF( a ) );
    EXPECT( vb.size() == gsl_DIMENSION_OF( b ) );
    EXPECT(  z.size() == size_t( 0 ) );
}

CASE( "span<>: Allows to obtain number of elements via length()" ) 
{
    int a[] = { 1, 2, 3, };
    int b[] = { 1, 2, 3, 4, 5, };

    span<int> z;
    span<int> va( a );
    span<int> vb( b );

    EXPECT( va.length() == gsl_DIMENSION_OF( a ) );
    EXPECT( vb.length() == gsl_DIMENSION_OF( b ) );
    EXPECT(  z.length() == size_t( 0 ) );
}

CASE( "span<>: Allows to obtain number of elements via used_length()" ) 
{
    int a[] = { 1, 2, 3, };
    int b[] = { 1, 2, 3, 4, 5, };

    span<int> z;
    span<int> va( a );
    span<int> vb( b );

    EXPECT( va.used_length() == gsl_DIMENSION_OF( a ) );
    EXPECT( vb.used_length() == gsl_DIMENSION_OF( b ) );
    EXPECT(  z.used_length() == size_t( 0 ) );
}

CASE( "span<>: Allows to obtain number of bytes via bytes()" ) 
{
    int a[] = { 1, 2, 3, };
    int b[] = { 1, 2, 3, 4, 5, };

    span<int> z;
    span<int> va( a );
    span<int> vb( b );

    EXPECT( va.bytes() == gsl_DIMENSION_OF( a ) * sizeof(int) );
    EXPECT( vb.bytes() == gsl_DIMENSION_OF( b ) * sizeof(int) );
    EXPECT(  z.bytes() == 0 * sizeof(int) );
}

CASE( "span<>: Allows to obtain number of bytes via used_bytes()" ) 
{
    int a[] = { 1, 2, 3, };
    int b[] = { 1, 2, 3, 4, 5, };

    span<int> z;
    span<int> va( a );
    span<int> vb( b );

    EXPECT( va.used_bytes() == gsl_DIMENSION_OF( a ) * sizeof(int) );
    EXPECT( vb.used_bytes() == gsl_DIMENSION_OF( b ) * sizeof(int) );
    EXPECT(  z.used_bytes() == 0 * sizeof(int) );
}

CASE( "span<>: Allows to swap with another view of the same type" )
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
    gyte be[] = { gyte(0x12), gyte(0x34), gyte(0x56), gyte(0x78), };
    gyte le[] = { gyte(0x78), gyte(0x56), gyte(0x34), gyte(0x12), };

    gyte * b = is_little_endian() ? le : be;
    
    span<int> va( a );
    span<const gyte> vb( va.as_bytes() );
    
    EXPECT( vb[0] == b[0] );
    EXPECT( vb[1] == b[1] );
    EXPECT( vb[2] == b[2] );
    EXPECT( vb[3] == b[3] );
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
    
    {for ( size_t i = 0; i < sizeof(type); ++i ) EXPECT( vb[i] == gyte(0) ); }

    vb[0] = gyte(0x42);

    EXPECT( vb[0] == gyte(0x42) );
    {for ( size_t i = 1; i < sizeof(type); ++i ) EXPECT( vb[i] == gyte(0) ); }
}

CASE( "span<>: Allows to view the elements as a view of another type" ) 
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

CASE( "span<>: Allows to change the elements from a view of another type" ) 
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

CASE( "span<>: Allows building from two pointers" )
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<int> v = as_span( arr, arr + gsl_DIMENSION_OF( arr ) );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
}

CASE( "span<>: Allows building from a non-null pointer and a size" )
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<int> v = as_span( arr, gsl_DIMENSION_OF( arr ) );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
}

CASE( "span<>: Allows building from a C-array" )
{
#if gsl_COMPILER_MSVC_VERSION != 6
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<int> v = as_span( arr );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
#else
    EXPECT( !!"not available for VC6" );
#endif
}

CASE( "span<>: Allows building from a std::array<>" )
{
# if gsl_HAVE_ARRAY
    std::array<int,9> arr = {{ 1, 2, 3, 4, 5, 6, 7, 8, 9, }};

    span<int> v = as_span( arr );

    EXPECT( std::equal( v.begin(), v.end(), arr.begin() ) );
#else
    EXPECT( !!"std::array<> is not available (no C++11)" );
#endif
}

CASE( "span<>: Allows building from a container (std::vector<>)" )
{
# if gsl_HAVE_INITIALIZER_LIST
    std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
#else
    std::vector<int> vec; {for ( int i = 1; i < 10; ++i ) vec.push_back(i); }
#endif
    span<int> v = as_span( vec );

    EXPECT( std::equal( v.begin(), v.end(), vec.begin() ) );
}


#if 0
CASE( "span_convertible" ) {}
CASE( "boundary_checks" ) {}
CASE( "span_parameter_test" ) {}
CASE( "md_access" ) {}
CASE( "span_factory_test" ) {}
CASE( "span_reshape_test" ) {}
CASE( "span_section_test" ) {}
CASE( "span_section" ) {}
CASE( "strided_span_constructors" ) {}
CASE( "strided_span_slice" ) {}
CASE( "strided_span_column_major" ) {}
CASE( "strided_span_bounds" ) {}
CASE( "strided_span_type_conversion" ) {}
CASE( "empty_arrays" ) {}
CASE( "index_constructor" ) {}
CASE( "index_operations" ) {}
CASE( "span_section_iteration" ) {}
CASE( "dynamic_span_section_iteration" ) {}
CASE( "strided_span_section_iteration" ) {}
CASE( "dynamic_strided_span_section_iteration" ) {}
CASE( "strided_span_section_iteration_3d" ) {}
CASE( "dynamic_strided_span_section_iteration_3d" ) {}
CASE( "strided_span_conversion" ) {}
CASE( "constructors" ) {}
CASE( "copyandassignment" ) {}
CASE( "span_first" ) {}
CASE( "span_last" ) {}
CASE( "custmized_span_size" ) {}
CASE( "span_sub" ) {}
CASE( "TestNullConstruction" ) {}
CASE( "ArrayConstruction" ) {}
CASE( "NonConstConstConversions" ) {}
CASE( "FixedSizeConversions" ) {}
CASE( "AsWriteableBytes" ) {}
CASE( "ArrayViewComparison" ) {}
#endif

}

// end of file
