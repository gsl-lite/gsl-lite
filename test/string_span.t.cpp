//
// gsl-lite is based on GSL: Guideline Support Library,
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

#include <string.h> // strlen()
#include <wchar.h>  // wcslen()

typedef string_span::index_type size_type;

CASE( "string_span: Disallows construction of a string_span from a cstring_span (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)" )
{
#if gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS
    cstring_span sv = "hello";

    string_span v2 = sv;
#endif
}

CASE( "string_span: Disallows construction of a string_span from a const std::string (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)" )
{
#if gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS
    const std::string s = "hello, world";

    string_span sv( s );

    EXPECT( sv.length() == size_type( 5 ) );
    EXPECT( std::string( sv.data() ) == s );
#endif
}

CASE( "string_span: Allows to default-construct" )
{
    string_span s;

    EXPECT( s.size() == size_type( 0 ) );
}

CASE( "string_span: Allows to construct from a nullptr (C++11)" )
{
#if gsl_HAVE_NULLPTR
    string_span  v( nullptr );
    cstring_span w( nullptr );

    EXPECT( v.size() == size_type( 0 ) );
    EXPECT( w.size() == size_type( 0 ) );
#else
    EXPECT( !!"nullptr is not available (no C++11)" );
#endif
}

CASE( "string_span: Allows to construct a cstring_span from a const C-string" )
{
    cstring_span sv = "hello";

    EXPECT( sv.length() == size_type( 5 ) );
    EXPECT( std::string( sv.data() ) ==  "hello" );
}

CASE( "string_span: Allows to construct a string_span from a non-const C-string and size" )
{
    char s[] = "hello";

    string_span sv( s, strlen( s ) );

    EXPECT( sv.length() == size_type( 5 ) );
    EXPECT( std::string( sv.data() ) == s );
}

CASE( "string_span: Allows to construct a string_span from a non-const C-string begin and end pointer" )
{
    char s[] = "hello";

    string_span sv( s, s + strlen( s ) );

    EXPECT( sv.length() == size_type( 5 ) );
    EXPECT( std::string( sv.data() ) == s );
}

CASE( "string_span: Allows to construct a string_span from a non-const C-array" )
{
    char s[] = { 'w', 'o', 'r', 'l', 'd', '\0' };

    string_span sv( s );

    EXPECT( sv.length() == size_type( 5 ) );
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
#if gsl_HAVE_ARRAY
    std::array<char,6> arr = {{ 'w', 'o', 'r', 'l', 'd', '\0' }};

    string_span sv( arr );

    EXPECT( sv.length() == size_type( 5 ) );
    EXPECT( std::equal( sv.begin(), sv.end(), arr.begin() ) );
#else
    EXPECT( !!"std::array<> is not available (no C++11)" );
#endif
}

CASE( "string_span: Allows to construct a string_span from a non-const container (std::vector)" )
{

#if gsl_HAVE_INITIALIZER_LIST
    std::vector<char> vec = { 'w', 'o', 'r', 'l', 'd', '\0' };
#else
    std::vector<char> vec( 6, 'w' );
#endif
#if gsl_HAVE_CONSTRAINED_SPAN_CONTAINER_CTOR || gsl_HAVE_UNCONSTRAINED_SPAN_CONTAINER_CTOR
    string_span sv( vec );

    EXPECT( sv.length() == size_type( 6 ) );
    EXPECT( std::equal( sv.begin(), sv.end(), vec.begin() ) );
#else
    EXPECT( !!"(un)constrained construction from container is not available" );
#endif
}

CASE( "string_span: Allows to construct a string_span from a non-const container, via a tag (std::vector)" )
{
# if gsl_HAVE_INITIALIZER_LIST
    std::vector<char> vec = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
#else
    std::vector<char> vec; {for ( int i = 1; i < 10; ++i ) vec.push_back(i); }
#endif
    string_span  sv( with_container, vec );

    EXPECT( sv.length() == size_type( 9 ) );
    EXPECT( std::equal( sv.begin(), sv.end(), vec.begin() ) );
}

CASE( "string_span: Allows to construct a cstring_span from a non-const C-string and size" )
{
    char s[] = "hello";

    cstring_span sv( s, strlen( s ) );

    EXPECT( sv.length() == size_type( 5 ) );
    EXPECT( std::string( sv.data() ) == s );
}

CASE( "string_span: Allows to construct a cstring_span from a non-const C-string begin and end pointer" )
{
    char s[] = "hello";

    cstring_span sv( s, s + strlen( s ) );

    EXPECT( sv.length() == size_type( 5 ) );
    EXPECT( std::string( sv.data() ) == s );
}

CASE( "string_span: Allows to construct a cstring_span from a non-const C-array" )
{
    char s[] = { 'w', 'o', 'r', 'l', 'd', '\0' };

    cstring_span sv( s );

#if gsl_HAVE_CONSTRAINED_SPAN_CONTAINER_CTOR || gsl_COMPILER_MSVC_VERSION
    EXPECT( sv.length() == size_type( 5 ) );
#else
    EXPECT( !!"Warning: Unconstrained span container constructor enabled: terminating '\0' included in span" );
    EXPECT( sv.length() == size_type( 6 ) );
#endif
    EXPECT( std::string( sv.data() ) == "world" );
}

CASE( "string_span: Allows to construct a cstring_span from a non-const std::string" )
{
#if gsl_HAVE_CONSTRAINED_SPAN_CONTAINER_CTOR || gsl_HAVE_UNCONSTRAINED_SPAN_CONTAINER_CTOR
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
#if gsl_HAVE_ARRAY
    std::array<char,6> arr = {{ 'w', 'o', 'r', 'l', 'd', '\0' }};

    cstring_span sv( arr );

    EXPECT( sv.length() == size_type( 5 ) );
    EXPECT( std::equal( sv.begin(), sv.end(), arr.begin() ) );
#else
    EXPECT( !!"std::array<> is not available (no C++11)" );
#endif
}

CASE( "string_span: Allows to construct a cstring_span from a non-const container (std::vector)" )
{
#if gsl_HAVE_INITIALIZER_LIST
    std::vector<char> vec = { 'w', 'o', 'r', 'l', 'd', '\0' };
#else
    std::vector<char> vec( 6, 'w' );
#endif
#if gsl_HAVE_CONSTRAINED_SPAN_CONTAINER_CTOR || gsl_HAVE_UNCONSTRAINED_SPAN_CONTAINER_CTOR
    cstring_span sv( vec );

    EXPECT( sv.length() == size_type( 6 ) );
    EXPECT( std::equal( sv.begin(), sv.end(), vec.begin() ) );
#else
    EXPECT( !!"(un)constrained construction from container is not available" );
#endif
}

CASE( "string_span: Allows to construct a cstring_span from a non-const container, via a tag (std::vector)" )
{
# if gsl_HAVE_INITIALIZER_LIST
    std::vector<char> vec = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
#else
    std::vector<char> vec; {for ( int i = 1; i < 10; ++i ) vec.push_back(i); }
#endif
    cstring_span sv( with_container, vec );

    EXPECT( sv.length() == size_type( 9 ) );
    EXPECT( std::equal( sv.begin(), sv.end(), vec.begin() ) );
}

CASE( "string_span: Allows to construct a cstring_span from a const C-string and size" )
{
    const char s[] = "hello";

    cstring_span sv( s, strlen( s ) );

    EXPECT( sv.length() == size_type( 5 ) );
    EXPECT( std::string( sv.data() ) == s );
}

CASE( "string_span: Allows to construct a cstring_span from a non-const C-string begin and end pointer" )
{
    const char s[] = "hello";

    cstring_span sv( s, s + strlen( s ) );

    EXPECT( sv.length() == size_type( 5 ) );
    EXPECT( std::string( sv.data() ) == s );
}

CASE( "string_span: Allows to construct a cstring_span from a const C-array" )
{
    const char s[] = { 'w', 'o', 'r', 'l', 'd', '\0' };

    cstring_span sv( s );

    EXPECT( sv.length() == size_type( 5 ) );
    EXPECT( std::string( sv.data() ) == "world" );
}

CASE( "string_span: Allows to construct a cstring_span from a const std::string" )
{
#if gsl_HAVE_CONSTRAINED_SPAN_CONTAINER_CTOR || gsl_HAVE_UNCONSTRAINED_SPAN_CONTAINER_CTOR
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
#if gsl_HAVE_ARRAY
    const std::array<char,6> arr = {{ 'w', 'o', 'r', 'l', 'd', '\0' }};

    cstring_span sv( arr );

    EXPECT( sv.length() == size_type( 5 ) );
    EXPECT( std::equal( sv.begin(), sv.end(), arr.begin() ) );
#else
    EXPECT( !!"std::array<> is not available (no C++11)" );
#endif
}

CASE( "string_span: Allows to construct a cstring_span from a const container (std::vector)" )
{
#if gsl_HAVE_INITIALIZER_LIST
    const std::vector<char> vec = { 'w', 'o', 'r', 'l', 'd', '\0' };
#else
    const std::vector<char> vec( 6, 'w' );
#endif
#if gsl_HAVE_CONSTRAINED_SPAN_CONTAINER_CTOR || gsl_HAVE_UNCONSTRAINED_SPAN_CONTAINER_CTOR
    cstring_span sv( vec );

    EXPECT( sv.length() == size_type( 6 ) );
    EXPECT( std::equal( sv.begin(), sv.end(), vec.begin() ) );
#else
    EXPECT( !!"(un)constrained construction from container is not available" );
#endif
}

CASE( "string_span: Allows to construct a cstring_span from a const container, via a tag (std::vector)" )
{
# if gsl_HAVE_INITIALIZER_LIST
    const std::vector<char> vec = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
#else
    std::vector<char> tmp; {for ( int i = 1; i < 10; ++i ) tmp.push_back(i); }
    const std::vector<char> vec( tmp );
#endif
    cstring_span sv( with_container, vec );

    EXPECT( sv.length() == size_type( 9 ) );
    EXPECT( std::equal( sv.begin(), sv.end(), vec.begin() ) );
}

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

    EXPECT( sv.length() == size_type( 5 ) );
    EXPECT( std::wstring( sv.data() ) == s );
}

CASE( "string_span: Allows to construct a wstring_span from a non-const C-array" )
{
    wchar_t s[] = { L'w', L'o', L'r', L'l', L'd', L'\0' };

    wstring_span sv( s );

    EXPECT( sv.length() == size_type( 5 ) );
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
#if gsl_HAVE_ARRAY
    std::array<wchar_t,6> arr = {{ 'w', 'o', 'r', 'l', 'd', '\0' }};

    wstring_span sv( arr );

    EXPECT( sv.length() == size_type( 5 ) );
    EXPECT( std::equal( sv.begin(), sv.end(), arr.begin() ) );
#else
    EXPECT( !!"std::array<> is not available (no C++11)" );
#endif
}

CASE( "string_span: Allows to construct a wstring_span from a non-const container (std::vector)" )
{
#if gsl_HAVE_INITIALIZER_LIST
    std::vector<wchar_t> vec = { L'w', L'o', L'r', L'l', L'd', L'\0' };
#else
    std::vector<wchar_t> vec( 6, 'w' );
#endif
#if gsl_HAVE_CONSTRAINED_SPAN_CONTAINER_CTOR || gsl_HAVE_UNCONSTRAINED_SPAN_CONTAINER_CTOR
    wstring_span sv( vec );

    EXPECT( sv.length() == size_type( 6 ) );
    EXPECT( std::equal( sv.begin(), sv.end(), vec.begin() ) );
#else
    EXPECT( !!"(un)constrained construction from container is not available" );
#endif
}

CASE( "string_span: Allows to construct a wstring_span from a non-const container, via a tag (std::vector)" )
{
# if gsl_HAVE_INITIALIZER_LIST
    std::vector<wchar_t> vec = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
#else
    std::vector<wchar_t> vec; {for ( int i = 1; i < 10; ++i ) vec.push_back(i); }
#endif
    wstring_span sv( with_container, vec );

    EXPECT( sv.length() == size_type( 9 ) );
    EXPECT( std::equal( sv.begin(), sv.end(), vec.begin() ) );
}

CASE( "string_span: Allows to construct a cwstring_span from a non-const C-string and size" )
{
    wchar_t s[] = L"hello";

    cwstring_span sv( s, wcslen( s ) );

    EXPECT( sv.length() == size_type( 5 ) );
    EXPECT( std::wstring( sv.data() ) == std::wstring( s ) );
}

CASE( "string_span: Allows to construct a cwstring_span from a non-const C-string begin and end pointer" )
{
    wchar_t s[] = L"hello";

    cwstring_span sv( s, s + wcslen( s ) );

    EXPECT( sv.length() == size_type( 5 ) );
    EXPECT( std::wstring( sv.data() ) == s );
}

CASE( "string_span: Allows to construct a cwstring_span from a non-const C-array" )
{
    wchar_t s[] = { L'w', L'o', L'r', L'l', L'd', L'\0' };

    cwstring_span sv( s );

#if gsl_HAVE_CONSTRAINED_SPAN_CONTAINER_CTOR || gsl_COMPILER_MSVC_VERSION
    EXPECT( sv.length() == size_type( 5 ) );
#else
    EXPECT( !!"Warning: Unconstrained span container constructor enabled: terminating '\0' included in span" );
    EXPECT( sv.length() == size_type( 6 ) );
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
#if gsl_HAVE_ARRAY
    std::array<wchar_t,6> arr = {{ 'w', 'o', 'r', 'l', 'd', '\0' }};

    cwstring_span sv( arr );

    EXPECT( sv.length() == size_type( 5 ) );
    EXPECT( std::equal( sv.begin(), sv.end(), arr.begin() ) );
#else
    EXPECT( !!"std::array<> is not available (no C++11)" );
#endif
}

CASE( "string_span: Allows to construct a cwstring_span from a non-const container (std::vector)" )
{
#if gsl_HAVE_INITIALIZER_LIST
    std::vector<wchar_t> vec = { L'w', L'o', L'r', L'l', L'd', L'\0' };
#else
    std::vector<wchar_t> vec( 6, 'w' );
#endif
#if gsl_HAVE_CONSTRAINED_SPAN_CONTAINER_CTOR || gsl_HAVE_UNCONSTRAINED_SPAN_CONTAINER_CTOR
    cwstring_span sv( vec );

    EXPECT( sv.length() == size_type( 6 ) );
    EXPECT( std::equal( sv.begin(), sv.end(), vec.begin() ) );
#else
    EXPECT( !!"(un)constrained construction from container is not available" );
#endif
}

CASE( "string_span: Allows to construct a cwstring_span from a non-const container, via a tag (std::vector)" )
{
# if gsl_HAVE_INITIALIZER_LIST
    std::vector<wchar_t> vec = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
#else
    std::vector<wchar_t> vec; {for ( int i = 1; i < 10; ++i ) vec.push_back(i); }
#endif
    cwstring_span sv( with_container, vec );

    EXPECT( sv.length() == size_type( 9 ) );
    EXPECT( std::equal( sv.begin(), sv.end(), vec.begin() ) );
}

CASE( "string_span: Allows to construct a cwstring_span from a const C-string and size" )
{
    const wchar_t s[] = L"hello";

    cwstring_span sv( s, wcslen( s ) );

    EXPECT( sv.length() == size_type( 5 ) );
    EXPECT( std::wstring( sv.data() ) == std::wstring( s ) );
}

CASE( "string_span: Allows to construct a cwstring_span from a const C-string begin and end pointer" )
{
    const wchar_t s[] = L"hello";

    cwstring_span sv( s, s + wcslen( s ) );

    EXPECT( sv.length() == size_type( 5 ) );
    EXPECT( std::wstring( sv.data() ) == s );
}

CASE( "string_span: Allows to construct a cwstring_span from a const C-array" )
{
    const wchar_t s[] = { L'w', L'o', L'r', L'l', L'd', L'\0' };

    cwstring_span sv( s );

    EXPECT( sv.length() == size_type( 5 ) );
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
#if gsl_HAVE_ARRAY
    const std::array<wchar_t,6> arr = {{ 'w', 'o', 'r', 'l', 'd', '\0' }};

    cwstring_span sv( arr );

    EXPECT( sv.length() == size_type( 5 ) );
    EXPECT( std::equal( sv.begin(), sv.end(), arr.begin() ) );
#else
    EXPECT( !!"std::array<> is not available (no C++11)" );
#endif
}

CASE( "string_span: Allows to construct a cwstring_span from a const container (std::vector)" )
{
#if gsl_HAVE_INITIALIZER_LIST
    const std::vector<wchar_t> vec = { L'w', L'o', L'r', L'l', L'd', L'\0' };
#else
    const std::vector<wchar_t> vec( 6, 'w' );
#endif
#if gsl_HAVE_CONSTRAINED_SPAN_CONTAINER_CTOR || gsl_HAVE_UNCONSTRAINED_SPAN_CONTAINER_CTOR
    cwstring_span sv( vec );

    EXPECT( sv.length() == size_type( 6 ) );
    EXPECT( std::equal( sv.begin(), sv.end(), vec.begin() ) );
#else
    EXPECT( !!"(un)constrained construction from container is not available" );
#endif
}

CASE( "string_span: Allows to construct a cwstring_span from a const container, via a tag (std::vector)" )
{
# if gsl_HAVE_INITIALIZER_LIST
    const std::vector<wchar_t> vec = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
#else
    std::vector<wchar_t> tmp; {for ( int i = 1; i < 10; ++i ) tmp.push_back(i); }
    const std::vector<wchar_t> vec( tmp );
#endif
    cwstring_span sv( with_container, vec );

    EXPECT( sv.length() == size_type( 9 ) );
    EXPECT( std::equal( sv.begin(), sv.end(), vec.begin() ) );
}

CASE( "string_span: Allows to copy-construct from another span of the same type" )
{
}

CASE( "string_span: Allows to copy-construct from another span of a compatible type" )
{
}

#if gsl_CPP11_OR_GREATER

CASE( "string_span: Allows to move-construct from another span of the same type (C++11)" )
{
}

#endif

CASE( "string_span: Allows to copy-assign from another span of the same type" )
{
}

#if gsl_CPP11_OR_GREATER

CASE( "string_span: Allows to move-assign from another span of the same type (C++11)" )
{
}

#endif

CASE( "string_span: Allows to create a sub span of the first n elements" )
{
}

CASE( "string_span: Allows to create a sub span of the last n elements" )
{
}

CASE( "string_span: Allows to create a sub span starting at a given offset" )
{
}

CASE( "string_span: Allows to create a sub span starting at a given offset with a given length" )
{
}

CASE( "string_span: Allows to create an empty sub span at full offset" )
{
}

CASE( "string_span: Allows to create an empty sub span at full offset with zero length" )
{
}

CASE( "string_span: Allows forward iteration" )
{
}

CASE( "string_span: Allows const forward iteration" )
{
}

CASE( "string_span: Allows reverse iteration" )
{
}

CASE( "string_span: Allows const reverse iteration" )
{
}

CASE( "string_span: Allows to observe an element via array indexing" )
{
}

CASE( "string_span: Allows to observe an element via call indexing" )
{
}

CASE( "string_span: Allows to observe an element via at()" )
{
}

CASE( "string_span: Allows to observe an element via data()" )
{
}

CASE( "string_span: Allows to change an element via array indexing" )
{
}

CASE( "string_span: Allows to change an element via call indexing" )
{
}

CASE( "string_span: Allows to change an element via at()" )
{
}

CASE( "string_span: Allows to change an element via data()" )
{
}

CASE( "string_span: Allows to compare a string_span with another string_span" )
{
    char s[] = "hello";
    char t[] = "world";

    string_span sv( s, strlen( s ) );
    string_span tv( t, strlen( s ) );

    EXPECT( sv.length() == size_type( 5 ) );
    EXPECT( tv.length() == size_type( 5 ) );

    EXPECT( sv == sv );
    EXPECT( sv != tv );
    EXPECT( sv <= sv );
    EXPECT( sv <= tv );
    EXPECT( sv <  tv );
    EXPECT( tv >= tv );
    EXPECT( tv >= sv );
    EXPECT( tv >  sv );
}

CASE( "string_span: Allows to compare empty spans as equal" )
{
}

CASE( "string_span: Allows to compare a string_span with a cstring_span" )
{
#if gsl_CONFIG_ALLOWS_NONSTRICT_SPAN_COMPARISON
    char s[] = "hello";

    string_span   sv( s, strlen( s ) );
    cstring_span csv( s, strlen( s ) );

    EXPECT( sv == csv );
#else
    EXPECT( !!"string_span<>: cannot compare different types (gsl_CONFIG_ALLOWS_NONSTRICT_SPAN_COMPARISON=0)" );
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

    EXPECT( sz.length() == size_type( 0 ) );
    EXPECT( sa.length() == size_type( strlen( a ) ) );
    EXPECT( sb.length() == size_type( strlen( b ) ) );
}

CASE( "string_span: Allows to obtain the number of elements via size()" )
{
    char a[] = "hello";
    char b[] = "world";
    string_span sz;
    string_span sa = a;
    string_span sb = b;

    EXPECT( sz.size() == size_type( 0 ) );
    EXPECT( sa.size() == size_type( strlen( a ) ) );
    EXPECT( sb.size() == size_type( strlen( b ) ) );
}

CASE( "string_span: Allows to obtain the number of bytes via length_bytes()" )
{
    wchar_t a[] = L"hello";
    wchar_t b[] = L"world";
    wstring_span sz;
    wstring_span sa = a;
    wstring_span sb = b;

    EXPECT( sz.length_bytes() == size_type( 0 ) );
    EXPECT( sa.length_bytes() == size_type( sizeof(wchar_t) * wcslen( a ) ) );
    EXPECT( sb.length_bytes() == size_type( sizeof(wchar_t) * wcslen( b ) ) );
}

CASE( "string_span: Allows to obtain the number of bytes via size_bytes()" )
{
    wchar_t a[] = L"hello";
    wchar_t b[] = L"world";
    wstring_span sz;
    wstring_span sa = a;
    wstring_span sb = b;

    EXPECT( sz.size_bytes() == size_type( 0 ) );
    EXPECT( sa.size_bytes() == size_type( sizeof(wchar_t) * wcslen( a ) ) );
    EXPECT( sb.size_bytes() == size_type( sizeof(wchar_t) * wcslen( b ) ) );
}

CASE( "to_string(): Allows to explicitly convert from string_span to std::string" )
{
    char s[] = "hello";

    string_span sv( s, strlen( s ) );

    EXPECT( sv.length() == size_type( 5 ) );
    EXPECT( to_string( sv ) == s );
}

CASE( "to_string(): Allows to explicitly convert from cstring_span to std::string" )
{
    const char s[] = "hello";

    cstring_span sv( s, strlen( s ) );

    EXPECT( sv.length() == size_type( 5 ) );
    EXPECT( to_string( sv ) == s );
}

CASE( "to_string(): Allows to explicitly convert from wstring_span to std::wstring" )
{
    wchar_t s[] = L"hello";

    wstring_span sv( s, wcslen( s ) );

    std::wstring ws( to_string( wstring_span( s, wcslen( s ) ) ) );

    EXPECT( sv.length() == size_type( 5 ) );
    EXPECT( std::equal( ws.begin(), ws.end(), s ) );
}

CASE( "to_string(): Allows to explicitly convert from cwstring_span to std::wstring" )
{
    wchar_t s[] = L"hello";

    std::wstring ws( to_string( cwstring_span( s, wcslen( s ) ) ) );

    EXPECT( ws.length() == size_type( 5 ) );
    EXPECT( std::equal( ws.begin(), ws.end(), s ) );
}

CASE( "ensure_z(): Disallows to build a string_span from a const C-string" )
{
#if gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS
    const char s[] = "hello";

    string_span sv = ensure_z( s );

    EXPECT( sv.length() == 5 );
    EXPECT( std::string( sv.data() ) == s );
#endif
}

CASE( "ensure_z(): Disallows to build a wstring_span from a const wide C-string" )
{
#if gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS
    const wchar_t s[] = L"hello";

    wstring_span sv = ensure_z( s );

    EXPECT( sv.length() == 5 );
    EXPECT( std::wstring( sv.data() ) == std::wstring( s ) );
#endif
}

CASE( "ensure_z(): Allows to build a string_span from a non-const C-string" )
{
    char s[] = "hello";

#if gsl_COMPILER_MSVC_VERSION != 6
    string_span sv = ensure_z( s );
#else
    string_span sv = ensure_z( &s[0] );
#endif
    EXPECT( sv.length() == size_type( 5 ) );
    EXPECT( std::string( sv.data() ) == s );
}

CASE( "ensure_z(): Allows to build a cstring_span from a non-const C-string" )
{
    char s[] = "hello";

#if gsl_COMPILER_MSVC_VERSION != 6
    cstring_span sv = ensure_z( s );
#else
    cstring_span sv = ensure_z( &s[0] );
#endif

    EXPECT( sv.length() == size_type( 5 ) );
    EXPECT( std::string( sv.data() ) == s );
}

CASE( "ensure_z(): Allows to build a cstring_span from a const C-string" )
{
    const char s[] = "hello";

#if gsl_COMPILER_MSVC_VERSION != 6
    cstring_span sv = ensure_z( s );
#else
    cstring_span sv = ensure_z( &s[0] );
#endif

    EXPECT( sv.length() == size_type( 5 ) );
    EXPECT( std::string( sv.data() ) == s );
}

CASE( "ensure_z(): Allows to build a wstring_span from a non-const wide C-string" )
{
    wchar_t s[] = L"hello";

#if gsl_COMPILER_MSVC_VERSION != 6
    wstring_span sv = ensure_z( s );
#else
    wstring_span sv = ensure_z( &s[0] );
#endif

    EXPECT( sv.length() == size_type( 5 ) );
    EXPECT( std::wstring( sv.data() ) == std::wstring( s ) );
}

CASE( "ensure_z(): Allows to build a cwstring_span from a non-const wide C-string" )
{
    wchar_t s[] = L"hello";

#if gsl_COMPILER_MSVC_VERSION != 6
    cwstring_span sv = ensure_z( s );
#else
    cwstring_span sv = ensure_z( &s[0] );
#endif

    EXPECT( sv.length() == size_type( 5 ) );
    EXPECT( std::wstring( sv.data() ) == std::wstring( s ) );
}

CASE( "ensure_z(): Allows to build a cwstring_span from a const wide C-string" )
{
    const wchar_t s[] = L"hello";

#if gsl_COMPILER_MSVC_VERSION != 6
    cwstring_span sv = ensure_z( s );
#else
    cwstring_span sv = ensure_z( &s[0] );
#endif

    EXPECT( sv.length() == size_type( 5 ) );
    EXPECT( std::wstring( sv.data() ) == std::wstring( s ) );
}

CASE( "ensure_z(): Allows to specify ultimate location of the sentinel and ensure its presence" )
{
    const char * s = "hello"; // not: s[]

    EXPECT_THROWS( ensure_z( s, size_type( 3 ) ) );
}

// end of file
