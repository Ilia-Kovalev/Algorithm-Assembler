/*
Copyright 2019 Ilia S. Kovalev

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef HETEROGENEOUS_CONTAINER_FUNCTIONS_HPP
#define HETEROGENEOUS_CONTAINER_FUNCTIONS_HPP

#include <type_traits>

namespace algorithm_assembler::utils
{
	template<class... Containers>
	struct concatenation;

	template<typename... Ts1, typename... Ts2, template<typename...> class C, class C3, class... Containers>
	struct concatenation<C<Ts1...>, C<Ts2...>, C3, Containers...>
	{
		using type = typename concatenation<C<Ts1..., Ts2...>, C3, Containers...>::type;
	};

	template<typename... Ts1, typename... Ts2, template<typename...> class C>
	struct concatenation<C<Ts1...>, C<Ts2...>>
	{
		using type = C<Ts1..., Ts2...>;
	};

	template<class Container>
	struct concatenation<Container>
	{
		using type = Container;
	};

	template<class... Containers>
	using concatenation_t = typename concatenation<Containers...>::type;


	template<typename Container, size_t Index>
	struct at;

	template<typename Container, size_t Index>
	using type_at_t = typename at<Container, Index>::type;

	template<typename H, typename... Ts, template<typename...> class C>
	struct at<C<H, Ts...>, 0>
	{
		using type = H;
	};


	template<size_t N, typename H, typename... Ts, template<typename...> class C>
	struct at<C<H, Ts...>, N>
	{
		static_assert(N < sizeof...(Ts) + 1, "index out of bounds");
		
		using type = type_at_t<C<Ts...>, N - 1>;
	};


	template<typename, typename...>
	struct push_front;

	template<typename... Ts1, typename... Ts2, template<typename...> class C>
	struct push_front<C<Ts1...>, Ts2...>
	{
		using type = C<Ts2..., Ts1...>;
	};

	template<typename Container, typename... Types>
	using push_front_t = typename push_front<Container, Types...>::type;



	template<typename, typename...>
	struct push_back;

	template<typename... Ts1, typename... Ts2, template<typename...> class C>
	struct push_back<C<Ts1...>, Ts2...>
	{
		using type = C<Ts1..., Ts2...>;
	};

	template<typename Container, typename... Types>
	using push_back_t = typename push_back<Container, Types...>::type;


	template<typename Container, typename Type, typename = void>
	struct contains : public std::false_type {};

	template<typename Type, typename H, typename... T, template<typename...> class Container>
	struct contains < Container<H, T...>, Type,
		std::enable_if_t<std::is_same_v<Type, H>>
	> : public std::true_type {};

	template<typename Type, typename H, typename... T, template<typename...> class Container>
	struct contains <Container<H, T...>, Type,
		std::enable_if_t<!std::is_same_v<Type, H>>
	> : public contains<Container<T...>, Type> {};

	template<typename Container, typename Type>
	constexpr bool contains_v = contains<Container, Type>::value;



	template<typename Container>
	struct tail;

	template<typename Container>
	using tail_t = typename tail<Container>::type;

	template<typename H, typename... T, template<typename...> class Container>
	struct tail<Container<H, T...>>
	{
		using type = Container<T...>;
	};

	template<template<typename...> class Container>
	struct tail<Container<>>
	{
		using type = Container<>;
	};


	template<typename Container, typename T, typename = void>
	struct remove;

	template<typename Container, typename T>
	using remove_t = typename remove<Container, T>::type;

	template<template<typename...> class Container, typename Type, typename H, typename... T>
	struct remove<Container<H, T...>, Type,
		std::enable_if_t<std::is_same_v<H, Type>>
	>
	{
		using type = remove_t<Container<T...>, Type>;
	};

	template<template<typename...> class Container, typename Type, typename H, typename... T>
	struct remove<Container<H, T...>, Type,
		std::enable_if_t<!std::is_same_v<H, Type>>
	>
	{
		using type = concatenation_t <
			Container<H>,
			remove_t<Container<T...>, Type>>;
	};

	template<template<typename...> class Container, typename Type>
	struct remove<Container<>, Type>
	{
		using type = Container<>;
	};



	template<typename Container>
	struct unique;

	template<typename Container>
	using unique_t = typename unique<Container>::type;

	template<typename H, typename... T, template<typename...> class Container>
	struct unique<Container<H, T...>>
	{
		using type = push_front_t<unique_t<remove_t<Container<T...>, H>>, H>;
	};

	template<template<typename...> class Container>
	struct unique<Container<>>
	{
		using type = Container<>;
	};


	template<typename Type, class Container, size_t i = 0, typename = void>
	struct index;

	template<typename Type, class Container>
	constexpr size_t index_v = index<Type, Container>::value;

	template<typename Type, typename H, typename... T, template<typename...> class Container, size_t i>
	struct index<Type, Container<H, T...>, i,
		std::enable_if_t<!std::is_same_v<Type, H>>
	> : public index<Type, Container<T...>, i + 1> {};

	template<typename Type, typename H, typename... T, template<typename...> class Container, size_t i>
	struct index<Type, Container<H, T...>, i,
		std::enable_if_t<std::is_same_v<Type, H>>
	> : public std::integral_constant<size_t, i> {};



	template<class Container, typename Function>
	struct map;

	template<typename... Content, template <typename> class F, template<typename...> class Container, typename T>
	struct map<Container<Content...>, F<T>>
	{
		using type = Container<typename F<Content>::type...>;
	};

	template<class Container, class Function>
	using map_t = typename map<Container, Function>::type;



	template<class Container, typename Type, typename = void>
	struct drop_while_type;

	template<class Container, typename Type>
	using drop_while_type_t = typename drop_while_type<Container, Type>::type;

	template<typename Type, template<typename...> class Container, typename H, typename... T>
	struct drop_while_type<Container<H, T...>, Type,
		std::enable_if_t<std::is_same_v<Type, H>>
	>
	{
		using type = Container<H, T...>;
	};

	template<typename Type, template<typename...> class Container, typename H, typename... T>
	struct drop_while_type<Container<H, T...>, Type,
		std::enable_if_t<!std::is_same_v<Type, H>>
	>
	{
		using type = drop_while_type_t<Container<T...>, Type>;
	};


	template<class Intersection, class Container1, class Container2, typename = void> struct intersection_impl;

	template<
		class Intersection,
		template<typename...> class Container1,
		typename H, typename... Ts,
		class Container2
	>
		struct intersection_impl<Intersection, Container1<H, Ts...>, Container2, 

		std::enable_if_t<contains_v<Container2, H>>
		>
	{
		using type = typename intersection_impl<push_back_t<Intersection, H>, Container1<Ts...>, Container2>::type;
	};

	template<
		class Intersection,
		template<typename...> class Container1,
		typename H, typename... Ts,
		class Container2
	>
		struct intersection_impl<Intersection, Container1<H, Ts...>, Container2,

		std::enable_if_t<!contains_v<Container2, H>>
		>
	{
		using type = typename intersection_impl<Intersection, Container1<Ts...>, Container2>::type;
	};

	template<
		class Intersection,
		template<typename...> class Container1,
		class Container2
	>
		struct intersection_impl<Intersection, Container1<>, Container2
		>
	{
		using type = typename Intersection;
	};


	template<class Container, class... Containers> struct intersection;

	template<template<typename...> class Container1, class Container2, class... Containers, typename... Ts>
	struct intersection<Container1<Ts...>, Container2, Containers...>
	{
		using current_intersection = typename intersection_impl<Container1<>, Container1<Ts...>, Container2>::type;
		using type = typename intersection<current_intersection, Containers...>::type;
	};

	template<class Container>
	struct intersection<Container>
	{
		using type = Container;
	};
	
	template<class Container, class... Containers>
	using intersection_t = typename intersection<unique_t<Container>, Containers...>::type;
}



#endif
