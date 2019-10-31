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

#ifndef TYPELIST_HPP
#define TYPELIST_HPP

#include "typelist_functions.hpp"
#include "heterogeneous_container_functions.hpp"


namespace algorithm_assembler::utils::typelist_detail
{
	template<typename... Ts>
	struct Typelist_base
	{
		template<template<typename...> class Container>
		using values_container = typename to_nested_containers<Container<>, Ts...>::node;

		constexpr static std::size_t size = sizeof...(Ts);

		template<typename... New_types>
		using push_back = Typelist<Ts..., New_types...>;

		template<typename... New_types>
		using push_front = Typelist<New_types..., Ts...>;

		using flat = flatten_t<Typelist<Ts...>>;

		template<typename T>
		constexpr static bool contains = contains_v<Typelist<Ts...>, T>;
	};
}

namespace algorithm_assembler::utils
{
	template<typename...> struct Typelist;

	template<> 
	struct Typelist<> : public typelist_detail::Typelist_base<>
	{
		static constexpr bool is_empty = true;
	};

	template<typename H, typename... Ts>
	struct Typelist<H, Ts...> : public typelist_detail::Typelist_base<H, Ts...>
	{
	private:
		using M = typename typelist_detail::Typelist_base<H, Ts...>;
	public:
		template <size_t Index>
		using at = type_at_t<Typelist<H, Ts...>, Index>;

		using head = H;
		using back = at<M::size - 1>;
		using tail = Typelist<Ts...>;

		static constexpr bool is_empty = false;
	};

	template <typename T>
	struct is_type_list : std::false_type {};
	template <typename... Ts>
	struct is_type_list<Typelist<Ts...>> : std::true_type {};
	template<class T>
	inline constexpr bool is_type_list_v = is_type_list<T>::value;
}

#endif
