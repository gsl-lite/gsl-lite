# *gsl-lite* migration guide

**Contents:**  
- [Adapting your code](#adapting-your-code)
- [Using GSL compatibility mode](#using-gsl-compatibility-mode)


Starting with v1.0, *gsl-lite* now lives in the single header file `<gsl-lite/gsl-lite.hpp>`, and all its symbols reside in namespace
`gsl_lite`. By default, *gsl-lite* no longer defines a namespace `gsl` or the unprefixed `Expects()` and `Ensures()` macros for
precondition and postcondition checking.

This change enables coexistence with [Microsoft GSL](https://github.com/microsoft/GSL) or other GSL implementations. (https://github.com/gsl-lite/gsl-lite/issues/194)

## Adapting your code

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

## Using GSL compatibility mode

To minimize the impact of the breaking changes, *gsl-lite* introduces an optional *GSL compatibility mode* controlled by the
new configuration switch [`gsl_FEATURE_GSL_COMPATIBILITY_MODE`](doc/Features.md#feature-selection-macros), which is is disabled
by default and can be enabled by defining `gsl_FEATURE_GSL_COMPATIBILITY_MODE=1`.

If the GSL compatibility mode is enabled, *gsl-lite* additionally makes the following global definitions:
```c++
namespace gsl = ::gsl_lite;
#define Expects( x )  gsl_Expects( x )
#define Ensures( x )  gsl_Ensures( x )
```

The GSL compatibility mode precludes the use of *gsl-lite* and Microsoft GSL in the same translation unit. Therefore, do not use
the GSL compatibility mode when using *gsl-lite* in a public header file of a library. (Cf. the notes on
[using *gsl-lite* in libraries](#using-gsl-lite-in-libraries) below.)

The GSL compatibility mode causes no link-time interference between *gsl-lite* and as Microsoft GSL. Both libraries may be used in
the same project as long as no translation unit includes both at the same time.

The legacy header file `<gsl/gsl-lite.hpp>` now forwards to `<gsl-lite/gsl-lite.hpp>` and implicitly enables the GSL compatibility mode.
When the legacy header is included, it emits a warning message that urges to either migrate to header `<gsl-lite/gsl-lite.hpp>`,
namespace `gsl_lite` and the prefixed contract checking macros `gsl_Expects()` and `gsl_Ensures()`, or to explicitly request GSL
compatibility by defining `gsl_FEATURE_GSL_COMPATIBILITY_MODE=1`.
