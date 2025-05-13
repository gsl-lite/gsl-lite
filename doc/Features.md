# Features

**Contents:**  
- [Contract and assertion checks](#contract-and-assertion-checks): `gsl_Expects( pred )`, `gsl_Ensures( pred )`, `gsl_Assert( pred )`, and more
- [Pointer annotations](#pointer-annotations): `owner<P>`, `not_null<P>`, and `not_null_ic<P>`
- [Numeric type conversions](#numeric-type-conversions): `narrow<T>( u )`, `narrow_failfast<T>( u )`, and `narrow_cast<T>( u )`
- [Safe contiguous ranges](#safe-contiguous-ranges): `span<T, Extent>`
- [Bounds-checked element access](#bounds-checked-element-access): `at( container, index )`
- [Integer type aliases](#integer-type-aliases): `index`, `dim`, `stride`, `diff`
- [String type aliases](#string-type-aliases): `zstring`, `czstring`, `wzstring`, `cwzstring`
- [Ad-hoc resource management (C++11 and higher)](#ad-hoc-resource-management-c11-and-higher): `finally( action )`, `on_return( action )`, and `on_error( action )`
- [Feature checking macros](#feature-checking-macros)
- [Polyfills](#polyfills)
- [Configuration options and switches](#configuration-options-and-switches)
- [Configuration changes, deprecated and removed features](#configuration-changes-deprecated-and-removed-features)


## Contract and assertion checks

(Core Guidelines reference: [GSL.assert: Assertions](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#SS-assertions))

There are several macros for expressing preconditions, postconditions, and invariants:

- `gsl_FailFast()` to indicate unreachable code
- `gsl_Expects( cond )` for simple preconditions
- `gsl_Ensures( cond )` for simple postconditions
- `gsl_Assert( cond )` for simple assertions
- `gsl_ExpectsDebug( cond )` for debug-mode preconditions
- `gsl_EnsuresDebug( cond )` for debug-mode postconditions
- `gsl_AssertDebug( cond )` for debug-mode assertions
- `gsl_ExpectsAudit( cond )` for preconditions that are expensive or include potentially opaque function calls
- `gsl_EnsuresAudit( cond )` for postconditions that are expensive or include potentially opaque function calls
- `gsl_AssertAudit( cond )` for assertions that are expensive or include potentially opaque function calls

**Example:**
```c++
template< class RandomIt >
auto median( RandomIt first, RandomIt last )
{
    gsl_Expects( first != last );

        // Verifying that a range of elements is sorted is an expensive operation that changes the
        // computational complexity of the function `median()` from 𝒪(1) to 𝒪(n). Therefore, we
        // express it as an audit-level contract check.
    gsl_ExpectsAudit( std::is_sorted( first, last ) );

    auto count = last - first;
    return count % 2 != 0
        ? first[ count / 2 ]
        : std::midpoint( first[ count / 2 ], first[ count / 2 + 1 ] );
}
```

The behavior of the different flavors of pre-/postcondition checks and assertions depends on a number of [configuration macros](#contract-checking-configuration-macros):

- The macros **`gsl_Expects()`**, **`gsl_Ensures()`**, and **`gsl_Assert()`** are compiled to runtime checks unless contract
  checking is disabled with the `gsl_CONFIG_CONTRACT_CHECKING_OFF` configuration macro.

- Contract checks expressed with **`gsl_ExpectsAudit()`**, **`gsl_EnsuresAudit()`**, and **`gsl_AssertAudit()`** are discarded by
  default. In order to have them checked at runtime, the `gsl_CONFIG_CONTRACT_CHECKING_AUDIT` configuration macro must be defined.

- The macros **`gsl_ExpectsDebug()`**, **`gsl_EnsuresDebug()`**, and **`gsl_AssertDebug()`** are compiled to runtime checks unless
  contract checking is disabled by defining `gsl_CONFIG_CONTRACT_CHECKING_OFF` or assertions are disabled by defining `NDEBUG`.
  They can be used in place of the [`assert()`](https://en.cppreference.com/w/cpp/error/assert) macro from the C standard library.
  (Note that defining `gsl_CONFIG_CONTRACT_CHECKING_AUDIT` also enables checking of the `gsl_*Debug()` macros regardless of
  whether `NDEBUG` is defined.)

- The **`gsl_Expects*()`**, **`gsl_Ensures*()`**, **`gsl_Assert*()`** categories of checks can be disabled individually with the
  `gsl_CONFIG_CONTRACT_CHECKING_EXPECTS_OFF`, `gsl_CONFIG_CONTRACT_CHECKING_ENSURES_OFF`, or `gsl_CONFIG_CONTRACT_CHECKING_ASSERT_OFF`
  configuration macros.

- **`gsl_FailFast()`** is similar to `gsl_Assert( false )` but is guaranteed to interrupt the current path of execution even
  if contract checking is disabled with configuration macros. It is useful in places which should not be reachable during program
  execution. For example:
  ```c++
  enum class Color : int { red, green, blue };
  std::string colorToString( Color color )
  {
      switch (color)
      {
      case Color::red:   return "red";
      case Color::green: return "green";
      case Color::blue:  return "blue";
      }
      gsl_FailFast();
  }
  ```
  The C++ language permits casting any representable integer value to an enum. Therefore, `colorToString(Color(0xFF00FF))`
  is legal C++, but not actually supported by this `colorToString()` implementation. `gsl_FailFast()` is employed here to ensure
  that passing unsupported values to `colorToString()` will be detected at runtime.  
    
  `gsl_FailFast()` behaves as if annotated by the [`[[noreturn]]`](https://en.cppreference.com/w/cpp/language/attributes/noreturn)
  attribute. A C++11 compiler will therefore not emit a warning about a missing return statement in `colorToString()`.
  <!--(Unrelated note: the `switch` statement above deliberately elides the `default:` clause. Because the switch statement is wrapped
  in a dedicated function, we can use `return` instead of `break` to conclude the `case` clauses, and hence the default handler
  can simply go after the `switch` statement. The benefit of avoiding the `default:` clause is that most compilers will understand
  that the `switch` statement is supposed to handle all defined enumeration values and thus issue a warning if the programmer adds
  a new enumeration value (say, `Color::yellow`) but forgets to amend the `switch` statement.)-->


## Pointer annotations

- [`owner<P>` (C++11 and higher)](#ownerp-c11-and-higher)
- [`not_null<P>`](#not_nullp)
    - [Motivation](#motivation)
    - [Usage](#usage)
    - [Nullability and the moved-from state](#nullability-and-the-moved-from-state)
- [`not_null_ic<P>`](#not_null_icp)

(Core Guidelines reference: [GSL.view: Views](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#gslview-views))

*gsl-lite* defines the three pointer type annotations `owner<P>`, `not_null<P>`, and `not_null_ic<P>` which let the user signify additional intent:

- `owner<P>` indicates that a raw pointer `P` carries ownership of the object it points to, and is thus responsible for managing its lifetime.
- `not_null<P>` and `not_null_ic<P>` indicate that a pointer `P` must not be `nullptr`.

### `owner<P>` (C++11 and higher)

`gsl_lite::owner<P>` is a type alias that solely serves the purpose of expressing intent:
```c++
template< class P >
requires ( std::is_pointer_v<P> )
using owner = P;
```
(*Note:* The actual definition uses a SFINAE constraint to support C\+\+11 to C\+\+17).

As far as the type system and the runtime behavior is concerned, `owner<P>` is exactly equivalent to `P`. However, the annotation
conveys intent to a reader, and static analysis tools may use the annotation impose semantic checks based on the assumption of ownership.

If possible, a smart pointer with automatic lifetime management such as [`std::unique_ptr<>`](https://en.cppreference.com/w/cpp/memory/unique_ptr)
or [`std::shared_ptr<>`](https://en.cppreference.com/w/cpp/memory/shared_ptr) should be used instead. `owner<>` is meant to annotate raw
pointers which cannot currently be replaced with a smart pointer.


### `not_null<>`

`gsl_lite::not_null<P>` is a class template that wraps a pointer type `P` while enforcing non-nullability.

`not_null<P>` has no default constructor, and its unary constructors use [`gsl_Expects()`](#contract-and-assertion-checking-macros) to check
that their argument is not `nullptr`. Therefore, when attempting to construct a `not_null<P>` from a null pointer, a runtime contract violation is triggered:
```c++
using gsl_lite::not_null;

int i = 42;
int * pi = &i;
int * pn = nullptr;
//not_null<int *> n;  // compile error: no default constructor
//not_null<int *> npi = pi;  // compile error: no implicit conversion
not_null<int *> npi = not_null( pi );  // explicit conversion: runtime null check
//not_null<int *> nn = nullptr;  // compile error: no implicit conversion
//not_null<int *> nn = not_null( nullptr );  // compile error: no explicit conversion
not_null<int *> npn = not_null( pn );  // explicit conversion: runtime null check ⇒ runtime contract violation
```

#### Motivation

<!--C++ knows two kinds of indirections: pointers and references. A pointer can refer to a special value known as `nullptr`,
which indicates it does not point to any object, and can be subsequently reassigned. In contrast, references must always
refer to a valid object, and they can be assigned only once, as part of their initialization.-->

When defining function signatures, it is customary to use pointers and references to indicate whether an object
reference is required or optional:
```c++
void lock( Mutex & m );  // requires a valid object reference

struct ListNode
{
    ListNode* prev;
    ListNode* next;
    int payload;
};
bool tryRemove( ListNode * x );  // also accepts a `nullptr`
```

But this convention does not apply to every situation. For example, accepting a pointer argument can also emphasize the fact
that the object's memory address may be taken and stored, as in the case of a list or tree node. Storing the memory address
of a by-reference argument "feels" wrong, and may be flagged by static analyzers:
```c++
void insertAfter( ListNode & x, ListNode & newNode )
{
    newNode.prev = &x;  // <-- Eww.
    newNode.next = x.next;
    x.next = &newNode;  // <-- Eww.
}
```
The function would be less awkward if it accepted pointers.

To make sure a pointer argument is not `nullptr`, we can add a precondition check to the function:
```c++
void remove( ListNode * x )
{
    gsl_Expects( x != nullptr );

    if ( x->prev != nullptr ) x->prev->next = x->next;
    if ( x->next != nullptr ) x->next->prev = x->prev;
    delete x;
}
```

There are other situations when an object cannot be passed by reference, but a `nullptr` may still be unwanted. The most
familiar case is passing ownership, best expressed with a smart pointer:
```c++
void insertAfter( ListNode * x, std::unique_ptr<ListNode> newNode )
{
    gsl_Expects( x != nullptr );
    gsl_Expects( newNode != nullptr );

    newNode->prev = x;
    newNode->next = x->next;
    x->next = newNode.release();
}
```

Writing all the precondition checks against `nullptr` quickly becomes tedious. And unlike the contract checks once envisioned
for C++20, precondition checks expressed with `gsl_Expects()` are not part of the function signature, which therefore does
not convey that it cannot handle `nullptr` input.

This is where `not_null<>` comes in.  With `not_null<>`, the precondition can be "lifted" into the type system,
and thus into the function signature:

```c++
void remove( not_null<ListNode*> x )
{
    if ( x->prev != nullptr ) x->prev->next = x->next;
    delete x;
}
```

All `not_null<>` constructors check their arguments for `nullptr` with `gsl_Expects()`, so the functions above can already assume
that their arguments will never be `nullptr`, and the explicit precondition checks can be omitted.

`not_null<>` can also be used with smart pointers, so the signature of the function `insertAfter()` could be changed to
```c++
void insertAfter( not_null<ListNode*> x, not_null<std::unique_ptr<ListNode>> newNode );
```


#### Usage

`not_null<P>` strives to behave like the underlying type `P` as transparently as reasonably possible:

- There is no runtime size overhead: `sizeof( not_null<P> ) == sizeof( P )`.
- `not_null<P>` implicitly converts to `Q` if `P` implicitly converts to `Q`.  
  For example, `not_null<int *>` implicitly converts to `int const *`.
- `not_null<P>` explicitly converts to `Q` if `P` explicitly converts to `Q`.
- `not_null<P>` can be dereferenced if `P` can be dereferenced.
- If `P` can be copied, `not_null<P>` can be copied. If `P` can be moved, `not_null<P>` can be moved.  
  For example, `not_null<T *>` is copyable, `not_null<std::unique_ptr<T>>` is movable but not copyable, and `not_null<std::shared_ptr<T>>` is copyable and movable.
- If `P` points to a `struct`, `class`, or `union`, `not_null<P>` defines the member access operator `operator->`.
- If `P` has a member function `P::get()`, `not_null<P>` also defines a member function `not_null<P>::get()` which forwards to `P::get()`.
- If `P` is a function pointer or a nullable function object such as [`std::function<>`](https://en.cppreference.com/w/cpp/utility/functional/function),
  `not_null<>` defines the function call operator `operator()` which forwards the call to `P`:  
  ```c++
  long call( not_null<long (*)(long)> func, long arg )
  {
      return func( arg );
  }
  ```
- `Q` implicitly converts to `not_null<P>` if `Q` is non-nullable and implicitly converts to `P`.  
  For example, [`std::labs`](https://en.cppreference.com/w/cpp/numeric/math/abs) implicitly converts to `not_null<long (*)(long)>`:
  ```c++
  long callAbs( long val )
  {
      return call( std::labs, val );
  }
  ```
- `Q` explicitly converts to `not_null<P>` if `Q` is nullable and implicitly converts to `P`, or if `Q` explicitly converts to `P`.  
  For example:
  ```c++
  std::string readLine( not_null<std::FILE*> file );
  std::FILE* file = ...;
  //readLine( file );  // does not compile: `file` is nullable
  readLine( not_null( file ) );  // compiles and executes a check at runtime
  ```
- If `P` is hashable (that is, the [`std::hash<P>`](https://en.cppreference.com/w/cpp/utility/hash) specialization is *enabled*), `not_null<P>` is hashable.

For C++14 and older, where [class template argument deduction](https://en.cppreference.com/w/cpp/language/class_template_argument_deduction)
is not available, *gsl-lite* defines a set of helper functions `make_not_null()` for explicitly constructing `not_null<>` objects.

*gsl-lite* additionally defines the helper functions `make_unique<T>()` and `make_shared<T>()` which behave like
[`std::make_unique<T>()`](https://en.cppreference.com/w/cpp/memory/unique_ptr/make_unique) and
[`std::make_shared<T>()`](https://en.cppreference.com/w/cpp/memory/shared_ptr/make_shared) but return `not_null<std::unique_ptr<T>>`
and `not_null<std::shared_ptr<T>>`, respectively.

`not_null<P>` is meant to point to single objects, not arrays of objects. It therefore does not define a subscript operator,
pointer increment or decrement operators, or pointer addition or subtraction operators, and
`gsl_lite::make_unique<T>()` and `gsl_lite::make_shared<T>()` are not defined for array types.

To extract the nullable object wrapped by a `not_null<>` object, call `gsl_lite::as_nullable()`:
```c++
auto npi = gsl_lite::make_unique<int>( 42 );  // not_null<std::unique_ptr<int>>
auto pi = gsl_lite::as_nullable( std::move( npi ) );  // std::unique_ptr<int>
```

This is useful when accessing operations of `P` not forwarded by `not_null<P>`:
```c++
void insertAfter( not_null<ListNode*> x, not_null<std::unique_ptr<ListNode>> newNode )
{
    newNode->prev = x;
    newNode->next = x->next;
    x->next = gsl_lite::as_nullable( std::move( newNode ) ).release();  // no `release()` member function on `not_null<>`
}
```


#### Nullability and the moved-from state

Because `not_null<P>` retains the copyability and movability of `P`, a `not_null<P>` object may have a *moved-from state* if the
underlying pointer `P` has one. Therefore, a `not_null<P>` object may in fact be `nullptr` after it has been moved from:
```c++
auto x = gsl_lite::make_unique<int>( 42 );  // not_null<std::unique_ptr<int>>()
auto y = std::move( x );  // x is now nullptr
*x = 43;  // dereferencing a nullptr ⇒ runtime contract violation
```
This is where *gsl-lite*'s implementation of `not_null<>` differs from the implementation of Microsoft GSL, which ensures that
its `not_null<P>` cannot ever become `nullptr`, rendering its `not_null<std::unique_ptr<T>>` immovable (cf. [microsoft/GSL#1022](https://github.com/microsoft/GSL/issues/1022)).
While this choice would prevent the error above, it inhibits many interesting use cases for `not_null<>`.
For example, consider the following resource handle class:

```c++
struct FileCloser { void operator ()(std::FILE* file) const { std::fclose( file ); } };
using FilePtr = std::unique_ptr<std::FILE, FileCloser>;

class FileHandle  // movable
{
private:
    FilePtr file_;

public:
    explicit FileHandle( FilePtr _file )
        : file_( std::move( _file ) )
    {
    }
    int getc()
    {
        return std::fgetc( file_.get() );
    }
    ...
};
```

In this class, null checks can be introduced simply by changing the definition of `FilePtr` to
```c++
using FilePtr = not_null<std::unique_ptr<std::FILE, FileCloser>>;
```
Any code constructing a `FileHandle` will then have to make an explicit `not_null()` check:
```c++
std::FILE* rawFile = std::fopen( ... );
if ( rawFile == nullptr ) throw std::runtime_error( ... );
auto file = FileHandle( not_null( rawFile ) );
```
But any function accepting a `FileHandle` can now be sure that the handle is not `nullptr`:
```c++
std::vector<std::string>
readLines( FileHandle file )
{
    // file cannot hold a nullptr here
    ...
}
```
Again, one could say that `not_null<>` "lifts" the precondition of non-nullability into the type system.

Technically, non-nullability is not necessarily an invariant of `not_null<>`, and thus our redefinition of
`FilePtr` does not add a non-nullability invariant to `FileHandle`.
However, because a `nullptr` can appear only in the *moved-from state*, a nullability contract violation
can occur only if a moved-from object is being used. Because *use-after-move* errors can often be detected by static
analysis tools, this may be considered "safe enough" for practical purposes.

It should be noted that, for movable types `P`, `not_null<P>` enforces a runtime contract check on every access.
Therefore, when calling the `getc()` member function on a moved-from `FileHandle` object, a runtime contract violation
would be triggered on the `file_.get()` call. This means that, through adopting `not_null<>`, `FileHandle::getc()`
adopts the implicit precondition that the object be not in the moved-from state.

<!--(Recommended further reading: Herb Sutter's article ["Move, simply"](https://herbsutter.com/2020/02/17/move-simply/),
which argues that objects that have a special moved-from state in which most of their operations may not be used
are buggy, and [Sean Parent's rebuttal](https://herbsutter.com/2020/02/17/move-simply/#comment-41129) in the comments below.)-->

A `not_null<P>` cannot be directly compared to a `nullptr` because it is not meant to be nullable.
If you have to check for the moved-from state, use the `gsl_lite::is_valid()` predicate:
```c++
auto npi = gsl_lite::make_unique<int>( 42 );
// ...
//if (npi == nullptr) { ... }  // compile error
if ( !gsl_lite::is_valid( npi ) ) { ... }  // ok
```


### `not_null_ic<P>`

(*Note:* `not_null_ic<>` is a *gsl-lite* extension and not part of the C++ Core Guidelines.)

`gsl_lite::not_null_ic<P>` is a class template that wraps a pointer type `P` while enforcing non-nullability.
It provides all the guarantees and run-time checks of [`not_null<P>`](#not_nullp) but relaxes the requirements
of its conversion constructors: implicit conversion from `U` to `not_null_ic<P>` is allowed if `U` implicitly
converts to `P`.

`not_null<>` does not allow implicit conversion from nullable types:
```c++
void use( not_null<int *> p );
int i;
//use( &i );  // compile error: no implicit conversion
use( not_null( &i ) );  // runtime check
```

This choice has the generally desirable consequence that it encourages propagation of non-nullability.
Explicit conversions are needed only when converting a nullable to a non-nullable pointer; therefore, as
more and more of a code base is converted to `not_null<>`, fewer explicit conversions need to be used.

However, in some code bases it may not be feasible to insert explicit not-null checks at every invocation
site. In such a situation, `gsl_lite::not_null_ic<P>` can be used instead. `not_null_ic<P>` derives from `not_null<P>`
but additionally allows implicit construction from nullable types:

```c++
void use( not_null_ic<int *> p );
int i;
use( &i );  // runtime check
```

(Compatibility note: Microsoft GSL defines the classes `not_null<>` and `strict_not_null<>` which behave
like *gsl-lite*'s `not_null_ic<>` and `not_null<>`, respectively.)


## Numeric type conversions

(Core Guidelines reference: [GSL.util: Utilities](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#gslutil-utilities))

- [`narrow<T>( u )`](#narrowt-u), a checked numeric cast
- [`narrow_failfast<T>( u )`](#narrow_failfastt-u), a checked numeric cast
- [`narrow_cast<T>( u )`](#narrow_castt-u), an unchecked numeric cast

### `narrow<T>( u )`

`gsl_lite::narrow<T>( u )` is a numeric cast that is not meant to be lossy. If narrowing leads to a change of sign or
loss of information, an exception of type `gsl_lite::narrowing_error` is thrown.

**Example:**
```c++
double volume = ...;  // (m³)
double bucketCapacity = ...;  // (m³)
double numBucketsF = std::ceil( volume/bucketCapacity );
auto numBuckets = gsl::narrow<gsl::dim>( numBucketsF );
```

In this example, an exception will be thrown if `numBucketsF` is not an integer (for instance, `std::ceil(-INFINITY)` will return `-INFINITY`),
or if the value cannot be represented by the integer type `gsl::dim`.


### `narrow_failfast<T>( u )`

(*Note:* `narrow_failfast<T>( u )` is a *gsl-lite* extension and not part of the C++ Core Guidelines.)

`gsl_lite::narrow_failfast<T>( u )` is a numeric cast that is not meant to be lossy, which is verified with
[`gsl_Assert()`](#contract-and-assertion-checks). If narrowing leads to a change of sign or loss of information, an
assertion violation is triggered.

The `narrow<T>( u )` function specified by the C++ Core Guidelines throws an exception, thereby indicating exceptional circumstances
(for instance, "input data too large"). The exception may be caught and dealt with at runtime. Contrariwise, the purpose of
`narrow_failfast<T>( u )` is to detect and prevent programming errors which the user of the program cannot do anything about.

**Example:**
```c++
auto vec = std::vector{ ... };
auto elem = ...;
auto pos = std::find( vec.begin(), vec.end(), elem );  // let us assume `pos < vec.end()`
auto diff = pos - vec.end();  // <-- bug: we accidentally swapped `pos` and `vec.end()`
auto size = gsl::narrow<std::size_t>( diff );  // assertion violation: `diff` is negative
auto part = std::vector( size );
```


### `narrow_cast<T>( u )`

`narrow_cast<T>( u )` is a numeric cast in which loss of information is acceptable. It is exactly equivalent to `static_cast<T>( u )`, the
only difference is that `narrow_cast<>()` conveys the intent that truncation or sign change is expressly acceptable.

**Example 1:**
```c++
auto allBitsSet = gsl::narrow_cast<std::uint32_t>( -1 );  // sign change to 0xFFFFFFFF
```

**Example 2:**
```c++
int floor( float val )
{
    gsl_Expects( val >= std::numeric_limits<int>::lowest() && val <= std::numeric_limits<int>::max() );

    return gsl_lite::narrow_cast<int>( val );  // truncation is acceptable here
}
```


## Safe contiguous ranges

(Core Guidelines reference: [GSL.view: Views](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#gslview-views))

*gsl-lite* defines a class `gsl_lite::span<T, Extent>` that represents a contiguous sequence of objects. The interface
of `span<>` is identical to that of [`std::span<>`](https://en.cppreference.com/w/cpp/container/span),
but all operations in *gsl-lite*'s `span<>` use [`gsl_Expects()`](#contract-and-assertion-checks) to
check their preconditions at runtime. `span<>::iterator` also verifies the preconditions of all its operations
with `gsl_ExpectsDebug()`.

For C++14 and older, where [class template argument deduction](https://en.cppreference.com/w/cpp/language/class_template_argument_deduction)
is not available, *gsl-lite* defines a set of helper functions `make_span()` for explicitly constructing `span<>` objects.


## Bounds-checked element access

(Core Guidelines reference: [GSL.util: Utilities](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#gslutil-utilities))

The `gsl_lite::at()` function offers bounds-checked element access for all sized containers with random access.
Exposition-only definition:

```c++
template< class Container >
auto at( Container& c, index i )
{
    gsl_Expects( i >= 0 && i < std::ssize( c ) );
    return c[ i ];
}
```


## Integer type aliases

(Core Guidelines reference: [GSL.util: Utilities](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#gslutil-utilities))

(*Note:* `dim`, `stride`, and `diff` are *gsl-lite* extensions and not part of the C++ Core Guidelines.)

[Rule ES.107](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Res-subscripts) of the C++ Core Guidelines suggests
"Don't use `unsigned` for subscripts, prefer `gsl::index`", giving several good reasons for preferring a signed over an unsigned
type for indexing. For this purpose, the GSL defines `index` as a type alias for `std::ptrdiff_t`.

*gsl-lite* defines the `gsl_lite::index` type alias along with a few other aliases:

Type alias           | Purpose |
--------------------:|:----------------|
`gsl_lite::index`    | Signed integer type for indexes and subscripts |
`gsl_lite::dim`      | Signed integer type for sizes |
`gsl_lite::stride`   | Signed integer type for index strides |
`gsl_lite::diff`     | Signed integer type for index differences |

**Example:**
```c++
auto x = std::vector{ ... };
auto dx = std::vector( x.size() - 1 );
gsl_lite::dim n = std::ssize( x );
for ( gsl_lite::index i = 0; i < n - 1; ++i )
{
    y[ i ] = x[ i ] - x[ i - 1 ];
}
```

`index`, `dim`, `stride`, and `diff` are all aliases for `std::ptrdiff_t` unless the [`gsl_CONFIG_INDEX_TYPE`](#gsl_config_index_typestdptrdiff_t)
configuration macro is set to a different type (not recommended).


## String type aliases

(Core Guidelines reference: [GSL.view: Views](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#gslview-views))

(*Note:* `wzstring` and `cwzstring` are *gsl-lite* extensions and not part of the C++ Core Guidelines.)

*gsl-lite* defines the following aliases for C-style strings (that is, a zero-terminated sequence of characters or a `nullptr`):

Type alias  | Type              | Purpose                                       |
-----------:|:------------------|-----------------------------------------------|
`zstring`   | `char *`          | 0-terminated `char` string                    |
`czstring`  | `char const *`    | `const` view of 0-terminated `char` string    |
`wzstring`  | `wchar_t *`       | 0-terminated `wchar_t` string                 |
`cwzstring` | `wchar_t const *` | `const` view of 0-terminated `wchar_t` string |


## Ad-hoc resource management (C++11 and higher)

(Core Guidelines reference: [GSL.util: Utilities](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#gslutil-utilities))

(*Note:* `on_return()` and `on_error()` are *gsl-lite* extensions and not part of the C++ Core Guidelines.)

*gsl-lite* defines the following helpers for ad-hoc resource management:

- `gsl_lite::finally( action )` constructs and returns an object which invokes `action` upon destruction.
- `gsl_lite::on_return( action )` constructs and returns an object which invokes `action` upon destruction only if no exception was thrown.
- `gsl_lite::on_error( action )` constructs and returns an object which invokes `action` upon destruction only if an exception was thrown.

[Rule R.1](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#r1-manage-resources-automatically-using-resource-handles-and-raii-resource-acquisition-is-initialization)
of the C++ Core Guidelines suggests: "Manage resources automatically using resource handles and [RAII](https://en.cppreference.com/w/cpp/language/raii)
(Resource Acquisition Is Initialization)". While this advice is sound, it may sometimes be inconvenient to always define a resource handle type
for every situation in which a resource needs to be cleaned up.

For example, we might wish to modernize the following code which uses the [C standard library](https://en.cppreference.com/w/cpp/header/cstdio)
to read from a file:
```c++
std::vector<std::string> readLines( char const * filename )
{
    std::FILE * file = std::fopen( filename, "r" );
    if ( !file ) throw std::runtime_error(...);
    std::vector<std::string> result;
    ...  // implementation omitted
    std::fclose( file );
    return result;
}
```
This code is not exception-safe: if the (omitted) implementation throws an exception, `fclose()` is never called on the `file` handle.
The problem of exception safety is typically addressed by defining a [resource handle type for `FILE`](#nullability-and-the-moved-from-state).

Alternatively, we can fix the problem by using `gsl_lite::finally()`:
```c++
std::vector<std::string> readLines( char const * filename )
{
    std::FILE * file = std::fopen( filename, "r" );
    if ( !file ) throw std::runtime_error(...);
    auto _ = gsl_lite::finally( [&] { std::fclose( file ); } );
    std::vector<std::string> result;
    ...  // implementation omitted
    return result;
}
```
The destructor of the local object `_` will call `std::fclose( file )`, regardless of whether the function returns normally or whether
it was interrupted by an exception. This ensures that the `file` handle does not leak.


## Feature checking macros

(*Note:* Feature checking macros are a *gsl-lite* extension and not part of the C++ Core Guidelines.)

The following preprocessor macros can be used to identify features of the C++ build environment:

Name                                    | Notes           |
---------------------------------------:|:----------------|
**Language and library support:**       | &nbsp;          |
`gsl_CPPxx_OR_GREATER`                  | Whether C++xx language features are available<br>(substitute `11`, `14`, `17`, `20`, `23`, `26`) |
`gsl_STDLIB_CPPXX_OR_GREATER`           | Whether C++xx standard library features are available<br>(substitute `11`, `14`, `17`, `20`, `23`, `26`) |
**Compiler version detection:**       | &nbsp;          |
`gsl_BETWEEN( V, L, H )`                | V ≥ L and V < H |
`gsl_COMPILER_GNUC_VERSION`             | Evaluates to version number when compiled with GNU GCC, 0 otherwise |
`gsl_COMPILER_CLANG_VERSION`            | Evaluates to version number when compiled with Clang, 0 otherwise |
`gsl_COMPILER_MSVC_VERSION`             | Evaluates to version number when compiled with Microsoft Visual C++, 0 otherwise |
`gsl_COMPILER_APPLECLANG_VERSION`       | Evaluates to version number when compiled with Apple Clang, 0 otherwise |
`gsl_COMPILER_NVCC_VERSION`             | Evaluates to version number when compiled with NVIDIA NVCC, 0 otherwise |
`gsl_COMPILER_ARMCC_VERSION`            | Evaluates to version number when compiled with ARMCC, 0 otherwise |
`gsl_DEVICE_CODE`                       | Whether CUDA device code is being compiled |
`gsl_HAVE( EXCEPTIONS )`                | Evaluates to 1 if exceptions are available, 0 when compiling with exceptions disabled |
`gsl_HAVE( WCHAR )`                     | Evaluates to 1 if `wchar_t` type is available, 0 otherwise |

When a new revision of the C++ language is standardized, features often become available gradually in compilers and standard libraries.
C\+\+20 introduces a set of preprocessor macros to check for the availability of [language features](https://en.cppreference.com/w/cpp/feature_test)
and [standard library features](https://en.cppreference.com/w/cpp/feature_test#Library_features); but such macros are not necessarily available in
implementations predating C\+\+20. For this purpose, *gsl-lite* defines a limited set of feature checking macros.
They all follow the pattern `gsl_HAVE( xx )`, where `xx` is to be replaced by a token representing a given language feature. `gsl_HAVE( xx )` evaluates
to 1 if the corresponding language or library feature is available, 0 otherwise.

Name                                    | Notes           |
---------------------------------------:|:----------------|
**Language features:**<br>macro:        | &nbsp;<br>determines availability of:                    |
`gsl_HAVE( C99_PREPROCESSOR )`          | C99-compatible [preprocessor](https://en.cppreference.com/w/c/preprocessor) |
`gsl_HAVE( AUTO )`                      | [`auto`](https://en.cppreference.com/w/cpp/language/auto) (C++11) |
`gsl_HAVE( RVALUE_REFERENCE )`          | [rvalue references](https://en.cppreference.com/w/cpp/language/reference) (C++11) |
`gsl_HAVE( FUNCTION_REF_QUALIFIER )`    | [ref-qualified member functions](https://en.cppreference.com/w/cpp/language/member_functions) (C++11) |
`gsl_HAVE( ENUM_CLASS )`                | [`enum class`](https://en.cppreference.com/w/cpp/language/enum) (C++11) |
`gsl_HAVE( ALIAS_TEMPLATE )`            | [alias templates](https://en.cppreference.com/w/cpp/language/type_alias) (C++11) |
`gsl_HAVE( DEFAULT_FUNCTION_TEMPLATE_ARG )` | [default template arguments for function templates](https://en.cppreference.com/w/cpp/language/function_template) (C++11) |
`gsl_HAVE( EXPLICIT )`                  | [`explicit`](https://en.cppreference.com/w/cpp/language/explicit) specifier (C++11) |
`gsl_HAVE( VARIADIC_TEMPLATE )`         | [variadic templates](https://en.cppreference.com/w/cpp/language/pack) (C++11) |
`gsl_HAVE( IS_DELETE )`                 | [deleted functions](https://en.cppreference.com/w/cpp/keyword/delete) (C++11) |
`gsl_HAVE( IS_DEFAULT )`                | [explicitly defaulted functions](https://en.cppreference.com/w/cpp/keyword/default) (C++11) |
`gsl_HAVE( NOEXCEPT )`                  | [`noexcept`](https://en.cppreference.com/w/cpp/language/noexcept_spec) specifier and [`noexcept()`](https://en.cppreference.com/w/cpp/language/noexcept) operator (C++11) |
`gsl_HAVE( NORETURN )`                  | [`[[noreturn]]`](https://en.cppreference.com/w/cpp/language/attributes/noreturn) attribute (C++11) |
`gsl_HAVE( EXPRESSION_SFINAE )`         | [expression SFINAE](https://en.cppreference.com/w/cpp/language/sfinae) |
`gsl_HAVE( OVERRIDE_FINAL )`            | [`override`](https://en.cppreference.com/w/cpp/language/override) and [`final`](https://en.cppreference.com/w/cpp/language/final) specifiers (C++11) |
`gsl_HAVE( DECLTYPE_AUTO )`             | [`decltype(auto)`](https://en.cppreference.com/w/cpp/language/decltype) (C++11) |
`gsl_HAVE( DEPRECATED )`                | [`[[deprecated]]`](https://en.cppreference.com/w/cpp/language/attributes/deprecated) attribute (C++11) |
`gsl_HAVE( ENUM_CLASS_CONSTRUCTION_FROM_UNDERLYING_TYPE )` | constructing `enum class` from the underlying type (C++17) |
`gsl_HAVE( DEDUCTION_GUIDES )`          | [`class template argument deduction`](https://en.cppreference.com/w/cpp/language/class_template_argument_deduction) guides (C++17) |
`gsl_HAVE( NODISCARD )`                 | [`[[nodiscard]]`](https://en.cppreference.com/w/cpp/language/attributes/nodiscard) attribute (C++17) |
`gsl_HAVE( MAYBE_UNUSED )`              | [`[[maybe_unused]]`](https://en.cppreference.com/w/cpp/language/attributes/maybe_unused) attribute (C++17) |
`gsl_HAVE( CONSTEXPR_xx )`              | C++xx [`constexpr`](https://en.cppreference.com/w/cpp/language/constexpr) features (substitute `11`, `14`, `17`, `20`, `23`, `26`) |
**Standard library features:**<br>macro: | &nbsp;<br>determines availability of:                    |
`gsl_HAVE( ADDRESSOF )`                 | [`std::addressof()`](https://en.cppreference.com/w/cpp/memory/addressof) (C++11) |
`gsl_HAVE( ARRAY )`                     | [`std::array<>`](https://en.cppreference.com/w/cpp/container/array) (C++11) |
`gsl_HAVE( TYPE_TRAITS )`               | [`<type_traits>`](https://en.cppreference.com/w/cpp/header/type_traits) header (C++11) |
`gsl_HAVE( CONTAINER_DATA_METHOD )`     | `data()` member function on containers |
`gsl_HAVE( STD_DATA )`                  | [`std::data()`](https://en.cppreference.com/w/cpp/iterator/data) (C++17) |
`gsl_HAVE( STD_SSIZE )`                 | [`std::ssize()`](https://en.cppreference.com/w/cpp/iterator/size) (C++20) |
`gsl_HAVE( HASH )`                      | [`std::hash<>`](https://en.cppreference.com/w/cpp/utility/hash) (C++11) |
`gsl_HAVE( SIZED_TYPES )`               | [sized integer type aliases]() (C++11) |
`gsl_HAVE( SHARED_PTR )`                | [`std::shared_ptr<>`](https://en.cppreference.com/w/cpp/memory/shared_ptr) (C++11) |
`gsl_HAVE( UNIQUE_PTR )`                | [`std::unique_ptr<>`](https://en.cppreference.com/w/cpp/memory/unique_ptr) (C++11) |
`gsl_HAVE( MAKE_SHARED )`               | [`std::make_shared<>()`](https://en.cppreference.com/w/cpp/memory/shared_ptr/make_shared) (C++11) |
`gsl_HAVE( MAKE_UNIQUE )`               | [`std::make_unique<>()`](https://en.cppreference.com/w/cpp/memory/unique_ptr/make_unique) (C++14) |
`gsl_HAVE( MOVE_FORWARD )`              | [`std::move()`](https://en.cppreference.com/w/cpp/utility/move) and [`std::forward<>()`](https://en.cppreference.com/w/cpp/utility/forward) (C++11) |
`gsl_HAVE( NULLPTR )`                   | [`nullptr`](https://en.cppreference.com/w/cpp/language/nullptr) keyword (C++11) |
`gsl_HAVE( UNCAUGHT_EXCEPTIONS )`       | [`std::uncaught_exceptions()`](https://en.cppreference.com/w/cpp/error/uncaught_exception) (C++17) |
`gsl_HAVE( INITIALIZER_LIST )`          | [`std::initializer_list<>`](https://en.cppreference.com/w/cpp/utility/initializer_list) (C++11) |
`gsl_HAVE( REMOVE_CVREF )`              | [`std::remove_cvref<>`](https://en.cppreference.com/w/cpp/types/remove_cvref) (C++20) |


## Polyfills

(*Note:* Polyfills are a *gsl-lite* extension and not part of the C++ Core Guidelines.)

*gsl-lite* defines some macros, types, and functions for use with earlier versions of C++:

- [Keyword and attribute macros](#keyword-and-attribute-macros)
- [Code generation macros](#code-generation-macros)
- [Types and functions](#types-and-functions)

### Keyword and attribute macros

The keyword and attribute macros allow to conditionally take advantage of newer language features if available:

Name                        | Notes           |
---------------------------:|:----------------|
`gsl_constexpr`             | Expands to [`constexpr`](https://en.cppreference.com/w/cpp/language/constexpr) in C++11 and higher, to nothing otherwise |
`gsl_constexprXX`           | Expands to [`constexpr`](https://en.cppreference.com/w/cpp/language/constexpr) in C++XX and higher, to nothing otherwise<br>(substitute 14, 17, 20, 23, 26) |
`gsl_explicit`              | Expands to [`explicit`](https://en.cppreference.com/w/cpp/language/explicit) specifier in C++11 and higher, to nothing otherwise |
`gsl_is_delete`             | Expands to `= delete` in C++11 and higher, to nothing otherwise |
`gsl_is_delete_access`      | Expands to `public` in C++11 and higher, to `private` otherwise |
`gsl_noexcept`              | Expands to [`noexcept`](https://en.cppreference.com/w/cpp/language/noexcept_spec) specifier in C++11 and higher, to nothing otherwise |
`gsl_noexcept_if( expr )`   | Expands to [`noexcept( expr )`](https://en.cppreference.com/w/cpp/language/noexcept) operator in C++11 and higher, to nothing otherwise |
`gsl_nullptr`               | Expands to `nullptr` in C++11 and higher, to `NULL` otherwise |
`gsl_NORETURN`              | Expands to [`[[noreturn]]`](https://en.cppreference.com/w/cpp/language/attributes/noreturn) attribute in C++11 and higher, to a compiler-specific attribute if available, or to nothing otherwise |
`gsl_DEPRECATED`            | Expands to [`[[deprecated]]`](https://en.cppreference.com/w/cpp/language/attributes/deprecated) attribute in C++14 and higher, to nothing otherwise |
`gsl_DEPRECATED_MSG( msg )` | Expands to [`[[deprecated( msg )]]`](https://en.cppreference.com/w/cpp/language/attributes/deprecated) attribute in C++14 and higher, to nothing otherwise |
`gsl_NODISCARD`             | Expands to [`[[nodiscard]]`](https://en.cppreference.com/w/cpp/language/attributes/nodiscard) attribute in C++17 and higher, to nothing otherwise |
`gsl_MAYBE_UNUSED`          | Expands to [`[[maybe_unused]]`](https://en.cppreference.com/w/cpp/language/attributes/maybe_unused) attribute in C++17 and higher, or to nothing otherwise |
`gsl_MAYBE_UNUSED_MEMBER`   | Expands to [`[[maybe_unused]]`](https://en.cppreference.com/w/cpp/language/attributes/maybe_unused) attribute in C++17 and higher for compilers other than GNU GCC, or to nothing otherwise |
`gsl_NO_UNIQUE_ADDRESS`<br>(≥ C++20) | expands to [`[[msvc::no_unique_address]]`](https://devblogs.microsoft.com/cppblog/msvc-cpp20-and-the-std-cpp20-switch/) for MSVC, to [`[[no_unique_address]]`](https://en.cppreference.com/w/cpp/language/attributes/no_unique_address) otherwise |

### Code generation macros

The following macros help avoid writing repetitive code:

- `gsl_DEFINE_ENUM_BITMASK_OPERATORS( e )`:  
  Defines bitmask operators `\|`, `&`, `^`, `~`, `\|=`, `&=`, and `^=` for the enum type `e`.  
  Example:  
  ```c++
  enum class Vegetables
  {
      tomato   = 0b001,
      onion    = 0b010,
      eggplant = 0b100
  };
  gsl_DEFINE_ENUM_BITMASK_OPERATORS( Vegetables )
  ```

- `gsl_DEFINE_ENUM_RELATIONAL_OPERATORS( e )`:  
  Defines relational operators (`<=>` in C++20 and newer, `<`, `>`, `<=`, and `>=` otherwise) for the enum type `e`.  
  Example:  
  ```c++
  enum class OperatorPrecedence
  {
      additive = 0,
      multiplicative = 1,
      power = 2
  };
  gsl_DEFINE_ENUM_RELATIONAL_OPERATORS( OperatorPrecedence )
  ```

### Types and functions

The following types and functions implement some functionality added only in later C++ standards:

Name                                    | C++ feature     |
---------------------------------------:|:----------------|
`std11::add_const<>`                    | [`std::add_const<>`](https://en.cppreference.com/w/cpp/types/add_cv) (C++11) |
`std11::remove_const<>`<br>`std11::remove_volatile<>`<br>`std11::remove_cv<>` | [`std::remove_const<>`](https://en.cppreference.com/w/cpp/types/remove_cv) (C++11)<br>[`std::remove_volatile<>`](https://en.cppreference.com/w/cpp/types/remove_cv) (C++11)<br>[`std::remove_cv<>`](https://en.cppreference.com/w/cpp/types/remove_cv) (C++11) |
`std11::remove_reference<>`             | [`std::remove_reference<>`](https://en.cppreference.com/w/cpp/types/remove_reference) (C++11) |
`std11::integral_constant<>`<br>`std11::true_type`<br>`std11::false_type`<br>`std17::bool_constant<>`| [`std::integral_constant<>`](https://en.cppreference.com/w/cpp/types/integral_constant) (C++11)<br>[`std::true_type`](https://en.cppreference.com/w/cpp/types/integral_constant) (C++11)<br>[`std::false_type`](https://en.cppreference.com/w/cpp/types/integral_constant) (C++11)<br>[`std::bool_constant<>`](https://en.cppreference.com/w/cpp/types/integral_constant) (C++17) |
`std14::make_unique<>()`                | [`std::make_unique<>()`](https://en.cppreference.com/w/cpp/memory/unique_ptr/make_unique) (C++14) |
`std11::uncaught_exceptions()` (sic)    | [`std::uncaught_exceptions()`](https://en.cppreference.com/w/cpp/error/uncaught_exception) (C++17) |
`std17::negation<>`                     | [`std::negation<>`](https://en.cppreference.com/w/cpp/types/negation) (C++17) |
`std17::conjunction<>` (≥ C\+\+11)      | [`std::conjunction<>`](https://en.cppreference.com/w/cpp/types/conjunction) (C++17) |
`std17::disjunction<>` (≥ C\+\+11)      | [`std::disjunction<>`](https://en.cppreference.com/w/cpp/types/disjunction) (C++17) |
`std17::void_t<>` (≥ C\+\+11)           | [`std::void_t<>`](https://en.cppreference.com/w/cpp/types/void_t) (C++17) |
`std17::size()`<br>`std20::ssize()`     | [`std::size()`](https://en.cppreference.com/w/cpp/iterator/size) (C++17)<br>[`std::ssize()`](https://en.cppreference.com/w/cpp/iterator/size) (C++20) |
`std17::data()`                         | [`std::data()`](https://en.cppreference.com/w/cpp/iterator/data) (C++17) |
`std20::endian`                         | [`std::endian`](https://en.cppreference.com/w/cpp/types/endian) (C++20) |
`std20::type_identity<>`                | [`std::type_identity<>`](https://en.cppreference.com/w/cpp/types/type_identity) (C++20) |
`std20::identity`                       | [`std::identity`](https://en.cppreference.com/w/cpp/utility/functional/identity) (C++20) |
`std20::remove_cvref<>`                 | [`std::remove_cvref<>`](https://en.cppreference.com/w/cpp/types/remove_cvref) (C++20) |


## Configuration options and switches

- [Contract checking configuration macros](#contract-checking-configuration-macros)
    - [Runtime enforcement](#runtime-enforcement)
    - [Contract violation handling](#contract-violation-handling)
    - [Unenforced contract checks](#unenforced-contract-checks)
- [Feature selection macros](#feature-selection-macros)
- [Other configuration macros](#other-configuration-macros)

*gsl-lite* is customizable through a large number of configuration options and switches.
The configuration of contract checks may be useful for various purposes (performance, unit testing, fail-safe environments).
The main purpose of the other configuration options is backward compatibility.

The configuration macros may affect the API and ABI of *gsl-lite* in ways that renders it incompatible with other code.
Therefore, as a general rule, **do not define, or rely on, any of *gsl-lite*'s configuration options or switches when using *gsl-lite* in a library**.

### Contract checking configuration macros

With the configuration macros described in the following sections, the user can exert fine-grained control over the runtime behavior
of contract checks expressed with [`gsl_Expects()`, `gsl_Ensures()`, `gsl_Assert()` and other contract checking macros](#contract-and-assertion-checks).
The configuration options for contract violation response follow the suggestions originally suggested in proposal [N4415](http://wg21.link/n4415),
with some refinements inspired by [P1710](http://wg21.link/P1710)/[P1730](http://wg21.link/P1730).


#### Runtime enforcement

The following macros control whether contracts are checked at runtime:

- **`gsl_CONFIG_CONTRACT_CHECKING_AUDIT`**  
  Define this macro to have contracts expressed with `gsl_ExpectsAudit()`, `gsl_EnsuresAudit()`, and `gsl_AssertAudit()` checked
  at runtime.

- **`gsl_CONFIG_CONTRACT_CHECKING_ON` (default)**  
  Define this macro to have contracts expressed with `gsl_Expects()`, `gsl_Ensures()`, `gsl_Assert()`, and `gsl_FailFast()`
  checked at runtime, Contracts expressed with `gsl_ExpectsDebug()`, `gsl_EnsuresDebug()`, and `gsl_AssertDebug()` are also
  checked at runtime (unless `NDEBUG` is defined and `gsl_CONFIG_CONTRACT_CHECKING_AUDIT` is not). **This is the default.**
 
- **`NDEBUG`**
  This macro traditionally disables runtime checks for the [`assert()`](https://en.cppreference.com/w/c/error/assert) macro from
  the C standard library. Additionally, contracts expressed with `gsl_ExpectsDebug()`, `gsl_EnsuresDebug()`, and `gsl_AssertDebug()`
  are not evaluated or checked at runtime if `NDEBUG` is defined and `gsl_CONFIG_CONTRACT_CHECKING_AUDIT` is not.

- **`gsl_CONFIG_CONTRACT_CHECKING_OFF`**  
  Define this macro to disable all runtime checking of contracts and invariants.  
    
  Note that `gsl_FailFast()` checks will trigger runtime failure even if runtime checking is disabled.

- If desired, the macros **`gsl_CONFIG_DEVICE_CONTRACT_CHECKING_AUDIT`**, **`gsl_CONFIG_DEVICE_CONTRACT_CHECKING_ON`**, and
  **`gsl_CONFIG_DEVICE_CONTRACT_CHECKING_OFF`** can be used to configure contract checking for CUDA device code separately. If
  neither of these macros is defined, device code uses the same configuration as host code.  
    
  It may be reasonable to define `gsl_CONFIG_DEVICE_CONTRACT_CHECKING_OFF` in Release builds because the performance impact of
  runtime checks can be grave in device code, while it is often negligible in host code.

The following macros can be used to selectively disable checking for a particular kind of contract:

- **`gsl_CONFIG_CONTRACT_CHECKING_EXPECTS_OFF`**  
  Define this macro to disable runtime checking of precondition contracts expressed with `gsl_Expects()`, `gsl_ExpectsDebug()`, and `gsl_ExpectsAudit()`.

- **`gsl_CONFIG_CONTRACT_CHECKING_ENSURES_OFF`**  
  Define this macro to disable runtime checking of postcondition contracts expressed with `gsl_Ensures()`, `gsl_EnsuresDebug()`, and `gsl_EnsuresAudit()`.

- **`gsl_CONFIG_CONTRACT_CHECKING_ASSERT_OFF`**  
  Define this macro to disable runtime checking of assertions expressed with `gsl_Assert()`, `gsl_AssertDebug()`, and `gsl_AssertAudit()`.


#### Contract violation handling

The following macros control the handling of runtime contract violations:

- **`gsl_CONFIG_CONTRACT_VIOLATION_ASSERTS` (default)**  
  If this macro is defined, and if the `assert()` macro is available for runtime checks (that is, if `NDEBUG` is not defined),
  contract checking macros are implemented in terms of `assert()`. If `assert()` is unavailable (i.e. if `NDEBUG` was defined),
  `std::abort()` is called directly when a contract is violated. **This is the default.**  
    
  This option may be preferable over `gsl_CONFIG_CONTRACT_VIOLATION_TERMINATES` because `assert()` prints diagnostic information
  (such as the current source file, a line number, and the function name), and because vendor-specific extensions of `assert()`
  can be used (for instance, the `assert()` implementation of the Microsoft C runtime displays a dialog box which permits breaking
  into the debugger or continuing execution).  
    
  Note that `gsl_FailFast()` will call `std::abort()` if `assert()` continues execution.

- **`gsl_CONFIG_CONTRACT_VIOLATION_TERMINATES`**  
  Define this macro to call `std::terminate()` on a contract violation.  
    
  Termination on contract violation is the behavior specified by the Core Guidelines for GSL contract checks.

- **`gsl_CONFIG_CONTRACT_VIOLATION_TRAPS`**  
  Define this macro to execute a trap instruction on a contract violation.  
    
  Trap instructions may yield smaller codegen and can thus result in better-performing code. However, they usually lead to
  catastrophic failure and may be difficult to diagnose for some platforms.

- **`gsl_CONFIG_CONTRACT_VIOLATION_THROWS`**  
  Define this macro to throw a `std::runtime_error`-derived exception `gsl_lite::fail_fast` on contract violation.  
  Handling contract violations with exceptions can be desirable when executing in an interactive programming environment, or if
  there are other reasons why process termination must be avoided.  
    
  This setting is also useful for writing unit tests that exercise contract checks.

- **`gsl_CONFIG_CONTRACT_VIOLATION_CALLS_HANDLER`**  
  Define this macro to call a user-defined handler function `gsl_lite::fail_fast_assert_handler()` on a contract violation.
  The user must provide a definition of the following function:
  ```c++
  namespace gsl_lite_ {
      gsl_api void fail_fast_assert_handler(
          char const * expression, char const * message,
          char const * file, int line );
  } // namespace gsl_lite
  ```  
    
  Note that `gsl_FailFast()` will call `std::terminate()` if `fail_fast_assert_handler()` returns.

- If desired, the macros **`gsl_CONFIG_DEVICE_CONTRACT_VIOLATION_ASSERTS`**, **`gsl_CONFIG_DEVICE_CONTRACT_VIOLATION_TRAPS`**, and
  **`gsl_CONFIG_DEVICE_CONTRACT_VIOLATION_CALLS_HANDLER`** can be used to configure contract violation handling for CUDA device
  code separately. If neither of these macros is defined, device code uses the following defaults:
  - `gsl_CONFIG_CONTRACT_VIOLATION_TERMINATES` → `gsl_CONFIG_DEVICE_CONTRACT_VIOLATION_ASSERTS`
  - `gsl_CONFIG_CONTRACT_VIOLATION_ASSERTS` → `gsl_CONFIG_DEVICE_CONTRACT_VIOLATION_ASSERTS`
  - `gsl_CONFIG_CONTRACT_VIOLATION_THROWS` → `gsl_CONFIG_DEVICE_CONTRACT_VIOLATION_ASSERTS`
  - `gsl_CONFIG_CONTRACT_VIOLATION_TRAPS` → `gsl_CONFIG_DEVICE_CONTRACT_VIOLATION_TRAPS`
  - `gsl_CONFIG_CONTRACT_VIOLATION_CALLS_HANDLER` → `gsl_CONFIG_DEVICE_CONTRACT_VIOLATION_CALLS_HANDLER`


#### Unenforced contract checks

The following macros control what happens with individual contract checks which are not enforced at runtime. Note that these
macros do not disable runtime contract checking; they only configure what happens to contracts which are not checked as a result
of configuration, e.g. for any contract check if `gsl_CONFIG_CONTRACT_CHECKING_OFF` is defined, or for audit-level and debug-level
contract checks if `NDEBUG` is defined.

- **`gsl_CONFIG_UNENFORCED_CONTRACTS_ELIDE` (default)**  
  Contract checks disabled by configuration will be discarded. **This is the default.**  
    
  Note that `gsl_FailFast()` calls are never discarded.
  
  Even for discarded contract checks, *gsl-lite* will by default still verify that the contract check forms a valid Boolean
  expression by using the C++11 features `decltype()` and `static_assert()`. This may lead to problems if the contract check
  expression cannot be used in an unevaluated context, for instance, when using a lambda expression in C++11/14/17.
  
  The compile-time verification of contract check expressions is controlled by the configuration macro
  `gsl_CONFIG_VALIDATES_UNENFORCED_CONTRACT_EXPRESSIONS`, which defaults to *`1`*. To suppress the verification, define
  `gsl_CONFIG_VALIDATES_UNENFORCED_CONTRACT_EXPRESSIONS=0`.

- **`gsl_CONFIG_UNENFORCED_CONTRACTS_ASSUME`**  
  For contracts expressed with `gsl_Expects()`, `gsl_Ensures()`, and `gsl_Assert()` which are not checked as a result of
  configuration, instruct the compiler to assume that they always hold true. This is expressed with compiler-specific intrinsics
  such as `__assume()`.  
    
  Contract checks expressed with `gsl_ExpectsDebug()`, `gsl_EnsuresDebug()`, `gsl_AssertDebug()` , `gsl_ExpectsAudit()`,
  `gsl_EnsuresAudit()`, and `gsl_AssertAudit()` which are not checked at runtime (due to definition of `NDEBUG` or one of the
  aforementioned configuration macros) are discarded.  
    
  Explicitly injecting the assumption that contracts hold true implies that violating contracts causes undefined behavior. This
  may give the compiler more opportunities for optimization, but it is usually dangerous and, like all occurrences of undefined
  behavior, it can have devastating consequences.  
    
  The use of compiler-specific "assume" intrinsics may lead to spurious runtime evaluation of contract expressions. Because
  *gsl-lite* implements contract checks with macros (rather than as a language feature as the defunct C++2a Contracts proposal
  did), it cannot reliably suppress runtime evaluation for all compilers. For instance, if the contract check fed to the "assume"
  intrinsic comprises a function call which is opaque to the compiler, many compilers will generate the runtime function call.
  Therefore, `gsl_Expects()`, `gsl_Ensures()`, and `gsl_Assert()` should be used only for conditions that can be proven
  side-effect-free by the compiler, and `gsl_ExpectsDebug()`, `gsl_EnsuresDebug()`, `gsl_AssertDebug()`, `gsl_ExpectsAudit()`,
  `gsl_EnsuresAudit()`, and `gsl_AssertAudit()` for everything else. In practice, this means that `gsl_Expects()`,
  `gsl_Ensures()`, and `gsl_Assert()` should only be used for simple comparisons of scalar values, for simple inlineable getters,
  and for comparisons of class objects with trivially inlineable comparison operators.  
    
  Revisiting the [example given above](##contract-and-assertion-checks):  
  ```c++
  template< class RandomIt >
  auto median( RandomIt first, RandomIt last )
  {
          // Comparing iterators for equality boils down to a comparison of pointers. An optimizing
          // compiler will inline the comparison operator and understand that the comparison is free of
          // side-effects, and hence generate no code in `gsl_CONFIG_UNENFORCED_CONTRACTS_ASSUME` mode.
      gsl_Expects( first != last );
  
          // If we cannot trust the compiler to understand that this function call is free of
          // side-effects, we should use `gsl_ExpectsDebug()` or `gsl_ExpectsAudit()`. This particular
          // function call is expensive, so we use an audit-level contract check.
      gsl_ExpectsAudit( std::is_sorted( first, last ) );
  
      auto count = last - first;
      return count % 2 != 0
          ? first[ count / 2 ]
          : std::midpoint( first[ count / 2 ], first[ count / 2 + 1 ] );
  }
  ```

- If desired, the macros **`gsl_CONFIG_DEVICE_UNENFORCED_CONTRACTS_ELIDE`** and **`gsl_CONFIG_DEVICE_UNENFORCED_CONTRACTS_ASSUME`**
  can be used to configure handling of unenforced contract checks for CUDA device code separately. If neither of these macros is
  defined, device code uses the same configuration as host code.


### Feature selection macros

#### `gsl_FEATURE_GSL_COMPATIBILITY_MODE=0`

To minimize the impact of the breaking changes, *gsl-lite* introduces an optional *GSL compatibility mode* controlled by the new configuration switch
`gsl_FEATURE_GSL_COMPATIBILITY_MODE`, which is is disabled by default and can be enabled by defining `gsl_FEATURE_GSL_COMPATIBILITY_MODE=1`.
**Default is 0.**

If the GSL compatibility mode is enabled, *gsl-lite* additionally makes the following global definitions:
```c++
namespace gsl = ::gsl_lite;
#define Expects( x )  gsl_Expects( x )
#define Ensures( x )  gsl_Ensures( x )
```

The GSL compatibility mode precludes the use of *gsl-lite* and Microsoft GSL in the same translation unit. Therefore, when making use of *gsl-lite*
in a public header file of a library, the GSL compatibility mode should not be enabled.

The GSL compatibility mode causes no link-time interference between *gsl-lite* and as Microsoft GSL. Both libraries may be used in the same project as
long as no translation unit includes both at the same time.

The legacy header file `<gsl/gsl-lite.hpp>` now forwards to `<gsl-lite/gsl-lite.hpp>` and implicitly enables the GSL compatibility mode. When the legacy
header is included, it emits a warning message which urges to either migrate to header `<gsl-lite/gsl-lite.hpp>`, namespace `gsl_lite` and the prefixed
contract checking macros `gsl_Expects()` and `gsl_Ensures()`, or to explicitly request GSL compatibility by defining `gsl_FEATURE_GSL_COMPATIBILITY_MODE=1`.

#### `gsl_FEATURE_EXPERIMENTAL_RETURN_GUARD=0`
Provide experimental types `final_action_return<>` and `final_action_error<>` and convenience functions `on_return()` and `on_error()`. **Default is 0.**

#### `gsl_FEATURE_STRING_SPAN=0`
String spans and related functionality are no longer part of the GSL specification. If the macro `gsl_FEATURE_STRING_SPAN` is set to 1, *gsl-lite*
continues to provide an implementation of the class `basic_string_span<>` along with the aliases `string_span`, `cstring_span`, `wstring_span`, `cwstring_span`,
the deprecated class `basic_zstring_span<>` with the aliases `zstring_span`, `czstring_span`, `wzstring_span`, `cwzstring_span`, and related classes and
functions such as `to_string()`, and `ensure_z()`. **Default is 0.**

#### `gsl_FEATURE_BYTE=0`
The `byte` type has been superseded by [`std::byte`](https://en.cppreference.com/w/cpp/types/byte) in C++17 and thus is no longer part of the GSL specification.
If the macro `gsl_FEATURE_BYTE` is set to 1, *gsl-lite* continues to provide an implementation of `byte` and related functions such as `as_bytes()`, `to_byte()`,
`as_bytes()`, and `as_writable_bytes()`. **Default is 0.**

#### `gsl_FEATURE_WITH_CONTAINER_TO_STD=0`
Define this to the highest C++ standard (98, 3, 11, 14, 17, 20) you want to include tagged-construction via `with_container`, or 0 to disable the feature. **Default is 0.**

#### `gsl_FEATURE_MAKE_SPAN_TO_STD=99`
Define this to the highest C++ standard (98, 3, 11, 14, 17, 20) you want to include `make_span()` creator functions, or 0 to disable the feature. **Default is 99 for inclusion with any standard.**

#### `gsl_FEATURE_BYTE_SPAN_TO_STD=99`
Define this to the highest C++ standard (98, 3, 11, 14, 17, 20) you want to include `byte_span()` creator functions, or 0 to disable the feature. **Default is 99 for inclusion with any standard.**


### Other configuration macros

#### `gsl_CONFIG_ACKNOWLEDGE_NONSTANDARD_ABI=0`
Define this to 1 to explicitly acknowledge that you are using *gsl-lite* with a non-standard ABI and that you control the build flags of all components linked into your target.
**Default is 0.**

#### `gsl_api`

Functions in *gsl-lite* are decorated with `gsl_api` where appropriate. **By default `gsl_api` is defined empty for non-CUDA platforms and `__host__ __device__` for the CUDA platform.**
Define this macro to specify your own function decoration. 

**NOTE:** When a custom `gsl_api` macro is defined, *gsl-lite* emits a warning to notify the programmer that this alters the binary interface of *gsl-lite*, leading to possible ODR violations.
The warning can be explicitly overridden by defining `gsl_CONFIG_ACKNOWLEDGE_NONSTANDARD_ABI=1`.

#### `gsl_CPLUSPLUS`
Define this macro to override the auto-detection of the supported C++ standard if your compiler does not set the `__cplusplus` macro correctly.

#### `gsl_CONFIG_DEPRECATE_TO_LEVEL=9`
Define this to and including the level you want deprecation; see table [Deprecated features](#deprecated-features) below. **Default is 9.**

#### `gsl_CONFIG_SPAN_INDEX_TYPE=std::size_t`
Define this macro to the type to use for indices in `span<>` and `basic_string_span<>`. **Default is `std::size_t`.**

**NOTE:** When a custom span index type is defined, *gsl-lite* emits a warning to notify the programmer that this alters the binary interface of *gsl-lite*, leading to possible ODR violations.
The warning can be explicitly overridden by defining `gsl_CONFIG_ACKNOWLEDGE_NONSTANDARD_ABI=1`.

#### `gsl_CONFIG_INDEX_TYPE=std::ptrdiff_t`
Define this macro to the type to use for `gsl_lite::index`. **Default is `std::ptrdiff_t`.**

**NOTE:** When a custom index type is defined, *gsl-lite* emits a warning to notify the programmer that this alters the binary interface of *gsl-lite*, leading to possible ODR violations.
The warning can be explicitly overridden by defining `gsl_CONFIG_ACKNOWLEDGE_NONSTANDARD_ABI=1`.

#### `gsl_CONFIG_NOT_NULL_EXPLICIT_CTOR=1`
Define this macro to 0 to make `not_null<>`'s constructor implicit. **Default is 1.**

Preferably, rather than defining this macro to 0, use [`not_null_ic<>`](#not_null_icp) if you desire implicit construction.

#### `gsl_CONFIG_TRANSPARENT_NOT_NULL=1`
If this macro is defined to 1, `not_null<>` supports typical member functions of the underlying smart pointer transparently (currently `get()`), while adding precondition checks.
This is conformant behavior but may be incompatible with older code which expects that `not_null<>::get()` returns the underlying pointer itself. **Default is 1.**

#### `gsl_CONFIG_NOT_NULL_GET_BY_CONST_REF=0`
Define this macro to 1 to have the legacy non-transparent version of `not_null<>::get()` return `T const &` instead of `T`. This may improve performance with types that have an expensive copy-constructor.
This macro must not be defined if `gsl_CONFIG_TRANSPARENT_NOT_NULL` is 1. **Default is 0 for `T`.**

#### `gsl_CONFIG_ALLOWS_SPAN_COMPARISON=0`
Define this macro to 1 to support equality comparison and relational comparison of spans. C++20 `std::span<>` does not support comparison because semantics (deep vs. shallow) are unclear. **Default is 0.**

#### `gsl_CONFIG_ALLOWS_NONSTRICT_SPAN_COMPARISON=0`
Define this macro to 1 to support equality comparison and relational comparison of spans of different types, e.g. of different const-volatile-ness. To be able to compare a string_span with a cstring_span, non-strict span comparison must be available. **Default is 0.**

#### `gsl_CONFIG_ALLOWS_UNCONSTRAINED_SPAN_CONTAINER_CTOR=0`
Define this macro to 1 to add the unconstrained span constructor for containers for pre-C++11 compilers that cannot constrain the constructor. This constructor may prove too greedy and interfere with other constructors. **Default is 0.**

Note: an alternative is to use the constructor tagged `with_container`: `span<V> s(gsl_lite::with_container, cont)`.

#### `gsl_CONFIG_NARROW_THROWS_ON_TRUNCATION=1`
If this macro is 1, `narrow<>()` always throws a `narrowing_error` exception if the narrowing conversion loses information due to truncation.
If `gsl_CONFIG_NARROW_THROWS_ON_TRUNCATION` is 0 and `gsl_CONFIG_CONTRACT_VIOLATION_THROWS` is not defined, `narrow<>()` instead terminates on
information loss (using `std::terminate()` if available and a trap instruction otherwise, e.g. for CUDA device code). **Default is 1.**

**NOTE:** When `gsl_CONFIG_NARROW_THROWS_ON_TRUNCATION` is defined as 0, *gsl-lite* emits a warning to notify the programmer that this may lead to possible ODR violations.
The warning can be explicitly overridden by defining `gsl_CONFIG_ACKNOWLEDGE_NONSTANDARD_ABI=1`.

#### `gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS=0`
Define this macro to 1 to experience the by-design compile-time errors of the GSL components in the test suite. **Default is 0.**


## Configuration changes, deprecated and removed features

- [Configuration changes](#configuration-changes)
- [Deprecated features](#deprecated-features)
- [Removed features](#removed-features)

### Configuration changes

*gsl-lite* v1.0 changed the default values for several configuration options and switches:

  - [`gsl_FEATURE_STRING_SPAN`](#gsl_feature_string_span0):  
    Version-1 default: `gsl_FEATURE_STRING_SPAN=0`  
    Version-0 default: `gsl_FEATURE_STRING_SPAN=1`  
    Reason: string spans are no longer part of the GSL specification.

  - [`gsl_FEATURE_BYTE`](#gsl_feature_byte0):  
    Version-1 default: `gsl_FEATURE_BYTE=0`  
    Version-0 default: `gsl_FEATURE_BYTE=1`  
    Reason: `byte` has been superseded by [`std::byte`](https://en.cppreference.com/w/cpp/types/byte) in C++17.

  - [`gsl_CONFIG_DEPRECATE_TO_LEVEL`](#gsl_config_deprecate_to_level9):  
    Version-1 default: `gsl_CONFIG_DEPRECATE_TO_LEVEL=9`
    Version-0 default: `gsl_CONFIG_DEPRECATE_TO_LEVEL=0`  

  - [`gsl_CONFIG_INDEX_TYPE`](#gsl_config_index_typestdptrdiff_t):  
    Version-1 default: `std::ptrdiff_t`  
    Version-0 default: `gsl_CONFIG_SPAN_INDEX_TYPE` (defaults to `std::size_t`)  
    Reason: the GSL specifies `gsl_lite::index` to be a signed type.

  - [`gsl_CONFIG_ALLOWS_SPAN_COMPARISON`](#gsl_config_allows_span_comparison0):  
    Version-1 default: `gsl_CONFIG_ALLOWS_SPAN_COMPARISON=0`  
    Version-0 default: `gsl_CONFIG_ALLOWS_SPAN_COMPARISON=1`  
    Reason: C++20 `std::span<>` does not support comparison because semantics (deep vs. shallow) are unclear.

  - [`gsl_CONFIG_NOT_NULL_EXPLICIT_CTOR`](#gsl_config_not_null_explicit_ctor1):  
    Version-1 default: `gsl_CONFIG_NOT_NULL_EXPLICIT_CTOR=1`  
    Version-0 default: `gsl_CONFIG_NOT_NULL_EXPLICIT_CTOR=0`  
    Reason: see [M-GSL/#395](https://github.com/Microsoft/GSL/issues/395). (Note that `not_null<>` in Microsoft GSL has an implicit constructor, cf. [M-GSL/#699](https://github.com/Microsoft/GSL/issues/699).)

  - [`gsl_CONFIG_TRANSPARENT_NOT_NULL`](#gsl_config_transparent_not_null1):  
    Version-1 default: `gsl_CONFIG_TRANSPARENT_NOT_NULL=1`  
    Version-0 default: `gsl_CONFIG_TRANSPARENT_NOT_NULL=0`  
    Reason: enables conformant behavior for `not_null<>::get()`.

  - [`gsl_CONFIG_NARROW_THROWS_ON_TRUNCATION`](#gsl_config_narrow_throws_on_truncation1):  
    Version-1 default: `gsl_CONFIG_NARROW_THROWS_ON_TRUNCATION=1`  
    Version-0 default: `gsl_CONFIG_NARROW_THROWS_ON_TRUNCATION=0`  
    Reason: enables conformant behavior for `narrow<>()` (cf. [#52](https://github.com/gsl-lite/gsl-lite/issues/52)).

  - [default runtime contract violation handling](#contract-checking-configuration-macros):  
    Version-1 default: `gsl_CONFIG_CONTRACT_VIOLATION_ASSERTS`  
    Version-0 default: `gsl_CONFIG_CONTRACT_VIOLATION_TERMINATES`  
    Reason: the mode enabled by `gsl_CONFIG_CONTRACT_VIOLATION_ASSERTS` is consistent with the behavior of the `assert()` macro
    while retaining runtime contract checks even if `NDEBUG` is defined.


### Deprecated features

The following features are deprecated since the indicated version. See macro [`gsl_CONFIG_DEPRECATE_TO_LEVEL`](#other-configuration-macros) on how to control deprecation using the indicated level.

Version | Level | Feature / Notes |
-------:|:-----:|:----------------|
 1.0.0  |   9   | `span<>::as_span<>()` (unsafe) |
0.41.0  |   7   | `basic_string_span<>`, `basic_zstring_span<>` and related aliases<br>(no longer part of the C++ Core Guidelines specification) |
0.35.0  |   -   | `gsl_CONFIG_CONTRACT_LEVEL_ON`, `gsl_CONFIG_CONTRACT_LEVEL_OFF`, `gsl_CONFIG_CONTRACT_LEVEL_EXPECTS_ONLY` and `gsl_CONFIG_CONTRACT_LEVEL_ENSURES_ONLY`<br>(use `gsl_CONFIG_CONTRACT_CHECKING_ON`, `gsl_CONFIG_CONTRACT_CHECKING_OFF`, &nbs;`gsl_CONFIG_CONTRACT_CHECKING_ENSURES_OFF`, `gsl_CONFIG_CONTRACT_CHECKING_EXPECTS_OFF` instead) |
0.7.0   |   -   | `gsl_CONFIG_ALLOWS_SPAN_CONTAINER_CTOR`<br>(use `gsl_CONFIG_ALLOWS_UNCONSTRAINED_SPAN_CONTAINER_CTOR` instead,<br>or consider `span(with_container, cont)`) |


### Removed features

The following features are removed since the indicated version.

Version | Feature / Notes |
-------:|:----------------|
1.0.0   | `finally()`, `on_return()`, and `on_error()` for pre-C++11        |
&nbsp;  | `Owner()` and `implicit` macros |
&nbsp;  | `basic_string_span<>`, `basic_zstring_span<>` and related aliases |
&nbsp;  | `as_writeable_bytes()`, call indexing for spans, and `span::at()` |
&nbsp;  | `gsl_CONFIG_CONTRACT_LEVEL_ON`, `gsl_CONFIG_CONTRACT_LEVEL_OFF`, `gsl_CONFIG_CONTRACT_LEVEL_EXPECTS_ONLY` and `gsl_CONFIG_CONTRACT_LEVEL_ENSURES_ONLY`<br>(use `gsl_CONFIG_CONTRACT_CHECKING_ON`, `gsl_CONFIG_CONTRACT_CHECKING_OFF`, `gsl_CONFIG_CONTRACT_CHECKING_ENSURES_OFF`, `gsl_CONFIG_CONTRACT_CHECKING_EXPECTS_OFF` instead) |
&nbsp;  | `span( std::nullptr_t, index_type )`<br>(`span( pointer, index_type )` is called instead) |
&nbsp;  | `span( U *, index_type )`<br>(`span( pointer, index_type )` is called instead) |
&nbsp;  | `span( std::shared_ptr<T> const & p )` |
&nbsp;  | `span( std::unique_ptr<T> const & p )` |
&nbsp;  | `span<>::length()`<br>(use `span<>::size()` instead) |
&nbsp;  | `span<>::length_bytes()`<br>(use `span<>::size_bytes()` instead) |
&nbsp;  | member `span<>::as_bytes()`, `span<>::as_writeable_bytes()` |
