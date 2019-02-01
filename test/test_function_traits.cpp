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

// Bad definition gives compile error for non-function type Arg:
template <typename T>
inline constexpr bool bad_is_free_function_v
          = ltl::is_function_v<T> && !ltl::function_is_cvref_v<T>;

static_assert( bad_is_free_function_v<void()> );
static_assert( ! bad_is_free_function_v<void() const> );
// Compile error for non-function type Arg:
//static_assert( ! bad_is_free_function_v<void> );

template <typename T>
inline constexpr bool good_is_free_function_v
          = std::conjunction_v< ltl::is_function<T>,
               std::negation<ltl::is_function_cvref<T>>>;

static_assert( good_is_free_function_v<void()> );
static_assert( ! good_is_free_function_v<void() const> );
static_assert( ! good_is_free_function_v<void> );

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
static_assert(! typename F::is_reference_lvalue());
static_assert(! typename F::is_reference_rvalue());
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
SAME( typename F::set_cvref_noexcept_t<true,true,ltl::rval_ref_v,true>,
                                   void() const volatile && noexcept );
SAME( typename F::set_signature_t<int(bool)>, int(bool) );

// Test function traits for simple func F=void()
static_assert(! ltl::function_is_const<f>());
static_assert(! ltl::function_is_volatile<f>());
static_assert(! ltl::function_is_cv<f>());
static_assert(! ltl::function_is_reference_lvalue<f>());
static_assert(! ltl::function_is_reference_rvalue<f>());
static_assert(! ltl::function_is_reference<f>());
static_assert(! ltl::function_is_cvref<f>());
static_assert(! ltl::function_is_noexcept<f>());
static_assert(! ltl::function_is_variadic<f>());

static_assert(! ltl::function_is_const_v<f>);
static_assert(! ltl::function_is_volatile_v<f>);
static_assert(! ltl::function_is_cv_v<f>);
static_assert(! ltl::function_is_reference_lvalue_v<f>);
static_assert(! ltl::function_is_reference_rvalue_v<f>);
static_assert(! ltl::function_is_reference_v<f>);
static_assert(! ltl::function_is_cvref_v<f>);
static_assert(! ltl::function_is_noexcept_v<f>);
static_assert(! ltl::function_is_variadic_v<f>);

SAME( ltl::function_return_type_t<f>, void );
SAME( ltl::function_signature_t<f>, void() );
SAME( ltl::function_remove_cvref_t<f>, void() );
SAME( ltl::function_arg_types<f>, ltl::arg_types<> );

SAME( ltl::function_set_const_t<f,true>, fc );
SAME( ltl::function_set_volatile_t<f,true>, fv );
SAME( ltl::function_set_cv_t<f,true,true>, fcv );
SAME( ltl::function_set_reference_t<f,ltl::lval_ref_v>, fl );
SAME( ltl::function_set_reference_t<f,ltl::rval_ref_v>, fr );
SAME( ltl::function_set_cvref_t<f,true,false,ltl::lval_ref_v>, fcl );
SAME( ltl::function_set_cvref_t<f,true,false>, fc );
SAME( ltl::function_set_noexcept_t<f,true>, void() noexcept );
SAME( ltl::function_set_variadic_t<f,true>, void(...) );
SAME( ltl::function_set_return_type_t<f,int>, int() );
SAME( ltl::function_set_signature_t<f,int(bool)>, int(bool));

SAME( ltl::function_set_const<f,true>, Fc );
SAME( ltl::function_set_volatile<f,true>, Fv );
SAME( ltl::function_set_cv<f,true,true>, Fcv );
SAME( ltl::function_set_reference<f,ltl::lval_ref_v>, Fl );
SAME( ltl::function_set_reference<f,ltl::rval_ref_v>, Fr );
SAME( ltl::function_set_cvref<f,true,false,ltl::lval_ref_v>, Fcl );
SAME( ltl::function_set_cvref<f,true,false>, Fc );
SAME( ltl::function_set_noexcept<f,true>, ltl::function_traits<void() noexcept> );
SAME( ltl::function_set_variadic<f,true>, ltl::function_traits<void(...)> );
SAME( ltl::function_set_signature<f,int(bool)>, ltl::function_traits<int(bool)> );
}

namespace cmplx_func
{
// Test function_traits<F> member traits for more complex free func
struct R {};
using P = void const*;
using Q = R &&;

using f = R(P,Q,...);
using fc = R(P, Q, ...) const;
using fv = R(P, Q, ...) volatile;
using fcv = R(P, Q, ...) const volatile;
using fl = R(P, Q, ...) &;
using fr = R(P, Q, ...) &&;
using fnx = R(P, Q, ...) noexcept;
using fclnx = R(P, Q, ...) const & noexcept;

static_assert(std::conjunction<
	ltl::is_function<f>,
	ltl::is_function<fc>,
	ltl::is_function<fv>,
	ltl::is_function<fcv>,
	ltl::is_function<fl>,
	ltl::is_function<fr>,
	ltl::is_function<fnx>,
	ltl::is_function<fclnx>>()
	);

static_assert(std::conjunction<
	ltl::is_free_function<f>,
	ltl::is_free_function<fnx>>()
	);

static_assert( ! std::disjunction<
	ltl::is_free_function<fc>,
	ltl::is_free_function<fv>,
	ltl::is_free_function<fcv>,
	ltl::is_free_function<fl>,
	ltl::is_free_function<fr>,
	ltl::is_free_function<fclnx>>()
	);

using F =     ltl::function_traits<R(P, Q, ...)>;
using Fc =    ltl::function_traits<R(P, Q, ...) const>;
using Fv =    ltl::function_traits<R(P, Q, ...) volatile>;
using Fcv =   ltl::function_traits<R(P, Q, ...) const volatile>;
using Fl =    ltl::function_traits<R(P, Q, ...) &>;
using Fr =    ltl::function_traits<R(P, Q, ...) &&>;
using Fnx =   ltl::function_traits<R(P, Q, ...) noexcept>;
using Fclnx = ltl::function_traits<R(P, Q, ...) const & noexcept>;

static_assert(
	   !typename F::is_const()
    && !typename F::is_volatile()
    && !typename F::is_cv()
    && !typename F::is_reference_lvalue()
    && !typename F::is_reference_rvalue()
    && !typename F::is_reference()
    && !typename F::is_cvref()
    && !typename F::is_noexcept()
    &&  typename F::is_variadic()
	);
static_assert(
	    typename Fc::is_const()
	&& !typename Fc::is_volatile()
	&&  typename Fc::is_cv()
	&& !typename Fc::is_reference_lvalue()
	&& !typename Fc::is_reference_rvalue()
	&& !typename Fc::is_reference()
	&&  typename Fc::is_cvref()
	&& !typename Fc::is_noexcept()
	&&  typename Fc::is_variadic()
	);
static_assert(
	   !typename Fv::is_const()
	&&  typename Fv::is_volatile()
	&&  typename Fv::is_cv()
	&& !typename Fv::is_reference_lvalue()
	&& !typename Fv::is_reference_rvalue()
	&& !typename Fv::is_reference()
	&&  typename Fv::is_cvref()
	&& !typename Fv::is_noexcept()
	&&  typename Fv::is_variadic()
	);
static_assert(
	    typename Fcv::is_const()
	&&  typename Fcv::is_volatile()
	&&  typename Fcv::is_cv()
	&& !typename Fcv::is_reference_lvalue()
	&& !typename Fcv::is_reference_rvalue()
	&& !typename Fcv::is_reference()
	&&  typename Fcv::is_cvref()
	&& !typename Fcv::is_noexcept()
	&&  typename Fcv::is_variadic()
	);
static_assert(
	   !typename Fl::is_const()
	&& !typename Fl::is_volatile()
	&& !typename Fl::is_cv()
	&&  typename Fl::is_reference_lvalue()
	&& !typename Fl::is_reference_rvalue()
	&&  typename Fl::is_reference()
	&&  typename Fl::is_cvref()
	&& !typename Fl::is_noexcept()
	&&  typename Fl::is_variadic()
	);
static_assert(
	   !typename Fr::is_const()
	&& !typename Fr::is_volatile()
	&& !typename Fr::is_cv()
	&& !typename Fr::is_reference_lvalue()
	&&  typename Fr::is_reference_rvalue()
	&&  typename Fr::is_reference()
	&&  typename Fr::is_cvref()
	&& !typename Fr::is_noexcept()
	&&  typename Fr::is_variadic()
	);
static_assert(
	    typename Fclnx::is_const()
	&& !typename Fclnx::is_volatile()
	&&  typename Fclnx::is_cv()
	&&  typename Fclnx::is_reference_lvalue()
	&& !typename Fclnx::is_reference_rvalue()
	&&  typename Fclnx::is_reference()
	&&  typename Fclnx::is_cvref()
	&&  typename Fclnx::is_noexcept()
	&&  typename Fclnx::is_variadic()
	);


static_assert(
	   !ltl::function_is_const<f>()
	&& !ltl::function_is_volatile<f>()
	&& !ltl::function_is_cv<f>()
	&& !ltl::function_is_reference_lvalue<f>()
	&& !ltl::function_is_reference_rvalue<f>()
	&& !ltl::function_is_reference<f>()
	&& !ltl::function_is_cvref<f>()
	&& !ltl::function_is_noexcept<f>()
	&&  ltl::function_is_variadic<f>()
	);
static_assert(
	    ltl::function_is_const<fc>()
	&& !ltl::function_is_volatile<fc>()
	&&  ltl::function_is_cv<fc>()
	&& !ltl::function_is_reference_lvalue<fc>()
	&& !ltl::function_is_reference_rvalue<fc>()
	&& !ltl::function_is_reference<fc>()
	&&  ltl::function_is_cvref<fc>()
	&& !ltl::function_is_noexcept<fc>()
	&&  ltl::function_is_variadic<fc>()
	);
static_assert(
   	   !ltl::function_is_const<fv>()
	&&  ltl::function_is_volatile<fv>()
	&&  ltl::function_is_cv<fv>()
	&& !ltl::function_is_reference_lvalue<fv>()
	&& !ltl::function_is_reference_rvalue<fv>()
	&& !ltl::function_is_reference<fv>()
	&&  ltl::function_is_cvref<fv>()
	&& !ltl::function_is_noexcept<fv>()
	&&  ltl::function_is_variadic<fv>()
	);
static_assert(
	    ltl::function_is_const<fcv>()
	&&  ltl::function_is_volatile<fcv>()
	&&  ltl::function_is_cv<fcv>()
	&& !ltl::function_is_reference_lvalue<fcv>()
	&& !ltl::function_is_reference_rvalue<fcv>()
	&& !ltl::function_is_reference<fcv>()
	&&  ltl::function_is_cvref<fcv>()
	&& !ltl::function_is_noexcept<fcv>()
	&&  ltl::function_is_variadic<fcv>()
	);
static_assert(
	   !ltl::function_is_const<fl>()
	&& !ltl::function_is_volatile<fl>()
	&& !ltl::function_is_cv<fl>()
	&&  ltl::function_is_reference_lvalue<fl>()
	&& !ltl::function_is_reference_rvalue<fl>()
	&&  ltl::function_is_reference<fl>()
	&&  ltl::function_is_cvref<fl>()
	&& !ltl::function_is_noexcept<fl>()
	&&  ltl::function_is_variadic<fl>()
	);
static_assert(
	   !ltl::function_is_const<fr>()
	&& !ltl::function_is_volatile<fr>()
	&& !ltl::function_is_cv<fr>()
	&& !ltl::function_is_reference_lvalue<fr>()
	&&  ltl::function_is_reference_rvalue<fr>()
	&&  ltl::function_is_reference<fr>()
	&&  ltl::function_is_cvref<fr>()
	&& !ltl::function_is_noexcept<fr>()
	&&  ltl::function_is_variadic<fr>()
	);
static_assert(
	    ltl::function_is_const<fclnx>()
	&& !ltl::function_is_volatile<fclnx>()
	&&  ltl::function_is_cv<fclnx>()
	&&  ltl::function_is_reference_lvalue<fclnx>()
	&& !ltl::function_is_reference_rvalue<fclnx>()
	&&  ltl::function_is_reference<fclnx>()
	&&  ltl::function_is_cvref<fclnx>()
	&&  ltl::function_is_noexcept<fclnx>()
	&&  ltl::function_is_variadic<fclnx>()
	);

SAME(typename F::type, f);
SAME(typename F::return_type_t, R);
SAME(typename F::signature_t, f);
SAME(typename F::remove_cvref_t, f);
SAME(typename F::arg_types<>, ltl::arg_types<P,Q>);

SAME(typename F::set_const_t<true>, fc);
SAME(typename F::set_volatile_t<true>, fv);
SAME(typename F::set_cv_t<true, true>, fcv);
SAME(typename F::set_reference_t<ltl::lval_ref_v>, fl);
SAME(typename F::set_reference_t<ltl::rval_ref_v>, fr);
SAME(typename F::set_cvref_t<true, false, ltl::lval_ref_v>, R(P,Q,...)const&);
SAME(typename F::set_cvref_t<true, false>, fc);
SAME(typename F::set_noexcept_t<true>, fnx);
SAME(typename F::set_variadic_t<false>, R(P,Q));
SAME(typename F::set_return_type_t<int>, int(P,Q,...));

SAME(typename F::set_cvref_noexcept_t<true, true, ltl::rval_ref_v, true>,
	R(P, Q, ...) const volatile && noexcept);
SAME(typename Fclnx::set_signature_t<int(bool)>, int(bool) const & noexcept);

SAME(typename F::set_const<true>, Fc);
SAME(typename F::set_volatile<true>, Fv);
SAME(typename F::set_cv<true, true>, Fcv);
SAME(typename F::set_reference<ltl::lval_ref_v>, Fl);
SAME(typename F::set_reference<ltl::rval_ref_v>, Fr);
SAME(typename F::set_cvref<false, false, ltl::lval_ref_v>, Fl);
SAME(typename F::set_cvref<true, false>, Fc);
SAME(typename F::set_noexcept<true>, Fnx);

// Test function traits for simple func F=void()
SAME(ltl::function_return_type_t<f>, R);
SAME(ltl::function_signature_t<f>, f);
SAME(ltl::function_remove_cvref_t<f>, f);
SAME(ltl::function_arg_types<f>, ltl::arg_types<P,Q>);

SAME(ltl::function_set_const_t<f, true>, fc);
SAME(ltl::function_set_volatile_t<f, true>, fv);
SAME(ltl::function_set_cv_t<f, true, true>, fcv);
SAME(ltl::function_set_reference_t<f, ltl::lval_ref_v>, fl);
SAME(ltl::function_set_reference_t<f, ltl::rval_ref_v>, fr);
SAME(ltl::function_set_cvref_t<f, true, false, ltl::lval_ref_v>, ltl::function_remove_noexcept_t<fclnx>);
SAME(ltl::function_set_cvref_t<f, true, false>, fc);
SAME(ltl::function_set_noexcept_t<f, true>, fnx);
SAME(ltl::function_set_variadic_t<f, false>, R(P,Q));
SAME(ltl::function_set_return_type_t<f, int>, int(P,Q,...));
SAME(ltl::function_set_signature_t<fclnx, int(bool)>, int(bool) const & noexcept);

SAME(ltl::function_set_const<f, true>, Fc);
SAME(ltl::function_set_volatile<f, true>, Fv);
SAME(ltl::function_set_cv<f, true, true>, Fcv);
SAME(ltl::function_set_reference<f, ltl::lval_ref_v>, Fl);
SAME(ltl::function_set_reference<f, ltl::rval_ref_v>, Fr);
SAME(ltl::function_set_cvref<f, true, false, ltl::lval_ref_v>, ltl::function_remove_noexcept<fclnx>);
SAME(ltl::function_set_cvref<f, true, false>, Fc);
SAME(ltl::function_set_variadic<f, false>, ltl::function_traits<R(P, Q)>);
SAME(ltl::function_set_return_type<f, int>, ltl::function_traits<int(P, Q, ...)>);
SAME(ltl::function_set_signature<fclnx, int(bool)>, ltl::function_traits<int(bool) const & noexcept>);
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
