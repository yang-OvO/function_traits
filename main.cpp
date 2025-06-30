#include "rclcpp_function_traits.hpp"

#include <iostream>

// using namespace function_traits;
using namespace rclcpp::function_traits;

template<class Callable>
void fun(Callable&&)
{
	size_t size = function_traits<Callable>::arity;
	std::cout << "Callable with " << size << " params \n";
	std::cout << __PRETTY_FUNCTION__ << "\n";
	std::cout << "--------------------------------------\n";
}

void foo(int, long, char, float) {}

struct S { void operator()(bool, float, double) {} };

struct Bar
{
	void f(double, int) {}
};


int main(int argc, char **argv)
{
	(void)argc, (void)(argv);
	using namespace std::placeholders;

	fun([](){});
	fun([](int){});
	std::function<void(char, char)> f{[](char, char){}};
	fun(f);
	fun(foo);
	fun(&foo);
	fun(std::bind(&foo, _1, _2, _3, _4));
	fun(S{});
	fun(fun<decltype(foo)>);
	Bar b;
	fun(std::bind(&Bar::f, &b, _1, _2));

	static_assert(arity_comparator<4, decltype(foo)>::value, "arguments number wrong");
	static_assert(check_arguments<decltype(f), char, char>::value, "arguments not matching");
	static_assert(same_arguments<decltype(std::bind(&Bar::f, &b, _1, _2)), void(double, int)>::value, "arguments not matching");

}

