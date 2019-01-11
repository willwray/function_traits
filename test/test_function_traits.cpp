#include "function_traits.hpp"

template <typename> struct wotype;

// Check that ref_qual_v addition does reference collapse
static_assert( ltl::null_ref_v + ltl::null_ref_v == ltl::null_ref_v );
static_assert( ltl::null_ref_v + ltl::lval_ref_v == ltl::lval_ref_v );
static_assert( ltl::null_ref_v + ltl::rval_ref_v == ltl::rval_ref_v );

static_assert( ltl::lval_ref_v + ltl::null_ref_v == ltl::lval_ref_v );
static_assert( ltl::lval_ref_v + ltl::lval_ref_v == ltl::lval_ref_v );
static_assert( ltl::lval_ref_v + ltl::rval_ref_v == ltl::lval_ref_v );

static_assert( ltl::rval_ref_v + ltl::null_ref_v == ltl::rval_ref_v );
static_assert( ltl::rval_ref_v + ltl::lval_ref_v == ltl::lval_ref_v );
static_assert( ltl::rval_ref_v + ltl::rval_ref_v == ltl::rval_ref_v );

// Check reference_v<T> function
static_assert( ltl::reference_v<int>() == ltl::null_ref_v );
static_assert( ltl::reference_v<void>() == ltl::null_ref_v );
static_assert( ltl::reference_v<int&>() == ltl::lval_ref_v );
static_assert( ltl::reference_v<int&&>() == ltl::rval_ref_v );

static_assert( ltl::reference_v<int()>() == ltl::null_ref_v );
static_assert( ltl::reference_v<void()>() == ltl::null_ref_v );
static_assert( ltl::reference_v<int()&>() == ltl::lval_ref_v );
static_assert( ltl::reference_v<int()&&>() == ltl::rval_ref_v );
static_assert( ltl::reference_v<void() volatile& noexcept>()
                                           == ltl::lval_ref_v );


// Test is_free_function trait
static_assert( ! ltl::is_free_function_v<void> );
static_assert( ! ltl::is_free_function_v<int> );
static_assert( ! ltl::is_free_function_v<int() &> );
static_assert( ! ltl::is_free_function_v<int() const> );

static_assert( ltl::is_free_function_v<int()> );
//static_assert( ltl::is_free_function_v<void(*)()> );


using fir = ltl::function_is_reference<int>;
stat
//static_assert( not ltl::function_is_reference_v<int> );

using fv = void();
using FV = ltl::function_traits<fv>;

static_assert(! typename FV::is_const());
static_assert(! typename FV::is_volatile());
static_assert(! typename FV::is_cv());
static_assert(! typename FV::is_lvalue_reference());
static_assert(! typename FV::is_rvalue_reference());
static_assert(! typename FV::is_reference());
static_assert(! typename FV::is_cvref());
static_assert(! typename FV::is_variadic());

static_assert( ltl::function_traits<void() noexcept>::is_noexcept());
static_assert( ltl::function_traits<void(...) noexcept>::is_noexcept());

static_assert( ltl::function_is_const_v<void() const &>);
static_assert(!ltl::function_is_const_v<void() volatile>);

static_assert(
   std::is_same_v< ltl::function_add_const_t<void() &>,
                    void() const&> );
static_assert(!ltl::function_is_const_v<void() volatile>);

using FVCX = void() const noexcept;

static_assert(
        ltl::function_is_const_v< FVCX >
     && ltl::function_is_cvref_v< FVCX >
     && ltl::function_is_noexcept_v< FVCX >
);

using FVFB = void(char,bool());

static_assert(
   std::is_void_v< ltl::function_return_type_t< FVFB > >
&& std::is_same_v< ltl::function_args_t< FVFB >,
                   ltl::function_parameter_types<char,bool(*)()> >
);

int main()
{
    return 0;
}
