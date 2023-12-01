# *gsl-lite*: Guidelines Support Library for C++98, C++11 up


| metadata | build  | packages | try online |
| -------- | ------ | -------- | ---------- |
| [![Language](https://badgen.net/badge/C++/98,11+/blue)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization) <br> [![License](https://badgen.net/github/license/gsl-lite/gsl-lite)](https://opensource.org/licenses/MIT) <br> [![Version](https://badgen.net/github/release/gsl-lite/gsl-lite)](https://github.com/gsl-lite/gsl-lite/releases)   |   [![Azure Pipelines build status](https://dev.azure.com/gsl-lite/gsl-lite/_apis/build/status/gsl-lite.gsl-lite?branchName=master)](https://dev.azure.com/gsl-lite/gsl-lite/_build/latest?definitionId=1&branchName=master) <br> [![AppVeyor build status](https://ci.appveyor.com/api/projects/status/1v6eqy68m8g7tm06?svg=true)](https://ci.appveyor.com/project/gsl-lite/gsl-lite)   |   [![Vcpkg](https://badgen.net/badge/latest/on%20Vcpkg/blue)](https://vcpkg.info/port/gsl-lite) <br> [![single header](https://badgen.net/badge/latest/single%20header/blue)](https://raw.githubusercontent.com/gsl-lite/gsl-lite/master/include/gsl/gsl-lite.hpp)   |   [![Try it on Compiler Explorer](https://badgen.net/badge/on/Compiler%20Explorer/blue)](https://gcc.godbolt.org/z/nGK1GT9qM) <br> [![Try it on Wandbox](https://badgen.net/badge/on/Wandbox/blue)](https://wandbox.org/permlink/bNcvCGNG19giZJLo)   |


*gsl-lite* is an implementation of the [C++ Core Guidelines Support Library](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-gsl) originally based on [Microsoft GSL](https://github.com/microsoft/gsl).

**Contents**  
- [Example usage](#example-usage)
- [In a nutshell](#in-a-nutshell)
- [License](#license)
- [Dependencies](#dependencies)
- [Installation and use](#installation-and-use)
- [Version semantics](#version-semantics)
- [Using *gsl-lite* in libraries](#using-gsl-lite-in-libraries)
- [Configuration options](#configuration-options)
- [Features](#features)
- [Deprecation](#deprecation)
- [Reported to work with](#reported-to-work-with)
- [Building the tests](#building-the-tests)
- [Other GSL implementations](#other-gsl-implementations)
- [Notes and references](#notes-and-references)
- [Appendix](#appendix)


Example usage
-------------

```Cpp
#include <gsl/gsl-lite.hpp>

int * use( gsl::not_null<int *> p ) 
{
    // use p knowing it's not nullptr, NULL or 0.
    return p;
}

struct Widget
{
    Widget() : owned_ptr_( new int(42) ) {}
    ~Widget() { delete owned_ptr_; }

    void work() { non_owned_ptr_ = use( owned_ptr_ ); }
    
    gsl::owner<int *> owned_ptr_;  // if alias template support
    int * non_owned_ptr_;
};

int main()
{
    Widget w;
    w.work();
}
```


In a nutshell
-------------
**gsl-lite** is a single-file header-only implementation of the [C++ Core Guidelines Support Library](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-gsl) originally based on [Microsoft GSL](https://github.com/microsoft/gsl) and adapted for C++98, C++03. It also works when compiled as C++11, C++14, C++17, C++20.

The Guidelines Support Library (GSL) contains functions and types that are suggested for use by the [C++ Core Guidelines](https://github.com/isocpp/CppCoreGuidelines) maintained by the [Standard C++ Foundation](https://isocpp.org/). The library includes types like `owner<>`, `not_null<>`, `span<>`, `string_span` and [others](#features).

*gsl-lite* recognizes when it is compiled for the CUDA platform and decorates some functions with `__host__` and `__device__`. See also section [API macro](#api-macro).


License
-------
*gsl-lite* uses the [MIT](LICENSE) license.
 

Dependencies
------------
*gsl-lite* has no other dependencies than the [C++ standard library](http://en.cppreference.com/w/cpp/header).


Installation and use
--------------------

### As CMake package

The recommended way to consume *gsl-lite* in your CMake project is to use `find_package()` and `target_link_libraries()`:

```CMake
cmake_minimum_required( VERSION 3.15 FATAL_ERROR )
    
find_package( gsl-lite 0.41 REQUIRED )
    
project( my-program LANGUAGES CXX )
    
add_executable( my-program main.cpp )
target_link_libraries( my-program PRIVATE gsl::gsl-lite-v1 )
```

There are different ways to make the `gsl-lite` package available to your project:

<details>
<summary>Using Vcpkg</summary>
<p>

1. For the [Vcpkg package manager](https://github.com/microsoft/vcpkg/), simply run Vcpkg's install command:

        vcpkg install gsl-lite

2. Now, configure your project passing the Vcpkg toolchain file as a parameter:

        cd <my-program-source-dir>
        mkdir build
        cd build
        cmake -DCMAKE_TOOLCHAIN_FILE=<vcpkg-root>/scripts/buildsystems/vcpkg.cmake ..
        cmake --build ../build

</p></details>

<details>
<summary>Using an exported build directory</summary>
<p>

1. Clone the *gsl-lite* repository and configure a build directory with CMake:

        git clone git@github.com:gsl-lite/gsl-lite.git <gsl-lite-source-dir>
        cd <gsl-lite-source-dir>
        mkdir build
        cd build
        cmake ..

2. Now, configure your project passing the CMake build directory as a parameter:

        cd <my-program-source-dir>
        mkdir build
        cd build
        cmake -Dgsl-lite_DIR:PATH=<gsl-lite-source-dir>/build ..
        cmake --build ../build

    See [example/cmake-pkg/Readme.md](example/cmake-pkg/Readme.md) for a complete example.
</p></details>


### Other options

*gsl-lite* is a header-only library; if you do not want to use the CMake package, or if you use a different build system, all
you need to do is to add the "include" subdirectory of the *gsl-lite* source directory to your include path:

        git clone git@github.com:gsl-lite/gsl-lite.git <gsl-lite-source-dir>
        g++ -std=c++03 -I<gsl-lite-source-dir>/include main.cpp

*gsl-lite* is also a single-header library; if you want to avoid external dependencies, it suffices to copy the header file
["include/gsl/gsl-lite.hpp"](https://raw.githubusercontent.com/gsl-lite/gsl-lite/master/include/gsl/gsl-lite.hpp) to a
subdirectory of your project:

        git clone git@github.com:gsl-lite/gsl-lite.git <gsl-lite-source-dir>
        mkdir -p external/include/gsl
        cp <gsl-lite-source-dir>/include/gsl/gsl-lite.hpp external/include/gsl/
        
        g++ -std=c++03 -Iexternal/include main.cpp


Version semantics
-----------------

*gsl-lite* strives to follow [Semantic Versioning](https://semver.org/) guidelines. Although we are still in the "initial development" stage (version 0\.*), we generally maintain
[API](https://en.wikipedia.org/wiki/Application_programming_interface) and [ABI](https://en.wikipedia.org/wiki/Application_binary_interface) compatibility and avoid breaking changes in minor and patch releases.

Development of *gsl-lite* happens in the `master` branch. Versioning semantics apply only to tagged releases: there is no stability guarantee between individual commits in the `master` branch, i.e. anything
added since the last tagged release may be renamed, removed, have the semantics changed, etc. without further notice.

A minor-version release will be compatible (in both ABI and API) with the previous minor-version release (with [rare exceptions](https://github.com/gsl-lite/gsl-lite/issues/156) while we are still in version 0.\*).
Thus, once a change is released, it becomes part of the API.

Some of the [configuration options](#configuration-options) affect the API and ABI of *gsl-lite*. Most configuration options exist because a change we wanted to make would have broken backward compatibility,
so many recent changes and improvements are currently opt-in. The current plan is to toggle the default values of these configuration options for the next major version release.

To simplify migration to the next major version, *gsl-lite* 0.36 introduces the notion of *versioned defaults*. By setting the configuration option `gsl_CONFIG_DEFAULTS_VERSION=0` or `gsl_CONFIG_DEFAULTS_VERSION=1`,
a set of version-specific default options can be selected. Alternatively, when consuming *gsl-lite* [as a CMake package](#as-cmake-package), versioned defaults can be selected by linking to the target
`gsl::gsl-lite-v0` or `gsl::gsl-lite-v1` rather than `gsl::gsl-lite`.

The following table gives an overview of the configuration options affected by versioned defaults:


Macro                                                                                | v0 default                                               | v1 default        | |
------------------------------------------------------------------------------------:|:---------------------------------------------------------|-------------------|-|
[`gsl_FEATURE_OWNER_MACRO`](#gsl_feature_owner_macro1)                               | 1                                                        | 0                 | an unprefixed macro `Owner()` may interfere with user code |
[`gsl_FEATURE_GSL_LITE_NAMESPACE`](#gsl_feature_gsl_lite_namespace0)                 | 0                                                        | 1                 | cf. [Using *gsl-lite* in libraries](#using-gsl-lite-in-libraries) |
[`gsl_CONFIG_DEPRECATE_TO_LEVEL`](#gsl_config_deprecate_to_level0)                   | 0                                                        | 8                 | |
[`gsl_CONFIG_INDEX_TYPE`](#gsl_config_index_typegsl_config_span_index_type)          | `gsl_CONFIG_SPAN_INDEX_TYPE` (defaults to `std::size_t`) | `std::ptrdiff_t`  | the GSL specifies `gsl::index` to be a signed type, and M-GSL also uses `std::ptrdiff_t` |
[`gsl_CONFIG_ALLOWS_SPAN_COMPARISON`](#gsl_config_allows_span_comparison1)           | 1                                                        | 0                 | C++20 `std::span<>` does not support comparison because semantics (deep vs. shallow) are unclear |
[`gsl_CONFIG_NOT_NULL_EXPLICIT_CTOR`](#gsl_config_not_null_explicit_ctor0)           | 0                                                        | 1                 | cf. reasoning in [M-GSL/#395](https://github.com/Microsoft/GSL/issues/395) (note that `not_null<>` in M-GSL has an implicit constructor, cf. [M-GSL/#699](https://github.com/Microsoft/GSL/issues/699)) |
[`gsl_CONFIG_TRANSPARENT_NOT_NULL`](#gsl_config_transparent_not_null0)               | 0                                                        | 1                 | enables conformant behavior for `not_null<>::get()` |
[`gsl_CONFIG_NARROW_THROWS_ON_TRUNCATION`](#gsl_config_narrow_throws_on_truncation0) | 0                                                        | 1                 | enables conformant behavior for `narrow<>()` (cf. [#52](https://github.com/gsl-lite/gsl-lite/issues/52)) |

Note that the v1 defaults are not yet stable; future 0.\* releases may introduce more configuration switches with different version-specific defaults.



Using *gsl-lite* in libraries
-----------------------------

Many features of *gsl-lite* are very useful for defining library interfaces, e.g. spans, precondition checks, or `gsl::not_null<>`. As such, we encourage using *gsl-lite* in your libraries.
However, please mind the following considerations:

-  *gsl-lite* is an implementation of the [Guidelines Support Library](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-gsl), which is not a library but a non-formal specification.
   There are other libraries implementing the GSL, most notably the [Microsoft GSL](https://github.com/microsoft/GSL/) (herein often referred to as "M-GSL"). Both libraries live in different headers and consist
   of unrelated implementations. There is considerable API compatibility between M-GSL and *gsl-lite*, but some differences are inevitable because the GSL specification is rather loose and informal, and because
   both implementations take some liberties at interpreting and extending the specification (cf. e.g. [#6](https://github.com/gsl-lite/gsl-lite/issues/6), [#52](https://github.com/gsl-lite/gsl-lite/issues/52),
   [#153](https://github.com/gsl-lite/gsl-lite/issues/153)). Also, the ABIs of *gsl-lite* and M-GSL are generally incompatible.

-  It is not clear whether the GSL specification envisions that multiple implementations of the specification should coexist (cf. [CppCoreGuidelines/#1519](https://github.com/isocpp/CppCoreGuidelines/issues/1519)),
   but because all existing implementations currently live in the same `namespace gsl`, using more than one GSL implementation in the same target will usually fail with compile/link errors. This is clearly
   an impediment for using either in a library because the library would thereby force its consumers to pick the same GSL implementation.

-  The API and ABI of *gsl-lite* can be altered by some of the [configuration options](#configuration-options). We consider the availability of these options a strength of *gsl-lite*, but the lack
   of an option-invariant API and ABI is another burden for libraries, which may or may not depend on a particular choice of configuration settings and implicitly force these upon their users.

Our goal is to make *gsl-lite* suitable for use in libraries; we want to address all of these concerns in the next major version. But if you want to use *gsl-lite* in a library today, we recommend to

- use version-1 defaults (cf. [Version semantics](#version-semantics))
- include the new header \<gsl-lite/gsl-lite.hpp\> rather than \<gsl/gsl-lite.hpp\>
- refer to the new `namespace gsl_lite` instead of `namespace gsl` (or define a `namespace gsl = ::gsl_lite;` alias in your own namespace)
- use the prefixed contract checking macros `gsl_Expects()`/`gsl_Ensures()` rather than the unprefixed `Expects()`/`Ensures()`  
  (M-GSL prefixes its macros with uppercase `GSL_`; we traditionally consider lowercase `gsl_` the realm of *gsl-lite*)
- avoid any changes to the configuration options


Example:

```cmake
# my-statistics-lib/CMakeLists.txt
find_package( gsl-lite 0.41 REQUIRED )

add_library( my-statistics-lib STATIC mean.cpp )
target_link_libraries( my-statistics-lib PUBLIC gsl::gsl-lite-v1 )
```

```c++
// my-statistics-lib/include/my-statistics-lib/mean.hpp

#include <gsl-lite/gsl-lite.hpp>  // instead of <gsl/gsl-lite.hpp>

namespace my_statistics_lib {

    namespace gsl = ::gsl_lite; // convenience alias

    double mean( gsl::span<double const> elements )
    {
        gsl_Expects( !elements.empty() );  // instead of Expects()
        ...
    }

} // namespace my_statistics_lib
```

The idea is that *gsl-lite* will move all its definitions to `namespace gsl_lite` in the next major version, and provide a `namespace gsl` with aliases only if the traditional header \<gsl/gsl-lite.hpp\> is
included. This way, any code that only uses the new header \<gsl-lite/gsl-lite.hpp\> will not risk collision with M-GSL.


Configuration options
---------------------

**Contents**  
- [API macro](#api-macro)
- [Standard selection macro](#standard-selection-macro)
- [Feature selection macros](#feature-selection-macros)
- [Contract checking configuration macros](#contract-checking-configuration-macros)
- [Microsoft GSL compatibility macros](#microsoft-gsl-compatibility-macros)
- [Other configuration macros](#other-configuration-macros)

### API macro

#### `gsl_api`
Functions in *gsl-lite* are decorated with `gsl_api` where appropriate. **By default `gsl_api` is defined empty for non-CUDA platforms and `__host__ __device__` for the CUDA platform.** Define this macro to specify your own function decoration. 

### Standard selection macro

#### `gsl_CPLUSPLUS`
Define this macro to override the auto-detection of the supported C++ standard if your compiler does not set the `__cplusplus` macro correctly.

### Feature selection macros

#### `gsl_FEATURE_WITH_CONTAINER_TO_STD=99`
Define this to the highest C++ standard (98, 3, 11, 14, 17, 20) you want to include tagged-construction via `with_container`. **Default is 99 for inclusion with any standard.**

#### `gsl_FEATURE_MAKE_SPAN_TO_STD=99`
Define this to the highest C++ standard (98, 3, 11, 14, 17, 20) you want to include `make_span()` creator functions. **Default is 99 for inclusion with any standard.**

#### `gsl_FEATURE_BYTE_SPAN_TO_STD=99`
Define this to the highest C++ standard (98, 3, 11, 14, 17, 20) you want to include `byte_span()` creator functions. **Default is 99 for inclusion with any standard.**

#### `gsl_FEATURE_IMPLICIT_MACRO=0`
Define this macro to 1 to provide the `implicit` macro. **Default is 0.**

#### `gsl_FEATURE_OWNER_MACRO=1`
At default macro `Owner()` is defined for all C++ versions. This may be useful to transition  from a compiler that doesn't provide alias templates to one that does. Define this macro to 0 to omit the `Owner()` macro. **Default is 1.**

#### `gsl_FEATURE_EXPERIMENTAL_RETURN_GUARD=0`
Provide experimental types `final_action_return` and `final_action_error` and convenience functions `on_return()` and `on_error()`. **Default is 0.**

#### `gsl_FEATURE_GSL_LITE_NAMESPACE=0`
Define this to additionally define a `namespace gsl_lite` with most of the *gsl-lite* API available, cf. [Using *gsl-lite* in libraries](#using-gsl-lite-in-libraries). **Default is 0.**


### Contract checking configuration macros

*gsl-lite* provides contract violation response control as originally suggested in proposal [N4415](http://wg21.link/n4415), with
some refinements inspired by [P1710](http://wg21.link/P1710)/[P1730](http://wg21.link/P1730).

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

The macros `Expects()` and `Ensures()` are also provided as aliases for `gsl_Expects()` and `gsl_Ensures()`.

The behavior of the different flavors of pre-/postcondition checks and assertions depends on a number of configuration macros:

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


The following macros control whether contracts are checked at runtime:

- **`gsl_CONFIG_CONTRACT_CHECKING_AUDIT`**  
  Define this macro to have contracts expressed with `gsl_ExpectsAudit()`, `gsl_EnsuresAudit()`, and `gsl_AssertAudit()` checked
  at runtime.

- **`gsl_CONFIG_CONTRACT_CHECKING_ON` (default)**  
  Define this macro to have contracts expressed with `gsl_Expects()`, `gsl_Ensures()`, `gsl_Assert()`, and `gsl_FailFast()`
  checked at runtime. **This is the default.**
 
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


The following macros control the handling of runtime contract violations:

- **`gsl_CONFIG_CONTRACT_VIOLATION_TERMINATES` (default)**  
  Define this macro to call `std::terminate()` on a contract violation. **This is the default.**  
    
  Termination on contract violation is the behavior specified by the Core Guidelines for GSL contract checks.

- **`gsl_CONFIG_CONTRACT_VIOLATION_ASSERTS`**  
  If this macro is defined, and if the `assert()` macro is available for runtime checks (that is, if `NDEBUG` is not defined),
  contract checking macros are implemented in terms of `assert()`. If `assert()` is unavailable (i.e. if `NDEBUG` was defined),
  `std::abort()` is called directly when a contract is violated.  
    
  This option may be preferable over `gsl_CONFIG_CONTRACT_VIOLATION_TERMINATES` because `assert()` prints diagnostic information
  (such as the current source file, a line number, and the function name), and because vendor-specific extensions of `assert()`
  can be used (e.g. the `assert()` implementation of the Microsoft C runtime displays a dialog box which permits breaking into the
  debugger or continuing execution).  
    
  Note that `gsl_FailFast()` will call `std::abort()` if `assert()` continues execution.

- **`gsl_CONFIG_CONTRACT_VIOLATION_TRAPS`**  
  Define this macro to execute a trap instruction on a contract violation.  
    
  Trap instructions may yield smaller codegen and can thus result in better-performing code. However, they usually lead to
  catastrophic failure and may be difficult to diagnose for some platforms.

- **`gsl_CONFIG_CONTRACT_VIOLATION_THROWS`**  
  Define this macro to throw a `std::runtime_error`-derived exception `gsl::fail_fast` on contract violation.  
  Handling contract violations with exceptions can be desirable when executing in an interactive programming environment, or if
  there are other reasons why process termination must be avoided.  
    
  This macro is also useful for writing unit tests that exercise contract checks.

- **`gsl_CONFIG_CONTRACT_VIOLATION_CALLS_HANDLER`**  
  Define this macro to call a user-defined handler function `gsl::fail_fast_assert_handler()` on a contract violation.
  The user must provide a definition of the following function:
  ```c++
  namespace gsl {
      gsl_api void fail_fast_assert_handler(
          char const * expression, char const * message,
          char const * file, int line );
  }
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
  did), it cannot reliably suppress runtime evaluation for all compilers. E.g., if the contract check fed to the "assume"
  intrinsic comprises a function call which is opaque to the compiler, many compilers will generate the runtime function call.
  Therefore, `gsl_Expects()`, `gsl_Ensures()`, and `gsl_Assert()` should be used only for conditions that can be proven
  side-effect-free by the compiler, and `gsl_ExpectsDebug()`, `gsl_EnsuresDebug()`, `gsl_AssertDebug()`, `gsl_ExpectsAudit()`,
  `gsl_EnsuresAudit()`, and `gsl_AssertAudit()` for everything else. In practice, this means that `gsl_Expects()`,
  `gsl_Ensures()`, and `gsl_Assert()` should only be used for simple comparisons of scalar values, for simple inlineable getters,
  and for comparisons of class objects with trivially inlineable comparison operators.  
    
  Example:  
  ```c++
  template< class RandomIt >
  auto median( RandomIt first, RandomIt last )
  {
          // Comparing iterators for equality boils down to a comparison of pointers. An optimizing
          // compiler will inline the comparison operator and understand that the comparison is free
          // of side-effects, and hence generate no code in `gsl_CONFIG_UNENFORCED_CONTRACTS_ASSUME` mode.
      gsl_Expects( first != last );
  
          // Verifying that a range of elements is sorted may be an expensive operation, and we
          // cannot trust the compiler to understand that it is free of side-effects, so we use an
          // audit-level contract check.
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


### Microsoft GSL compatibility macros

#### `GSL_UNENFORCED_ON_CONTRACT_VIOLATION`

Equivalent to defining `gsl_CONFIG_CONTRACT_CHECKING_OFF`.

#### `GSL_TERMINATE_ON_CONTRACT_VIOLATION`

Equivalent to defining `gsl_CONFIG_CONTRACT_VIOLATION_TERMINATES`.

#### `GSL_THROW_ON_CONTRACT_VIOLATION`

Equivalent to defining `gsl_CONFIG_CONTRACT_VIOLATION_THROWS`.


### Other configuration macros

#### `gsl_CONFIG_DEPRECATE_TO_LEVEL=0`
Define this to and including the level you want deprecation; see table [Deprecation](#deprecation) below. **Default is 0 for no deprecation.**

#### `gsl_CONFIG_SPAN_INDEX_TYPE=std::size_t`
Define this macro to the type to use for indices in `span<>` and `basic_string_span<>`. Microsoft GSL uses `std::ptrdiff_t`. **Default for *gsl-lite* is `std::size_t`.**

#### `gsl_CONFIG_INDEX_TYPE=gsl_CONFIG_SPAN_INDEX_TYPE`
Define this macro to the type to use for `gsl::index`. Microsoft's GSL uses `std::ptrdiff_t`. **Default for *gsl-lite* is `std::size_t`.**

#### `gsl_CONFIG_NOT_NULL_EXPLICIT_CTOR=0`
Define this macro to 1 to make `not_null<>`'s constructor explicit. **Default is 0.** Note that in Microsoft's GSL the constructor is explicit. For implicit construction you can also use the *gsl-lite*-specific `not_null<>`-derived class `not_null_ic<>`.

#### `gsl_CONFIG_TRANSPARENT_NOT_NULL=0`
Define this macro to 1 to have `not_null<>` support typical member functions of the underlying smart pointer transparently (currently `get()`), while adding precondition checks. This is conformant behavior but may be incompatible with older code which expects that `not_null<>::get()` returns the underlying pointer itself. **Default is 0.**

#### `gsl_CONFIG_NOT_NULL_GET_BY_CONST_REF=0`
Define this macro to 1 to have the legacy non-transparent version of `not_null<>::get()` return `T const &` instead of `T`. This may improve performance with types that have an expensive copy-constructor. This macro must not be defined if `gsl_CONFIG_TRANSPARENT_NOT_NULL` is 1. **Default is 0 for `T`.**

#### `gsl_CONFIG_ALLOWS_SPAN_COMPARISON=1`
Define this macro to 0 to omit the ability to compare spans. C++20 `std::span<>` does not support comparison because semantics (deep vs. shallow) are unclear. **Default is 1.**

#### `gsl_CONFIG_ALLOWS_NONSTRICT_SPAN_COMPARISON=1`
Define this macro to 0 to omit the ability to compare spans of different types, e.g. of different const-volatile-ness. To be able to compare a string_span with a cstring_span, non-strict span comparison must be available. **Default is 1.**

#### `gsl_CONFIG_ALLOWS_UNCONSTRAINED_SPAN_CONTAINER_CTOR=0`
Define this macro to 1 to add the unconstrained span constructor for containers for pre-C++11 compilers that cannot constrain the constructor. This constructor may prove too greedy and interfere with other constructors. **Default is 0.**

Note: an alternative is to use the constructor tagged `with_container`: `span<V> s(gsl::with_container, cont)`.

#### `gsl_CONFIG_NARROW_THROWS_ON_TRUNCATION=0`
Define this macro to 1 to have `narrow<>()` always throw a `narrowing_error` exception if the narrowing conversion loses information due to truncation. If `gsl_CONFIG_NARROW_THROWS_ON_TRUNCATION` is 0 and `gsl_CONFIG_CONTRACT_VIOLATION_THROWS` is not defined, `narrow<>()` instead calls `std::terminate()` on information loss. **Default is 0.**

#### `gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS=0`
Define this macro to 1 to experience the by-design compile-time errors of the GSL components in the test suite. **Default is 0.**



Features
--------
See also section [GSL: Guidelines Support Library](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#S-gsl) of the C++ Core Guidelines [9]. 

Feature / library           | GSL     | M-GSL   | *gsl-lite* | Notes |
----------------------------|:-------:|:-------:|:----------:|:------|
**1.Lifetime&nbsp;safety**  | &nbsp;  | &nbsp;  | &nbsp;     | &nbsp; |
**1.1 Indirection**         | &nbsp;  | &nbsp;  | &nbsp;     | &nbsp; |
`not_null<>`                | ✓      | ✓      | ✓         | Wrap any indirection and enforce non-null,<br>see also [Other configuration macros](#other-configuration-macros) |
`not_null_ic<>`             | -       | -       | ✓         | not_null with implicit constructor, allowing [copy-initialization](https://en.cppreference.com/w/cpp/language/copy_initialization) |
**1.2 Ownership**           | &nbsp;  | &nbsp;  | &nbsp;     | &nbsp; |
`owner<>`                   | ✓      | ✓      | ≥&nbsp;C++11    | Owned raw pointers |
`Owner()`                   | -       | -       | ✓         | Macro for pre-C++11;<br>see also [Feature selection macros](#feature-selection-macros) |
`unique_ptr<>`              | ✓      | ✓      | ≥&nbsp;C++11    | `std::unique_ptr<>` |
`unique_ptr<>`              | -       | -       | <&nbsp;C++11    | VC10, VC11 |
`shared_ptr<>`              | ✓      | ✓      | ≥&nbsp;C++11    | `std::shared_ptr<>` |
`shared_ptr<>`              | -       | -       | <&nbsp;C++11    | VC10, VC11 |
`stack_array<>`             | ✓      | -       | -          | A stack-allocated array, fixed size |
`dyn_array<>`               | ?       | -       | -          | A heap-allocated array, fixed size |
**2.Bounds&nbsp;safety**    | &nbsp;  | &nbsp;  | &nbsp;     | &nbsp; |
**2.1 Tag Types**           | &nbsp;  | &nbsp;  | &nbsp;     | &nbsp; |
`zstring`                   | ✓      | ✓      | ✓         | a `char*` (C-style string) |
`wzstring`                  | -       | ✓      | ✓         | a `wchar_t*` (C-style string) |
`czstring`                  | ✓      | ✓      | ✓         | a `const char*` (C-style string) |
`cwzstring`                 | -       | ✓      | ✓         | a `const wchar_t*` (C-style string) |
`**2.2 Views**              | &nbsp;  | &nbsp;  | &nbsp;     | &nbsp; |
`span<>`                    | ✓      | ✓      | 1D&nbsp;views   | A view of contiguous T's, replace (*,len),<br>see also proposal [p0122](http://wg21.link/p0122) |
`span_p<>`                  | ✓      | -       | -          | A view of contiguous T's that ends at the first element for which predicate(*p) is true |
`make_span()`               | -       | ✓      | ✓         | Create a span |
`byte_span()`               | -       | -       | ✓         | Create a span of bytes from a single object |
`as_bytes()`                | -       | ✓      | ✓         | A span as bytes |
`as_writable_bytes`         | -       | ✓      | ✓         | A span as writable bytes |
`zstring_span`              | -       | ✓      | ✓         | `basic_zstring_span< char >` |
`wzstring_span`             | -       | ✓      | ✓         | `basic_zstring_span< wchar_t >` |
`czstring_span`             | -       | ✓      | ✓         | `basic_zstring_span< const char >` |
`cwzstring_span`            | -       | ✓      | ✓         | `basic_zstring_span< const wchar_t >` |
`ensure_z()`                | -       | ✓      | ✓         | Create a `cstring_span` or `cwstring_span` |
`to_string()`               | -       | ✓      | ✓         | Convert a `string_span` to `std::string` or `std::wstring` |
**2.3 Indexing**            | &nbsp;  | &nbsp;  | &nbsp;     | &nbsp; |
`at()`                      | ✓      | ✓      | ≥&nbsp;C++11    | Bounds-checked way of accessing<br>static arrays, `std::array<>`, `std::vector<>` |
`at()`                      | -       | -       | <&nbsp;C++11    | static arrays, `std::vector<>`<br>`std::array<>` : VC11 |
**3. Assertions**           | &nbsp;  | &nbsp;  | &nbsp;     | &nbsp; |
`Expects()`                 | ✓      | ✓      | ✓         | Precondition assertion |
`Ensures()`                 | ✓      | ✓      | ✓         | Postcondition assertion |
`gsl_Expects()`             | -       | -       | ✓         | Precondition assertion |
`gsl_Ensures()`             | -       | -       | ✓         | Postcondition assertion |
`gsl_Assert()`              | -       | -       | ✓         | Assertion |
`gsl_FailFast()`            | -       | -       | ✓         | Fail-fast termination |
`gsl_ExpectsDebug()`        | -       | -       | ✓         | Debug-mode precondition assertion |
`gsl_EnsuresDebug()`        | -       | -       | ✓         | Debug-mode postcondition assertion |
`gsl_AssertDebug()`         | -       | -       | ✓         | Debug-mode assertion |
`gsl_ExpectsAudit()`        | -       | -       | ✓         | Audit-level precondition assertion |
`gsl_EnsuresAudit()`        | -       | -       | ✓         | Audit-level postcondition assertion |
`gsl_AssertAudit()`         | -       | -       | ✓         | Audit-level assertion |
**4. Utilities**            | &nbsp;  | &nbsp;  | &nbsp;     | &nbsp; |
`index`                     | ✓      | ✓      | ✓         | type for container indexes and subscripts, <br>see [Other configuration macros](#other-configuration-macros) |
`dim`                       | -      | -      | ✓         | type for container sizes |
`stride`                    | -      | -      | ✓         | type for index strides |
`diff`                      | -      | -      | ✓        | type for index differences |
`byte`                      | -       | ✓      | ✓         | byte type, see also proposal [p0298](http://wg21.link/p0298) |
`final_action<>`            | ✓      | ✓      | ≥&nbsp;C++11    | Action at the end of a scope |
`finally()`                 | ✓      | ✓      | ≥&nbsp;C++11    | Make a `final_action<>` |
`on_return()`               | -       | -       | ≥&nbsp;C++11    | Make a `final_action_return<>, [experimental](#feature-selection-macros) |
`on_error()`                | -       | -       | ≥&nbsp;C++11    | Make a `final_action_error<>`, [experimental](#feature-selection-macros) |
`narrow_cast<>`             | ✓      | ✓      | ✓         | Searchable narrowing casts of values |
`narrow<>()`                | ✓      | ✓      | ✓         | Checked narrowing cast |
`narrow_failfast<>()`       | -       | -       | ✓         | Fail-fast narrowing cast |
`[[implicit]]`              | ✓      | -       | C++??      | Symmetric with explicit |
`implicit`                  | -       | -       | ✓         | Macro, see [Feature selection macros](#feature-selection-macros) |
`move_owner`                | ?       | -       | -          | ... |
**5. Algorithms**           | &nbsp;  | &nbsp;  | &nbsp;     | &nbsp; |
`copy()`                    | &nbsp;  | &nbsp;  | &nbsp;     | Copy from source span to destination span |
`size()`                    | &nbsp;  | &nbsp;  | &nbsp;     | Size of span, unsigned |
`ssize()`                   | &nbsp;  | &nbsp;  | &nbsp;     | Size of span, signed |
**6. Concepts**             | &nbsp;  | &nbsp;  | &nbsp;     | &nbsp; |
...                         | &nbsp;  | &nbsp;  | &nbsp;     | &nbsp; |

Note: *gsl-lite* treats VC12 (VS2013) and VC14 (VS2015) as C++11 (`gsl_CPP11_OR_GREATER`: 1).


Deprecation
-----------
The following features are deprecated since the indicated version. See macro [`gsl_CONFIG_DEPRECATE_TO_LEVEL`](#other-configuration-macros) on how to control deprecation using the indicated level.

Version | Level | Feature / Notes |
-------:|:-----:|:----------------|
0.42.0  |   8   | `finally()`, `on_return()`, and `on_error()` for pre-C++11        |
0.41.0  |   7   | `basic_string_span<>`, `basic_zstring_span<>` and related aliases |
0.37.0  |   6   | `as_writeable_bytes()`, call indexing for spans, and `span::at()` |
&nbsp;  |&nbsp; | Use `as_writable_bytes()`, subscript indexing |
0.35.0  |   -   | `gsl_CONFIG_CONTRACT_LEVEL_ON`, `gsl_CONFIG_CONTRACT_LEVEL_OFF`, `gsl_CONFIG_CONTRACT_LEVEL_EXPECTS_ONLY` and `gsl_CONFIG_CONTRACT_LEVEL_ENSURES_ONLY` |
&nbsp;  |&nbsp; | Use `gsl_CONFIG_CONTRACT_CHECKING_ON`, `gsl_CONFIG_CONTRACT_CHECKING_OFF`, `gsl_CONFIG_CONTRACT_CHECKING_ENSURES_OFF`, `gsl_CONFIG_CONTRACT_CHECKING_EXPECTS_OFF` |
0.31.0  |   5   | `span( std::nullptr_t, index_type )` |
&nbsp;  |&nbsp; | `span( pointer, index_type )` is used |
0.31.0  |   5   | `span( U *, index_type size )` |
&nbsp;  |&nbsp; | `span( pointer, index_type )` is used |
0.31.0  |   5   | `span( U (&arr)[N] )` |
&nbsp;  |&nbsp; | `span( element_type (&arr)[N] )` is used |
0.31.0  |   5   | `span( std::array< U, N > [const] & arr )` |
&nbsp;  |&nbsp; | `span( std::array< value_type, N > [const] & arr )` is used |
0.29.0  |   4   | `span( std::shared_ptr<T> const & p )` |
&nbsp;  |&nbsp; | &mdash; |
0.29.0  |   4   | `span( std::unique_ptr<T> const & p )` |
&nbsp;  |&nbsp; | &mdash; |
0.29.0  |   3   | `span<>::length()` |
&nbsp;  |&nbsp; | Use `span<>::size()` |
0.29.0  |   3   | `span<>::length_bytes()` |
&nbsp;  |&nbsp; | Use `span<>::size_bytes()` |
0.17.0  |   2   | member `span<>::as_bytes()`, `span<>::as_writeable_bytes()` |
&nbsp;  |&nbsp; | &mdash; |
0.7.0   |   -   | `gsl_CONFIG_ALLOWS_SPAN_CONTAINER_CTOR` |
&nbsp;  |&nbsp; | Use `gsl_CONFIG_ALLOWS_UNCONSTRAINED_SPAN_CONTAINER_CTOR`,<br>or consider `span(with_container, cont)`. |


Reported to work with
---------------------
The table below mentions the compiler versions and platforms *gsl-lite* is reported to work with.

Compiler             | OS              | Platforms | Versions          | CI |
--------------------:|:----------------|-----------|------------------:|----|
GCC                  | Linux           | x64       | 4.7 and newer     | [7, 8, 9, 10, 11](https://dev.azure.com/gsl-lite/gsl-lite/_build?definitionId=1) |
GCC (MinGW)          | Windows         | x86, x64  | 4.8.4 and newer   |    |
GCC (DJGPP)          | DOSBox, FreeDOS | x86       | 7.2               |    |
GCC                  | MacOS           | x64       | 6 and newer       | [10, 11, 12](https://dev.azure.com/gsl-lite/gsl-lite/_build?definitionId=1) |
Clang                | Linux           | x64       | 3.5 and newer     | [6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16](https://dev.azure.com/gsl-lite/gsl-lite/_build?definitionId=1) |
Clang with libstdc++ | Linux           | x64       | 11 and newer      | [16](https://dev.azure.com/gsl-lite/gsl-lite/_build?definitionId=1) |
Clang                | Windows         | x64       | version shipped with VS 2019 | [latest](https://dev.azure.com/gsl-lite/gsl-lite/_build?definitionId=1) |
MSVC (Visual Studio) | Windows         | x86, x64  | VS 2010 and newer | VS [2010, 2012, 2013, 2015, 2017](https://ci.appveyor.com/project/gsl-lite/gsl-lite), [2019, 2022](https://dev.azure.com/gsl-lite/gsl-lite/_build?definitionId=1) |
AppleClang (Xcode)   | MacOS           | x64       | 7.3 and newer     | [11.0.3, 12, 12.0.5, 13, 14](https://dev.azure.com/gsl-lite/gsl-lite/_build?definitionId=1) |
NVCC (CUDA Toolkit)  | Linux, Windows  | x64       | 10.2 and newer    | [11.8, 12.1](https://dev.azure.com/gsl-lite/gsl-lite/_build?definitionId=1) |
ARMCC                |                 | ARM       | 5 and newer       | |


Building the tests
------------------
To build the tests:

- [CMake](http://cmake.org), version 3.15 or later to be installed and in your PATH.
- A [suitable compiler](#reported-to-work-with). 

The [*lest* test framework](https://github.com/martinmoene/lest) is included in the [test folder](test).
 
The following steps assume that the [*gsl-lite* source code](https://github.com/gsl-lite/gsl-lite) has been cloned into a directory named `C:\gsl-lite`.

1. Create a directory for the build outputs.  
Here we use `C:\gsl-lite\build`.

        cd C:\gsl-lite
        mkdir build
        cd build

2. Configure the build directory with CMake:

        cmake -DGSL_LITE_OPT_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug ..

3. Build the test suite:

        cmake --build . --config Debug

4. Run the test suite:

        ctest -V -C Debug

All tests should pass, indicating your platform is supported and you are ready to use *gsl-lite*. See the table with [supported types and functions](#features).


Other GSL implementations
-------------------------
- Microsoft. [Guidelines Support Library (GSL)](https://github.com/microsoft/GSL).
- Vicente J. Botet Escriba. [Guidelines Support Library (GSL)](https://github.com/viboes/GSL).


Notes and references
--------------------
### Proposals, specification
[1] [`std::span<>` on cppreference](https://en.cppreference.com/w/cpp/container/span).  
[2] [`std::span<>` in C++20 Working Draft](http://eel.is/c++draft/views).  
[3] [P0091 - Template argument deduction for class templates](http://wg21.link/p0091).  
[4] [P0122 - span: bounds-safe views for sequences of objects](http://wg21.link/p0122).  
[5] [P0123 - string_span: bounds-safe views for sequences of characters](http://wg21.link/p0123).  
[6] [P0298 - A byte type definition](http://wg21.link/p0298).  
[7] [P0805 - Comparing Containers](http://wg21.link/p0805).

### Articles
[8] [Standard C++ Foundation](https://isocpp.org/).  
[9] Standard C++ Foundation. [C++ Core Guidelines](https://github.com/isocpp/CppCoreGuidelines).  
[10] Microsoft. [Guidelines Support Library (GSL)](https://github.com/microsoft/gsl).  
[11] Bjarne Stroustrup. [Writing good C++14 (PDF)](https://github.com/isocpp/CppCoreGuidelines/raw/master/talks/Stroustrup%20-%20CppCon%202015%20keynote.pdf) &mdash; [Video](https://www.youtube.com/watch?t=9&v=1OEu9C51K2A). CppCon 2015.  
[12] Herb Sutter. [Writing good C++14&hellip; By default (PDF)](https://github.com/isocpp/CppCoreGuidelines/raw/master/talks/Sutter%20-%20CppCon%202015%20day%202%20plenary%20.pdf) &mdash; [Video](https://www.youtube.com/watch?v=hEx5DNLWGgA). CppCon 2015.  
[13] Gabriel Dos Reis. [Contracts for Dependable C++ (PDF)](https://github.com/isocpp/CppCoreGuidelines/raw/master/talks/Contracts-for-Dependable-C%2B%2B.pdf)  &mdash; Video. CppCon 2015.  
[14] Bjarne Stroustrup et al. [A brief introduction to C++’s model for type- and resource-safety](https://github.com/isocpp/CppCoreGuidelines/raw/master/docs/Introduction%20to%20type%20and%20resource%20safety.pdf).  
[15] Herb Sutter and Neil MacIntosh. [Lifetime Safety: Preventing Leaks and Dangling](https://github.com/isocpp/CppCoreGuidelines/raw/master/docs/Lifetimes%20I%20and%20II%20-%20v0.9.1.pdf). 21 Sep 2015.

### Compiler feature testing
[16] cppreference.com. [Feature testing](https://en.cppreference.com/w/cpp/feature_test).

### C++ features in various compilers
[17] cppreference.com. [C++ compiler support](https://en.cppreference.com/w/cpp/compiler_support).


Appendix
--------

<a id="a1"></a>
### A.1 Compile-time information

In the test runner, the version of *gsl-lite* is available via tag `[.version]`. The following tags are available for information on the compiler and on the C++ standard library used: `[.compiler]`, `[.stdc++]`, `[.stdlanguage]` and `[.stdlibrary]`.

<a id="a2"></a>
### A.2 *gsl-lite* test specification

<details>
<summary>click to expand</summary>
<p>

```
gsl_Expects(): Allows a true expression
gsl_Ensures(): Allows a true expression
gsl_Assert(): Allows a true expression
gsl_Expects(): Terminates on a false expression
gsl_Ensures(): Terminates on a false expression
gsl_Assert(): Terminates on a false expression
gsl_FailFast(): Suppresses compiler warning about missing return value
gsl_FailFast(): Terminates
gsl_ExpectsDebug(): Allows a true expression
gsl_EnsuresDebug(): Allows a true expression
gsl_AssertDebug(): Allows a true expression
gsl_ExpectsDebug(): Terminates on a false expression in debug build or AUDIT mode
gsl_EnsuresAudit(): Terminates on a false expression in debug build or AUDIT mode
gsl_AssertAudit(): Terminates on a false expression in debug build or AUDIT mode
gsl_ExpectsAudit(): Allows a true expression
gsl_EnsuresAudit(): Allows a true expression
gsl_AssertAudit(): Allows a true expression
gsl_ExpectsAudit(): Terminates on a false expression in AUDIT mode
gsl_EnsuresAudit(): Terminates on a false expression in AUDIT mode
gsl_AssertAudit(): Terminates on a false expression in AUDIT mode
gsl_Expects(): No warnings produced for function calls in precondition checks
gsl_Expects(): Supports explicit conversions to bool
at(): Terminates access to non-existing C-array elements
at(): Terminates access to non-existing std::array elements (C++11)
at(): Terminates access to non-existing std::vector elements
at(): Terminates access to non-existing std::initializer_list elements (C++11)
at(): Terminates access to non-existing gsl::span elements
at(): Allows to access existing C-array elements
at(): Allows to access existing std::array elements (C++11)
at(): Allows to access existing std::vector elements
at(): Allows to access std::initializer_list elements (C++11)
at(): Allows to access gsl::span elements
byte: Allows to construct from integral via static cast (C++17)
byte: Allows to construct from integral via byte() (C++17)
byte: Allows to construct from integral via to_byte()
byte: Allows to convert to integral via to_integer()
byte: Allows comparison operations
byte: Allows bitwise or operation
byte: Allows bitwise and operation
byte: Allows bitwise x-or operation
byte: Allows bitwise or assignment
byte: Allows bitwise and assignment
byte: Allows bitwise x-or assignment
byte: Allows shift-left operation
byte: Allows shift-right operation
byte: Allows shift-left assignment
byte: Allows shift-right assignment
byte: Provides constexpr non-assignment operations (C++11)
byte: Provides constexpr assignment operations (C++14)
byte: Provides hash support (C++11)
equal()
lexicographical_compare()
conjunction<> and disjunction<>: Short-circuiting is handled correctly
conjunction<> and disjunction<>: First suitable type is chosen as base
span<>: free comparation functions fail for different const-ness [issue #32]
span<>: constrained container constructor suffers hard failure for arguments with reference-returning data() function [issue #242]
byte: aliasing rules lead to undefined behaviour when using enum class [issue #34](GSL issue #313, PR #390)
string_span<>: must not include terminating '\0' [issue #53]
string_span<>: to_string triggers SFINAE errors on basic_string_span's move & copy constructor with Clang-3.9 (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS) [issue #53a]
narrow<>(): Allows narrowing double to float without MSVC level 4 warning C4127: conditional expression is constant [issue #115]
detail::is_compatible_container<>: Not a proper type trait [PR #238]
not_null<>: Disallows default construction (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)
not_null<>: Disallows construction from nullptr_t, NULL or 0 (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)
not_null<>: Disallows construction from a unique pointer to underlying type (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)
not_null<>: Layout is compatible to underlying type
not_null<>: Convertibility is correctly reported by type traits
not_null<>: Copyability and assignability are correctly reported by type traits
not_null<>: Disallows assignment from unrelated pointers (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)
not_null<>: Terminates construction from a null pointer value (raw pointer)
not_null<>: Terminates construction from related pointer types for null pointer value (raw pointer)
not_null<>: Terminates assignment from a null pointer value (raw pointer)
not_null<>: Terminates assignment from related pointer types for null pointer value (raw pointer)
not_null<>: Allows to construct from a non-null underlying pointer (raw pointer)
not_null<>: Returns underlying pointer with get() (raw pointer)
not_null<>: Allows to construct from a non-null underlying pointer (raw pointer) with make_not_null()
not_null<>: Allows to construct from a non-null underlying pointer (raw pointer) with deduction guide
not_null<>: Allows to construct a const pointer from a non-null underlying pointer (raw pointer)
not_null<>: Converts to underlying pointer (raw pointer)
as_nullable: Converts to underlying pointer (raw pointer)
not_null<>: Allows to construct from a non-null related pointer (raw pointer)
not_null<>: Allows to construct a const pointer from a non-null related pointer (raw pointer)
not_null<>: Allows to construct from a not_null related pointer type (raw pointer)
not_null<>: Allows to construct a const pointer from a not_null related pointer type (raw pointer)
not_null<>: Converts to a related pointer (raw pointer)
as_nullable: Converts to a related pointer (raw pointer)
not_null<>: Allows assignment from a not_null related pointer type (raw pointer)
not_null<>: Allows assignment to a const pointer from a not_null related pointer type (raw pointer)
not_null<>: Allows indirect member access (raw pointer)
not_null<>: Allows dereferencing (raw pointer)
not_null<>: Allows to check whether object is valid (raw pointer)
not_null<>: Terminates swap of a moved-from value (shared_ptr)
not_null<>: Tolerates self-move-assignment of a moved-from value (shared_ptr)
not_null<>: Terminates self-swap of a moved-from value (shared_ptr)
not_null<>: Terminates construction from a null pointer value (shared_ptr)
not_null<>: Terminates construction from related pointer types for null pointer value (shared_ptr)
not_null<>: Terminates assignment from a null pointer value (shared_ptr)
not_null<>: Terminates assignment from related pointer types for null pointer value (shared_ptr)
not_null<>: Terminates propagation of a moved-from value (shared_ptr)
not_null<>: Allows self-swap (shared_ptr)
not_null<>: Allows swap (shared_ptr)
not_null<>: Allows to construct from a non-null underlying pointer (shared_ptr)
not_null<>: Allows to construct from a non-null raw pointer with explicit conversion (shared_ptr)
not_null<>: Returns underlying pointer or raw pointer with get() (shared_ptr)
not_null<>: Allows to move from a not_null pointer to an underlying pointer (shared_ptr)
as_nullable: Allows to move from a not_null pointer to an underlying pointer (shared_ptr)
not_null<>: Allows to construct from a non-null underlying pointer (shared_ptr) with make_not_null()
not_null<>: Allows to construct from a non-null underlying pointer (shared_ptr) with deduction guide
not_null<>: Allows to construct a const pointer from a non-null underlying pointer (shared_ptr)
not_null<>: Converts to underlying pointer (shared_ptr)
as_nullable: Converts to underlying pointer (shared_ptr)
as_nullable: Terminates for moved-from pointer (shared_ptr)
not_null<>: Allows to construct from a non-null related pointer (shared_ptr)
not_null<>: Allows to construct a const pointer from a non-null related pointer (shared_ptr)
not_null<>: Allows to construct from a not_null related pointer type (shared_ptr)
not_null<>: Allows to construct a const pointer from a not_null related pointer type (shared_ptr)
not_null<>: Converts to a related pointer (shared_ptr)
as_nullable: Converts to a related pointer (shared_ptr)
not_null<>: Allows assignment from a not_null related pointer type (shared_ptr)
not_null<>: Allows assignment to a const pointer from a not_null related pointer type (shared_ptr)
not_null<>: Allows indirect member access (shared_ptr)
not_null<>: Allows dereferencing (shared_ptr)
not_null<>: Allows to check whether object is valid (shared_ptr)
not_null<>: Terminates swap of a moved-from value (unique_ptr)
not_null<>: Tolerates self-move-assignment of a moved-from value (unique_ptr)
not_null<>: Terminates self-swap of a moved-from value (unique_ptr)
not_null<>: Terminates construction from a null pointer value (unique_ptr)
not_null<>: Terminates construction from related pointer types for null pointer value (unique_ptr)
not_null<>: Terminates assignment from a null pointer value (unique_ptr)
not_null<>: Terminates assignment from related pointer types for null pointer value (unique_ptr)
not_null<>: Terminates propagation of a moved-from value (unique_ptr)
not_null<>: Allows self-swap (unique_ptr)
not_null<>: Allows swap (unique_ptr)
not_null<>: Allows to construct from a non-null underlying pointer (unique_ptr)
not_null<>: Allows to construct from a non-null raw pointer with explicit conversion (unique_ptr)
not_null<>: Returns underlying pointer or raw pointer with get() (unique_ptr)
not_null<>: Allows to move from a not_null pointer to an underlying pointer (unique_ptr)
as_nullable: Allows to move from a not_null pointer to an underlying pointer (unique_ptr)
not_null<>: Allows to move to a related pointer from a not_null pointer (unique_ptr)
as_nullable: Allows to move to a related pointer from a not_null pointer (unique_ptr)
not_null<>: Allows to construct from a non-null underlying pointer (unique_ptr) with make_not_null()
not_null<>: Allows to construct from a non-null underlying pointer (unique_ptr) with deduction guide
not_null<>: Allows to construct a const pointer from a non-null underlying pointer (unique_ptr)
not_null<>: Converts to underlying pointer (unique_ptr)
as_nullable: Converts to underlying pointer (unique_ptr)
as_nullable: Terminates for moved-from pointer (unique_ptr)
not_null<>: Allows to construct from a non-null related pointer (unique_ptr)
not_null<>: Allows to construct a const pointer from a non-null related pointer (unique_ptr)
not_null<>: Allows to construct from a not_null related pointer type (unique_ptr)
not_null<>: Allows to construct a const pointer from a not_null related pointer type (unique_ptr)
not_null<>: Converts to a related pointer (unique_ptr)
as_nullable: Converts to a related pointer (unique_ptr)
not_null<>: Allows assignment from a not_null related pointer type (unique_ptr)
not_null<>: Allows assignment to a const pointer from a not_null related pointer type (unique_ptr)
not_null<>: Allows indirect member access (unique_ptr)
not_null<>: Allows dereferencing (unique_ptr)
not_null<>: Allows to check whether object is valid (unique_ptr)
not_null<>: Allows to construct a not_null<shared_ptr<T>> from a non-null unique_ptr<T>
not_null<>: Allows to construct a not_null<shared_ptr<const T>> from a non-null unique_ptr<T>
not_null<>: Allows to construct a not_null<shared_ptr<T>> from a related non-null unique_ptr<U>
not_null<>: Allows to construct a not_null<shared_ptr<const T>> from a related non-null unique_ptr<U>
not_null<>: Allows to construct a not_null<shared_ptr<T>> from a not_null<unique_ptr<T>>
not_null<>: Allows to convert to weak_ptr<T> from a not_null<shared_ptr<T>>
not_null<>: Allows to convert from a not_null<shared_ptr<T>> to a user-defined type with explicit conversion constructor
not_null<>: Allows to construct a not_null<shared_ptr<const T>> from a not_null<unique_ptr<T>>
not_null<>: Allows to construct a not_null<shared_ptr<T>> from a related not_null<unique_ptr<U>>
not_null<>: Allows to construct a not_null<shared_ptr<const T>> from a related not_null<unique_ptr<U>>
not_null<>: Allows assignment to a not_null<shared_ptr<T>> from a related not_null<unique_ptr<U>>
not_null<>: Allows assignment to a not_null<shared_ptr<const T>> from a related not_null<unique_ptr<U>>
not_null<>: make_unique<T>() returns not_null<unique_ptr<T>>
not_null<>: make_shared<T>() returns not_null<shared_ptr<T>>
not_null<>: Allows assignment from a non-null bare recast pointer
not_null<>: Allows implicit conversion to underlying type
not_null<>: Allows to construct from a non-null user-defined ref-counted type
not_null<>: Allows to compare equal to another not_null of the same type
not_null<>: Allows to compare unequal to another not_null of the same type
not_null<>: Allows to compare less than another not_null of the same type
not_null<>: Allows to compare less than or equal to another not_null of the same type
not_null<>: Allows to compare greater than another not_null of the same type
not_null<>: Allows to compare greater than or equal to another not_null of the same type
not_null<>: Allows to compare equal to a raw pointer of the same type
not_null<>: Allows to compare unequal to a raw pointer of the same type
not_null<>: Allows to compare less than a raw pointer of the same type
not_null<>: Allows to compare less than or equal to a raw pointer of the same type
not_null<>: Allows to compare greater than a raw pointer of the same type
not_null<>: Allows to compare greater than or equal to a raw pointer of the same type
not_null<>: Able to deduce element_type of raw pointers
not_null<>: Able to deduce element_type of unique_ptr
not_null<>: Able to deduce element_type of shared_ptr
not_null<>: Able to deduce element_type of normal user-defined smart pointers
not_null<>: Able to correctly deduce element_type of user-defined smart pointers even if typedef and result of dereferencing differs
not_null<>: Able to deduce element_type of user-defined smart pointers even if they do not have an element_type typedef
not_null<>: Able to deduce element_type of user-defined smart pointers even if they do not have an element_type typedef, and element_type differs from T
not_null<>: Can handle void*
not_null<>: Can handle unique_ptr<void, DeleterT>
not_null<>: Can handle shared_ptr<void>
not_null<>: Hashes match the hashes of the wrapped pointer
not_null<>: Hash functor disabled for non-hashable pointers and enabled for hashable pointers
owner<>: Disallows construction from a non-pointer type (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)
owner<>: Allows its use as the (pointer) type it stands for
Owner(): Allows its use as the (pointer) type it stands for
span<>: Disallows construction from a temporary value (C++11) (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)
span<>: Disallows construction from a C-array of incompatible type (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)
span<>: Disallows construction from a std::array of incompatible type (C++11) (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)
span<>: Terminates construction from a nullptr and a non-zero size (C++11)
span<>: Terminates construction from two pointers in the wrong order
span<>: Terminates construction from a null pointer and a non-zero size
span<>: Terminates creation of a sub span of the first n elements for n exceeding the span
span<>: Terminates creation of a sub span of the last n elements for n exceeding the span
span<>: Terminates creation of a sub span outside the span
span<>: Terminates access outside the span
span<>: Terminates access with front() and back() on empty span
span<>: Allows to default-construct
span<>: Allows to construct from a nullptr and a zero size (C++11)
span<>: Allows to construct from a single object (C++11)
span<>: Allows to construct from a const single object (C++11)
span<>: Allows to construct from two pointers
span<>: Allows to construct from two pointers to const
span<>: Allows to construct from a non-null pointer and a size
span<>: Allows to construct from a non-null pointer to const and a size
span<>: Allows to construct from a temporary pointer and a size
span<>: Allows to construct from a temporary pointer to const and a size
span<>: Allows to construct from any pointer and a zero size
span<>: Allows to construct from a C-array
span<>: Allows to construct from a const C-array
span<>: Allows to construct from a C-array with size via decay to pointer (potentially dangerous)
span<>: Allows to construct from a const C-array with size via decay to pointer (potentially dangerous)
span<>: Allows to construct from a std::initializer_list<> (C++11)
span<>: Allows to construct from a std::array<> (C++11)
span<>: Allows constexpr use (C++14)
span<>: Allows to construct from a std::array<> with const data (C++11) [deprecated-5]
span<>: Allows to construct from a container (std::vector<>)
span<>: Allows to construct from a temporary container (potentially dangerous)
span<>: Allows to tag-construct from a container (std::vector<>)
span<>: Allows to tag-construct from a temporary container (potentially dangerous)
span<>: Allows to construct from an empty gsl::shared_ptr (C++11) [deprecated-4]
span<>: Allows to construct from an empty gsl::unique_ptr (C++11) [deprecated-4]
span<>: Allows to construct from an empty gsl::unique_ptr (array, C++11) [deprecated-4]
span<>: Allows to construct from a non-empty gsl::shared_ptr (C++11) [deprecated-4]
span<>: Allows to construct from a non-empty gsl::unique_ptr (C++11) [deprecated-4]
span<>: Allows to construct from a non-empty gsl::unique_ptr (array, C++11) [deprecated-4]
span<>: Allows to default construct in a constexpr context
span<>: Allows to copy-construct from another span of the same type
span<>: Allows to copy-construct from another span of a compatible type
span<>: Allows to move-construct from another span of the same type (C++11)
span<>: Allows to copy-assign from another span of the same type
span<>: Allows to move-assign from another span of the same type (C++11)
span<>: Allows to create a sub span of the first n elements
span<>: Allows to create a sub span of the last n elements
span<>: Allows to create a sub span starting at a given offset
span<>: Allows to create a sub span starting at a given offset with a given length
span<>: Allows to create an empty sub span at full offset
span<>: Allows to create an empty sub span at full offset with zero length
span<>: Allows forward iteration
span<>: Allows const forward iteration
span<>: Allows reverse iteration
span<>: Allows const reverse iteration
span<>: Allows to observe an element via array indexing
span<>: Allows to observe an element via front() and back()
span<>: Allows to observe an element via data()
span<>: Allows to change an element via array indexing
span<>: Allows to change an element via front() and back()
span<>: Allows to change an element via data()
span<>: Allows to test for empty span via empty(), empty case
span<>: Allows to test for empty span via empty(), non-empty case
span<>: Allows to obtain the number of elements via size(), as configured
span<>: Allows to obtain the number of elements via ssize(), signed
span<>: Allows to obtain the number of elements via length() [deprecated-3]
span<>: Allows to obtain the number of bytes via size_bytes()
span<>: Allows to obtain the number of bytes via length_bytes() [deprecated-3]
span<>: Allows to swap with another span of the same type
span<>: Allows to view the elements as read-only bytes [deprecated-2 as member]
span<>: Allows to view and change the elements as writable bytes [deprecated-2 as member]
span<>: Allows to view the elements as a span of another type
span<>: Allows to change the elements from a span of another type
copy(): Allows to copy a span to another span of the same element type
copy(): Allows to copy a span to another span of a different element type
size(): Allows to obtain the number of elements in span via size(span), unsigned
ssize(): Allows to obtain the number of elements in span via ssize(span), signed
make_span(): (gsl_FEATURE_MAKE_SPAN=1)
make_span(): Allows to build from two pointers
make_span(): Allows to build from two const pointers
make_span(): Allows to build from a non-null pointer and a size
make_span(): Allows to build from a non-null const pointer and a size
make_span(): Allows to build from a C-array
make_span(): Allows to build from a const C-array
make_span(): Allows building from a std::initializer_list<> (C++11)
make_span(): Allows to build from a std::array<> (C++11)
make_span(): Allows to build from a const std::array<> (C++11)
make_span(): Allows to build from a container (std::vector<>)
make_span(): Allows to build from a const container (std::vector<>)
make_span(): Allows to build from a temporary container (potentially dangerous)
make_span(): Allows to tag-build from a container (std::vector<>)
make_span(): Allows to tag-build from a temporary container (potentially dangerous)
make_span(): Allows to build from an empty gsl::shared_ptr (C++11) [deprecated-4]
make_span(): Allows to build from an empty gsl::unique_ptr (C++11) [deprecated-4]
make_span(): Allows to build from an empty gsl::unique_ptr (array, C++11) [deprecated-4]
make_span(): Allows to build from a non-empty gsl::shared_ptr (C++11) [deprecated-4]
make_span(): Allows to build from a non-empty gsl::unique_ptr (C++11) [deprecated-4]
make_span(): Allows to build from a non-empty gsl::unique_ptr (array, C++11) [deprecated-4]
byte_span() (gsl_FEATURE_BYTE_SPAN=1)
byte_span(): Allows to build a span of gsl::byte from a single object
byte_span(): Allows to build a span of const gsl::byte from a single const object
string_span: Disallows construction of a string_span from a cstring_span (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)
string_span: Disallows construction of a string_span from a const std::string (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)
string_span: Allows to default-construct
string_span: Allows to construct from a nullptr (C++11)
string_span: Allows to construct a cstring_span from a const C-string
string_span: Allows to construct a string_span from a non-const C-string and size
string_span: Allows to construct a string_span from a non-const C-string begin and end pointer
string_span: Allows to construct a string_span from a non-const C-array
string_span: Allows to construct a string_span from a non-const std::string
string_span: Allows to construct a string_span from a non-const std::array (C++11)
string_span: Allows to construct a string_span from a non-const container (std::vector)
string_span: Allows to construct a string_span from a non-const container, via a tag (std::vector)
string_span: Allows to construct a cstring_span from a non-const C-string and size
string_span: Allows to construct a cstring_span from a non-const C-string begin and end pointer
string_span: Allows to construct a cstring_span from a non-const C-array
string_span: Allows to construct a cstring_span from a non-const std::string
string_span: Allows to construct a cstring_span from a non-const std::array (C++11)
string_span: Allows to construct a cstring_span from a non-const container (std::vector)
string_span: Allows to construct a cstring_span from a non-const container, via a tag (std::vector)
string_span: Allows to construct a cstring_span from a const C-string and size
string_span: Allows to construct a cstring_span from a non-const C-string begin and end pointer
string_span: Allows to construct a cstring_span from a const C-array
string_span: Allows to construct a cstring_span from a const std::string
string_span: Allows to construct a cstring_span from a const std::array (C++11)
string_span: Allows to construct a cstring_span from a const container (std::vector)
string_span: Allows to construct a cstring_span from a const container, via a tag (std::vector)
string_span: Allows to construct a wstring_span from a non-const C-string and size
string_span: Allows to construct a wstring_span from a non-const C-string begin and end pointer
string_span: Allows to construct a wstring_span from a non-const C-array
string_span: Allows to construct a wstring_span from a non-const std::wstring
string_span: Allows to construct a wstring_span from a non-const std::array (C++11)
string_span: Allows to construct a wstring_span from a non-const container (std::vector)
string_span: Allows to construct a wstring_span from a non-const container, via a tag (std::vector)
string_span: Allows to construct a cwstring_span from a non-const C-string and size
string_span: Allows to construct a cwstring_span from a non-const C-string begin and end pointer
string_span: Allows to construct a cwstring_span from a non-const C-array
string_span: Allows to construct a cwstring_span from a non-const std::wstring
string_span: Allows to construct a cwstring_span from a non-const std::array (C++11)
string_span: Allows to construct a cwstring_span from a non-const container (std::vector)
string_span: Allows to construct a cwstring_span from a non-const container, via a tag (std::vector)
string_span: Allows to construct a cwstring_span from a const C-string and size
string_span: Allows to construct a cwstring_span from a const C-string begin and end pointer
string_span: Allows to construct a cwstring_span from a const C-array
string_span: Allows to construct a cwstring_span from a const std::wstring
string_span: Allows to construct a cwstring_span from a const std::array (C++11)
string_span: Allows to construct a cwstring_span from a const container (std::vector)
string_span: Allows to construct a cwstring_span from a const container, via a tag (std::vector)
string_span: Allows to copy-construct from another span of the same type
string_span: Allows to copy-construct from another span of a compatible type
string_span: Allows to move-construct from another span of the same type (C++11)
string_span: Allows to copy-assign from another span of the same type
string_span: Allows to move-assign from another span of the same type (C++11)
string_span: Allows to create a sub span of the first n elements
string_span: Allows to create a sub span of the last n elements
string_span: Allows to create a sub span starting at a given offset
string_span: Allows to create a sub span starting at a given offset with a given length
string_span: Allows to create an empty sub span at full offset
string_span: Allows to create an empty sub span at full offset with zero length
string_span: Allows forward iteration
string_span: Allows const forward iteration
string_span: Allows reverse iteration
string_span: Allows const reverse iteration
string_span: Allows to observe an element via array indexing
string_span: Allows to observe an element via front() and back()
string_span: Allows to observe an element via data()
string_span: Allows to change an element via array indexing
string_span: Allows to change an element via front() and back()
string_span: Allows to change an element via data()
string_span: Allows to compare a string_span with another string_span
string_span: Allows to compare empty span to non-empty span
string_span: Allows to compare a string_span with a cstring_span
string_span: Allows to compare with types convertible to string_span
string_span: Allows to test for empty span via empty(), empty case
string_span: Allows to test for empty span via empty(), non-empty case
string_span: Allows to obtain the number of elements via length()
string_span: Allows to obtain the number of elements via size()
string_span: Allows to obtain the number of bytes via length_bytes()
string_span: Allows to obtain the number of bytes via size_bytes()
string_span: Allows to view the elements as read-only bytes
zstring_span: Terminates construction of a zstring_span from an empty span
zstring_span: Allows to construct a zstring_span from a zero-terminated empty string (via span)
zstring_span: Allows to construct a zstring_span from a zero-terminated non-empty string (via span)
zstring_span: Terminates construction of a zstring_span from a non-zero-terminated string (via span)
zstring_span: Terminates construction of a wzstring_span from an empty span
zstring_span: Allows to construct a wzstring_span from a zero-terminated empty string (via span)
zstring_span: Allows to construct a wzstring_span from a zero-terminated non-empty string (via span)
zstring_span: Terminates construction of a wzstring_span from a non-zero-terminated string (via span)
zstring_span: Allows to use a zstring_span with a legacy API via member assume_z()
zstring_span: Allows to use a wzstring_span with a legacy API via member assume_z()
to_string(): Allows to explicitly convert from string_span to std::string
to_string(): Allows to explicitly convert from cstring_span to std::string
to_string(): Allows to explicitly convert from wstring_span to std::wstring
to_string(): Allows to explicitly convert from cwstring_span to std::wstring
ensure_z(): Disallows to build a string_span from a const C-string
ensure_z(): Disallows to build a wstring_span from a const wide C-string
ensure_z(): Allows to build a string_span from a non-const C-string
ensure_z(): Allows to build a cstring_span from a non-const C-string
ensure_z(): Allows to build a cstring_span from a const C-string
ensure_z(): Allows to build a wstring_span from a non-const wide C-string
ensure_z(): Allows to build a cwstring_span from a non-const wide C-string
ensure_z(): Allows to build a cwstring_span from a const wide C-string
ensure_z(): Allows to specify ultimate location of the sentinel and ensure its presence
operator<<: Allows printing a string_span to an output stream
operator<<: Allows printing a cstring_span to an output stream
operator<<: Allows printing a wstring_span to an output stream
operator<<: Allows printing a cwstring_span to an output stream
finally: Allows to run lambda on leaving scope
finally: Allows to run function (bind) on leaving scope
finally: Allows to run function (pointer) on leaving scope
on_return: Allows to perform action on leaving scope without exception (gsl_FEATURE_EXPERIMENTAL_RETURN_GUARD)
on_error: Allows to perform action on leaving scope via an exception (gsl_FEATURE_EXPERIMENTAL_RETURN_GUARD)
narrow_cast<>: Allows narrowing without value loss
narrow_cast<>: Allows narrowing with value loss
narrow<>(): Allows narrowing without value loss
narrow<>(): Terminates when narrowing with value loss
narrow<>(): Terminates when narrowing with sign loss
narrow_failfast<>(): Allows narrowing without value loss
narrow_failfast<>(): Terminates when narrowing with value loss
narrow_failfast<>(): Terminates when narrowing with sign loss
CUDA: Precondition/postcondition checks and assertions can be used in kernel code
CUDA: span<> can be passed to kernel code
CUDA: span<> can be used in kernel code
CUDA: not_null<> can be passed to and used in kernel code
CUDA: gsl_FailFast() can be used in kernel code
```
</p>
</details>
