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
         - noexcept: true or false       2 combinations

    Function 'signature' here refers to return type R and parameters P...
    (with optional C-style varargs but without qualifiers and noexcept):

      function_signature_t<F> // removes cvref qualifiers and noexcept

    Function types with cvref qualifiers are 'abominable' (see doc refs).
    To test if function type F is cvref qualified use predicate trait:

      function_is_cvref_v<F> // true if any c,v or ref qualifier present

    This library follows std trait conventions:

      '_t' suffix for result type   (the trait is a type alias template)
      '_v' suffix for result value  (the trait is a variable template)
       no suffix for a result class (the trait is a class template
                                     with 'type' or 'value' member)
    no suffix e.g.:
      function_is_cvref<F> // class inherited from std::bool_constant<?>

    Conventional 'add' and 'remove' traits mutate their named trait:

      function_add_noexcept_t<F> // add noexcept specifier
      function_remove_cvref_t<F> // remove all cv and ref qualifiers
                                 // leaving signature and exception spec

    Unconventionally, this library also provides transforming / mutating
    'set' traits that take extra template arguments, e.g.:

      function_set_noexcept_t<F,B>  // set noexcept(B) for bool const B
      function_set_signature_t<F,S> // set S as the function signature
                                    // keeping cvref-nx of function F

    Reference qualifiers are represented by an enum type ref_qual_v:

      - ref_qual_v{}  no reference qualifier
      - lval_ref_v    lvalue reference qualifier: &
      - rval_ref_v    rvalue reference qualifier: &&

    Template function reference_v<T> returns the reference type of T:

      reference_v<T>() -> ref_qual_v

    The 'set_reference' traits then allow to copy between function types:

      function_set_reference<F, reference_v<G>()> // copy G ref qual to F

      function_set_reference<F, lval_ref_v>   // set ref qual to &
      function_set_reference_lvalue<F>        // set ref qual to &

      function_set_reference<F, ref_qual_v{}> // set ref qual to none
      function_remove_reference<F>            // set ref qual to none

    (There is no 'add_reference' because this may be taken to imply a
     reference collapse semantic which does not apply to function types.)

    Copying all cvref qualifiers is verbose with 'set_cvref' so this is
    provided as a 'set_cvref_as' trait:

      function_set_cvref_as_t<F,G> // copy cvref quals of G to F


  function<F>: class containing traits of function type F as members
  ===========
    Template 'trait class' function<F> implements traits as members.
    It is exposed to the user, not hidden as an implementation detail,
    because it offers an alternative interface.

    Examples of global traits and their equivalent 'member' traits:

      function_is_const_v<F>    function<F>::is_const()
      function_is_const<F>      typename function<F>::is_const
      function_return_type_t<F> typename function<F>::return_type_t
      function_set_const<F,?>   typename function<F>::template set_const<?>

    Disambiguating 'typename' and 'template' keywords may be needed.

    Note: MSVC may need __cdecl for varargs (untested)
*/

namespace ltl
{

// function<F> class: a collection of member traits for function type F
//   or an incomplete type for non-function type F
template <typename F>
struct function;

// ref_qual_v: a value to represent a reference qualifier
//   ref_qual_v{}  no reference qualifier
//   lval_ref_v    lvalue reference qualifier: &
//   rval_ref_v    rvalue reference qualifier: &&
enum ref_qual_v
{
  lval_ref_v = 1,
  rval_ref_v = 2
};

// reference_v<T>() returns a ref_qual_v value representing
//  the reference qualifier on type T
template <typename T>
constexpr ref_qual_v reference_v()
{
  if constexpr (std::is_function_v<T>)
    return typename function<T>::is_lvalue_reference() ? lval_ref_v
         : typename function<T>::is_rvalue_reference() ? rval_ref_v : ref_qual_v{};
  else
    return std::is_lvalue_reference_v<T> ? lval_ref_v
         : std::is_rvalue_reference_v<T> ? rval_ref_v : ref_qual_v{};
}


// function_quals<setter, flags...> Flags for 24 cvref-noexcept combos
// with injected setter template and template member aliases
template <template <bool,bool,ref_qual_v,bool> typename set_quals_t,
          bool c, bool v, ref_qual_v ref, bool nx>
struct function_quals
{
  using quals_t = function_quals;

  using is_const = std::bool_constant<c>;
  using is_volatile = std::bool_constant<v>;
  using is_lvalue_reference = std::bool_constant<ref==lval_ref_v>;
  using is_rvalue_reference = std::bool_constant<ref==rval_ref_v>;
  using is_noexcept = std::bool_constant<nx>;

  using is_cv = std::bool_constant<c||v>;
  using is_reference = std::bool_constant<ref>;
  using is_cvref = std::bool_constant<c||v||ref!=ref_qual_v{}>;

  template <bool C>
    using set_const_t = set_quals_t<C,v,ref,nx>;
  template <bool V>
    using set_volatile_t = set_quals_t<c,V,ref,nx>;
  template <bool C, bool V>
   using set_cv_t = set_quals_t<C,V,ref,nx>;
  template <ref_qual_v Ref>
    using set_reference_t = set_quals_t<c,v,Ref,nx>;
  template <bool C, bool V, ref_qual_v Ref=ref_qual_v{}>
    using set_cvref_t = set_quals_t<C,V,Ref,nx>;
  template <bool NX>
    using set_noexcept_t = set_quals_t<c,v,ref,NX>;

  template <bool C>
    using set_const = function<set_const_t<C>>;
  template <bool V>
    using set_volatile = function<set_volatile_t<V>>;
  template <bool C, bool V>
    using set_cv = function<set_cv_t<C,V>>;
  template <ref_qual_v Ref>
    using set_reference = function<set_reference_t<Ref>>;
  template <bool C, bool V, ref_qual_v Ref=ref_qual_v{}>
    using set_cvref = function<set_cvref_t<C,V,Ref>>;
  template <bool NX>
    using set_noexcept = function<set_noexcept_t<NX>>;
};


// A default type-list type for returning function parameter types
template <typename...> struct function_parameter_types;

// function_base<F>: a base class for function<F> holding its 'signature'
//   R(P...)     - return type R, parameter types P..., or
//   R(P...,...) - with a trailing variadic parameter pack ...
// but not including any function cvref qualifiers or noexcept specifier.
template <typename F>
struct function_base;

// A macro definition is used to expand non-variadic and variadic signatures.
// Clang warns when a variadic signature omits the comma; R(P... ...), so
// __VA_ARGS__ = ,... includes the leading comma, present as needed
// (C++20's __VA_OPT__(,...) is another way to expand with leading comma).

// function_base<F> specialisations for non-variadic and variadic signatures
#define FUNCTION_BASE(...) \
template <typename R, typename... P>\
struct function_base<R(P...__VA_ARGS__)>\
{\
 using signature_t = R(P...__VA_ARGS__);\
 using signature_noexcept_t = R(P...__VA_ARGS__) noexcept;\
\
 using is_variadic = std::bool_constant<#__VA_ARGS__[0]>;\
\
 template <bool V>\
   using set_variadic_t = std::conditional_t<V,R(P...,...),R(P...)>;\
\
 using return_type = R;\
 template <typename r>\
 using set_return_type_t = r(P...__VA_ARGS__);\
\
 template <template <typename...> typename T>\
   using args_t = T<P...>;\
\
 template <bool,bool,ref_qual_v,bool>\
 struct set_quals;\
\
 template<bool nx> struct set_quals<0,0,ref_qual_v{},nx> { using type = R(P...__VA_ARGS__) noexcept(nx); };\
 template<bool nx> struct set_quals<1,0,ref_qual_v{},nx> { using type = R(P...__VA_ARGS__) const noexcept(nx); };\
 template<bool nx> struct set_quals<0,1,ref_qual_v{},nx> { using type = R(P...__VA_ARGS__) volatile noexcept(nx); };\
 template<bool nx> struct set_quals<1,1,ref_qual_v{},nx> { using type = R(P...__VA_ARGS__) const volatile noexcept(nx); };\
\
 template<bool nx> struct set_quals<0,0,lval_ref_v,nx> { using type = R(P...__VA_ARGS__) & noexcept(nx); };\
 template<bool nx> struct set_quals<1,0,lval_ref_v,nx> { using type = R(P...__VA_ARGS__) const & noexcept(nx); };\
 template<bool nx> struct set_quals<0,1,lval_ref_v,nx> { using type = R(P...__VA_ARGS__) volatile & noexcept(nx); };\
 template<bool nx> struct set_quals<1,1,lval_ref_v,nx> { using type = R(P...__VA_ARGS__) const volatile & noexcept(nx); };\
\
 template<bool nx> struct set_quals<0,0,rval_ref_v,nx> { using type = R(P...__VA_ARGS__) && noexcept(nx); };\
 template<bool nx> struct set_quals<1,0,rval_ref_v,nx> { using type = R(P...__VA_ARGS__) const && noexcept(nx); };\
 template<bool nx> struct set_quals<0,1,rval_ref_v,nx> { using type = R(P...__VA_ARGS__) volatile && noexcept(nx); };\
 template<bool nx> struct set_quals<1,1,rval_ref_v,nx> { using type = R(P...__VA_ARGS__) const volatile && noexcept(nx); };\
\
 template <bool c, bool v, ref_qual_v ref, bool nx>\
 using set_qualifiers_t = typename set_quals<c,v,ref,nx>::type;\
};
FUNCTION_BASE()
FUNCTION_BASE(,...) // leading comma forwarded via macro varargs
#undef FUNCTION_BASE

// CV_REF_Q_QUALIFIERS(Q,...)
// X-macro list to expand the 12 cv,ref combos
//   arg Q to inject optional final qualifier / specifier - noexcept
//   ... varargs to pass through C/C++ varargs (inluding a leading comma)
#define CV_REF_Q_QUALIFIERS(Q,...) \
X(,,Q,__VA_ARGS__)\
X(,&,Q,__VA_ARGS__)\
X(,&&,Q,__VA_ARGS__)\
X(const,,Q,__VA_ARGS__)\
X(const,&,Q,__VA_ARGS__)\
X(const,&&,Q,__VA_ARGS__)\
X(volatile,,Q,__VA_ARGS__)\
X(volatile,&,Q,__VA_ARGS__)\
X(volatile,&&,Q,__VA_ARGS__)\
X(const volatile,,Q,__VA_ARGS__)\
X(const volatile,&,Q,__VA_ARGS__)\
X(const volatile,&&,Q,__VA_ARGS__)

// X-macro list to expand all 48 cv,ref,noexcept,variadic combos
#define CV_REF_NOEXCEPT_QUALIFIERS_VARIAD \
CV_REF_Q_QUALIFIERS(,)\
CV_REF_Q_QUALIFIERS(noexcept,)\
CV_REF_Q_QUALIFIERS(, ,...)\
CV_REF_Q_QUALIFIERS(noexcept, ,...) // leading comma for variadic match

#define X(CV,REF,NX,...) \
template <typename R, typename... P>\
struct function<R(P...__VA_ARGS__) CV REF NX>\
: function_base<R(P...__VA_ARGS__)>\
, function_quals<\
    function_base<R(P...__VA_ARGS__)>::template set_qualifiers_t,\
    std::is_const_v<int CV>,\
    std::is_volatile_v<int CV>,\
    reference_v<int REF>(),\
    noexcept(std::declval<void()NX>()())>\
{\
  using type = R(P...__VA_ARGS__) CV REF NX;\
  using remove_cvref_t = R(P...__VA_ARGS__) NX;\
\
 template <bool V>\
   using set_variadic_t = std::conditional_t<V,\
     R(P...,...) CV REF NX, R(P...) CV REF NX>;\
\
  template <typename>\
   struct set_signature;\
  template <typename r, typename... p>\
   struct set_signature<r(p...)> {\
    using type = r(p...) CV REF NX; };\
  template <typename r, typename... p>\
   struct set_signature<r(p...,...)> {\
    using type = r(p...,...) CV REF NX; };\
  template <typename B>\
   using set_signature_t = typename set_signature<B>::type;\
};
CV_REF_NOEXCEPT_QUALIFIERS_VARIAD
#undef X
#undef CV_REF_NOEXCEPT_QUALIFIERS_VARIAD

#define FUNCTION_IS_LIST \
X(const) X(volatile) X(cv)\
X(reference) X(lvalue_reference) X(rvalue_reference)\
X(cvref)\
X(noexcept)\
X(variadic)

#define X(QUAL) \
template <typename F>\
struct function_is_##QUAL : function<F>::is_##QUAL {};\
template <typename F>\
inline constexpr bool function_is_##QUAL##_v =\
         typename function<F>::is_##QUAL();
FUNCTION_IS_LIST
#undef X
#undef FUNCTION_IS_LIST

// set_const, add_const / remove_const
template <typename F, bool C>
using function_set_const = typename function<F>::template set_const<C>;
template <typename F, bool C>
using function_set_const_t = typename function<F>::template set_const_t<C>;

template <typename F>
using function_add_const = function_set_const<F,true>;
template <typename F>
using function_add_const_t = function_set_const_t<F,true>;

template <typename F>
using function_remove_const = function_set_const<F,false>;
template <typename F>
using function_remove_const_t = function_set_const_t<F,false>;

// set_volatile, add_volatile / remove_volatile
template <typename F, bool V>
using function_set_volatile = typename function<F>::template set_volatile<V>;
template <typename F, bool V>
using function_set_volatile_t = typename function<F>::template set_volatile_t<V>;

template <typename F>
using function_add_volatile = function_set_volatile<F,true>;
template <typename F>
using function_add_volatile_t = function_set_volatile_t<F,true>;

template <typename F>
using function_remove_volatile = function_set_volatile<F,false>;
template <typename F>
using function_remove_volatile_t = function_set_volatile_t<F,false>;

// set_cv, remove_cv (add_cv would add c AND v while is_cv tests c OR v)
template <typename F, bool C, bool V>
using function_set_cv = typename function<F>::template set_cv<V>;
template <typename F, bool C, bool V>
using function_set_cv_t = typename function<F>::template set_cv_t<V>;

template <typename F>
using function_remove_cv = function_set_cv<F,false,false>;
template <typename F>
using function_remove_cv_t = function_set_cv_t<F,false,false>;

// set_reference, set_reference_lvalue, set_reference_rvalue
template <typename F, ref_qual_v R>
using function_set_reference = typename function<F>::template set_reference<R>;
template <typename F, ref_qual_v R>
using function_set_reference_t = typename function<F>::template set_reference_t<R>;

template <typename F>
using function_set_reference_lvalue = function_set_reference<F,lval_ref_v>;
template <typename F>
using function_set_reference_lvalue_t = function_set_reference_t<F,lval_ref_v>;

template <typename F>
using function_set_reference_rvalue = function_set_reference<F,rval_ref_v>;
template <typename F>
using function_set_reference_rvalue_t = function_set_reference_t<F,rval_ref_v>;

/*
// add reference should do reference-collapsing
template <typename F>
using function_add_reference = function_set_reference<F,true>;
template <typename F>
using function_add_reference_t = function_set_reference_t<F,true>;
*/
template <typename F>
using function_remove_reference = function_set_reference<F,ref_qual_v{}>;
template <typename F>
using function_remove_reference_t = function_set_reference_t<F,ref_qual_v{}>;

// set_cvref, set_cvref_as, remove_cvref (add_cvref makes no sense)
template <typename F, bool C, bool V, ref_qual_v R = ref_qual_v{}>
using function_set_cvref = typename function<F>::template set_cvref<V,R>;
template <typename F, bool C, bool V, ref_qual_v R = ref_qual_v{}>
using function_set_cvref_t = typename function<F>::template set_cvref_t<V,R>;

template <typename F, typename S>
using function_set_cvref_as = function_set_cvref<F,
  function_is_const_v<S>,function_is_volatile_v<S>,reference_v<S>()>;
template <typename F, typename S>
using function_set_cvref_as_t = function_set_cvref_t<F,
  function_is_const_v<S>,function_is_volatile_v<S>,reference_v<S>()>;

template <typename F>
using function_remove_cvref_t = typename function<F>::remove_cvref_t;
template <typename F>
using function_remove_cvref = function<function_remove_cvref_t<F>>;

// set_noexcept, add_noexcept / remove_noexcept
template <typename F, bool N>
using function_set_noexcept = typename function<F>::template set_noexcept<N>;
template <typename F, bool N>
using function_set_noexcept_t = typename function<F>:: template set_noexcept_t<N>;

template <typename F>
using function_add_noexcept = function_set_noexcept<F,true>;
template <typename F>
using function_add_noexcept_t = function_set_noexcept_t<F,true>;

template <typename F>
using function_remove_noexcept = function_set_noexcept<F,false>;
template <typename F>
using function_remove_noexcept_t = function_set_noexcept_t<F,false>;

// set_variadic, add_variadic / remove_variadic
template <typename F, bool A>
using function_set_variadic = typename function<F>::template set_variadic<A>;
template <typename F, bool A>
using function_set_variadic_t = typename function<F>:: template set_variadic_t<A>;

template <typename F>
using function_add_variadic = function_set_variadic<F,true>;
template <typename F>
using function_add_variadic_t = function_set_variadic_t<F,true>;

template <typename F>
using function_remove_variadic = function_set_variadic<F,false>;
template <typename F>
using function_remove_variadic_t = function_set_variadic_t<F,false>;

// return_type
template <typename F>
using function_return_type_t = typename function<F>::return_type;
template <typename F>
struct function_return_type { using type = function_return_type_t<F>; };

// set_return_type
template <typename F, typename T>
using function_set_return_type_t = typename function<F>::template set_return_type_t<T>;
template <typename F, typename T>
struct function_set_return_type { using type = function_set_return_type_t<F,T>; };

// signature 'remove_cvref_noexcept'
template <typename F>
using function_signature_t = typename function<F>::signature_t;
template <typename F>
using function_signature = function<function_signature_t<F>>;

// signature_noexcept
template <typename F>
using function_signature_noexcept_t = typename function<F>::signature_noexcept_t;
template <typename F>
using function_signature_noexcept = function<function_signature_noexcept_t<F>>;

// set_signature
template <typename F, typename S>
using function_set_signature_t = typename function<F>::template set_signature_t<S>;
template <typename F, typename S>
struct function_set_signature { using type = function_set_signature_t<F,S>; };

// args_t
template <typename F, template <typename...> typename T = function_parameter_types>
using function_args_t = typename function<F>::template args_t<T>;

} // namespace ltl