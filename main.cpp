#include <iostream>

#include "function_traits.hpp"
using namespace mylib::function_traits;

template<class Callable>
void fun(Callable&&)
{
	constexpr size_t size = function_traits<Callable>::arity;
	std::cout << "Callable with " << size << " params \n";

#if defined(__GNUC__) || defined(__clang__)
	std::cout << __PRETTY_FUNCTION__ << "\n";
#elif defined(_MSC_VER)
	std::cout << __FUNCSIG__ << "\n";
#endif
	std::cout << "--------------------------------------\n";
}

void foo(int, long, char, float) {}

struct S { void operator()(bool, float, double) const{} };

struct Bar
{
    void f(double, int) const {}
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
	fun(std::bind(foo, _1, _2, _3, _4));
	fun(S{});
	fun(fun<decltype(foo)>);
	Bar b;
	fun(&Bar::f);
	fun(std::bind(&Bar::f, &b, _1, _2));

	auto lambda = [](int, float){};
	static_assert(arity_comparator<2, decltype(lambda)>::value, "arguments number is wrong");
	static_assert(arity_comparator<4, decltype(foo)>::value, "arguments number iswrong");
	static_assert(check_arguments<decltype(f), char, char>::value, "arguments not matching");
	static_assert(same_arguments<decltype(std::bind(&Bar::f, &b, _1, _2)), void(double, int)>::value, "arguments not matching");

}
