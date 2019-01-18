# `function_traits` reference

### `namespace ltl`
`ltl` is the namespace for all traits and utilities in `function_traits`  
(pronounce it as you like; LTL as in STL or 'litl' as little Italy said fast).

* Traits prefixed with `function_` are only defined for C++ function types.  
* Traits prefixed with `is_` are (predicates) defined for any C++ type  
  (returning an empty class type for non-function type argument).

## Introduction

This reference deals with the function traits in groups:

* [Predicate traits](#predicate_traits): `is_function_*<T>` and `function_is_*<F>`
* Type traits (property 'getters'): `function_*<F>`
* Modifying traits, no Args: `function_add_*<F>`, `function_remove_*<F>`
* Modifying traits with Args (property 'setter's): `function_set_*<F,Args...>`

Each group has a documented interface for its set of properties ('wildcard' `*`).  
Once you understand the properties of C+ function types, the meaning of  
each trait follows logically from the property `*` and the trait group.  
See the [Readme](readme.md) here and then the reference [P0172R0](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0172r0.html) for starters.

The property 'setter' traits are an unconventional

## Terminology, conventional and unconventional

Function types are awkward types, even among C++ types.

In particular, all the properties of a C++ function type are on one level;  
changing the function signature, modifying cvref qualifiers or noexcept -  
these act to change the type itself, they do not 'nest' or 'compound'  
(function is a compound type - a compound of its return and argument types)

For conventional `std` traits, `add` and `remove` variants mostly correspond  
to 'compounding' and 'decompounding' modifiers

Following `std` convention, there are variants 


A few words here to 


<details><summary>Table of function type terminology</summary>

|name<br>for function subtype| has `cvref` qualifiers<br>[`const`] [`volatile`] [`&`\|`&&`]| is `noexcept`<br>`noexcept(true)`|
|-| - |-|
|'**function**': general function| don't care | don't care |
|'**free**' function<br>'normal', 'ordinary', 'plain' | NO | don't care |
|function '**signature**'<br>(a free function subtype) | NO | NO ==<br>`noexcept(false)`|
|'**abominable**' function | YES | don't care |

</details>



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
|`function_args_t<F,tuple_type>`|`args<F,tuple_type>`|
|`function_return_type<F>`|`return_type<F>`|
|`function_signature<F>`| c.f. `function_type`|
||`qualified_class_of<F>`|
||`remove_transaction_safe<F>`|
||`add_transaction_safe<F>`|
||`class_of<F>`|
||`apply_member_pointer<F>`|

</details>