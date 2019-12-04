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

#include "test_objects.hpp"

using namespace std;

using namespace algorithm_assembler::detail;
namespace aa = algorithm_assembler;

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