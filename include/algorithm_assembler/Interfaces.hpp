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
#include "detail/typelist.hpp"
#include "detail/interfaces_detail.hpp"

namespace algorithm_assembler
{
	/// <summary>
	/// Interface for modules of main processing pipeline.
	/// </summary>
	template<typename Output_, typename... Inputs_>
	class Functor : detail::Functor
	{
	public:
		using Output = Output_;
		using Inputs = typelist::Typelist<Inputs_...>;

		/// <summary>
		/// Processes input data.
		/// </summary>
		/// <param name="...ins">One or multiple input arguments.</param>
		/// <returns>Result of input processing.</returns>
		virtual Output_ operator()(Inputs_... ins) = 0;
	};

	/// <summary>
	/// Specialisation for functor without inputs.
	/// </summary>
	template<typename Output_>
	class Functor<Output_> : detail::Functor
	{
	public:
		using Output_type = Output_;
		using Input_types = typelist::Typelist<>;

		virtual Output_type operator()() = 0;

		/// <summary>
		/// Should return true, if module is able to return data.
		/// </summary>
		bool is_active() const = 0;
	};

	/// <summary>
	/// Interface for modules transforming auxiliary data. Template arguments specify updating policy and transformed types.
	/// </summary>
	template<Updating_policy, typename, typename...>
	class Transforms;

	template<Updating_policy UP, typename T>
	class Transforms<UP, T>
	{
	public:
		/// <summary>
		/// Transforms referenced value.
		/// </summary>
		virtual void transform(T& in) const = 0;
	};

	template<typename T>
	class Transforms <Updating_policy::sometimes, T>
	{
	public:
		/// <summary>
		/// Transforms referenced value.
		/// </summary>
		virtual void transform(T& in) const = 0;

		virtual bool transformation_changed() const = 0;
	};

	template<Updating_policy UP, typename T, typename... Ts>
	class Transforms:
		public Transforms<UP, T>,
		public Transforms<UP, Ts>...
	{
		template<Updating_policy UP>
		using transformed_types = typelist::Typelist<T, Ts...>;
	};


	namespace detail
	{
		template<typename T>
		class Demandands : virtual Demandant
		{
		public:
			/// <summary>
			/// Sets specified value.
			/// </summary>
			virtual void set(const T& in) = 0;
		};
	}

	/// <summary>
	/// Interface for modules requiring auxiliary data. Template arguments specify required data types.
	/// </summary>
	template<typename T, typename... Ts>
	class Demandands :
		public detail::Demandands<T>,
		public detail::Demandands<Ts>...
	{
		using demanded_types = typelist::Typelist<T, Ts...>;
	};

	/// <summary>
	/// Interface for modules generating auxiliary data.
	/// </summary>
	template<Updating_policy UP, typename T, typename... Ts>
	class Generates :
		public detail::Generates<UP, T>,
		public detail::Generates<UP, Ts>...
	{
	public:
		template<Updating_policy UP>
		using generates_types = typelist::Typelist<T, Ts...>;
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
