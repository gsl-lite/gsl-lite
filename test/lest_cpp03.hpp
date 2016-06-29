// Copyright 2013, 2014, 2015, 2016 by Martin Moene
//
// lest is based on ideas by Kevlin Henney, see video at
// http://skillsmatter.com/podcast/agile-testing/kevlin-henney-rethinking-unit-testing-in-c-plus-plus
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef LEST_LEST_HPP_INCLUDED
#define LEST_LEST_HPP_INCLUDED

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <cctype>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <ctime>

#ifdef __clang__
# pragma clang diagnostic ignored "-Woverloaded-shift-op-parentheses"
# pragma clang diagnostic ignored "-Wunused-comparison"
# pragma clang diagnostic ignored "-Wunused-value"
#elif defined __GNUC__
# pragma GCC   diagnostic ignored "-Wunused-value"
#endif

#define  lest_VERSION "1.27.0"

#ifndef  lest_FEATURE_COLOURISE
# define lest_FEATURE_COLOURISE 0
#endif

#ifndef  lest_FEATURE_LITERAL_SUFFIX
# define lest_FEATURE_LITERAL_SUFFIX 0
#endif

#ifndef  lest_FEATURE_REGEX_SEARCH
# define lest_FEATURE_REGEX_SEARCH 0
#endif

#ifndef  lest_FEATURE_TIME
# define lest_FEATURE_TIME 1
#endif

#ifndef lest_FEATURE_TIME_PRECISION
#define lest_FEATURE_TIME_PRECISION  0
#endif

#ifdef _WIN32
# define lest_PLATFORM_IS_WINDOWS 1
#endif

#if lest_FEATURE_REGEX_SEARCH
# include <regex>
#endif

#if lest_FEATURE_TIME
# if lest_PLATFORM_IS_WINDOWS
#  include <iomanip>
#  include <windows.h>
# else
#  include <iomanip>
#  include <sys/time.h>
# endif
#endif

#if defined(_MSC_VER)
# define lest_COMPILER_MSVC_VERSION   (_MSC_VER / 100 - 5 - (_MSC_VER < 1900))
#else
# define lest_COMPILER_MSVC_VERSION   0
#endif

#if lest_COMPILER_MSVC_VERSION == 6
# define lest_COMPILER_IS_MSVC6  1
#endif

#if ( __cplusplus >= 201103L ) || lest_COMPILER_MSVC_VERSION >= 12
# define lest_CPP11_OR_GREATER  1
#endif

#if lest_CPP11_OR_GREATER || lest_COMPILER_MSVC_VERSION >= 10

# include <cstdint>
# include <random>
# include <tuple>

namespace lest
{
    using std::tie;
}

#else

# if !defined(__clang__) && defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Weffc++"
# endif

namespace lest
{
    template< typename T1, typename T2>
    struct Tie
    {
        Tie( T1 & first, T2 & second )
        : first( first ), second( second ) {}

        std::pair<T1, T2> const &
        operator=( std::pair<T1, T2> const & rhs )
        {
            first  = rhs.first;
            second = rhs.second;
            return rhs;
        }

    private:
        void operator=( Tie const & );

        T1 & first;
        T2 & second;
    };

    template<typename T1, typename T2>
    inline Tie<T1,T2> tie( T1 & first, T2 & second )
    {
      return Tie<T1, T2>( first, second );
    }

# if !defined(__clang__) && defined(__GNUC__)
#  pragma GCC diagnostic pop
# endif

}
#endif // lest_CPP11_OR_GREATER

namespace lest
{
#ifdef lest_COMPILER_IS_MSVC6
    using ::strtol;
    using ::rand;
    using ::srand;

    inline double abs( double x ) { return ::fabs( x ); }

    template< typename T >
    T const & (min)(T const & a, T const & b) { return a <= b ? a : b; }
#else
    using std::abs;
    using std::min;
    using std::strtol;
    using std::rand;
    using std::srand;
#endif
}

#if ! defined( lest_NO_SHORT_MACRO_NAMES ) && ! defined( lest_NO_SHORT_ASSERTION_NAMES )
# define SETUP             lest_SETUP
# define SECTION           lest_SECTION

# define EXPECT            lest_EXPECT
# define EXPECT_NOT        lest_EXPECT_NOT
# define EXPECT_NO_THROW   lest_EXPECT_NO_THROW
# define EXPECT_THROWS     lest_EXPECT_THROWS
# define EXPECT_THROWS_AS  lest_EXPECT_THROWS_AS

# define SCENARIO          lest_SCENARIO
# define GIVEN             lest_GIVEN
# define WHEN              lest_WHEN
# define THEN              lest_THEN
# define AND_WHEN          lest_AND_WHEN
# define AND_THEN          lest_AND_THEN
#endif

#define lest_SCENARIO( sketch  )  lest_CASE(    lest::text("Scenario: ") + sketch  )
#define lest_GIVEN(    context )  lest_SETUP(   lest::text(   "Given: ") + context )
#define lest_WHEN(     story   )  lest_SECTION( lest::text(   " When: ") + story   )
#define lest_THEN(     story   )  lest_SECTION( lest::text(   " Then: ") + story   )
#define lest_AND_WHEN( story   )  lest_SECTION( lest::text(   "  And: ") + story   )
#define lest_AND_THEN( story   )  lest_SECTION( lest::text(   "  And: ") + story   )

#define lest_TEST \
    lest_CASE

#define lest_CASE( specification, proposition ) \
    static void lest_FUNCTION( lest::env & ); \
    namespace { lest::add_test lest_REGISTRAR( specification, lest::test( proposition, lest_FUNCTION ) ); } \
    static void lest_FUNCTION( lest::env & lest_env )

#define lest_ADD_TEST( specification, test ) \
    specification.push_back( test )

#define lest_SETUP( context ) \
    for ( int lest__section = 0, lest__count = 1; lest__section < lest__count; lest__count -= 0==lest__section++ )

#define lest_SECTION( proposition ) \
    static int lest_UNIQUE( id ) = 0; \
    if ( lest::guard( lest_UNIQUE( id ), lest__section, lest__count ) ) \
        for ( int lest__section = 0, lest__count = 1; lest__section < lest__count; lest__count -= 0==lest__section++ )

#define lest_EXPECT( expr ) \
    do { \
        try \
        { \
            if ( lest::result score = lest_DECOMPOSE( expr ) ) \
                throw lest::failure( lest_LOCATION, #expr, score.decomposition ); \
            else if ( lest_env.pass ) \
                lest::report( lest_env.os, lest::passing( lest_LOCATION, #expr, score.decomposition ), lest_env.testing ); \
        } \
        catch(...) \
        { \
            lest::inform( lest_LOCATION, #expr ); \
        } \
    } while ( lest::is_false() )

#define lest_EXPECT_NOT( expr ) \
    do { \
        try \
        { \
            if ( lest::result score = lest_DECOMPOSE( expr ) ) \
            { \
                if ( lest_env.pass ) \
                    lest::report( lest_env.os, lest::passing( lest_LOCATION, lest::not_expr( #expr ), lest::not_expr( score.decomposition ) ), lest_env.testing ); \
            } \
            else \
                throw lest::failure( lest_LOCATION, lest::not_expr( #expr ), lest::not_expr( score.decomposition ) ); \
        } \
        catch(...) \
        { \
            lest::inform( lest_LOCATION, lest::not_expr( #expr ) ); \
        } \
    } while ( lest::is_false() )

#define lest_EXPECT_NO_THROW( expr ) \
    do \
    { \
        try { expr; } \
        catch (...) { lest::inform( lest_LOCATION, #expr ); } \
        if ( lest_env.pass ) \
            lest::report( lest_env.os, lest::got_none( lest_LOCATION, #expr ), lest_env.testing ); \
    } while ( lest::is_false() )

#define lest_EXPECT_THROWS( expr ) \
    do \
    { \
        try \
        { \
            expr; \
        } \
        catch (...) \
        { \
            if ( lest_env.pass ) \
                lest::report( lest_env.os, lest::got( lest_LOCATION, #expr ), lest_env.testing ); \
            break; \
        } \
        throw lest::expected( lest_LOCATION, #expr ); \
    } \
    while ( lest::is_false() )

#define lest_EXPECT_THROWS_AS( expr, excpt ) \
    do \
    { \
        try \
        { \
            expr; \
        }  \
        catch ( excpt & ) \
        { \
            if ( lest_env.pass ) \
                lest::report( lest_env.os, lest::got( lest_LOCATION, #expr, lest::of_type( #excpt ) ), lest_env.testing ); \
            break; \
        } \
        catch (...) {} \
        throw lest::expected( lest_LOCATION, #expr, lest::of_type( #excpt ) ); \
    } \
    while ( lest::is_false() )

#define lest_DECOMPOSE( expr ) ( lest::expression_decomposer() << expr )

#define lest_UNIQUE(  name       ) lest_UNIQUE2( name, __LINE__ )
#define lest_UNIQUE2( name, line ) lest_UNIQUE3( name, line )
#define lest_UNIQUE3( name, line ) name ## line

#define lest_LOCATION  lest::location(__FILE__, __LINE__)

#define lest_FUNCTION  lest_UNIQUE(__lest_function__  )
#define lest_REGISTRAR lest_UNIQUE(__lest_registrar__ )

#define lest_DIMENSION_OF( a ) ( sizeof(a) / sizeof(0[a]) )

namespace lest {

typedef std::string       text;
typedef std::vector<text> texts;

struct env;

struct test
{
    text name;
    void (* behaviour)( env & );

    test( text name, void (* behaviour)( env & ) )
    : name( name ), behaviour( behaviour ) {}
};

typedef std::vector<test> tests;
typedef tests test_specification;

struct add_test
{
    add_test( tests & specification, test const & test_case )
    {
        specification.push_back( test_case );
    }
};

struct result
{
    const bool passed;
    const text decomposition;

    result( bool passed, text decomposition )
    : passed( passed ), decomposition( decomposition ) {}
    operator bool() { return ! passed; }
};

struct location
{
    const text file;
    const int line;

    location( text file, int line )
    : file( file ), line( line ) {}
};

struct comment
{
    const text info;

    comment( text info ) : info( info ) {}
    operator bool() { return ! info.empty(); }
};

struct message : std::runtime_error
{
    const text kind;
    const location where;
    const comment note;

    ~message() throw() {}

    message( text kind, location where, text expr, text note = "" )
    : std::runtime_error( expr ), kind( kind ), where( where ), note( note ) {}
};

struct failure : message
{
    failure( location where, text expr, text decomposition )
    : message( "failed", where, expr + " for " + decomposition ) {}
};

struct success : message
{
    success( text kind, location where, text expr, text note = "" )
    : message( kind, where, expr, note ) {}
};

struct passing : success
{
    passing( location where, text expr, text decomposition )
    : success( "passed", where, expr + " for " + decomposition ) {}
};

struct got_none : success
{
    got_none( location where, text expr )
    : success( "passed: got no exception", where, expr ) {}
};

struct got : success
{
    got( location where, text expr )
    : success( "passed: got exception", where, expr ) {}

    got( location where, text expr, text excpt )
    : success( "passed: got exception " + excpt, where, expr ) {}
};

struct expected : message
{
    expected( location where, text expr, text excpt = "" )
    : message( "failed: didn't get exception", where, expr, excpt ) {}
};

struct unexpected : message
{
    unexpected( location where, text expr, text note = "" )
    : message( "failed: got unexpected exception", where, expr, note ) {}
};

struct guard
{
    int & id;
    int const & section;

    guard( int & id, int const & section, int & count )
    : id( id ), section( section )
    {
        if ( section == 0 )
            id = count++ - 1;
    }
    operator bool() { return id == section; }
};

class approx
{
public:
    explicit approx ( double magnitude )
    : epsilon_  ( std::numeric_limits<float>::epsilon() * 100 )
    , scale_    ( 1.0 )
    , magnitude_( magnitude ) {}

    static approx custom() { return approx( 0 ); }

    approx operator()( double magnitude )
    {
        approx approx ( magnitude );
        approx.epsilon( epsilon_  );
        approx.scale  ( scale_    );
        return approx;
    }

    double magnitude() const { return magnitude_; }

    approx & epsilon( double epsilon ) { epsilon_ = epsilon; return *this; }
    approx & scale  ( double scale   ) { scale_   = scale;   return *this; }

    friend bool operator == ( double lhs, approx const & rhs )
    {
        // Thanks to Richard Harris for his help refining this formula.
        return lest::abs( lhs - rhs.magnitude_ ) < rhs.epsilon_ * ( rhs.scale_ + (lest::min)( lest::abs( lhs ), lest::abs( rhs.magnitude_ ) ) );
    }

    friend bool operator == ( approx const & lhs, double rhs ) { return  operator==( rhs, lhs ); }
    friend bool operator != ( double lhs, approx const & rhs ) { return !operator==( lhs, rhs ); }
    friend bool operator != ( approx const & lhs, double rhs ) { return !operator==( rhs, lhs ); }

private:
    double epsilon_;
    double scale_;
    double magnitude_;
};

inline bool is_false(           ) { return false; }
inline bool is_true ( bool flag ) { return  flag; }

inline text not_expr( text message )
{
    return "! ( " + message + " )";
}

inline text with_message( text message )
{
    return "with message \"" + message + "\"";
}

inline text of_type( text type )
{
    return "of type " + type;
}

inline void inform( location where, text expr )
{
    try
    {
        throw;
    }
    catch( failure const & )
    {
        throw;
    }
    catch( std::exception const & e )
    {
        throw unexpected( where, expr, with_message( e.what() ) ); \
    }
    catch(...)
    {
        throw unexpected( where, expr, "of unknown type" ); \
    }
}

// Expression decomposition:

#if lest_CPP11_OR_GREATER || lest_COMPILER_MSVC_VERSION >= 10
inline std::string to_string( std::nullptr_t const &      ) { return "nullptr"; }
#endif
inline std::string to_string( std::string    const & text ) { return "\"" + text + "\"" ; }
inline std::string to_string( char const *   const & text ) { return "\"" + std::string( text ) + "\"" ; }
inline std::string to_string( char           const & text ) { return "\'" + std::string( 1, text ) + "\'" ; }

inline std::ostream & operator<<( std::ostream & os, approx const & appr )
{
    return os << appr.magnitude();
}

template <typename T>
std::string to_string( T const & value, int=0 /* VC6 */ )
{
    std::ostringstream os; os << std::boolalpha << value; return os.str();
}

template<typename T1, typename T2>
std::string to_string( std::pair<T1,T2> const & pair )
{
    std::ostringstream oss;
    oss << "{ " << to_string( pair.first ) << ", " << to_string( pair.second ) << " }";
    return oss.str();
}

#if lest_CPP11_OR_GREATER

template<typename TU, std::size_t N>
struct make_tuple_string
{
    static std::string make( TU const & tuple )
    {
        std::ostringstream os;
        os << to_string( std::get<N - 1>( tuple ) ) << ( N < std::tuple_size<TU>::value ? ", ": " ");
        return make_tuple_string<TU, N - 1>::make( tuple ) + os.str();
    }
};

template<typename TU>
struct make_tuple_string<TU, 0>
{
    static std::string make( TU const & ) { return ""; }
};

template<typename ...TS>
auto to_string( std::tuple<TS...> const & tuple ) -> std::string
{
    return "{ " + make_tuple_string<std::tuple<TS...>, sizeof...(TS)>::make( tuple ) + "}";
}
#endif

template <typename L, typename R>
std::string to_string( L const & lhs, std::string op, R const & rhs )
{
    std::ostringstream os; os << to_string( lhs ) << " " << op << " " << to_string( rhs ); return os.str();
}

template <typename L>
struct expression_lhs
{
    L lhs;

    expression_lhs( L lhs ) : lhs( lhs ) {}

    operator result() { return result( !!lhs, to_string( lhs ) ); }

    template <typename R> result operator==( R const & rhs ) { return result( lhs == rhs, to_string( lhs, "==", rhs ) ); }
    template <typename R> result operator!=( R const & rhs ) { return result( lhs != rhs, to_string( lhs, "!=", rhs ) ); }
    template <typename R> result operator< ( R const & rhs ) { return result( lhs <  rhs, to_string( lhs, "<" , rhs ) ); }
    template <typename R> result operator<=( R const & rhs ) { return result( lhs <= rhs, to_string( lhs, "<=", rhs ) ); }
    template <typename R> result operator> ( R const & rhs ) { return result( lhs >  rhs, to_string( lhs, ">" , rhs ) ); }
    template <typename R> result operator>=( R const & rhs ) { return result( lhs >= rhs, to_string( lhs, ">=", rhs ) ); }
};

struct expression_decomposer
{
    template <typename L>
    expression_lhs<L const &> operator<< ( L const & operand )
    {
        return expression_lhs<L const &>( operand );
    }
};

// Reporter:

#if lest_FEATURE_COLOURISE

inline text red  ( text words ) { return "\033[1;31m" + words + "\033[0m"; }
inline text green( text words ) { return "\033[1;32m" + words + "\033[0m"; }
inline text gray ( text words ) { return "\033[1;30m" + words + "\033[0m"; }

inline bool starts_with( text words, text with )
{
    return 0 == words.find( with );
}

inline text replace( text words, text from, text to )
{
    size_t pos = words.find( from );
    return pos == std::string::npos ? words : words.replace( pos, from.length(), to  );
}

inline text colour( text words )
{
    if      ( starts_with( words, "failed" ) ) return replace( words, "failed", red  ( "failed" ) );
    else if ( starts_with( words, "passed" ) ) return replace( words, "passed", green( "passed" ) );

    return replace( words, "for", gray( "for" ) );
}

inline bool is_cout( std::ostream & os ) { return &os == &std::cout; }

struct colourise
{
    const text words;

    colourise( text words )
    : words( words ) {}

    // only colourise for std::cout, not for a stringstream as used in tests:

    std::ostream & operator()( std::ostream & os ) const
    {
        return is_cout( os ) ? os << colour( words ) : os << words;
    }
};

inline std::ostream & operator<<( std::ostream & os, colourise words ) { return words( os ); }
#else
inline text colourise( text words ) { return words; }
#endif

inline text pluralise( text word,int n )
{
    return n == 1 ? word : word + "s";
}

inline std::ostream & operator<<( std::ostream & os, comment note )
{
    return os << (note ? " " + note.info : "" );
}

inline std::ostream & operator<<( std::ostream & os, location where )
{
#ifdef __GNUG__
    return os << where.file << ":" << where.line;
#else
    return os << where.file << "(" << where.line << ")";
#endif
}

inline void report( std::ostream & os, message const & e, text test )
{
    os << e.where << ": " << colourise( e.kind ) << e.note << ": " << test << ": " << colourise( e.what() ) << std::endl;
}

// Test runner:

#if lest_FEATURE_REGEX_SEARCH
    inline bool search( text re, text line )
    {
        return std::regex_search( line, std::regex( re ) );
    }
#else
    inline bool case_insensitive_equal( char a, char b )
    {
        return tolower( a ) == tolower( b );
    }

    inline bool search( text part, text line )
    {
        return std::search(
            line.begin(), line.end(),
            part.begin(), part.end(), case_insensitive_equal ) != line.end();
    }
#endif

inline bool match( texts whats, text line )
{
    for ( texts::iterator what = whats.begin(); what != whats.end() ; ++what )
    {
        if ( search( *what, line ) )
            return true;
    }
    return false;
}

inline bool hidden( text name )
{
#if lest_FEATURE_REGEX_SEARCH
    texts skipped; skipped.push_back( "\\[\\.\\]" ); skipped.push_back( "\\[hide\\]" );
#else
    texts skipped; skipped.push_back( "[." ); skipped.push_back( "[hide]" );
#endif
    return match( skipped, name );
}

inline bool none( texts args )
{
    return args.size() == 0;
}

inline bool select( text name, texts include )
{
    if ( none( include ) )
    {
        return ! hidden( name );
    }

    bool any = false;
    for ( texts::reverse_iterator pos = include.rbegin(); pos != include.rend(); ++pos )
    {
        text & part = *pos;

        if ( part == "@" || part == "*" )
            return true;

        if ( search( part, name ) )
            return true;

        if ( '!' == part[0] )
        {
            any = true;
            if ( search( part.substr(1), name ) )
                return false;
        }
        else
        {
            any = false;
        }
    }
    return any && ! hidden( name );
}

inline int indefinite( int repeat ) { return repeat == -1; }

typedef unsigned long seed_t;

struct options
{
    options()
    : help(false), abort(false), count(false), list(false), tags(false), time(false)
    , pass(false), lexical(false), random(false), version(false), repeat(1), seed(0) {}

    bool help;
    bool abort;
    bool count;
    bool list;
    bool tags;
    bool time;
    bool pass;
    bool lexical;
    bool random;
    bool version;
    int  repeat;
    seed_t seed;
};

struct env
{
    std::ostream & os;
    bool pass;
    text testing;

    env( std::ostream & os, bool pass )
    : os( os ), pass( pass ), testing() {}

    env & operator()( text test )
    {
        testing = test; return *this;
    }
};

struct action
{
    std::ostream & os;

    action( std::ostream & os ) : os( os ) {}

    operator      int() { return 0; }
    bool        abort() { return false; }
    action & operator()( test ) { return *this; }

private:
    action( action const & );
};

struct print : action
{
    print( std::ostream & os ) : action( os ) {}

    print &  operator()( test testing )
    {
        os << testing.name << "\n"; return *this;
    }
};

inline texts tags( text name, texts result = texts() )
{
    size_t none = std::string::npos;
    size_t lb   = name.find_first_of( "[" );
    size_t rb   = name.find_first_of( "]" );

    if ( lb == none || rb == none )
        return result;

    result.push_back( name.substr( lb, rb - lb + 1 ) );

    return tags( name.substr( rb + 1 ), result );
}

struct ptags : action
{
    std::set<text> result;

    ptags( std::ostream & os ) : action( os ), result() {}

    ptags & operator()( test testing )
    {
        texts tags_( tags( testing.name ) );
        for ( texts::iterator pos = tags_.begin(); pos != tags_.end() ; ++pos )
            result.insert( *pos );

        return *this;
    }

    ~ptags()
    {
        std::copy( result.begin(), result.end(), std::ostream_iterator<text>( os, "\n" ) );
    }
};

struct count : action
{
    int n;

    count( std::ostream & os ) : action( os ), n( 0 ) {}

    count & operator()( test ) { ++n; return *this; }

    ~count()
    {
        os << n << " selected " << pluralise("test", n) << "\n";
    }
};

#if lest_FEATURE_TIME

#if lest_PLATFORM_IS_WINDOWS
# if lest_COMPILER_IS_MSVC6
    typedef /*un*/signed __int64 uint64_t;
# else
    typedef unsigned long long uint64_t;
# endif
#else
# if ! lest_CPP11_OR_GREATER
    typedef unsigned long long uint64_t;
# endif
#endif

#if lest_PLATFORM_IS_WINDOWS
    inline uint64_t current_ticks()
    {
        static uint64_t hz = 0, hzo = 0;
        if ( ! hz )
        {
            QueryPerformanceFrequency( (LARGE_INTEGER *) &hz  );
            QueryPerformanceCounter  ( (LARGE_INTEGER *) &hzo );
        }
        uint64_t t; QueryPerformanceCounter( (LARGE_INTEGER *) &t );

        return ( ( t - hzo ) * 1000000 ) / hz;
    }
#else
    inline uint64_t current_ticks()
    {
        timeval t; gettimeofday( &t, NULL );
        return static_cast<uint64_t>( t.tv_sec ) * 1000000ull + static_cast<uint64_t>( t.tv_usec );
    }
#endif

struct timer
{
    const uint64_t start_ticks;

    timer() : start_ticks( current_ticks() ) {}

    double elapsed_seconds() const
    {
        return ( current_ticks() - start_ticks ) / 1e6;
    }
};

struct times : action
{
    env output;
    options option;
    int selected;
    int failures;

    timer total;

    times( std::ostream & os, options option )
    : action( os ), output( os, option.pass ), option( option ), selected( 0 ), failures( 0 ), total()
    {
        os << std::setfill(' ') << std::fixed << std::setprecision( lest_FEATURE_TIME_PRECISION );
    }

    operator int() { return failures; }

    bool abort() { return option.abort && failures > 0; }

    times & operator()( test testing )
    {
        timer t;

        try
        {
            testing.behaviour( output( testing.name ) );
        }
        catch( message const & )
        {
            ++failures;
        }

        os << std::setw(5) << ( 1000 * t.elapsed_seconds() ) << " ms: " << testing.name  << "\n";

        return *this;
    }

    ~times()
    {
        os << "Elapsed time: " << std::setprecision(1) << total.elapsed_seconds() << " s\n";
    }
};
#else
struct times : action { times( std::ostream &, options ) : action( os ) {} };
#endif

struct confirm : action
{
    env output;
    options option;
    int selected;
    int failures;

    confirm( std::ostream & os, options option )
    : action( os ), output( os, option.pass ), option( option ), selected( 0 ), failures( 0 ) {}

    operator int() { return failures; }

    bool abort() { return option.abort && failures > 0; }

    confirm & operator()( test testing )
    {
        try
        {
            ++selected; testing.behaviour( output( testing.name ) );
        }
        catch( message const & e )
        {
            ++failures; report( os, e, testing.name );
        }
        return *this;
    }

    ~confirm()
    {
        if ( failures > 0 )
        {
            os << failures << " out of " << selected << " selected " << pluralise("test", selected) << " " << colourise( "failed.\n" );
        }
        else if ( option.pass )
        {
            os << "All " << selected << " selected " << pluralise("test", selected) << " " << colourise( "passed.\n" );
        }
    }
};

template<typename Action>
bool abort( Action & perform )
{
    return perform.abort();
}

template< typename Action >
Action & for_test( tests specification, texts in, Action & perform, int n = 1 )
{
    for ( int i = 0; indefinite( n ) || i < n; ++i )
    {
        for ( tests::iterator pos = specification.begin(); pos != specification.end() ; ++pos )
        {
            test & testing = *pos;

            if ( select( testing.name, in ) )
                if ( abort( perform( testing ) ) )
                    return perform;
        }
    }
    return perform;
}

inline bool test_less( test const & a, test const & b ) { return a.name < b.name; }

inline void sort( tests & specification )
{
    std::sort( specification.begin(), specification.end(), test_less );
}

// Use struct to avoid VC6 error C2664 when using free function:

struct rng { int operator()( int n ) { return lest::rand() % n; } };

inline void shuffle( tests & specification, options option )
{
#if lest_CPP11_OR_GREATER
    std::shuffle( specification.begin(), specification.end(), std::mt19937( option.seed ) );
#else
    lest::srand( option.seed );

    rng generator;
    std::random_shuffle( specification.begin(), specification.end(), generator );
#endif
}

inline int stoi( text num )
{
    return lest::strtol( num.c_str(), NULL, 10 );
}

inline bool is_number( text arg )
{
    const text digits = "0123456789";
    return text::npos != arg.find_first_of    ( digits )
        && text::npos == arg.find_first_not_of( digits );
}

inline seed_t seed( text opt, text arg )
{
    // std::time_t: implementation dependent

    if ( arg == "time" )
        return static_cast<seed_t>( time( NULL ) );

    if ( is_number( arg ) )
        return lest::stoi( arg );

    throw std::runtime_error( "expecting 'time' or positive number with option '" + opt + "', got '" + arg + "' (try option --help)" );
}

inline int repeat( text opt, text arg )
{
    const int num = lest::stoi( arg );

    if ( indefinite( num ) || num >= 0 )
        return num;

    throw std::runtime_error( "expecting '-1' or positive number with option '" + opt + "', got '" + arg + "' (try option --help)" );
}

inline std::pair<text, text>
split_option( text arg )
{
    text::size_type pos = arg.rfind( '=' );

    return pos == text::npos
                ? std::make_pair( arg, text() )
                : std::make_pair( arg.substr( 0, pos ), arg.substr( pos + 1 ) );
}

inline std::pair<options, texts>
split_arguments( texts args )
{
    options option; texts in;

    bool in_options = true;

    for ( texts::iterator pos = args.begin(); pos != args.end() ; ++pos )
    {
        text opt, val, arg = *pos;
        tie( opt, val ) = split_option( arg );

        if ( in_options )
        {
            if      ( opt[0] != '-'                             ) { in_options     = false;           }
            else if ( opt == "--"                               ) { in_options     = false; continue; }
            else if ( opt == "-h"      || "--help"       == opt ) { option.help    =  true; continue; }
            else if ( opt == "-a"      || "--abort"      == opt ) { option.abort   =  true; continue; }
            else if ( opt == "-c"      || "--count"      == opt ) { option.count   =  true; continue; }
            else if ( opt == "-g"      || "--list-tags"  == opt ) { option.tags    =  true; continue; }
            else if ( opt == "-l"      || "--list-tests" == opt ) { option.list    =  true; continue; }
            else if ( opt == "-t"      || "--time"       == opt ) { option.time    =  true; continue; }
            else if ( opt == "-p"      || "--pass"       == opt ) { option.pass    =  true; continue; }
            else if (                     "--version"    == opt ) { option.version =  true; continue; }
            else if ( opt == "--order" && "declared"     == val ) { /* by definition */   ; continue; }
            else if ( opt == "--order" && "lexical"      == val ) { option.lexical =  true; continue; }
            else if ( opt == "--order" && "random"       == val ) { option.random  =  true; continue; }
            else if ( opt == "--random-seed" ) { option.seed   = seed  ( "--random-seed", val ); continue; }
            else if ( opt == "--repeat"      ) { option.repeat = repeat( "--repeat"     , val ); continue; }
            else throw std::runtime_error( "unrecognised option '" + opt + "' (try option --help)" );
        }
        in.push_back( arg );
    }
    return std::make_pair( option, in );
}

inline int usage( std::ostream & os )
{
    os <<
        "\nUsage: test [options] [test-spec ...]\n"
        "\n"
        "Options:\n"
        "  -h, --help         this help message\n"
        "  -a, --abort        abort at first failure\n"
        "  -c, --count        count selected tests\n"
        "  -g, --list-tags    list tags of selected tests\n"
        "  -l, --list-tests   list selected tests\n"
        "  -p, --pass         also report passing tests\n"
#if lest_FEATURE_TIME
        "  -t, --time         list duration of selected tests\n"
#endif
        "  --order=declared   use source code test order (default)\n"
        "  --order=lexical    use lexical sort test order\n"
        "  --order=random     use random test order\n"
        "  --random-seed=n    use n for random generator seed\n"
        "  --random-seed=time use time for random generator seed\n"
        "  --repeat=n         repeat selected tests n times (-1: indefinite)\n"
        "  --version          report lest version and compiler used\n"
        "  --                 end options\n"
        "\n"
        "Test specification:\n"
        "  \"@\", \"*\" all tests, unless excluded\n"
        "  empty    all tests, unless tagged [hide] or [.optional-name]\n"
#if lest_FEATURE_REGEX_SEARCH
        "  \"re\"     select tests that match regular expression\n"
        "  \"!re\"    omit tests that match regular expression\n"
#else
        "  \"text\"   select tests that contain text (case insensitive)\n"
        "  \"!text\"  omit tests that contain text (case insensitive)\n"
#endif
        ;
    return 0;
}

inline text compiler()
{
    std::ostringstream os;
#if   defined (__clang__ )
    os << "clang " << __clang_version__;
#elif defined (__GNUC__  )
    os << "gcc " << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__;
#elif defined ( _MSC_VER )
    os << "MSVC " << lest_COMPILER_MSVC_VERSION << " (" << _MSC_VER << ")";
#else
    os << "[compiler]";
#endif
    return os.str();
}

inline int version( std::ostream & os )
{
    os << "lest version "  << lest_VERSION << "\n"
       << "Compiled with " << compiler()   << " on " << __DATE__ << " at " << __TIME__ << ".\n"
       << "For more information, see https://github.com/martinmoene/lest.\n";
    return 0;
}

inline int run( tests specification, texts arguments, std::ostream & os = std::cout )
{
    try
    {
        options option; texts in;
        tie( option, in ) = split_arguments( arguments );

        if ( option.lexical ) {    sort( specification         ); }
        if ( option.random  ) { shuffle( specification, option ); }

        if ( option.help    ) { return usage  ( os ); }
        if ( option.version ) { return version( os ); }
        if ( option.count   ) { count count_( os         ); return for_test( specification, in, count_ ); }
        if ( option.list    ) { print print_( os         ); return for_test( specification, in, print_ ); }
        if ( option.tags    ) { ptags ptags_( os         ); return for_test( specification, in, ptags_ ); }
        if ( option.time    ) { times times_( os, option ); return for_test( specification, in, times_ ); }

        { confirm confirm_( os, option ); return for_test( specification, in, confirm_, option.repeat ); }
    }
    catch ( std::exception const & e )
    {
        os << "Error: " << e.what() << "\n";
        return 1;
    }
}

// VC6: make<cont>(first,last) replaces cont(first,last)

template<typename C, typename T>
C make( T const * first, T const * const last )
{
    C result;
    for ( ; first != last; ++first )
    {
       result.push_back( *first );
    }
    return result;
}

inline tests make_tests( test const * first, test const * const last )
{
    return make<tests>( first, last );
}

inline texts make_texts( char const * const * first, char const * const * last )
{
    return make<texts>( first, last );
}

// Traversal of test[N] (test_specification[N]) set up to also work with MSVC6:

template <typename C> test const *         test_begin( C const & c ) { return &*c; }
template <typename C> test const *           test_end( C const & c ) { return test_begin( c ) + lest_DIMENSION_OF( c ); }

template <typename C> char const * const * text_begin( C const & c ) { return &*c; }
template <typename C> char const * const *   text_end( C const & c ) { return text_begin( c ) + lest_DIMENSION_OF( c ); }

template <typename C> tests make_tests( C const & c ) { return make_tests( test_begin( c ), test_end( c ) ); }
template <typename C> texts make_texts( C const & c ) { return make_texts( text_begin( c ), text_end( c ) ); }

inline int run( tests const & specification, int argc, char * argv[], std::ostream & os = std::cout )
{
    return run( specification, make_texts( argv + 1, argv + argc ), os  );
}

inline int run( tests const & specification, std::ostream & os = std::cout )
{
    return run( specification, texts(), os );
}

template <typename C>
int run(  C const & specification, texts args, std::ostream & os = std::cout )
{
    return run( make_tests( specification ), args, os  );
}

template <typename C>
int run(  C const & specification, int argc, char * argv[], std::ostream & os = std::cout )
{
    return run( make_tests( specification ), argv, argc, os  );
}

template <typename C>
int run(  C const & specification, std::ostream & os = std::cout )
{
    return run( make_tests( specification ), os  );
}

} // namespace lest

#endif // LEST_LEST_HPP_INCLUDED
