#include <type_traits>
#include <tuple>

#include "algorithm_assembler/detail/Typelist.hpp"

namespace typelist
{
	using namespace algorithm_assembler::typelist;

	constexpr void type()
	{
		using testlist = Typelist<int, char, bool, int, float, float>;

		static_assert(std::is_same_v<
			testlist::at<0>, 
			int
		>);
		static_assert(std::is_same_v<
			testlist::at<1>, 
			char
		>);
		static_assert(std::is_same_v<
			testlist::at<2>, 
			bool
		>);
		static_assert(std::is_same_v<
			testlist::at<3>, 
			int
		>);
		static_assert(std::is_same_v<
			testlist::at<4>,
			float
		>);
		static_assert(std::is_same_v<
			testlist::at<5>, 
			float
		>);
	}

	constexpr void size()
	{
		using testlist = Typelist<int, char, bool, int, float, float>;

		static_assert(testlist::size == 6);
		static_assert(Typelist<>::size == 0);
	}

	constexpr void tuple()
	{
		static_assert(std::is_same_v<Typelist<>::values_container<std::tuple>, std::tuple<>>);

		using case1 = Typelist<int, char, bool, int, float, float>::values_container<std::tuple>;
		using correct1 = std::tuple<int, char, bool, int, float, float>;

		static_assert(std::is_same_v<case1, correct1>);

		using case2 = Typelist<int, char, Typelist<bool, bool>, int, float, float>::values_container<std::tuple>;
		using correct2 = std::tuple<int, char, std::tuple<bool, bool>, int, float, float>;

		static_assert(std::is_same_v<case2, correct2>);
	}

	constexpr void modification()
	{
		using testlist = Typelist<int, char, bool, int, float, float>;

		static_assert(std::is_same_v<
			testlist::head, 
			int
		>);
		static_assert(std::is_same_v<
			testlist::tail, 
			Typelist<char, bool, int, float, float>
		>);
		static_assert(std::is_same_v<
			testlist::back, 
			float
		>);


		static_assert(std::is_same_v<
			Typelist<>::push_back<>, 
			Typelist<>
		>);
		static_assert(std::is_same_v<
			Typelist<int>::push_back<>,
			Typelist<int>
		>);
		static_assert(std::is_same_v<
			Typelist<>::push_back<int>, 
			Typelist<int>
		>);
		static_assert(std::is_same_v<
			Typelist<>::push_back<int, bool>,
			Typelist<int, bool>
		>);
		static_assert(std::is_same_v<
			Typelist<float>::push_back<int>,
			Typelist<float, int>
		>);
		static_assert(std::is_same_v<
			Typelist<bool>::push_back<int, bool>, 
			Typelist<bool, int, bool>
		>);


		static_assert(std::is_same_v<
			Typelist<>::push_front<>, 
			Typelist<>
		>);
		static_assert(std::is_same_v<
			Typelist<int>::push_front<>, 
			Typelist<int>
		>);
		static_assert(std::is_same_v<
			Typelist<>::push_front<int>, 
			Typelist<int>
		>);
		static_assert(std::is_same_v<
			Typelist<>::push_front<int, bool>, 
			Typelist<int, bool>
		>);
		static_assert(std::is_same_v<
			Typelist<float>::push_front<int>,
			Typelist<int, float>
		>);
		static_assert(std::is_same_v<
			Typelist<bool>::push_front<int, bool>,
			Typelist<int, bool, bool>
		>);


		static_assert(std::is_same_v<
			Typelist<>::flat,
			Typelist<>
		>);
		static_assert(std::is_same_v<
			Typelist<int>::flat,
			Typelist<int>
		>);
		static_assert(std::is_same_v<
			Typelist<int, bool>::flat,
			Typelist<int, bool>
		>);
		static_assert(std::is_same_v<
			Typelist<Typelist<>, bool>::flat,
			Typelist<bool>
		>);
		static_assert(std::is_same_v<
			Typelist<Typelist<>, bool, Typelist<>, Typelist<>>::flat,
			Typelist<bool>
		>);
		static_assert(std::is_same_v<
			Typelist<Typelist<int, double>, bool, Typelist<>, Typelist<char>>::flat,
			Typelist<int, double, bool, char>
		>);
	}

	constexpr void predicates()
	{
		static_assert(!is_type_list_v<int>);
		static_assert(is_type_list_v<Typelist<>>);
		static_assert(is_type_list_v<Typelist<int>>);
		static_assert(is_type_list_v<Typelist<int, char>>);

		static_assert(Typelist<>::is_empty);
		static_assert(!Typelist<bool>::is_empty);

		static_assert(!Typelist<>::contains<int>);
		static_assert(!Typelist<bool, float>::contains<int>);
		static_assert(Typelist<bool, float>::contains<bool>);
		static_assert(!Typelist<bool, Typelist<int>, float>::contains<int>);

	}

	constexpr void test()
	{
		type();
		size();
		tuple();
		modification();
		predicates();
	}

}