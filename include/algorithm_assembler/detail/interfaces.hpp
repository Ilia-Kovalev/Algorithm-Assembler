#ifndef INTERFACES_DETAIL_HPP
#define INTERFACES_DETAIL_HPP

namespace algorithm_assembler
{
	enum class Updating_policy;
}

namespace algorithm_assembler::detail
{
	class Functor {};

	class Uses_settings {};

	class Demandant {};

	class Generator {};
	template<Updating_policy> class Generatating_policy : virtual Generator {};
	template<typename> class Generates_type : virtual Generator {};

	template<Updating_policy UP, typename T> class Generates :
		virtual Generator,
		virtual Generatating_policy<UP>,
		Generates_type<T>
	{};

	template<typename T> class Generates<Updating_policy::sometimes, T> :
		virtual Generator,
		virtual Generatating_policy<Updating_policy::sometimes>,
		Generates_type<T>
	{
	public:

		/// <summary>
		/// Indicator of new data in a module.
		/// </summary>
		/// <returns>
		///   <c>true</c> if a module has new data; otherwise, <c>false</c>.
		/// </returns>
		virtual bool has_new_data() = 0;
	};
}

#endif