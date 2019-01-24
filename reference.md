# `function_traits` reference

## `namespace ltl`

`ltl` is the namespace for all traits and utilities in `function_traits`.  
>Pronounce it as you like; LTL as in STL or 'litl' as 'little Italy' said fast.

* Traits prefixed with `function_` are defined for C++ function types only  
* Traits prefixed with `is_` are defined for any C++ type ('safe' predicates)  




## Synopsis

<details><summary>List of traits (total 50, or 86 including _t or _v variants)</summary>

```c++
// Key
// ===
  template <typename T> // Indicates trait is defined for all C++ types T
  template <Function F> // Indicates trait is defined for function types F
                        // *only* (concept Function = is_function_v<F>)

  P            // P is the predicate of a predicate trait; type -> bool
  P<T>, P<F>   // The predicate P evaluated on type T or function type F

  predicate_base<P,T> // An empty struct for non-function type T, or
  predicate_base<P,F> // bool_constant<P<F>> for function type F

  FuncTr      // FuncTr is the function type modifier of a function trait
  FuncTr<F>   // The modified function type result of the function trait
              // (may take Args: FuncTr<F,Args...> : F -> F' function type)

  RefQual<T>     // 3-valued ref_qual; is T lval-ref, rval-ref or not a ref
  FuncRefQual<F> // 3-valued ref_qual; has func F lval, rval or no ref qual

  function_traits<F> // A class containing public member 'type' alias for F
                     // using type = F;
```

```c++
// 'Top level' predicate traits is_*function<T>, is_*function_v<T>
// ============================
template <typename T> struct is_*function : bool_constant<P<T>> {};
template <typename T>
     inline constexpr bool is_*function_v = bool{P<T>>};

  is_function             // equivalent to std::is_function
  is_free_function        // true for a non-cvref-qualified function type
```

```c++
// Function predicate traits is_function_*<T>
// =========================
template <typename T> struct is_function_* : predicate_base<P,T> {};

  is_function_const
  is_function_volatile
  is_function_cv
  is_function_reference
  is_function_lvalue_reference
  is_function_rvalue_reference
  is_function_cvref
  is_function_noexcept
  is_function_variadic
```

```c++
// Function predicate traits function_is_*<F>
// =========================
template <Function F> using function_is_* = bool_constant<P<F>>;
template <Function F>
     inline constexpr bool function_is_*_v = bool{<P<F>>};

  function_is_const
  function_is_volatile
  function_is_cv
  function_is_reference
  function_is_lvalue_reference
  function_is_rvalue_reference
  function_is_cvref
  function_is_noexcept
  function_is_variadic
```

```c++
// Reference qualifier value traits *reference_v<T>
// ================================
template <typename T>
     inline constexpr ref_qual reference_v = RefQual<T>;
template <Function F>
     inline constexpr ref_qual function_reference_v = FuncRefQual<F>;

  reference_v<T>          // Ordinary top level reference qualifier value
  function_reference_v<F> // Function reference qualifier value
```

```c++
// Function signature traits
// =========================
template <typename F> using function_return_type_t = /* Return type of F */
template <typename F> struct function_return_type /* class typedef type */
template <typename F, template <typename...> typename T = ltl::arg_types>
                      using function_arg_types = T<P...>; // P... arg types

  function_return_type_t // alias to the return type R of F
  function_return_type   // class containing public member type alias for R
  function_arg_types     // a typelist (arg_types default) of F's arg types
```

```c++
// Function modifying traits taking no arguments
// =========================
template <Function F> using function_**_t = FuncTr<F>;
template <Function F> using function_** = function_traits<FuncTr<F>>;

  function_signature // could be 'function_remove_cvref_noexcept'

  function_add_const             function_remove_const
  function_add_volatile          function_remove_volatile
/* ***  no add_cv  *** */        function_remove_cv

  function_add_noexcept          function_remove_noexcept
  function_add_variadic          function_remove_variadic

                                 function_remove_reference
  function_set_reference_lvalue
  function_set_reference_rvalue

                                 function_remove_cvref
```

```c++
// Function modifying traits taking arguments
// =========================
template <Function F, Args...> using function_**_t = FuncTr<F,Args...>;
template <Function F, Args...> using function_** = function_traits<
                                                     FuncTr<F,Args...>>;

  function_add_reference  <F, ref_qual ref>  // add does reference-collapse
  function_set_reference  <F, ref_qual ref>  // set does not ref-collapse

  function_set_const     <F, bool C>
  function_set_volatile  <F, bool V>
  function_set_noexcept  <F, bool NX>
  function_set_variadic  <F, bool Varg>

  function_set_cv        <F, bool C, bool V>

  function_set_cvref     <F, bool C, bool V, ref_qual ref>

  function_set_return_type <F, R>         // requires R = valid return type
  function_set_signature   <F, FuncSig>   // requires FuncSig = a signature
  function_set_cvref_as    <F, Function FuncSource>
```

</details>

## Traits indexed by group

Following `std` convention, there are `_t` or `_v` variants as appropriate

* [Top level predicate traits](#top-level-predicates) classify C++ function types among all C++ types  
`is_function<T>` equivalent to `std::is_function<T>`  
`is_free_function<T>` true for `T` a non-cvref-qualified function type

* [Function predicate traits](#function-predicate-traits): `is_function_*<T>`, `function_is_*<F>`  
For`*` in `const`, `volatile`, `cv`, `cvref`, `noexcept`, `variadic`,  
`reference`, `lvalue_reference`, `rvalue_reference`

* [Reference value traits](#reference-value-traits): evaluate to a value of enum type `ltl::ref_qual`  
`function_reference_v<F>` for function type reference qualification  
`reference_v<T>` for ordinary top-level reference qualification  


* [Signature type traits](#signature-type-traits): 'getters' for function return type and  arg types  
`function_return_type<F>` returns the return type of F  
`function_arg_types<F>` returns a type-list of parameter types of F  
`function_signature<F>` returns just the `R(P...)` or `R(P...,...)`  
'signature' (so could be called `function_remove_cvref_noexcept`)

* [Add / remove traits](#add-/-remove-traits): `function_add_*<F>`, `function_remove_*<F>`  
(with no Args) For`*` in `const`, `volatile`, `noexcept`, `variadic`  
(with no Args) `function_remove_cvref<F>` (no add; remove only)  
(with no Args) `function_remove_reference<F>` (remove only)  
(one ref Arg) `function_add_reference<F,ref_qual>` (ref-collapse)

* [Set traits](#set-traits): (property 'setters'): `function_set_*<F,Args...>`  
(with no Args) For`*` in `reference_lvalue`, `reference_rvalue`  
(one bool Arg) For`*` in `const`, `volatile`, `noexcept`, `variadic`  
(two bool Args, C,V) `function_set_cv<F,C,V>`  
(one ref_qual Arg) `function_set_reference<F,ref_qual>`  
(two bool, one ref) `function_set_cvref<F,C,V,ref_qual>`  
(one ret-type arg) `function_set_return_type<F,R>`  
(one signature Arg) `function_set_signature<F,FuncSig>`

* [Copy trait](#copy-trait): `function_set_cvref_as<F,G>`  
(`function_set_signature` can copy cvref and noexcept)  
(individual qualifiers can be copied using `function_set_*` traits)

----

## Terminology

### Terms to classify C++ function subtypes</summary>

|function subtype name| has `cvref` qualifiers<br>[`const`] [`volatile`] [`&`\|`&&`] | is `noexcept`<br>i.e. `noexcept(true)`|
|-| - |-|
|'**function**' type<br>(general function type)| don't care<br>(if it has qualifiers or not)| don't care |
|'**free**' function type<br>('normal', 'ordinary', 'plain') | NO | don't care |
|function '**signature**' type<br>(a free function subtype) | NO | NO ==<br>`noexcept(false)`|
|'abominable' function type<br>(**cvref**-qualified function type)| YES | don't care 


>'free' indicates that these subtypes are the valid types of free functions.  
'signature', as used here, is chosen to exclude any exception spec.  
'abominable' is a recognised, colloquial, term for 'cvref-qualified'.

----

## Properties of function types

See the [Readme](readme.md) material and reference [P0172R0](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0172r0.html) for an introduction.

All the properties of a C++ function type are on one level - its signature,  
cvref qualifiers and exception specification are all equally part of its type.

For the purposes of this API a function type is broken down as:  
1. Return type `R` and parameter types `P...`  
2. 'Flags' for its c,v,ref and noexcept properties, and variadic-ness

Function property 'flags':

* A pair of `bool` flags for `const`, `volatile`
* A `ref_qual` flag for [`&`|`&&`]  
(`ref_qual` is a 3-valued enum; `null_ref_v`, `lval_ref_v`, `rval_ref_v`)  
* A `bool` flag for `noexcept(bool)` 
* A `bool` flag for `R(P...)` or `R(P...,...)`  
(non-variadic or variadic function signature).


Function properties are modified by '`add`', '`remove`' and '`set`' traits.  
Generally, '`add`' and '`remove`' traits modify a fully named property, so take no  
extra arguments, while '`set`' traits take arguments for property 'flag' values to set.  

Reference qualifiers are the exception, because '`add`' implies reference-collapse  
(which is not necessarily a natural semantics for function reference qualifiers).  
The 'no argument' traits are called '`set`' (rather than '`add`') with name ordered  
as '`set_reference_lvalue`' to make clear it "sets reference flag to lval-ref".  
There is both an '`add`' and a '`set`' trait taking a single `ref_qual` argument,  
which the '`add`' trait implementing reference collapse behaviour:

    function_remove_reference<F>     // no ambiguity with 'remove'

    function_set_reference_lvalue<F> // not 'set_lvalue_reference'
    function_set_reference_rvalue<F>

    function_add_reference<F, ref_qual ref> // reference-collapse
    function_set_reference<F, ref_qual ref> // no ref-collapse

----

## Predicate traits

Predicate traits test a true-or-false property of a type

'Top level' predicates, `is_function` and `is_free_function` evaluate true | false  
for any C++ type
(type trait inherits from `std::true_type` | `std::false_type`).

The remaining function predicate traits treat non-function types differently:

* `is_function_*<T>` return an empty type for non-function type `T`
* `function_is_*<T>` cause a compile error on non-function type `T`  

(`is_function_*` is the 'safe' or 'SFINAE-friendly' version of `function_is_*`)  
(`is_function_*` has no `_v` suffix variant - use equivalent `function_is_*_v`).

### Top Level Predicates

A pair of 'top level' predicate traits classify C++ function types among all C++ types:

* **`ltl::is_function<T>`** equivalent to [`std::is_function`](https://en.cppreference.com/w/cpp/types/is_function)
* **`ltl::is_free_function<T>`** true for `T` a non-cvref-qualified function type

```C++
template <typename T> struct is_*function : bool_constant<P<T>> {};
template <typename T>
     inline constexpr bool is_*function_v = bool{P<T>>};
```

#### `is_function`

Use **`ltl::is_function`** in preference to `std::is_function` in a condition  
guarding instantiation of a function trait (because it saves redundant work)  
(for example, see implementation of `is_free_function_v` next).

#### `is_free_function`

Checks if the argument type is a free function type:
>`true` if `F` is a function type without cvref qualifiers  
`false` if `F` is not a function type or is a cvref qualified function type 

Example implementation of **`is_free_function_v`**

```c++
template <typename F>
inline constexpr bool is_free_function_v = []{
             if constexpr (is_function_v<F>)
                 return !function_is_cvref_v<F>;
             return false; }();
```

### Function predicate traits

#### SFINAE-friendly predicates: `is_function_*`

```c++
// predicate_base<P,T> // An empty struct for non-function type T, or
// predicate_base<P,F> // bool_constant<P<F>> for function type F
template <typename T> struct is_function_* : predicate_base<P,T> {};
```

Note: no `_v` suffix variant - use equivalent `function_is_*_v`

* **`is_function_const`**
* **`is_function_volatile`**
* **`is_function_cv`**
* **`is_function_reference`**
* **`is_function_lvalue_reference`**
* **`is_function_rvalue_reference`**
* **`is_function_cvref`**
* **`is_function_noexcept`**
* **`is_function_variadic`**

#### Simple predicates: `function_is_*`

```c++
template <Function F> using function_is_* = bool_constant<P<F>>;
template <Function F>
     inline constexpr bool function_is_*_v = bool{<P<F>>};
```

>As type trait, alias to `std` `true_type` / `false_type`  
As value trait `_v`, evaluate true | false

Compile fail for non-function type arguments.

* **`function_is_const`**
* **`function_is_volatile`**
* **`function_is_cv`**
* **`function_is_reference`**
* **`function_is_lvalue_reference`**
* **`function_is_rvalue_reference`**
* **`function_is_cvref`**
* **`function_is_noexcept`**
* **`function_is_variadic`**

----

## Reference value traits

Reflect reference qualification of a type by returning an enumerated value.

* **`function_reference_v<F>`** for function type reference qualification
* **`reference_v<T>`** for ordinary top-level reference qualification

Evaluate to a value of enum type `ltl::ref_qual`

```c++
enum ref_qual { null_ref_v, rval_ref_v, lval_ref_v };

template <typename T>
     inline constexpr ref_qual reference_v = RefQual<T>;
template <Function F>
     inline constexpr ref_qual function_reference_v = FuncRefQual<F>;
```

The **addition operator** for `ref_qual` values is defined to do reference collapse:

```c++
constexpr ref_qual operator+( ref_qual a, ref_qual b);
```

----

## Signature type traits

These traits are 'getters' for function return type and  arg types:

* **`function_return_type<F>`** returns the return type of F
* **`function_arg_types<F>`** returns a type-list of parameter types of F

```c++
template <typename F> using function_return_type_t = /* Return type of F */
template <typename F> struct function_return_type  {
  using type = function_return_type_t<F>;
};

template <typename F, template <typename...> typename T = ltl::arg_types>
                      using function_arg_types = T<P...>; // P... arg types
```

For example, to get the parameter types of a function type in a `std::tuple`:

```c++
  ltl::function_arg_types< int(char, bool[4]), std::tuple >;
  // Evaluates to std::tuple< char, bool* >
  // Note the usual decay in array type function arguments
```

>There is no corresponding '`set`' trait to set the argument types (instead, construct  
and set the function signature, including return type and possible varargs).

The function signature getter could be called `function_remove_cvref_noexcept`  
(and so included in the next section on add and remove traits)

* **`function_signature<F>`** returns just the `R(P...)` or `R(P...,...)`

```c++
template <typename F> using function_signature_t = /* function signature */
template <typename F>
using function_signature = function_traits<function_signature_t<F>>;
```

----

## Add / remove traits


* **`function_add/remove_const`**
* **`function_add/remove_volatile`**
* **`function_add/remove_noexcept`**
* **`function_add/remove_variadic`**

Remove-only:

* **`function_remove_reference`**
* **`function_remove_cvref`**

```c++
template <Function F> using function_**_t = FuncTr<F>;
template <Function F> using function_** = function_traits<FuncTr<F>>;
```

The above are all modifying traits that take no arguments.

The trait to 'add' a reference qualifier takes an argument of type `ref_qual`  
and 'adds' it to any existing reference qualifier, performing reference collapse:

* **`function_add_reference<F,ref_qual>`** (ref-collapse)

```c++
template <typename F, ref_qual R>
using function_add_reference =
      function_set_reference<F, function_reference_v<F> + R>;

template <typename F, ref_qual R>
using function_add_reference_t =
      function_set_reference_t<F, function_reference_v<F> + R>;
```
----

## Set traits

```c++
template <Function F, Args...> using function_**_t = FuncTr<F,Args...>;
template <Function F, Args...> using function_** = function_traits<
                                                     FuncTr<F,Args...>>;
```

Reference qualifier setter, takes one `ref_qual` Arg. No reference collapse  
(see `function_add_reference` above for trait with reference collapse):

One `ref_qual` Arg:

* **`function_set_reference  <F, ref_qual ref>`**

One `bool` Arg:

* **`function_set_const     <F, bool C>`**
* **`function_set_volatile  <F, bool V>`**
* **`function_set_noexcept  <F, bool NX>`**
* **`function_set_variadic  <F, bool Varg>`**

Two `bool` Args:

* **`function_set_cv        <F, bool C, bool V>`**

Two `bool`, one `ref_qual` Arg:

* **`function_set_cvref     <F, bool C, bool V, ref_qual ref>`**

Other Args:

* **`function_set_return_type <F, R>`**
* **`function_set_signature   <F, FuncSig>`**

Setting return type requires a valid return type (not array or function type).  
Setting signature requires FuncSig is a simple function signature argument.  

----

## Copy trait

* **`function_set_cvref_as    <F, Function FuncSource>`**

This is a particular case of the previous section "Set traits".  
It is provided for convenience in copying cvref qualifiers from a  
source to a target function type:

```c++
template <Function F, Function S>
using function_set_cvref_as =
      function_set_cvref<F, function_is_const_v<S>,
                            function_is_volatile_v<S>,
                            function_reference_v<S>>;
```

with similar implementation for `function_set_cvref_as_t` alias trait.

### Other copy techniques

Clearly, individual properties can be copied in the same way as above, using  
`function_set_*` traits and predicates or 'getter' traits for the properties.

`function_set_signature`, in particular, can copy cvref and noexcept  
from a source to a target function type:

```c++
  function_set_signature<S, function_signature_t<F>>
```
effectively copies `S`'s cvref qualifiers and exception spec to `F`'s signature  
(note the reversed Args - it actually copies `F`'s signature to `S`)

```c++
template <Function F, Function S>
using function_set_cvref_noexcept_as =
      function_set_signature<S, function_signature_t<F>>;
```

----
