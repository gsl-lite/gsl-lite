# *gsl-lite*: defensive programming utilities for C++98, C++11 up


| metadata | build  | packages | try online |
| -------- | ------ | -------- | ---------- |
| [![Language](https://badgen.net/badge/C++/98,11+/blue)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization) <br> [![License](https://badgen.net/github/license/gsl-lite/gsl-lite)](https://opensource.org/licenses/MIT) <br> [![Version](https://badgen.net/github/release/gsl-lite/gsl-lite)](https://github.com/gsl-lite/gsl-lite/releases)   |   [![Azure Pipelines build status](https://dev.azure.com/gsl-lite/gsl-lite/_apis/build/status/gsl-lite.gsl-lite?branchName=master)](https://dev.azure.com/gsl-lite/gsl-lite/_build/latest?definitionId=1&branchName=master) <br> [![AppVeyor build status](https://ci.appveyor.com/api/projects/status/1v6eqy68m8g7tm06?svg=true)](https://ci.appveyor.com/project/gsl-lite/gsl-lite)   |   [![Vcpkg](https://badgen.net/badge/latest/on%20Vcpkg/blue)](https://vcpkg.io/en/package/gsl-lite) <br> [![single header](https://badgen.net/badge/latest/single%20header/blue)](https://raw.githubusercontent.com/gsl-lite/gsl-lite/master/include/gsl/gsl-lite.hpp)   |   [![Try it on Compiler Explorer](https://badgen.net/badge/on/Compiler%20Explorer/blue)](https://gcc.godbolt.org/z/6frqr8PsK) <br> [![Try it on Wandbox](https://badgen.net/badge/on/Wandbox/blue)](https://wandbox.org/permlink/OIU7oseCTLsiNQ8A)   |


***gsl-lite*** is a portable, single-file, header-only library for defensive programming based on the [C++ Core Guidelines Support Library](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-gsl) specification.

**Example code:**
```c++
#include <memory>
#include <utility>
#include <numeric>

#include <gsl-lite/gsl-lite.hpp>

float mean( gsl_lite::span<float const> values )  // contiguous range with bounds checks
{
    gsl_Expects( !values.empty() );  // precondition check

    double sum = std::accumulate( values.begin(), values.end(), 0. );
    double result = sum / std::ssize( values );
    return gsl_lite::narrow_failfast<float>( result );  // checked numeric cast
}

class Resource { };

gsl_lite::not_null<std::unique_ptr<Resource>>  // type-encoded postcondition
acquireResource()
{
    return gsl_lite::make_unique<Resource>();  // `make_unique<>()` with non-nullable result
}

void consumeResource(
    gsl_lite::not_null<std::unique_ptr<Resource>> resource );  // type-encoded precondition
```

*gsl-lite* strives to implement the [Guidelines Support Library specification](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-gsl) of the C++ Core Guidelines
maintained by the [Standard C++ Foundation](https://isocpp.org/).
The library is originally based on [Microsoft GSL](https://github.com/microsoft/gsl) and was adapted for C++98, C++03. It also works when compiled as C++11, C++14, C++17, C++20, or C++23.

*gsl-lite* does not interfere with Microsoft GSL since both libraries live in different namespaces (`gsl_lite` vs. `gsl`) and employ different macro prefixes (`gsl_` vs. `GSL_`).

*gsl-lite* recognizes when it is compiled for the CUDA platform and decorates some functions with `__host__` and `__device__` accordingly.

*gsl-lite* has no other dependencies than the [C++ standard library](http://en.cppreference.com/w/cpp/header).


## Documentation

- [Why *gsl-lite*?](#why-gsl-lite):
- [Installation and use](doc/Installation-and-use.md):
    - [Getting started](doc/Installation-and-use.md#getting-started)
    - [Migration guide](doc/Installation-and-use.md#migration-guide)
    - [Reported to work with](doc/Installation-and-use.md#reported-to-work-with)
    - [Using *gsl-lite* in libraries](#using-gsl-lite-in-libraries)
- [Features](doc/Features.md)
<!--    - [Contract and assertion checks](doc/Features.md#contract-and-assertion-checks): `gsl_Expects()`, `gsl_Ensures()`, `gsl_Assert()`, and more
    - [Pointer annotations](doc/Features.md#pointer-annotations): `owner<P>`, `not_null<P>`, and `not_null_ic<P>`
    - [Numeric type conversions](doc/Features.md#numeric-type-conversions): `narrow<T>(U)`, `narrow_failfast<T>(U)`, and `narrow_cast<T>(U)`
    - [Safe contiguous ranges](doc/Features.md#safe-contiguous-ranges): `span<T, Extent>`
    - [Bounds-checked element access](doc/Features.md#bounds-checked-element-access): `at()`
    - [Semantic integer types](doc/Features.md#semantic-integer-types): `index`, `dim`, `stride`, `diff`
    - [Ad-hoc RAII (C++11 and higher)](doc/Features.md#ad-hoc-raii-c11-and-higher): `finally()`, `on_return()`, and `on_error()`
    - [Feature checking macros](doc/Features.md#feature-checking-macros)
    - [Polyfills](doc/Features.md#polyfills)
    - [Configuration options and switches](doc/Features.md#configuration-options-and-switches)
    - [Configuration changes, deprecated and removed features](doc/Features.md#configuration-changes-deprecated-and-removed-features)-->
- [Dependencies](#dependencies)
- [Version semantics](#version-semantics)
- [License](#license)


## Why *gsl-lite*?

Although the [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines) only contain a (loose) specification
of the Guidelines Support Library and acknowledge there could be different implementations, [Microsoft GSL](https://github.com/microsoft/gsl)
is the *de facto* default implementation of the Guidelines Support Library. Therefore, if you are looking for an implementation of the
C++ Core Guidelines support library, you may want to use Microsoft GSL.

That said, *gsl-lite* differs from Microsoft GSL in the following ways:

- *gsl-lite* supports C++98, C++03, C++11, and older compilers.
- *gsl-lite* supports CUDA, and many of its features can be used in CUDA kernel code.
- [Contract and assertion checks](doc/Features.md#contract-and-assertion-checks) are more fine-grained, and runtime enforcement is
  [configurable](doc/Features.md#contract-checking-configuration-macros).
- [`not_null<>`](doc/Features.md#not_null) disallows implicit conversion construction (like `strict_not_null<>` in Microsoft GSL).
- In *gsl-lite*, `not_null<P>` retains the copyability and movability of `P` and therefore has a [*moved-from state*](doc/Features.md#nullability-and-the-moved-from-state).
  In Microsoft GSL, `not_null<P>` is not movable if `P` is movable but not copyable.
- *gsl-lite* defines some [feature testing macros](doc/Features.md#feature-checking-macros) and [polyfills](doc/Features.md#polyfills) useful for targeting multiple versions of C++.
- *gsl-lite* comes as a single-header library.


## Features

See also Section&nbsp;[GSL: Guidelines support library](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#S-gsl) of the C++ Core Guidelines.

Feature \\ library | GSL spec | MS GSL | *gsl&#8209;lite* | Notes |
------------------------------------------------------------------------|:-----------:|:-------------:|:-------------------:|:------|
[**Views:**](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#gslview-views) | &nbsp;  | &nbsp;  | &nbsp;       | &nbsp;|
[`owner<>`](doc/Features.md#owner-c11-and-higher)                       | ✓          | ✓             | ✓¹                 | Annotate a raw pointer that carries ownership |
[`not_null<>`](doc/Features.md#not_null)                                | ✓          | ✓             | ✓                  | Annotate a (smart) pointer that must not be `nullptr`<br>Enforces non-nullability at runtime<br>(cf. `strict_not_null<>` in Microsoft GSL) |
[`not_null_ic<>`](doc/Features.md#not_null_ic)                          | -           | ✓             | ✓                  | Like `not_null<>` but allows implicit construction from nullable pointers<br>(cf. `not_null<>` in Microsoft GSL) |
[`make_unique<>()`](doc/Features.md#not_null)                           | -           | -             | ✓                  | Like [`std::make_unique<T>()`](https://en.cppreference.com/w/cpp/memory/unique_ptr/make_unique) but returns `not_null<std::unique_ptr<T>>` |
[`make_shared<>()`](doc/Features.md#not_null)                           | -           | -             | ✓                  | Like [`std::make_shared<T>()`](https://en.cppreference.com/w/cpp/memory/shared_ptr/make_shared) but returns `not_null<std::shared_ptr<T>>` |
[`span<>`](doc/Features.md#safe-contiguous-ranges)                      | ✓          | ✓             | ✓                  | Like [`std::span<>`](https://en.cppreference.com/w/cpp/container/span) but with bounds-checking |
[`zstring`<br>`czstring`](doc/Features.md#semantic-string-type-aliases)   | ✓        | ✓             | ✓                  | Aliases for `char *` and `char const *` to be used for 0-terminated strings (C-style strings) |
[`wzstring`<br>`wczstring`](doc/Features.md#semantic-string-type-aliases) | -        | ✓             | ✓                  | Aliases for `wchar_t *` and `wchar_t const *` to be used for 0-terminated strings (C-style strings) |
[**Assertions:**](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#gslassert-assertions) | &nbsp; | &nbsp; | &nbsp; | &nbsp;|
[`Expects()`](doc/Features.md#contract-and-assertion-checks)            | ✓          | ✓             | (✓)                 | Checks precondition at runtime<br>(only in [GSL compatibility mode](doc/Features.md#gsl_feature_gsl_compatibility_mode0)) |
[`Ensures()`](doc/Features.md#contract-and-assertion-checks)            | ✓          | ✓             | (✓)                 | Checks precondition at runtime<br>(only in [GSL compatibility mode](doc/Features.md#gsl_feature_gsl_compatibility_mode0)) |
[`gsl_Expects()`](doc/Features.md#contract-and-assertion-checks)        | -           | -             | ✓                   | Checks precondition at runtime |
[`gsl_ExpectsDebug()`](doc/Features.md#contract-and-assertion-checks)   | -           | -             | ✓                   | Checks precondition at runtime unless [`NDEBUG`](https://en.cppreference.com/w/cpp/error/assert) is defined |
[`gsl_ExpectsAudit()`](doc/Features.md#contract-and-assertion-checks)   | -           | -             | ✓                   | Checks precondition at runtime if [audit mode](doc/Features.md#runtime-enforcement) is enabled |
[`gsl_Ensures()`](doc/Features.md#contract-and-assertion-checks)        | -           | -             | ✓                   | Checks postcondition at runtime |
[`gsl_EnsuresDebug()`](doc/Features.md#contract-and-assertion-checks)   | -           | -             | ✓                   | Checks postcondition at runtime unless [`NDEBUG`](https://en.cppreference.com/w/cpp/error/assert) is defined |
[`gsl_EnsuresAudit()`](doc/Features.md#contract-and-assertion-checks)   | -           | -             | ✓                   | Checks postcondition at runtime if [audit mode](doc/Features.md#runtime-enforcement) is enabled |
[`gsl_Assert()`](doc/Features.md#contract-and-assertion-checks)         | -           | -             | ✓                   | Checks invariant at runtime |
[`gsl_AssertDebug()`](doc/Features.md#contract-and-assertion-checks)    | -           | -             | ✓                   | Checks invariant at runtime unless [`NDEBUG`](https://en.cppreference.com/w/cpp/error/assert) is defined |
[`gsl_AssertAudit()`](doc/Features.md#contract-and-assertion-checks)    | -           | -             | ✓                   | Checks invariant at runtime if [audit mode](doc/Features.md#runtime-enforcement) is enabled |
[**Utilities:**](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#gslutil-utilities) | &nbsp;  | &nbsp;        | &nbsp;            | &nbsp;|
[`finally()`](doc/Features.md#ad-hoc-raii-c11-and-higher)               | ✓          | ✓             | ✓¹                 | Returns an object that executes a given action in its destructor; use for ad-hoc [RAII](https://en.cppreference.com/w/cpp/language/raii) |
[`on_return()`](doc/Features.md#ad-hoc-raii-c11-and-higher)             | -           | -             | (✓¹)                | Creates an object that executes a given action in its destructor if no exception occurred<br>([opt-in](doc/Features.md#gsl_feature_experimental_return_guard0) feature) |
[`on_error()`](doc/Features.md#ad-hoc-raii-c11-and-higher)              | -           | -             | (✓¹)                | Creates an object that executes a given action in its destructor if an exception was thrown<br>([opt-in](doc/Features.md#gsl_feature_experimental_return_guard0) feature) |
[`at()`](doc/Features.md#bounds-checked-element-access)                 | ✓          | ✓             | ✓                 | Bounds-checked element access for C-style arrays and containers with random access |
[`index`](doc/Features.md#emantic-integer-type-aliases)                 | ✓          | ✓             | ✓                 | Signed integer type for indexes and subscripts |
[`dim`](doc/Features.md#emantic-integer-type-aliases)                   | -           | -             | ✓                 | Signed integer type for sizes |
[`stride`](doc/Features.md#emantic-integer-type-aliases)                | -           | -             | ✓                 | Signed integer type for index strides |
[`diff`](doc/Features.md#emantic-integer-type-aliases)                  | -           | -             | ✓                 | Signed integer type for index differences |
[`narrow_cast<>()`](doc/Features.md#narrow_castt-u)                     | ✓          | ✓             | ✓                 | A narrowing cast which tolerates lossy conversions;<br> equivalent to `static_cast<>()` |
[`narrow<>()`](doc/Features.md#narrowt-u)                               | ✓          | ✓             | ✓                 | A checked narrowing cast; throws `narrowing_error` if cast is lossy |
[`narrow_failfast<>()`](doc/Features.md#narrow_failfastt-u)             | -           | -             | ✓                 | A checked narrowing cast; fails runtime contract check if cast is lossy |
[**Feature checking macros:**](doc/Features.md#feature-checking-macros) | &nbsp;      | &nbsp;        | &nbsp;             | &nbsp; |
`gsl_CPPxx_OR_GREATER`                                                  | -           | -             | ✓                 | Whether C++xx language features are available<br>(substitute `11`, `14`, `17`, `20`, `23`, `26`) |
`gsl_COMPILER_xx_VERSION`                                               | -           | -             | ✓                 | Evaluates to version number when compiled with `xx`, or 0 otherwise<br>(substitute `GNUC`, `CLANG`, `MSVC`, `APPLECLANG`, `NVCC`, `ARMCC`) |
[**Polyfills**](doc/Features.md#polyfills)                              | &nbsp;      | &nbsp;        | &nbsp;             | &nbsp; |

¹: C++11 or newer required


## Dependencies

*gsl-lite* has no dependencies other than the [C++ standard library](http://en.cppreference.com/w/cpp/header).


## Version semantics

*gsl-lite* strives to follow [Semantic Versioning](https://semver.org/) guidelines. We strive to maintain
[API](https://en.wikipedia.org/wiki/Application_programming_interface) and [ABI](https://en.wikipedia.org/wiki/Application_binary_interface) compatibility
and avoid breaking changes in minor and patch releases.

Development of *gsl-lite* happens in the `master` branch. Versioning semantics apply only to tagged releases: there is no stability guarantee between individual
commits in the `master` branch, that is, anything added since the last tagged release may be renamed, removed, or have the semantics changed without further notice.

A minor-version release will be compatible (in both ABI and API) with the previous minor-version release. Thus, once a change is released, it becomes part of the API.

Some of the [configuration options](doc/Features.md#configuration-options-and-switches) may affect the API and ABI of *gsl-lite*.


## License

*gsl-lite* uses the [MIT](LICENSE) license.
