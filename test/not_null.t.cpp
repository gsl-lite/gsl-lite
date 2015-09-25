// Copyright 2015 by Martin Moene
//
// gsl-lite is based on GSL: Guidelines Support Library,
// https://github.com/microsoft/gsl
//
// This code is licensed under the MIT License (MIT). 
//

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

CASE( "not_null<>: Can construct from a non-null pointer" )
{
    int i = 12; 
    not_null< int* > p( &i );

    EXPECT( p.get() == &i );
}

CASE( "not_null<>: Can construct from a non-null user-defined ref-counted type" )
{
    int i = 12; 
    RefCounted< int > rp = RefCounted<int>(&i);
    not_null<   int*>  p( rp );

    EXPECT( p.get() == &i );
}

CASE( "not_null<>: Doesn't compile construction from a null pointer (define gsl_CONFIRM_COMPILATION_ERRORS)" )
{
#ifdef gsl_CONFIRM_COMPILATION_ERRORS
# if gsl_HAVE_NULLPTR
    not_null< int* > p1 = nullptr; 
# else
    not_null< int* > p1 = NULL;
# endif 
    not_null< std::vector<char>* > p2 = 0;
    not_null< int* > p3;

# if gsl_HAVE_UNIQUE_PTR
    std::unique_ptr< int > up = std::make_unique<int>( 120 );
    not_null< int* > p4 = up;
# endif
#endif
}

CASE( "not_null<>: Terminates/throws if constructed from a null pointer" )
{
#if gsl_HAVE_NULLPTR
    struct F { static void blow() { int * z = nullptr; not_null<int*> p(z); } };
#else
    struct F { static void blow() { int * z = NULL; not_null<int*> p(z); } };
#endif

    EXPECT_THROWS( F::blow() );
}

CASE( "not_null<>: Terminates/throws if assigned a null pointer" )
{
    int i = 12;
    not_null<int*> p = &i; 
#if gsl_HAVE_NULLPTR
    int * q = nullptr;
#else
    int * q = NULL;
#endif 

    EXPECT_THROWS( p = q );
}

CASE( "not_null<>: Can access value if pointer is non-null" )
{
    struct F { static bool helper( not_null<int*> p ) { return *p == 12; } };

    int i = 12;
    not_null< int* > p = &i; 

    EXPECT( F::helper(p) );
}

CASE( "not_null<>: Can assign to pointer to base from pointer to derived if non-null " )
{
    MyDerived derived;
    not_null< MyDerived* > p = &derived;
    not_null< MyBase*    > q = p;

    EXPECT( q == p );
}

CASE( "not_null<>: Can assign to pointer from bare recast pointer if non-null" )
{
    MyDerived derived;
    not_null< MyDerived* > p = &derived;
    not_null< Unrelated* > t = reinterpret_cast< Unrelated* >( p.get() );

    EXPECT( (void*)p.get() == (void*)t.get() );
}

CASE( "not_null<>: Doesn't compile assignment to an unrelated pointer (define gsl_CONFIRM_COMPILATION_ERRORS)" )
{
    MyDerived derived;
    not_null< MyDerived* > p = &derived;

#ifdef gsl_CONFIRM_COMPILATION_ERRORS
    not_null< Unrelated* > r = p;
    not_null< Unrelated* > s = reinterpret_cast< Unrelated* >( p );
#endif
}

}

// end of file
