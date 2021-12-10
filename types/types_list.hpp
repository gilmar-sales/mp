#pragma once

#include <utility>
#include <tuple>

#if defined(__linux__) || defined(__MINGW32__)
    #include <experimental/tuple>
#endif // __linux__

#include "../iteration/static_for.hpp"

namespace mp {
    namespace impl {

        //type_list
        template<typename...Ts>
        struct type_list;

        template<typename T>
        struct type_list<T> {
            using type = T;
            static constexpr int index = 0;
        };

        template<typename T, typename... Ts>
        struct type_list<T, Ts...> : type_list<Ts...>  {
            using type = T;
            static constexpr int index = type_list<Ts...>::index + 1;
        };

        //tuple index_of 
        template <class T, class Tuple>
        struct index_of;

        template <typename T>
        struct index_of<T, std::tuple<>> {
            static constexpr std::size_t value = -1;
        };

        template <class T, class... Ts>
        struct index_of<T, std::tuple<T, Ts...>> {
            static constexpr std::size_t value = 0;
        };

        template <class T, class U, class... Ts>
        struct index_of<T, std::tuple<U, Ts...>> {
            static constexpr std::size_t value = 1 + index_of<T, std::tuple<Ts...>>::value;
        };

        //tuple size_of

        template<typename...Ts>
        struct size_of;

        template<typename T>
        struct size_of<std::tuple<T>> {
            static constexpr int value = 1;
        };

        template<typename T, typename... Ts>
        struct size_of<std::tuple<T, Ts...>> : size_of<std::tuple<Ts...>>  {
            static constexpr int value = size_of<std::tuple<Ts...>>::value + 1;
        };

        //tuple contains
        template <typename T, typename Tuple>
        struct contains;

        template <typename T>
        struct contains<T, std::tuple<>> : std::false_type {};

        template <typename T, typename U, typename... Ts>
        struct contains<T, std::tuple<U, Ts...>> : contains<T, std::tuple<Ts...>> {};

        template <typename T, typename... Ts>
        struct contains<T, std::tuple<T, Ts...>> : std::true_type {};


        //make a type_list be a tuple
        template<class List> struct as_tuple;

        template<template<class...> class List, class... Elements>
        struct as_tuple<List<Elements...>> {
            using type = std::tuple<Elements...>;
        };


        //unwrap a list to another container
        template<template<class...> class Container, class List>
        struct rename;

        template<template<class...> class Container,
            template<class...> class List,
            class... Elements
        >
        struct rename<Container, List<Elements...>> { using type = Container<Elements...>; };


        //generate a tuple of N Ts
        template<typename T, size_t N, typename... Ts>
        struct generate_tuple
        {
            using type = typename generate_tuple<T, N-1, T, Ts...>::type; 
        };

        template<typename T, typename... Ts>
        struct generate_tuple<T, 0, Ts...>
        {
            using type = std::tuple<Ts...>;
        };

        //tuple_from_func_args
        template<typename F, typename... Args>
        struct extract_func_args;

        template<typename F, typename... Args>
        struct extract_func_args<F(Args...)>
        {
            using tuple = std::tuple<Args...>;
        };
    }

    template<typename... Ts>
    using type_list = impl::type_list<Ts...>;
    
    template<typename T, size_t N>
    using generate_tuple = typename impl::generate_tuple<T, N>::type;

    template<typename T>
    using extract_func_args = typename impl::extract_func_args<T>::tuple;

    template<template<class...> class Container, class List>
    using rename = typename impl::rename<Container, List>::type;

    template <typename T, typename Tuple>
    using contains = typename impl::contains<T, Tuple>::type;

    template<typename T, typename Tuple>
    constexpr size_t index_of() {
        return impl::index_of<T, Tuple>::value;
    }

    template<typename Tuple>
    constexpr size_t size_of() {
        return impl::size_of<Tuple>::value;
    }

    template <typename TF, typename TTuple>
    void for_tuple(TF&& f, TTuple&& t)
    {
        auto adapted = [f = FWD(f)](auto&&... xs)
        {
            for_args(f, FWD(xs)...);
        };

        #if defined(__linux__) || defined(__MINGW32__)
            std::experimental::apply(adapted, FWD(t));
        #else
            std::apply(adapted, FWD(t));
        #endif // __linux__
    }

}