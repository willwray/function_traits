## Design
<h3>Naming</h3>
This library consistently uses the term 'function' where P0172 uses 'member':

>**`function`**: a type `F` for which [`std::is_function<F>`](https://en.cppreference.com/w/cpp/types/is_function) is `true_type`.
><br>Not to be confused with [`std::function`](https://en.cppreference.com/w/cpp/utility/functional/function):
a wrapper for [*`Callable`*](https://en.cppreference.com/w/cpp/named_req/Callable) types.

This unfortunate C++11 naming clash poisoned future `std` usage of 'function'.
<br>This may be why P0172 uses 'member' - quoting end of section 2.1:

>Note that it is not possible to create a function that has an abominable type.
><br>Rather, the cv-ref qualifier applies to the implicit `*this` reference when
><br>calling a member function. However, **abominable function types are <br>specifically function types, and not member function types.**
><br>This is evident from the lack of ability to specify the type of class the
<br>abominable function would be a member of, when declaring such a type.
* P0172R0 **Abominable Function Types** by Alisdair Meredith, Nov 2015

So, both 'member' and 'function' are misleading terms on their own.
<br>This library, outside the `std` namespace, is free to use 'function'
<br>as the more consistent choice.

<h3>Order</h3>
This library always puts `function` up front and qualifiers at the end.
<br>E.g. for the predicates:

| P0172 proposed `std` name      | `function_trait` name        |
|---|---|
|`is_const_member`               |`function_is_const`           |
|`is_volatile_member`            |`function_is_volatile`        |
|`is_cv_member`                  |`function_is_cv`              |
|`is_lvalue_reference_member`    |`function_is_lvalue_reference`|
|`is_rvalue_reference_member`    |`function_is_rvalue_reference`|
|`has_member_qualifiers`         |`function_is_cvref`           |
|`is_noexcept` (Boost.CallableTraits)|`function_is_noexcept`    |

A function_trait name such as `function_is_const`
<br>can be read more verbosely as `function_type_is_const_qualified`
<br>(the is_noexcept trait reads as `function_type_has_noexcept_specifier`)

Why put '`function`' upfront?
<br>Why not put '`is`' upfront, as is convention for predicates?

`is_function_const` | `is_function_type_const_qualified` ?

1. In this design, the global trait name reflects the member trait name
<br>   `function_is_noexcept<F>`   'global trait'
<br>   `function<F>::is_noexcept`  'member trait'

2. The `function_traits` are only defined for `function` types.
<br>For use with non-function types, guard with `std::is_function`:
```cpp
  std::is_function_v<T> && function_is_noexcept_v<T>
```
* This reads "is the type T a function and, if so, is it noexcept?".
<br>This combined trait is more appropriately called `is_function_noexcept`.

<h3>Setters</h3>
From P0172:

>Some obvious issues with this proposal are deferred [...]. For example,
><br>it is not clear whether the following should be supported, or what it should mean:
```cpp
using result = add_member_lvalue_reference_t<void() &&>;
```

More examples: the 'mutating' traits:
| P0172 proposed `std` name | `function_trait` name |
|---|---|
|`add_member_const`              |`function_add_const`         |
|`add_member_volatile`           |`function_add_volatile`      |
|`add_member_cv`                 |`function_add_cv`            |
|`add_member_lvalue_reference`   |`function_set_reference<1,0>`|
|`add_member_rvalue_reference`   |`function_set_reference<0,1>`|
|`remove_member_const`           |`function_remove_const`      |
|`remove_member_volatile`        |`function_remove_volatile`   |
|`remove_member_cv`              |`function_remove_cv`         |
|`remove_member_lvalue_reference`|`function_set_reference<0,0>`|
|`remove_member_rvalue_reference`|`function_set_reference<0,0>`|
|`remove_all_member_qualifiers`  |`function_remove_cvref`      |

Note how `set` is used, with Boolean arguments, to replace `add`/`remove`

