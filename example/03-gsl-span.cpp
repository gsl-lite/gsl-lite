#include "gsl/gsl-lite.hpp"
#include "nonstd/span.hpp"

int main()
{
    const int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    { using gsl::span;    span<const int> spn( arr ); }
    { using nonstd::span; span<const int> spn( arr ); }
}

// cl -nologo -EHsc -Dspan_FEATURE_MAKE_SPAN -I../include -I../../span-lite/include 03-gsl-span.cpp && 03-gsl-span
// g++ -Wall        -Dspan_FEATURE_MAKE_SPAN -I../include -I../../span-lite/include -o 03-gsl-span 03-gsl-span.cpp && 03-gsl-span
