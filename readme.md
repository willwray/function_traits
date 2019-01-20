# **C++ `function_traits` library**

## Type traits for properties of C++ function types

### The anatomy of a general C++17 function type:

* **`R(P...`**[**`,...`**]**`)`** [**`const`**] [**`volatile`**] [**`&`**|**`&&`**] **`noexcept(X)`**

templated on **`<typename R, typename... P, bool X>`**  

* **`R(P...)`** : Function '**signature**'; its **return** type **`R`**, **parameter** types **`P...`** and
* `R(P...,`**`...`**`)` : existence of C-style **varargs** (denoted by trailing ellipsis **`...`**)
* `R(P...)` **`noexcept(X)`** : Function **exception** specification; X = true | false
* `R(P...)` [**`const`**] [**`volatile`**] [**`&`**|**`&&`**] : Function **cvref** qualifiers; **12** combos

**`function_traits`** provide complete reflection / modification of function types.  
They are useful in generic code that must handle all function types including  
possibly cvref-qualified function types - the 'abominable'  function types.

<details><summary>More on C++ function types</summary>

>**C++ function types** include the types of ordinary C/C++ free functions, e.g.:

    int()                 or  auto() -> int
    void() noexcept       or  auto() noexcept -> void
    void(int)             or  auto(int) -> void
    int(char const*,...)  or  auto(char const*,...) -> int

>C++ function types can also have cvref qualifiers:

    int() const&          or  auto() const& -> int
    void() && noexcept    or  auto() && noexcept -> void
    void(int) volatile    or  auto(int) volatile -> void

>Such cvref-qualified function types are an artifact of the C++ type system.  
Member functions carry cvref qualifiers for the implicit `*this` reference  
used in calling the member function, so cvref-qualified function types arise  
as part of pointer-to-member-function types.
>
>You cannot declare an ordinary free function with a cvref type and it is  
forbidden to form a pointer or a reference to a cvref-qualified function type.
>
>The standard type trait `std::is_function_v<F>` is true for all function types.  
>  
>The C++17 standard library doesn't provide other traits for C++ function types  
(mainly due to the complications caused by the possible cvref qualifers).  
This library fills in for the lack of function traits in the standard library.
</details>

----

<details><summary>Copyright &copy; 2018 Will Wray. Distributed under the Boost Software License, V1.0</summary>

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

(Also at [boost.org](http://www.boost.org/LICENSE_1_0.txt) and accompanying file [LICENSE_1_0.txt](LICENSE_1_0.txt))

</details>

----

## **Description | Background | Motivation | Aims**

<details><summary><b>Description</b></summary>

**Type trait**: a template-based interface to query or modify the properties of types.

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
Once C++20 is available, constraints will be added to improve error reporting. 

</details>

<details><summary><b>Background</b>: P0172R0 Abominable Function Types</summary>

Quoting from [P0172R0](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0172r0.html) section 2.1, Definition:

>[...] an *abominable* function type is the type produced by writing a function type  
followed by a cv-ref qualifier.

Example:

```cpp
   using regular    = void();
   using abominable = void() const volatile &&;
```

>In the example above, `regular` names a familiar function type [...] no surprises,  
`abominable` also names a function type, not a reference type, and  
despite appearances, is neither a const nor a volatile qualified type.  
There is no such thing as a cv-qualified function type in the type system,  
and the abominable function type is something else entirely.

* P0172R0 **Abominable Function Types** by Alisdair Meredith, Nov 2015

</details>

<details><summary>Boost.CallableTraits: A P0172 implementation and more</summary>

>[Boost.CallableTraits](https://www.boost.org/doc/libs/develop/libs/callable_traits/doc/html/) implements P0172R0's suggested library interface,  
extended to support general [Callable](https://en.cppreference.com/w/cpp/named_req/Callable) types on top of C++ function types.  
It is a robust, reviewed library with tests, compatibility matrix and CI.

</details>

<details><summary><b>Motivation</b>: Provide the 24 (or 48) required specializations</summary>

See also [Boost.CallableTraits Motivation](https://www.boost.org/doc/libs/develop/libs/callable_traits/doc/html/index.html#callable_traits.introduction.motivation)

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
>### Copy traits
>I wanted traits to copy qualifiers from source to target function types<sup>1</sup>.  
Since all 24/48 specializations are needed to implement *any* function trait  
with full generality, one might as well write a full collection of traits.
>
>[1] C++ standard library `copy_*` traits are proposed in [P1016](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1016r0.pdf)  
"A few additional type manipulation utilities" (unlikely to be in C++20).  
Copy traits like this are not implemented yet in Boost.CallableTraits  
though there's an open [issue](https://github.com/boostorg/callable_traits/issues/139) to add a `copy_member_cvref` trait.
>
>This function_traits library provides a couple of options:  
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

    **Minimal**: avoid bloat and duplication in the interface (not that easy).  
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

## [Reference | Design Notes](reference.md)

## **Examples**

<details><summary>Getting started</summary>

>First, put the header file where you want it and configure your include path.  
Here, the `ltl` include directory reflects `function_traits` namespace, `ltl`:

```c++
#include <ltl/function_traits.hpp>
```

Or, just cut and paste the header 

All `function_traits` are defined only for function types; it is an error to  
instantiate a function trait with a non-function type template parameter.  
This is reflected in the naming, with 'function' always first; `function_*`.

```c++
  ltl::function_is_noexcept_v< int > // compile error
```

To use function traits with non-function types, guard the trait instantiation  
with an `if constexpr` `std::is_function_v` block (or overload with constraints). 

>(Using a function trait with a non function type gives a hard, non-SFINAE, error  
with a nasty error message from the compiler.)

</details>

<details><summary>Predicate traits and type property traits</summary>

>First, put the header file where you want it and configure your include path.  
Here, the `ltl` include directory reflects `function_traits` namespace, `ltl`:

```c++
#include <ltl/function_traits.hpp>
```

All `function_traits` are defined only for function types; it is an error to  
instantiate a function trait with a non-function type template parameter.  
This is reflected in the naming, with 'function' always first; `function_is_*`.  
To use function traits with non-function types you must guard with `is_function`. 

>(Using a function trait with a non function type gives a hard, non-SFINAE, error  
with a nasty error message from the compiler.)
>
>Test if a function type is const / cvref / noexcept:  

```c++
  using Fc = void() const noexcept;

  static_assert(
          ltl::function_is_const_v< Fc >
       && ltl::function_is_cvref_v< Fc >
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
                   , std::tuple< char, bool(*)() >
     >
  );
```

>A standalone predicate trait is provided to check that a function type  
is a valid free function type, i.e. it has no 'abominable' cvref qualifiers:  

```c++
  void f() noexcept;

  static_assert( ltl::is_free_function_v<decltype(f)> );
```

`is_free_function` is defined for all types (it is `false` for non-function types):

For example, `is_free_function_v` is :

```c++
template <typename F>
inline constexpr bool is_free_function_v = []{
         if constexpr(std::is_function_v<F>)
           return !function_is_cvref_v<F>;
         return false; }();
```

</details>

<details><summary>Modifying traits; add / remove and set traits</summary>

>Conventional `add_*`, `remove_*` traits modify the given property `*`.  
They take no arguments beyond the function type to modify:

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

>The `set` trait for reference qualifiers takes a `ltl::ref_qual_v` argument  
(an enum type with values `lval_ref_v`, `rval_ref_v` or `ref_qual_v{}`)

```c++
    static_assert(
       std::is_same_v< function_set_reference_t<void() &, rval_ref_v>
                                                void() && >
    );
```

>There is a `function_add_reference<F,R>` trait that does reference collapse  
such that 'adding' an rvalue-ref to an lvalue-ref gives an lvalue-ref, consistent with  
`std::add_rvalue_reference` for object types, `&` + `&&` => `&`, even though  
reference collapse is not necessarily natural for function reference qualifiers.
>
>There are no `function_add_reference_lvalue` / `_rvalue` traits.  
Instead, there are `function_set_reference_lvalue` / `_rvalue` traits  
(read as "given a function type, set its reference qualifier to lvalue reference")  
(unlike other setters, but like `add` traits, they take no arguments).
>
>Setters for type properties take type arguments; to change function return type:

```c++
    static_assert(
       std::is_same_v< function_set_return_type_t<int(), void>,
                                                  void() >);
```

>`set_*_as` traits provide a way to copy properties to the target function type  
from a source function type template argument - e.g. to copy cvref qualifiers:

```c++
    static_assert( std::is_same_v<
                     function_set_cvref_as_t<void() const, int() &>,
                                             void() & >);
```

</b></details>

<details><summary>A small example of function_traits usage</summary>

An illustrative example that type-checks `printf`-like member functions that may  
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
