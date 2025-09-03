#pragma once

#include <tuple>
#include <functional>
#include <type_traits>

namespace mylib
{
namespace function_traits
{

template <typename T>
struct function_traits;

template <typename R, typename... Args>
struct function_traits<R(Args...)>
{
    static const std::size_t arity = sizeof...(Args);
    using arguments = std::tuple<Args...>;
    using return_type = R;

    template <std::size_t N>
    using argument_type = typename std::tuple_element<N, arguments>::type;
};

// Free function pointers
template <typename R, typename... Args>
struct function_traits<R(*)(Args...)> : function_traits<R(Args...)>
{};

// Function reference
template <typename R, typename... Args>
struct function_traits<R(&)(Args...)> : function_traits<R(Args...)>
{};

// Member function pointer (non-const)
template <typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...)> : function_traits<R(C&, Args...)> {};

// Member function pointer (const)
template <typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...) const> : function_traits<R(const C&, Args...)> {};

// Member function pointer (volatile)
template <typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...) volatile> : function_traits<R(volatile C&, Args...)> {};

// Member function pointer (const volatile)
template <typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...) const volatile> : function_traits<R(const volatile C&, Args...)> {};

// Remove the first item in a tuple
template<typename T>
struct tuple_tail;

template<typename Head, typename ... Tail>
struct tuple_tail<std::tuple<Head, Tail ...>>
{
  using type = std::tuple<Tail ...>;
};

// Functors (with operator() const)
template <typename FunctionT>
struct function_traits
{
    using arguments = typename tuple_tail<
            typename function_traits<decltype( &FunctionT::operator())>::arguments
        >::type;

    static constexpr std::size_t arity = std::tuple_size<arguments>::value;

    template<std::size_t N>
    using argument_type = typename std::tuple_element<N, arguments>::type;

    using return_type = typename function_traits<decltype( &FunctionT::operator())>::return_type;
};

template <typename FunctionT>
struct function_traits<FunctionT&>
    : function_traits<FunctionT>
{};

template <typename FunctionT>
struct function_traits<FunctionT&&>
    : function_traits<FunctionT>
{};

// Copied from ros2 to handle std::bind and some useful utilities

// Copyright 2014 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// std::bind for object methods
template<typename ClassT, typename ReturnTypeT, typename ... Args, typename ... FArgs>
#if defined DOXYGEN_ONLY
struct function_traits<std::bind<ReturnTypeT (ClassT::*)(Args ...), FArgs ...>>
#elif defined _LIBCPP_VERSION  // libc++ (Clang)
struct function_traits<std::__bind<ReturnTypeT (ClassT::*)(Args ...), FArgs ...>>
#elif defined _GLIBCXX_RELEASE  // glibc++ (GNU C++ >= 7.1)
struct function_traits<std::_Bind<ReturnTypeT(ClassT::* (FArgs ...))(Args ...)>>
#elif defined __GLIBCXX__  // glibc++ (GNU C++)
struct function_traits<std::_Bind<std::_Mem_fn<ReturnTypeT (ClassT::*)(Args ...)>(FArgs ...)>>
#elif defined _MSC_VER  // MS Visual Studio
struct function_traits<
    std::_Binder<std::_Unforced, ReturnTypeT (ClassT::*)(Args ...), FArgs ...>>
#else
#error "Unsupported C++ compiler / standard library"
#endif
    : function_traits<ReturnTypeT(Args ...)>
{};

// std::bind for object const methods
template<typename ClassT, typename ReturnTypeT, typename ... Args, typename ... FArgs>
#if defined DOXYGEN_ONLY
struct function_traits<std::bind<ReturnTypeT (ClassT::*)(Args ...) const, FArgs ...>>
#elif defined _LIBCPP_VERSION  // libc++ (Clang)
struct function_traits<std::__bind<ReturnTypeT (ClassT::*)(Args ...) const, FArgs ...>>
#elif defined _GLIBCXX_RELEASE  // glibc++ (GNU C++ >= 7.1)
struct function_traits<std::_Bind<ReturnTypeT(ClassT::* (FArgs ...))(Args ...) const>>
#elif defined __GLIBCXX__  // glibc++ (GNU C++)
struct function_traits<std::_Bind<std::_Mem_fn<ReturnTypeT (ClassT::*)(Args ...) const>(FArgs ...)>>
#elif defined _MSC_VER  // MS Visual Studio
struct function_traits<
    std::_Binder<std::_Unforced, ReturnTypeT (ClassT::*)(Args ...) const, FArgs ...>>
#else
#error "Unsupported C++ compiler / standard library"
#endif
    : function_traits<ReturnTypeT(Args ...)>
{};

// std::bind for free functions
template<typename ReturnTypeT, typename ... Args, typename ... FArgs>
#if defined DOXYGEN_ONLY
struct function_traits<std::bind<ReturnTypeT( &)(Args ...), FArgs ...>>
#elif defined _LIBCPP_VERSION  // libc++ (Clang)
struct function_traits<std::__bind<ReturnTypeT( &)(Args ...), FArgs ...>>
#elif defined __GLIBCXX__  // glibc++ (GNU C++)
struct function_traits<std::_Bind<ReturnTypeT(*(FArgs ...))(Args ...)>>
#elif defined _MSC_VER  // MS Visual Studio
struct function_traits<std::_Binder<std::_Unforced, ReturnTypeT( &)(Args ...), FArgs ...>>
#else
#error "Unsupported C++ compiler / standard library"
#endif
    : function_traits<ReturnTypeT(Args ...)>
{};

template <std::size_t arity, typename Callable>
struct arity_comparator : std::integral_constant<bool, arity == function_traits<Callable>::arity>
{};

template <typename Callable, typename ... Args>
struct check_arguments : std::is_same<
    typename function_traits<Callable>::arguments, std::tuple<Args...>
>
{};

template <typename CallableA, typename CallableB>
struct same_arguments : std::is_same<
    typename function_traits<CallableA>::arguments,
    typename function_traits<CallableB>::arguments
>
{};

namespace detail
{

template <typename R, typename ... Args>
struct as_std_function_helper;

template <typename R, typename ... Args>
struct as_std_function_helper<R, std::tuple<Args...>>
{
    using type = std::function<R(Args...)>;
};

}; // namespace detail

template <
    typename FunctionT,
    typename FunctionTraits = function_traits<FunctionT>
>
struct as_std_function
{
    using type = typename detail::as_std_function_helper<
        typename FunctionTraits::return_type,
        typename FunctionTraits::arguments
    >::type;
};

// ------------------------------------------------------------------

}; // namespace function_traits
}; // namespace mylib
