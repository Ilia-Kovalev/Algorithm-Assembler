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

#ifndef INTERFACES_DETAIL_HPP
#define INTERFACES_DETAIL_HPP

#include "../interfaces.hpp"
#include "../enums.hpp"

namespace algorithm_assembler
{
	template<Updating_policy, typename, typename...> struct Types_with_policy;
}

namespace algorithm_assembler::detail
{
	class Functor_ {};


	class Generator {};

	template<Updating_policy> class Generatating_policy {};

	template<typename T> class Generates_type {};

	template<Updating_policy UP, typename T> 
	class Generates_type_with_policy :
		virtual public Generator,
		virtual public Generatating_policy<UP>,
		public Generates_type<T>
	{
	};

	template<typename T> 
	class Generates_type_with_policy<Updating_policy::sometimes, T> :
		virtual public Generator,
		virtual public Generatating_policy<Updating_policy::sometimes>,
		public Generates_type<T>
	{
	public:
		template<typename T_> bool has_new_data() const;

		/// <summary>
		/// Indicator of new data in a module.
		/// </summary>
		/// <returns>
		///   <c>true</c> if a module has new data; otherwise, <c>false</c>.
		/// </returns>
		template<> virtual bool has_new_data<T>() const;
	};


	template<typename Types_with_policy>
	class Generates_types_with_policy;

	template<Updating_policy UP, typename T, typename... Ts>
	class Generates_types_with_policy<Types_with_policy<UP, T, Ts... >> :
		public Generates_type_with_policy<UP, T>,
		public Generates_type_with_policy<UP, Ts>...
	{};


	class Transformer {};

	template<Updating_policy> class Transformation_policy {};

	template<typename T> class Transforms_type {};

	template<Updating_policy UP, typename T> 
	class Transforms_type_with_policy :
		virtual public Transformer,
		virtual public Transformation_policy<UP>,
		public Transforms_type<T>
	{
	public:
		/// <summary>
		/// Transforms referenced value.
		/// </summary>
		virtual void transform(T& data) = 0;
	};

	template<typename T> 
	class Transforms_type_with_policy<Updating_policy::sometimes, T> :
		virtual public Transformer,
		virtual public Transformation_policy<Updating_policy::sometimes>,
		public Transforms_type<T>
	{
	public:
		/// <summary>
		/// Transforms referenced value.
		/// </summary>
		virtual void transform(T& data) = 0;

		template<typename T_> bool transformation_changed() const;

		/// <summary>
		/// Indicates if transformations was changed.
		/// </summary>
		/// <returns></returns>
		template<> virtual bool transformation_changed<T>() const;
	};

	template<typename Types_with_policy>
	class Transforms_types_with_policy;

	template<Updating_policy UP, typename T, typename... Ts>
	class Transforms_types_with_policy<Types_with_policy<UP, T, Ts...>> :
		public Transforms_type_with_policy<UP, T>,
		public Transforms_type_with_policy<UP, Ts>...
	{};


	class Demandant {};

	template<typename T>
	class Demands_type : virtual public Demandant
	{
	public:
		/// <summary>
		/// Sets specified value.
		/// </summary>
		virtual void set(const T& in) = 0;
	};



	class Uses_settings {};


}

#endif