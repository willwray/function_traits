## General form of traits

A trait can be a global 'standalone' trait or a member trait of `function<F>` 

The predicate member traits are aliases of `bool_constant<?>`
<br>(so they are equal to `true_type` or `false_type` depending on `?`).
<br>The bool value itself can be extracted from the `bool_constant`
<br>via its `value` member or by invoking its function call operator

| function\<F\> member trait<hr>function_trait\<F\> standalone | <br>P0172R0 / callable_trait\<F\> standalone |
|----|----|
|`function<F>::is_*` == `bool_constant<?>`<br>`function<F>::is_*()`= `bool{?}`<hr>`function_is_*<F> : bool_constant<?>`<br>`function_is_*_v<F>` = `bool{?}`|<br><br>`is_*_member<F> : bool_constant<?>`<br>`is_*_member_v<F>` = `bool{?}`|

## **Predicate** traits
The predicate traits test true/false properties of function types.
<br>They are provided as member traits as well as standalone traits.


| function\<F\> member trait<hr>function_trait\<F\> standalone | <br>P0172R0 / callable_trait\<F\> standalone |
|----|----|
|`function<F>::is_const`<br>`function_is_const<F>`|<br>`is_const_member<F>`|
|`function<F>::is_volatile`<br>`function_is_volatile<F>`|<br>`is_volatile_member<F>`|
|`function<F>::is_cv` (Note: const *OR* volatile)<br>`function_is_cv<F>`| (Note: const *AND* volatile)<br>`is_cv_member<F>`|
|`function<F>::is_lvalue_reference`<br>`function_is_lvalue_reference<F>`|<br>`is_lvalue_reference_member<F>`|
|`function<F>::is_rvalue_reference`<br>`function_is_rvalue_reference<F>`|<br>`is_rvalue_reference_member<F>`|
|`function<F>::is_reference`<br>`function_is_reference<F>`|<br>`is_reference_member<F>`|
|`function<F>::is_cvref`<br>`function_is_cvref<F>`|<br>`has_member_qualifiers<F>`|
|`function<F>::is_qualified` (cvref or noexcept)<br>`function_is_qualified<F>`||
|`function<F>::is_noexcept`<br>`function_is_noexcept<F>`|<br>`is_noexcept<F>`|
|`function<F>::is_variadic`<br>`function_is_variadic<F>`|<br>`has_varargs<F>`|
||||
|`function_set_const<F>`<br>`function_set_const<F,true>`<br>`function<F>::template set_const<true>`|`add_member_const<F>`|
|`function_set_cv<F>`|`add_member_cv<F>`|
|`function_set_lvalue_reference<F>`|`add_member_lvalue_reference<F>`|
|`function_set_rvalue_reference<F>`|`add_member_rvalue_reference<F>`|
|`function_set_volatile<F>`|`add_member_volatile<F>`|
|`function_set_noexcept<F>`|`add_noexcept<F>`|
||`add_transaction_safe<F>`|
|`add_varargs`|`add_varargs<F>`|
|`apply_member_pointer`|`apply_member_pointer<F>`|
|`apply_return`|`apply_return<F>`|
|`args`|`args<F>`|
|`class_of`|`class_of<F>`|
|`function_type`|`function_type<F>`|
|`qualified_class_of`|`qualified_class_of<F>`|
|`remove_member_const`|`remove_member_const<F>`|
|`remove_member_cv`|`remove_member_cv<F>`|
|`remove_member_reference`|`remove_member_reference<F>`|
|`remove_member_volatile`|`remove_member_volatile<F>`|
|`remove_noexcept`|`remove_noexcept<F>`|
|`remove_transaction_safe`|`remove_transaction_safe<F>`|
|`remove_varargs`|`remove_varargs<F>`|
|`return_type`|`return_type<F>`|

