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


|function_trait\<F\>  | P0172R0 / callable_trait\<F\>  |
|----|----|
|`function_is_const<F>`|`is_const_member<F>`|
|`function_is_volatile<F>`|`is_volatile_member<F>`|
|`function_is_cv<F>` (Note: const *OR* volatile)| `is_cv_member<F>` (Note: const *AND* volatile)|
|`function_is_lvalue_reference<F>`|`is_lvalue_reference_member<F>`|
|`function_is_rvalue_reference<F>`|`is_rvalue_reference_member<F>`|
|`function_is_reference<F>`|`is_reference_member<F>`|
|`function_is_cvref<F>`|`has_member_qualifiers<F>`|
|`function_is_noexcept<F>`|`is_noexcept<F>`|
|`function_is_variadic<F>`|`has_varargs<F>`|




---
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
