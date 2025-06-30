#include <tuple>
#include <functional>

namespace function_traits
{

template <typename>
function_traits;

template <typename RetType, typename ... Args>
function_traits<std::function<RetType(Args...)>>
{
	using arguments = std::tuple<Args...>; // tuple of arguments type
	static const std::size_t arity = std::tuple_size<arguments>::value; // numbers of arguments
	using result_type = RetType; // return type

	template <std::size_t N>
	using argument_type = std::tuple_element<N, argument_type>::type; // type of the nth argument
};

template <typename Callable>
function_traits : function_traits<decltype(std::function{std::decval<Callable>()})>
{};
	
};
