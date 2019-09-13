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

#include <sstream>  // std::ostringstream
#include <string.h> // strlen()

#if gsl_HAVE( WCHAR )
# include <wchar.h> // wcslen()
#endif

using namespace gsl;

typedef string_span::index_type index_type;

template< typename T >
inline std::vector<T> vector_iota( int n )
{
  std::vector<T> result;

  for ( int i = 0; i < n; ++i )
    result.push_back( static_cast<T>( i ) );

  return result;
}

CASE( "string_span: Disallows construction of a string_span from a cstring_span (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)" )
{
#if gsl_CONFIG( CONFIRMS_COMPILATION_ERRORS )
    cstring_span sv = "hello";

    string_span v2 = sv;
#endif
}

CASE( "string_span: Disallows construction of a string_span from a const std::string (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)" )
{
#if gsl_CONFIG( CONFIRMS_COMPILATION_ERRORS )
    const std::string s = "hello, world";

    string_span sv( s );

    EXPECT( sv.length() == index_type( 5 ) );
    EXPECT( std::string( sv.data() ) == s );
#endif
}

CASE( "string_span: Allows to default-construct" )
{
    string_span s;

    EXPECT( s.size() == index_type( 0 ) );
}

CASE( "string_span: Allows to construct from a nullptr (C++11)" )
{
#if gsl_HAVE( NULLPTR )
    string_span  v( nullptr );
    cstring_span w( nullptr );

    EXPECT( v.size() == index_type( 0 ) );
    EXPECT( w.size() == index_type( 0 ) );
#else
    EXPECT( !!"nullptr is not available (no C++11)" );
#endif
}

CASE( "string_span: Allows to construct a cstring_span from a const C-string" )
{
    cstring_span sv = "hello";

    EXPECT( sv.length() == index_type( 5 ) );
    EXPECT( std::string( sv.data() ) ==  "hello" );
}

CASE( "string_span: Allows to construct a string_span from a non-const C-string and size" )
{
    char s[] = "hello";

    string_span sv( s, strlen( s ) );

    EXPECT( sv.length() == index_type( 5 ) );
    EXPECT( std::string( sv.data() ) == s );
}

CASE( "string_span: Allows to construct a string_span from a non-const C-string begin and end pointer" )
{
    char s[] = "hello";

    string_span sv( s, s + strlen( s ) );

    EXPECT( sv.length() == index_type( 5 ) );
    EXPECT( std::string( sv.data() ) == s );
}

CASE( "string_span: Allows to construct a string_span from a non-const C-array" )
{
    char s[] = { 'w', 'o', 'r', 'l', 'd', '\0' };

    string_span sv( s );

    EXPECT( sv.length() == index_type( 5 ) );
    EXPECT( std::string( sv.data() ) == "world" );
}

CASE( "string_span: Allows to construct a string_span from a non-const std::string" )
{
    std::string s = "hello, world";

    string_span sv( s );

    EXPECT( sv.length() == 12u );
    EXPECT( std::string( sv.data() ) == s );
}

CASE( "string_span: Allows to construct a string_span from a non-const std::array (C++11)" )
{
#if gsl_HAVE( ARRAY )
    std::array<char,6> arr = {{ 'w', 'o', 'r', 'l', 'd', '\0' }};

    string_span sv( arr );

    EXPECT( sv.length() == index_type( 5 ) );
    EXPECT( std::equal( sv.begin(), sv.end(), arr.begin() ) );
#else
    EXPECT( !!"std::array<> is not available (no C++11)" );
#endif
}

CASE( "string_span: Allows to construct a string_span from a non-const container (std::vector)" )
{

#if gsl_HAVE( INITIALIZER_LIST )
    std::vector<char> vec = { 'w', 'o', 'r', 'l', 'd', '\0' };
#else
    std::vector<char> vec( 6, 'w' );
#endif
#if gsl_HAVE( CONSTRAINED_SPAN_CONTAINER_CTOR ) || gsl_HAVE( UNCONSTRAINED_SPAN_CONTAINER_CTOR )
    string_span sv( vec );

    EXPECT( sv.length() == index_type( 6 ) );
    EXPECT( std::equal( sv.begin(), sv.end(), vec.begin() ) );
#else
    EXPECT( !!"(un)constrained construction from container is not available" );
#endif
}

CASE( "string_span: Allows to construct a string_span from a non-const container, via a tag (std::vector)" )
{
#if gsl_FEATURE_TO_STD( WITH_CONTAINER )
    std::vector<char> vec = vector_iota<char>( 10 );

    string_span sv( with_container, vec );

    EXPECT( sv.length() == index_type( 10 ) );
    EXPECT( std::equal( sv.begin(), sv.end(), vec.begin() ) );
#else
    EXPECT( !!"with_container is not available (gsl_FEATURE_WITH_CONTAINER_TO_STD)" );
#endif
}

CASE( "string_span: Allows to construct a cstring_span from a non-const C-string and size" )
{
    char s[] = "hello";

    cstring_span sv( s, strlen( s ) );

    EXPECT( sv.length() == index_type( 5 ) );
    EXPECT( std::string( sv.data() ) == s );
}

CASE( "string_span: Allows to construct a cstring_span from a non-const C-string begin and end pointer" )
{
    char s[] = "hello";

    cstring_span sv( s, s + strlen( s ) );

    EXPECT( sv.length() == index_type( 5 ) );
    EXPECT( std::string( sv.data() ) == s );
}

CASE( "string_span: Allows to construct a cstring_span from a non-const C-array" )
{
    char s[] = { 'w', 'o', 'r', 'l', 'd', '\0' };

    cstring_span sv( s );

#if gsl_HAVE( CONSTRAINED_SPAN_CONTAINER_CTOR ) || gsl_COMPILER_MSVC_VERSION
    EXPECT( sv.length() == index_type( 5 ) );
#elif gsl_HAVE( UNCONSTRAINED_SPAN_CONTAINER_CTOR )
    EXPECT( !!"Warning: Unconstrained span container constructor enabled: terminating '\0' included in span" );
    EXPECT( sv.length() == index_type( 6 ) );
#else
    EXPECT( sv.length() == index_type( 5 ) );
#endif
    EXPECT( std::string( sv.data() ) == "world" );
}

CASE( "string_span: Allows to construct a cstring_span from a non-const std::string" )
{
#if gsl_HAVE( CONSTRAINED_SPAN_CONTAINER_CTOR ) || gsl_HAVE( UNCONSTRAINED_SPAN_CONTAINER_CTOR )
    std::string s = "hello, world";

    cstring_span sv( s );

    EXPECT( sv.length() == 12u );
    EXPECT( std::string( sv.data() ) == s );
#else
    EXPECT( !!"(un)constrained construction from container is not available" );
#endif
}

CASE( "string_span: Allows to construct a cstring_span from a non-const std::array (C++11)" )
{
#if gsl_HAVE( ARRAY )
    std::array<char,6> arr = {{ 'w', 'o', 'r', 'l', 'd', '\0' }};

    cstring_span sv( arr );

    EXPECT( sv.length() == index_type( 5 ) );
    EXPECT( std::equal( sv.begin(), sv.end(), arr.begin() ) );
#else
    EXPECT( !!"std::array<> is not available (no C++11)" );
#endif
}

CASE( "string_span: Allows to construct a cstring_span from a non-const container (std::vector)" )
{
#if gsl_HAVE( INITIALIZER_LIST )
    std::vector<char> vec = { 'w', 'o', 'r', 'l', 'd', '\0' };
#else
    std::vector<char> vec( 6, 'w' );
#endif
#if gsl_HAVE( CONSTRAINED_SPAN_CONTAINER_CTOR ) || gsl_HAVE( UNCONSTRAINED_SPAN_CONTAINER_CTOR )
    cstring_span sv( vec );

    EXPECT( sv.length() == index_type( 6 ) );
    EXPECT( std::equal( sv.begin(), sv.end(), vec.begin() ) );
#else
    EXPECT( !!"(un)constrained construction from container is not available" );
#endif
}

CASE( "string_span: Allows to construct a cstring_span from a non-const container, via a tag (std::vector)" )
{
#if gsl_FEATURE_TO_STD( WITH_CONTAINER )
    std::vector<char> vec = vector_iota<char>( 9 );

    cstring_span sv( with_container, vec );

    EXPECT( sv.length() == index_type( 9 ) );
    EXPECT( std::equal( sv.begin(), sv.end(), vec.begin() ) );
#else
    EXPECT( !!"with_container is not available (gsl_FEATURE_WITH_CONTAINER_TO_STD)" );
#endif
}

CASE( "string_span: Allows to construct a cstring_span from a const C-string and size" )
{
    const char s[] = "hello";

    cstring_span sv( s, strlen( s ) );

    EXPECT( sv.length() == index_type( 5 ) );
    EXPECT( std::string( sv.data() ) == s );
}

CASE( "string_span: Allows to construct a cstring_span from a non-const C-string begin and end pointer" )
{
    const char s[] = "hello";

    cstring_span sv( s, s + strlen( s ) );

    EXPECT( sv.length() == index_type( 5 ) );
    EXPECT( std::string( sv.data() ) == s );
}

CASE( "string_span: Allows to construct a cstring_span from a const C-array" )
{
    const char s[] = { 'w', 'o', 'r', 'l', 'd', '\0' };

    cstring_span sv( s );

    EXPECT( sv.length() == index_type( 5 ) );
    EXPECT( std::string( sv.data() ) == "world" );
}

CASE( "string_span: Allows to construct a cstring_span from a const std::string" )
{
#if gsl_HAVE( CONSTRAINED_SPAN_CONTAINER_CTOR ) || gsl_HAVE( UNCONSTRAINED_SPAN_CONTAINER_CTOR )
    const std::string s = "hello, world";

    cstring_span sv( s );

    EXPECT( sv.length() == 12u );
    EXPECT( std::string( sv.data() ) == s );
#else
    EXPECT( !!"(un)constrained construction from container is not available" );
#endif
}

CASE( "string_span: Allows to construct a cstring_span from a const std::array (C++11)" )
{
#if gsl_HAVE( ARRAY )
    const std::array<char,6> arr = {{ 'w', 'o', 'r', 'l', 'd', '\0' }};

    cstring_span sv( arr );

    EXPECT( sv.length() == index_type( 5 ) );
    EXPECT( std::equal( sv.begin(), sv.end(), arr.begin() ) );
#else
    EXPECT( !!"std::array<> is not available (no C++11)" );
#endif
}

CASE( "string_span: Allows to construct a cstring_span from a const container (std::vector)" )
{
#if gsl_HAVE( INITIALIZER_LIST )
    const std::vector<char> vec = { 'w', 'o', 'r', 'l', 'd', '\0' };
#else
    const std::vector<char> vec( 6, 'w' );
#endif
#if gsl_HAVE( CONSTRAINED_SPAN_CONTAINER_CTOR ) || gsl_HAVE( UNCONSTRAINED_SPAN_CONTAINER_CTOR )
    cstring_span sv( vec );

    EXPECT( sv.length() == index_type( 6 ) );
    EXPECT( std::equal( sv.begin(), sv.end(), vec.begin() ) );
#else
    EXPECT( !!"(un)constrained construction from container is not available" );
#endif
}

CASE( "string_span: Allows to construct a cstring_span from a const container, via a tag (std::vector)" )
{
#if gsl_FEATURE_TO_STD( WITH_CONTAINER )
    const std::vector<char> vec = vector_iota<char>( 9 );

    cstring_span sv( with_container, vec );

    EXPECT( sv.length() == index_type( 9 ) );
    EXPECT( std::equal( sv.begin(), sv.end(), vec.begin() ) );
#else
    EXPECT( !!"with_container is not available (gsl_FEATURE_WITH_CONTAINER_TO_STD)" );
#endif
}

#if gsl_HAVE( WCHAR )

CASE( "string_span: Allows to construct a wstring_span from a non-const C-string and size" )
{
    wchar_t s[] = L"hello";

    wstring_span sv( s, wcslen( s ) );

    EXPECT( std::wstring( sv.data() ) == std::wstring( s ) );
}

CASE( "string_span: Allows to construct a wstring_span from a non-const C-string begin and end pointer" )
{
    wchar_t s[] = L"hello";

    wstring_span sv( s, s + wcslen( s ) );

    EXPECT( sv.length() == index_type( 5 ) );
    EXPECT( std::wstring( sv.data() ) == s );
}

CASE( "string_span: Allows to construct a wstring_span from a non-const C-array" )
{
    wchar_t s[] = { L'w', L'o', L'r', L'l', L'd', L'\0' };

    wstring_span sv( s );

    EXPECT( sv.length() == index_type( 5 ) );
    EXPECT( std::wstring( sv.data() ) == std::wstring( s ) );
}

CASE( "string_span: Allows to construct a wstring_span from a non-const std::wstring" )
{
    std::wstring s = L"hello, world";

    wstring_span sv( s );

    EXPECT( sv.length() == 12u );
    EXPECT( std::wstring( sv.data() ) == s );
}

CASE( "string_span: Allows to construct a wstring_span from a non-const std::array (C++11)" )
{
#if gsl_HAVE( ARRAY )
    std::array<wchar_t,6> arr = {{ 'w', 'o', 'r', 'l', 'd', '\0' }};

    wstring_span sv( arr );

    EXPECT( sv.length() == index_type( 5 ) );
    EXPECT( std::equal( sv.begin(), sv.end(), arr.begin() ) );
#else
    EXPECT( !!"std::array<> is not available (no C++11)" );
#endif
}

CASE( "string_span: Allows to construct a wstring_span from a non-const container (std::vector)" )
{
#if gsl_HAVE( INITIALIZER_LIST )
    std::vector<wchar_t> vec = { L'w', L'o', L'r', L'l', L'd', L'\0' };
#else
    std::vector<wchar_t> vec( 6, 'w' );
#endif
#if gsl_HAVE( CONSTRAINED_SPAN_CONTAINER_CTOR ) || gsl_HAVE( UNCONSTRAINED_SPAN_CONTAINER_CTOR )
    wstring_span sv( vec );

    EXPECT( sv.length() == index_type( 6 ) );
    EXPECT( std::equal( sv.begin(), sv.end(), vec.begin() ) );
#else
    EXPECT( !!"(un)constrained construction from container is not available" );
#endif
}

CASE( "string_span: Allows to construct a wstring_span from a non-const container, via a tag (std::vector)" )
{
#if gsl_FEATURE_TO_STD( WITH_CONTAINER )
    std::vector<wchar_t> vec = vector_iota<wchar_t>( 9 );

    wstring_span sv( with_container, vec );

    EXPECT( sv.length() == index_type( 9 ) );
    EXPECT( std::equal( sv.begin(), sv.end(), vec.begin() ) );
#else
    EXPECT( !!"with_container is not available (gsl_FEATURE_WITH_CONTAINER_TO_STD)" );
#endif
}

CASE( "string_span: Allows to construct a cwstring_span from a non-const C-string and size" )
{
    wchar_t s[] = L"hello";

    cwstring_span sv( s, wcslen( s ) );

    EXPECT( sv.length() == index_type( 5 ) );
    EXPECT( std::wstring( sv.data() ) == std::wstring( s ) );
}

CASE( "string_span: Allows to construct a cwstring_span from a non-const C-string begin and end pointer" )
{
    wchar_t s[] = L"hello";

    cwstring_span sv( s, s + wcslen( s ) );

    EXPECT( sv.length() == index_type( 5 ) );
    EXPECT( std::wstring( sv.data() ) == s );
}

CASE( "string_span: Allows to construct a cwstring_span from a non-const C-array" )
{
    wchar_t s[] = { L'w', L'o', L'r', L'l', L'd', L'\0' };

    cwstring_span sv( s );

#if gsl_HAVE( CONSTRAINED_SPAN_CONTAINER_CTOR ) || gsl_COMPILER_MSVC_VERSION
    EXPECT( sv.length() == index_type( 5 ) );
#elif gsl_HAVE( UNCONSTRAINED_SPAN_CONTAINER_CTOR )
    EXPECT( !!"Warning: Unconstrained span container constructor enabled: terminating '\0' included in span" );
    EXPECT( sv.length() == index_type( 6 ) );
#else
    EXPECT( sv.length() == index_type( 5 ) );
#endif
    EXPECT( std::wstring( sv.data() ) == std::wstring( s ) );
}

CASE( "string_span: Allows to construct a cwstring_span from a non-const std::wstring" )
{
    std::wstring s = L"hello, world";

    cwstring_span sv( s );

    EXPECT( sv.length() == 12u );
    EXPECT( std::wstring( sv.data() ) == s );
}

CASE( "string_span: Allows to construct a cwstring_span from a non-const std::array (C++11)" )
{
#if gsl_HAVE( ARRAY )
    std::array<wchar_t,6> arr = {{ 'w', 'o', 'r', 'l', 'd', '\0' }};

    cwstring_span sv( arr );

    EXPECT( sv.length() == index_type( 5 ) );
    EXPECT( std::equal( sv.begin(), sv.end(), arr.begin() ) );
#else
    EXPECT( !!"std::array<> is not available (no C++11)" );
#endif
}

CASE( "string_span: Allows to construct a cwstring_span from a non-const container (std::vector)" )
{
#if gsl_HAVE( INITIALIZER_LIST )
    std::vector<wchar_t> vec = { L'w', L'o', L'r', L'l', L'd', L'\0' };
#else
    std::vector<wchar_t> vec( 6, 'w' );
#endif
#if gsl_HAVE( CONSTRAINED_SPAN_CONTAINER_CTOR ) || gsl_HAVE( UNCONSTRAINED_SPAN_CONTAINER_CTOR )
    cwstring_span sv( vec );

    EXPECT( sv.length() == index_type( 6 ) );
    EXPECT( std::equal( sv.begin(), sv.end(), vec.begin() ) );
#else
    EXPECT( !!"(un)constrained construction from container is not available" );
#endif
}

CASE( "string_span: Allows to construct a cwstring_span from a non-const container, via a tag (std::vector)" )
{
#if gsl_FEATURE_TO_STD( WITH_CONTAINER )
    std::vector<wchar_t> vec = vector_iota<wchar_t>( 9 );

    cwstring_span sv( with_container, vec );

    EXPECT( sv.length() == index_type( 9 ) );
    EXPECT( std::equal( sv.begin(), sv.end(), vec.begin() ) );
#else
    EXPECT( !!"with_container is not available (gsl_FEATURE_WITH_CONTAINER_TO_STD)" );
#endif
}

CASE( "string_span: Allows to construct a cwstring_span from a const C-string and size" )
{
    const wchar_t s[] = L"hello";

    cwstring_span sv( s, wcslen( s ) );

    EXPECT( sv.length() == index_type( 5 ) );
    EXPECT( std::wstring( sv.data() ) == std::wstring( s ) );
}

CASE( "string_span: Allows to construct a cwstring_span from a const C-string begin and end pointer" )
{
    const wchar_t s[] = L"hello";

    cwstring_span sv( s, s + wcslen( s ) );

    EXPECT( sv.length() == index_type( 5 ) );
    EXPECT( std::wstring( sv.data() ) == s );
}

CASE( "string_span: Allows to construct a cwstring_span from a const C-array" )
{
    const wchar_t s[] = { L'w', L'o', L'r', L'l', L'd', L'\0' };

    cwstring_span sv( s );

    EXPECT( sv.length() == index_type( 5 ) );
    EXPECT( std::wstring( sv.data() ) == std::wstring( s ) );
}

CASE( "string_span: Allows to construct a cwstring_span from a const std::wstring" )
{
    const std::wstring s = L"hello, world";

    cwstring_span sv( s );

    EXPECT( sv.length() == 12u );
    EXPECT( std::wstring( sv.data() ) == s );
}

CASE( "string_span: Allows to construct a cwstring_span from a const std::array (C++11)" )
{
#if gsl_HAVE( ARRAY )
    const std::array<wchar_t,6> arr = {{ 'w', 'o', 'r', 'l', 'd', '\0' }};

    cwstring_span sv( arr );

    EXPECT( sv.length() == index_type( 5 ) );
    EXPECT( std::equal( sv.begin(), sv.end(), arr.begin() ) );
#else
    EXPECT( !!"std::array<> is not available (no C++11)" );
#endif
}

CASE( "string_span: Allows to construct a cwstring_span from a const container (std::vector)" )
{
#if gsl_HAVE( INITIALIZER_LIST )
    const std::vector<wchar_t> vec = { L'w', L'o', L'r', L'l', L'd', L'\0' };
#else
    const std::vector<wchar_t> vec( 6, 'w' );
#endif
#if gsl_HAVE( CONSTRAINED_SPAN_CONTAINER_CTOR ) || gsl_HAVE( UNCONSTRAINED_SPAN_CONTAINER_CTOR )
    cwstring_span sv( vec );

    EXPECT( sv.length() == index_type( 6 ) );
    EXPECT( std::equal( sv.begin(), sv.end(), vec.begin() ) );
#else
    EXPECT( !!"(un)constrained construction from container is not available" );
#endif
}

CASE( "string_span: Allows to construct a cwstring_span from a const container, via a tag (std::vector)" )
{
#if gsl_FEATURE_TO_STD( WITH_CONTAINER )
    const std::vector<wchar_t> vec = vector_iota<wchar_t>( 9 );

    cwstring_span sv( with_container, vec );

    EXPECT( sv.length() == index_type( 9 ) );
    EXPECT( std::equal( sv.begin(), sv.end(), vec.begin() ) );
#else
    EXPECT( !!"with_container is not available (gsl_FEATURE_WITH_CONTAINER_TO_STD)" );
#endif
}

#endif // gsl_HAVE( WCHAR )

CASE( "string_span: Allows to copy-construct from another span of the same type" )
{
    cstring_span a = "hello";

    cstring_span b( a );

    EXPECT( a.length() == b.length() );
    EXPECT( std::equal( a.begin(), a.end(), b.begin() ) );
}

CASE( "string_span: Allows to copy-construct from another span of a compatible type" )
{
    char hello[] = "hello";
    string_span a = hello;

    cstring_span b( a );

    EXPECT( a.length() == b.length() );
    EXPECT( std::equal( a.begin(), a.end(), b.begin() ) );
}

CASE( "string_span: Allows to move-construct from another span of the same type (C++11)" )
{
#if gsl_CPP11_OR_GREATER
    char hello[] = "hello";

    string_span a(( string_span( hello ) ));

    EXPECT( a.length() == index_type( strlen( hello ) ) );
    EXPECT( std::equal( a.begin(), a.end(), hello ) );
#else
    EXPECT( !!"move-semantics are not available (no C++11)" );
#endif
}

CASE( "string_span: Allows to copy-assign from another span of the same type" )
{
    char hello[] = "hello";
    string_span a = hello;
    string_span b;

    b = a;

    EXPECT( a.length() == b.length() );
    EXPECT( std::equal( a.begin(), a.end(), b.begin() ) );
}

CASE( "string_span: Allows to move-assign from another span of the same type (C++11)" )
{
#if gsl_CPP11_OR_GREATER
    char hello[] = "hello";
    string_span a;

    a = string_span( hello );

    EXPECT( a.length() == index_type( strlen( hello ) ) );
    EXPECT( std::equal( a.begin(), a.end(), hello ) );
#else
    EXPECT( !!"move-semantics are not available (no C++11)" );
#endif
}

CASE( "string_span: Allows to create a sub span of the first n elements" )
{
    char hello[] = "hello";
    string_span a( hello );
    index_type count = 2;

    string_span s = a.first( count );

    EXPECT( s.size() == count );
    EXPECT( std::equal( s.begin(), s.end(), hello ) );
}

CASE( "string_span: Allows to create a sub span of the last n elements" )
{
    char hello[] = "hello";
    string_span a( hello );
    index_type count = 2;

    string_span s = a.last( count );

    EXPECT( s.size() == count );
    EXPECT( std::equal( s.begin(), s.end(), hello + strlen(hello) - count ) );
}

CASE( "string_span: Allows to create a sub span starting at a given offset" )
{
    char hello[] = "hello";
    string_span a( hello );
    index_type offset = 1;

    string_span s = a.subspan( offset );

    EXPECT( s.size() == index_type( strlen(hello) - offset ) );
    EXPECT( std::equal( s.begin(), s.end(), hello + offset ) );
}

CASE( "string_span: Allows to create a sub span starting at a given offset with a given length" )
{
    char hello[] = "hello";
    string_span a( hello );
    index_type offset = 1;
    index_type length = 1;

    string_span s = a.subspan( offset, length );

    EXPECT( s.size() == length );
    EXPECT( std::equal( s.begin(), s.end(), hello + offset ) );
}

CASE( "string_span: Allows to create an empty sub span at full offset" )
{
    char hello[] = "hello";
    string_span a( hello );
    index_type offset = strlen( hello );

    string_span s = a.subspan( offset );

    EXPECT( s.empty() );
}

CASE( "string_span: Allows to create an empty sub span at full offset with zero length" )
{
    char hello[] = "hello";
    string_span a( hello );
    index_type offset = strlen( hello );
    index_type length = 0;

    string_span s = a.subspan( offset, length );

    EXPECT( s.empty() );
}

CASE( "string_span: Allows forward iteration" )
{
    char hello[] = "hello";
    string_span a( hello );

    for ( string_span::iterator pos = a.begin(); pos != a.end(); ++pos )
    {
        index_type i = narrow<index_type>( std::distance( a.begin(), pos ) );
        EXPECT( *pos == a[i] );
    }
}

CASE( "string_span: Allows const forward iteration" )
{
    char hello[] = "hello";
    string_span a( hello );

    for ( string_span::const_iterator pos = a.begin(); pos != a.end(); ++pos )
    {
        index_type i = narrow<index_type>( std::distance( a.cbegin(), pos ) );
        EXPECT( *pos == a[i] );
    }
}

CASE( "string_span: Allows reverse iteration" )
{
    char hello[] = "hello";
    string_span a( hello );

    for ( string_span::reverse_iterator pos = a.rbegin(); pos != a.rend(); ++pos )
    {
        index_type dist = narrow<index_type>( std::distance( a.rbegin(), pos ) );
        EXPECT( *pos == a[ a.size() - 1 - dist ] );
    }
}

CASE( "string_span: Allows const reverse iteration" )
{
    char hello[] = "hello";
    string_span a( hello );

    for ( string_span::const_reverse_iterator pos = a.crbegin(); pos != a.crend(); ++pos )
    {
        index_type dist = narrow<index_type>( std::distance( a.crbegin(), pos ) );
        EXPECT( *pos == a[ a.size() - 1 - dist ] );
    }
}

CASE( "string_span: Allows to observe an element via array indexing" )
{
    char hello[] = "hello";
    string_span a( hello );

    for ( index_type i = 0; i < a.size(); ++i )
    {
        EXPECT( a[i] == hello[i] );
    }
}

CASE( "string_span: Allows to observe an element via call indexing" )
{
    char hello[] = "hello";
    string_span a( hello );

    for ( index_type i = 0; i < a.size(); ++i )
    {
        EXPECT( a(i) == hello[i] );
    }
}

#if 0
CASE( "string_span: Allows to observe an element via at()" )
{
    char hello[] = "hello";
    string_span a( hello );

    for ( index_type i = 0; i < a.size(); ++i )
    {
        EXPECT( a.at(i) == hello[i] );
    }
}
#endif

CASE( "string_span: Allows to observe an element via data()" )
{
    char hello[] = "hello";
    string_span a( hello );

    EXPECT( *a.data() == *a.begin() );

    for ( index_type i = 0; i < a.size(); ++i )
    {
        EXPECT( a.data()[i] == hello[i] );
    }
}

CASE( "string_span: Allows to change an element via array indexing" )
{
    char hello[] = "hello";
    string_span a( hello );

    a[1] = '7';

    EXPECT( hello[1] == '7' );
}

CASE( "string_span: Allows to change an element via call indexing" )
{
    char hello[] = "hello";
    string_span a( hello );

    a(1) = '7';

    EXPECT( hello[1] == '7' );
}

#if 0
CASE( "string_span: Allows to change an element via at()" )
{
    char hello[] = "hello";
    string_span a( hello );

    a.at(1) = '7';

    EXPECT( hello[1] == '7' );
}
#endif

CASE( "string_span: Allows to change an element via data()" )
{
    char hello[] = "hello";
    string_span a( hello );

    *a.data() = '7';

    EXPECT( hello[0] == '7' );
}

CASE( "string_span: Allows to compare a string_span with another string_span" )
{
    char s[] = "hello";
    char t[] = "world";

    string_span sv( s, strlen( s ) );
    string_span tv( t, strlen( s ) );

    EXPECT( sv.length() == index_type( 5 ) );
    EXPECT( tv.length() == index_type( 5 ) );

    EXPECT( sv == sv );
    EXPECT( sv != tv );
    EXPECT( sv <= sv );
    EXPECT( sv <= tv );
    EXPECT( sv <  tv );
    EXPECT( tv >= tv );
    EXPECT( tv >= sv );
    EXPECT( tv >  sv );
}

CASE( "string_span: Allows to compare empty spans as equal (questionable)" "[.]" )
{
    string_span a, b;

    EXPECT( a == b );
}

CASE( "string_span: Allows to compare empty span to non-empty span" )
{
    char s[] = "hello";
    string_span a, b( s, strlen( s ) );

    EXPECT( a != b );
    EXPECT( b != a );
}

CASE( "string_span: Allows to compare a string_span with a cstring_span" )
{
#if gsl_CONFIG( ALLOWS_NONSTRICT_SPAN_COMPARISON )
    char s[] = "hello";

    string_span   sv( s, strlen( s ) );
    cstring_span csv( s, strlen( s ) );

    EXPECT( sv == csv );
#else
    EXPECT( !!"string_span: cannot compare different types (gsl_CONFIG_ALLOWS_NONSTRICT_SPAN_COMPARISON=0)" );
#endif
}

CASE( "string_span: Allows to compare with types convertible to string_span" )
{
#if gsl_CONFIG( ALLOWS_NONSTRICT_SPAN_COMPARISON )

    char const * phello   = "hello";
    char const   ahello[] = "hello";
    std::string  shello   = "hello";
    cstring_span        a = phello;

    EXPECT(     a == phello );
    EXPECT(     a == ahello );
    EXPECT(     a == shello );
    EXPECT(     a <= phello );
    EXPECT(     a <= ahello );
    EXPECT(     a <= shello );
    EXPECT(     a >= phello );
    EXPECT(     a >= ahello );
    EXPECT(     a >= shello );

    EXPECT_NOT( a != phello );
    EXPECT_NOT( a != ahello );
    EXPECT_NOT( a != shello );
    EXPECT_NOT( a  < phello );
    EXPECT_NOT( a  < ahello );
    EXPECT_NOT( a  < shello );
    EXPECT_NOT( a  > phello );
    EXPECT_NOT( a  > ahello );
    EXPECT_NOT( a  > shello );

#if gsl_HAVE( OWNER_TEMPLATE )
    EXPECT(     phello == a );
    EXPECT(     ahello == a );
    EXPECT(     shello == a );
    EXPECT(     phello <= a );
    EXPECT(     ahello <= a );
    EXPECT(     shello <= a );
    EXPECT(     phello >= a );
    EXPECT(     ahello >= a );
    EXPECT(     shello >= a );

    EXPECT_NOT( phello != a );
    EXPECT_NOT( ahello != a );
    EXPECT_NOT( shello != a );
    EXPECT_NOT( phello  < a );
    EXPECT_NOT( ahello  < a );
    EXPECT_NOT( shello  < a );
    EXPECT_NOT( phello  > a );
    EXPECT_NOT( ahello  > a );
    EXPECT_NOT( shello  > a );
#endif

#if gsl_HAVE( ARRAY )
    std::array<char,6> world = {{ 'w', 'o', 'r', 'l', 'd', '\0' }};
    cstring_span   b = world;

    EXPECT( b == world );
# if gsl_HAVE( OWNER_TEMPLATE )
    EXPECT( world == b );
# endif
#endif
#else
    EXPECT( !!"string_span: cannot compare different types (gsl_CONFIG_ALLOWS_NONSTRICT_SPAN_COMPARISON=0)" );
#endif
}

CASE( "string_span: Allows to test for empty span via empty(), empty case" )
{
    string_span sz;

    EXPECT( sz.empty() );
}

CASE( "string_span: Allows to test for empty span via empty(), non-empty case" )
{
    cstring_span s = "hello";

    EXPECT_NOT( s.empty() );
}

CASE( "string_span: Allows to obtain the number of elements via length()" )
{
    char a[] = "hello";
    char b[] = "world";
    string_span sz;
    string_span sa = a;
    string_span sb = b;

    EXPECT( sz.length() == index_type( 0 ) );
    EXPECT( sa.length() == index_type( strlen( a ) ) );
    EXPECT( sb.length() == index_type( strlen( b ) ) );
}

CASE( "string_span: Allows to obtain the number of elements via size()" )
{
    char a[] = "hello";
    char b[] = "world";
    string_span sz;
    string_span sa = a;
    string_span sb = b;

    EXPECT( sz.size() == index_type( 0 ) );
    EXPECT( sa.size() == index_type( strlen( a ) ) );
    EXPECT( sb.size() == index_type( strlen( b ) ) );
}

#if gsl_HAVE( WCHAR )

CASE( "string_span: Allows to obtain the number of bytes via length_bytes()" )
{
    wchar_t a[] = L"hello";
    wchar_t b[] = L"world";
    wstring_span sz;
    wstring_span sa = a;
    wstring_span sb = b;

    EXPECT( sz.length_bytes() == index_type( 0 ) );
    EXPECT( sa.length_bytes() == index_type( sizeof(wchar_t) * wcslen( a ) ) );
    EXPECT( sb.length_bytes() == index_type( sizeof(wchar_t) * wcslen( b ) ) );
}

CASE( "string_span: Allows to obtain the number of bytes via size_bytes()" )
{
    wchar_t a[] = L"hello";
    wchar_t b[] = L"world";
    wstring_span sz;
    wstring_span sa = a;
    wstring_span sb = b;

    EXPECT( sz.size_bytes() == index_type( 0 ) );
    EXPECT( sa.size_bytes() == index_type( sizeof(wchar_t) * wcslen( a ) ) );
    EXPECT( sb.size_bytes() == index_type( sizeof(wchar_t) * wcslen( b ) ) );
}

#endif // gsl_HAVE( WCHAR )

CASE( "string_span: Allows to view the elements as read-only bytes" )
{
    char const *    hello = "hello";
    cstring_span s( hello);

    span<const gsl::byte> b = as_bytes( s );

    EXPECT( b[0] == to_byte( hello[0] ) );
    EXPECT( b[1] == to_byte( hello[1] ) );
    EXPECT( b[2] == to_byte( hello[2] ) );
    EXPECT( b[3] == to_byte( hello[3] ) );
    EXPECT( b[4] == to_byte( hello[4] ) );
}

//------------------------------------------------------------------------
// zstring_span

CASE( "zstring_span: Allows to construct a zstring_span from a zero-terminated empty string (via span)" )
{
    char zero[] = "";

    zstring_span zsv( make_span( zero, 1 ) );

    EXPECT_NOT( zsv.empty() );
    EXPECT(     zsv.ensure_z().size() == 0u );
    EXPECT(     zsv.as_string_span().size() == 0u );
    EXPECT(     std::string( zsv.as_string_span().data() ) == "" );
}

CASE( "zstring_span: Allows to construct a zstring_span from a zero-terminated non-empty string (via span)" )
{
    char hello[] = "hello, world";
    const index_type len = std::string(hello).length();

    zstring_span zsv( make_span( hello, len + 1 ) );

    EXPECT_NOT( zsv.empty() );
    EXPECT(     zsv.ensure_z().size() == len );
    EXPECT(     zsv.as_string_span().size() == len );
    EXPECT(     std::string( zsv.as_string_span().data() ) == hello );
}

CASE( "zstring_span: Terminates construction of a zstring_span from a non-zero-terminated string (via span)" )
{
    struct F
    {
        static void blow()
        {
            char hello[] = "hello, worldxxx";
            const index_type len = std::string(hello).length();

            zstring_span zsv( make_span( hello, len ) );
        }
    };

    EXPECT_THROWS( F::blow() );
}

#if gsl_HAVE( WCHAR )

CASE( "zstring_span: Allows to construct a wzstring_span from a zero-terminated empty string (via span)" )
{
    wchar_t zero[] = L"";

    wzstring_span wzsv( make_span( zero, 1 ) );

    EXPECT_NOT( wzsv.empty() );
    EXPECT(     wzsv.ensure_z().size() == 0u );
    EXPECT(     wzsv.as_string_span().size() == 0u );
    EXPECT(     std::wstring( wzsv.as_string_span().data() ) == L"" );
}

CASE( "zstring_span: Allows to construct a wzstring_span from a zero-terminated non-empty string (via span)" )
{
    wchar_t hello[] = L"hello, world";
    const index_type len = std::wstring(hello).length();

    wzstring_span wzsv( make_span( hello, len + 1 ) );

    EXPECT_NOT( wzsv.empty() );
    EXPECT(     wzsv.ensure_z().size() == len );
    EXPECT(     wzsv.as_string_span().size() == len );
//  EXPECT(     std::wstring( wzsv.as_string_span().data() ) == hello );
}

CASE( "zstring_span: Terminates construction of a wzstring_span from a non-zero-terminated string (via span)" )
{
    struct F
    {
        static void blow()
        {
            wchar_t hello[] = L"hello, worldxxx";
            const index_type len = std::wstring(hello).length();

            wzstring_span wzsv( make_span( hello, len ) );
        }
    };

    EXPECT_THROWS( F::blow() );
}

#endif // gsl_HAVE( WCHAR )

CASE( "zstring_span: Allows to use a zstring_span with a legacy API via member assume_z()" )
{
    char hello[] = "hello, world";
    const std::string::size_type len = std::string(hello).length();

    zstring_span zsv( make_span( hello, len + 1 ) );
    czstring     czs( zsv.assume_z() );

    EXPECT( strlen( czs ) == len );
    EXPECT( *(czs + len ) == '\0');
}

#if gsl_HAVE( WCHAR )

CASE( "zstring_span: Allows to use a wzstring_span with a legacy API via member assume_z()" )
{
    wchar_t hello[] = L"hello, world";
    const index_type len = std::wstring(hello).length();

    wzstring_span wzsv( make_span( hello, len + 1 ) );
    cwzstring     cwzs( wzsv.assume_z() );

    EXPECT( wcslen( cwzs ) == len );
    EXPECT( *(cwzs + len ) == '\0');
}

#endif // gsl_HAVE( WCHAR )

//------------------------------------------------------------------------
// global functions:

CASE( "to_string(): Allows to explicitly convert from string_span to std::string" )
{
    char s[] = "hello";

    string_span sv( s, strlen( s ) );

    EXPECT( sv.length() == index_type( 5 ) );
    EXPECT( to_string( sv ) == s );
}

CASE( "to_string(): Allows to explicitly convert from cstring_span to std::string" )
{
    const char s[] = "hello";

    cstring_span sv( s, strlen( s ) );

    EXPECT( sv.length() == index_type( 5 ) );
    EXPECT( to_string( sv ) == s );
}

#if gsl_HAVE( WCHAR )

CASE( "to_string(): Allows to explicitly convert from wstring_span to std::wstring" )
{
    wchar_t s[] = L"hello";

    wstring_span sv( s, wcslen( s ) );

    std::wstring ws( to_string( wstring_span( s, wcslen( s ) ) ) );

    EXPECT( sv.length() == index_type( 5 ) );
    EXPECT( std::equal( ws.begin(), ws.end(), s ) );
}

CASE( "to_string(): Allows to explicitly convert from cwstring_span to std::wstring" )
{
    wchar_t s[] = L"hello";

    std::wstring ws( to_string( cwstring_span( s, wcslen( s ) ) ) );

    EXPECT( ws.length() == 5u );
    EXPECT( std::equal( ws.begin(), ws.end(), s ) );
}

#endif // gsl_HAVE( WCHAR )

CASE( "ensure_z(): Disallows to build a string_span from a const C-string" )
{
#if gsl_CONFIG( CONFIRMS_COMPILATION_ERRORS )
    const char s[] = "hello";

    string_span sv = ensure_z( s );

    EXPECT( sv.length() == 5u );
    EXPECT( std::string( sv.data() ) == s );
#endif
}

#if gsl_HAVE( WCHAR )
CASE( "ensure_z(): Disallows to build a wstring_span from a const wide C-string" )
{
#if gsl_CONFIG( CONFIRMS_COMPILATION_ERRORS )
    const wchar_t s[] = L"hello";

    wstring_span sv = ensure_z( s );

    EXPECT( sv.length() == 5 );
    EXPECT( std::wstring( sv.data() ) == std::wstring( s ) );
#endif
}
#endif // gsl_HAVE( WCHAR )

CASE( "ensure_z(): Allows to build a string_span from a non-const C-string" )
{
    char s[] = "hello";

#if gsl_COMPILER_MSVC_VERSION != 60
    string_span sv = ensure_z( s );
#else
    string_span sv = ensure_z( &s[0] );
#endif
    EXPECT( sv.length() == index_type( 5 ) );
    EXPECT( std::string( sv.data() ) == s );
}

CASE( "ensure_z(): Allows to build a cstring_span from a non-const C-string" )
{
    char s[] = "hello";

#if gsl_COMPILER_MSVC_VERSION != 60
    cstring_span sv = ensure_z( s );
#else
    cstring_span sv = ensure_z( &s[0] );
#endif

    EXPECT( sv.length() == index_type( 5 ) );
    EXPECT( std::string( sv.data() ) == s );
}

CASE( "ensure_z(): Allows to build a cstring_span from a const C-string" )
{
    const char s[] = "hello";

#if gsl_COMPILER_MSVC_VERSION != 60
    cstring_span sv = ensure_z( s );
#else
    cstring_span sv = ensure_z( &s[0] );
#endif

    EXPECT( sv.length() == index_type( 5 ) );
    EXPECT( std::string( sv.data() ) == s );
}

#if gsl_HAVE( WCHAR )

CASE( "ensure_z(): Allows to build a wstring_span from a non-const wide C-string" )
{
    wchar_t s[] = L"hello";

#if gsl_COMPILER_MSVC_VERSION != 60
    wstring_span sv = ensure_z( s );
#else
    wstring_span sv = ensure_z( &s[0] );
#endif

    EXPECT( sv.length() == index_type( 5 ) );
    EXPECT( std::wstring( sv.data() ) == std::wstring( s ) );
}

CASE( "ensure_z(): Allows to build a cwstring_span from a non-const wide C-string" )
{
    wchar_t s[] = L"hello";

#if gsl_COMPILER_MSVC_VERSION != 60
    cwstring_span sv = ensure_z( s );
#else
    cwstring_span sv = ensure_z( &s[0] );
#endif

    EXPECT( sv.length() == index_type( 5 ) );
    EXPECT( std::wstring( sv.data() ) == std::wstring( s ) );
}

CASE( "ensure_z(): Allows to build a cwstring_span from a const wide C-string" )
{
    const wchar_t s[] = L"hello";

#if gsl_COMPILER_MSVC_VERSION != 60
    cwstring_span sv = ensure_z( s );
#else
    cwstring_span sv = ensure_z( &s[0] );
#endif

    EXPECT( sv.length() == index_type( 5 ) );
    EXPECT( std::wstring( sv.data() ) == std::wstring( s ) );
}

#endif // gsl_HAVE( WCHAR )

CASE( "ensure_z(): Allows to specify ultimate location of the sentinel and ensure its presence" )
{
    const char * s = "hello"; // not: s[]

    EXPECT_THROWS( ensure_z( s, index_type( 3 ) ) );
}

CASE ( "operator<<: Allows printing a string_span to an output stream" )
{
    std::ostringstream oss;
    char s[] = "hello";
    string_span sv = ensure_z( s );

    oss << sv << '\n'
        << std::right << std::setw(10) << sv << '\n'
        << sv << '\n'
        << std::setfill('.') << std::left << std::setw(10) << sv;

    EXPECT( oss.str() == "hello\n     hello\nhello\nhello....." );
}

CASE ( "operator<<: Allows printing a cstring_span to an output stream" )
{
    std::ostringstream oss;
    char s[] = "hello";
    cstring_span sv = ensure_z( s );

    oss << sv << '\n'
        << std::right << std::setw(10) << sv << '\n'
        << sv << '\n'
        << std::setfill('.') << std::left << std::setw(10) << sv;

    EXPECT( oss.str() == "hello\n     hello\nhello\nhello....." );
}

#if gsl_HAVE( WCHAR )

CASE ( "operator<<: Allows printing a wstring_span to an output stream" )
{
    std::wostringstream oss;
    wchar_t s[] = L"hello";
    wstring_span sv = ensure_z( s );

    oss << sv << '\n'
        << std::right << std::setw(10) << sv << '\n'
        << sv << '\n'
        << std::setfill(L'.') << std::left << std::setw(10) << sv;

    EXPECT( oss.str() == L"hello\n     hello\nhello\nhello....." );
}

CASE ( "operator<<: Allows printing a cwstring_span to an output stream" )
{
    std::wostringstream oss;
    wchar_t s[] = L"hello";
    cwstring_span sv = ensure_z( s );

    oss << sv << '\n'
        << std::right << std::setw(10) << sv << '\n'
        << sv << '\n'
        << std::setfill(L'.') << std::left << std::setw(10) << sv;

    EXPECT( oss.str() == L"hello\n     hello\nhello\nhello....." );
}

#endif // gsl_HAVE( WCHAR )

#if ! gsl_HAVE( WCHAR )

CASE( "string_span: wstring_span and cwstring_span not available (wchar_t not available)" )
{
    EXPECT( true );
}
#endif // gsl_HAVE( WCHAR )

// end of file
