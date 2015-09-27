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
prompt>g++ -std=c++03 -Wall -I../include/ -o 01-basic.exe 01-basic.cpp && 01-basic.exe
```

In a nutshell
-------------
**gsl-lite** is a variant of Microsoft's implementation of the [Guidelines Support Library (GSL)](https://github.com/Microsoft/GSL) adapted for C++98, C++03. It should also work when compiled as C++11, C++14. 

The Guidelines Support Library (GSL) contains functions and types that are suggested for use by the [C++ Core Guidelines](https://github.com/isocpp/CppCoreGuidelines) maintained by the [Standard C++ Foundation](https://isocpp.org/). 

The library includes types like `owner<>`, `not_null<>` and others.


License
-------
*gsl-lite* uses the [MIT](LICENSE) license.
 

Dependencies
------------
*gsl-lite* has no other dependencies than the [C++ standard library](http://en.cppreference.com/w/cpp/header).


Installation
------------
*gsl-lite* is a multiple-file header-only library. Put `gsl-lite.h` and the supporting files in the [include](include) folder directly into the project source tree or somewhere reachable from your project.


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
- [Feature selection macros](#feature-selection-macros)

### Feature selection macros

\-D<b>gsl\_FEATURE\_HAVE\_IMPLICIT\_MACRO</b>=1  
Define this macro to 0 to omit the `implicit` macro. Default is 1.

\-D<b>gsl\_FEATURE\_HAVE\_OWNER\_MACRO</b>=1  
At default macro `Owner()` is defined for all C++ versions. This may be useful to transition  from a compiler that doesn't provide alias templates to one that does. Define this macro to 0 to omit the `Owner()` macro. Default is 1.

\-D<b>gsl\_FEATURE_THROW\_ON\_FAILURE</b>=0  
Define this macro to 1 to throw a std::runtime_exception-derived exception `Guide::fail_fast` instead of calling `std::terminate()` on a GSL assertion. Default is 0.

Features
--------
See also section [GSL: Guideline support library](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#gsl-guideline-support-library) of the C++ Core Guidelines [2]. 

Feature / library           | GSL     | M-GSL   | GSL-Lite| Notes |
----------------------------|:-------:|:-------:|:-------:|:------|
**1.Lifetime&nbsp;safety**  | &nbsp;  | &nbsp;  | &nbsp;  | &nbsp; |
**1.1 Ownership**           | &nbsp;  | &nbsp;  | &nbsp;  | &nbsp; |
owner<>                     | &#10003;| &#10003;| &#10003;| Owned raw pointers |
Owner()                     | -       | -       | &#10003;| Macro for pre-C++11;<br>see also [Feature selection macros](#feature-selection-macros) |
unique_ptr<>                | &#10003;| &#10003;| >= C++11| std::unique_ptr<> |
shared_ptr<>                | &#10003;| &#10003;| >= C++11| std::shared_ptr<> |
stack_array<>               | &#10003;| -       | -       | A stack-allocated array, fixed size |
dyn_array<>                 | ?       | -       | -       | A heap-allocated array, fixed size |
**2.Bounds&nbsp;safety**    | &nbsp;  | &nbsp;  | &nbsp;  | &nbsp; |
**2.1 Views**               | &nbsp;  | &nbsp;  | &nbsp;  | &nbsp; |
not_null<>                  | &#10003;| &#10003;| &#10003;| Wrap any indirection and enforce non-null |
maybe_null<>                | -       | &#10003;| -       | &nbsp; |
array_view<>                | &#10003;| &#10003;| -       | A view of contiguous T's, replace (*,len) |
string_view<>               | &#10003;| &#10003;| -       | array_view&lt;char> |
cstring_view<>              | &#10003;| &#10003;| -       | array_view&lt;const char> |
zstring                     | &#10003;| &#10003;| &#10003;| a char* (C-style string) |
wzstring                    | -       | &#10003;| &#10003;| a wchar_t* (C-style string) |
czstring                    | &#10003;| &#10003;| &#10003;| a const char* (C-style string) |
cwzstring                   | -       | &#10003;| &#10003;| a const wchar_t* (C-style string) |
**2.2 Indexing**            | &nbsp;  | &nbsp;  | &nbsp;  | &nbsp; |
at()                        | &#10003;| &#10003;| some    | Bounds-checked way of accessing static arrays, std::array, std::vector |
**3. Assertions**           | &nbsp;  | &nbsp;  | &nbsp;  | &nbsp; |
Expects()                   | &#10003;| &#10003;| &#10003;| Precondition assertion |
Ensures()                   | &#10003;| &#10003;| &#10003;| Postcondition assertion |
**4. Utilities**            | &nbsp;  | &nbsp;  | &nbsp;  | &nbsp; |
Final_act<>                 | &#10003;| &#10003;| >= C++11| Action at the end of a scope |
finally()                   | &#10003;| &#10003;| >= C++11| Make a Final_act<> |
Final_act                   | -       | -       | <  C++11| Currently only void(*)() |
finally()                   | -       | -       | <  C++11| Make a Final_act |
narrow_cast<>               | &#10003;| &#10003;| &#10003;| Searchable narrowing casts of values |
narrow()                    | &#10003;| &#10003;| &#10003;| Checked version of narrow_cast() |
implicit                    | &#10003;| -       | &#10003;| Symmetric with explicit |
move_owner                  | ?       | -       | -       | ... |
**5. Concepts**             | &nbsp;  | &nbsp;  | &nbsp;  | &nbsp; |
...                         | &nbsp;  | &nbsp;  | &nbsp;  | &nbsp; |

Note: GSL Lite treats VC12 (VS2013) and VC13 (VS2015) as C++11 (gsl_CPP11_OR_GREATER is 1).


Reported to work with
---------------------
The table below mentions the compiler versions *gsl-lite* is reported to work with.

OS        | Compiler   | Versions |
---------:|:-----------|:---------|
Windows   | Clang/LLVM | ? |
&nbsp;    | GCC        | 4.9.2 |
&nbsp;    | Visual C++ (Visual Studio)| 6 (6), 10 (2010), 11 (2012), 12 (2013), 13 (2015) |
GNU/Linux | Clang/LLVM | 3.4 |
&nbsp;    | GCC        | 5.1 |
&nbsp;    | Clang/LLVM | 3.4 |
OS X      | ?          | ?   |


Other GSL implementations
-------------------------
- Microsoft. [Guidelines Support Library (GSL)](https://github.com/microsoft/gsl).
- Mattia Basaglia. CxxMiscLib [gsl.hpp](https://github.com/mbasaglia/Cxx-MiscLib/blob/master/include/misclib/gsl.hpp), [tests](https://github.com/mbasaglia/Cxx-MiscLib/blob/master/test/gsl.cpp).


Notes and references
--------------------
[1] [Standard C++ Foundation](https://isocpp.org/).  
[2] Standard C++ Foundation. [C++ Core Guidelines](https://github.com/isocpp/CppCoreGuidelines).  
[3] Microsoft. [Guidelines Support Library (GSL)](https://github.com/microsoft/gsl).  
[4] Bjarne Stroustrup. [Writing good C++14 (PDF)](https://github.com/isocpp/CppCoreGuidelines/raw/master/talks/Stroustrup%20-%20CppCon%202015%20keynote.pdf) &mdash; [Video](https://www.youtube.com/watch?t=9&v=1OEu9C51K2A). CppCon 2015.  
[5] Herb Sutter. [Writing good C++14&hellip; By default (PDF)](https://github.com/isocpp/CppCoreGuidelines/raw/master/talks/Sutter%20-%20CppCon%202015%20day%202%20plenary%20.pdf) &mdash; [Video](https://www.youtube.com/watch?v=hEx5DNLWGgA). CppCon 2015.  
[6] Gabriel Dos Reis. [Contracts for Dependable C++ (PDF)](https://github.com/isocpp/CppCoreGuidelines/raw/master/talks/Contracts-for-Dependable-C%2B%2B.pdf)  &mdash; Video. CppCon 2015.  
[7] Herb Sutter and Neil MacIntosh. [Lifetime Safety: Preventing Leaks and Dangling
I. Approach II. Informal overview and rationale](https://github.com/isocpp/CppCoreGuidelines/raw/master/docs/Lifetimes%20I%20and%20II%20-%20v0.9.1.pdf). Version 0.9.1, 2015-09-21.
