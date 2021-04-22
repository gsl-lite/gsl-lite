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
#include <sstream>


// CUDA error checking macro borrowed from https://stackoverflow.com/a/14038590 and adapted.
#define myCudaErrchk(ans) ( myCudaErrchkImpl( (ans), #ans, __FILE__, __LINE__ ) )
inline void myCudaErrchkImpl( cudaError_t code, char const * expr, char const * file, int line )
{
    if ( code != cudaSuccess )
    {
        std::ostringstream sstr;
        sstr << "CUDA error in '" << expr << "' at " << file << "(" << line << "): "
             << cudaGetErrorString( code );
        throw std::runtime_error( sstr.str() );
   }
}


struct CudaMallocDeleter
{
    template< class T >
    void operator ()( T* data ) const
    {
        cudaFree( data );
    }
};

template< class T >
using CudaUniquePtr = std::unique_ptr<T, CudaMallocDeleter>;

template< class T >
struct CudaAllocHelper
{
    static CudaUniquePtr<T> cudaAllocUnique()
    {
        T* ptr;
        auto ec = cudaMalloc( &ptr, sizeof( T ) );
        if ( ec != cudaSuccess ) throw std::bad_alloc{ };
        return CudaUniquePtr<T>( ptr );
    }
};
template< class T >
struct CudaAllocHelper<T[]>
{
    static CudaUniquePtr<T[]> cudaAllocUnique( std::size_t n )
    {
        T* ptr;
        auto ec = cudaMalloc( &ptr, sizeof( T ) * n );
        if ( ec != cudaSuccess ) throw std::bad_alloc{ };
        return CudaUniquePtr<T[]>( ptr );
    }
};
template< class T >
CudaUniquePtr<T> cudaAllocUnique()
{
    return CudaAllocHelper<T>::cudaAllocUnique();
}
template< class T >
CudaUniquePtr<T> cudaAllocUnique( std::size_t n )
{
    return CudaAllocHelper<T>::cudaAllocUnique(n);
}


__global__ void preconditionAssertionKernel( int i, int j )
{
    gsl_Expects( i >= 0 );
    gsl_ExpectsAudit( i < j );
    gsl_Ensures( i >= 0 );
    gsl_EnsuresAudit( i < j );
    gsl_Assert( i >= 0 );
    gsl_AssertAudit( i < j );
}

CASE( "CUDA: Precondition/postcondition checks and assertions can be used in kernel code" )
{
    preconditionAssertionKernel<<<1, 1>>>( 0, 1 );
    myCudaErrchk( cudaPeekAtLastError() );  // check for invalid launch arguments
    myCudaErrchk( cudaDeviceSynchronize() );  // check for execution errors
}


__global__ void spanKernel( gsl::span< int > span )
{
    int* data = span.data();
    gsl_CONFIG_SPAN_INDEX_TYPE size = span.size();
    if (size > 0)
    {
        span[0] = 42;
        at( span, 0 ) = 42;
    }
}

CASE( "CUDA: span<> can be passed to kernel code" )
{
    spanKernel<<<1, 1>>>( gsl::span< int >( ) );
    myCudaErrchk( cudaPeekAtLastError() );  // check for invalid launch arguments
    myCudaErrchk( cudaDeviceSynchronize() );  // check for execution errors
}

CASE( "CUDA: span<> can be used in kernel code" )
{
    std::size_t n = 3;
    auto array = cudaAllocUnique<int[]>(n);
    auto span = gsl::make_span(array.get(), n);
    spanKernel<<<1, 1>>>( span );
    myCudaErrchk( cudaPeekAtLastError() );  // check for invalid launch arguments
    myCudaErrchk( cudaDeviceSynchronize() );  // check for execution errors
}


__global__ void notNullRawKernel( gsl::not_null< int* > ptr )
{
    *ptr = 1;
    auto ptr2 = ptr;
    *ptr2 = 2;
    ptr = ptr2;
}

// Not supported yet because `std::unique_ptr<>` member functions are neither `constexpr` nor `__host__ __device__`.
//__global__ void notNullUniqueKernel( gsl::not_null< CudaUniquePtr< int > > ptr )
//{
//    *ptr = 3;
//    auto ptr2 = std::move( ptr );
//    *ptr2 = 4;
//    ptr = std::move( ptr2 );
//}

CASE( "CUDA: not_null<> can be passed to and used in kernel code" )
{
    auto pi = cudaAllocUnique<int>();

    notNullRawKernel<<<1, 1>>>( gsl::make_not_null( pi.get() ) );
    myCudaErrchk( cudaPeekAtLastError() );  // check for invalid launch arguments
    myCudaErrchk( cudaDeviceSynchronize() );  // check for execution errors

    //notNullUniqueKernel<<<1, 1>>>( gsl::make_not_null( std::move( pi ) ) );
    //myCudaErrchk( cudaPeekAtLastError() );  // check for invalid launch arguments
    //myCudaErrchk( cudaDeviceSynchronize() );  // check for execution errors
}


__global__ void failFastKernel()
{
    gsl_FailFast();
}

CASE( "CUDA: gsl_FailFast() can be used in kernel code" )
{
    failFastKernel<<<1, 1>>>();
    myCudaErrchk( cudaPeekAtLastError() );
    EXPECT( cudaDeviceSynchronize() != cudaSuccess );
    // Note that executing a trap instruction or an assertion in a CUDA kernel destroys the context, so this should be
    // the last test case we execute.
}

