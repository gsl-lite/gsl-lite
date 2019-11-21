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
    static_assert( !std::is_assignable<    not_null< MyBase* >, MyDerived* >::value, "static assertion failed" );
# endif

    // Do not permit conversion to subclass pointer.
    static_assert( !std::is_constructible< MyDerived*, not_null< MyBase* > >::value, "static assertion failed" );
    static_assert( !std::is_assignable<    MyDerived*, not_null< MyBase* > >::value, "static assertion failed" );

    // Permit construction and assignment from subclass pointer.
    static_assert(  std::is_constructible< not_null< std::unique_ptr< MyBase > >, std::unique_ptr< MyDerived > >::value, "static assertion failed" );
# if gsl_CONFIG( NOT_NULL_EXPLICIT_CTOR ) && !gsl_BETWEEN( gsl_COMPILER_APPLECLANG_VERSION, 1, 900 )
    static_assert( !std::is_assignable<    not_null< std::unique_ptr< MyBase > >, std::unique_ptr< MyDerived > >::value, "static assertion failed" );
# endif

    // Do not permit copy construction and assignment from move-only subclass pointer.
    static_assert( !std::is_constructible< not_null< std::unique_ptr< MyBase > >, std::unique_ptr< MyDerived > const & >::value, "static assertion failed" );
# if !gsl_BETWEEN( gsl_COMPILER_APPLECLANG_VERSION, 1, 900 )
    static_assert( !std::is_assignable<    not_null< std::unique_ptr< MyBase > >, std::unique_ptr< MyDerived > const & >::value, "static assertion failed" );
# endif

    // Permit construction and assignment from `not_null<>` with subclass pointer.
    static_assert(  std::is_constructible< not_null< std::unique_ptr< MyBase > >, not_null< std::unique_ptr< MyDerived > > >::value, "static assertion failed" );
    static_assert(  std::is_assignable<    not_null< std::unique_ptr< MyBase > >, not_null< std::unique_ptr< MyDerived > > >::value, "static assertion failed" );

    // Do not permit copy construction and assignment from move-only `not_null<>` with subclass pointer.
    static_assert( !std::is_constructible< not_null< std::unique_ptr< MyBase > >, not_null< std::unique_ptr< MyDerived > > const & >::value, "static assertion failed" );
# if !gsl_BETWEEN( gsl_COMPILER_APPLECLANG_VERSION, 1, 900 )
    static_assert( !std::is_assignable<    not_null< std::unique_ptr< MyBase > >, not_null< std::unique_ptr< MyDerived > > const & >::value, "static assertion failed" );
# endif

    // Permit conversion to superclass pointer.
# if gsl_HAVE( FUNCTION_REF_QUALIFIER )
    static_assert(  std::is_constructible< std::unique_ptr< MyBase >, not_null< std::unique_ptr< MyDerived > > >::value, "static assertion failed" );
    static_assert(  std::is_assignable<    std::unique_ptr< MyBase >, not_null< std::unique_ptr< MyDerived > > >::value, "static assertion failed" );
# endif

    // Do not permit conversion with copy to move-only superclass pointer.
    static_assert( !std::is_constructible< std::unique_ptr< MyBase >, not_null< std::unique_ptr< MyDerived > > const & >::value, "static assertion failed" );
# if !gsl_BETWEEN( gsl_COMPILER_APPLECLANG_VERSION, 1, 900 )
    static_assert( !std::is_assignable<    std::unique_ptr< MyBase >, not_null< std::unique_ptr< MyDerived > > const & >::value, "static assertion failed" );
# endif

    // Do not permit construction and assignment from superclass pointer.
# if !gsl_BETWEEN( gsl_COMPILER_CLANG_VERSION, 1, 400 ) && !gsl_BETWEEN( gsl_COMPILER_APPLECLANG_VERSION, 1, 1001 )
    static_assert( !std::is_constructible< not_null< std::unique_ptr< MyDerived > >, std::unique_ptr< MyBase > >::value, "static assertion failed" );
    static_assert( !std::is_assignable<    not_null< std::unique_ptr< MyDerived > >, std::unique_ptr< MyBase > >::value, "static assertion failed" );

    static_assert( !std::is_constructible< not_null< std::unique_ptr< MyDerived > >, not_null< std::unique_ptr< MyBase > > >::value, "static assertion failed" );
    static_assert( !std::is_assignable<    not_null< std::unique_ptr< MyDerived > >, not_null< std::unique_ptr< MyBase > > >::value, "static assertion failed" );
# endif

    // Do not conversion to subclass pointer.
    static_assert( !std::is_constructible< std::unique_ptr< MyDerived >, not_null< std::unique_ptr< MyBase > > >::value, "static assertion failed" );
    static_assert( !std::is_assignable<    std::unique_ptr< MyDerived >, not_null< std::unique_ptr< MyBase > > >::value, "static assertion failed" );
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

#if gsl_HAVE( SHARED_PTR )
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
    not_null< shared_ptr< int > > p( make_shared< int >(12) );
    shared_ptr< int > z = gsl_nullptr;

    EXPECT_THROWS( p = not_null< shared_ptr< int > >( z ) );
}

CASE( "not_null<>: Terminates assignment from related pointer types for null pointer value (shared_ptr)" )
{
    shared_ptr< MyDerived >  z = gsl_nullptr;
    not_null< shared_ptr< MyBase > > p( make_shared< MyDerived >() );

    EXPECT_THROWS( p = not_null< shared_ptr< MyDerived > >( z ) );
}

CASE( "not_null<>: Terminates propagation of a moved-from value (shared_ptr)" )
{
    shared_ptr< int > pi = make_shared< int >(12);
    not_null< shared_ptr< int > > p( std::move( pi ) );
    not_null< shared_ptr< int > > q( std::move( p ) );

    EXPECT_THROWS( not_null< shared_ptr< int > > v( p ) );
    EXPECT_THROWS( not_null< shared_ptr< int > > v( std::move( p ) ) );
    EXPECT_THROWS( q = p );
    EXPECT_THROWS( q = std::move( p ) );
}

CASE( "not_null<>: Allows to construct from a non-null underlying pointer (shared_ptr)" )
{
    shared_ptr< int > pi = make_shared< int >(12);
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
    shared_ptr< int > pi = make_shared< int >(12);
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
    shared_ptr< int > pi = make_shared< int >(12);
    int* raw(pi.get());

    not_null< shared_ptr< int > > p ( std::move(pi) ); // There...
    pi = std::move(p); // ...and back again.

    EXPECT_THROWS( *p );
    EXPECT( pi.get() == raw );
#endif
}

CASE( "not_null<>: Allows to construct from a non-null underlying pointer (shared_ptr) with make_not_null()" )
{
    shared_ptr< int > pi = make_shared< int >(12);

    not_null< shared_ptr< int > > p = make_not_null( pi );

    EXPECT( p == pi );
}

CASE( "not_null<>: Allows to construct from a non-null underlying pointer (shared_ptr) with deduction guide" )
{
#if gsl_HAVE( DEDUCTION_GUIDES )
    shared_ptr< int > pi = make_shared< int >(12);

    not_null p( pi );

    EXPECT( p == pi );
#endif
}

CASE( "not_null<>: Allows to construct a const pointer from a non-null underlying pointer (shared_ptr)" )
{
    shared_ptr< int > pi = make_shared< int >(12);
    not_null< shared_ptr< const int > > p( pi );

    EXPECT( p == pi );
}

CASE( "not_null<>: Converts to underlying pointer (shared_ptr)" )
{
    shared_ptr< int > pi = make_shared< int >();
    not_null< shared_ptr< int > > p( pi );

    take_shared_by_val<int>( p );
    take_shared_by_ref<int>( p );
}

CASE( "not_null<>: Allows to construct from a non-null related pointer (shared_ptr)" )
{
    shared_ptr< MyDerived > pderived = make_shared< MyDerived >();
    not_null< shared_ptr< MyBase > > p( pderived );

    EXPECT( p == pderived );
}

CASE( "not_null<>: Allows to construct a const pointer from a non-null related pointer (shared_ptr)" )
{
    shared_ptr< MyDerived > pderived = make_shared< MyDerived >();
    not_null< shared_ptr< const MyBase > > p( pderived );

    EXPECT( p == pderived );
}

CASE( "not_null<>: Allows to construct from a not_null related pointer type (shared_ptr)" )
{
    shared_ptr< MyDerived > pderived = make_shared< MyDerived >();
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
    shared_ptr< MyDerived > pderived = make_shared< MyDerived >();
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
    shared_ptr< MyDerived > pderived = make_shared< MyDerived >();
    not_null< shared_ptr< MyDerived > > p( pderived );

    take_shared_by_val<MyBase>( p );
    take_shared_by_ref<MyBase>( p );
}

CASE( "not_null<>: Allows assignment from a not_null related pointer type (shared_ptr)" )
{
    shared_ptr< MyDerived > pderived = make_shared< MyDerived >();
    not_null< shared_ptr< MyDerived > > p( pderived );
    not_null< shared_ptr< MyBase >    > q( p );

    q = p;

    EXPECT( q == p );
}

CASE( "not_null<>: Allows assignment to a const pointer from a not_null related pointer type (shared_ptr)" )
{
    shared_ptr< MyDerived > pderived = make_shared< MyDerived >();
    not_null< shared_ptr< MyDerived    > > p( pderived );
    not_null< shared_ptr< const MyBase > > q( p );

    q = p;

    EXPECT( q == p );
}

CASE( "not_null<>: Allows indirect member access (shared_ptr)" )
{
    using namespace nonlocal;
    S s = { 'a', 7 };
    not_null< shared_ptr< S > > p( make_shared< S >(s) );

    EXPECT( p->c == 'a' );
    EXPECT( p->i ==  7  );
}

CASE( "not_null<>: Allows dereferencing (shared_ptr)" )
{
    shared_ptr< int > pi = make_shared< int >(12);
    not_null< shared_ptr< int > > p( pi );

    EXPECT( *p == *pi );
}

#endif // gsl_HAVE( SHARED_PTR )

#if gsl_HAVE( UNIQUE_PTR )

# if ! gsl_HAVE( MAKE_UNIQUE ) && ! gsl_HAVE( VARIADIC_TEMPLATE )
template<typename T>
unique_ptr<T> make_unique()
{
    return unique_ptr<T>(new T());
}

template<typename T, typename Arg>
unique_ptr<T> make_unique(Arg&& arg)
{
    return unique_ptr<T>(new T(std::forward<Arg>(arg)));
}
# endif

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
    not_null< unique_ptr< int > > p( make_unique< int >(12) );
    unique_ptr< int > z = gsl_nullptr;

    EXPECT_THROWS( p = not_null< unique_ptr< int > >( std::move(z) ) );
}

CASE( "not_null<>: Terminates assignment from related pointer types for null pointer value (unique_ptr)" )
{
    unique_ptr< MyDerived >  z = gsl_nullptr;
    not_null< unique_ptr< MyBase > > p( make_unique< MyDerived >() );

    EXPECT_THROWS( p = not_null< unique_ptr< MyDerived > >( std::move(z) ) );
}

CASE( "not_null<>: Terminates propagation of a moved-from value (unique_ptr)" )
{
    unique_ptr< int > pi = make_unique< int >(12);
    not_null< unique_ptr< int > > p( std::move( pi ) );
    not_null< unique_ptr< int > > q( std::move( p ) );

    EXPECT_THROWS( not_null< unique_ptr< int > >( std::move( p ) ) );
    EXPECT_THROWS( q = std::move( p ) );
}

CASE( "not_null<>: Allows to construct from a non-null underlying pointer (unique_ptr)" )
{
    unique_ptr< int > pi = make_unique< int >(12);
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
    unique_ptr< int > pi = make_unique< int >(12);
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
    unique_ptr< int > pi = make_unique< int >(12);
    int* raw(pi.get());

    not_null< unique_ptr< int > > p ( std::move(pi) ); // There...
    pi = std::move(p); // ...and back again.

    EXPECT_THROWS( *p );
    EXPECT( pi.get() == raw );
#endif
}

CASE( "not_null<>: Allows to move to a related pointer from a not_null pointer (unique_ptr)" )
{
#if gsl_HAVE( FUNCTION_REF_QUALIFIER )
    unique_ptr< MyDerived > pderived = make_unique< MyDerived >();
    MyDerived* raw(pderived.get());
    not_null< unique_ptr< MyDerived > > p ( std::move(pderived) );
    unique_ptr< MyBase > pbase = std::move(p);

    EXPECT_THROWS( *p );
    EXPECT( pbase.get() == raw );
#endif
}

CASE( "not_null<>: Allows to construct from a non-null underlying pointer (unique_ptr) with make_not_null()" )
{
    unique_ptr< int > pi = make_unique< int >(12);
    int* raw(pi.get());

    not_null< unique_ptr< int > > p = make_not_null( std::move(pi) );

    EXPECT( &*p == raw );
}

CASE( "not_null<>: Allows to construct from a non-null underlying pointer (unique_ptr) with deduction guide" )
{
#if gsl_HAVE( DEDUCTION_GUIDES )
    unique_ptr< int > pi = make_unique< int >(12);
    int* raw(pi.get());

    not_null p( std::move(pi) );

    EXPECT( &*p == raw );
#endif
}

CASE( "not_null<>: Allows to construct a const pointer from a non-null underlying pointer (unique_ptr)" )
{
    unique_ptr< int > pi = make_unique< int >(12);
    int* raw(pi.get());
    not_null< unique_ptr< const int > > p( std::move(pi) );

    EXPECT( &*p == raw );
}

CASE( "not_null<>: Converts to underlying pointer (unique_ptr)" )
{
#if gsl_HAVE( FUNCTION_REF_QUALIFIER )
    unique_ptr< int > pi = make_unique< int >();
    not_null< unique_ptr< int > > p( std::move(pi) );

    take_unique_by_val<int>( std::move(p) );
    //take_unique_by_ref<int>( p ); // We sacrifice the ability to convert to `unique_ptr<> const &`, cf. comment regarding conversion operators in gsl-lite.hpp.
#endif
}

CASE( "not_null<>: Allows to construct from a non-null related pointer (unique_ptr)" )
{
    unique_ptr< MyDerived > pderived = make_unique< MyDerived >();
    MyDerived* raw(pderived.get());
    not_null< unique_ptr< MyBase > > p( std::move(pderived) );

    EXPECT( &*p == raw );
}

CASE( "not_null<>: Allows to construct a const pointer from a non-null related pointer (unique_ptr)" )
{
    unique_ptr< MyDerived > pderived = make_unique< MyDerived >();
    MyDerived* raw(pderived.get());
    not_null< unique_ptr< const MyBase > > p( std::move(pderived) );

    EXPECT( &*p == raw );
}

CASE( "not_null<>: Allows to construct from a not_null related pointer type (unique_ptr)" )
{
    unique_ptr< MyDerived > pderived = make_unique< MyDerived >();
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
    unique_ptr< MyDerived > pderived = make_unique< MyDerived >();
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
    unique_ptr< MyDerived > pderived = make_unique< MyDerived >();
    not_null< unique_ptr< MyDerived > > p( std::move(pderived) );

    take_unique_by_val<MyBase>( std::move(p) );
    //take_unique_by_ref<MyBase>( p ); // We sacrifice the ability to convert to `unique_ptr<> const &`, cf. comment regarding conversion operators in gsl-lite.hpp.
#endif
}

CASE( "not_null<>: Allows assignment from a not_null related pointer type (unique_ptr)" )
{
    unique_ptr< MyDerived > pderived = make_unique< MyDerived >();
	MyDerived* raw(pderived.get());
    not_null< unique_ptr< MyDerived > > p( std::move(pderived) );
    not_null< unique_ptr< MyBase >    > q( make_unique< MyBase >() );

    q = std::move(p);

    EXPECT( &*q == raw );
}

CASE( "not_null<>: Allows assignment to a const pointer from a not_null related pointer type (unique_ptr)" )
{
    unique_ptr< MyDerived > pderived = make_unique< MyDerived >();
	MyDerived* raw(pderived.get());
    not_null< unique_ptr< MyDerived    > > p( std::move(pderived) );
    not_null< unique_ptr< const MyBase > > q( make_unique< MyBase >() );

    q = std::move(p);

    EXPECT( &*q == raw );
}

CASE( "not_null<>: Allows indirect member access (unique_ptr)" )
{
    using namespace nonlocal;
    S s = { 'a', 7 };
    not_null< unique_ptr< S > > p( make_unique< S >(s) );

    EXPECT( p->c == 'a' );
    EXPECT( p->i ==  7  );
}

CASE( "not_null<>: Allows dereferencing (unique_ptr)" )
{
    int i = 12;
    unique_ptr< int > pi = make_unique< int >(i);
    not_null< unique_ptr< int > > p( std::move(pi) );

    EXPECT( *p == i );
}

#endif // gsl_HAVE( UNIQUE_PTR )

#if gsl_HAVE( UNIQUE_PTR ) && gsl_HAVE( SHARED_PTR )

CASE( "not_null<>: Allows to construct a not_null<shared_ptr<T>> from a non-null unique_ptr<T>" )
{
    unique_ptr< int > pi = make_unique< int >(12);
    int* raw(pi.get());
    not_null< shared_ptr< int > > p( std::move(pi) );

    EXPECT( &*p == raw );
}

CASE( "not_null<>: Allows to construct a not_null<shared_ptr<const T>> from a non-null unique_ptr<T>" )
{
    unique_ptr< int > pi = make_unique< int >(12);
    int* raw(pi.get());
    not_null< shared_ptr< const int > > p( std::move(pi) );

    EXPECT( &*p == raw );
}

CASE( "not_null<>: Allows to construct a not_null<shared_ptr<T>> from a related non-null unique_ptr<U>" )
{
    unique_ptr< MyDerived > pderived = make_unique< MyDerived >();
    MyDerived* raw(pderived.get());
    not_null< shared_ptr< MyBase > > p( std::move(pderived) );

    EXPECT( &*p == raw );
}

CASE( "not_null<>: Allows to construct a not_null<shared_ptr<const T>> from a related non-null unique_ptr<U>" )
{
    unique_ptr< MyDerived > pderived = make_unique< MyDerived >();
    MyDerived* raw(pderived.get());
    not_null< shared_ptr< const MyBase > > p( std::move(pderived) );

    EXPECT( &*p == raw );
}

CASE( "not_null<>: Allows to construct a not_null<shared_ptr<T>> from a not_null<unique_ptr<T>>" )
{
    unique_ptr< int > pi = make_unique< int >(12);
    int* raw(pi.get());
    not_null< unique_ptr< int > > p( std::move(pi) );

#if gsl_CPP11_OR_GREATER
    not_null< shared_ptr< int > > q = std::move(p);
#else
    not_null< shared_ptr< int > > q(std::move(p)); // in C++98, we cannot differentiate between implicit and explicit cases, so conversion is always explicit
#endif

    EXPECT( &*q == raw );
}

CASE( "not_null<>: Allows to construct a not_null<shared_ptr<const T>> from a not_null<unique_ptr<T>>" )
{
    unique_ptr< int > pi = make_unique< int >(12);
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
    unique_ptr< MyDerived > pderived = make_unique< MyDerived >();
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
    unique_ptr< MyDerived > pderived = make_unique< MyDerived >();
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
    unique_ptr< MyDerived > pderived = make_unique< MyDerived >();
	MyDerived* raw(pderived.get());
    not_null< unique_ptr< MyDerived > > p( std::move(pderived) );
    not_null< shared_ptr< MyBase >    > q( make_shared< MyBase >() );

    q = std::move(p);

    EXPECT( &*q == raw );
}

CASE( "not_null<>: Allows assignment to a not_null<shared_ptr<const T>> from a related not_null<unique_ptr<U>>" )
{
    unique_ptr< MyDerived > pderived = make_unique< MyDerived >();
	MyDerived* raw(pderived.get());
    not_null< unique_ptr< MyDerived    > > p( std::move(pderived) );
    not_null< shared_ptr< const MyBase > > q( make_shared< MyBase >() );

    q = std::move(p);

    EXPECT( &*q == raw );
}

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
    int    v1; int          v2;
    int * pv1; int const * pv2;

    NotNull()
    :  v1(   7 ),  v2(  42 )
    , pv1( &v1 ), pv2( &v2 ) {}

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

// end of file
