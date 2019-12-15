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

#ifndef INTERFACES_HPP
#define INTERFACES_HPP

#include "enums.hpp"
#include "utils/typelist.hpp"
#include "detail/interfaces_detail.hpp"
#include "utils/heterogeneous_container_functions.hpp"

namespace algorithm_assembler
{
	/// <summary>
	/// Interface for modules of main processing pipeline.
	/// </summary>
	template<typename Output, typename... Inputs> class Functor;

	/// <summary>
	/// Specialisation for functor with inputs.
	/// </summary>
	template<typename Output, typename Input, typename... Inputs>
	class Functor<Output, Input, Inputs...> : public virtual detail::Functor_
	{
	public:
		using Output_type = Output;
		using Input_types = utils::Typelist<Input, Inputs...>;

		/// <summary>
		/// Processes input data.
		/// </summary>
		/// <param name="...ins">One or multiple input arguments.</param>
		/// <returns>Result of input processing.</returns>
		virtual Output operator()(Input in, Inputs... ins) = 0;
	};

	/// <summary>
	/// Specialisation for functor without inputs (data source).
	/// </summary>
	template<typename Output>
	class Functor<Output> : public virtual detail::Functor_
	{
	public:
		using Output_type = Output;
		using Input_types = utils::Typelist<>;

		virtual Output operator()() = 0;

		/// <summary>
		/// Should return true, if module is able to return data.
		/// </summary>
		virtual bool is_active() const = 0;
	};

	/// <summary>
	/// Helper struct for passing types with updating policy to interfaces Generates and Transforms.
	/// </summary>
	template<Updating_policy UP, typename T, typename... Ts>
	struct Types_with_policy
	{
		constexpr static Updating_policy policy = UP;
		using types = utils::Typelist<T, Ts...>;
	};

	/// <summary>
	/// Interface for modules generating auxiliary data. 
	/// Arguments are passed via Types_with_policy class.
	/// </summary>
	template<typename Types_with_policy, typename... Ts>
	class Generates :
		public detail::Generates_types_with_policy<Types_with_policy>,
		public detail::Generates_types_with_policy<Ts>...
	{
	public:
		// Macros are necessary to add in inherited classes before overriding virtual methods.
		// AA_GENERATES for Updating_policy never and always
		// AA_GENERATES for Updating_policy sometimes
		#define AA_GENERATES template<typename T, class F> static T get(F&);
		#define AA_GENERATES_SOMETIMES template<typename T, class F> static T get(F&); \
										template <typename> bool has_new_data() const;

		using Generates_types = utils::concatenation_t<
			typename Types_with_policy::types,
			typename Ts::types...
		>;

		using Generates_types_grouped = utils::Typelist<Types_with_policy, Ts...>;
	};


	/// <summary>
	/// Interface for modules transforming auxiliary data.
	/// Arguments are passed via Types_with_policy class.
	/// </summary>
	template<typename Types_with_policy, typename... Ts>
	class Transforms:
		public detail::Transforms_types_with_policy<Types_with_policy>,
		public detail::Transforms_types_with_policy<Ts>...
	{
	public:
		// Macros are necessary to add in inherited classes before overriding virtual methods.
		#define AA_TRANSFORMS_SOMETIMES template<typename> bool transformation_changed() const;

		using Transforms_types = utils::concatenation_t<
			typename Types_with_policy::types,
			typename Ts::types...
		>;

		using Transforms_types_grouped = utils::Typelist<Types_with_policy, Ts...>;
	};


	/// <summary>
	/// Interface for modules requiring auxiliary data.
	/// </summary>
	template<typename T, typename... Ts>
	class Demands :
		public detail::Demands_type<T>,
		public detail::Demands_type<Ts>...
	{
	public:
		using Demands_types = utils::Typelist<T, Ts...>;
	};


	/// <summary>
	/// Interface for modules getting parameters from settings object.
	/// </summary>
	template<class Settings_type_>
	class Uses_settings : detail::Uses_settings
	{
	public:

		using Settings_type = Settings_type_;

		/// <summary>
		/// Sets settings.
		/// </summary>
		virtual void set(const Settings_type& settings) = 0;
	};
}

#endif
