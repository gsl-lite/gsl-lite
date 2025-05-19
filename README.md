# *gsl-lite*


| metadata | build  | packages | try online |
| -------- | ------ | -------- | ---------- |
| [![Language](https://badgen.net/badge/C++/98,11+/blue)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization) <br> [![License](https://badgen.net/github/license/gsl-lite/gsl-lite)](https://opensource.org/licenses/MIT) <br> [![Version](https://badgen.net/github/release/gsl-lite/gsl-lite)](https://github.com/gsl-lite/gsl-lite/releases)   |   [![Azure Pipelines build status](https://dev.azure.com/gsl-lite/gsl-lite/_apis/build/status/gsl-lite.gsl-lite?branchName=master)](https://dev.azure.com/gsl-lite/gsl-lite/_build/latest?definitionId=1&branchName=master) <br> [![AppVeyor build status](https://ci.appveyor.com/api/projects/status/1v6eqy68m8g7tm06?svg=true)](https://ci.appveyor.com/project/gsl-lite/gsl-lite)   |   [![Vcpkg](https://badgen.net/badge/latest/on%20Vcpkg/blue)](https://vcpkg.io/en/package/gsl-lite) <br> [![single header](https://badgen.net/badge/latest/single%20header/blue)](https://raw.githubusercontent.com/gsl-lite/gsl-lite/master/include/gsl-lite/gsl-lite.hpp)   |   [![Try it on Compiler Explorer](https://badgen.net/badge/on/Compiler%20Explorer/blue)](https://gcc.godbolt.org/z/6frqr8PsK) <br> [![Try it on Wandbox](https://badgen.net/badge/on/Wandbox/blue)](https://wandbox.org/permlink/OIU7oseCTLsiNQ8A)   |


***gsl-lite*** is a portable, single-file, header-only library for defensive programming based on the [C++ Core Guidelines Support Library](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-gsl) specification.


## Contents

- [Example usage](#example-usage)
- [In a nutshell](#in-a-nutshell)
- [License](#license)
- [Dependencies](#dependencies)
- [Installation and use](#installation-and-use)
- [Why *gsl-lite*?](#why-gsl-lite)
- [Features](#features)
- [Reference documentation](doc/Reference.md)
- [Migration guide](#migration-guide)
- [Using *gsl-lite* in libraries](#using-gsl-lite-in-libraries)
- [Reported to work with](#reported-to-work-with)
- [Version semantics](#version-semantics)
- [Contributing](#contributing)


## Example usage

```c++
#include <memory>
#include <utility>
#include <numeric>

#include <gsl-lite/gsl-lite.hpp>


namespace my_lib {

        // Define this in your own namespace.
    namespace gsl = ::gsl_lite;

        // `span<T[, Extent]>`: contiguous range with bounds checks
    double mean( gsl::span<double const> values )
    {
            // `gsl_Expects( cond )`: precondition check
        gsl_Expects( !values.empty() );
    
        double sum = std::accumulate( values.begin(), values.end(), 0. );
        return sum / std::ssize( values );
    }

    class Resource
    {
        ...
    public:
        Resource( std::size_t size );
    };

        // Type-encoded precondition with `not_null<P>`
    void consumeResource( gsl::not_null<std::unique_ptr<Resource>> resource );

        // Type-encoded postcondition with `not_null<P>`
    gsl::not_null<std::unique_ptr<Resource>> acquireResource( int size )
    {
            // A flavor of `make_unique<T>()` which returns `not_null<std::unique_ptr<T>>`
        return gsl::make_unique<Resource>(
            gsl::narrow_failfast<std::size_t>( size ));  // A checked numeric cast.
    }

} // namespace my_lib
```


## In a nutshell

*gsl-lite* strives to implement the [Guidelines Support Library specification](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-gsl) of the C++ Core Guidelines
maintained by the [Standard C++ Foundation](https://isocpp.org/).
The library is originally based on [Microsoft GSL](https://github.com/microsoft/gsl) and was adapted for C++98, C++03. It also works when compiled as C++11, C++14, C++17, C++20, or C++23.

*gsl-lite* does not interfere with Microsoft GSL since both libraries live in different namespaces (`gsl_lite` vs. `gsl`).

*gsl-lite* recognizes when it is compiled for the CUDA platform and decorates some functions with `__host__` and `__device__` accordingly.


## License

*gsl-lite* uses the [MIT](LICENSE) license.


## Dependencies

*gsl-lite* has no dependencies other than the [C++ standard library](http://en.cppreference.com/w/cpp/header).


## Installation and use

The recommended way to consume *gsl-lite* in your CMake project is to use `find_package()` to locate the package `gsl-lite`
and `target_link_libraries()` to link to the imported target `gsl-lite::gsl-lite`:

```cmake
cmake_minimum_required( VERSION 3.20 FATAL_ERROR )

project( my-program LANGUAGES CXX )

find_package( gsl-lite 1.0 REQUIRED )

add_executable( my-program main.cpp )
target_compile_features( my-program PRIVATE cxx_std_17 )
target_link_libraries( my-program PRIVATE gsl-lite::gsl-lite )
```

*gsl-lite* is available via [Vcpkg](https://vcpkg.io/en/package/gsl-lite), [Conan](https://conan.io/center/recipes/gsl-lite),
and possibly other package managers. It may also be obtained with [CPM](https://github.com/cpm-cmake/CPM.cmake):
```cmake
CPMAddPackage( NAME gsl-lite VERSION 1.0.0 GITHUB_REPOSITORY gsl-lite/gsl-lite )
```
See the directories [example/with-CPM](https://github.com/gsl-lite/gsl-lite/tree/master/example/with-CPM) and
[example/with-Vcpkg](https://github.com/gsl-lite/gsl-lite/tree/master/example/with-Vcpkg) for example projects
that use CPM and Vcpkg, respectively, to obtain *gsl-lite*.

Once the build system is set up, include the `<gsl-lite/gsl-lite.hpp>` header file to use *gsl-lite*:

```c++
// main.cpp

#include <iostream>

#include <gsl-lite/gsl-lite.hpp>

void printCmdArgs( gsl_lite::span<gsl_lite::zstring const> cmdArgs )
{
    gsl_Expects( !cmdArgs.empty() );

    auto argsWithoutExeName = cmdArgs.subspan( 1 );
    for ( auto arg : argsWithoutExeName )
    {
        std::cout << arg << "\n";
    }
}

int main( int argc, char* argv[] )
{
    auto numArgs = gsl_lite::narrow_failfast<std::size_t>( argc );
    auto cmdArgs = gsl_lite::span( argv, numArgs );
    printCmdArgs( cmdArgs );
}
```


## Why *gsl-lite*?

*gsl-lite* is different from [Microsoft GSL](https://github.com/microsoft/gsl), the default implementation of the
[C++ Core Guidelines support library (GSL)](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines), in the following ways:

- *gsl-lite* maintains support for older versions of C++ (C++98, C++03, C++11) and older compilers.  
  (see: [Reported to work with](#reported-to-work-with))
- *gsl-lite* supports [CUDA](https://developer.nvidia.com/cuda-toolkit), and many of its features can be used in CUDA kernel code.
- [Contract and assertion checks](doc/Reference.md#contract-and-assertion-checks) are more fine-grained, and runtime enforcement is
  [configurable](doc/Reference.md#contract-checking-configuration-macros).
- In *gsl-lite*, `not_null<P>` retains the copyability and movability of `P` and therefore may have a [*moved-from state*](doc/Reference.md#nullability-and-the-moved-from-state),
  which Microsoft GSL [expressly disallows](https://github.com/microsoft/GSL/issues/1022#issuecomment-1022713632).
  As a consequence, `not_null<std::unique_ptr<T>>` is movable in *gsl-lite* but not in Microsoft GSL.
- *gsl-lite* defines [feature testing macros](doc/Reference.md#feature-checking-macros) and [polyfills](doc/Reference.md#polyfills) useful for targeting multiple versions of C++.
- *gsl-lite* comes as a single-header library.


## Features

See the [reference documentation](doc/Reference.md) for a detailed explanation of the features provided by *gsl-lite*, and
Section&nbsp;[GSL: Guidelines support library](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-gsl) of the C++ Core Guidelines
for the specification of the Guidelines support library.


Feature \\ library | GSL spec | MS GSL | *gsl&#8209;lite* | Notes |
------------------------------------------------------------------------|:-----------:|:-------------:|:-------------------:|:-------|
[**Views:**](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#gslview-views) | &nbsp;  | &nbsp;  | &nbsp;       | &nbsp; |
[`owner<>`](doc/Reference.md#ownerp-c11-and-higher)                      | ✓          | ✓             | ✓¹¹                | Annotate a raw pointer that carries ownership |
[`not_null<>`](doc/Reference.md#not_nullp)                               | ✓          | ✓             | ✓                  | Annotate a (smart) pointer that must not be `nullptr`; enforces non-nullability at runtime<br>(cf. `strict_not_null<>` in Microsoft GSL) |
[`not_null_ic<>`](doc/Reference.md#not_null_icp)                         | -           | ✓             | ✓                  | Like `not_null<>` but allows implicit construction from nullable pointers<br>(cf. `not_null<>` in Microsoft GSL) |
[`make_unique<>()`](doc/Reference.md#not_nullp)                          | -           | -             | ✓¹¹                | Like [`std::make_unique<T>()`](https://en.cppreference.com/w/cpp/memory/unique_ptr/make_unique) but returns `not_null<std::unique_ptr<T>>` |
[`make_shared<>()`](doc/Reference.md#not_nullp)                          | -           | -             | ✓¹¹                | Like [`std::make_shared<T>()`](https://en.cppreference.com/w/cpp/memory/shared_ptr/make_shared) but returns `not_null<std::shared_ptr<T>>` |
[`span<>`](doc/Reference.md#safe-contiguous-ranges)                      | ✓          | ✓             | ✓                  | Like [`std::span<>`](https://en.cppreference.com/w/cpp/container/span) but with bounds-checking |
[`zstring`<br>`czstring`](doc/Reference.md#string-type-aliases)          | ✓          | ✓             | ✓                  | Aliases for `char *` and `char const *` to be used for 0-terminated strings (C-style strings) |
[`wzstring`<br>`wczstring`](doc/Reference.md#string-type-aliases)        | -           | ✓             | ✓                  | Aliases for `wchar_t *` and `wchar_t const *` to be used for 0-terminated strings (C-style strings) |
[**Assertions:**](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#gslassert-assertions) | &nbsp; | &nbsp; | &nbsp; | &nbsp;|
[`Expects()`](doc/Reference.md#contract-and-assertion-checks)            | ✓          | ✓             | (✓)                 | Checks precondition at runtime<br>(only defined in [GSL compatibility mode](doc/Reference.md#gsl_feature_gsl_compatibility_mode0)) |
[`Ensures()`](doc/Reference.md#contract-and-assertion-checks)            | ✓          | ✓             | (✓)                 | Checks precondition at runtime<br>(only defined in [GSL compatibility mode](doc/Reference.md#gsl_feature_gsl_compatibility_mode0)) |
[`gsl_Expects()`](doc/Reference.md#contract-and-assertion-checks)        | -           | -             | ✓                   | Checks precondition at runtime |
[`gsl_ExpectsDebug()`](doc/Reference.md#contract-and-assertion-checks)   | -           | -             | ✓                   | Checks precondition at runtime<br>unless [`NDEBUG`](https://en.cppreference.com/w/cpp/error/assert) is defined |
[`gsl_ExpectsAudit()`](doc/Reference.md#contract-and-assertion-checks)   | -           | -             | ✓                   | Checks precondition at runtime<br>if [audit mode](doc/Reference.md#runtime-enforcement) is enabled |
[`gsl_Ensures()`](doc/Reference.md#contract-and-assertion-checks)        | -           | -             | ✓                   | Checks postcondition at runtime |
[`gsl_EnsuresDebug()`](doc/Reference.md#contract-and-assertion-checks)   | -           | -             | ✓                   | Checks postcondition at runtime<br>unless [`NDEBUG`](https://en.cppreference.com/w/cpp/error/assert) is defined |
[`gsl_EnsuresAudit()`](doc/Reference.md#contract-and-assertion-checks)   | -           | -             | ✓                   | Checks postcondition at runtime<br>if [audit mode](doc/Reference.md#runtime-enforcement) is enabled |
[`gsl_Assert()`](doc/Reference.md#contract-and-assertion-checks)         | -           | -             | ✓                   | Checks invariant at runtime |
[`gsl_AssertDebug()`](doc/Reference.md#contract-and-assertion-checks)    | -           | -             | ✓                   | Checks invariant at runtime<br>unless [`NDEBUG`](https://en.cppreference.com/w/cpp/error/assert) is defined |
[`gsl_AssertAudit()`](doc/Reference.md#contract-and-assertion-checks)    | -           | -             | ✓                   | Checks invariant at runtime<br>if [audit mode](doc/Reference.md#runtime-enforcement) is enabled |
[**Utilities:**](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#gslutil-utilities) | &nbsp;  | &nbsp; | &nbsp; | &nbsp; |
[`finally()`](doc/Reference.md#ad-hoc-resource-management-c11-and-higher) | ✓         | ✓             | ✓¹¹                | Returns an object that executes a given action in its destructor; use for ad hoc resource cleanup |
[`on_return()`](doc/Reference.md#ad-hoc-resource-management-c11-and-higher) | -        | -             | (✓¹¹)               | Creates an object that executes a given action in its destructor if no exception occurred<br>([opt-in](doc/Reference.md#gsl_feature_experimental_return_guard0) feature) |
[`on_error()`](doc/Reference.md#ad-hoc-resource-management-c11-and-higher) | -         | -             | (✓¹¹)               | Creates an object that executes a given action in its destructor if an exception was thrown<br>([opt-in](doc/Reference.md#gsl_feature_experimental_return_guard0) feature) |
[`at()`](doc/Reference.md#bounds-checked-element-access)                 | ✓          | ✓             | ✓                 | Bounds-checked element access for C-style arrays and containers with random access |
[`index`](doc/Reference.md#integer-type-aliases)                         | ✓          | ✓             | ✓                 | Signed integer type for indexes and subscripts |
[`dim`](doc/Reference.md#integer-type-aliases)                           | -           | -             | ✓                 | Signed integer type for sizes |
[`stride`](doc/Reference.md#integer-type-aliases)                        | -           | -             | ✓                 | Signed integer type for index strides |
[`diff`](doc/Reference.md#integer-type-aliases)                          | -           | -             | ✓                 | Signed integer type for index differences |
[`narrow_cast<>()`](doc/Reference.md#narrow_castt-u-)                    | ✓          | ✓             | ✓                 | Narrowing cast which tolerates lossy conversions; equivalent to `static_cast<>()` |
[`narrow<>()`](doc/Reference.md#narrowt-u-)                              | ✓          | ✓             | ✓                 | Checked narrowing cast; throws `narrowing_error` if cast is lossy |
[`narrow_failfast<>()`](doc/Reference.md#narrow_failfastt-u-)            | -           | -             | ✓                 | Checked narrowing cast; fails assertion check if cast is lossy |

¹¹: C++11 or newer required


## Migration guide

Starting with v1.0, *gsl-lite* lives in the single header file `<gsl-lite/gsl-lite.hpp>`, and all its symbols reside in namespace
`gsl_lite`. By default, *gsl-lite* no longer defines a namespace `gsl` or the unprefixed `Expects()` and `Ensures()` macros for
precondition and postcondition checking.

This change enables coexistence with [Microsoft GSL](https://github.com/microsoft/GSL) ([#194](https://github.com/gsl-lite/gsl-lite/issues/194)).

### Adapting your code

If you are migrating from *gsl-lite* v0.\*, adapt your code by referencing namespace `gsl_lite` rather than namespace `gsl`, and by
using the prefixed macros `gsl_Expects()` and `gsl_Ensures()` rather than the unprefixed macros `Expects()` and `Ensures()` for
precondition and postcondition checking.

Note that *gsl-lite* v1 also changed the defaults for many of *gsl-lite*'s configuration options. See the
[v1.0 release notes](https://github.com/gsl-lite/gsl-lite/releases/tag/v1.0.0) for a comprehensive list of changes.
This should not affect you if you had already opted in to version-1 defaults by setting `gsl_CONFIG_DEFAULTS_VERSION=1`
or by linking to the `gsl::gsl-lite-v1` target in CMake.

To reduce the pervasiveness of required changes, it can be useful to define a namespace alias inside your own namespace:

```c++
// my-lib.hpp

#include <gsl-lite/gsl-lite.hpp>  // instead of <gsl/gsl-lite.hpp>

namespace my_lib {

    namespace gsl = ::gsl_lite;  // convenience alias

    inline double median( gsl::span<double const> elements )
    {
        gsl_Expects( !elements.empty() );  // instead of Expects()
        ...
    }

} // namespace my_lib
```

### Using the GSL compatibility mode

To minimize the impact of the breaking changes, *gsl-lite* introduces an optional *GSL compatibility mode* controlled by the
new configuration switch [`gsl_FEATURE_GSL_COMPATIBILITY_MODE`](doc/Reference.md#feature-selection-macros), which is is disabled
by default and can be enabled by defining `gsl_FEATURE_GSL_COMPATIBILITY_MODE=1`.

If the GSL compatibility mode is enabled, *gsl-lite* additionally makes the following global definitions:
```c++
namespace gsl = ::gsl_lite;
#define Expects( x )  gsl_Expects( x )
#define Ensures( x )  gsl_Ensures( x )
```

The GSL compatibility mode precludes the use of *gsl-lite* and Microsoft GSL in the same translation unit. Therefore, **do not use
the GSL compatibility mode when using *gsl-lite* in a public header file of a library.** (See notes on
[using *gsl-lite* in libraries](#using-gsl-lite-in-libraries) below.)

The GSL compatibility mode causes no link-time interference between *gsl-lite* and as Microsoft GSL. Both libraries may be used in
the same project as long as no translation unit includes both at the same time.

The legacy header file `<gsl/gsl-lite.hpp>` now forwards to `<gsl-lite/gsl-lite.hpp>` and implicitly enables the GSL compatibility mode.
When the legacy header is included, it emits a warning message that urges to either migrate to header `<gsl-lite/gsl-lite.hpp>`,
namespace `gsl_lite` and the prefixed contract checking macros `gsl_Expects()` and `gsl_Ensures()`, or to explicitly request GSL
compatibility by defining `gsl_FEATURE_GSL_COMPATIBILITY_MODE=1`.


## Using *gsl-lite* in libraries

Many features of *gsl-lite* are very useful for defining library interfaces, for instance spans, contract checks, or `not_null<>`.

*gsl-lite* can coexist with Microsoft GSL. However, the GSL compatibility mode of *gsl-lite* may cause interference with Microsoft GSL.
Also, *gsl-lite* is customizable through a large number of configuration options and switches. These configuration macros may affect the API and
ABI of *gsl-lite* in ways that renders it incompatible with other code. How *gsl-lite* is configured should be the prerogative of the consumer,
not the author, of a library.

Therefore, when using *gsl-lite* in a library, please mind the following suggestions:

- Do not define, or rely on, any of *gsl-lite*'s configuration options or switches when using *gsl-lite* in a library.
- In particular, do not enable the GSL compatibility mode.
- Do not use the legacy header file `<gsl/gsl-lite.hpp>`, which implicitly enables the GSL compatibility mode; use the header `<gsl-lite/gsl-lite.hpp>` instead.
- Use namespace `gsl_lite` rather than namespace `gsl`; if desired, define a `namespace gsl = ::gsl_lite;` alias in your own namespace.
- Use the prefixed contract checking macros `gsl_Expects()` and `gsl_Ensures()` rather than the unprefixed macros `Expects()` and `Ensures()`.


## Reported to work with

The table below mentions the compiler versions and platforms *gsl-lite* is reported to work with.

Compiler             | OS              | Platforms | Versions          | CI |
--------------------:|:----------------|-----------|------------------:|----|
GCC                  | Linux           | x64       | 4.7 and newer     | [9, 10, 11, 12, 13, 14](https://dev.azure.com/gsl-lite/gsl-lite/_build?definitionId=1) |
GCC (MinGW)          | Windows         | x86, x64  | 4.8.4 and newer   |    |
GCC (DJGPP)          | DOSBox, FreeDOS | x86       | 7.2               |    |
GCC                  | MacOS           | x64       | 6 and newer       | [11, 12, 13, 14](https://dev.azure.com/gsl-lite/gsl-lite/_build?definitionId=1) |
Clang                | Linux           | x64       | 3.5 and newer     | [11, 12, 13, 14, 15, 16, 17, 18, 19](https://dev.azure.com/gsl-lite/gsl-lite/_build?definitionId=1) |
Clang with libstdc++ | Linux           | x64       | 11 and newer      | [19](https://dev.azure.com/gsl-lite/gsl-lite/_build?definitionId=1) |
Clang                | Windows         | x64       | version shipped with VS | VS [2019, 2022](https://dev.azure.com/gsl-lite/gsl-lite/_build?definitionId=1) |
MSVC (Visual Studio) | Windows         | x86, x64  | VS 2010 and newer | VS [2010, 2012, 2013, 2015, 2017](https://ci.appveyor.com/project/gsl-lite/gsl-lite), [2019, 2022](https://dev.azure.com/gsl-lite/gsl-lite/_build?definitionId=1) |
AppleClang (Xcode)   | MacOS           | x64       | 7.3 and newer     | [14, 15, 16](https://dev.azure.com/gsl-lite/gsl-lite/_build?definitionId=1) |
NVCC (CUDA Toolkit)  | Linux, Windows  | x64       | 10.2 and newer    | [12.8](https://dev.azure.com/gsl-lite/gsl-lite/_build?definitionId=1) |
ARMCC                |                 | ARM       | 5 and newer       | |


## Version semantics

*gsl-lite* follows [Semantic Versioning](https://semver.org/) guidelines. We maintain [API](https://en.wikipedia.org/wiki/Application_programming_interface) and
[ABI](https://en.wikipedia.org/wiki/Application_binary_interface) compatibility and avoid breaking changes in minor and patch releases.

Development of *gsl-lite* happens in the `master` branch. Versioning semantics apply only to tagged releases: there is no stability guarantee between individual
commits in the `master` branch, that is, anything added since the last tagged release may be renamed, removed, or have the semantics changed without further notice.

A minor-version release will be compatible (in both ABI and API) with the previous minor-version release. Thus, once a change is released, it becomes part of the API.

Some of the [configuration options](doc/Reference.md#configuration-options-and-switches) may affect the API and ABI of *gsl-lite*.


## Contributing

Contributions to *gsl-lite* through [pull requests](https://github.com/gsl-lite/gsl-lite/pulls) or [issues](https://github.com/gsl-lite/gsl-lite/issues) are welcome.

*gsl-lite* comes with a test suite that uses an included, slightly modified copy of the [*lest* test framework](https://github.com/martinmoene/lest).
To build *gsl-lite* with the test suite, enable the CMake build option `GSL_LITE_OPT_BUILD_TESTS` when configuring the project.
