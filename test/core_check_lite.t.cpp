
#include <tuple>        // for ignore
#include <vector>
#include <type_traits>
#include <string_view>

#include <gsl-lite/gsl-lite.hpp>


// Cf. https://docs.microsoft.com/en-us/cpp/code-quality/code-analysis-for-cpp-corecheck for a list of warnings in the MSVC
// C++ Core Guidelines Checker.


namespace {

std::string_view get_string_view() noexcept { return "hi"; }
gsl_lite::span<int const> get_span() noexcept { return { }; }
void get_span_ref() noexcept
{
    auto const & sv_ref = get_string_view(); // expect C26445
    std::ignore = sv_ref;
    auto const & span_ref = get_span(); // expect C26445
    std::ignore = span_ref;
}

std::vector<int> get_vector(gsl_lite::span<int const>) noexcept { return { }; }
void assign_temporary_span() noexcept
{
    auto span = get_span();
    span = get_vector(span); // expect C26449
}

void test_not_null_for_null()
{
    int i = 0;
    gsl_lite::not_null<int*> const pi(&i);
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
int dereferenceNotNull(gsl_lite::not_null<int const*> pi)
{
    return *pi;
}
int dereferenceNotNull2(gsl_lite::not_null<int const*> pi)
{
    return *pi;
}
int dereferenceNotNull3(int const* pi)
{
    Expects(pi != nullptr);
    return *pi;
}

int* newInt() noexcept { return nullptr; }
gsl_lite::owner<int*> newIntOwner() noexcept { return nullptr; }
void newDelete() noexcept
{
    int* pi = newInt();
    [[gsl::suppress(r.11)]] delete pi; // expect C26401
    gsl_lite::owner<int*> pi2 = newInt(); // expect C26406
    std::ignore = pi2;
    gsl_lite::owner<int*> pi3 = newIntOwner();
    [[gsl::suppress(r.11)]] delete pi3;
}

} // anonymous namespace
