#include "function_traits.hpp"

static_assert( ltl::reference_v<int>() == ltl::ref_qual_v{} );
static_assert( ltl::reference_v<int&>() == ltl::lval_ref_v );
static_assert( ltl::reference_v<int&&>() == ltl::rval_ref_v );

static_assert( ltl::reference_v<int()>() == ltl::ref_qual_v{} );
static_assert( ltl::reference_v<int()&>() == ltl::lval_ref_v );
static_assert( ltl::reference_v<int()&&>() == ltl::rval_ref_v );

using fv = void();
using FV = ltl::function<fv>;

static_assert(not FV::is_const());
static_assert(not FV::is_volatile());
static_assert(not FV::is_cv());
static_assert(not FV::is_lvalue_reference());
static_assert(not FV::is_rvalue_reference());
static_assert(not FV::is_reference());
static_assert(not FV::is_cvref());
static_assert(not FV::is_variadic());

static_assert( ltl::function<void() noexcept>::is_noexcept());
static_assert( ltl::function<void(...) noexcept>::is_noexcept());

static_assert( ltl::function_is_const_v<void() const &>);
static_assert(!ltl::function_is_const_v<void() volatile>);

int main()
{
    return 0;
}
