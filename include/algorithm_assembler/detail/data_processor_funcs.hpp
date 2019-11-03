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

#ifndef DATA_PROCESSOR_FUNCS
#define DATA_PROCESSOR_FUNCS

#include "../utils/tuple.hpp"

namespace algorithm_assembler::detail
{
	template<typename T, typename Tuple, typename = std::enable_if_t<utils::is_tuple_v<Tuple>>>
	inline T&& tuple_get_wrapper(Tuple&& t)
	{
		if constexpr (utils::contains_v<Tuple, T>)
			return std::get<T>(std::forward<Tuple>(t));

		if constexpr (utils::contains_v<Tuple, std::remove_reference_t<T>>)
			return std::get<std::remove_reference_t<T>>(std::forward<Tuple>(t));

		if constexpr (utils::contains_v<Tuple, std::remove_const_t<std::remove_reference_t<T>>>)
			return std::get<std::remove_const_t<std::remove_reference_t<T>>>(std::forward<Tuple>(t));

		if constexpr (utils::contains_v<Tuple, std::add_lvalue_reference_t<std::remove_const_t<std::remove_reference_t<T>>>>)
			return std::get<std::add_lvalue_reference_t<std::remove_const_t<std::remove_reference_t<T>>>>(std::forward<Tuple>(t));

		if constexpr (utils::contains_v<Tuple, std::add_lvalue_reference_t<T>>)
			return std::get<std::add_lvalue_reference_t<T>>(std::forward<Tuple>(t));
	}

	template<class F, typename Input>
	inline auto process_though_functor(F& f, Input&& in) -> typename F::Output_type
	{
		return f(std::forward<Input>(in));
	}

	template<class F, typename... Inputs, typename... F_ins>
	inline auto process_though_functor(
		F& f,
		std::tuple<Inputs...>&& in_tuple,
		utils::Typelist<F_ins...>&&
	) -> typename F::Output_type
	{
		using In_tuple = std::tuple<Inputs...>;

		return f(std::forward<std::remove_reference_t<F_ins>>(
			tuple_get_wrapper<F_ins>(std::forward<In_tuple>(in_tuple))
			)...);
	}

	template<class F, class... Fs>
	inline auto process_data(F& f, Fs&... tail)
	{
		if constexpr (sizeof...(tail) > 0)
			return process_data(f(), tail...);
		else
			return f();
	}

	template<typename Input, class F, class... Fs, 
		typename = std::enable_if_t<!std::is_base_of_v<Functor, std::remove_reference_t<Input>>>
	>
	inline auto process_data(Input&& in, F& f, Fs&... tail)
	{
		auto result = [&f, &in]()
		{
			if constexpr (utils::is_tuple_v<std::remove_reference_t<Input>>)
				return process_though_functor(f, std::forward<Input>(in), F::Input_types{});
			else
				return process_though_functor(f, std::forward<Input>(in));
		};

		if constexpr (sizeof...(tail) > 0)
			return process_data(result(), tail...);
		else
			return result();
	}

}

#endif
