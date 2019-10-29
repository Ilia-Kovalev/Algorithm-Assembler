#ifndef INTERFACES_HPP
#define INTERFACES_HPP

#include "detail/Typelist.hpp"
#include "detail/Interfaces.hpp"

namespace algorithm_assembler
{
	/// <summary>
	/// Defines hints for auxiliary data updates.
	/// </summary>
	enum class Updating_policy
	{
		never,		/// Auxiliary data is constant.
		sometimes,	/// Auxiliary data is updated when a module indicates about changes.
		always		/// Auxiliary data updates on every iteration.
	};

	/// <summary>
	/// Interface for modules of main processing pipeline.
	/// </summary>
	template<typename Output_, typename... Inputs_>
	class Functor : detail::Functor
	{
	public:
		using Output = Output_;
		using Inputs = Typelist<Inputs_...>;

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
		using Output_type = Output;
		using Input_types = Typelist<>;

		virtual Output operator()() = 0;

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
	class Transforms <UP, typename T>
	{
	public:
		/// <summary>
		/// Transforms referenced value.
		/// </summary>
		virtual void transform(T& in) const = 0;
	};

	template<typename T>
	class Transforms <Updating_policy::sometimes, typename T>
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
		usind demanded_types = typelist::Typelist<T, Ts...>
	};


	namespace detail
	{
		template<Updating_policy UP, typename T>
		class Provides :
			virtual Providing_policy<UP>,
			virtual Providing_type<T>
		{
		public:
			template<typename T1>
			const T1& get() const noexcept;

			template<>
			const T& get<T>() const noexcept
			{
				if constexpr (CH == Changes::sometimes)
					Generator<Changes::sometimes>::has_new_data_ = false;
				return data_;
			};

#pragma warning(suppress: 26440) // noexcept or not depends on T.
			void set(const T & in)
			{
				data_ = in;
				if constexpr (CH == Changes::sometimes)
					Generator<Changes::sometimes>::has_new_data_ = true;
			}

		private:
			T data_;
		};
	}

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
		using generates_types = Typelist<T, Ts...>;

		/// <summary>
		/// Returns data of type T_.
		template<typename T_>
		virtual T_ get() = 0;
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
