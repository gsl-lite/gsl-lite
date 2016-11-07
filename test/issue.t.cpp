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

CASE( "span<>: free comparation functions fail for different const-ness" "[.issue #32]" )
{
#if gsl_CONFIG_ALLOWS_NONSTRICT_SPAN_COMPARISON
    char data[] = { 'a', 'b' };
    string_span  a = as_span( data );
    cstring_span b = as_span( data ).last( 1 );

    // worked as expected before 0.9.0, in 0.9.[0,1] converts to bool and compares equal!

    EXPECT( a != b );
#else
    EXPECT( !!"span<>: cannot compare different types (gsl_CONFIG_ALLOWS_NONSTRICT_SPAN_COMPARISON=0)" );
#endif
}

CASE( "byte: aliasing rules lead to undefined behaviour when using enum class" "[.issue #34](GSL #313)" )
{
    struct F {
        static int f( int & i, gsl::byte & r )
        {
           i = 7;
           r <<= 1;
           return i;
        }
    };

   int i;
   EXPECT( 14 == F::f( i, reinterpret_cast<gsl::byte&>( i ) ) );
}

CASE( "string_span<>: must not include terminating '\\0'" "[.issue #53]" )
{
    char const data[] = "ab";
    char const * text = "ab";
    cstring_span span = "ab";

    cstring_span a( data );
    cstring_span b( ensure_z( text ) );

    // may include terminating '\0'
    // works differently from M-GSL and basic_string_span proposal:

    EXPECT( span.length() == 2 );
    EXPECT(    a.length() == 2 );
    EXPECT(    b.length() == 2 );

    EXPECT( a == span );
    EXPECT( a == b );
}

// end of file
