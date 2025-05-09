# Installation and use

**Contents:**  
- [Getting started](#getting-started)
- [Migration guide](#migration-guide)
    - [Adapting your code](#adapting-your-code)
    - [Using GSL compatibility mode](#using-gsl-compatibility-mode)
- [Reported to work with](#reported-to-work-with)
- [Using *gsl-lite* in libraries](#using-gsl-lite-in-libraries)
- [Contributing](#contributing)


## Getting started

The recommended way to consume *gsl-lite* in your CMake project is to use `find_package()` and `target_link_libraries()`:

```CMake
cmake_minimum_required( VERSION 3.20 FATAL_ERROR )

find_package( gsl-lite 1.0 REQUIRED )

project( my-program LANGUAGES CXX )

add_executable( my-program main.cpp )
target_link_libraries( my-program PRIVATE gsl-lite::gsl-lite )
```

*gsl-lite* is available via [Vcpkg](https://vcpkg.io/en/package/gsl-lite), [Conan](https://conan.io/center/recipes/gsl-lite),
and possibly other package managers. It may also be obtained with [CPM](https://github.com/cpm-cmake/CPM.cmake):
```cmake
CPMAddPackage( NAME gsl-lite VERSION 1.0.0 GITHUB_REPOSITORY gsl-lite/gsl-lite )
```
See the directories [example/with-CPM](example/with-CPM) and [example/with-Vcpkg](example/with-Vcpkg) for example projects
that use CPM and Vcpkg, respectively, to obtain *gsl-lite*.

Once the build system is set up, include the `<gsl-lite/gsl-lite.hpp>` header file to use *gsl-lite*:

```c++
// main.cpp

#include <iostream>

#include <gsl-lite/gsl-lite.hpp>

void printArgs( gsl_lite::span<gsl_lite::zstring const> args )
{
    for ( auto arg : args )
    {
        std::cout << arg << "\n";
    }
}

int main( int argc, char* argv[] )
{
    gsl_Expects( argc > 0 );

    auto args = gsl_lite::span( argv, argc );
    auto argsWithoutExeName = args.subspan( 1 );
    printArgs( argsWithoutExeName );
}
```


## Migration guide

Starting with v1.0, *gsl-lite* now lives in the single header file `<gsl-lite/gsl-lite.hpp>`, and all its symbols reside in namespace
`gsl_lite`. By default, *gsl-lite* no longer defines a namespace `gsl` or the unprefixed `Expects()` and `Ensures()` macros for
precondition and postcondition checking.

This change enables coexistence with [Microsoft GSL](https://github.com/microsoft/GSL) or other GSL implementations. (#194)

### Adapting your code

If you are migrating from *gsl-lite* v0.\*, adapt your code by referencing namespace `gsl_lite` rather than namespace `gsl`, and by
using the prefixed macros `gsl_Expects()` and `gsl_Ensures()` rather than the unprefixed macros `Expects()` and `Ensures()` for
precondition and postcondition checking.

To avoid any code changes, you may enable [GSL compatibility mode](#using-gsl-compatibility-mode) instead.

To minimize the pervasiveness code changes, it can be useful to define a namespace alias *inside your own namespace*:

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

### Using GSL compatibility mode

To minimize the impact of the breaking changes, *gsl-lite* introduces an optional *GSL compatibility mode* controlled by the
new configuration switch [`gsl_FEATURE_GSL_COMPATIBILITY_MODE`](doc/Features.md#feature-selection-macros), which is is disabled
by default and can be enabled by defining `gsl_FEATURE_GSL_COMPATIBILITY_MODE=1`.

If the GSL compatibility mode is enabled, *gsl-lite* additionally makes the following global definitions:
```c++
namespace gsl = ::gsl_lite;
#define Expects( x )  gsl_Expects( x )
#define Ensures( x )  gsl_Ensures( x )
```

The GSL compatibility mode precludes the use of *gsl-lite* and Microsoft GSL in the same translation unit. Therefore, when making
use of *gsl-lite* in a public header file of a library, the GSL compatibility mode should not be enabled.

The GSL compatibility mode causes no link-time interference between *gsl-lite* and as Microsoft GSL. Both libraries may be used in
the same project as long as no translation unit includes both at the same time.

The legacy header file `<gsl/gsl-lite.hpp>` now forwards to `<gsl-lite/gsl-lite.hpp>` and implicitly enables the GSL compatibility mode.
When the legacy header is included, it emits a warning message which urges to either migrate to header `<gsl-lite/gsl-lite.hpp>`,
namespace `gsl_lite` and the prefixed contract checking macros `gsl_Expects()` and `gsl_Ensures()`, or to explicitly request GSL
compatibility by defining `gsl_FEATURE_GSL_COMPATIBILITY_MODE=1`.


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


## Using *gsl-lite* in libraries

Many features of *gsl-lite* are very useful for defining library interfaces, for instance spans, contract checks, or `not_null<>`.

*gsl-lite* can coexist with Microsoft GSL. However, the GSL compatibility mode of *gsl-lite* may cause interference with Microsoft GSL. Therefore,
make sure not to use the legacy header file `<gsl/gsl-lite.hpp>`, which implicitly enables the GSL compatibility mode. Be sure to migrate your code
to use namespace `gsl_lite` rather than namespace `gsl` (or define a `namespace gsl = ::gsl_lite;` alias in your own namespace)
and the prefixed contract checking macros `gsl__Expects()` and `gsl__Ensures()` rather than the unprefixed macros `Expects()` and `Ensures()`.

*gsl-lite* is customizable through a large number of configuration options and switches.
The configuration macros may affect the API and ABI of *gsl-lite* in ways that renders it incompatible with other code.
Therefore, as a general rule, **do not define, or rely on, any of *gsl-lite*'s configuration options or switches when using *gsl-lite* in a library**.


## Contributing

Contributions to *gsl-lite* through [pull requests](https://github.com/gsl-lite/gsl-lite/pulls) or [issues](https://github.com/gsl-lite/gsl-lite/issues) are welcome.

*gsl-lite* comes with a test suite that uses an included, slightly modified copy of the [*lest* test framework](https://github.com/martinmoene/lest).
To build *gsl-lite* with the test suite, enable the CMake build option `GSL_LITE_OPT_BUILD_TESTS` when configuring the project.
When submitting a PR, please add tests that cover your proposed changes, and make sure that all other tests still succeed.
