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
#include <vector>

using namespace gsl;

namespace {

struct MyBase {};
struct MyDerived : public MyBase {};
struct Unrelated {};

// Stand-in for a user-defined ref-counted class.
template<typename T>
struct RefCounted
{
    RefCounted( T * p ) : p_( p ) {}
    operator T *() const { return p_; }
    T * p_;
};

}

CASE( "not_null<>: Disallows default construction (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)" )
{
#if gsl_CONFIG( CONFIRMS_COMPILATION_ERRORS )
    not_null< int* > p;
#endif
}

CASE( "not_null<>: Disallows construction from nullptr_t, NULL or 0 (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)" )
{
#if gsl_CONFIG( CONFIRMS_COMPILATION_ERRORS )
# if gsl_HAVE( NULLPTR )
    not_null< int* > p( nullptr );
# endif
    not_null< int* > q( NULL );
    not_null< std::vector<char>* > r( 0 );
#endif
}

CASE( "not_null<>: Disallows construction from a unique pointer to underlying type (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)" )
{
#if gsl_CONFIG( CONFIRMS_COMPILATION_ERRORS )
# if gsl_HAVE( UNIQUE_PTR )
    std::unique_ptr< int > up( new int(120) );
    not_null< int* > p4( up );
# endif
#endif
}

CASE( "not_null<>: Disallows assignment from unrelated pointers (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)" )
{
#if gsl_CONFIG( CONFIRMS_COMPILATION_ERRORS )
    MyDerived derived;
    not_null< MyDerived* > p( &derived );

    not_null< Unrelated* > r( p );
    not_null< Unrelated* > s( reinterpret_cast< Unrelated* >( p ) );
#endif
}

CASE( "not_null<>: Terminates construction from a null pointer value" )
{
#if gsl_HAVE( NULLPTR )
    struct F { static void blow() { int * z = nullptr; not_null<int*> p(z); } };
#else
    struct F { static void blow() { int * z = NULL; not_null<int*> p(z); } };
#endif

    EXPECT_THROWS( F::blow() );
}

CASE( "not_null<>: Terminates construction from related pointer types for null pointer value" )
{
#if gsl_HAVE( NULLPTR )
    struct F { static void blow() { MyDerived * z = nullptr; not_null<MyBase*> p(z); } };
#else
    struct F { static void blow() { MyDerived * z = NULL; not_null<MyBase*> p(z); } };
#endif

    EXPECT_THROWS( F::blow() );
}

CASE( "not_null<>: Terminates assignment from a null pointer value" )
{
    int i = 12;
    not_null<int*> p( &i );
#if gsl_HAVE( NULLPTR )
    int * z = nullptr;
#else
    int * z = NULL;
#endif

    EXPECT_THROWS( p = not_null<int*>( z ) );
}

CASE( "not_null<>: Terminates assignment from related pointer types for null pointer value" )
{
#if gsl_HAVE( NULLPTR )
    MyDerived * z = nullptr;
#else
    MyDerived * z = NULL;
#endif
    MyDerived derived;
    not_null< MyBase* > p( &derived );

    EXPECT_THROWS( p = not_null<MyDerived*>( z ) );
}

CASE( "not_null<>: Allows to construct from a non-null underlying pointer" )
{
    int i = 12;

    not_null< int* > p( &i );

    EXPECT( p.get() == &i );
}

CASE( "not_null<>: Allows to construct from a non-null user-defined ref-counted type" )
{
    int i = 12;
    RefCounted< int > rp = RefCounted<int>( &i );

    not_null<   int*>  p( rp );

    EXPECT( p.get() == &i );
}

CASE( "not_null<>: Allows to construct from a non-null related pointer" )
{
    MyDerived derived;
    not_null< MyBase* > p( &derived );

    EXPECT( p.get() == &derived );
}

CASE( "not_null<>: Allows to construct from a not_null related pointer type" )
{
    MyDerived derived;
    not_null< MyDerived* > p( &derived );

    not_null< MyBase* > q = p;

    EXPECT( q == p );
}

CASE( "not_null<>: Allows assignment from a not_null related pointer type" )
{
    MyDerived derived;
    not_null< MyDerived* > p( &derived );
    not_null< MyBase*    > q = p;

    q = p;

    EXPECT( q == p );
}

CASE( "not_null<>: Allows assignment from a non-null bare recast pointer" )
{
    MyDerived derived;
    not_null< MyDerived* > p( &derived );

    not_null< Unrelated* > t( reinterpret_cast< Unrelated* >( p.get() ) );

    EXPECT( p.get() == reinterpret_cast< MyDerived* >( t.get() ) );
}

CASE( "not_null<>: Allows implicit conversion to underlying type" )
{
    struct F { static bool helper( not_null<int*> p ) { return *p == 12; } };

    int i = 12;
    not_null< int* > p( &i );

    EXPECT( F::helper( p ) );
}

namespace nonlocal
{
    struct S { char c; int i; };
}

CASE( "not_null<>: Allows indirect member access" )
{
    using namespace nonlocal;
    S s = { 'a', 7 };
    not_null< S* > p( &s );

    EXPECT( p->c == 'a' );
    EXPECT( p->i ==  7  );
}

CASE( "not_null<>: Allows dereferencing" )
{
    int i = 12;
    not_null< int* > p( &i );

    EXPECT( *p == i );
}

namespace {

// provide not_null pointers to compare:

struct NotNull
{
    int    v1; int          v2;
    int * pv1; int const * pv2;

    NotNull()
    :  v1(   7 ),  v2(  42 )
    , pv1( &v1 ), pv2( &v2 ) {}

    not_null<int       *> p1() const { return not_null<int       *>( pv1 ); }
    not_null<int const *> p2() const { return not_null<int const *>( pv2 ); }
};
}


CASE( "not_null<>: Allows to compare equal to another not_null of the same type" )
{
    NotNull _;

    EXPECT(     _.p1() == _.p1() );
    EXPECT_NOT( _.p1() == _.p2() );
}

CASE( "not_null<>: Allows to compare unequal to another not_null of the same type" )
{
    NotNull _;

    EXPECT(     _.p1() != _.p2() );
    EXPECT_NOT( _.p1() != _.p1() );
}

CASE( "not_null<>: Allows to compare less than another not_null of the same type" )
{
    NotNull _;

    EXPECT_NOT( _.p1() < _.p1() );
    EXPECT(   ( _.p1() < _.p2() ) == ( _.pv1 < _.pv2 ) );
    EXPECT(   ( _.p2() < _.p1() ) == ( _.pv2 < _.pv1 ) );
}

CASE( "not_null<>: Allows to compare less than or equal to another not_null of the same type" )
{
    NotNull _;

    EXPECT( ( _.p1() <= _.p1() ) );
    EXPECT( ( _.p1() <= _.p2() ) == ( _.pv1 <= _.pv2 ) );
    EXPECT( ( _.p2() <= _.p1() ) == ( _.pv2 <= _.pv1 ) );
}

CASE( "not_null<>: Allows to compare greater than another not_null of the same type" )
{
    NotNull _;

    EXPECT_NOT( _.p1() > _.p1() );
    EXPECT(   ( _.p1() > _.p2() ) == ( _.pv1 > _.pv2 ) );
    EXPECT(   ( _.p2() > _.p1() ) == ( _.pv2 > _.pv1 ) );
}

CASE( "not_null<>: Allows to compare greater than or equal to another not_null of the same type" )
{
    NotNull _;

    EXPECT(   _.p1() >= _.p1() );
    EXPECT( ( _.p1() >= _.p2() ) == ( _.pv1 >= _.pv2 ) );
    EXPECT( ( _.p2() >= _.p1() ) == ( _.pv2 >= _.pv1 ) );
}

// raw pointer

CASE( "not_null<>: Allows to compare equal to a raw pointer of the same type" )
{
    NotNull _;

    EXPECT(     _.p1() == _.pv1 );
    EXPECT_NOT( _.p1() == _.pv2 );
}

CASE( "not_null<>: Allows to compare unequal to a raw pointer of the same type" )
{
    NotNull _;

    EXPECT(     _.p1() != _.pv2 );
    EXPECT_NOT( _.p1() != _.pv1 );
}

CASE( "not_null<>: Allows to compare less than a raw pointer of the same type" )
{
    NotNull _;

    EXPECT_NOT( _.p1() < _.pv1 );
    EXPECT(   ( _.p1() < _.pv2 ) == ( _.pv1 < _.pv2 ) );
    EXPECT(   ( _.p2() < _.pv1 ) == ( _.pv2 < _.pv1 ) );
}

CASE( "not_null<>: Allows to compare less than or equal to a raw pointer of the same type" )
{
    NotNull _;

    EXPECT( ( _.p1() <= _.pv1 ) );
    EXPECT( ( _.p1() <= _.pv2 ) == ( _.pv1 <= _.pv2 ) );
    EXPECT( ( _.p2() <= _.pv1 ) == ( _.pv2 <= _.pv1 ) );
}

CASE( "not_null<>: Allows to compare greater than a raw pointer of the same type" )
{
    NotNull _;

    EXPECT_NOT( _.p1() > _.pv1 );
    EXPECT(   ( _.p1() > _.pv2 ) == ( _.pv1 > _.pv2 ) );
    EXPECT(   ( _.p2() > _.pv1 ) == ( _.pv2 > _.pv1 ) );
}

CASE( "not_null<>: Allows to compare greater than or equal to a raw pointer of the same type" )
{
    NotNull _;

    EXPECT(   _.p1() >= _.pv1 );
    EXPECT( ( _.p1() >= _.pv2 ) == ( _.pv1 >= _.pv2 ) );
    EXPECT( ( _.p2() >= _.pv1 ) == ( _.pv2 >= _.pv1 ) );
}

// end of file
