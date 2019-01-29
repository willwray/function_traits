#include <tuple>
#include "function_traits.hpp"

struct Log0 { int log(char const* fmt) const noexcept; };
struct LogV { int log(char const* fmt,...) const & noexcept; };

template <class C, typename F, typename... Vargs>
int logger(F C::* log_mf, Vargs... vargs)
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
template int logger(decltype(&LogV::log), int);
// Compile fail; variadic logger without any forwarded varargs
//template int logger(decltype(&LogV::log));

int main()
{
	return 0;
}