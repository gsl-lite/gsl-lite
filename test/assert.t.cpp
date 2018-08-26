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

namespace {

bool expects( bool x ) { Expects( x ); return x; } 
bool ensures( bool x ) { Ensures( x ); return x; }
    
}

CASE( "Expects(): Allows a true expression" )
{
    EXPECT( expects( true  ) );
}

CASE( "Ensures(): Allows a true expression" )
{
    EXPECT( ensures( true  ) );
}

CASE( "Expects(): Terminates on a false expression" )
{
    EXPECT_THROWS( expects( false ) );
}

CASE( "Ensures(): Terminates on a false expression" )
{
    EXPECT_THROWS( ensures( false ) );
}

// end of file
