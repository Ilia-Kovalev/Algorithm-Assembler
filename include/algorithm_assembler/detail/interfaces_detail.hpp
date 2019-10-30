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

namespace algorithm_assembler::detail
{
	class Functor {};



	class Generator {};

	template<Updating_policy> class Generatating_policy {};

	template<typename T> class Generates_type {};

	template<Updating_policy UP, typename T> class Generates :
		virtual public Generator,
		virtual public Generatating_policy<UP>,
		virtual public Generates_type<T>
	{
	public:
		template<typename T_> T_ get();

		/// <summary>
		/// Method to get generated data of specified type.
		/// </summary>
		template<> virtual T get<T>();
	};

	template<typename T> class Generates<Updating_policy::sometimes, T> :
		virtual public Generator,
		virtual public Generatating_policy<Updating_policy::sometimes>,
		public Generates_type<T>
	{
	public:

		template<typename T_> T_ get();

		/// <summary>
		/// Method to get generated data of specified type.
		/// </summary>
		template<> virtual T get<T>();

		template<typename T_> bool has_new_data() const;

		/// <summary>
		/// Indicator of new data in a module.
		/// </summary>
		/// <returns>
		///   <c>true</c> if a module has new data; otherwise, <c>false</c>.
		/// </returns>
		template<> virtual bool has_new_data<T>() const;
	};

	template<typename T> class Generates<Updating_policy::never, T> :
		virtual public Generator,
		virtual public Generatating_policy<Updating_policy::never>,
		public Generates_type<T>
	{
	public:
		template<typename T_> T_ get() const;

		/// <summary>
		/// Method to get generated data of specified type.
		/// </summary>
		template<> virtual T get<T>() const;
	};



	class Transformer {};

	template<Updating_policy> class Transformation_policy {};

	template<typename T> class Transforms_type {};

	template<Updating_policy UP, typename T> class Transforms :
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

	template<typename T> class Transforms<Updating_policy::sometimes, T> :
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

	template<typename T> class Transforms<Updating_policy::never, T> :
		virtual public Transformer,
		virtual public Transformation_policy<Updating_policy::never>,
		public Transforms_type<T>
	{
	public:
		/// <summary>
		/// Transforms referenced value.
		/// </summary>
		virtual void transform(T& data) const = 0;
	};




	class Demandant {};

	template<typename T>
	class Demandands : virtual public Demandant
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