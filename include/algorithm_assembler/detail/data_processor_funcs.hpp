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
	template<typename T>
	struct unpack_optional
	{
		using type = T;
	};

	template<typename T>
	struct unpack_optional<std::optional<T>>
	{
		using type = T;
	};

	template<typename T>
	struct to_optional
	{
		using type = std::optional<T>;
	};


	template<class F, typename... Generated>
	inline auto get_generated(F& f, utils::Typelist<Generated...>&&)
	{
		return std::make_tuple(F::get<Generated>(f)...);
	}

	template<typename T, class F>
	inline bool check_new_transformations(F& f)
	{
		if constexpr (std::is_base_of_v<Transforms_type_with_policy<Updating_policy::sometimes, T>, F>)
			return f.is_transformation_changed<T>();
		else
			return false;
	}

	template<typename Generated, class F, class... Fs>
	inline std::optional<Generated> get_optional_generated_by_type(F& f, Fs&... fs)
	{
		if constexpr (std::is_base_of_v<Generates_type_with_policy<Updating_policy::never, Generated>, F>)
		{
			if ((check_new_transformations<Generated>(fs) || ...))
				return F::get<Generated>(f);
			else
				return {};
		}
		else if constexpr (std::is_base_of_v<Generates_type_with_policy<Updating_policy::sometimes, Generated>, F>)
		{
			if (f.has_new_data<Generated>() || (check_new_transformations<Generated>(fs) || ...))
				return F::get<Generated>(f);
			else
				return {};
		}

		return {};
	}

	template<class F, class... Fs, typename... Generated>
	inline auto get_optional_generated(F& f, utils::Typelist<Generated...>&&, Fs&... fs)
	{
		return std::make_tuple(get_optional_generated_by_type<Generated>(f, fs...)...);
	}

	template<class F, typename T>
	inline void transform_type(F& f, T& in)
	{
		if constexpr (std::is_base_of_v<Transforms_type<T>, F>)
			f.transform(in);
	}

	template<class F, typename T>
	inline void transform_type(F& f, std::optional<T>& in)
	{
		if constexpr (std::is_base_of_v<Transforms_type<T>, F>)
			if (in.has_value())
				f.transform(in.value());
	}

	template<class F, typename... Ts>
	inline void transform(F& f, std::tuple<Ts...>& in_tuple)
	{
		(transform_type(f, std::get<Ts>(in_tuple)), ...);
	}

	template<class F, typename T>
	inline void set_type_to_demandant(F& f, const T& in)
	{
		if constexpr (std::is_base_of_v<Demands_type<T>, F>)
			f.set(in);
	}

	template<class F, typename T>
	inline void set_type_to_demandant(F& f, const std::optional<T>& in)
	{
		if constexpr (std::is_base_of_v<Demands_type<T>, F>)
			if (in.has_value())
				f.set(in.value());
	}

	template<class F, typename... Ts>
	inline void set_to_demandant(F& f, const std::tuple<Ts...>& in_tuple)
	{
		(set_type_to_demandant(f, std::get<Ts>(in_tuple)), ...);
	}

	template<class F, class... Fs, typename... Ts>
	inline void initialize_const_aux_data(std::tuple<Ts...>&& aux, F& f, Fs&... tail)
	{
		using Available_aux_types = utils::Typelist<Ts...>;

		transform(f, aux);
		set_to_demandant(f, aux);

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
		return std::make_tuple(std::get<Ts>(std::forward<Tuple>(t))...);
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
		using Available_aux_types = utils::map_t<utils::Typelist<Ts...>, unpack_optional<void>>;
		

		set_to_demandant(f, aux);

		if constexpr (sizeof...(Fs) > 0)
		{
			auto&& output = process_through_functor(f,
				std::forward<Input>(in),
				F::Input_types{}
			);

			transform(f, aux);

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

			using Optional = utils::concatenation_t<
				utils::substraction_t<
					utils::intersection_t<
						Demanded_generated_now,
						get_generated_types_of_module_by_policy_t<F, Updating_policy::sometimes>
					>,
					get_generated_types_by_policy_t<Updating_policy::always, F>
				>,
				utils::intersection_t<
					Demanded_generated_now,
					get_generated_types_of_module_by_policy_t<F, Updating_policy::never>,
					get_transformed_types_by_policy_t<Updating_policy::sometimes, Fs...>
				>
			>;

			using Non_optional = utils::substraction_t<
				Demanded_generated_now,
				Optional
			>;

			using Remaining_types = utils::concatenation_t<
				utils::intersection_t<
					utils::Typelist<Ts...>,
					get_demanded_types_t<Fs...>
				>,
				utils::intersection_t<
					utils::Typelist<Ts...>,
					utils::map_t<get_demanded_types_t<Fs...>, to_optional<void>>
				>
			>;

			return process_data(
				std::forward<F::Output_type>(output),
				std::tuple_cat(
					filter_listed_types(
						std::forward<std::tuple<Ts...>>(aux),
						Remaining_types{}
					),
					get_generated(f, Non_optional{}),
					get_optional_generated(f, Optional{}, tail...)
				),
				tail...);
		}
		else
			return process_through_functor(f, std::forward<Input>(in), F::Input_types{});
	}

}

#endif
