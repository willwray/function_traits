# `function_traits` reference

### `namespace ltl`
`ltl` is the namespace for all traits and utilities in `function_traits`  
(pronounce it as you like; LTL as in STL or 'litl' as 'little Italy' said fast).

* Traits prefixed with `is_` are (predicates) defined for any C++ type  
* Traits prefixed with `function_` are only defined for C++ function types  

A pair of 'top level' predicate traits classify C++ function types among all C++ types:

* `ltl::is_function<T>` equivalent to `std::is_function<T>`
* `ltl::is_free_function<T>` true for `T` a non-cvref-qualified function type

These top level predicates evaluate as `std::true_type` or `std::false_type`  
unlike the function predicate traits which treat non-function types differently:

* `is_function_*<T>` return an empty type for non-function type `T`
* `function_is_*<T>` cause a compile error on non-function type `T`

This is indicated by a `Function` concept - `template <Function F>`  
though the library targets C++17 so does not yet support C++20 concepts.

## Synopsis

<details><summary>List of traits (82 and counting, including _t | _v variants)</summary>

```c++
// 'Top level' predicate traits is_*<T>, is_*_v<T>
// ============================
template <typename T> struct is_* : bool_constant<P<T>> {};
template <typename T> bool is_*_v = bool{P<T>>};

  is_function
  is_free_function
```

```c++
// Function predicate traits is_function_*<T>
// =========================
template <typename T> struct is_function_* : predicate_base<P,T> {};
// predicate_base<P,T> is an empty struct for non-function type T
//                  or is bool_constant<P<T>> for function type T
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
template <Function F> using function_is_*  = bool_constant<P<F>>;
template <Function F> bool function_is_*_v = bool{<P<F>>};

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
template <typename T> ref_qual *reference_v = RefQual<T>;

  reference_v<T>          (ordinary top level reference qual)
  function_reference_v<F> (defined only on function types F)
```

```c++
// Function signature traits
// =========================

  function_return_type
  function_arg_types
  function_signature
```

```c++
// Function modifying traits taking no arguments
// =========================
template <Function F> using function_*_*_t = F_; // modified type;
template <Function F> using function_*_* = function_traits<F_>;

  function_add_const             function_remove_const
  function_add_volatile          function_remove_volatile
  function_add_noexcept          function_remove_noexcept
  function_add_variadic          function_remove_variadic
                                 function_remove_reference
  function_set_reference_lvalue
  function_set_reference_rvalue
```

```c++
// Function modifying traits taking arguments
// =========================
template <Function F, Args...> using function_*_*_t = F_; // modified type;
template <Function F, Args...> using function_*_* = function_traits<F_>;

  function_add_reference  <F, ref_qual ref>  // 'add' does reference-collapse
  function_set_reference  <F, ref_qual ref>

  function_set_const     <F, bool C>
  function_set_volatile  <F, bool V>
  function_set_noexcept  <F, bool NX>
  function_set_variadic  <F, bool Varg>

  function_set_cv        <F, bool C, bool V>

  function_set_cvref     <F, bool C, bool V, ref_qual ref>

  function_set_return_type <F, R>
  function_set_signature   <F, FuncSig>
  function_set_cvref_as    <F, FuncSource>
```

</details>

## Traits by group

* [Predicate traits](#predicate-traits): `is_function_*<T>`, `function_is_*<F>`  
For`*` in `const`, `volatile`, `cv`, `cvref`, `noexcept`, `variadic`,  
`reference`, `lvalue_reference`, `rvalue_reference`

* [Reference value traits](#reference-value): `function_reference_v<F>`  
(and `reference_v<T>` for ordinary top-level reference qualifier)  
Evaluate to a value of enum type `ltl::ref_qual`

* [Type traits](#type-traits): (type property 'getters') `function_*<F>`  
For`*` in `signature`, `return_type`, `arg_types`

* [Add / remove traits](#add-remove-traits): `function_add_*<F>`, `function_remove_*<F>`  
(with no Args) For`*` in `const`, `volatile`, `noexcept`, `variadic`  
(with no Args) `function_remove_reference<F>` (remove only)  
(one ref Arg) `function_add_reference<F,ref_qual>` (ref-collapse)

* [Set traits](#set-traits): (property 'setters'): `function_set_*<F,Args...>`  
(with no Args) For`*` in `reference_lvalue`, `reference_rvalue`  
(one bool Arg) For`*` in `const`, `volatile`, `noexcept`, `variadic`  
(two bool Args, C,V) `function_set_cv<F,C,V>`  
(one ref_qual Arg) `function_set_reference<F,ref_qual>`  
(two bool, one ref) `function_set_cvref<F,C,V,ref_qual>`  
(one signature Arg) `function_set_signature<F,FuncSig>`

* [Copy traits](#copy-traits): `function_set_cvref_as<F,G>`  
(`function_set_signature` can copy cvref and noexcept)  
(individual qualifiers can be copied using `function_set_*` traits)

## Terminology

### Terms to classify C++ function subtypes</summary>

|function subtype name| has `cvref` qualifiers<br>[`const`] [`volatile`] [`&`\|`&&`] | is `noexcept`<br>`noexcept(true)`|
|-| - |-|
|'**function**' type<br>(general function type)| don't care<br>(if it has qualifiers or not)| don't care |
|'**free**' function type<br>('normal', 'ordinary', 'plain') | NO | don't care |
|function '**signature**' type<br>(a free function subtype) | NO | NO ==<br>`noexcept(false)`|
|'**abominable**' function type<br>(cvref-qualified function type)| YES | don't care 


>'free' indicates that these subtypes are the valid types of free functions.  
'signature', as used here, is chosen to exclude any exception spec.  
'abominable' is a recognised, colloquial, term for 'cvref-qualified'.

</details>

## Properties of function types

For an introduction to the properties of C++ function types  
see the [Readme](readme.md) material and reference [P0172R0](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0172r0.html) for starters.

All the properties of a C++ function type are on one level - its signature,  
cvref qualifiers and exception specification are all equally part of its type.

For the purposes of this API a function type is broken down as:  
1. Return type `R` and parameter types `P...`  
2. 'Flags' for its c,v,ref and noexcept properties, and variadic-ness

Function 'property flags':

* A pair of `bool` flags for `const`, `volatile`
* A `ref_qual` flag for [`&`|`&&`]  
(three-valued: no-ref, lvalue-ref or rvalue-ref)
* A `bool` flag for `noexcept(bool)` 
* A `bool` flag for `R(P...)` or `R(P...,...)`  
(non-variadic or variadic function signature).


'`add`' and '`remove`' are not the best names for traits to modify function properties  
as these conventional terms imply compounding or reference-collapsing.  


Compound types and compounding

Arbitrarily complex types are constructed from fundamental types by compounding;  
adding pointers, array extents or top-level reference, or forming enums or classes  
or function types:

>A C++ function type is a [compound type](https://en.cppreference.com/w/cpp/types/is_compound):  
Its function 'signature' is a compound of its return type and argument types.  
Any `cvref` qualifiers or `noexcept` are not compounded onto the signature -  
they are part of the function type.

Forming a function type is a compounding operation.  
Changing the type itself is not a compounding operation.

Clearly, changing its signature changes it to a different function type.  
Similarly, changing cvref qualifiers changes the type itself to a differently-  
qualified type, even though with the same signature. In other words,  
the function qualifiers do not 'compound' or 'nest' onto a base signature.

The `add` and `remove` variants of conventional `std` traits have 'compounding'  
as a mental model; `add_pointer` and `add_extent` compound their base type.  
On the other hand, adding `cv` or `ref` qualifiers does not always compound -  
`cv` qualifiers attach to their base type, so a cv-qualified type is not a compound,  
while a reference qualifier can only be

and 'decompounding' modifiers. For instance, adding a  
reference qualifier

Following `std` convention, there are variants 


A few words here to 





## Predicate traits

### Predicates for checking properties of general type

These predicate traits can be evaluated for any type.

<details><summary><code>is_function</code></summary>

`ltl::is_function` is equivalent to [`std::is_function`](https://en.cppreference.com/w/cpp/types/is_function)

Saves redundant instantiation of `std::is_function`  
(for instance, to 'guard' uses of function traits for non-function types)

</details>

<details><summary><code>is_free_function</code></summary>

`is_free_function_v<F>` checks if type `F` is a free function type  
>`true` if `F` is a function type without cvref qualifiers  
`false` if `F` is not a function type or is a cvref qualified function type 
</details>



### Predicates for function type classification

Simple predicate aliases to `std` `true_type` / `false_type`

* `function_is_const`
* `function_is_volatile`
* `function_is_cv`
* `function_is_reference`
* `function_is_lvalue_reference`
* `function_is_rvalue_reference`
* `function_is_cvref`
* `function_is_noexcept`
* `function_is_variadic`

'Lazy' predicates

* `is_function_const`
* `is_function_volatile`
* `is_function_cv`
* `is_function_reference`
* `is_function_lvalue_reference`
* `is_function_rvalue_reference`
* `is_function_cvref`
* `is_function_noexcept`
* `is_function_variadic`


The predicate member traits are aliases of `bool_constant<?>`
<br>(so they are equal to `true_type` or `false_type` depending on `?`).
<br>The bool value itself can be extracted from the `bool_constant`
<br>via its `value` member or by invoking its function call operator

| function\<F\> member trait<hr>function_trait\<F\> standalone | <br>P0172R0 / callable_trait\<F\> standalone |
|----|----|
|`function<F>::is_*` == `bool_constant<?>`<br>`function<F>::is_*()`= `bool{?}`<hr>`function_is_*<F> : bool_constant<?>`<br>`function_is_*_v<F>` = `bool{?}`|<br><br>`is_*_member<F> : bool_constant<?>`<br>`is_*_member_v<F>` = `bool{?}`|

## Type traits

## Add remove traits

* `function_add_*<F>`
* `function_remove_*<F>`

`const`, `volatile`, `noexcept`, `variadic`

These are modifying traits which take no arguments.  
The 

## Set traits


## function_traits and P0172 equivalent

<details><summary><b>Predicate</b> traits</summary>

Predicate traits test a true-or-false property of a type  
returning a type derived from [`std::bool_constant`](https://en.cppreference.com/w/cpp/types/integral_constant)  
i.e. inherited from `std::true_type` or `std::false_type`  

The bool value itself can be extracted from the `bool_constant`  
via its `value` member or by invoking its function call operator.  
Alternatively, a `_v` suffix defines a templated boolean variable -  
i.e. `is_*_v` directly gives `true` or `false` value for property `*`.  
For example, these are all equivalent:

    function_is_noexcept<F>::value
    function_is_noexcept<F>()
    function_is_noexcept_v<F>

|function_trait\<F\>  | P0172R0 / callable_trait\<F\>  |
|----|----|
|`is_free_function<F>`|`not has_member_qualifiers<F>`|

|function_trait\<F\>  | P0172R0 / callable_trait\<F\>  |
|----|----|
|`is_free_function<F>`|`not has_member_qualifiers<F>`|
|`function_is_const<F>`|`is_const_member<F>`|
|`function_is_volatile<F>`|`is_volatile_member<F>`|
|`function_is_cv<F>` (Note: const *OR* volatile)| `is_cv_member<F>` (Note: const *AND* volatile)|
|`function_is_lvalue_reference<F>`|`is_lvalue_reference_member<F>`|
|`function_is_rvalue_reference<F>`|`is_rvalue_reference_member<F>`|
|`function_is_reference<F>`|`is_reference_member<F>`|
|`function_is_cvref<F>`|`has_member_qualifiers<F>`|
|`function_is_noexcept<F>`|`is_noexcept<F>`|
|`function_is_variadic<F>`|`has_varargs<F>`|

</details>

<details><summary><b>Modifying traits</b>: add | remove</summary>

|function_trait\<F\>  | P0172R0 / callable_trait\<F\>  |
|----|----|
|`function_add_const<F>`<br>`function_set_const<F,true>`<br>`function_set_cv<F,true,false>`|`add_member_const<F>`|
|`function_remove_const<F>`|`remove_member_const<F>`|
|`function_add_volatile<F>`<br>`function_set_volatile<F,true>`<br>`function_set_cv<F,false,true>`|`add_member_volatile<F>`|
|`function_remove_volatile<F>`|`remove_member_volatile<F>`|
|`function_set_cv<F,true,true>`|`add_member_cv<F>`|
|`function_remove_cv<F>`|`remove_member_cv<F>`|
|`function_set_reference_lvalue<F>`<br>`function_set_reference<F,lval_ref_v>`|`add_member_lvalue_reference<F>`|
|`function_set_reference_rvalue<F>`<br>`function_set_reference<F,rval_ref_v>`|`add_member_rvalue_reference<F>`|
|`function_remove_reference<F>`|`remove_member_reference<F>`|
|`function_remove_cvref<F>`| c.f. `function_type`|
|`function_signature<F>`<br>`function_signature_noexcept<F>`<br>`function_remove_cvref<F>`|`function_type<F>`|
|`function_set_cvref<F,bool,bool,ref>`<br>`function_set_cvref_as<F,G>`||
|`function_add_noexcept<F>`<br>`function_set_noexcept<F,true>`|`add_noexcept<F>`|
|`function_remove_noexcept<F>`<br>`function_set_noexcept<F,false>`|`remove_noexcept<F>`|
|`function_add_variadic<F>`<br>`function_set_variadic<F,true>`|`add_varargs<F>`|
|`function_remove_variadic<F>`|`remove_varargs<F>`|
|`function_set_return_type<F,R>`|`apply_return<F,R>`|
|`function_set_signature<F,S>`||
|`function_arg_types<F,tuple_type>`|`args<F,tuple_type>`|
|`function_return_type<F>`|`return_type<F>`|
|`function_signature<F>`| c.f. `function_type`|
||`qualified_class_of<F>`|
||`remove_transaction_safe<F>`|
||`add_transaction_safe<F>`|
||`class_of<F>`|
||`apply_member_pointer<F>`|

</details>