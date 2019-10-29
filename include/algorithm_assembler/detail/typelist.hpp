#ifndef TYPELIST_HPP
#define TYPELIST_HPP

#include "typelist_functions.hpp"
#include "heterogeneous_container_functions.hpp"

namespace algorithm_assembler::typelist::detail
{
	template<typename... Ts>
	struct Common_members
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
		constexpr static bool contains = container_functions::contains_v<Typelist<Ts...>, T>;
	};
}

namespace algorithm_assembler::typelist
{
	template<typename...> struct Typelist;

	template<> 
	struct Typelist<> : public detail::Common_members<> 
	{
		static constexpr bool is_empty = true;
	};

	template<typename H, typename... Ts>
	struct Typelist<H, Ts...> : public detail::Common_members<H, Ts...>
	{
	private:
		using M = typename detail::Common_members<H, Ts...>;
	public:
		template <size_t Index>
		using at = container_functions::type_at_t<Typelist<H, Ts...>, Index>;

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
