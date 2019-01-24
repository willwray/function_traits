#include "function_traits.hpp"

#define SAME(...) static_assert(std::is_same_v<__VA_ARGS__> );

// Test if type T has a member called 'value'
template <typename T, typename = void>
inline constexpr bool has_value = false;

template <typename T>
inline constexpr bool has_value<T,std::void_t<decltype(T::value)>> = true;

template <typename> struct wotype;

// Check that ref_qual_v addition does reference collapse correctly
static_assert( ltl::null_ref_v + ltl::null_ref_v == ltl::null_ref_v );
static_assert( ltl::null_ref_v + ltl::lval_ref_v == ltl::lval_ref_v );
static_assert( ltl::null_ref_v + ltl::rval_ref_v == ltl::rval_ref_v );

static_assert( ltl::lval_ref_v + ltl::null_ref_v == ltl::lval_ref_v );
static_assert( ltl::lval_ref_v + ltl::lval_ref_v == ltl::lval_ref_v );
static_assert( ltl::lval_ref_v + ltl::rval_ref_v == ltl::lval_ref_v ); // Coll

static_assert( ltl::rval_ref_v + ltl::null_ref_v == ltl::rval_ref_v );
static_assert( ltl::rval_ref_v + ltl::lval_ref_v == ltl::lval_ref_v ); // apse
static_assert( ltl::rval_ref_v + ltl::rval_ref_v == ltl::rval_ref_v );

// Check reference_v<T> function
static_assert( ltl::reference_v<int> == ltl::null_ref_v );
static_assert( ltl::reference_v<void> == ltl::null_ref_v );
static_assert( ltl::reference_v<int&> == ltl::lval_ref_v );
static_assert( ltl::reference_v<int&&> == ltl::rval_ref_v );

static_assert( ltl::reference_v<int(&)()noexcept> == ltl::lval_ref_v );
static_assert( ltl::reference_v<void(&&)()> == ltl::rval_ref_v );

static_assert( ltl::reference_v<int()> == ltl::null_ref_v );
static_assert( ltl::reference_v<void()> == ltl::null_ref_v );
static_assert( ltl::reference_v<int()&> == ltl::null_ref_v );
static_assert( ltl::reference_v<int()&&> ==ltl::null_ref_v );
static_assert( ltl::reference_v<void() volatile& noexcept>
                                         == ltl::null_ref_v );

// Check function_reference_v<T> function
static_assert( ltl::function_reference_v<int()> == ltl::null_ref_v );
static_assert( ltl::function_reference_v<void()> == ltl::null_ref_v );
static_assert( ltl::function_reference_v<int()&> == ltl::lval_ref_v );
static_assert( ltl::function_reference_v<int()&&> == ltl::rval_ref_v );
static_assert( ltl::function_reference_v<void() volatile& noexcept>
                                           == ltl::lval_ref_v );

// Test is_function trait
static_assert( ltl::is_function_v<int()>);
static_assert(!ltl::is_function_v<int>);

// Test is_free_function trait
static_assert( ! ltl::is_free_function_v<void> );
static_assert( ! ltl::is_free_function_v<int> );
static_assert( ! ltl::is_free_function_v<int() &> );
static_assert( ! ltl::is_free_function_v<int() const> );

static_assert( ltl::is_free_function_v<int()> );
static_assert( ltl::is_free_function_v<void() noexcept(true)> );

// The 'is_function_*' predicate traits are defined for all types so
// these 'lazy' traits gives no error for non-function type
using ifr = ltl::is_function_reference<int>;
// note -   ltl::function_is_reference<int> causes a compile error
static_assert( !has_value<ifr>,
  "is_function_trait<non function type> should not have a value member");

using ifrf = ltl::is_function_reference<int()&>;
static_assert( has_value<ifrf>,
  "is_function_trait<function type> should have a value member");

using firf = ltl::function_is_reference<int()&>;
static_assert( has_value<firf>,
  "function_is_trait<function type> should have a value member");

namespace auto_void
{
// Test function_traits<F> member traits for simple func F=void()
using f = void();
using fc = void() const;
using fv = void() volatile;
using fcv = void() const volatile;
using fl = void() &;
using fr = void() &&;
using fcl = void() const&;
using F = ltl::function_traits<f>;
using Fc = ltl::function_traits<fc>;
using Fv = ltl::function_traits<fv>;
using Fcv = ltl::function_traits<fcv>;
using Fl = ltl::function_traits<fl>;
using Fr = ltl::function_traits<fr>;
using Fcl = ltl::function_traits<fcl>;

static_assert(! typename F::is_const());
static_assert(! typename F::is_volatile());
static_assert(! typename F::is_cv());
static_assert(! typename F::is_lvalue_reference());
static_assert(! typename F::is_rvalue_reference());
static_assert(! typename F::is_reference());
static_assert(! typename F::is_cvref());
static_assert(! typename F::is_noexcept());
static_assert(! typename F::is_variadic());

SAME( typename F::type, void() );
SAME( typename F::return_type_t, void );
SAME( typename F::signature_t, void() );
SAME( typename F::remove_cvref_t, void() );
SAME( typename F::arg_types<>, ltl::arg_types<> );

SAME( typename F::set_const_t<true>, fc );
SAME( typename F::set_volatile_t<true>, fv );
SAME( typename F::set_cv_t<true,true>, fcv );
SAME( typename F::set_reference_t<ltl::lval_ref_v>, fl );
SAME( typename F::set_reference_t<ltl::rval_ref_v>, fr );
SAME( typename F::set_cvref_t<true,false,ltl::lval_ref_v>, fcl );
SAME( typename F::set_cvref_t<true,false>, fc );
SAME( typename F::set_noexcept_t<true>, void() noexcept );
SAME( typename F::set_variadic_t<true>, void(...) );
SAME( typename F::set_return_type_t<int>, int() );

SAME( typename F::set_const<true>, Fc );
SAME( typename F::set_volatile<true>, Fv );
SAME( typename F::set_cv<true,true>, Fcv );
SAME( typename F::set_reference<ltl::lval_ref_v>, Fl );
SAME( typename F::set_reference<ltl::rval_ref_v>, Fr );
SAME( typename F::set_cvref<true,false,ltl::lval_ref_v>, Fcl );
SAME( typename F::set_cvref<true,false>, Fc );
SAME( typename F::set_noexcept<true>, ltl::function_traits<void() noexcept> );
//SAME( typename F::set_variadic<true>, ltl::function_traits<void(...)> );

//SAME( typename F::set_noexcept<true>, void() noexcept );
//SAME( typename F::set_cvref_noexcept_t<true,true,ltl::rval_ref_v,true>,
//                                   void() const volatile && noexcept );
//SAME( typename F::set_signature_t<int(bool)>, int(bool) );
}

namespace cmplx_func
{
// Test function_traits<F> member traits for more complex free func
template <typename T> std::is_const<T> ttfn(T*,...) noexcept;
using fnx = decltype(ttfn<void const>);
using f = std::is_const<void const>(void const*,...);
using Fnx = ltl::function_traits<fnx>;
using F = ltl::function_traits<f>;

static_assert(! typename Fnx::is_const());
static_assert(! typename Fnx::is_volatile());
static_assert(! typename Fnx::is_cv());
static_assert(! typename Fnx::is_lvalue_reference());
static_assert(! typename Fnx::is_rvalue_reference());
static_assert(! typename Fnx::is_reference());
static_assert(! typename Fnx::is_cvref());
static_assert(  typename Fnx::is_noexcept());
static_assert(  typename Fnx::is_variadic());
SAME( typename Fnx::type, fnx );
SAME( typename Fnx::return_type_t, std::is_const<void const> );
SAME( typename Fnx::signature_t, f );
SAME( typename Fnx::remove_cvref_t, fnx);
SAME( typename Fnx::arg_types<>, ltl::arg_types<void const*> );
}


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
&& std::is_same_v< ltl::function_arg_types< FVFB >,
                   ltl::arg_types<char,bool(*)()> >
);

int main()
{
    return 0;
}
