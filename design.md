# Design

## Naming

This library consistently uses the term 'function' where P0172 uses 'member':

### 'function'

>**`function`**: a type `F` for which [`std::is_function<F>`](https://en.cppreference.com/w/cpp/types/is_function) is `true_type`.  
Not to be confused with [`std::function`](https://en.cppreference.com/w/cpp/utility/functional/function):
a wrapper for [*`Callable`*](https://en.cppreference.com/w/cpp/named_req/Callable) types.

This unfortunate C++11 naming clash muddied future `std` usage of 'function'.

Despite this, the term 'function' has established meaning, in the traits domain,  
as 'C++ function type' in general, including abominable function types.

On the other hand, outside of the traits domain, 'C++ function' or 'function' itself  
implies a free function whose type is never abominable. Now, `std::function`  
has conflated 'function' with function objects and callables of all kinds.

With `function_traits` squarely in the traits domain and wholly outwith the  
`std` namespace, it is entirely appropriate to reappropriate the term 'function'.

`function_traits` uses `ltl::function_` prefix for all but one of its traits.

The one remaining trait uses `_function` suffix to refine `std::is_function`;  
`ltl::is_free_function` is true for non-abominable functions.

### 'member'

[P0172](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0172r0.html) uses '`_member`', e.g. `is_const_member`, `add_member_const`

This usage is not justified in the paper, though it is discussed at the end of  
section 2.1 "Definition" (my **emphasis**):

>Note that it is not possible to create a function that has an abominable type.  
Rather, the cv-ref qualifier applies to the implicit `*this` reference when  
calling a member function. However, **abominable function types are  
specifically function types, and not member function types.**  
This is evident from the lack of ability to specify the type of class the  
abominable function would be a member of, when declaring such a type.

* P0172R0 **Abominable Function Types** by Alisdair Meredith, Nov 2015

The emphasized text appears to argue against the use of 'member' to refer  
to general function types, whether abominable or not, and argue for 'function'.

To be more precise, there is no such thing as a member function type, only the  
related type of its pointer-to-member-function so, equally, free function types  
are not (pointer-to-) member function types. It is odd to use the term 'member'  
for general function types that include the types of free functions.

On the other hand, it is true that general function types, abominable or not,  
can be extracted from (pointer-to-) member function types whereas the types  
of free functions can only be non-abominable. In this sense, 'member' may be  
used to emphasize 'general' function type.

Within the type system, function types merely model aspects of actual functions  
as needed for type checking. They need not be bound to function entities at all.  
Types can be used to encode pure information. Function types are rich - their  
parameters encode a tuple of (decayed) types and their qualifiers can encode  
24 or 48 (with varargs) discrete values.

Section 2.5 explains "Why do Abominable Function Types Exist".  
Adapting the example code:

```c++
  struct S { void f() const; };

  template <typename F> auto type(F S::*) -> F;

  using F = decltype(type(&S::f));
```

Given a const-qualified member function `f` of `S`:  
`&S::f` is a pointer-to-member-function of type `void(S::*)() const`  
or, more concisely, `F S::*` with `F` = `void() const`

So, both 'member' and 'function' are misleading terms on their own.  
This library, outside the `std` namespace, is free to use 'function'  
as the more consistent choice.

### Order

This library always puts `function` up front and qualifiers at the end.  
E.g. for the predicates:

| P0172 proposed `std` name      | `function_trait` name        |
|---|---|
|`is_const_member`               |`function_is_const`           |
|`is_volatile_member`            |`function_is_volatile`        |
|`is_cv_member`                  |`function_is_cv`              |
|`is_lvalue_reference_member`    |`function_is_reference_lvalue`|
|`is_rvalue_reference_member`    |`function_is_reference_rvalue`|
|`has_member_qualifiers`         |`function_is_cvref`           |
|`is_noexcept` (Boost.CallableTraits)|`function_is_noexcept`    |

A function_trait name such as `function_is_const`  
can be read more verbosely as `function_type_is_const_qualified`  
(the is_noexcept trait reads as `function_type_has_noexcept_specifier`)

Why put '`function`' upfront?  
Why not put '`is`' upfront, as is convention for predicates?

`is_function_const` | `is_function_type_const_qualified` ?

1. In this design, the global trait name reflects the member trait name  
   `function_is_noexcept<F>`   'global trait'  
   `function<F>::is_noexcept`  'member trait'

2. The `function_traits` are only defined for `function` types.  
For use with non-function types, guard with `std::is_function`:

```c++
  std::is_function_v<T> && function_is_noexcept_v<T>
```

* This reads "is the type T a function and, if so, is it noexcept?".  
This combined trait is more appropriately called `is_function_noexcept`.

### Setters vs add / remove

From P0172:

>Some obvious issues with this proposal are deferred [...]. For example,  
it is not clear whether the following should be supported, or what it should mean:

```c++
using result = add_member_lvalue_reference_t<void() &&>;
```

More examples: the 'mutating' traits:
| P0172 proposed `std` name | `function_trait` name |
|---|---|
|`add_member_const`              |`function_add_const`         |
|`add_member_volatile`           |`function_add_volatile`      |
|`add_member_cv`                 |`function_add_cv`            |
|`add_member_lvalue_reference`   |`function_set_reference_lvalue`|
|`add_member_rvalue_reference`   |`function_set_reference_rvalue`|
|`remove_member_const`           |`function_remove_const`      |
|`remove_member_volatile`        |`function_remove_volatile`   |
|`remove_member_cv`              |`function_remove_cv`         |
|`remove_member_lvalue_reference`|`function_remove_reference`  |
|`remove_member_rvalue_reference`|`function_remove_reference`  |
|`remove_all_member_qualifiers`  |`function_remove_cvref`      |

Note how '`set`' is used for function reference qualifier in place of `add`.


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


## function_traits and P0172 equivalent

| function_trait\<F\>  | P0172R0 / callable_trait\<F\>  |
|----|----|
| `is_function_*<F>`|`is_*_member<F>`|

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
|`function_is_reference_lvalue<F>`|`is_lvalue_reference_member<F>`|
|`function_is_reference_rvalue<F>`|`is_rvalue_reference_member<F>`|
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
|`function_set_reference_lvalue<F>`<br>`function_set_reference_lvalue_`|`add_member_lvalue_reference<F>`|
|`function_set_reference_rvalue<F>`<br>`function_set_reference_rvalue_`|`add_member_rvalue_reference<F>`|
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