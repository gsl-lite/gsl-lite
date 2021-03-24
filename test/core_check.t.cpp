
#include <tuple>        // for ignore
#include <vector>
#include <type_traits>
#include <string_view>

#include <gsl/gsl-lite.hpp>


// Cf. https://docs.microsoft.com/en-us/cpp/code-quality/code-analysis-for-cpp-corecheck for a list of warnings in the MSVC
// C++ Core Guidelines Checker.


std::string_view get_string_view() noexcept { return "hi"; }
gsl::span<int const> get_span() noexcept { return { }; }
gsl_lite::span<int const> get_span2() noexcept { return { }; }
void get_span_ref() noexcept
{
    auto const & sv_ref = get_string_view(); // expect C26445
    std::ignore = sv_ref;
    auto const & span_ref = get_span(); // expect C26445
    std::ignore = span_ref;
    auto const & span2_ref = get_span2(); // expect C26445
    std::ignore = span2_ref;
}

std::vector<int> get_vector(gsl::span<int const>) noexcept { return { }; }
void assign_temporary_span() noexcept
{
    auto span = get_span();
    span = get_vector(span); // expect C26449
}

void test_not_null_for_null()
{
    int i = 0;
    gsl::not_null<int*> const pi(&i);
    std::ignore = pi;
    //std::ignore = pi == nullptr; // expect C26431 (does not compile for gsl-lite)
    gsl_lite::not_null<int*> const pi2(&i);
    std::ignore = pi2;
    //std::ignore = pi2 == nullptr; // expect C26431 (does not compile for gsl-lite)
}

int dereference(int const* pi) noexcept // expect C26429
{
    return *pi;
}
int dereferenceNotNull(gsl::not_null<int*> pi)
{
    return *pi;
}
int dereferenceNotNull2(gsl_lite::not_null<int*> pi)
{
    return *pi;
}

int* newInt() noexcept { return nullptr; }
gsl::owner<int*> newIntOwner() noexcept { return nullptr; }
gsl_lite::owner<int*> newIntOwner2() noexcept { return nullptr; }
void newDelete() noexcept
{
    int* pi = newInt();
    [[gsl::suppress(r.11)]] delete pi; // expect C26401
    gsl::owner<int*> pi2 = newInt(); // expect C26406
    std::ignore = pi2;
    gsl::owner<int*> pi3 = newIntOwner();
    [[gsl::suppress(r.11)]] delete pi3;
    gsl_lite::owner<int*> pi4 = newIntOwner2();
    [[gsl::suppress(r.11)]] delete pi4;
}

int main() noexcept
{
}
