# GSL Lite: Guideline Support Library for C++98, C++11 up

[![Language](https://img.shields.io/badge/language-C++-blue.svg)](https://isocpp.org/)  [![Standard](https://img.shields.io/badge/c%2B%2B-98-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization) [![Standard](https://img.shields.io/badge/c%2B%2B-11-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization) [![Standard](https://img.shields.io/badge/c%2B%2B-14-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization) [![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT) [![Build Status](https://travis-ci.org/martinmoene/gsl-lite.svg?branch=master)](https://travis-ci.org/martinmoene/gsl-lite) [![Build status](https://ci.appveyor.com/api/projects/status/1ha3wnxtam547m8p?svg=true)](https://ci.appveyor.com/project/martinmoene/gsl-lite) [![Version](https://badge.fury.io/gh/martinmoene%2Fgsl-lite.svg)](https://github.com/martinmoene/gsl-lite/releases) [![download](https://img.shields.io/badge/latest%20version%20%20-download-blue.svg)](https://raw.githubusercontent.com/martinmoene/gsl-lite/master/include/gsl/gsl-lite.h)


GSL Lite is based on the [Microsoft Guideline Support Library (GSL)](https://github.com/microsoft/gsl). 

**Contents**  
- [Example usage](#example-usage)
- [In a nutshell](#in-a-nutshell)
- [License](#license)
- [Dependencies](#dependencies)
- [Installation](#installation)
- [Synopsis](#synopsis)
- [Features](#features)
- [Reported to work with](#reported-to-work-with)
- [Building the tests](#building-the-tests)
- [Other GSL implementations](#other-gsl-implementations)
- [Notes and references](#notes-and-references)
- [Appendix](#appendix)


Example usage
-------------

```Cpp
#include "gsl-lite.h"

using namespace gsl;

int * use( not_null<int *> p ) 
{
    // use p knowing it's not nullptr, NULL or 0.
    
    return p;
}

struct Widget
{
    Widget() : owned_ptr( new int(42) ) {}
    ~Widget() { delete owned_ptr; }

    void work() { non_owned_ptr = use( owned_ptr ); }
    
    owner<int *> owned_ptr;	// if alias template support
//  Owner(int *) owned_ptr;	// C++98 up
    int * non_owned_ptr;
};

int main()
{
    Widget w;
    w.work();
}
```

### Compile and run

```
prompt>g++ -std=c++03 -Wall -I../include/gsl -o 01-basic.exe 01-basic.cpp && 01-basic.exe
```

In a nutshell
-------------
**gsl-lite** is a single-file header-only variant of Microsoft's implementation of the [Guideline Support Library (GSL)](https://github.com/Microsoft/GSL) adapted for C++98, C++03. It should also work when compiled as C++11, C++14. 

The Guideline Support Library (GSL) contains functions and types that are suggested for use by the [C++ Core Guidelines](https://github.com/isocpp/CppCoreGuidelines) maintained by the [Standard C++ Foundation](https://isocpp.org/). The library includes types like `owner<>`, `not_null<>`, `span<>`, `string_span` and [others](#features).

*gsl-lite* recognizes when it is compiled for the CUDA platform and decorates functions (methods) with `__host__` and `__device__`. See also section [API macro](#api-macro).


License
-------
*gsl-lite* uses the [MIT](LICENSE) license.
 

Dependencies
------------
*gsl-lite* has no other dependencies than the [C++ standard library](http://en.cppreference.com/w/cpp/header).


Installation
------------
*gsl-lite* is a single-file header-only library. Put `gsl-lite.h` in the [include](include) folder directly into the project source tree or somewhere reachable from your project.


Synopsis
--------

**Contents**  
- [API macro](#api-macro)
- [Feature selection macros](#feature-selection-macros)
- [Contract violation response macros](#contract-violation-response-macros)
- [Microsoft GSL compatibility macros](#microsoft-gsl-compatibility-macros)
- [Other configuration macros](#other-configuration-macros)

### API macro

\-D<b>gsl\_api</b>=""  
Functions (methods) are decorated with `gsl_api`. At default `gsl_api` is defined empty for non-CUDA platforms and `__host__ __device__` for the CUDA platform. Define this macro to specify your own function decoration. 

### Feature selection macros

\-D<b>gsl\_FEATURE\_HAVE\_IMPLICIT\_MACRO</b>=1  
Define this macro to 0 to omit the `implicit` macro. Default is 1.

\-D<b>gsl\_FEATURE\_HAVE\_OWNER\_MACRO</b>=1  
At default macro `Owner()` is defined for all C++ versions. This may be useful to transition  from a compiler that doesn't provide alias templates to one that does. Define this macro to 0 to omit the `Owner()` macro. Default is 1.

\-D<b>gsl\_FEATURE\_EXPERIMENTAL\_RETURN\_GUARD</b>=0  
Provide experimental types `final_act_return` and `final_act_error` and convenience functions `on_return()` and `on_error()`. Default is 0.

### Contract violation response macros

*gsl-lite* provides contract violation response control as suggested in proposal [N4415](http://wg21.link/n4415).

\-D<b>gsl\_CONFIG\_CONTRACT\_LEVEL\_ON</b>  
Define this macro to include both `Expects` and `Ensures` in the code. This is the default case.
 
\-D<b>gsl\_CONFIG\_CONTRACT\_LEVEL\_OFF</b>  
Define this macro to exclude both `Expects` and `Ensures` from the code.

\-D<b>gsl\_CONFIG_CONTRACT\_LEVEL\_EXPECTS\_ONLY</b>  
Define this macro to include `Expects` in the code and exclude `Ensures` from the code.

\-D<b>gsl\_CONFIG\_CONTRACT\_LEVEL\_ENSURES\_ONLY</b>  
Define this macro to exclude `Expects` from the code and include `Ensures` in the code.

\-D<b>gsl\_CONFIG\_CONTRACT\_VIOLATION\_TERMINATES</b>  
Define this macro to call `std::terminate()` on a GSL contract violation in `Expects`, `Ensures` and `narrow`. This is the default case.

\-D<b>gsl\_CONFIG\_CONTRACT\_VIOLATION\_THROWS</b>  
Define this macro to throw a std::runtime_exception-derived exception `gsl::fail_fast` instead of calling `std::terminate()` on a GSL contract violation in `Expects`, `Ensures` and throw a std::exception-derived exception `narrowing_error` on discarding  information in `narrow`.

### Microsoft GSL compatibility macros

\-D<b>GSL_UNENFORCED_ON_CONTRACT_VIOLATION</b>  
Equivalent to -Dgsl_CONFIG_CONTRACT_LEVEL_OFF.

\-D<b>GSL\_THROW\_ON\_CONTRACT\_VIOLATION</b>  
Equivalent to -Dgsl\_CONFIG\_CONTRACT\_VIOLATION\_THROWS.

\-D<b>GSL\_TERMINATE\_ON\_CONTRACT\_VIOLATION</b>  
Equivalent to -Dgsl\_CONFIG\_CONTRACT\_VIOLATION\_TERMINATES.

### Other configuration macros

\-D<b>gsl\_CONFIG\_SPAN\_INDEX\_TYPE</b>=size_t
Define this macro to the type to use for indices in `span` and `basic_string_span`. Microsoft's GSL uses `std::ptrdiff_t`. Default for *gsl lite* is `size_t`.

\-D<b>gsl\_CONFIG\_ALLOWS\_NONSTRICT\_SPAN\_COMPARISON</b>=1  
Define this macro to 0 to omit the ability to compare spans of different types, e.g. of different const-volatile-ness. To be able to compare a string_span with a cstring_span, non-strict span comparison must be available. Default is 1.

\-D<b>gsl\_CONFIG\_ALLOWS\_UNCONSTRAINED\_SPAN\_CONTAINER\_CTOR</b>=0  
Define this macro to 1 to add the unconstrained span constructor for containers for pre-C++11 compilers that cannot constrain the constructor. This constructor may prove too greedy and interfere with other constructors. Default is 0.

Note: an alternative is to use the constructor tagged `with_container`: span&lt;_value_type_> *s*(with_container, *cont*). 

\-D<b>gsl\_CONFIG\_CONFIRMS\_COMPILATION\_ERRORS</b>=0  
Define this macro to 1 to experience the by-design compile-time errors of the GSL components in the test suite. Default is 0.


Features
--------
See also section [GSL: Guideline support library](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#S-gsl) of the C++ Core Guidelines [2]. 

Feature / library           | GSL     | M-GSL   | GSL-Lite| Notes |
----------------------------|:-------:|:-------:|:-------:|:------|
**1.Lifetime&nbsp;safety**  | &nbsp;  | &nbsp;  | &nbsp;  | &nbsp; |
**1.1 Indirection**         | &nbsp;  | &nbsp;  | &nbsp;  | &nbsp; |
not_null<>                  | &#10003;| &#10003;| &#10003;| Wrap any indirection and enforce non-null |
**1.2 Ownership**           | &nbsp;  | &nbsp;  | &nbsp;  | &nbsp; |
owner<>                     | &#10003;| &#10003;| >=C++11 | Owned raw pointers |
Owner()                     | -       | -       | &#10003;| Macro for pre-C++11;<br>see also [Feature selection macros](#feature-selection-macros) |
unique_ptr<>                | &#10003;| &#10003;| >=C++11 | std::unique_ptr<> |
unique_ptr<>                | -       | -       | < C++11 | VC10, VC11 |
shared_ptr<>                | &#10003;| &#10003;| >=C++11 | std::shared_ptr<> |
shared_ptr<>                | -       | -       | < C++11 | VC10, VC11<br>see also [Extract Boost smart pointers](#a1-extract-boost-smart-pointers) |
stack_array<>               | &#10003;| -       | -       | A stack-allocated array, fixed size |
dyn_array<>                 | ?       | -       | -       | A heap-allocated array, fixed size |
**2.Bounds&nbsp;safety**    | &nbsp;  | &nbsp;  | &nbsp;  | &nbsp; |
**2.1 Tag Types**           | &nbsp;  | &nbsp;  | &nbsp;  | &nbsp; |
zstring                     | &#10003;| &#10003;| &#10003;| a char* (C-style string) |
wzstring                    | -       | &#10003;| &#10003;| a wchar_t* (C-style string) |
czstring                    | &#10003;| &#10003;| &#10003;| a const char* (C-style string) |
cwzstring                   | -       | &#10003;| &#10003;| a const wchar_t* (C-style string) |
**2.2 Views**               | &nbsp;  | &nbsp;  | &nbsp;  | &nbsp; |
span<>                      | &#10003;| &#10003;| 1D views| A view of contiguous T's, replace (*,len) |
span_p<>                    | &#10003;| -       | -       | A view of contiguous T's that ends at the first element for which predicate(*p) is true |
make_span()                 | -       | &#10003;| &#10003;| Create a span |
as_bytes()                  | -       | &#10003;| &#10003;| A span as bytes |
as_writeable_bytes          | -       | &#10003;| &#10003;| A span as writeable bytes |
basic_string_span<>         | -       | &#10003;| &#10003;| See also propasal [p0123](http://wg21.link/p0123) |
string_span                 | &#10003;| &#10003;| &#10003;| basic_string_span&lt;char> |
wstring_span                | -       | &#10003;| &#10003;| basic_string_span&lt;wchar_t > |
cstring_span                | &#10003;| &#10003;| &#10003;| basic_string_span&lt;const char> |
cwstring_span               | -       | &#10003;| &#10003;| basic_string_span&lt;const wchar_t > |
ensure_z()                  | -       | &#10003;| &#10003;| Create a cstring_span or cwstring_span |
to_string()                 | -       | &#10003;| &#10003;| Convert a string_span to std::string or std::wstring |
**2.3 Indexing**            | &nbsp;  | &nbsp;  | &nbsp;  | &nbsp; |
at()                        | &#10003;| &#10003;| >=C++11 | Bounds-checked way of accessing<br>static arrays, std::array, std::vector |
at()                        | -       | -       | < C++11 | static arrays, std::vector<br>std::array : VC11 |
**3. Assertions**           | &nbsp;  | &nbsp;  | &nbsp;  | &nbsp; |
Expects()                   | &#10003;| &#10003;| &#10003;| Precondition assertion |
Ensures()                   | &#10003;| &#10003;| &#10003;| Postcondition assertion |
**4. Utilities**            | &nbsp;  | &nbsp;  | &nbsp;  | &nbsp; |
byte                        | -       | &#10003;| &#10003;| byte type, see also proposal [p0298](http://wg21.link/p0298) |
final_act<>                 | &#10003;| &#10003;| >=C++11 | Action at the end of a scope |
final_act                   | -       | -       | < C++11 | Currently only void(*)() |
finally()                   | &#10003;| &#10003;| >=C++11 | Make a final_act<> |
finally()                   | -       | -       | < C++11 | Make a final_act |
final_act_return            | -       | -       | < C++11 | Currently only void(*)(), [experimental](#feature-selection-macros) |
on_return()                 | -       | -       | >=C++11 | Make a final_act_return<>, [experimental](#feature-selection-macros) |
on_return()                 | -       | -       | < C++11 | Make a final_act_return, [experimental](#feature-selection-macros) |
final_act_error             | -       | -       | < C++11 | Currently only void(*)(), [experimental](#feature-selection-macros) |
on_error()                  | -       | -       | >=C++11 | Make a final_act_error<>, [experimental](#feature-selection-macros) |
on_error()                  | -       | -       | < C++11 | Make a final_act_error, [experimental](#feature-selection-macros) |
narrow_cast<>               | &#10003;| &#10003;| &#10003;| Searchable narrowing casts of values |
narrow()                    | &#10003;| &#10003;| &#10003;| Checked version of narrow_cast() |
implicit                    | &#10003;| -       | &#10003;| Symmetric with explicit |
move_owner                  | ?       | -       | -       | ... |
**5. Algorithms**           | &nbsp;  | &nbsp;  | &nbsp;  | &nbsp; |
copy()                      | &nbsp;  | &nbsp;  | &nbsp;  | Copy from source span to destination span |
**6. Concepts**             | &nbsp;  | &nbsp;  | &nbsp;  | &nbsp; |
...                         | &nbsp;  | &nbsp;  | &nbsp;  | &nbsp; |

Note: GSL Lite treats VC12 (VS2013) and VC14 (VS2015) as C++11 (gsl_CPP11_OR_GREATER: 1).


Reported to work with
---------------------
The table below mentions the compiler versions *gsl-lite* is reported to work with.

OS        | Compiler   | Versions |
---------:|:-----------|:---------|
Windows   | Clang/LLVM | ? |
&nbsp;    | GCC        | 4.8.4, 4.9.2, 5.2.0 |
&nbsp;    | Visual C++<br>(Visual Studio)| 6 (6) via header [gsl-lite-vc6.h](include/gsl/gsl-lite-vc6.h)<br>8 (2005), 10 (2010), 11 (2012),<br>12 (2013), 14 (2015) |
GNU/Linux | Clang/LLVM | 3.4 |
&nbsp;    | GCC        | 5.1 |
OS X      | ?          | ?   |


Building the tests
------------------
To build the tests you need:

- [CMake](http://cmake.org), version 2.8.7 or later to be installed and in your PATH.
- A [suitable compiler](#reported-to-work-with). 

The [*lest* test framework](https://github.com/martinmoene/lest)  is included in the [test folder](test).
 
The following steps assume that the [GSL Lite source code](https://github.com/martinmoene/gsl-lite) has been cloned into a directory named `c:\gsl-lite`.

1. Create a directory for the build outputs for a particular architecture.  
Here we use c:\gsl-lite\build-win-x86-vc10.

        cd c:\gsl-lite
        md build-win-x86-vc10
        cd build-win-x86-vc10

2. Configure CMake to use the compiler of your choice (run `cmake --help` for a list).

        cmake -G "Visual Studio 10 2010" ..

3. Build the test suite in the Debug configuration (alternatively use Release).    

        cmake --build . --config Debug

4. Run the test suite.    

        ctest -V -C Debug

All tests should pass, indicating your platform is supported and you are ready to use *gsl-lite*. See the table with [supported types and functions](#features).


Other GSL implementations
-------------------------
- Microsoft. [Guideline Support Library (GSL)](https://github.com/microsoft/gsl).
- Vicente J. Botet Escriba. [Guideline Support Library (GSL)](https://github.com/viboes/GSL).
- Mattia Basaglia. CxxMiscLib [gsl.hpp](https://github.com/mbasaglia/Cxx-MiscLib/blob/master/include/misclib/gsl.hpp), [tests](https://github.com/mbasaglia/Cxx-MiscLib/blob/master/test/gsl.cpp).


Notes and references
--------------------
### References
[1] [Standard C++ Foundation](https://isocpp.org/).  
[2] Standard C++ Foundation. [C++ Core Guidelines](https://github.com/isocpp/CppCoreGuidelines).  
[3] Microsoft. [Guideline Support Library (GSL)](https://github.com/microsoft/gsl).  
[4] Bjarne Stroustrup. [Writing good C++14 (PDF)](https://github.com/isocpp/CppCoreGuidelines/raw/master/talks/Stroustrup%20-%20CppCon%202015%20keynote.pdf) &mdash; [Video](https://www.youtube.com/watch?t=9&v=1OEu9C51K2A). CppCon 2015.  
[5] Herb Sutter. [Writing good C++14&hellip; By default (PDF)](https://github.com/isocpp/CppCoreGuidelines/raw/master/talks/Sutter%20-%20CppCon%202015%20day%202%20plenary%20.pdf) &mdash; [Video](https://www.youtube.com/watch?v=hEx5DNLWGgA). CppCon 2015.  
[6] Gabriel Dos Reis. [Contracts for Dependable C++ (PDF)](https://github.com/isocpp/CppCoreGuidelines/raw/master/talks/Contracts-for-Dependable-C%2B%2B.pdf)  &mdash; Video. CppCon 2015.  
[7] Bjarne Stroustrup et al. [A brief introduction to C++’s model for type- and resource-safety](https://github.com/isocpp/CppCoreGuidelines/raw/master/docs/Introduction%20to%20type%20and%20resource%20safety.pdf).  
[8] Herb Sutter and Neil MacIntosh. [Lifetime Safety: Preventing Leaks and Dangling](https://github.com/isocpp/CppCoreGuidelines/raw/master/docs/Lifetimes%20I%20and%20II%20-%20v0.9.1.pdf). 21 Sep 2015.

### Compiler feature testing
[9] cppreference.com. [Feature Test Recommendations](http://en.cppreference.com/w/cpp/experimental/feature_test).  
[10] cppreference.com. [Feature testing macros](http://en.cppreference.com/w/User:D41D8CD98F/feature_testing_macros).  

### C++ features in various Visual C++ compilers
[11] Visual CPP Team. [C++0x Core Language Features In VC10: The Table](http://blogs.msdn.com/b/vcblog/archive/2010/04/06/c-0x-core-language-features-in-vc10-the-table.aspx). Microsoft. 6 April 2010.  
[12] Visual CPP Team. [C++11 Features in Visual C++ 11](http://blogs.msdn.com/b/vcblog/archive/2011/09/12/10209291.aspx). Microsoft. 12 September 2011.  
[13] Joel Coehoorn. [C++11 features in Visual Studio 2012](http://stackoverflow.com/a/7422058/437272). StackOverflow. 14 September 2011.  
[14] Stephan T. Lavavej. [C++11/14 Features In Visual Studio 14 CTP3](http://blogs.msdn.com/b/vcblog/archive/2014/08/21/c-11-14-features-in-visual-studio-14-ctp3.aspx). Microsoft. 21 August 2014.  
[15] Stephan T. Lavavej. [C++11/14/17 Features In VS 2015 RTM](http://blogs.msdn.com/b/vcblog/archive/2015/06/19/c-11-14-17-features-in-vs-2015-rtm.aspx). Microsoft. 19 June 2015.  

Appendix
--------
### A.1 Extract Boost smart pointers

To obtain a subset of Boost only containing the smart pointers, use the [bcp command](http://www.boost.org/doc/libs/1_59_0/tools/bcp/doc/html/index.html) like:

    C:\Libraries\boost\boost_1_51>bin\bcp scoped_ptr.hpp shared_ptr.hpp weak_ptr.hpp make_shared.hpp C:\Libraries\boost-shared_ptr

The smart pointers of Boost 1.51 can be used with VC6.

### A.2 GSL Lite test specification

```
Expects(): Allows a true expression
Ensures(): Allows a true expression
Expects(): Terminates on a false expression
Ensures(): Terminates on a false expression
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
byte: Allows to construct from integral via static cast (C++11)
byte: Allows to construct from integral via byte() (C++11)
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
not_null<>: Disallows default construction (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)
not_null<>: Disallows construction from nullptr_t, NULL or 0 (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)
not_null<>: Disallows construction from a unique pointer to underlying type (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)
not_null<>: Disallows assignment from unrelated pointers (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)
not_null<>: Terminates construction from a null pointer value
not_null<>: Terminates construction from related pointer types for null pointer value
not_null<>: Terminates assignment from a null pointer value
not_null<>: Terminates assignment from related pointer types for null pointer value
not_null<>: Allows to construct from a non-null underlying pointer
not_null<>: Allows to construct from a non-null user-defined ref-counted type
not_null<>: Allows to construct from a non-null related pointer
not_null<>: Allows to construct from a not_null related pointer type
not_null<>: Allows assignment from a not_null related pointer type
not_null<>: Allows assignment from a non-null bare recast pointer
not_null<>: Allows implicit conversion to underlying type
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
span<>: Allows to default-construct
span<>: Allows to construct from a nullptr and a zero size (C++11)
span<>: Allows to construct from a l-value (C++11)
span<>: Allows to construct from a const l-value (C++11)
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
span<>: Allows to construct from a std::array<> (C++11)
span<>: Allows to construct from a std::array<> with const data (C++11)
span<>: Allows to construct from a container (std::vector<>)
span<>: Allows to tag-construct from a container (std::vector<>)
span<>: Allows to construct from an empty gsl::shared_ptr (C++11)
span<>: Allows to construct from an empty gsl::unique_ptr (C++11)
span<>: Allows to construct from an empty gsl::unique_ptr (array, C++11)
span<>: Allows to construct from a non-empty gsl::shared_ptr (C++11)
span<>: Allows to construct from a non-empty gsl::unique_ptr (C++11)
span<>: Allows to construct from a non-empty gsl::unique_ptr (array, C++11)
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
span<>: Allows to observe an element via call indexing
span<>: Allows to observe an element via at()
span<>: Allows to observe an element via data()
span<>: Allows to change an element via array indexing
span<>: Allows to change an element via call indexing
span<>: Allows to change an element via at()
span<>: Allows to change an element via data()
span<>: Allows to compare equal to another span of the same type
span<>: Allows to compare unequal to another span of the same type
span<>: Allows to compare less than another span of the same type
span<>: Allows to compare less than or equal to another span of the same type
span<>: Allows to compare greater than another span of the same type
span<>: Allows to compare greater than or equal to another span of the same type
span<>: Allows to compare to another span of the same type and different cv-ness (non-standard)
span<>: Allows to compare empty spans as equal
span<>: Allows to test for empty span via empty(), empty case
span<>: Allows to test for empty span via empty(), non-empty case
span<>: Allows to obtain the number of elements via size()
span<>: Allows to obtain the number of elements via length()
span<>: Allows to obtain the number of bytes via size_bytes()
span<>: Allows to obtain the number of bytes via length_bytes()
span<>: Allows to swap with another span of the same type
span<>: Allows to view the elements as read-only bytes
span<>: Allows to view and change the elements as writable bytes
span<>: Allows to view the elements as a span of another type
span<>: Allows to change the elements from a span of another type
span<>: Allows to copy a span to another span of the same element type
span<>: Allows to copy a span to another span of a different element type
make_span(): Allows building from two pointers
make_span(): Allows building from two const pointers
make_span(): Allows building from a non-null pointer and a size
make_span(): Allows building from a non-null const pointer and a size
make_span(): Allows building from a C-array
make_span(): Allows building from a const C-array
make_span(): Allows building from a std::array<> (C++11)
make_span(): Allows building from a const std::array<> (C++11)
make_span(): Allows building from a container (std::vector<>)
make_span(): Allows building from a const container (std::vector<>)
make_span(): Allows building from an empty gsl::shared_ptr (C++11)
make_span(): Allows building from an empty gsl::unique_ptr (C++11)
make_span(): Allows building from an empty gsl::unique_ptr (array, C++11)
make_span(): Allows building from a non-empty gsl::shared_ptr (C++11)
make_span(): Allows building from a non-empty gsl::unique_ptr (C++11)
make_span(): Allows building from a non-empty gsl::unique_ptr (array, C++11)
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
string_span: Allows to observe an element via call indexing
string_span: Allows to observe an element via data()
string_span: Allows to change an element via array indexing
string_span: Allows to change an element via call indexing
string_span: Allows to change an element via data()
string_span: Allows to compare a string_span with another string_span
string_span: Allows to compare empty spans as equal
string_span: Allows to compare a string_span with a cstring_span
string_span: Allows to compare with types convertible to string_span
string_span: Allows to test for empty span via empty(), empty case
string_span: Allows to test for empty span via empty(), non-empty case
string_span: Allows to obtain the number of elements via length()
string_span: Allows to obtain the number of elements via size()
string_span: Allows to obtain the number of bytes via length_bytes()
string_span: Allows to obtain the number of bytes via size_bytes()
string_span: Allows to view the elements as read-only bytes
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
finally: Allows to run lambda on leaving scope
finally: Allows to run function (bind) on leaving scope
finally: Allows to run function (pointer) on leaving scope
finally: Allows to move final_act
on_return: Allows to perform action on leaving scope without exception (gsl_FEATURE_EXPERIMENTAL_RETURN_GUARD)
on_error: Allows to perform action on leaving scope via an exception (gsl_FEATURE_EXPERIMENTAL_RETURN_GUARD)
narrow_cast<>: Allows narrowing without value loss
narrow_cast<>: Allows narrowing with value loss
narrow<>(): Allows narrowing without value loss
narrow<>(): Terminates when narrowing with value loss
narrow<>(): Terminates when narrowing with sign loss
```
