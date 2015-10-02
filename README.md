# GSL Lite: Guidelines Support Library for C++98, C++03 and C++11 up &ensp; [![Build Status](https://travis-ci.org/martinmoene/gsl-lite.svg?branch=master)](https://travis-ci.org/martinmoene/gsl-lite) [![Build status](https://ci.appveyor.com/api/projects/status/1ha3wnxtam547m8p?svg=true)](https://ci.appveyor.com/project/martinmoene/gsl-lite)

GSL Lite is based on the [Microsoft Guidelines Support Library (GSL)](https://github.com/microsoft/gsl). 

**Contents**  
- [Example usage](#example-usage)
- [In a nutshell](#in-a-nutshell)
- [License](#license)
- [Dependencies](#dependencies)
- [Installation](#installation)
- [Building the tests](#building-the-tests)
- [Synopsis](#synopsis)
- [Features](#features)
- [Reported to work with](#reported-to-work-with)
- [Other GSL implementations](#other-gsl-implemetations)
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
**gsl-lite** is a variant of Microsoft's implementation of the [Guidelines Support Library (GSL)](https://github.com/Microsoft/GSL) adapted for C++98, C++03. It should also work when compiled as C++11, C++14. 

The Guidelines Support Library (GSL) contains functions and types that are suggested for use by the [C++ Core Guidelines](https://github.com/isocpp/CppCoreGuidelines) maintained by the [Standard C++ Foundation](https://isocpp.org/). 

The library includes types like `owner<>`, `not_null<>` and [others](#features).


License
-------
*gsl-lite* uses the [MIT](LICENSE) license.
 

Dependencies
------------
*gsl-lite* has no other dependencies than the [C++ standard library](http://en.cppreference.com/w/cpp/header).


Installation
------------
*gsl-lite* is a single-file header-only library. Put `gsl-lite.h` in the [include](include) folder directly into the project source tree or somewhere reachable from your project.


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


Synopsis
--------

**Contents**  
- [Configuration macros](#configuration-macros)
- [Feature selection macros](#feature-selection-macros)

### Configuration macros

\-D<b>gsl\_CONFIG\_CONFIRMS\_COMPILATION\_ERRORS</b>=0  
Define this macro to 1 to experience the by-design compile-time errors of the GSL components in the test suite. Default is 0.

\-D<b>gsl\_CONFIG\_THROWS\_FOR\_TESTING</b>=0  
Define this macro to 1 to throw a std::runtime_exception-derived exception `gsl::fail_fast` instead of calling `std::terminate()` on a GSL assertion. Use for testing only. Default is 0.

### Feature selection macros

\-D<b>gsl\_FEATURE\_HAVE\_IMPLICIT\_MACRO</b>=1  
Define this macro to 0 to omit the `implicit` macro. Default is 1.

\-D<b>gsl\_FEATURE\_HAVE\_OWNER\_MACRO</b>=1  
At default macro `Owner()` is defined for all C++ versions. This may be useful to transition  from a compiler that doesn't provide alias templates to one that does. Define this macro to 0 to omit the `Owner()` macro. Default is 1.


Features
--------
See also section [GSL: Guideline support library](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#gsl-guideline-support-library) of the C++ Core Guidelines [2]. 

Feature / library           | GSL     | M-GSL   | GSL-Lite| Notes |
----------------------------|:-------:|:-------:|:-------:|:------|
**1.Lifetime&nbsp;safety**  | &nbsp;  | &nbsp;  | &nbsp;  | &nbsp; |
**1.1 Indirection**         | &nbsp;  | &nbsp;  | &nbsp;  | &nbsp; |
not_null<>                  | &#10003;| &#10003;| &#10003;| Wrap any indirection and enforce non-null |
maybe_null<>                | -       | &#10003;| -       | &nbsp; |
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
**2.1 Views**               | &nbsp;  | &nbsp;  | &nbsp;  | &nbsp; |
array_view<>                | &#10003;| &#10003;| 1D views| A view of contiguous T's, replace (*,len) |
string_view<>               | &#10003;| &#10003;| &#10003;| array_view&lt;char> |
wstring_view<>              | -       | &#10003;| &#10003;| array_view&lt;wchar_t > |
cstring_view<>              | &#10003;| &#10003;| &#10003;| array_view&lt;const char> |
cwstring_view<>             | -       | &#10003;| &#10003;| array_view&lt;const wchar_t > |
zstring                     | &#10003;| &#10003;| &#10003;| a char* (C-style string) |
wzstring                    | -       | &#10003;| &#10003;| a wchar_t* (C-style string) |
czstring                    | &#10003;| &#10003;| &#10003;| a const char* (C-style string) |
cwzstring                   | -       | &#10003;| &#10003;| a const wchar_t* (C-style string) |
**2.2 Indexing**            | &nbsp;  | &nbsp;  | &nbsp;  | &nbsp; |
at()                        | &#10003;| &#10003;| >=C++11 | Bounds-checked way of accessing<br>static arrays, std::array, std::vector |
at()                        | -       | -       | < C++11 | static arrays, std::vector<br>std::array : VC11 |
**3. Assertions**           | &nbsp;  | &nbsp;  | &nbsp;  | &nbsp; |
Expects()                   | &#10003;| &#10003;| &#10003;| Precondition assertion |
Ensures()                   | &#10003;| &#10003;| &#10003;| Postcondition assertion |
**4. Utilities**            | &nbsp;  | &nbsp;  | &nbsp;  | &nbsp; |
Final_act<>                 | &#10003;| &#10003;| >=C++11 | Action at the end of a scope |
Final_act                   | -       | -       | < C++11 | Currently only void(*)() |
finally()                   | &#10003;| &#10003;| >=C++11 | Make a Final_act<> |
finally()                   | -       | -       | < C++11 | Make a Final_act |
narrow_cast<>               | &#10003;| &#10003;| &#10003;| Searchable narrowing casts of values |
narrow()                    | &#10003;| &#10003;| &#10003;| Checked version of narrow_cast() |
implicit                    | &#10003;| -       | &#10003;| Symmetric with explicit |
move_owner                  | ?       | -       | -       | ... |
**5. Concepts**             | &nbsp;  | &nbsp;  | &nbsp;  | &nbsp; |
...                         | &nbsp;  | &nbsp;  | &nbsp;  | &nbsp; |

Note: GSL Lite treats VC12 (VS2013) and VC13 (VS2015) as C++11 (gsl_CPP11_OR_GREATER: 1).


Reported to work with
---------------------
The table below mentions the compiler versions *gsl-lite* is reported to work with.

OS        | Compiler   | Versions |
---------:|:-----------|:---------|
Windows   | Clang/LLVM | ? |
&nbsp;    | GCC        | 4.9.2 |
&nbsp;    | Visual C++<br>(Visual Studio)| 6 (6), 8 (2005), 10 (2010), 11 (2012),<br>12 (2013), 13 (2015) |
GNU/Linux | Clang/LLVM | 3.4 |
&nbsp;    | GCC        | 5.1 |
OS X      | ?          | ?   |


Other GSL implementations
-------------------------
- Microsoft. [Guidelines Support Library (GSL)](https://github.com/microsoft/gsl).
- Mattia Basaglia. CxxMiscLib [gsl.hpp](https://github.com/mbasaglia/Cxx-MiscLib/blob/master/include/misclib/gsl.hpp), [tests](https://github.com/mbasaglia/Cxx-MiscLib/blob/master/test/gsl.cpp).


Notes and references
--------------------
### References
[1] [Standard C++ Foundation](https://isocpp.org/).  
[2] Standard C++ Foundation. [C++ Core Guidelines](https://github.com/isocpp/CppCoreGuidelines).  
[3] Microsoft. [Guidelines Support Library (GSL)](https://github.com/microsoft/gsl).  
[4] Bjarne Stroustrup. [Writing good C++14 (PDF)](https://github.com/isocpp/CppCoreGuidelines/raw/master/talks/Stroustrup%20-%20CppCon%202015%20keynote.pdf) &mdash; [Video](https://www.youtube.com/watch?t=9&v=1OEu9C51K2A). CppCon 2015.  
[5] Herb Sutter. [Writing good C++14&hellip; By default (PDF)](https://github.com/isocpp/CppCoreGuidelines/raw/master/talks/Sutter%20-%20CppCon%202015%20day%202%20plenary%20.pdf) &mdash; [Video](https://www.youtube.com/watch?v=hEx5DNLWGgA). CppCon 2015.  
[6] Gabriel Dos Reis. [Contracts for Dependable C++ (PDF)](https://github.com/isocpp/CppCoreGuidelines/raw/master/talks/Contracts-for-Dependable-C%2B%2B.pdf)  &mdash; Video. CppCon 2015.  
[7] Herb Sutter and Neil MacIntosh. [Lifetime Safety: Preventing Leaks and Dangling
I. Approach II. Informal overview and rationale](https://github.com/isocpp/CppCoreGuidelines/raw/master/docs/Lifetimes%20I%20and%20II%20-%20v0.9.1.pdf). Version 0.9.1, 2015-09-21.

### Compiler feature testing
[8] cppreference.com. [Feature Test Recommendations](http://en.cppreference.com/w/cpp/experimental/feature_test).  
[9] cppreference.com. [Feature testing macros](http://en.cppreference.com/w/User:D41D8CD98F/feature_testing_macros).  

### C++ features in various Visual C++ compilers
[10] Visual CPP Team. [C++0x Core Language Features In VC10: The Table](http://blogs.msdn.com/b/vcblog/archive/2010/04/06/c-0x-core-language-features-in-vc10-the-table.aspx). Microsoft. 6 April 2010.  
[11] Visual CPP Team. [C++11 Features in Visual C++ 11](http://blogs.msdn.com/b/vcblog/archive/2011/09/12/10209291.aspx). Microsoft. 12 September 2011.  
[12] Joel Coehoorn. [C++11 features in Visual Studio 2012](http://stackoverflow.com/a/7422058/437272). StackOverflow. 14 September 2011.  
[13] Stephan T. Lavavej. [C++11/14 Features In Visual Studio 14 CTP3](http://blogs.msdn.com/b/vcblog/archive/2014/08/21/c-11-14-features-in-visual-studio-14-ctp3.aspx). Microsoft. 21 August 2014.  
[14] Stephan T. Lavavej. [C++11/14/17 Features In VS 2015 RTM](http://blogs.msdn.com/b/vcblog/archive/2015/06/19/c-11-14-17-features-in-vs-2015-rtm.aspx). Microsoft. 19 June 2015.  

Appendix
--------
### A.1 Extract Boost smart pointers

To obtain a subset of Boost only containing the smart pointers, use the [bcp command](http://www.boost.org/doc/libs/1_59_0/tools/bcp/doc/html/index.html) like:

    C:\Libraries\boost\boost_1_51>bin\bcp scoped_ptr.hpp shared_ptr.hpp weak_ptr.hpp make_shared.hpp C:\Libraries\boost-shared_ptr

The smart pointers of Boost 1.51 can be used with VC6.

### A.2 GSL Lite test specification

```
array_view<>: Disallows ... (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)
array_view<>: Terminates construction from a nullptr and a non-zero size
array_view<>: Terminates construction from two pointers in the wrong order
array_view<>: Terminates construction from a null pointer and a non-zero size
array_view<>: Allows default construction
array_view<>: Allows construction from a nullptr and a zero size
array_view<>: Allows construction from two pointers
array_view<>: Allows construction from a non-null pointer and a size
array_view<>: Allows construction from a any pointer and a zero size
array_view<>: Allows construction from a C-array
array_view<>: Allows construction from a std::array<>
array_view<>: Allows construction from a container (std::vector<>)
array_view<>: Allows construction from another view of the same type
array_view<>: Allows assignment from another view of the same type
array_view<>: Allows forward iteration
array_view<>: Allows const forward iteration
array_view<>: Allows reverse iteration
array_view<>: Allows const reverse iteration
array_view<>: Allows conversion to bool (true if non-empty)
array_view<>: Allows element access via array indexing
array_view<>: Allows element access via at()
array_view<>: Allows element access via data()
array_view<>: Allows to compare equal to another view of the same type
array_view<>: Allows to compare unequal to another view of the same type
array_view<>: Allows to compare less than another view of the same type
array_view<>: Allows to compare less than or equal to another view of the same type
array_view<>: Allows to compare greater than another view of the same type
array_view<>: Allows to compare greater than or equal to another view of the same type
array_view<>: Allows to test for empty view via empty(), empty case
array_view<>: Allows to test for empty view via empty(), non-empty case
array_view<>: Allows to obtain number of elements via size()
array_view<>: Allows to obtain number of elements via length()
array_view<>: Allows to obtain number of elements via used_length()
array_view<>: Allows to obtain number of bytes via bytes()
array_view<>: Allows to obtain number of bytes via used_bytes()
array_view<>: Allows to swap with another view of the same type
array_view<>: Allows to view the elements as read-only bytes
array_view<>: Allows to view and change the elements as writable bytes
array_view<>: Allows to view the elements as a view of another type
array_view<>: Allows to change the elements from a view of another type
at(): Terminates access to non-existing C-array elements
at(): Terminates access to non-existing std::array elements
at(): Terminates access to non-existing std::vector elements
at(): Allows access to existing C-array elements
at(): Allows access to existing std::array elements
at(): Allows access to existing std::vector elements
not_null<>: Disallows default construction (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)
not_null<>: Disallows construction from nullptr_t, NULL or 0 (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)
not_null<>: Disallows construction from a unique pointer to underlying type (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)
not_null<>: Disallows assignment from unrelated pointers (define gsl_CONFIG_CONFIRMS_COMPILATION_ERRORS)
not_null<>: Terminates construction from a null pointer value
not_null<>: Terminates construction from related pointer types for null pointer value
not_null<>: Terminates assignment from a null pointer value
not_null<>: Terminates assignment from related pointer types for null pointer value
not_null<>: Allows construction from a non-null underlying pointer
not_null<>: Allows construction from a non-null user-defined ref-counted type
not_null<>: Allows construction from a non-null related pointer
not_null<>: Allows construction from a not_null related pointer type
not_null<>: Allows assignment from a not_null related pointer type
not_null<>: Allows assignment from a non-null bare recast pointer
not_null<>: Allows implicit conversion to underlying type
owner<>: Allows its use as the (pointer) type it stands for
Owner(): Allows its use as the (pointer) type it stands for
finally: Allows lambda to run
finally: Allows function with bind
finally: Allows pointer to function
narrow_cast<>: Allows narrowing without loss
narrow_cast<>: Allows narrowing with loss
narrow<>(): Allows narrowing without loss
narrow<>(): Terminates when narrowing with loss
```
