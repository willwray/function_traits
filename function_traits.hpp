//    Copyright (c) 2018 Will Wray https://keybase.io/willwray
//
//   Distributed under the Boost Software License, Version 1.0.
//          (http://www.boost.org/LICENSE_1_0.txt)
//
//    Repo: https://github.com/willwray/function_traits

#include <type_traits>

/*
  "function_traits.hpp": function signature, cvref and noexcept traits
   ^^^^^^^^^^^^^^^^^^^
       Type trait: A compile-time template-based interface to
                     query or modify the properties of types.

   This header provides traits for C++ function types:

     - Function signature:  R(P...) or R(P..., ...)
         - Return type      R
         - Parameter types    P...
         - Presence of variadic parameter pack ...
           ...C-style trailing elipsis: 2 combinations

     - Function cvref qualifiers       12 combinations total:
         - cv:  const and/or volatile   4 combinations
         - ref: lvalue & or rvalue &&  x3 combinations

     - Function exception specification:
        - noexcept(bool): true|false    2 combinations

   Function 'signature' here refers to return type R and parameters P...
   (with optional C-style varargs but without qualifiers and noexcept):

     function_signature_t<F> // removes cvref qualifiers and noexcept

   This library follows std trait conventions:

     '_t' suffix for result type   (the trait is a type alias template)
     '_v' suffix for result value  (the trait is a variable template)
      no suffix for a result class (the trait is a class template
                                    with 'type' or 'value' member)

   Function types with cvref qualifiers are 'abominable' (see doc refs).
   To test if function type F is cvref qualified use predicate trait:

     function_is_cvref<F> // class inherited from std::bool_constant<?>
     function_is_cvref_v<F> // true if any c,v or ref qualifier present

   To test if a type T is a function type that is not cvref qualified:

     is_free_function_v<T> // is_function_v<T> && !function_is_cvref<T>

   This is the only function_trait that is defined for any type.  
   All other function_* traits are defined only for function types.

   Conventional 'add' and 'remove' traits modify their named trait:

     function_add_noexcept_t<F> // add noexcept specifier
     function_remove_cvref_t<F> // remove all cv and ref qualifiers
                                // leaving signature and exception spec

   Unconventionally, this library also provides transforming / mutating
   'set' traits that take extra template arguments, e.g.:

     function_set_noexcept_t<F,B>  // set noexcept(B) for bool const B
     function_set_signature_t<F,S> // set S as the function signature
                                   // keeping cvref-nx of function F

   Reference qualifiers are represented by an enum type ref_qual:

     - null_ref_v    no reference qualifier, same as ref_qual{}
     - lval_ref_v    lvalue reference qualifier: &
     - rval_ref_v    rvalue reference qualifier: &&

   Addition of ref_qual values gives the same reference collapsed
   result as compounding references on ordinary object types.
   Template function reference_v<T> returns the reference type of T:

     reference_v<T>() -> ref_qual  // defined for all types

   For function types is gives the function type reference qualifier.
   For other types it gives the ordinary reference qualifier.
   The 'set_reference' traits then allow to copy between function types:

     function_set_reference<F, reference_v<G>()> // copy G ref qual to F

     function_set_reference<F, lval_ref_v>   // set ref qual to &
     function_set_reference_lvalue<F>        // set ref qual to &

     function_set_reference<F, null_ref_v>   // set ref qual to none
     function_remove_reference<F>            // set ref qual to none

     function_add_reference<F, rval_ref_v>   // does reference collapse!
                                             // (so F& + && = F&)

   Copying all cvref qualifiers is verbose with 'set_cvref' so this is
   provided as a 'set_cvref_as' trait:

     function_set_cvref_as_t<F,G> // copy cvref quals of G to F
*/

// GCC and Clang deduce noexcept via partial specialization
// MSVC doesn't deduce yet (early 2019 V 15.9.4 Preview 1.0)
#if defined(__GNUC__)
#   define NOEXCEPT_DEDUCED
#endif

// Fallback macro switch for lack of noexcept deduction
#if defined(NOEXCEPT_DEDUCED)
#   define NOEXCEPT_ND(NON, ...) __VA_ARGS__
#else
#   define NOEXCEPT_ND(NON, ...) NON
#endif

// Test noexcept deduction - compile fail if deduction fails
#if defined(NOEXCEPT_DEDUCED)
namespace test {
constexpr void voidfn();
// GCC appears to need R introduced & deduced in order to deduce X
template <typename R, bool X>
constexpr auto noexcept_deduction(R() noexcept(X)) -> std::true_type;
constexpr auto noexcept_deduction(...) -> std::false_type;
static_assert(decltype(noexcept_deduction(voidfn))(),
              "NOEXCEPT_DEDUCED flag is set but deduction fails");
} // namespace test
#endif

namespace ltl
{
// function_traits<F>
// class template: a collection of member traits for function type F
//                 or an incomplete type for non-function type F
template <typename F> class function_traits;
// Note: the 24 (or 48) partial specializations for function_traits<F>
// are generated by macro-expansion.

// A default type-list type for returning function parameter types
template <typename...> struct function_parameter_types;

// ref_qual: a value to represent a reference qualifier
//   null_ref_v    no reference qualifier
//   rval_ref_v    rvalue reference qualifier: &&
//   lval_ref_v    lvalue reference qualifier: &
enum ref_qual { null_ref_v, rval_ref_v, lval_ref_v = 3 };

// ref_qual operator+( ref_qual, ref_qual)
// 'adds' reference qualifiers with reference collapse
constexpr ref_qual operator+( ref_qual a, ref_qual b)
{
  return static_cast<ref_qual>(a|b);
}

// reference_v<T>() returns a ref_qual value representing
//  the reference qualifier on type T (a general type or function type)
template <typename T>
constexpr ref_qual reference_v()
{
  if constexpr (std::is_function_v<T>) {
    using F = function_traits<T>;
    return typename F::is_lvalue_reference()
              ? lval_ref_v
              : typename F::is_rvalue_reference() ? rval_ref_v : null_ref_v;
    } else {
      return std::is_lvalue_reference_v<T>
              ? lval_ref_v
              : std::is_rvalue_reference_v<T> ? rval_ref_v : null_ref_v;
    }
}

namespace impl
{
// is_function implementation can be removed with C++20 concepts as it can
// be implemented directly as = requires { sizeof(function_traits<T>); };
template <typename T, typename = decltype(sizeof(int))>
inline constexpr bool is_function_v = false;

template <typename T>
inline constexpr bool is_function_v<T,decltype(sizeof(function_traits<T>))>
                                    = true;

// function_cvref_nx<setter, c, v, ref, nx>
// Convenience collection of type aliases for cvref & noexcept properties.
// Injected setter template set_cvref_nx is used for template set_* aliases.
template <template <bool, bool, ref_qual, bool> typename set_cvref_nx,
          bool c, bool v, ref_qual ref, bool nx>
struct function_cvref_nx
{
  using is_const = std::bool_constant<c>;
  using is_volatile = std::bool_constant<v>;
  using is_lvalue_reference = std::bool_constant<ref == lval_ref_v>;
  using is_rvalue_reference = std::bool_constant<ref == rval_ref_v>;
  using is_noexcept = std::bool_constant<nx>;

  using is_cv = std::bool_constant<c || v>;  // Note: const OR volatile
  using is_reference = std::bool_constant<ref != null_ref_v>;
  using is_cvref = std::bool_constant<c || v || ref != null_ref_v>;

  template <bool C> using set_const_t = set_cvref_nx<C, v, ref, nx>;
  template <bool V> using set_volatile_t = set_cvref_nx<c, V, ref, nx>;
  template <bool C, bool V> using set_cv_t = set_cvref_nx<C, V, ref, nx>;
  template <ref_qual R> using set_reference_t = set_cvref_nx<c, v, R, nx>;
  template <bool C, bool V, ref_qual R = null_ref_v>
  using set_cvref_t = set_cvref_nx<C, V, R, nx>;
  template <bool NX> using set_noexcept_t = set_cvref_nx<c, v, ref, NX>;

  template <bool C> using set_const = function_traits<set_const_t<C>>;
  template <bool V> using set_volatile = function_traits<set_volatile_t<V>>;
  template <bool C, bool V> using set_cv = function_traits<set_cv_t<C, V>>;
  template <ref_qual R>
  using set_reference = function_traits<set_reference_t<R>>;
  template <bool C, bool V, ref_qual R = null_ref_v>
  using set_cvref = function_traits<set_cvref_t<C, V, R>>;
  template <bool NX> using set_noexcept = function_traits<set_noexcept_t<NX>>;
};

// function_base<F>:
// Base class template for function_traits<F> holding F's 'signature'
//   R(P...)     - return type R, parameter types P..., or
//   R(P...,...) - with a trailing variadic parameter pack ...
// but not including any function cvref qualifiers or noexcept specifier.
template <typename F> class function_base;
// A macro definition is used to expand non-variadic and variadic signatures.
// Clang warns when a variadic signature omits the comma; R(P... ...), so
// __VA_ARGS__ = ,... includes the leading comma, present as needed
// (C++20's __VA_OPT__(,...) is another way to expand with leading comma).
// MSVC doesn't handle empty variadic macro, so add a BOGUS macro parameter.

// function_base<F> specialisations for non-variadic and variadic signatures
#define FUNCTION_BASE(BOGUS,...) \
template <typename R, typename... P>                                   \
class function_base<R(P...__VA_ARGS__)>                                \
{                                                                      \
 public:                                                               \
  using return_type_t = R;                                             \
  using signature_t = R(P...__VA_ARGS__);                              \
  using is_variadic = std::bool_constant<bool(#__VA_ARGS__[0])>;       \
  template <template <typename...> typename T=function_parameter_types>\
  using args_t = T<P...>; \
 private:\
  template <typename T> struct id { using type = T; };\
  template <bool c, bool v, ref_qual r, bool nx>\
  static constexpr auto setcvrnx()\
  {\
    if constexpr (r == null_ref_v) {\
      if constexpr(!v) {\
        if constexpr(!c)\
          return id<R(P...__VA_ARGS__) noexcept(nx)>{};\
        else\
          return id<R(P...__VA_ARGS__) const noexcept(nx)>{};\
      } else {\
        if constexpr(!c)\
          return id<R(P...__VA_ARGS__) volatile noexcept(nx)>{};\
        else\
          return id<R(P...__VA_ARGS__) const volatile noexcept(nx)>{};\
      }\
    } else if constexpr (r == lval_ref_v) {\
      if constexpr(!v) {\
        if constexpr(!c)\
          return id<R(P...__VA_ARGS__) & noexcept(nx)>{};\
        else\
          return id<R(P...__VA_ARGS__) const & noexcept(nx)>{};\
      } else {\
        if constexpr(!c)\
          return id<R(P...__VA_ARGS__) volatile & noexcept(nx)>{};\
        else\
          return id<R(P...__VA_ARGS__) const volatile & noexcept(nx)>{};\
      }\
    } else {\
      if constexpr(!v) {\
        if constexpr(!c)\
          return id<R(P...__VA_ARGS__) && noexcept(nx)>{};\
        else\
          return id<R(P...__VA_ARGS__) const && noexcept(nx)>{};\
      } else {\
        if constexpr(!c)\
          return id<R(P...__VA_ARGS__) volatile && noexcept(nx)>{};\
        else\
          return id<R(P...__VA_ARGS__) const volatile && noexcept(nx)>{};\
      }\
    }\
  }\
 public:                                                                     \
  template <bool c, bool v, ref_qual r, bool nx>                             \
  using set_cvref_noexcept_t = typename decltype(setcvrnx<c,v,r,nx>())::type;\
}
FUNCTION_BASE(,);
FUNCTION_BASE(,,...); // leading comma forwarded via macro varargs
#undef FUNCTION_BASE

} // namespace impl

// function_traits<F> specializations for 24 cvref varargs combinations
//                          or for 48 cvref varargs noexcept combinations
#define CV_REF(CV,REF,NX,...) \
template <typename R, typename... P NOEXCEPT_ND(,,bool X)>                   \
class function_traits<R(P...__VA_ARGS__) CV REF noexcept(NOEXCEPT_ND(NX,X))> \
    : public impl::function_base<R(P...__VA_ARGS__)>,                        \
      public impl::function_cvref_nx<                                        \
          impl::function_base<R(P...__VA_ARGS__)>::                          \
          template set_cvref_noexcept_t,                                     \
          std::is_const_v<int CV>, std::is_volatile_v<int CV>,               \
          reference_v<int REF>(), NOEXCEPT_ND(NX,X)>                         \
{                                                                            \
  enum : bool { nx = NOEXCEPT_ND(NX,X) };                                    \
  template <typename> struct set_signature;                                  \
  template <typename r, typename... p> struct set_signature<r(p...)> {       \
    using type = r(p...) CV REF noexcept(nx); };                             \
  template <typename r, typename... p> struct set_signature<r(p..., ...)> {  \
    using type = r(p..., ...) CV REF noexcept(nx); };                        \
public:                                                                      \
  using type = R(P...__VA_ARGS__) CV REF noexcept(nx);                       \
  using remove_cvref_t = R(P...__VA_ARGS__) noexcept(nx);                    \
  template <typename r> using set_return_type_t =                            \
      r(P...__VA_ARGS__) CV REF noexcept(nx);                                \
  template <bool V> using set_variadic_t = std::conditional_t<V,             \
      R(P..., ...) CV REF noexcept(nx), R(P...) CV REF noexcept(nx)>;        \
  template <typename B>                                                      \
  using set_signature_t = typename set_signature<B>::type;                   \
};

// CV_REF_QUALIFIERS(...)
// X-macro list to expand the 12 cv-ref combos, and
//   pass through X; optional true|false noexcept(bool) specification, and ...
//   ... varargs to pass through C/C++ varargs (inluding a leading comma)
#define CV_REF_QUALIFIERS(X, ...)           \
  CV_REF(, , X, __VA_ARGS__)                \
  CV_REF(, &, X, __VA_ARGS__)               \
  CV_REF(, &&, X, __VA_ARGS__)              \
  CV_REF(const, , X, __VA_ARGS__)           \
  CV_REF(const, &, X, __VA_ARGS__)          \
  CV_REF(const, &&, X, __VA_ARGS__)         \
  CV_REF(volatile, , X, __VA_ARGS__)        \
  CV_REF(volatile, &, X, __VA_ARGS__)       \
  CV_REF(volatile, &&, X, __VA_ARGS__)      \
  CV_REF(const volatile, , X, __VA_ARGS__)  \
  CV_REF(const volatile, &, X, __VA_ARGS__) \
  CV_REF(const volatile, &&, X, __VA_ARGS__)

// X-macro list to expand all 24 or 48 variadic,cv,ref,[noexcept] combos
#if defined(NOEXCEPT_DEDUCED)
  CV_REF_QUALIFIERS(,)
  CV_REF_QUALIFIERS(, ,...) // leading comma for variadic match
#else
  CV_REF_QUALIFIERS(true,)
  CV_REF_QUALIFIERS(true, ,...) // leading comma for variadic match
  CV_REF_QUALIFIERS(false,)
  CV_REF_QUALIFIERS(false, ,...) // leading comma for variadic match
#endif
#undef CV_REF
#undef CV_REF_QUALIFIERS
#undef NOEXCEPT_DEDUCED
#undef NOEXCEPT_ND


// Predicate traits for c,v,ref,noexcept,variadic properties

template <typename F> struct function_is_const
                : function_traits<F>::is_const {};
template <typename F> inline constexpr bool function_is_const_v =
                        typename function_traits<F>::is_const();

template <typename F> struct function_is_volatile
                : function_traits<F>::is_volatile {};
template <typename F> inline constexpr bool function_is_volatile_v =
                        typename function_traits<F>::is_volatile();

template <typename F> struct function_is_cv
                : function_traits<F>::is_cv {};
template <typename F> inline constexpr bool function_is_cv_v =
                        typename function_traits<F>::is_cv();

template <typename F> struct function_is_reference
                : function_traits<F>::is_reference {};
template <typename F> inline constexpr bool function_is_reference_v =
                        typename function_traits<F>::is_reference();

template <typename F> struct function_is_lvalue_reference
                : function_traits<F>::is_lvalue_reference {};
template <typename F> inline constexpr bool function_is_lvalue_reference_v =
                        typename function_traits<F>::is_lvalue_reference();

template <typename F> struct function_is_rvalue_reference
                : function_traits<F>::is_rvalue_reference {};
template <typename F> inline constexpr bool function_is_rvalue_reference_v =
                        typename function_traits<F>::is_rvalue_reference();

template <typename F> struct function_is_cvref
                : function_traits<F>::is_cvref {};
template <typename F> inline constexpr bool function_is_cvref_v =
                        typename function_traits<F>::is_cvref();

template <typename F> struct function_is_noexcept
                : function_traits<F>::is_noexcept {};
template <typename F> inline constexpr bool function_is_noexcept_v =
                        typename function_traits<F>::is_noexcept();

template <typename F> struct function_is_variadic
                : function_traits<F>::is_variadic {};
template <typename F> inline constexpr bool function_is_variadic_v =
                        typename function_traits<F>::is_variadic();

// ltl::is_function - same as std::is_function
// - using this definition saves redundant instantiation of std::function
template <typename T> struct is_function
                           : std::bool_constant<impl::is_function_v<T>> {};
template <typename T>
inline constexpr bool is_function_v = impl::is_function_v<T>;
                                 // = requires {sizeof(function_traits<T>);};

// is_free_function_v<F> : checks if type F is a free function type
//   true if F is a function type without cvref qualifiers
//   false if F is not a function type or is a cvref qualified function type
template <typename F>
inline constexpr bool is_free_function_v = []{
             if constexpr (is_function_v<F>)
               return !function_is_cvref_v<F>;
             return false; }();

template <typename F> struct is_free_function
        : std::bool_constant<is_free_function_v<F>> {};

// set_const, add_const / remove_const
template <typename F, bool C>
using function_set_const = typename function_traits<F>::template set_const<C>;
template <typename F, bool C>
using function_set_const_t =
    typename function_traits<F>::template set_const_t<C>;

template <typename F> using function_add_const = function_set_const<F, true>;
template <typename F>
using function_add_const_t = function_set_const_t<F, true>;

template <typename F>
using function_remove_const = function_set_const<F, false>;
template <typename F>
using function_remove_const_t = function_set_const_t<F, false>;

// set_volatile, add_volatile / remove_volatile
template <typename F, bool V>
using function_set_volatile =
    typename function_traits<F>::template set_volatile<V>;
template <typename F, bool V>
using function_set_volatile_t =
    typename function_traits<F>::template set_volatile_t<V>;

template <typename F>
using function_add_volatile = function_set_volatile<F, true>;
template <typename F>
using function_add_volatile_t = function_set_volatile_t<F, true>;

template <typename F>
using function_remove_volatile = function_set_volatile<F, false>;
template <typename F>
using function_remove_volatile_t = function_set_volatile_t<F, false>;

// set_cv, remove_cv (add_cv would add c AND v while is_cv tests c OR v)
template <typename F, bool C, bool V>
using function_set_cv = typename function_traits<F>::template set_cv<V>;
template <typename F, bool C, bool V>
using function_set_cv_t = typename function_traits<F>::template set_cv_t<V>;

template <typename F>
using function_remove_cv = function_set_cv<F, false, false>;
template <typename F>
using function_remove_cv_t = function_set_cv_t<F, false, false>;

// set_reference, set_reference_lvalue, set_reference_rvalue
template <typename F, ref_qual R>
using function_set_reference =
    typename function_traits<F>::template set_reference<R>;
template <typename F, ref_qual R>
using function_set_reference_t =
    typename function_traits<F>::template set_reference_t<R>;

template <typename F>
using function_set_reference_lvalue = function_set_reference<F, lval_ref_v>;
template <typename F>
using function_set_reference_lvalue_t = function_set_reference_t<F, lval_ref_v>;

template <typename F>
using function_set_reference_rvalue = function_set_reference<F, rval_ref_v>;
template <typename F>
using function_set_reference_rvalue_t = function_set_reference_t<F, rval_ref_v>;

// add reference does reference-collapsing
template <typename F, ref_qual R>
using function_add_reference = function_set_reference<F, reference_v<F>() + R>;
template <typename F, ref_qual R>
using function_add_reference_t =
    function_set_reference_t<F, reference_v<F>() + R>;

template <typename F>
using function_remove_reference = function_set_reference<F, null_ref_v>;
template <typename F>
using function_remove_reference_t = function_set_reference_t<F, null_ref_v>;

// set_cvref, set_cvref_as, remove_cvref
template <typename F, bool C, bool V, ref_qual R = null_ref_v>
using function_set_cvref =
    typename function_traits<F>::template set_cvref<V, R>;
template <typename F, bool C, bool V, ref_qual R = null_ref_v>
using function_set_cvref_t =
    typename function_traits<F>::template set_cvref_t<V, R>;

template <typename F, typename S>
using function_set_cvref_as =
    function_set_cvref<F, function_is_const_v<S>, function_is_volatile_v<S>,
                       reference_v<S>()>;
template <typename F, typename S>
using function_set_cvref_as_t =
    function_set_cvref_t<F, function_is_const_v<S>, function_is_volatile_v<S>,
                         reference_v<S>()>;

template <typename F>
using function_remove_cvref_t = typename function_traits<F>::remove_cvref_t;
template <typename F>
using function_remove_cvref = function_traits<function_remove_cvref_t<F>>;

// set_noexcept, add_noexcept / remove_noexcept
template <typename F, bool N>
using function_set_noexcept =
    typename function_traits<F>::template set_noexcept<N>;
template <typename F, bool N>
using function_set_noexcept_t =
    typename function_traits<F>::template set_noexcept_t<N>;

template <typename F>
using function_add_noexcept = function_set_noexcept<F, true>;
template <typename F>
using function_add_noexcept_t = function_set_noexcept_t<F, true>;

template <typename F>
using function_remove_noexcept = function_set_noexcept<F, false>;
template <typename F>
using function_remove_noexcept_t = function_set_noexcept_t<F, false>;

// set_variadic, add_variadic / remove_variadic
template <typename F, bool A>
using function_set_variadic =
    typename function_traits<F>::template set_variadic<A>;
template <typename F, bool A>
using function_set_variadic_t =
    typename function_traits<F>::template set_variadic_t<A>;

template <typename F>
using function_add_variadic = function_set_variadic<F, true>;
template <typename F>
using function_add_variadic_t = function_set_variadic_t<F, true>;

template <typename F>
using function_remove_variadic = function_set_variadic<F, false>;
template <typename F>
using function_remove_variadic_t = function_set_variadic_t<F, false>;

// return_type
template <typename F>
using function_return_type_t = typename function_traits<F>::return_type_t;
template <typename F> struct function_return_type {
  using type = function_return_type_t<F>;
};

// set_return_type
template <typename F, typename T>
using function_set_return_type_t =
    typename function_traits<F>::template set_return_type_t<T>;
template <typename F, typename T> using function_set_return_type =
  function_traits<function_set_return_type_t<F, T>>;

// signature, equivalent to 'remove_cvref_noexcept'
template <typename F>
using function_signature_t = typename function_traits<F>::signature_t;
template <typename F>
using function_signature = function_traits<function_signature_t<F>>;

// set_signature
template <typename F, typename S>
using function_set_signature_t =
    typename function_traits<F>::template set_signature_t<S>;
template <typename F, typename S> struct function_set_signature {
  using type = function_set_signature_t<F, S>;
};

// args_t
template <typename F,
          template <typename...> typename T = function_parameter_types>
using function_args_t = typename function_traits<F>::template args_t<T>;

} // namespace ltl
