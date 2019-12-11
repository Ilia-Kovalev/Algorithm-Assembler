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

#include "pch.h"

#include <algorithm_assembler/data_processor.hpp>
#include <algorithm_assembler/utils/typelist.hpp>

#include "test_objects.hpp"

using namespace std;

using namespace algorithm_assembler;
using namespace algorithm_assembler::detail;
namespace aa = algorithm_assembler;

template<typename... Ts>
using Typelist = algorithm_assembler::utils::Typelist<Ts...>;

INI_TEST_OBJECT(0)
INI_TEST_OBJECT(1)
INI_TEST_OBJECT(2)

TEST(Data_processor, source)
{
	reset_counters<0>();
	reset_counters<1>();
	reset_counters<2>();

	struct F0 : public aa::Functor<Test_data<string, 0>>
	{
		Test_data<string, 0> operator()() override { return "F0 created "s; }
		bool is_active() const override { return true; }
	};

	struct F1 : public aa::Functor<
		std::tuple<
			Test_data<string, 0>&&,
			Test_data<string, 1>&
		>,
		Test_data<string, 0>&&
	> {
		using Output = std::tuple<Test_data<string, 0>&&, Test_data<string, 1>&>;

		Test_data<string, 1> out1;

		Output operator()(Test_data<string, 0>&& in) override
		{
			out1.data = in.data + "F1 created "s;
			in.data += "F1 forwarded "s;

			return {
				std::forward<Test_data<string, 0>>(in),
				out1,
			};
		}
	};

	struct F2 : public aa::Functor<
		Test_data<string, 2>,
		Test_data<string, 0>&&,
		const Test_data<string, 1>&
	> {
		Test_data<string, 2> operator()(Test_data<string, 0>&& in0, const Test_data<string, 1>& in1) override
		{
			return in0.data + "/ " + in1.data;
		}
	};

	aa::Data_processor<F0, F1, F2> f;

	auto&& out = f();

	ASSERT_EQ(out, "F0 created F1 forwarded / F0 created F1 created "s);

	ASSERT_EQ(Test_object<0>::default_counter, 0);
	ASSERT_EQ(Test_object<0>::copy_counter, 0);
	ASSERT_EQ(Test_object<0>::move_counter, 0);
	ASSERT_EQ(Test_object<0>::other_counter, 1);

	ASSERT_EQ(Test_object<1>::default_counter, 1);
	ASSERT_EQ(Test_object<1>::copy_counter, 0);
	ASSERT_EQ(Test_object<1>::move_counter, 0);
	ASSERT_EQ(Test_object<1>::other_counter, 0);

	ASSERT_EQ(Test_object<2>::default_counter, 0);
	ASSERT_EQ(Test_object<2>::copy_counter, 0);
	ASSERT_EQ(Test_object<2>::move_counter, 0);
	ASSERT_EQ(Test_object<2>::other_counter, 1);
}

TEST(Data_processor, processor_simple)
{
	reset_counters<0>();

	struct F : public aa::Functor<
		Test_data<string, 0>&,
		Test_data<string, 0>&
	>
	{
		Test_data<string, 0>& operator()(Test_data<string, 0>& in) override
		{
			in.data += "processed"s;
			return in;
		}
	};

	aa::Data_processor<F> f;

	Test_data<string, 0> in("input "s);

	auto&& out = f(in);

	ASSERT_EQ(out, "input processed"s);

	ASSERT_EQ(Test_object<0>::default_counter, 0);
	ASSERT_EQ(Test_object<0>::copy_counter, 0);
	ASSERT_EQ(Test_object<0>::move_counter, 0);
	ASSERT_EQ(Test_object<0>::other_counter, 1);
}

TEST(Data_processor_detail, is_generator)
{
	struct D : public aa::Generates<Types_with_policy<Updating_policy::always, int, float>> {};
	struct ND {};

	static_assert(is_generator<D>::value);
	static_assert(!is_generator<ND>::value);
}

TEST(Data_processor_detail, filter_generators)
{
	struct D1 : public aa::Generates<Types_with_policy<Updating_policy::always, int, float>> {};
	struct D2 : public aa::Generates<Types_with_policy<Updating_policy::always, int, float>> {};
	struct ND {};

	using list = Typelist<D1, ND, D2>;

	static_assert(is_same_v<
		filter_generators_t<list>,
		Typelist<D1, D2>
	>);
}

TEST(Data_processor_detail, is_generating_policy)
{
	struct D1 : public aa::Generates<Types_with_policy<Updating_policy::always, int, float>> {};
	struct D2 : public aa::Generates<Types_with_policy<Updating_policy::never, int, float>> {};
	struct ND {};

	static_assert(is_generating_policy<Updating_policy::always>::predicate<D1>::value);
	static_assert(!is_generating_policy<Updating_policy::never>::predicate<D1>::value);
	static_assert(is_generating_policy<Updating_policy::never>::predicate<D2>::value);
	static_assert(!is_generating_policy<Updating_policy::never>::predicate<ND>::value);
}

TEST(Data_processor_detail, is_types_with_policy)
{
	using types1 = Types_with_policy<Updating_policy::never, void>;

	static_assert(is_types_with_policy<Updating_policy::never>::predicate<types1>::value);
	static_assert(!is_types_with_policy<Updating_policy::always>::predicate<types1>::value);
}

TEST(Data_processor_detail, get_generated_types_of_module_by_policy)
{
	struct D : 
		public aa::Generates<
			Types_with_policy<Updating_policy::always, int, float>,
			Types_with_policy<Updating_policy::never, double>
		>
	{};
	struct ND {};

	static_assert(is_same_v<
		get_generated_types_of_module_by_policy_t<D, Updating_policy::always>,
		Typelist<int, float>
	>);

	static_assert(is_same_v<
		get_generated_types_of_module_by_policy<D, Updating_policy::never>::type,
		Typelist<double>
	>);

	static_assert(is_same_v<
		get_generated_types_of_module_by_policy<D, Updating_policy::sometimes>::type,
		Typelist<>
	>);

	static_assert(is_same_v<
		get_generated_types_of_module_by_policy<ND, Updating_policy::sometimes>::type,
		Typelist<>
	>);
}


TEST(Data_processor_detail, get_generated_types)
{
	struct D :
		public aa::Generates<
		Types_with_policy<Updating_policy::always, int, float>,
		Types_with_policy<Updating_policy::never, double, float>
		>
	{};

	static_assert(is_same_v<
		get_generated_types_t<D>,
		Typelist<int, float, double>
	>);
}

TEST(Data_processor_detail, is_demandant)
{
	struct D : public aa::Demands<int, float> {};
	struct ND {};

	static_assert(is_demandant<D>::value);
	static_assert(!is_demandant<ND>::value);
}

TEST(Data_processor_detail, filter_demandands_t)
{
	struct D1 : public aa::Demands<int, float> {};
	struct D2 : public aa::Demands<int, float> {};
	struct ND {};

	using list = Typelist<D1, ND, D2>;

	static_assert(is_same_v<
		filter_demandands_t<list>,
		Typelist<D1, D2>
	>);
}


TEST(Data_processor_detail, get_demaded_types_of_module_t)
{
	struct D : public aa::Demands<int, float> {};

	static_assert(is_same_v<
		get_demaded_types_of_module_t<D>,
		Typelist<int, float>
	>);
}

TEST(Data_processor_detail, get_demanded_types)
{
	struct D1 : public aa::Demands<int, float> {};
	struct D2 : public aa::Demands<int, char, float> {};
	struct ND {};

	static_assert(is_same_v<
		get_demanded_types_t<ND, D1, D2>,
		Typelist<int, float, char>
	>);
}

TEST(Data_processor_detail, is_module_demands_type)
{
	struct D1 : public aa::Demands<int, float> {};
	struct D2 : public aa::Demands<int, char, float> {};
	struct ND {};

	static_assert(is_module_demands_type<int>::predicate<D1>::value);
	static_assert(!is_module_demands_type<char>::predicate<D1>::value);
	static_assert(is_module_demands_type<int>::predicate<D2>::value);
	static_assert(!is_module_demands_type<int>::predicate<ND>::value);
}

TEST(Data_processor_detail, filter_demandants_of_type)
{
	struct D1 : public aa::Demands<int, float> {};
	struct D2 : public aa::Demands<int, char, float> {};
	struct ND {};

	static_assert(is_same_v<
		filter_demandants_of_type<int, D1, D2, ND>,
		Typelist<D1, D2>
	>);

	static_assert(is_same_v<
		filter_demandants_of_type<char, D1, D2, ND>,
		Typelist<D2>
	>);

	static_assert(is_same_v<
		filter_demandants_of_type<bool, D1, D2, ND>,
		Typelist<>
	>);
}