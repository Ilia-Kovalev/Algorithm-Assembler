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

#ifndef TYPELIST_FUNCTIONS_HPP
#define TYPELIST_FUNCTIONS_HPP


#include "heterogeneous_container_functions.hpp"
#include "typelist.hpp"

namespace algorithm_assembler::typelist
{
	template<typename... Ts>
	struct Typelist;
}


namespace algorithm_assembler::typelist::detail
{
	template<class Node, typename...>
	struct to_nested_containers;

	template<template<typename...> class Node, typename T, typename... Ts>
	struct to_nested_containers<Node<>, T, Ts...>
	{
		using tail = typename to_nested_containers<Node<>, Ts...>::node;
		using node = container_functions::concatenation_t<Node<T>, tail>;
	};

	template<template<typename...> class Node, typename... Ts, typename... Listed_types>
	struct to_nested_containers<Node<>, Typelist<Listed_types...>, Ts...>
	{
		using tree_from_listed = typename to_nested_containers<Node<>, Listed_types...>::node;
		using tail = typename to_nested_containers<Node<>, Ts...>::node;
		using node = container_functions::concatenation_t<Node<tree_from_listed>, tail>;
	};

	template<template<typename...> class Node>
	struct to_nested_containers<Node<>>
	{
		using node = Node<>;
	};

	template<template<typename...> class Node, typename... Ts>
	using to_nested_containers_t = typename to_nested_containers<Node<>, Ts...>::node;



	template<size_t N, typename Typelist>
	using type_list_element_t = typename Typelist::template type<N>;



	template<typename Typelist>
	struct flatten;

	template<typename Typelist>
	using flatten_t = typename flatten<Typelist>::type;

	template<typename H, typename... T>
	struct flatten<Typelist<H, T...>>
	{
		using type = container_functions::concatenation_t<
			Typelist<H>,
			flatten_t<Typelist<T...>>
		>;
	};

	template<typename... Ts, typename... T>
	struct flatten<Typelist<
		Typelist<Ts...>, T...>
	>
	{
		using type = container_functions::concatenation_t<
			flatten_t< Typelist<Ts...>>,
			flatten_t< Typelist<T...>>
		>;
	};

	template<>
	struct flatten<Typelist<>>
	{
		using type = Typelist<>;
	};
}


#endif 

