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

#ifndef DATA_PROCESSOR_DETAIL_HPP
#define DATA_PROCESSOR_DETAIL_HPP


#include <tuple>

#include "../utils/typelist.hpp"
#include "../interfaces.hpp"
#include "data_processor_funcs.hpp"

namespace algorithm_assembler::detail
{
	template<class... Modules>
	class DP_Modules
	{
	protected:
		std::tuple<Modules...> modules_;
	};

	template<typename In_typelist, typename Out_type, typename Modules_list> class DP_Functor;

	template<typename In_type, typename... In_types, typename Out_type, class... Modules>
	class DP_Functor<utils::Typelist<In_type, In_types...>, Out_type, utils::Typelist<Modules...>> :
		public algorithm_assembler::Functor<Out_type, In_type, In_types...>,
		public virtual DP_Modules<Modules...>
	{
	public:
		inline Out_type operator()(In_type in, In_types... ins) override
		{
			int aux = 0; //TODO replace by actual data

			return process_data(
				std::forward_as_tuple(std::forward<In_type>(in), std::forward<In_types>(ins)...),
				aux,
				std::get<Modules>(modules_)...
			);
		}
	};

	template<typename Out_type, class... Modules>
	class DP_Functor<utils::Typelist<>, Out_type, utils::Typelist<Modules...>> :
		public algorithm_assembler::Functor<Out_type>,
		public virtual DP_Modules<Modules...>
	{
	public:
		inline Out_type operator()() override
		{
			return process_data(std::get<Modules>(modules_)...);
		}

		inline bool is_active() const override
		{
			return std::get<0>(modules_).is_active();
		}
	};

	template<class T>
	struct is_generator : public std::is_base_of<Generator, T> {};

	template<typename Modules_list>
	using filter_generators_t = utils::filter_t<Modules_list, is_generator<void>>;

	template<Updating_policy UP>
	struct is_generating_policy
	{
		template<class Module>
		struct predicate : public std::is_base_of<Generatating_policy<UP>, Module> {};
	};

	template<Updating_policy UP>
	struct is_types_with_policy
	{
		template<typename Types_with_policy>
		struct predicate
		{
			constexpr static bool value = UP == Types_with_policy::policy;
		};
	};

	template<class Module, Updating_policy, typename = void>
	struct get_generated_types_of_module_by_policy;

	template<class Module, Updating_policy UP>
	struct get_generated_types_of_module_by_policy<Module, UP,
		std::enable_if_t<std::is_base_of_v<Generatating_policy<UP>, Module>>
	>
	{
		using type = typename utils::filter_t<
			typename Module::Grouped_generated_types,
			typename is_types_with_policy<UP>::template predicate<void>
		>::head::types;
	};

	template<class Module, Updating_policy UP>
	struct get_generated_types_of_module_by_policy<Module, UP,
		std::enable_if_t<!std::is_base_of_v<Generatating_policy<UP>, Module>>
	>
	{
		using type = utils::Typelist<>;
	};

	template<class Module, Updating_policy UP>
	using get_generated_types_of_module_by_policy_t =
		typename get_generated_types_of_module_by_policy<Module, UP>::type;

	template<class Module>
	using get_generated_types_of_module_t = typename Module::Generated_types;

	template<typename Modules_list>
	struct get_generated_types;

	template<class Module, class... Modules>
	struct get_generated_types<utils::Typelist<Module, Modules...>>
	{
		using type = utils::unique_t<utils::concatenation_t<
			get_generated_types_of_module_t<Module>,
			get_generated_types_of_module_t<Modules>...>>;
	};

	template<>
	struct get_generated_types<utils::Typelist<>>
	{
		using type = utils::Typelist<>;
	};

	template<typename... Modules>
	using get_generated_types_t =
		typename get_generated_types<filter_generators_t<utils::Typelist<Modules...>>>::type;


	//template<typename Demanded_typelist, typename Transformed_typelist>
	//class DP_Input_aux_cache_impl;



	//template<class... Modules>
	//class DP_Input_aux_cache : 
	//	public DP_Input_aux_cache_impl<
	//		utils::substraction_t<
	//			get_demanded_types_t<Modules...>,
	//			get_generated_types_t<Modules...>
	//		>,
	//		utils::substraction_t<
	//			get_tr
	//	>
	//{};


	template<class T>
	struct is_demandant : public std::is_base_of<Demandant, T> {};

	template<typename Modules_list>
	using filter_demandands_t = utils::filter_t<Modules_list, is_demandant<void>>;

	template<class Module>
	using get_demaded_types_of_module_t = typename Module::Demanded_types;

	template<typename Modules_list>
	struct get_demanded_types;

	template<class Module, class... Modules>
	struct get_demanded_types<utils::Typelist<Module, Modules...>>
	{
		using type = utils::unique_t<utils::concatenation_t<
			get_demaded_types_of_module_t<Module>,
			get_demaded_types_of_module_t<Modules>...>>;
	};

	template<>
	struct get_demanded_types<utils::Typelist<>>
	{
		using type = utils::Typelist<>;
	};

	template<typename... Modules>
	using get_demanded_types_t = 
		typename get_demanded_types<filter_demandands_t<utils::Typelist<Modules...>>>::type;

	template<typename Demanded_type>
	struct is_module_demands_type
	{
		template<class Module>
		struct predicate : public std::is_base_of<Demands<Demanded_type>, Module> {};
	};

	template<typename Demanded_type, class... Modules>
	using filter_demandants_of_type = utils::filter_t<
		utils::Typelist<Modules...>,
		typename is_module_demands_type<Demanded_type>::template predicate<void>
	>;

	template<typename Modules_list, typename Demanded_type>
	class DP_Demandant_impl;

	template<class... Modules, typename Demanded_type>
	class DP_Demandant_impl<utils::Typelist<Modules...>, Demanded_type>
		: public virtual DP_Modules<Modules...>
	{
	public:
		inline void set(const Demanded_type& in) override
		{

		}
	};


	template <typename Modules_list, typename Types_list>
	class DP_Demandant;

	template<typename... _>
	class DP_Demandant<utils::Typelist<_...>, utils::Typelist<>> {};

	template<typename Modules_list, typename Demanded_type, typename... Demanded_types_>
	class DP_Demandant<Modules_list, utils::Typelist<Demanded_type, Demanded_types_...>> :
		public DP_Demandant_impl<Modules_list, Demanded_type>,
		public DP_Demandant_impl<Modules_list, Demanded_types_>...
	{
	public:
		using Demanded_types = utils::Typelist<Demanded_type, Demanded_types_...>;
	};








}
#endif
