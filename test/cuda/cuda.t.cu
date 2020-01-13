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

#include "../gsl-lite.t.hpp"

#include <memory>


__global__ void preconditionAssertionKernel( int i, int j )
{
    gsl_Expects( i >= 0 );
    gsl_ExpectsAudit( i < j );
}

CASE( "CUDA: Precondition and postcondition assertions can be used in kernel code" )
{
    preconditionAssertionKernel<<<1, 1>>>( 0, 1 );
    // TODO: check for failure
}


__global__ void spanKernel( gsl::span< int > span )
{
    int* data = span.data();
    gsl_CONFIG_SPAN_INDEX_TYPE size = span.size();
    if (size > 0)
    {
        span[ 0 ] = 42;
        at( span, 0 ) = 42;
    }
    // TODO: add more tests
}

CASE( "CUDA: span<> can be used in kernel code" )
{
    spanKernel<<<1, 1>>>( gsl::span< int >( ) );
    // TODO: check for failure
    // TODO: add more tests
}


__global__ void notNullKernel( gsl::not_null< int* > ptr )
{
    // TODO: add more tests
}

CASE( "CUDA: not_null<> can be used in kernel code" )
{
    // TODO: run kernel
    // TODO: check for failure
    // TODO: add more tests
}
