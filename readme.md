# **C++ `function_traits` library**

## Type traits for properties of C++ function types

Complete reflection of C++ function types  and modification of their properties.

**Anatomy** of a general C++17 function type:

```c++
template <typename R, typename... P, bool X>
using
function = R(P...[,...]) [const] [volatile] [&|&&] noexcept(X);
```

<details><summary><b>Dissected</b> - a breakdown of the general type, with library API terminology</summary>

>'`A`|`B`' for `A` or `B` alternatives - '[`C`]' for optional `C` term:

```c++
                                                                  /*
            _signature_    ________cvref________    noexcept_      *
           |           |  |                     |  |         |     */
function = R(P...[,...]) [const] [volatile] [&|&&] noexcept(X);   /*
                  |  |    |____   _______|   |  |                  *
                variadic       cv          reference               *
                                        lvalue | rvalue            */
```

Function **signature** (all API terms in **bold**):

* **`R(P...)`**|**`R(P...,...)`** : **signature** = **return_type** `R` and **arg_types** `P...`

>Here, '**signature**' refers to **return_type** `R` and **arg_types** (parameter) `P...`  
including any C-style varargs (termed '**variadic**', denoted by trailing ellipsis **`...`**)  
excluding everything after the function parens (i.e. no cvref or exception spec).

Function varargs existence is treated as a (`bool`) property for API purposes:

* **variadic** : API property name for presence of ellipsis: true | false

Function **noexcept** property (`bool`):

* **`noexcept(X)`** : Function exception specification; X = true | false

Function **cvref** properties (`bool`, `bool`, `ref_qual`):

* [**`const`**] [**`volatile`**] [**`&`**|**`&&`**] : Function **cvref** qualifiers; 12 combos

>Warning: the **cvref** API terms may be familiar from the `std` traits but have  
different meanings and behaviour as function type qualifiers (see API refs):
>
>* **const**, **volatile**, **cv** (const | volatile)  
>* **reference_lvalue**, **reference_rvalue**, **reference** (lval | rval)  
>* **cvref** (const | volatile | reference)

The **cvref** qualifiers divide the function types into two top level categories:

* **free** function types, with no cvref qualifiers - the valid types of free functions  
* **cvref** qualified function types, the so-called 'abominable' function types

Test with function traits `is_free_function<T>` or `function_is_cvref<F>`

</details>

----

<details><summary>Copyright &copy; 2019 Will Wray. Distributed under the Boost Software License, V1.0</summary>

### **Boost Software License** - Version 1.0 - August 17th, 2003

```txt
Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
```

[![License](https://img.shields.io/badge/license-boost%201.0-blue.svg)](https://www.boost.org/LICENSE_1_0.txt)

Also at [boost.org](http://www.boost.org/LICENSE_1_0.txt) and accompanying file [LICENSE_1_0.txt](LICENSE_1_0.txt)

</details>

----

<details><summary><b>Background</b>: C++ function types, the free & the abominable (P0172)</summary>

* <details><summary>C++ function types</summary>

    The `std` type trait `is_function_v<F>` is true for all C++ function types.

    C++ function types include the types of ordinary C/C++ free functions,  
    referred to here as 'free' function types:

    ```c++
    // free function types

      void(int)             or  auto(int) -> void
      char*() noexcept      or  auto() noexcept -> char*
      int(char const*,...)  or  auto(char const*,...) -> int
    ```

    >C++ function types can also have cvref qualifiers:

    ```c++
      int() const&          or  auto() const& -> int
      void() && noexcept    or  auto() && noexcept -> void
      void(int) volatile    or  auto(int) volatile -> void
    ```

    >Such cvref-qualified function types are an artifact of the C++ type system.  
Member functions carry cvref qualifiers for the implicit `*this` reference  
used in calling the member function, so cvref-qualified function types arise  
as part of pointer-to-member-function types.  
You cannot declare an ordinary free function with a cvref type and it is  
forbidden to form a pointer or a reference to a cvref-qualified function type.
    </details>

* <details><summary>P0172R0 <b>Abominable Function Types</b> by Alisdair Meredith, Nov 2015</summary>

    Quoting from [P0172R0](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0172r0.html) section **2.1,     Definition**:

    >[...] an *abominable* function type is the type produced by writing  
a function type followed by a cv-ref qualifier.

    Example:

    ```cpp
     using regular    = void();
     using abominable = void() const volatile &&;
    ```

    >In the example above, `regular` names a familiar function type [...],  
`abominable` also names a function type, not a reference type, and  
despite appearances, is neither a const nor a volatile qualified type.  
There is no such thing as a cv-qualified function type in the type system,  
and the abominable function type is something else entirely.
    </details>

* <details><summary>Boost.CallableTraits: A P0172 implementation and more</summary>

    >[Boost.CallableTraits](https://www.boost.org/doc/libs/develop/libs/callable_traits/doc/html/) implements P0172R0's suggested library interface,  
extended to support general [Callable](https://en.cppreference.com/w/cpp/named_req/Callable) types on top of C++ function types.  
It is a robust, reviewed library with tests, compatibility matrix and CI.
    </details>

</details>

## **Description | Motivation | Aims**

<details><summary><b>Description</b></summary>

>* **Type trait**:  
a template-based interface to query or modify the properties of types.

>**`function_traits`** is a library of traits for C++17 function types -  
no more, no less; it does not provide traits for general [Callable](https://en.cppreference.com/w/cpp/named_req/Callable) types  
(function traits can ease implementation of facilities like callable traits).
>
>It depends on std `<type_traits>` which it complements with function traits.  
The library uses `namespace ltl` for its traits, types and functions.
>
>It targets C++17 on recent gcc / clang / msvc compilers.  
Backwards compatibility, for older compilers or for pre-17, is not a priority.  
It is an 'alpha' design with an experimental interface, subject to change.  
Once C++20 is available, constraints will be added.

</details>

<details><summary><b>Motivation</b>: Provide the 24 (or 48) required specializations</summary>

>See also [Boost.CallableTraits Motivation](https://www.boost.org/doc/libs/develop/libs/callable_traits/doc/html/index.html#callable_traits.introduction.motivation)

**Function traits** are necessary to reflect the properties of function types.  
They may be useful in generic code that must handle general function types.

>'Abominable' function cvref qualifiers cannot be deduced concisely.  
C-style varargs - a trailing ellipsis ... - cannot be deduced concisely.  
A total of 24 separate template specializations are needed to match  
a possibly abominable or variadic function type:

* 12 combinations of cvref qualifiers (4 cv x 3 ref)
* x 2 for presence of C-style varargs (trailing ellipsis...)

>If `noexcept` is not deduced directly then 48 specializations are needed:  

* x 2 for `noexcept` true or false

>It is tedious to have to write all of the necessary specializations.  
This library provides the specializations wrapped up as function traits.
>
>Since all 24/48 specializations are needed to implement *any* function trait  
with full generality, one might as well write a full collection of traits.
>
>**'Setter' traits**
>
>I wanted traits to copy qualifiers from source to target function types (e.g.  
Boost.CallableTraits has an open [issue](https://github.com/boostorg/callable_traits/issues/139) to add a `copy_member_cvref` trait  
and `std::copy_*` traits are proposed in [P1016](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1016r0.pdf) "...type manipulation utilities")
>
>This library provides a couple of options:  
>`function_set_cvref_as<F,G>` copies `G`'s cvref qualifiers to `F`, or  
`function_set_signature<G, function_signature_t<F>>`  
effectively copies `G`'s cvref qualifiers and exception spec to `F`'s signature.

</details>

<details><summary>The 24 (or 48) function pattern specializations</summary>

24 template specializations are required to match any function type pattern  
(assuming that `noexcept` is deducible in partial specializations - see note below):

```c++
// Primary template
template<typename T> struct fun;

// The 24 template partial specializations
// to match cvref qualifiers (x12) and presence of varargs (x2)
// while deducing return type R, parameters P... and noexcept(bool)
template<class R, class... P, bool X> struct fun<R(P...) noexcept(X)> {};
template<class R, class... P, bool X> struct fun<R(P...) & noexcept(X)> {};
template<class R, class... P, bool X> struct fun<R(P...) && noexcept(X)> {};
template<class R, class... P, bool X> struct fun<R(P...) const noexcept(X)> {};
template<class R, class... P, bool X> struct fun<R(P...) const & noexcept(X)> {};
template<class R, class... P, bool X> struct fun<R(P...) const && noexcept(X)> {};
template<class R, class... P, bool X> struct fun<R(P...) volatile noexcept(X)> {};
template<class R, class... P, bool X> struct fun<R(P...) volatile & noexcept(X)> {};
template<class R, class... P, bool X> struct fun<R(P...) volatile && noexcept(X)> {};
template<class R, class... P, bool X> struct fun<R(P...) const volatile noexcept(X)> {};
template<class R, class... P, bool X> struct fun<R(P...) const volatile & noexcept(X)> {};
template<class R, class... P, bool X> struct fun<R(P...) const volatile && noexcept(X)> {};

template<class R, class... P, bool X> struct fun<R(P..., ...) noexcept(X)> {};
template<class R, class... P, bool X> struct fun<R(P..., ...) & noexcept(X)> {};
template<class R, class... P, bool X> struct fun<R(P..., ...) && noexcept(X)> {};
template<class R, class... P, bool X> struct fun<R(P..., ...) const noexcept(X)> {};
template<class R, class... P, bool X> struct fun<R(P..., ...) const & noexcept(X)> {};
template<class R, class... P, bool X> struct fun<R(P..., ...) const && noexcept(X)> {};
template<class R, class... P, bool X> struct fun<R(P..., ...) volatile noexcept(X)> {};
template<class R, class... P, bool X> struct fun<R(P..., ...) volatile & noexcept(X)> {};
template<class R, class... P, bool X> struct fun<R(P..., ...) volatile && noexcept(X)> {};
template<class R, class... P, bool X> struct fun<R(P..., ...) const volatile noexcept(X)> {};
template<class R, class... P, bool X> struct fun<R(P..., ...) const volatile & noexcept(X)> {};
template<class R, class... P, bool X> struct fun<R(P..., ...) const volatile && noexcept(X)> {};
```

Both GCC and Clang deduce noexcept as intended...  
Unfortunately, when `noexcept` was introduced as part of the type system  
the standard was not also updated to specify deduction of noexcept.  
This oversight should be corrected by a defect report before C++2a.

Currently (start of 2019) MSVC does not deduce noexcept and so requires  
the noexcept cases to be expanded via 48 specializations:

```c++
template<class R, class... P> struct fun<R(P...)> {};
template<class R, class... P> struct fun<R(P...) &> {};
template<class R, class... P> struct fun<R(P...) &&> {};
template<class R, class... P> struct fun<R(P...) const> {};
template<class R, class... P> struct fun<R(P...) const &> {};
template<class R, class... P> struct fun<R(P...) const &&> {};
template<class R, class... P> struct fun<R(P...) volatile> {};
template<class R, class... P> struct fun<R(P...) volatile &> {};
template<class R, class... P> struct fun<R(P...) volatile &&> {};
template<class R, class... P> struct fun<R(P...) const volatile> {};
template<class R, class... P> struct fun<R(P...) const volatile &> {};
template<class R, class... P> struct fun<R(P...) const volatile &&> {};
template<class R, class... P> struct fun<R(P..., ...)> {};
template<class R, class... P> struct fun<R(P..., ...) &> {};
template<class R, class... P> struct fun<R(P..., ...) &&> {};
template<class R, class... P> struct fun<R(P..., ...) const> {};
template<class R, class... P> struct fun<R(P..., ...) const &> {};
template<class R, class... P> struct fun<R(P..., ...) const &&> {};
template<class R, class... P> struct fun<R(P..., ...) volatile> {};
template<class R, class... P> struct fun<R(P..., ...) volatile &> {};
template<class R, class... P> struct fun<R(P..., ...) volatile &&> {};
template<class R, class... P> struct fun<R(P..., ...) const volatile> {};
template<class R, class... P> struct fun<R(P..., ...) const volatile &> {};
template<class R, class... P> struct fun<R(P..., ...) const volatile &&> {};

template<class R, class... P> struct fun<R(P...) noexcept> {};
template<class R, class... P> struct fun<R(P...) & noexcept> {};
template<class R, class... P> struct fun<R(P...) && noexcept> {};
template<class R, class... P> struct fun<R(P...) const noexcept> {};
template<class R, class... P> struct fun<R(P...) const & noexcept> {};
template<class R, class... P> struct fun<R(P...) const && noexcept> {};
template<class R, class... P> struct fun<R(P...) volatile noexcept> {};
template<class R, class... P> struct fun<R(P...) volatile & noexcept> {};
template<class R, class... P> struct fun<R(P...) volatile && noexcept> {};
template<class R, class... P> struct fun<R(P...) const volatile noexcept> {};
template<class R, class... P> struct fun<R(P...) const volatile & noexcept> {};
template<class R, class... P> struct fun<R(P...) const volatile && noexcept> {};
template<class R, class... P> struct fun<R(P..., ...) noexcept> {};
template<class R, class... P> struct fun<R(P..., ...) & noexcept> {};
template<class R, class... P> struct fun<R(P..., ...) && noexcept> {};
template<class R, class... P> struct fun<R(P..., ...) const noexcept> {};
template<class R, class... P> struct fun<R(P..., ...) const & noexcept> {};
template<class R, class... P> struct fun<R(P..., ...) const && noexcept> {};
template<class R, class... P> struct fun<R(P..., ...) volatile noexcept> {};
template<class R, class... P> struct fun<R(P..., ...) volatile & noexcept> {};
template<class R, class... P> struct fun<R(P..., ...) volatile && noexcept> {};
template<class R, class... P> struct fun<R(P..., ...) const volatile noexcept> {};
template<class R, class... P> struct fun<R(P..., ...) const volatile & noexcept> {};
template<class R, class... P> struct fun<R(P..., ...) const volatile && noexcept> {};
```

These 48 specializations are also listed in [Boost.CallableTraits](https://www.boost.org/doc/libs/develop/libs/callable_traits/doc/html/index.html#callable_traits.introduction.motivation) and [cppreference](https://en.cppreference.com/w/cpp/types/is_function) `is_function`

</details>

<details><summary><b>Aims</b>: A complete, minimal, forward looking, simple dependency</summary>

* <details><summary>A <b>complete</b> yet <b>minimal</b> set of function type traits</summary>

    **Complete**: provide a way to do any query or modification that may be needed;  
    if you see something that is not reasonably easy to do then open an issue.

    **Minimal**: avoid bloat and duplication in the interface (not easy - 50 traits!).  
    Narrow scope, single responsibility - function traits only, no more, no less.
    </details>

* <details><summary>In a <b>single header</b>, simple to take as a dependency</summary>

    **Simple dependency**: single header, self contained with docs.  
    Mesonbuild example as subproject / git submodule. CMake ToDo.  
    Of course, you can just copy the header or cut-n-paste.

    **Single header**: rather than 'fine-grain' headers per trait.  
    Because each trait has to pull in the full 24 (or 48) specializations,  
    even if a user may only want one of the many traits,  
    it seems not worth the complexity of providing individual headers  
    (if you can show benefits worth the complexity then open an issue).
    </details>

* <details><summary><b>Forward looking</b>: to concepts - down with SFINAE!</summary>

    Look towards concepts and contraints with no need for SFINAE tricks  
    No concern for backward **compatibility** or support of old compilers  
    **Diverge** from the P0172R0 suggested interface as appropriate  
    A clean, modern implementation (macro use confined to header).
    </details>

</details>

## [Reference](reference.md)

## **Examples**

<details><summary>Getting started</summary>

>First, put the header file where you want it and configure your include path.  
Here, the `ltl` include directory reflects `function_traits` namespace, `ltl`  
(or, just cut and paste the header):

```c++
#include <ltl/function_traits.hpp>
```

>All `function_*` traits are defined only for function types.  
Calling a `function_*` trait with a non function type gives a hard,  
non-SFINAE, error (with a nasty error message from the compiler).

```c++
  ltl::function_is_cvref< int > // compile error
```

>The `function_is_*` predicate traits have SFINAE-friendly siblings:

```c++
  ltl::is_function_cvref< int > // empty class
```

>Other `function_*` traits have no safe / SFINAE-friendly variants.  
To use these function traits with non-function types, you can guard the trait  
instantiation with `if constexpr (is_function_v<T>)`:

```c++
template <typename F>
inline constexpr bool is_free_function_v = []{
             if constexpr (is_function_v<F>)
                 return !function_is_cvref_v<F>;
             return false; }();
```

>(`conditional_t` doesn't work here as both branches instantiate.)

</details>

<details><summary>Predicate traits and type property traits</summary>

>Test if a function type is variadic and const and noexcept:  

```c++
  using Fc = void(...) const noexcept;

  static_assert(
          ltl::function_is_variadic_v< Fc >
       && ltl::function_is_const_v< Fc >
       && ltl::function_is_noexcept_v< Fc >
  );
```

>Get the return type of a function type and a type-list of its parameter types:

```c++
  #include <tuple>
  #include <type_traits>

  using Fcb = void( char, bool() );

  static_assert(
     std::is_void_v< ltl::function_return_type_t< Fcb > >
  && std::is_same_v< ltl::function_arg_types< Fcb, std::tuple >
                   , std::tuple< char, bool(*)() > >
  ); //                                    ^^^
     //                               note decay
```

</details>

<details><summary>Modifying traits; add / remove and set traits</summary>

>Conventional `add_*`, `remove_*` traits modify the given property `*`.  
They generally take no arguments beyond the function type to modify:

```c++
  using namespace ltl;
  static_assert(
      std::is_same_v< function_add_const_t<void() &>,
                                           void() const& >
   && std::is_same_v< function_remove_cvref_t<void() const &>,
                                              void() >
  );
```

>Some property traits act as `remove_*` traits; the 'signature' property trait  
effectively removes both cvref and noexcept:

```c++
  static_assert(
      std::is_same_v< function_signature_t<void() & noexcept>,
                                           void() >
  );
```

>`set_*` traits are more programmatic than `add_*` and `remove_*` traits.  
Setters for function cv qualifiers, noexcept and variadic take `bool` arguments:

```c++
  static_assert( function_is_noexcept_v<
                    function_set_noexcept_t<void(), true> >);
```

>The `set` trait for reference qualifiers takes a `ltl::ref_qual` argument  
(an enum type with values `lval_ref_v`, `rval_ref_v` or `null_ref_v`)

```c++
    static_assert(
       std::is_same_v< function_set_reference_t<void() &, rval_ref_v>
                                                void() && >
    );
```

>Reference collapse is not necessarily natural for function reference qualifiers.  
If you need it, `function_add_reference<F,R>` does reference collapse

```c++
    static_assert(
       std::is_same_v< function_add_reference_t<void() &, rval_ref_v>
                                                void() & >
    );
```

>('adding' an rvalue-ref to an lvalue-ref yields an lvalue-ref, consistent with  
`std::add_rvalue_reference` for ordinary reference types; `&` + `&&` => `&`)
>
>Setters for type properties take type arguments; to change function return type:

```c++
    static_assert(
       std::is_same_v< function_set_return_type_t<int(), void>,
                                                  void() >);
```

>The `set_cvref_as` trait provides a way to copy qualifiers to the target function type  
from a source function type:

```c++
    static_assert( std::is_same_v<
                     function_set_cvref_as_t<void() const, int() &>,
                                             void() & >);
```

</b></details>

<details><summary>A small example of function_traits usage</summary>

A contrived example that type-checks `printf`-like member functions that may  
or may not be variadic, then forwards a C++ argument pack to the C varargs  
(the vargs could be matched and type checked against the format string).

```cpp
#include <tuple>
#include "function_traits.hpp"

struct Log0 { int log(char const* fmt) const noexcept; };
struct LogV { int log(char const* fmt,...) const & noexcept; };

template <class C, typename F, typename... Vargs>
int logger(F C::* log_mf, Vargs... vargs) noexcept
{
    static_assert( std::is_function_v<F> );

    static_assert( ltl::function_is_const_v<F> );
    static_assert( ltl::function_is_noexcept_v<F> );
    static_assert( ltl::function_is_variadic_v<F>
                  == bool{sizeof...(vargs)} );

    using R = ltl::function_return_type_t<F>;
    using Ps = ltl::function_arg_types<F,std::tuple>;
    using P0 = std::tuple_element_t<0,Ps>;

    static_assert( std::is_same_v< R, int> );
    static_assert( std::is_same_v< P0, char const*> );

    return (C{}.*log_mf)("logger",vargs...);
}

template int logger(decltype(&Log0::log));
template int logger(decltype(&LogV::log),int);
```

</details>

----

## Build

Meson build script provided, e.g. use with ninja backend

```bash
meson build
ninja -C build
ninja -C build test
```

| Linux Travis| Windows Appveyor|
| :---: | :---: |
|gcc-8, clang-7<br>-std=c++17|MSVC 15.9.4<br>/std:c++latest|
| [![Build Status](https://travis-ci.org/willwray/function_traits.svg?branch=master)](https://travis-ci.org/willwray/function_traits) | [![Build status](https://ci.appveyor.com/api/projects/status/yxe3sawukwt5oqan?svg=true)](https://ci.appveyor.com/project/willwray/function-traits) |
