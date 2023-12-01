//
// gsl-lite is based on GSL: Guidelines Support Library.
// For more information see https://github.com/gsl-lite/gsl-lite
//
// Copyright (c) 2015-2019 Martin Moene
// Copyright (c) 2019-2021 Moritz Beutel
// Copyright (c) 2015-2018 Microsoft Corporation. All rights reserved.
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

template< class T >
void take_raw( T* )
{
}
#if gsl_HAVE( UNIQUE_PTR )
template< class T >
void take_unique_by_val( std::unique_ptr<T> )
{
}
template< class T >
void take_unique_by_ref( std::unique_ptr<T> const & )
{
}
#endif
#if gsl_HAVE( SHARED_PTR )
template< class T >
void take_shared_by_val( std::shared_ptr<T> )
{
}
template< class T >
void take_shared_by_ref( std::shared_ptr<T> const & )
{
}
template< class T >
struct ExplicitFromShared
{
    explicit ExplicitFromShared( std::shared_ptr<T> const & )
    {
    }
};
#endif

} // anonymous namespace

namespace nonlocal
{
    struct S { char c; int i; };
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

CASE( "not_null<>: Layout is compatible to underlying type" )
{
#if gsl_HAVE( TYPE_TRAITS )
    static_assert( sizeof( not_null< int* > ) == sizeof( int* ), "static assertion failed" );
    static_assert( sizeof( not_null< unique_ptr< int > > ) == sizeof( unique_ptr< int > ), "static assertion failed" );
    static_assert( sizeof( not_null< shared_ptr< int > > ) == sizeof( shared_ptr< int > ), "static assertion failed" );
#endif
}

CASE( "not_null<>: Convertibility is correctly reported by type traits" )
{
#if gsl_HAVE( TYPE_TRAITS ) && gsl_HAVE( UNIQUE_PTR ) && !gsl_BETWEEN( gsl_COMPILER_MSVC_VERSION, 1, 120 )

    // Permit conversion to underlying type.
    static_assert(  std::is_convertible< not_null< int* >, int* >::value, "static assertion failed" );
    static_assert(  std::is_convertible< not_null< int* >, not_null< int* > >::value, "static assertion failed" );
# if gsl_CONFIG( NOT_NULL_EXPLICIT_CTOR )
    static_assert( !std::is_convertible< int*, not_null< int* > >::value, "static assertion failed" );
# endif

    // Permit conversion to underlying type with implicit upcasts.
    static_assert(  std::is_convertible< not_null< MyDerived* >, MyBase* >::value, "static assertion failed" );
    static_assert(  std::is_convertible< not_null< MyDerived* >, not_null< MyBase* > >::value, "static assertion failed" );
# if gsl_CONFIG( NOT_NULL_EXPLICIT_CTOR )
    static_assert( !std::is_convertible< MyDerived*, not_null< MyBase* > >::value, "static assertion failed" );
# endif

    // Permit conversion to underlying type with implicit upcasts for move-only types.
# if gsl_HAVE( FUNCTION_REF_QUALIFIER )
    static_assert(  std::is_convertible< not_null< std::unique_ptr< MyDerived > >, std::unique_ptr< MyBase > >::value, "static assertion failed" );
    static_assert( !std::is_convertible< not_null< std::unique_ptr< MyBase > >, std::unique_ptr< MyDerived > >::value, "static assertion failed" );
# endif

    // Permit implicit upcasts for move-only types.
    static_assert(  std::is_convertible< not_null< std::unique_ptr< MyDerived > >, not_null< std::unique_ptr< MyBase > > >::value, "static assertion failed" );
# if gsl_CONFIG( NOT_NULL_EXPLICIT_CTOR )
    static_assert( !std::is_convertible< std::unique_ptr< MyDerived >, not_null< std::unique_ptr< MyBase > > >::value, "static assertion failed" );
# endif

    // `unique_ptr<T>` and `shared_ptr<T>` have explicit constructors for `T*` arguments; do not allow implicit conversion.
# if !gsl_BETWEEN( gsl_COMPILER_CLANG_VERSION, 1, 400 ) && !gsl_BETWEEN( gsl_COMPILER_APPLECLANG_VERSION, 1, 1001 )
    static_assert( !std::is_convertible< int*, not_null< std::unique_ptr< int > > >::value, "static assertion failed" );
    static_assert( !std::is_convertible< int*, not_null< std::shared_ptr< int > > >::value, "static assertion failed" );
# endif

    // Do not permit implicit downcasts for move-only types, with or without conversion.
# if !gsl_BETWEEN( gsl_COMPILER_CLANG_VERSION, 1, 400 ) && !gsl_BETWEEN( gsl_COMPILER_APPLECLANG_VERSION, 1, 1001 )
    static_assert( !std::is_convertible< not_null< std::unique_ptr< MyBase > >, not_null< std::unique_ptr< MyDerived > > >::value, "static assertion failed" );
    static_assert( !std::is_convertible< std::unique_ptr< MyBase >, not_null< std::unique_ptr< MyDerived > > >::value, "static assertion failed" );
# endif

    static_assert( !std::is_convertible< not_null < int* >, bool >::value, "static assertion failed" );
    static_assert( !std::is_convertible< not_null < std::unique_ptr< int > >, bool >::value, "static assertion failed" );
    static_assert( !std::is_convertible< not_null < std::shared_ptr< int > >, bool >::value, "static assertion failed" );
#endif // gsl_HAVE( TYPE_TRAITS ) && gsl_HAVE( UNIQUE_PTR ) && !gsl_BETWEEN( gsl_COMPILER_MSVC_VERSION, 1, 120 )
}

CASE( "not_null<>: Copyability and assignability are correctly reported by type traits" )
{
#if gsl_HAVE( TYPE_TRAITS ) && gsl_HAVE( UNIQUE_PTR ) && !gsl_BETWEEN( gsl_COMPILER_MSVC_VERSION, 1, 140 )
    // Permit copy construction and assignment for raw pointers.
    static_assert(  std::is_copy_constructible< not_null< int* > >::value, "static assertion failed" );
    static_assert(  std::is_copy_assignable<    not_null< int* > >::value, "static assertion failed" );

    // Do not permit copy construction and assignment for move-only types.
    static_assert( !std::is_copy_constructible< not_null< std::unique_ptr< int > > >::value, "static assertion failed" );
# if !gsl_BETWEEN( gsl_COMPILER_APPLECLANG_VERSION, 1, 900 )
    static_assert( !std::is_copy_assignable<    not_null< std::unique_ptr< int > > >::value, "static assertion failed" );
# endif

    // Permit explicit construction of underlying smart pointer from raw pointer type, using the smart pointer's explicit constructor.
    static_assert( std::is_constructible< not_null< std::unique_ptr< int > >, int* >::value, "static assertion failed" );
    static_assert( std::is_constructible< not_null< std::shared_ptr< int > >, int* >::value, "static assertion failed" );

    // Permit explicit construction from underlying pointer.
    static_assert(  std::is_constructible< not_null< MyBase* >, MyDerived* >::value, "static assertion failed" );
# if gsl_CONFIG( NOT_NULL_EXPLICIT_CTOR )
    static_assert( !std::is_assignable<    not_null< MyBase* >&, MyDerived* >::value, "static assertion failed" );
# endif

    // Do not permit conversion to subclass pointer.
    static_assert( !std::is_constructible< MyDerived*, not_null< MyBase* > >::value, "static assertion failed" );
# if !( defined( __NVCC__ ) && gsl_BETWEEN( gsl_COMPILER_GNUC_VERSION, 800, 900 ) ) // oddly specific bug workaround
    static_assert( !std::is_assignable<    MyDerived*&, not_null< MyBase* > >::value, "static assertion failed" );
# endif

    // Permit construction and assignment from subclass pointer.
    static_assert(  std::is_constructible< not_null< std::unique_ptr< MyBase > >, std::unique_ptr< MyDerived > >::value, "static assertion failed" );
# if gsl_CONFIG( NOT_NULL_EXPLICIT_CTOR ) && !gsl_BETWEEN( gsl_COMPILER_APPLECLANG_VERSION, 1, 900 )
    static_assert( !std::is_assignable<    not_null< std::unique_ptr< MyBase > >&, std::unique_ptr< MyDerived > >::value, "static assertion failed" );
# endif

    // Do not permit copy construction and assignment from move-only subclass pointer.
    static_assert( !std::is_constructible< not_null< std::unique_ptr< MyBase > >, std::unique_ptr< MyDerived > const & >::value, "static assertion failed" );
# if !gsl_BETWEEN( gsl_COMPILER_APPLECLANG_VERSION, 1, 900 )
    static_assert( !std::is_assignable<    not_null< std::unique_ptr< MyBase > >&, std::unique_ptr< MyDerived > const & >::value, "static assertion failed" );
# endif

    // Permit construction and assignment from `not_null<>` with subclass pointer.
    static_assert(  std::is_constructible< not_null< std::unique_ptr< MyBase > >, not_null< std::unique_ptr< MyDerived > > >::value, "static assertion failed" );
    static_assert(  std::is_assignable<    not_null< std::unique_ptr< MyBase > >&, not_null< std::unique_ptr< MyDerived > > >::value, "static assertion failed" );

    // Do not permit copy construction and assignment from move-only `not_null<>` with subclass pointer.
    static_assert( !std::is_constructible< not_null< std::unique_ptr< MyBase > >, not_null< std::unique_ptr< MyDerived > > const & >::value, "static assertion failed" );
# if !gsl_BETWEEN( gsl_COMPILER_APPLECLANG_VERSION, 1, 900 )
    static_assert( !std::is_assignable<    not_null< std::unique_ptr< MyBase > >&, not_null< std::unique_ptr< MyDerived > > const & >::value, "static assertion failed" );
# endif

    // Permit conversion to superclass pointer.
# if gsl_HAVE( FUNCTION_REF_QUALIFIER )
    static_assert(  std::is_constructible< std::unique_ptr< MyBase >, not_null< std::unique_ptr< MyDerived > > >::value, "static assertion failed" );
    static_assert(  std::is_assignable<    std::unique_ptr< MyBase >&, not_null< std::unique_ptr< MyDerived > > >::value, "static assertion failed" );
# endif

    // Do not permit conversion with copy to move-only superclass pointer.
    static_assert( !std::is_constructible< std::unique_ptr< MyBase >, not_null< std::unique_ptr< MyDerived > > const & >::value, "static assertion failed" );
# if !gsl_BETWEEN( gsl_COMPILER_APPLECLANG_VERSION, 1, 900 )
    static_assert( !std::is_assignable<    std::unique_ptr< MyBase >&, not_null< std::unique_ptr< MyDerived > > const & >::value, "static assertion failed" );
# endif

    // Do not permit construction and assignment from superclass pointer.
# if !gsl_BETWEEN( gsl_COMPILER_CLANG_VERSION, 1, 400 ) && !gsl_BETWEEN( gsl_COMPILER_APPLECLANG_VERSION, 1, 1001 )
    static_assert( !std::is_constructible< not_null< std::unique_ptr< MyDerived > >, std::unique_ptr< MyBase > >::value, "static assertion failed" );
    static_assert( !std::is_assignable<    not_null< std::unique_ptr< MyDerived > >&, std::unique_ptr< MyBase > >::value, "static assertion failed" );

    static_assert( !std::is_constructible< not_null< std::unique_ptr< MyDerived > >, not_null< std::unique_ptr< MyBase > > >::value, "static assertion failed" );
    static_assert( !std::is_assignable<    not_null< std::unique_ptr< MyDerived > >&, not_null< std::unique_ptr< MyBase > > >::value, "static assertion failed" );
# endif

    // Do not permit conversion to subclass pointer.
    static_assert( !std::is_constructible< std::unique_ptr< MyDerived >, not_null< std::unique_ptr< MyBase > > >::value, "static assertion failed" );
    static_assert( !std::is_assignable<    std::unique_ptr< MyDerived >&, not_null< std::unique_ptr< MyBase > > >::value, "static assertion failed" );

    // Do not permit conversion to bool
    static_assert( !std::is_constructible< bool, not_null < int* > >::value, "static assertion failed");
    static_assert( !std::is_assignable<    bool&, not_null < int* > >::value, "static assertion failed");

    static_assert( !std::is_constructible< bool, not_null < std::unique_ptr< int > > >::value, "static assertion failed");
    static_assert( !std::is_assignable<    bool&, not_null < std::unique_ptr< int > > >::value, "static assertion failed");

    static_assert( !std::is_constructible< bool, not_null < std::shared_ptr< int > > >::value, "static assertion failed");
    static_assert( !std::is_assignable<    bool&, not_null < std::shared_ptr< int > > >::value, "static assertion failed");
#endif // gsl_HAVE( TYPE_TRAITS ) && gsl_HAVE( UNIQUE_PTR ) && !gsl_BETWEEN( gsl_COMPILER_MSVC_VERSION, 1, 140 )
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

CASE( "not_null<>: Terminates construction from a null pointer value (raw pointer)" )
{
    struct F { static void blow() { int * z = gsl_nullptr; not_null<int*> p(z); } };

    EXPECT_THROWS( F::blow() );
}

CASE( "not_null<>: Terminates construction from related pointer types for null pointer value (raw pointer)" )
{
    struct F { static void blow() { MyDerived * z = gsl_nullptr; not_null<MyBase*> p(z); } };

    EXPECT_THROWS( F::blow() );
}

CASE( "not_null<>: Terminates assignment from a null pointer value (raw pointer)" )
{
    int i = 12;
    not_null<int*> p( &i );
    int * z = gsl_nullptr;

    EXPECT_THROWS( p = not_null<int*>( z ) );
}

CASE( "not_null<>: Terminates assignment from related pointer types for null pointer value (raw pointer)" )
{
    MyDerived * z = gsl_nullptr;
    MyDerived derived;
    not_null< MyBase* > p( &derived );

    EXPECT_THROWS( p = not_null<MyDerived*>( z ) );
}

CASE( "not_null<>: Allows to construct from a non-null underlying pointer (raw pointer)" )
{
    int i = 12;

    not_null< int* > p( &i );

    EXPECT( p == &i );
}

CASE( "not_null<>: Returns underlying pointer with get() (raw pointer)" )
{
#if !gsl_CONFIG( TRANSPARENT_NOT_NULL )
    int i = 12;
    not_null< int* > p( &i );

    int* pg = p.get();
    EXPECT( pg == &i );
#endif
}

CASE( "not_null<>: Allows to construct from a non-null underlying pointer (raw pointer) with make_not_null()" )
{
    int i = 12;

    not_null< int* > p = make_not_null( &i );

    EXPECT( p == &i );
}

CASE( "not_null<>: Allows to construct from a non-null underlying pointer (raw pointer) with deduction guide" )
{
#if gsl_HAVE( DEDUCTION_GUIDES )
    int i = 12;

    not_null p( &i );

    EXPECT( p == &i );
#endif
}

CASE( "not_null<>: Allows to construct a const pointer from a non-null underlying pointer (raw pointer)" )
{
    int i = 12;

    not_null< const int* > p( &i );

    EXPECT( p == &i );
}

CASE( "not_null<>: Converts to underlying pointer (raw pointer)" )
{
    int i = 12;
    not_null< int* > p( &i );

    take_raw<int>( p );
}

CASE( "as_nullable: Converts to underlying pointer (raw pointer)" )
{
    int i = 12;
    not_null< int* > p( &i );

    take_raw<int>( as_nullable( p ) );
#if gsl_HAVE( MOVE_FORWARD )
    take_raw<int>( as_nullable( std::move( p ) ) );
#endif
}

CASE( "not_null<>: Allows to construct from a non-null related pointer (raw pointer)" )
{
    MyDerived derived;
    not_null< MyBase* > p( &derived );

    EXPECT( p == &derived );
}

CASE( "not_null<>: Allows to construct a const pointer from a non-null related pointer (raw pointer)" )
{
    MyDerived derived;
    not_null< const MyBase* > p( &derived );

    EXPECT( p == &derived );
}

CASE( "not_null<>: Allows to construct from a not_null related pointer type (raw pointer)" )
{
    MyDerived derived;
    not_null< MyDerived* > p( &derived );
    
#if gsl_CPP11_OR_GREATER
    not_null< MyBase* > q = p;
#else
    not_null< MyBase* > q(p); // in C++98, we cannot differentiate between implicit and explicit cases, so conversion is always explicit
#endif

    EXPECT( q == p );
}

CASE( "not_null<>: Allows to construct a const pointer from a not_null related pointer type (raw pointer)" )
{
    MyDerived derived;
    not_null< MyDerived* > p( &derived );
    
#if gsl_CPP11_OR_GREATER
    not_null< const MyBase* > q = p;
#else
    not_null< const MyBase* > q(p); // in C++98, we cannot differentiate between implicit and explicit cases, so conversion is always explicit
#endif

    EXPECT( q == p );
}

CASE( "not_null<>: Converts to a related pointer (raw pointer)" )
{
    MyDerived derived;
    not_null< MyDerived* > p( &derived );

    take_raw<MyBase>( p );
}

CASE( "as_nullable: Converts to a related pointer (raw pointer)" )
{
    MyDerived derived;
    not_null< MyDerived* > p( &derived );

    take_raw<MyBase>( as_nullable( p ) );
}

CASE( "not_null<>: Allows assignment from a not_null related pointer type (raw pointer)" )
{
    MyDerived derived;
    not_null< MyDerived* > p( &derived );
    not_null< MyBase*    > q( p );

    q = p;

    EXPECT( q == p );
}

CASE( "not_null<>: Allows assignment to a const pointer from a not_null related pointer type (raw pointer)" )
{
    MyDerived derived;
    not_null< MyDerived*    > p( &derived );
    not_null< const MyBase* > q( p );

    q = p;

    EXPECT( q == p );
}

CASE( "not_null<>: Allows indirect member access (raw pointer)" )
{
    using namespace nonlocal;
    S s = { 'a', 7 };
    not_null< S* > p( &s );

    EXPECT( p->c == 'a' );
    EXPECT( p->i ==  7  );
}

CASE( "not_null<>: Allows dereferencing (raw pointer)" )
{
    int i = 12;
    not_null< int* > p( &i );

    EXPECT( *p == i );
}

CASE( "not_null<>: Allows to check whether object is valid (raw pointer)" )
{
    int i = 12;
    not_null< int* > p( &i );

    EXPECT( gsl::is_valid( p ) );

#if gsl_HAVE( MOVE_FORWARD )
    not_null< int* > q( std::move( p ) );

    EXPECT( gsl::is_valid( p ) ); // for raw pointers, moving `not_null<>` just makes a copy
    EXPECT( gsl::is_valid( q ) );
#endif
}

#if gsl_HAVE( MOVE_FORWARD )
template< class T >
void move_to( T& dest, T& src )
{
    dest = std::move( src );
}
#endif // gsl_HAVE( MOVE_FORWARD )

#if gsl_HAVE( SHARED_PTR )
CASE( "not_null<>: Terminates swap of a moved-from value (shared_ptr)" )
{
    shared_ptr< int > pi = std::make_shared< int >( 12 );
    not_null< shared_ptr< int > > p1( pi );
    not_null< shared_ptr< int > > p2( std::move( p1 ) );

    EXPECT_THROWS( swap( p1, p2 ) );
    EXPECT_THROWS( swap( p2, p1 ) );
}

CASE( "not_null<>: Tolerates self-move-assignment of a moved-from value (shared_ptr)" )
{
    shared_ptr< int > pi = std::make_shared< int >( 12 );
    not_null< shared_ptr< int > > p1( pi );
    not_null< shared_ptr< int > > p2( std::move( p1 ) );

    EXPECT_NO_THROW( move_to(p1, p1 ) );  // use extra indirection to suppress compiler warning about explicit self-move
}

CASE( "not_null<>: Terminates self-swap of a moved-from value (shared_ptr)" )
{
    shared_ptr< int > pi = std::make_shared< int >( 12 );
    not_null< shared_ptr< int > > p1( pi );
    not_null< shared_ptr< int > > p2( std::move( p1 ) );

    EXPECT_THROWS( swap( p1, p1 ) );
}

CASE( "not_null<>: Terminates construction from a null pointer value (shared_ptr)" )
{
    struct F { static void blow() { shared_ptr< int > z = gsl_nullptr; not_null< shared_ptr< int > > p(z); } };

    EXPECT_THROWS( F::blow() );
}

CASE( "not_null<>: Terminates construction from related pointer types for null pointer value (shared_ptr)" )
{
    struct F { static void blow() { shared_ptr< MyDerived > z = gsl_nullptr; not_null< shared_ptr< MyBase > > p(z); } };

    EXPECT_THROWS( F::blow() );
}

CASE( "not_null<>: Terminates assignment from a null pointer value (shared_ptr)" )
{
    not_null< shared_ptr< int > > p( std::make_shared< int >(12) );
    shared_ptr< int > z = gsl_nullptr;

    EXPECT_THROWS( p = not_null< shared_ptr< int > >( z ) );
}

CASE( "not_null<>: Terminates assignment from related pointer types for null pointer value (shared_ptr)" )
{
    shared_ptr< MyDerived >  z = gsl_nullptr;
    not_null< shared_ptr< MyBase > > p( std::make_shared< MyDerived >() );

    EXPECT_THROWS( p = not_null< shared_ptr< MyDerived > >( z ) );
}

CASE( "not_null<>: Terminates propagation of a moved-from value (shared_ptr)" )
{
    shared_ptr< int > pi = std::make_shared< int >(12);
    not_null< shared_ptr< int > > p( std::move( pi ) );
    not_null< shared_ptr< int > > q( std::move( p ) );

    EXPECT_THROWS( not_null< shared_ptr< int > > v( p ) );
    EXPECT_THROWS( not_null< shared_ptr< int > > v( std::move( p ) ) );
    EXPECT_THROWS( q = p );
    EXPECT_THROWS( q = std::move( p ) );
}

CASE( "not_null<>: Allows self-swap (shared_ptr)" )
{
    shared_ptr< int > pi = std::make_shared< int >( 12 );
    not_null< shared_ptr< int > > p( pi );

    EXPECT_NO_THROW( swap( p, p ) );
    EXPECT( p == pi );
}

CASE( "not_null<>: Allows swap (shared_ptr)" )
{
    shared_ptr< int > pi1 = std::make_shared< int >( 12 );
    shared_ptr< int > pi2 = std::make_shared< int >( 34 );
    not_null< shared_ptr< int > > p1( pi1 );
    not_null< shared_ptr< int > > p2( pi2 );

    EXPECT_NO_THROW( swap( p1, p2 ) );
    EXPECT( p1 == pi2 );
    EXPECT( p2 == pi1 );
}

CASE( "not_null<>: Allows to construct from a non-null underlying pointer (shared_ptr)" )
{
    shared_ptr< int > pi = std::make_shared< int >(12);
    not_null< shared_ptr< int > > p( pi );

    EXPECT( p == pi );
}

CASE( "not_null<>: Allows to construct from a non-null raw pointer with explicit conversion (shared_ptr)" )
{
    int* i = new int(12);
    not_null< shared_ptr< int > > p( i );
}

CASE( "not_null<>: Returns underlying pointer or raw pointer with get() (shared_ptr)" )
{
    shared_ptr< int > pi = std::make_shared< int >(12);
    not_null< shared_ptr< int > > p( pi );

#if gsl_CONFIG( TRANSPARENT_NOT_NULL )
    int* pg = p.get();
    EXPECT( pg == pi.get() );
#else
    shared_ptr< int > const & pg = p.get();
    EXPECT( pg == pi );
#endif
}

CASE( "not_null<>: Allows to move from a not_null pointer to an underlying pointer (shared_ptr)" )
{
#if gsl_HAVE( FUNCTION_REF_QUALIFIER )
    shared_ptr< int > pi = std::make_shared< int >(12);
    int* raw(pi.get());

    not_null< shared_ptr< int > > p ( std::move(pi) ); // There...
    pi = std::move(p); // ...and back again.

    EXPECT_THROWS( (void) *p );
    EXPECT( pi.get() == raw );
#endif
}

CASE( "as_nullable: Allows to move from a not_null pointer to an underlying pointer (shared_ptr)" )
{
    shared_ptr< int > pi = std::make_shared< int >(12);
    int* raw(pi.get());

    not_null< shared_ptr< int > > p ( std::move(pi) ); // There...
    pi = as_nullable( std::move(p) ); // ...and back again.

    EXPECT_THROWS( (void) *p );
    EXPECT( pi.get() == raw );
}

CASE( "not_null<>: Allows to construct from a non-null underlying pointer (shared_ptr) with make_not_null()" )
{
    shared_ptr< int > pi = std::make_shared< int >(12);

    not_null< shared_ptr< int > > p = make_not_null( pi );

    EXPECT( p == pi );
}

CASE( "not_null<>: Allows to construct from a non-null underlying pointer (shared_ptr) with deduction guide" )
{
#if gsl_HAVE( DEDUCTION_GUIDES )
    shared_ptr< int > pi = std::make_shared< int >(12);

    not_null p( pi );

    EXPECT( p == pi );
#endif
}

CASE( "not_null<>: Allows to construct a const pointer from a non-null underlying pointer (shared_ptr)" )
{
    shared_ptr< int > pi = std::make_shared< int >(12);
    not_null< shared_ptr< const int > > p( pi );

    EXPECT( p == pi );
}

CASE( "not_null<>: Converts to underlying pointer (shared_ptr)" )
{
    shared_ptr< int > pi = std::make_shared< int >();
    not_null< shared_ptr< int > > p( pi );

    take_shared_by_val<int>( p );
    take_shared_by_ref<int>( p );
}

CASE( "as_nullable: Converts to underlying pointer (shared_ptr)" )
{
    shared_ptr< int > pi = std::make_shared< int >();
    not_null< shared_ptr< int > > p( pi );

    take_shared_by_val<int>( as_nullable( p ) );
    take_shared_by_ref<int>( as_nullable( p ) );
}

CASE( "as_nullable: Terminates for moved-from pointer (shared_ptr)" )
{
    shared_ptr< int > pi = std::make_shared< int >();
    not_null< shared_ptr< int > > p( pi );
    not_null< shared_ptr< int > > p2( std::move( p ) );

    EXPECT_THROWS( (void) as_nullable( p ) );
}

CASE( "not_null<>: Allows to construct from a non-null related pointer (shared_ptr)" )
{
    shared_ptr< MyDerived > pderived = std::make_shared< MyDerived >();
    not_null< shared_ptr< MyBase > > p( pderived );

    EXPECT( p == pderived );
}

CASE( "not_null<>: Allows to construct a const pointer from a non-null related pointer (shared_ptr)" )
{
    shared_ptr< MyDerived > pderived = std::make_shared< MyDerived >();
    not_null< shared_ptr< const MyBase > > p( pderived );

    EXPECT( p == pderived );
}

CASE( "not_null<>: Allows to construct from a not_null related pointer type (shared_ptr)" )
{
    shared_ptr< MyDerived > pderived = std::make_shared< MyDerived >();
    not_null< shared_ptr< MyDerived > > p( pderived );

#if gsl_CPP11_OR_GREATER
    not_null< shared_ptr< MyBase > > q = p;
#else
    not_null< shared_ptr< MyBase > > q(p); // in C++98, we cannot differentiate between implicit and explicit cases, so conversion is always explicit
#endif

    EXPECT( q == p );
}

CASE( "not_null<>: Allows to construct a const pointer from a not_null related pointer type (shared_ptr)" )
{
    shared_ptr< MyDerived > pderived = std::make_shared< MyDerived >();
    not_null< shared_ptr< MyDerived > > p( pderived );

#if gsl_CPP11_OR_GREATER
    not_null< shared_ptr< const MyBase > > q = p;
#else
    not_null< shared_ptr< const MyBase > > q(p); // in C++98, we cannot differentiate between implicit and explicit cases, so conversion is always explicit
#endif

    EXPECT( q == p );
}

CASE( "not_null<>: Converts to a related pointer (shared_ptr)" )
{
    shared_ptr< MyDerived > pderived = std::make_shared< MyDerived >();
    not_null< shared_ptr< MyDerived > > p( pderived );

    take_shared_by_val<MyBase>( p );
    take_shared_by_ref<MyBase>( p );
}

CASE( "as_nullable: Converts to a related pointer (shared_ptr)" )
{
    shared_ptr< MyDerived > pderived = std::make_shared< MyDerived >();
    not_null< shared_ptr< MyDerived > > p( pderived );

    take_shared_by_val<MyBase>( as_nullable(p) );
    take_shared_by_ref<MyBase>( as_nullable(p) );
}

CASE( "not_null<>: Allows assignment from a not_null related pointer type (shared_ptr)" )
{
    shared_ptr< MyDerived > pderived = std::make_shared< MyDerived >();
    not_null< shared_ptr< MyDerived > > p( pderived );
    not_null< shared_ptr< MyBase >    > q( p );

    q = p;

    EXPECT( q == p );
}

CASE( "not_null<>: Allows assignment to a const pointer from a not_null related pointer type (shared_ptr)" )
{
    shared_ptr< MyDerived > pderived = std::make_shared< MyDerived >();
    not_null< shared_ptr< MyDerived    > > p( pderived );
    not_null< shared_ptr< const MyBase > > q( p );

    q = p;

    EXPECT( q == p );
}

CASE( "not_null<>: Allows indirect member access (shared_ptr)" )
{
    using namespace nonlocal;
    S s = { 'a', 7 };
    not_null< shared_ptr< S > > p( std::make_shared< S >(s) );

    EXPECT( p->c == 'a' );
    EXPECT( p->i ==  7  );
}

CASE( "not_null<>: Allows dereferencing (shared_ptr)" )
{
    shared_ptr< int > pi = std::make_shared< int >(12);
    not_null< shared_ptr< int > > p( pi );

    EXPECT( *p == *pi );
}

CASE( "not_null<>: Allows to check whether object is valid (shared_ptr)" )
{
    shared_ptr< int > pi = std::make_shared< int >(12);
    not_null< shared_ptr< int > > p( pi );

    EXPECT( gsl::is_valid( p ) );

    not_null< shared_ptr< int > > q( std::move( p ) );

    EXPECT_NOT( gsl::is_valid( p ) );
    EXPECT(     gsl::is_valid( q ) );
}

#endif // gsl_HAVE( SHARED_PTR )

#if gsl_HAVE( UNIQUE_PTR )

template<typename T>
unique_ptr<T> my_make_unique()
{
    return unique_ptr<T>(new T());
}

template<typename T, typename Arg>
unique_ptr<T> my_make_unique(Arg&& arg)
{
    return unique_ptr<T>(new T(std::forward<Arg>(arg)));
}

CASE( "not_null<>: Terminates swap of a moved-from value (unique_ptr)" )
{
    unique_ptr< int > pi = my_make_unique< int >( 12 );
    not_null< unique_ptr< int > > p1( std::move( pi ) );
    not_null< unique_ptr< int > > p2( std::move( p1 ) );

    EXPECT_THROWS( swap( p1, p2 ) );
    EXPECT_THROWS( swap( p2, p1 ) );
}

CASE( "not_null<>: Tolerates self-move-assignment of a moved-from value (unique_ptr)" )
{
    unique_ptr< int > pi = my_make_unique< int >( 12 );
    not_null< unique_ptr< int > > p1( std::move( pi ) );
    not_null< unique_ptr< int > > p2( std::move( p1 ) );

    EXPECT_NO_THROW( move_to(p1, p1 ) );  // use extra indirection to suppress compiler warning about explicit self-move
}

CASE( "not_null<>: Terminates self-swap of a moved-from value (unique_ptr)" )
{
    unique_ptr< int > pi = my_make_unique< int >( 12 );
    not_null< unique_ptr< int > > p1( std::move( pi ) );
    not_null< unique_ptr< int > > p2( std::move( p1 ) );

    EXPECT_THROWS( swap(p1, p1) );
}

CASE( "not_null<>: Terminates construction from a null pointer value (unique_ptr)" )
{
    struct F { static void blow() { unique_ptr< int > z = gsl_nullptr; not_null< unique_ptr< int > > p(std::move(z)); } };

    EXPECT_THROWS( F::blow() );
}

CASE( "not_null<>: Terminates construction from related pointer types for null pointer value (unique_ptr)" )
{
    struct F { static void blow() { unique_ptr< MyDerived > z = gsl_nullptr; not_null< unique_ptr< MyBase > > p(std::move(z)); } };

    EXPECT_THROWS( F::blow() );
}

CASE( "not_null<>: Terminates assignment from a null pointer value (unique_ptr)" )
{
    not_null< unique_ptr< int > > p( my_make_unique< int >(12) );
    unique_ptr< int > z = gsl_nullptr;

    EXPECT_THROWS( p = not_null< unique_ptr< int > >( std::move(z) ) );
}

CASE( "not_null<>: Terminates assignment from related pointer types for null pointer value (unique_ptr)" )
{
    unique_ptr< MyDerived >  z = gsl_nullptr;
    not_null< unique_ptr< MyBase > > p( my_make_unique< MyDerived >() );

    EXPECT_THROWS( p = not_null< unique_ptr< MyDerived > >( std::move(z) ) );
}

CASE( "not_null<>: Terminates propagation of a moved-from value (unique_ptr)" )
{
    unique_ptr< int > pi = my_make_unique< int >(12);
    not_null< unique_ptr< int > > p( std::move( pi ) );
    not_null< unique_ptr< int > > q( std::move( p ) );

    EXPECT_THROWS( not_null< unique_ptr< int > >( std::move( p ) ) );
    EXPECT_THROWS( q = std::move( p ) );
}

CASE( "not_null<>: Allows self-swap (unique_ptr)" )
{
    unique_ptr< int > pi = my_make_unique< int >( 12 );
    int* raw( pi.get() );
    not_null< unique_ptr< int > > p( std::move( pi ) );

    EXPECT_NO_THROW( swap( p, p ) );
    EXPECT( &*p == raw );
}

CASE( "not_null<>: Allows swap (unique_ptr)" )
{
    unique_ptr< int > pi1 = my_make_unique< int >( 12 );
    unique_ptr< int > pi2 = my_make_unique< int >( 34 );
    int* raw1( pi1.get() );
    int* raw2( pi2.get() );
    not_null< unique_ptr< int > > p1( std::move( pi1 ) );
    not_null< unique_ptr< int > > p2( std::move( pi2 ) );

    EXPECT_NO_THROW( swap( p1, p2 ) );
    EXPECT( &*p1 == raw2 );
    EXPECT( &*p2 == raw1 );
}

CASE( "not_null<>: Allows to construct from a non-null underlying pointer (unique_ptr)" )
{
    unique_ptr< int > pi = my_make_unique< int >(12);
    int* raw(pi.get());
    not_null< unique_ptr< int > > p( std::move(pi) );

    EXPECT( &*p == raw );
}

CASE( "not_null<>: Allows to construct from a non-null raw pointer with explicit conversion (unique_ptr)" )
{
    int* i = new int(12);
    not_null< unique_ptr< int > > p( i );
}

CASE( "not_null<>: Returns underlying pointer or raw pointer with get() (unique_ptr)" )
{
#if gsl_CONFIG( TRANSPARENT_NOT_NULL ) || gsl_CONFIG( NOT_NULL_GET_BY_CONST_REF )
    unique_ptr< int > pi = my_make_unique< int >(12);
    int* raw(pi.get());
    not_null< unique_ptr< int > > p( std::move(pi) );

# if gsl_CONFIG( TRANSPARENT_NOT_NULL )
    int* pg = p.get();
    EXPECT( pg == raw );
# else // gsl_CONFIG( NOT_NULL_GET_BY_CONST_REF )
    int* pg = p.get().get();
    EXPECT( pg == raw );
# endif
#endif
}

CASE( "not_null<>: Allows to move from a not_null pointer to an underlying pointer (unique_ptr)" )
{
#if gsl_HAVE( FUNCTION_REF_QUALIFIER )
    unique_ptr< int > pi = my_make_unique< int >(12);
    int* raw(pi.get());

    not_null< unique_ptr< int > > p ( std::move(pi) ); // There...
    pi = std::move(p); // ...and back again.

    EXPECT_THROWS( (void) *p );
    EXPECT( pi.get() == raw );
#endif
}

CASE( "as_nullable: Allows to move from a not_null pointer to an underlying pointer (unique_ptr)" )
{
    unique_ptr< int > pi = my_make_unique< int >(12);
    int* raw(pi.get());

    not_null< unique_ptr< int > > p ( std::move(pi) ); // There...
    pi = as_nullable( std::move(p) ); // ...and back again.

    EXPECT_THROWS( (void) *p );
    EXPECT( pi.get() == raw );
}

CASE( "not_null<>: Allows to move to a related pointer from a not_null pointer (unique_ptr)" )
{
#if gsl_HAVE( FUNCTION_REF_QUALIFIER )
    unique_ptr< MyDerived > pderived = my_make_unique< MyDerived >();
    MyDerived* raw(pderived.get());
    not_null< unique_ptr< MyDerived > > p ( std::move(pderived) );
    unique_ptr< MyBase > pbase = std::move(p);

    EXPECT_THROWS( (void) *p );
    EXPECT( pbase.get() == raw );
#endif
}

CASE( "as_nullable: Allows to move to a related pointer from a not_null pointer (unique_ptr)" )
{
    unique_ptr< MyDerived > pderived = my_make_unique< MyDerived >();
    MyDerived* raw(pderived.get());
    not_null< unique_ptr< MyDerived > > p ( std::move(pderived) );
    unique_ptr< MyBase > pbase = as_nullable( std::move(p) );

    EXPECT_THROWS( (void) *p );
    EXPECT( pbase.get() == raw );
}

CASE( "not_null<>: Allows to construct from a non-null underlying pointer (unique_ptr) with make_not_null()" )
{
    unique_ptr< int > pi = my_make_unique< int >(12);
    int* raw(pi.get());

    not_null< unique_ptr< int > > p = make_not_null( std::move(pi) );

    EXPECT( &*p == raw );
}

CASE( "not_null<>: Allows to construct from a non-null underlying pointer (unique_ptr) with deduction guide" )
{
#if gsl_HAVE( DEDUCTION_GUIDES )
    unique_ptr< int > pi = my_make_unique< int >(12);
    int* raw(pi.get());

    not_null p( std::move(pi) );

    EXPECT( &*p == raw );
#endif
}

CASE( "not_null<>: Allows to construct a const pointer from a non-null underlying pointer (unique_ptr)" )
{
    unique_ptr< int > pi = my_make_unique< int >(12);
    int* raw(pi.get());
    not_null< unique_ptr< const int > > p( std::move(pi) );

    EXPECT( &*p == raw );
}

CASE( "not_null<>: Converts to underlying pointer (unique_ptr)" )
{
#if gsl_HAVE( FUNCTION_REF_QUALIFIER )
    unique_ptr< int > pi = my_make_unique< int >();
    not_null< unique_ptr< int > > p( std::move(pi) );

    take_unique_by_val<int>( std::move(p) );
    //take_unique_by_ref<int>( p ); // We sacrifice the ability to convert to `unique_ptr<> const &`, cf. comment regarding conversion operators in gsl-lite.hpp.
#endif
}

CASE( "as_nullable: Converts to underlying pointer (unique_ptr)" )
{
    unique_ptr< int > pi = my_make_unique< int >();
    not_null< unique_ptr< int > > p( std::move(pi) );

    take_unique_by_ref<int>( as_nullable( p ) );
    take_unique_by_val<int>( as_nullable( std::move( p ) ) );
}

CASE( "as_nullable: Terminates for moved-from pointer (unique_ptr)" )
{
    unique_ptr< int > pi = my_make_unique< int >();
    not_null< unique_ptr< int > > p( std::move( pi ) );
    not_null< unique_ptr< int > > p2( std::move( p ) );

    EXPECT_THROWS( (void) as_nullable( p ) );
}

CASE( "not_null<>: Allows to construct from a non-null related pointer (unique_ptr)" )
{
    unique_ptr< MyDerived > pderived = my_make_unique< MyDerived >();
    MyDerived* raw(pderived.get());
    not_null< unique_ptr< MyBase > > p( std::move(pderived) );

    EXPECT( &*p == raw );
}

CASE( "not_null<>: Allows to construct a const pointer from a non-null related pointer (unique_ptr)" )
{
    unique_ptr< MyDerived > pderived = my_make_unique< MyDerived >();
    MyDerived* raw(pderived.get());
    not_null< unique_ptr< const MyBase > > p( std::move(pderived) );

    EXPECT( &*p == raw );
}

CASE( "not_null<>: Allows to construct from a not_null related pointer type (unique_ptr)" )
{
    unique_ptr< MyDerived > pderived = my_make_unique< MyDerived >();
    MyDerived* raw(pderived.get());
    not_null< unique_ptr< MyDerived > > p( std::move(pderived) );

#if gsl_CPP11_OR_GREATER
    not_null< unique_ptr< MyBase > > q = std::move(p);
#else
    not_null< unique_ptr< MyBase > > q(std::move(p)); // in C++98, we cannot differentiate between implicit and explicit cases, so conversion is always explicit
#endif

    EXPECT( &*q == raw );
}

CASE( "not_null<>: Allows to construct a const pointer from a not_null related pointer type (unique_ptr)" )
{
    unique_ptr< MyDerived > pderived = my_make_unique< MyDerived >();
    MyDerived* raw(pderived.get());
    not_null< unique_ptr< MyDerived > > p( std::move(pderived) );

#if gsl_CPP11_OR_GREATER
    not_null< unique_ptr< const MyBase > > q = std::move(p);
#else
    not_null< unique_ptr< const MyBase > > q(std::move(p)); // in C++98, we cannot differentiate between implicit and explicit cases, so conversion is always explicit
#endif

    EXPECT( &*q == raw );
}

CASE( "not_null<>: Converts to a related pointer (unique_ptr)" )
{
#if gsl_HAVE( FUNCTION_REF_QUALIFIER )
    unique_ptr< MyDerived > pderived = my_make_unique< MyDerived >();
    not_null< unique_ptr< MyDerived > > p( std::move(pderived) );

    take_unique_by_val<MyBase>( std::move(p) );
    //take_unique_by_ref<MyBase>( p ); // We sacrifice the ability to convert to `unique_ptr<> const &`, cf. comment regarding conversion operators in gsl-lite.hpp.
#endif
}

CASE( "as_nullable: Converts to a related pointer (unique_ptr)" )
{
    unique_ptr< MyDerived > pderived = my_make_unique< MyDerived >();
    not_null< unique_ptr< MyDerived > > p( std::move(pderived) );

    take_unique_by_val<MyBase>( as_nullable( std::move(p) ) );
}

CASE( "not_null<>: Allows assignment from a not_null related pointer type (unique_ptr)" )
{
    unique_ptr< MyDerived > pderived = my_make_unique< MyDerived >();
	MyDerived* raw(pderived.get());
    not_null< unique_ptr< MyDerived > > p( std::move(pderived) );
    not_null< unique_ptr< MyBase >    > q( my_make_unique< MyBase >() );

    q = std::move(p);

    EXPECT( &*q == raw );
}

CASE( "not_null<>: Allows assignment to a const pointer from a not_null related pointer type (unique_ptr)" )
{
    unique_ptr< MyDerived > pderived = my_make_unique< MyDerived >();
	MyDerived* raw(pderived.get());
    not_null< unique_ptr< MyDerived    > > p( std::move(pderived) );
    not_null< unique_ptr< const MyBase > > q( my_make_unique< MyBase >() );

    q = std::move(p);

    EXPECT( &*q == raw );
}

CASE( "not_null<>: Allows indirect member access (unique_ptr)" )
{
    using namespace nonlocal;
    S s = { 'a', 7 };
    not_null< unique_ptr< S > > p( my_make_unique< S >(s) );

    EXPECT( p->c == 'a' );
    EXPECT( p->i ==  7  );
}

CASE( "not_null<>: Allows dereferencing (unique_ptr)" )
{
    int i = 12;
    unique_ptr< int > pi = my_make_unique< int >(12);
    not_null< unique_ptr< int > > p( std::move(pi) );

    EXPECT( *p == i );
}

CASE( "not_null<>: Allows to check whether object is valid (unique_ptr)" )
{
    unique_ptr< int > pi = my_make_unique< int >( 12 );
    not_null< unique_ptr< int > > p( std::move(pi) );

    EXPECT( gsl::is_valid( p ) );

    not_null< unique_ptr< int > > q( std::move( p ) );

    EXPECT_NOT( gsl::is_valid( p ) );
    EXPECT(     gsl::is_valid( q ) );
}

#endif // gsl_HAVE( UNIQUE_PTR )

#if gsl_HAVE( UNIQUE_PTR ) && gsl_HAVE( SHARED_PTR )

CASE( "not_null<>: Allows to construct a not_null<shared_ptr<T>> from a non-null unique_ptr<T>" )
{
    unique_ptr< int > pi = my_make_unique< int >(12);
    int* raw(pi.get());
    not_null< shared_ptr< int > > p( std::move(pi) );

    EXPECT( &*p == raw );
}

CASE( "not_null<>: Allows to construct a not_null<shared_ptr<const T>> from a non-null unique_ptr<T>" )
{
    unique_ptr< int > pi = my_make_unique< int >(12);
    int* raw(pi.get());
    not_null< shared_ptr< const int > > p( std::move(pi) );

    EXPECT( &*p == raw );
}

CASE( "not_null<>: Allows to construct a not_null<shared_ptr<T>> from a related non-null unique_ptr<U>" )
{
    unique_ptr< MyDerived > pderived = my_make_unique< MyDerived >();
    MyDerived* raw(pderived.get());
    not_null< shared_ptr< MyBase > > p( std::move(pderived) );

    EXPECT( &*p == raw );
}

CASE( "not_null<>: Allows to construct a not_null<shared_ptr<const T>> from a related non-null unique_ptr<U>" )
{
    unique_ptr< MyDerived > pderived = my_make_unique< MyDerived >();
    MyDerived* raw(pderived.get());
    not_null< shared_ptr< const MyBase > > p( std::move(pderived) );

    EXPECT( &*p == raw );
}

CASE( "not_null<>: Allows to construct a not_null<shared_ptr<T>> from a not_null<unique_ptr<T>>" )
{
    unique_ptr< int > pi = my_make_unique< int >(12);
    int* raw(pi.get());
    not_null< unique_ptr< int > > p( std::move(pi) );

#if gsl_CPP11_OR_GREATER
    not_null< shared_ptr< int > > q = std::move(p);
#else
    not_null< shared_ptr< int > > q(std::move(p)); // in C++98, we cannot differentiate between implicit and explicit cases, so conversion is always explicit
#endif

    EXPECT( &*q == raw );
}

CASE( "not_null<>: Allows to convert to weak_ptr<T> from a not_null<shared_ptr<T>>" )
{
    shared_ptr< int > pi = std::make_shared< int >(12);
    int* raw(pi.get());
    not_null< shared_ptr< int > > p( std::move(pi) );

#if gsl_CPP11_OR_GREATER
    std::weak_ptr< int > q = p;
    std::vector< std::weak_ptr< int > > v;
    v.emplace_back( p );
#else
    std::weak_ptr< int > q(p); // in C++98, we cannot differentiate between implicit and explicit cases, so conversion is always explicit
#endif

    EXPECT_NOT( q.expired() );
    EXPECT( q.lock().get() == raw );
}

// C++14 has ambiguity issues with constructors and conversion operators that were fixed in C++17,
// cf. https://github.com/gsl-lite/gsl-lite/issues/275#issuecomment-678640600.
// Also, Clang ≤ 5 may pretend to support C++17 but it still suffers from the conversion ambiguity.
#if gsl_CPP17_OR_GREATER && ! ( gsl_BETWEEN(gsl_COMPILER_CLANG_VERSION, 1, 600) || gsl_BETWEEN(gsl_COMPILER_APPLECLANG_VERSION, 1, 1000) )
CASE( "not_null<>: Allows to convert from a not_null<shared_ptr<T>> to a user-defined type with explicit conversion constructor" )
{
    shared_ptr< int > pi = std::make_shared< int >(12);
    not_null< shared_ptr< int > > p( std::move(pi) );

    ExplicitFromShared< int > q(p);
    (void) q;
}
#endif // gsl_CPP17_OR_GREATER

CASE( "not_null<>: Allows to construct a not_null<shared_ptr<const T>> from a not_null<unique_ptr<T>>" )
{
    unique_ptr< int > pi = my_make_unique< int >(12);
    int* raw(pi.get());
    not_null< unique_ptr< int > > p( std::move(pi) );

#if gsl_CPP11_OR_GREATER
    not_null< shared_ptr< const int > > q = std::move(p);
#else
    not_null< shared_ptr< const int > > q(std::move(p)); // in C++98, we cannot differentiate between implicit and explicit cases, so conversion is always explicit
#endif

    EXPECT( &*q == raw );
}

CASE( "not_null<>: Allows to construct a not_null<shared_ptr<T>> from a related not_null<unique_ptr<U>>" )
{
    unique_ptr< MyDerived > pderived = my_make_unique< MyDerived >();
    MyDerived* raw(pderived.get());
    not_null< unique_ptr< MyDerived > > p( std::move(pderived) );

#if gsl_CPP11_OR_GREATER
    not_null< shared_ptr< MyBase > > q = std::move(p);
#else
    not_null< shared_ptr< MyBase > > q(std::move(p)); // in C++98, we cannot differentiate between implicit and explicit cases, so conversion is always explicit
#endif

    EXPECT( &*q == raw );
}

CASE( "not_null<>: Allows to construct a not_null<shared_ptr<const T>> from a related not_null<unique_ptr<U>>" )
{
    unique_ptr< MyDerived > pderived = my_make_unique< MyDerived >();
    MyDerived* raw(pderived.get());
    not_null< unique_ptr< MyDerived > > p( std::move(pderived) );

#if gsl_CPP11_OR_GREATER
    not_null< shared_ptr< const MyBase > > q = std::move(p);
#else
    not_null< shared_ptr< const MyBase > > q(std::move(p)); // in C++98, we cannot differentiate between implicit and explicit cases, so conversion is always explicit
#endif

    EXPECT( &*q == raw );
}

CASE( "not_null<>: Allows assignment to a not_null<shared_ptr<T>> from a related not_null<unique_ptr<U>>" )
{
    unique_ptr< MyDerived > pderived = my_make_unique< MyDerived >();
	MyDerived* raw(pderived.get());
    not_null< unique_ptr< MyDerived > > p( std::move(pderived) );
    not_null< shared_ptr< MyBase >    > q( std::make_shared< MyBase >() );

    q = std::move(p);

    EXPECT( &*q == raw );
}

CASE( "not_null<>: Allows assignment to a not_null<shared_ptr<const T>> from a related not_null<unique_ptr<U>>" )
{
    unique_ptr< MyDerived > pderived = my_make_unique< MyDerived >();
	MyDerived* raw(pderived.get());
    not_null< unique_ptr< MyDerived    > > p( std::move(pderived) );
    not_null< shared_ptr< const MyBase > > q( std::make_shared< MyBase >() );

    q = std::move(p);

    EXPECT( &*q == raw );
}

# if gsl_HAVE( VARIADIC_TEMPLATE )
#  if !gsl_BETWEEN( gsl_COMPILER_MSVC_VERSION, 120, 130 )  // VS 2013
CASE( "not_null<>: make_unique<T>() returns not_null<unique_ptr<T>>" )
{
    not_null< unique_ptr< int > > p = make_unique< int >();
    unique_ptr< int > q = std::move( p );
    (void) q;

    EXPECT( q.get() != nullptr );

}
#  endif // !gsl_BETWEEN( gsl_COMPILER_MSVC_VERSION, 120, 130 )
CASE( "not_null<>: make_shared<T>() returns not_null<shared_ptr<T>>" )
{
    not_null< shared_ptr< int > > p = make_shared< int >();
    shared_ptr< int > q = std::move( p );
    (void) q;

    EXPECT( q.get() != nullptr );
}
# endif // gsl_HAVE( VARIADIC_TEMPLATE )

#endif // gsl_HAVE( UNIQUE_PTR ) && gsl_HAVE( SHARED_PTR )

CASE( "not_null<>: Allows assignment from a non-null bare recast pointer" )
{
    MyDerived derived;
    not_null< MyDerived* > p( &derived );

    not_null< Unrelated* > t( reinterpret_cast< Unrelated* >( &*p ) );

    EXPECT( &*p == reinterpret_cast< MyDerived* >( &*t ) );
}

CASE( "not_null<>: Allows implicit conversion to underlying type" )
{
    struct F { static bool helper( not_null< int* > p ) { return *p == 12; } };

    int i = 12;
    not_null< int* > p( &i );

    EXPECT( F::helper( p ) );
}

CASE( "not_null<>: Allows to construct from a non-null user-defined ref-counted type" )
{
    int i = 12;
    RefCounted< int > rp = RefCounted<int>( &i );

    not_null<   int*>  p( rp );

    EXPECT( p == &i );
}

namespace {

// provide not_null pointers to compare:

struct NotNull
{
    int    vs[2];
    int * pv1; int const * pv2;

    NotNull()
    : pv1( &vs[ 0 ] ), pv2( &vs[ 1 ] )
    {
        vs[0] = 7;
        vs[1] = 42;
    }

    not_null< int       * > p1() const { return not_null< int       * >( pv1 ); }
    not_null< int const * > p2() const { return not_null< int const * >( pv2 ); }
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

// user-defined types to check element_type deduction

template<class T>
struct NormalPtr
{
    typedef T element_type;
    T* ptr;
    T& operator*() { return *ptr; }
    NormalPtr(T* ptr) : ptr(ptr) {}
};

template<class T>
struct ETLessNormalPtr
{
    // no element_type typedef
    T* ptr;
    T& operator*() { return *ptr; }
    ETLessNormalPtr(T* ptr) : ptr(ptr) {}
};

template<class T>
struct WeirdPtr // element_type and *p's type differ
{
    typedef T element_type;
    T* ptr;
    double operator*() { return *ptr; }
    WeirdPtr(T* ptr) : ptr(ptr) {}
};

template<class T>
struct ETLessWeirdPtr // // element_type and type T differ
{
    // no element_type typedef
    T* ptr;
    double operator*() { return *ptr; }
    ETLessWeirdPtr(T* ptr) : ptr(ptr) {}
}; 

#if gsl_HAVE( TYPE_TRAITS )
using std::is_same;
#else
template<class T, class U>
struct is_same { enum dummy { value = false }; };
 
template<class T>
struct is_same<T, T> { enum dummy { value = true }; };
#endif

CASE( "not_null<>: Able to deduce element_type of raw pointers" )
{
    EXPECT(( is_same< gsl::not_null< int* >::element_type, int >::value ));
    EXPECT(( is_same< gsl::not_null< const int* >::element_type, const int >::value ));
}

CASE( "not_null<>: Able to deduce element_type of unique_ptr" )
{
#if gsl_HAVE( UNIQUE_PTR )
    EXPECT(( is_same< gsl::not_null< std::unique_ptr< int > >::element_type, int >::value ));
    EXPECT(( is_same< gsl::not_null< std::unique_ptr< const int > >::element_type, const int >::value ));
#endif
}

CASE( "not_null<>: Able to deduce element_type of shared_ptr" )
{
#if gsl_HAVE( SHARED_PTR )
    EXPECT(( is_same< gsl::not_null< std::shared_ptr< int > >::element_type, int >::value ));
    EXPECT(( is_same< gsl::not_null< std::shared_ptr< const int > >::element_type, const int >::value ));
#endif
}

CASE( "not_null<>: Able to deduce element_type of normal user-defined smart pointers" )
{
    EXPECT(( is_same< gsl::not_null< NormalPtr< int > >::element_type, int >::value ));
    EXPECT(( is_same< gsl::not_null< NormalPtr< const int > >::element_type, const int >::value ));
}

CASE( "not_null<>: Able to correctly deduce element_type of user-defined smart pointers even if typedef and result of dereferencing differs" )
{
    EXPECT(( is_same< gsl::not_null< WeirdPtr< int > >::element_type, int >::value ));
    EXPECT(( is_same< gsl::not_null< WeirdPtr< const int > >::element_type, const int >::value ));
}

CASE( "not_null<>: Able to deduce element_type of user-defined smart pointers even if they do not have an element_type typedef" )
{
#if gsl_CPP11_OR_GREATER
    EXPECT(( is_same< gsl::not_null< ETLessNormalPtr< int > >::element_type, int >::value ));
    EXPECT(( is_same< gsl::not_null< ETLessNormalPtr< const int > >::element_type, const int >::value ));
#endif
}

CASE( "not_null<>: Able to deduce element_type of user-defined smart pointers even if they do not have an element_type typedef, and element_type differs from T" )
{
#if gsl_CPP11_OR_GREATER
    EXPECT(( is_same< gsl::not_null< ETLessWeirdPtr< int > >::element_type, double >::value ));
    EXPECT(( is_same< gsl::not_null< ETLessWeirdPtr< const int > >::element_type, double >::value ));
#endif
}

CASE( "not_null<>: Can handle void*" )
{
    int i = 42;
    void * vp = &i;

        // Direct construction
    gsl::not_null< void * > nvp( vp );
    EXPECT( nvp == vp );

        // Indirect construction
    nvp = gsl::make_not_null( vp );
    EXPECT( nvp == vp );

        // Implicit conversion from `not_null<>` with typed pointer argument
    gsl::not_null< int * > nip( &i );
    EXPECT( nvp == nip );
    gsl::not_null< void * > nvp2( nip );
    EXPECT( nvp2 == nip );

        // Implicit conversion from typed pointer
    EXPECT( nvp == &i );
    gsl::not_null< void * > nvp3( &i );
    EXPECT( nvp3 == nip );

        // Extract underlying value
    void * vp2 = gsl::as_nullable( nvp );
    EXPECT( vp2 == vp );

        // Explicit conversion to typed pointer argument is not supported!
    //gsl::not_null< int * > nip2 = static_cast< gsl::not_null< int * > >( nvp );
    //gsl::not_null< int * > nip2 = gsl::not_null< int * >( nvp );
    //int * ip = static_cast< int * >( nvp );
}

void int_const_deleter( void const * p )
{
    int const * pi = static_cast< int const * >( p );
    delete pi;
}

CASE( "not_null<>: Can handle unique_ptr<void, DeleterT>" )
{
#if gsl_CPP11_OR_GREATER
    std::unique_ptr< void, void ( * )( void const * ) > vp( new int( 42 ), int_const_deleter );
    void * p = vp.get();
    ( void ) p;

        // Direct construction
    gsl::not_null< std::unique_ptr< void, void ( * )( void const * ) > > nvp( std::move( vp ) );
# if gsl_CONFIG( TRANSPARENT_NOT_NULL )
    EXPECT( nvp.get() == p );
# endif // gsl_CONFIG( TRANSPARENT_NOT_NULL )
    vp = std::move( nvp );

        // Indirect construction
    nvp = gsl::make_not_null( std::move( vp ) );
# if gsl_CONFIG( TRANSPARENT_NOT_NULL )
    EXPECT( nvp.get() == p );
# endif // gsl_CONFIG( TRANSPARENT_NOT_NULL )

        // Implicit conversion from `not_null<>` with typed pointer argument
    std::unique_ptr< int, void ( * )( void const * ) > tp2( new int( 42 ), int_const_deleter );
    int * pi2 = tp2.get();
    ( void ) pi2;
    gsl::not_null< std::unique_ptr< int, void ( * )( void const * ) > > ntp2( std::move( tp2 ) );
# if gsl_CONFIG( TRANSPARENT_NOT_NULL )
    EXPECT( ntp2.get() == pi2 );
# endif // gsl_CONFIG( TRANSPARENT_NOT_NULL )
    gsl::not_null< std::unique_ptr< void, void ( * )( void const * ) > > nvp2( std::move( ntp2 ) );
# if gsl_CONFIG( TRANSPARENT_NOT_NULL )
    EXPECT( nvp2.get() == pi2 );
# endif // gsl_CONFIG( TRANSPARENT_NOT_NULL )

        // Implicit conversion from typed pointer
    std::unique_ptr< int, void ( * )( void const * ) > tp3( new int( 42 ), int_const_deleter );
    int * pi3 = tp3.get();
    gsl::not_null< std::unique_ptr< void, void ( * )( void const * ) > > nvp3( std::move( tp3 ) );
# if gsl_CONFIG( TRANSPARENT_NOT_NULL )
    EXPECT( nvp3.get() == pi3 );
# endif // gsl_CONFIG( TRANSPARENT_NOT_NULL )

        // Extract underlying value
    std::unique_ptr< void, void ( * )( void const * ) > vp3 = gsl::as_nullable( std::move( nvp3 ) );
    EXPECT( vp3.get() == pi3 );

        // Explicit conversion to typed pointer argument is not supported!
#endif
}

CASE( "not_null<>: Can handle shared_ptr<void>" )
{
#if gsl_CPP11_OR_GREATER
    std::shared_ptr< void > vp = std::make_shared<int>( 42 );
    void * p = vp.get();
    ( void ) p;

        // Direct construction
    gsl::not_null< std::shared_ptr< void > > nvp( vp );
# if gsl_CONFIG( TRANSPARENT_NOT_NULL )
    EXPECT( nvp.get() == p );
# endif // gsl_CONFIG( TRANSPARENT_NOT_NULL )
    vp = std::move( nvp );

        // Indirect construction
    nvp = gsl::make_not_null( vp );
# if gsl_CONFIG( TRANSPARENT_NOT_NULL )
    EXPECT( nvp.get() == p );
# endif // gsl_CONFIG( TRANSPARENT_NOT_NULL )

        // Implicit conversion from `not_null<>` with typed pointer argument
    std::shared_ptr< int > tp2 = std::make_shared<int>( 42 );
    int * pi2 = tp2.get();
    gsl::not_null< std::shared_ptr< int > > ntp2( tp2 );
# if gsl_CONFIG( TRANSPARENT_NOT_NULL )
    EXPECT( ntp2.get() == pi2 );
# endif // gsl_CONFIG( TRANSPARENT_NOT_NULL )
    gsl::not_null< std::shared_ptr< void > > nvp2( ntp2 );
# if gsl_CONFIG( TRANSPARENT_NOT_NULL )
    EXPECT( nvp2.get() == pi2 );
# endif // gsl_CONFIG( TRANSPARENT_NOT_NULL )

        // Implicit conversion from typed pointer
    gsl::not_null< std::shared_ptr< void > > nvp2b( tp2 );
# if gsl_CONFIG( TRANSPARENT_NOT_NULL )
    EXPECT( nvp2b.get() == pi2 );
# endif // gsl_CONFIG( TRANSPARENT_NOT_NULL )

        // Extract underlying value
    std::shared_ptr< void > vp2 = gsl::as_nullable( nvp2 );
    EXPECT( vp2.get() == pi2 );

        // Explicit conversion to typed pointer argument is not supported!
#endif
}

#if gsl_HAVE( HASH )

CASE( "not_null<>: Hashes match the hashes of the wrapped pointer" )
{
    int i = 42;
    not_null< const int* > raw_pointer = make_not_null( &i );
    EXPECT( std::hash< not_null< const int* > >()(raw_pointer) == std::hash< const int* >()( as_nullable( raw_pointer ) ) );
    not_null< std::unique_ptr< int > > unique_pointer = make_not_null( my_make_unique< int >(43) );
    EXPECT( std::hash< not_null< std::unique_ptr< int > > >()(unique_pointer) == std::hash< std::unique_ptr< int > >()( as_nullable( unique_pointer) ) );
    not_null< std::shared_ptr< int > > shared_pointer = make_not_null( std::make_shared< int >(43) );
    EXPECT( std::hash< not_null< std::shared_ptr< int > > >()(shared_pointer) == std::hash< std::shared_ptr< int > >()( as_nullable( shared_pointer) ) );
}

CASE( "not_null<>: Hash functor disabled for non-hashable pointers and enabled for hashable pointers" )
{
    EXPECT(( std::is_default_constructible< std::hash< not_null< int* > > >::value ));
    EXPECT(( std::is_default_constructible< std::hash< not_null< std::unique_ptr< int > > > >::value ));
    EXPECT(( std::is_default_constructible< std::hash< not_null< std::shared_ptr< int > > > >::value ));
# if gsl_STDLIB_CPP14_OR_GREATER && ! gsl_BETWEEN( gsl_COMPILER_MSVC_VERSION, 1, 141 ) && ! gsl_BETWEEN( gsl_COMPILER_APPLECLANG_VERSION, 1, 900 )
    // std::hash< NormalPtr< int > > isn't guaranteed to exist at all in C++11
    EXPECT_NOT( ( std::is_default_constructible< std::hash< not_null< NormalPtr< int > > > >::value ) );
# endif
}

#endif // gsl_HAVE( HASH )

// end of file
