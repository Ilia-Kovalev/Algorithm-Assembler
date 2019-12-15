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
	template<class F, typename... Generated>
	inline auto get_generated(F& f, utils::Typelist<Generated...>&&)
	{
		return std::tuple(F::get<Generated>(f)...);
	}

	template<class F, typename Tuple, typename... Transformed>
	inline void transform(F& f, Tuple& in_tuple, utils::Typelist<Transformed...>&&)
	{
		(f.transform(std::get<Transformed>(in_tuple)), ...);
	}

	template<class F, typename Tuple, typename... Demanded>
	inline void set_to_demandant(F& f, const Tuple& in_tuple, utils::Typelist<Demanded...>&&)
	{
		(f.set(std::get<Demanded>(in_tuple)), ...);
	}

	template<class F, class... Fs, typename... Ts>
	inline void initialize_const_aux_data(std::tuple<Ts...>&& aux, F& f, Fs&... tail)
	{
		using Available_aux_types = utils::Typelist<Ts...>;

		if constexpr (is_transformer_v<F>)
		{
			using Transformed_available_types = utils::intersection_t<
				Available_aux_types,
				typename F::Transforms_types
			>;

			transform(f, aux, Transformed_available_types{});
		}


		if constexpr (is_demandant_v<F>)
		{
			using Demanded_available_types = utils::intersection_t<
				Available_aux_types,
				typename F::Demands_types
			>;

			set_to_demandant(f, aux, Demanded_available_types{});
		}

		using Generated_now_const_types =
			get_generated_types_by_policy_t<Updating_policy::never, F>;

		using Transforming_later_non_const_types =
			utils::unique_t<utils::concatenation_t<
			get_transformed_types_by_policy_t<Updating_policy::always, Fs...>,
			get_transformed_types_by_policy_t<Updating_policy::sometimes, Fs...>
			>>;

		using Demanded_later_types =
			utils::unique_t<utils::concatenation_t<
			get_demanded_types_t<Fs...>
			>>;

		using Always_const_generated_types =
			utils::intersection_t<
			utils::substraction_t<
			Generated_now_const_types,
			Transforming_later_non_const_types
			>,
			Demanded_later_types
			>;

		constexpr bool is_generating_const_data =
			!Always_const_generated_types::is_empty;

		if constexpr (sizeof...(Fs) > 0)
			initialize_const_aux_data(
				std::tuple_cat(
					std::forward<std::tuple<Ts...>>(aux),
					get_generated(f, Always_const_generated_types{})
				),
				tail...
			);
	}

	template<typename T, typename Tuple>
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

	template<typename Tuple, typename... Ts>
	inline auto filter_listed_types(Tuple&& t, utils::Typelist<Ts...>&&)
	{
		return std::tuple(std::get<Ts>(std::forward<Tuple>(t))...);
	}

	template<class F, typename Input, typename _ = utils::Typelist<>,
		typename = std::enable_if_t<!utils::is_tuple_v<Input>>
	>
		inline auto process_through_functor(F& f, Input&& in, _ = _{}) -> typename F::Output_type
	{
		return f(std::forward<Input>(in));
	}

	template<class F, typename Tuple, typename... F_ins,
		typename = std::enable_if_t<utils::is_tuple_v<Tuple>>
	>
		inline auto process_through_functor(
			F& f,
			Tuple&& in_tuple,
			utils::Typelist<F_ins...>&&
		) -> typename F::Output_type
	{
		return f(
			tuple_get_wrapper<F_ins>(std::forward<Tuple>(in_tuple)
				)...);
	}

	template<typename Input, class F, class... Fs, typename... Ts>
	inline auto process_data(Input&& in, std::tuple<Ts...>&& aux, F& f, Fs&... tail)
		-> typename utils::Typelist<F, Fs...>::back::Output_type
	{
		using Available_aux_types = utils::Typelist<Ts...>;

		if constexpr (std::is_base_of_v<detail::Demandant, F>)
		{
			using Demanded_available_types = utils::intersection_t<
				Available_aux_types,
				F::Demands_types
			>;

			set_to_demandant(f, aux, Demanded_available_types{});
		}

		if constexpr (sizeof...(Fs) > 0)
		{
			auto&& output = process_through_functor(f,
				std::forward<Input>(in),
				F::Input_types{}
			);

			if constexpr (is_transformer_v<F>)
			{
				using Transformed_available_types = utils::intersection_t<
					Available_aux_types,
					typename F::Transforms_types,
					get_demanded_types_t<Fs...>
				>;

				transform(f, aux, Transformed_available_types{});
			}

			using Generated_now_const_types =
				get_generated_types_by_policy_t<Updating_policy::never, F>;

			using Transforming_later_non_const_types =
				utils::unique_t<utils::concatenation_t<
					get_transformed_types_by_policy_t<Updating_policy::always, Fs...>,
					get_transformed_types_by_policy_t<Updating_policy::sometimes, Fs...>
				>>;

			using Const_generated_types_transformed_later =
				utils::intersection_t<
					Generated_now_const_types,
					Transforming_later_non_const_types
				>;

			using Generated_now = utils::concatenation_t<
				get_generated_types_by_policy_t<Updating_policy::always, F>,
				get_generated_types_by_policy_t<Updating_policy::sometimes, F>,
				Const_generated_types_transformed_later
			>;

			using Demanded_generated_now = utils::intersection_t<
				Generated_now,
				get_demanded_types_t<Fs...>
			>;

			using Remaining_types = utils::intersection_t<
				Available_aux_types,
				get_demanded_types_t<Fs...>
			>;

			return process_data(
				std::forward<F::Output_type>(output),
				std::tuple_cat(
					filter_listed_types(
						std::forward<std::tuple<Ts...>>(aux),
						Remaining_types{}
					),
					get_generated(f, Demanded_generated_now{})
				),
				tail...);
		}
		else
			return process_through_functor(f, std::forward<Input>(in), F::Input_types{});
	}


}

#endif
