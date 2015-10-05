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

CASE( "string_view<>: Disallows construction of a string_view from a const C-string and size (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)" )
{
#if gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS
    char s[] = "hello";
    cstring_view sv = s;
    
    string_view v2 = sv;
    string_view v3 = "Hello";
#endif
}

CASE( "string_view<>: ToDo: Disallows construction of a string_view from a const std::string (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)" )
{
#if gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS
    const std::string s = "hello, world";

    string_view sv( s );
    
    EXPECT( std::string( sv.data() ) == s );
#endif
}

CASE( "string_view<>: Allows to create a string_view from a non-const C-string and size" )
{
    char s[] = "hello";

    string_view sv( s, gsl_DIMENSION_OF( s ) - 1 );
    
    EXPECT( std::string( sv.data() ) == s );
}

CASE( "string_view<>: Allows to create a string_view from a non-const C-array" )
{
    char s[] = { 'w', 'o', 'r', 'l', 'd', '\0' };

    string_view sv( s );
    
    EXPECT( std::string( sv.data() ) == "world" );
}

CASE( "string_view<>: Allows to create a string_view from a non-const std::array" )
{
#if gsl_HAVE_ARRAY
    std::array<char,6> arr = {{ 'w', 'o', 'r', 'l', 'd', '\0' }};

    string_view sv( arr );
    
    EXPECT( std::equal( sv.begin(), sv.end(), arr.begin() ) );
#else
    EXPECT( !!"std::array<> is not available (no C++11)" );
#endif
}

CASE( "string_view<>: Allows to create a string_view from a non-const std::vector" )
{
#if gsl_COMPILER_MSVC_VERSION != 6
#if gsl_HAVE_INITIALIZER_LIST
    std::vector<char> vec = { 'w', 'o', 'r', 'l', 'd', '\0' };
#else
    std::vector<char> vec( 'w', 5 );
#endif
    string_view sv( vec );
    
    EXPECT( std::equal( sv.begin(), sv.end(), vec.begin() ) );
#else
    EXPECT( !!"not available for VC6" );
#endif
}

CASE( "string_view<>: ToDo: Allows to create a string_view from a non-const std::string" )
{
#if 0
#if gsl_COMPILER_MSVC_VERSION != 6
    std::string s = "hello, world";

    string_view sv( s );
    
    EXPECT( std::string( sv.data() ) == s );
#else
    EXPECT( !!"Not available for MSVC6" );
#endif
#endif
}

CASE( "string_view<>: Allows to create a cstring_view from a const C-string and size" )
{
    const char s[] = "hello";

    cstring_view sv( s, gsl_DIMENSION_OF( s ) - 1 );
    
    EXPECT( std::string( sv.data() ) == s );
}

CASE( "string_view<>: Allows to create a cstring_view from a const C-array" )
{
    const char s[] = { 'w', 'o', 'r', 'l', 'd', '\0' };

    cstring_view sv( s );
    
    EXPECT( std::string( sv.data() ) == "world" );
}

CASE( "string_view<>: Allows to create a cstring_view from a const std::array" )
{
#if gsl_HAVE_ARRAY
    const std::array<char,6> arr = {{ 'w', 'o', 'r', 'l', 'd', '\0' }};

    cstring_view sv( arr );
    
    EXPECT( std::equal( sv.begin(), sv.end(), arr.begin() ) );
#else
    EXPECT( !!"std::array<> is not available (no C++11)" );
#endif
}

CASE( "string_view<>: Allows to create a cstring_view from a const std::vector" )
{
#if gsl_COMPILER_MSVC_VERSION != 6
#if gsl_HAVE_INITIALIZER_LIST
    const std::vector<char> vec = { 'w', 'o', 'r', 'l', 'd', '\0' };
#else
    const std::vector<char> vec( 'w', 5 );
#endif
    cstring_view sv( vec );
    
    EXPECT( std::equal( sv.begin(), sv.end(), vec.begin() ) );
#else
    EXPECT( !!"Not available for MSVC6" );
#endif
}

CASE( "string_view<>: Allows to create a cstring_view from a const std::string" )
{
#if gsl_COMPILER_MSVC_VERSION != 6
    const std::string s = "hello, world";

    cstring_view sv( s );
    
    EXPECT( std::string( sv.data() ) == s );
#else
    EXPECT( !!"Not available for MSVC6" );
#endif
}

CASE( "string_view<>: Allows to create a cstring_view from a non-const C-string and size" )
{
    char s[] = "hello";

    cstring_view sv( s, gsl_DIMENSION_OF( s ) - 1 );
    
    EXPECT( std::string( sv.data() ) == s );
}

CASE( "string_view<>: Allows to create a cstring_view from a non-const C-array" )
{
    char s[] = { 'w', 'o', 'r', 'l', 'd', '\0' };

    cstring_view sv( s );
    
    EXPECT( std::string( sv.data() ) == "world" );
}

CASE( "string_view<>: Allows to create a cstring_view from a non-const std::array" )
{
#if gsl_HAVE_ARRAY
    std::array<char,6> arr = {{ 'w', 'o', 'r', 'l', 'd', '\0' }};

    cstring_view sv( arr );
    
    EXPECT( std::equal( sv.begin(), sv.end(), arr.begin() ) );
#else
    EXPECT( !!"std::array<> is not available (no C++11)" );
#endif
}

CASE( "string_view<>: Allows to create a cstring_view from a non-const std::vector" )
{
#if gsl_COMPILER_MSVC_VERSION != 6
#if gsl_HAVE_INITIALIZER_LIST
    std::vector<char> vec = { 'w', 'o', 'r', 'l', 'd', '\0' };
#else
    std::vector<char> vec( 'w', 5 );
#endif
    cstring_view sv( vec );
    
    EXPECT( std::equal( sv.begin(), sv.end(), vec.begin() ) );
#else
    EXPECT( !!"Not available for MSVC6" );
#endif
}

CASE( "string_view<>: Allows to create a cstring_view from a non-const std::string" )
{
#if gsl_COMPILER_MSVC_VERSION != 6
    std::string s = "hello, world";

    cstring_view sv( s );
    
    EXPECT( std::string( sv.data() ) == s );
#else
    EXPECT( !!"Not available for MSVC6" );
#endif
}

CASE( "string_view<>: Allows to create a wstring_view from a non-const C-string and size" )
{
    wchar_t s[] = L"hello";

    wstring_view sv( s, gsl_DIMENSION_OF( s ) - 1 );

    EXPECT( std::wstring( sv.data() ) == std::wstring( s ) );
}

CASE( "string_view<>: Allows to create a wstring_view from a non-const C-array" )
{
    wchar_t s[] = { L'w', L'o', L'r', L'l', L'd', L'\0' };

    wstring_view sv( s );
    
    EXPECT( std::wstring( sv.data() ) == std::wstring( s ) );
}

CASE( "string_view<>: Allows to create a wstring_view from a non-const std::array" )
{
#if gsl_HAVE_ARRAY
    std::array<wchar_t,6> arr = {{ 'w', 'o', 'r', 'l', 'd', '\0' }};

    wstring_view sv( arr );
    
    EXPECT( std::equal( sv.begin(), sv.end(), arr.begin() ) );
#else
    EXPECT( !!"std::array<> is not available (no C++11)" );
#endif
}

CASE( "string_view<>: Allows to create a wstring_view from a non-const std::vector" )
{
#if gsl_COMPILER_MSVC_VERSION != 6
#if gsl_HAVE_INITIALIZER_LIST
    std::vector<wchar_t> vec = { L'w', L'o', L'r', L'l', L'd', L'\0' };
#else
    std::vector<wchar_t> vec( 'w', 5 );
#endif
    wstring_view sv( vec );
    
    EXPECT( std::equal( sv.begin(), sv.end(), vec.begin() ) );
#else
    EXPECT( !!"not available for VC6" );
#endif
}

CASE( "string_view<>: Allows to create a cwstring_view from a non-const C-string and size" )
{
    wchar_t s[] = L"hello";

    cwstring_view sv( s, gsl_DIMENSION_OF( s ) - 1 );
    
    EXPECT( std::wstring( sv.data() ) == std::wstring( s ) );
}

CASE( "string_view<>: Allows to create a cwstring_view from a non-const C-array" )
{
    wchar_t s[] = { L'w', L'o', L'r', L'l', L'd', L'\0' };

    cwstring_view sv( s );
    
    EXPECT( std::wstring( sv.data() ) == std::wstring( s ) );
}

CASE( "string_view<>: Allows to create a cwstring_view from a non-const std::array" )
{
#if gsl_HAVE_ARRAY
    std::array<wchar_t,6> arr = {{ 'w', 'o', 'r', 'l', 'd', '\0' }};

    cwstring_view sv( arr );
    
    EXPECT( std::equal( sv.begin(), sv.end(), arr.begin() ) );
#else
    EXPECT( !!"std::array<> is not available (no C++11)" );
#endif
}

CASE( "string_view<>: Allows to create a cwstring_view from a non-const std::vector" )
{
#if gsl_COMPILER_MSVC_VERSION != 6
#if gsl_HAVE_INITIALIZER_LIST
    std::vector<wchar_t> vec = { L'w', L'o', L'r', L'l', L'd', L'\0' };
#else
    std::vector<wchar_t> vec( 'w', 5 );
#endif
    cwstring_view sv( vec );
    
    EXPECT( std::equal( sv.begin(), sv.end(), vec.begin() ) );
#else
    EXPECT( !!"not available for VC6" );
#endif
}

CASE( "string_view<>: Allows to create a cwstring_view from a const C-string and size" )
{
    const wchar_t s[] = L"hello";

    cwstring_view sv( s, gsl_DIMENSION_OF( s ) - 1 );
    
    EXPECT( std::wstring( sv.data() ) == std::wstring( s ) );
}

CASE( "string_view<>: Allows to create a cwstring_view from a const C-array" )
{
    const wchar_t s[] = { L'w', L'o', L'r', L'l', L'd', L'\0' };

    cwstring_view sv( s );
    
    EXPECT( std::wstring( sv.data() ) == std::wstring( s ) );
}

CASE( "string_view<>: Allows to create a cwstring_view from a const std::array" )
{
#if gsl_HAVE_ARRAY
    const std::array<wchar_t,6> arr = {{ 'w', 'o', 'r', 'l', 'd', '\0' }};

    cwstring_view sv( arr );
    
    EXPECT( std::equal( sv.begin(), sv.end(), arr.begin() ) );
#else
    EXPECT( !!"std::array<> is not available (no C++11)" );
#endif
}

CASE( "string_view<>: Allows to create a cwstring_view from a const std::vector" )
{
#if gsl_COMPILER_MSVC_VERSION != 6
#if gsl_HAVE_INITIALIZER_LIST
    const std::vector<wchar_t> vec = { L'w', L'o', L'r', L'l', L'd', L'\0' };
#else
    const std::vector<wchar_t> vec( 'w', 5 );
#endif
    cwstring_view sv( vec );
    
    EXPECT( std::equal( sv.begin(), sv.end(), vec.begin() ) );
#else
    EXPECT( !!"not available for VC6" );
#endif
}

CASE( "string_view<>: Allows to explicitly convert from string_view to std::string" )
{
    char s[] = "hello";

    string_view sv( s, gsl_DIMENSION_OF( s ) - 1 );
    
    EXPECT( to_string( sv ) == s );
}

CASE( "string_view<>: Allows to explicitly convert from cstring_view to std::string" )
{
    const char s[] = "hello";

    cstring_view sv( s, gsl_DIMENSION_OF( s ) - 1 );
    
    EXPECT( to_string( sv ) == s );
}

CASE( "string_view<>: Allows to explicitly convert from wstring_view to std::wstring" )
{
    wchar_t s[] = L"hello";

    wstring_view sv( s, gsl_DIMENSION_OF( s ) - 1 );
    
    std::wstring ws( to_string( wstring_view( s, gsl_DIMENSION_OF( s ) - 1 ) ) );
    
    EXPECT( std::equal( ws.begin(), ws.end(), s ) );
}

CASE( "string_view<>: Allows to explicitly convert from cwstring_view to std::wstring" )
{
    wchar_t s[] = L"hello";

    std::wstring ws( to_string( cwstring_view( s, gsl_DIMENSION_OF( s ) - 1 ) ) );
    
    EXPECT( std::equal( ws.begin(), ws.end(), s ) );
}

CASE( "ensure_z(): Disallows to build a string_view from a const C-string" )
{
#if gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS
    const char s[] = "hello";
    
    string_view sv = ensure_z( s );
    
    EXPECT( sv.length() == 5 );
    EXPECT( std::string( sv.data() ) == s );
#endif
}

CASE( "ensure_z(): Disallows to build a wstring_view from a const wide C-string" )
{
#if gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS
    const wchar_t s[] = L"hello";
    
    wstring_view sv = ensure_z( s );
    
    EXPECT( sv.length() == 5 );
    EXPECT( std::wstring( sv.data() ) == std::wstring( s ) );
#endif
}

CASE( "ensure_z(): Allows to build a string_view from a non-const C-string" )
{
    char s[] = "hello";
    
#if gsl_COMPILER_MSVC_VERSION != 6
    string_view sv = ensure_z( s );
#else 
    string_view sv = ensure_z( &s[0] );
#endif   
    EXPECT( sv.length() == 5 );
    EXPECT( std::string( sv.data() ) == s );
}

CASE( "ensure_z(): Allows to build a cstring_view from a non-const C-string" )
{
    char s[] = "hello";
    
#if gsl_COMPILER_MSVC_VERSION != 6
    cstring_view sv = ensure_z( s );
#else
    cstring_view sv = ensure_z( &s[0] );
#endif
    
    EXPECT( sv.length() == 5 );
    EXPECT( std::string( sv.data() ) == s );
}

CASE( "ensure_z(): Allows to build a cstring_view from a const C-string" )
{
    const char s[] = "hello";
    
#if gsl_COMPILER_MSVC_VERSION != 6
    cstring_view sv = ensure_z( s );
#else
    cstring_view sv = ensure_z( &s[0] );
#endif
    
    EXPECT( sv.length() == 5 );
    EXPECT( std::string( sv.data() ) == s );
}

CASE( "ensure_z(): Allows to build a wstring_view from a non-const wide C-string" )
{
    wchar_t s[] = L"hello";
    
#if gsl_COMPILER_MSVC_VERSION != 6
    wstring_view sv = ensure_z( s );
#else
    wstring_view sv = ensure_z( &s[0] );
#endif
    
    EXPECT( sv.length() == 5 );
    EXPECT( std::wstring( sv.data() ) == std::wstring( s ) );
}

CASE( "ensure_z(): Allows to build a cwstring_view from a non-const wide C-string" )
{
    wchar_t s[] = L"hello";
    
#if gsl_COMPILER_MSVC_VERSION != 6
    cwstring_view sv = ensure_z( s );
#else
    cwstring_view sv = ensure_z( &s[0] );
#endif
    
    EXPECT( sv.length() == 5 );
    EXPECT( std::wstring( sv.data() ) == std::wstring( s ) );
}

CASE( "ensure_z(): Allows to build a cwstring_view from a const wide C-string" )
{
    const wchar_t s[] = L"hello";
    
#if gsl_COMPILER_MSVC_VERSION != 6
    cwstring_view sv = ensure_z( s );
#else
    cwstring_view sv = ensure_z( &s[0] );
#endif
    
    EXPECT( sv.length() == 5 );
    EXPECT( std::wstring( sv.data() ) == std::wstring( s ) );
}

CASE( "ensure_z(): Allows to specify ultimate location of the sentinel and ensure its presence" )
{
    const char * s = "hello"; // not: s[]
    
    EXPECT_THROWS( ensure_z( s, 3 ) );
}

}

// end of file
