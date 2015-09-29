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
#include <vector>

namespace {

struct MyBase {};
struct MyDerived : public MyBase {};
struct Unrelated {};

// Stand-in for a user-defined ref-counted class.
template<typename T>
struct RefCounted
{
    RefCounted( T * p ) : p_( p ) {}
    operator T *() { return p_; }
    T * p_;
};

CASE( "not_null<>: Disallows default construction (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)" )
{
#if gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS
    not_null< int* > p;
#endif
}

CASE( "not_null<>: Disallows construction from nullptr_t, NULL or 0 (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)" )
{
#if gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS
# if gsl_HAVE_NULLPTR
    not_null< int* > p = nullptr; 
# endif 
    not_null< int* > p = NULL;
    not_null< std::vector<char>* > q = 0;
#endif 
}

CASE( "not_null<>: Disallows construction from a unique pointer to underlying type (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)" )
{
#if gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS
# if gsl_HAVE_UNIQUE_PTR
    std::unique_ptr< int > up = std::make_unique<int>( 120 );
    not_null< int* > p4 = up;
# endif
#endif
}

CASE( "not_null<>: Disallows assignment from unrelated pointers (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)" )
{
#if gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS
    MyDerived derived;
    not_null< MyDerived* > p = &derived;

    not_null< Unrelated* > r = p;
    not_null< Unrelated* > s = reinterpret_cast< Unrelated* >( p );
#endif
}

CASE( "not_null<>: Terminates construction from a null pointer value" )
{
#if gsl_HAVE_NULLPTR
    struct F { static void blow() { int * z = nullptr; not_null<int*> p(z); } };
#else
    struct F { static void blow() { int * z = NULL; not_null<int*> p(z); } };
#endif

    EXPECT_THROWS( F::blow() );
}

CASE( "not_null<>: Terminates construction from related pointer types for null pointer value" )
{
#if gsl_HAVE_NULLPTR
    struct F { static void blow() { MyDerived * z = nullptr; not_null<MyBase*> p(z); } };
#else
    struct F { static void blow() { MyDerived * z = NULL; not_null<MyBase*> p(z); } };
#endif

    EXPECT_THROWS( F::blow() );
}

CASE( "not_null<>: Terminates assignment from a null pointer value" )
{
    int i = 12;
    not_null<int*> p = &i; 
#if gsl_HAVE_NULLPTR
    int * z = nullptr;
#else
    int * z = NULL;
#endif 

    EXPECT_THROWS( p = z );
}

CASE( "not_null<>: Terminates assignment from related pointer types for null pointer value" )
{
#if gsl_HAVE_NULLPTR
    MyDerived * z = nullptr;
#else
    MyDerived * z = NULL;
#endif 
    MyDerived derived;
    not_null< MyBase* > p = &derived;

    EXPECT_THROWS( p = z );
}

CASE( "not_null<>: Allows construction from a non-null underlying pointer" )
{
    int i = 12; 

    not_null< int* > p( &i );

    EXPECT( p.get() == &i );
}

CASE( "not_null<>: Allows construction from a non-null user-defined ref-counted type" )
{
    int i = 12; 
    RefCounted< int > rp = RefCounted<int>( &i );

    not_null<   int*>  p( rp );

    EXPECT( p.get() == &i );
}

CASE( "not_null<>: Allows construction from a non-null related pointer" )
{
    MyDerived derived;
    not_null< MyBase* > p = &derived;

    EXPECT( (void*)p.get() == &derived );
}

CASE( "not_null<>: Allows construction from a not_null related pointer type" )
{
    MyDerived derived;
    not_null< MyDerived* > p = &derived;

    not_null< MyBase* > q = p;

    EXPECT( q == p );
}

CASE( "not_null<>: Allows assignment from a not_null related pointer type" )
{
    MyDerived derived;
    not_null< MyDerived* > p = &derived;
    not_null< MyBase*    > q = p;
    
    q = p;

    EXPECT( q == p );
}

CASE( "not_null<>: Allows assignment from a non-null bare recast pointer" )
{
    MyDerived derived;
    not_null< MyDerived* > p = &derived;

    not_null< Unrelated* > t = reinterpret_cast< Unrelated* >( p.get() );

    EXPECT( (void*)p.get() == (void*)t.get() );
}

CASE( "not_null<>: Allows implicit conversion to underlying type" )
{
    struct F { static bool helper( not_null<int*> p ) { return *p == 12; } };

    int i = 12;
    not_null< int* > p = &i; 

    EXPECT( *p == i );
    EXPECT( F::helper( p ) );
}

}

// end of file
