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

#include <algorithm_assembler/interfaces.hpp>
#include <algorithm_assembler/detail/data_processor_funcs.hpp>

using namespace std;

using namespace algorithm_assembler::detail;
namespace aa = algorithm_assembler;

template<int N> struct Test_object
{
	static size_t default_counter;
	static size_t copy_counter;
	static size_t move_counter;
	static size_t other_counter;
	static size_t destr_counter;
	size_t f_counter = 0;

	Test_object() { ++default_counter; }
	Test_object(const Test_object<N>&) { ++copy_counter; }
	Test_object(Test_object<N>&&) noexcept { ++move_counter; }
	Test_object(int) { ++other_counter; }
	~Test_object() noexcept { ++destr_counter; }

	void f() { ++f_counter; }
};

#define INI_TEST_OBJECT(N) 	size_t Test_object<N>::default_counter = 0;\
							size_t Test_object<N>::copy_counter = 0;\
							size_t Test_object<N>::move_counter = 0;\
							size_t Test_object<N>::destr_counter = 0;\
							size_t Test_object<N>::other_counter = 0;

INI_TEST_OBJECT(0)
INI_TEST_OBJECT(1)
INI_TEST_OBJECT(2)
INI_TEST_OBJECT(3)
INI_TEST_OBJECT(4)

template <int N> constexpr void reset_counters()
{
	Test_object<N>::default_counter = 0;
	Test_object<N>::copy_counter = 0;
	Test_object<N>::move_counter = 0;
	Test_object<N>::destr_counter = 0;
	Test_object<N>::other_counter = 0;
}

TEST(Test, Test_object)
{
	reset_counters<0>();

	{
		Test_object<0> to;
		ASSERT_EQ(Test_object<0>::default_counter, 1);
		Test_object<0> to2(to);
		ASSERT_EQ(Test_object<0>::copy_counter, 1);
		Test_object<0> to3(std::move(to));
		ASSERT_EQ(Test_object<0>::move_counter, 1);
		to.f();
		ASSERT_EQ(to.f_counter, 1);
		to2.f();
		ASSERT_EQ(to.f_counter, 1);
	}
	ASSERT_EQ(Test_object<0>::destr_counter, 3);
}

template<typename T, int N>
struct Test_data : public Test_object<N>
{
	const size_t& dc = default_counter;
	const size_t& cc = copy_counter;
	const size_t& mc = move_counter;
	const size_t& oc = other_counter;
	const size_t& dsc = destr_counter;

	T data;

	Test_data() : Test_object<N>() {};
	Test_data(const Test_data<T, N>& o) : Test_object<N>(o) { data = o.data; }
	Test_data(Test_data<T, N>&& o) noexcept : Test_object<N>(std::move(o)) { data = std::move(o.data); o.data = "Moved";  }
	Test_data(const T& d) : Test_object<N>(0) { data = d; }
};

template<typename T, int N>
bool operator==(const Test_data<T, N>& td, const T& d)
{
	return td.data == d;
}

template<typename T, int N>
bool operator==(const T& d, const Test_data<T, N>& td)
{
	return td == d;
}

TEST(Data_processor_functions, process_though_functor_one_copy)
{
	reset_counters<0>();

	struct Test_functor : public aa::Functor<string, Test_data<string, 0>>
	{
		string operator()(Test_data<string, 0> t) override { return t.data + " processed"; }
	};

	auto& a = Test_object<0>::other_counter;

	Test_functor f;
	Test_data<std::string, 0> in = "Input"s;
	auto out = process_though_functor(f, in);

	ASSERT_EQ(out, "Input processed"s);

	ASSERT_EQ(Test_object<0>::default_counter, 0);
	ASSERT_EQ(Test_object<0>::copy_counter, 1);
	ASSERT_EQ(Test_object<0>::move_counter, 0);
	ASSERT_EQ(Test_object<0>::other_counter, 1);
}

TEST(Data_processor_functions, process_though_functor_one_const_ref)
{
	reset_counters<0>();

	struct Test_functor : public aa::Functor<string, const Test_data<string, 0>&>
	{
		string operator()(const Test_data<string, 0>& t) override { return t.data + " processed"; }
	};

	Test_functor f;
	Test_data<std::string, 0> in = "Input"s;
	auto out = process_though_functor(f, in);

	ASSERT_EQ(out, "Input processed"s);
	ASSERT_EQ(Test_object<0>::default_counter, 0);
	ASSERT_EQ(Test_object<0>::copy_counter, 0);
	ASSERT_EQ(Test_object<0>::move_counter, 0);
	ASSERT_EQ(Test_object<0>::other_counter, 1);
}

TEST(Data_processor_functions, process_though_functor_one_ref)
{
	reset_counters<0>();

	struct Test_functor : public aa::Functor<Test_data<string, 0>&, Test_data<string, 0>&>
	{
		Test_data<string, 0>& operator()(Test_data<string, 0>& t) override 
		{ 
			t.data += " processed";
			return t; 
		}
	};

	Test_functor f;
	Test_data<std::string, 0> in = "Input"s;
	auto& out = process_though_functor(f, in);

	ASSERT_EQ(out, "Input processed"s);
	ASSERT_EQ(in, "Input processed"s);
	ASSERT_EQ(Test_object<0>::default_counter, 0);
	ASSERT_EQ(Test_object<0>::copy_counter, 0);
	ASSERT_EQ(Test_object<0>::move_counter, 0);
	ASSERT_EQ(Test_object<0>::other_counter, 1);
}

TEST(Data_processor_functions, process_though_functor_one_forwarding)
{
	reset_counters<0>();

	struct Test_functor : public aa::Functor<Test_data<string, 0>&&, Test_data<string, 0>&&>
	{
		Test_data<string, 0>&& operator()(Test_data<string, 0>&& t) override
		{
			t.data += " processed";
			return forward<Test_data<string, 0>>(t);
		}
	};

	Test_functor f;
	auto&& in = Test_data<std::string, 0>("Input"s);
	auto&& out = process_though_functor(f, std::forward<Test_data<std::string, 0>>(in));

	ASSERT_EQ(out, "Input processed"s);
	ASSERT_EQ(in, "Input processed"s);
	ASSERT_EQ(Test_object<0>::default_counter, 0);
	ASSERT_EQ(Test_object<0>::copy_counter, 0);
	ASSERT_EQ(Test_object<0>::move_counter, 0);
	ASSERT_EQ(Test_object<0>::other_counter, 1);
}

TEST(Data_processor_functions, process_though_functor_one_moving)
{
	reset_counters<0>();

	struct Test_functor : public aa::Functor<string, Test_data<string, 0>&&>
	{
		string operator()(Test_data<string, 0>&& t) override
		{
			auto&& n = Test_data<string, 0>(move(t));
			n.data += " processed";
			return n.data;
		}
	};

	Test_functor f;
	auto&& in = Test_data<std::string, 0>("Input"s);
	auto&& out = process_though_functor(f, std::forward<Test_data<std::string, 0>>(in));

	ASSERT_EQ(out, "Input processed"s);
	ASSERT_EQ(in, "Moved"s);
	ASSERT_EQ(Test_object<0>::default_counter, 0);
	ASSERT_EQ(Test_object<0>::copy_counter, 0);
	ASSERT_EQ(Test_object<0>::move_counter, 1);
	ASSERT_EQ(Test_object<0>::other_counter, 1);
}


TEST(Data_processor_functions, process_though_functor_in_tuple)
{
	reset_counters<0>();
	reset_counters<1>();
	reset_counters<2>();
	reset_counters<3>();

	struct Test_functor : public aa::Functor<string, 
		Test_data<string, 0>, 
		const Test_data<string, 3> &,
		Test_data<string, 2>&,
		Test_data<string, 1>&&
	>
	{
		string operator()(
			Test_data<string, 0> t0,
			const Test_data<string, 3>& t1,
			Test_data<string, 2>& t2,
			Test_data<string, 1>&& t3
			) override
		{
			auto&& n3 = Test_data<string, 1>(move(t3));
			auto result = 
				t0.data + ' '
				+ t1.data + ' '
				+ t2.data + ' '
				+ n3.data;
			t2.data += " got"s;
			return result;
		}
	};


	Test_functor f;
	Test_data<std::string, 0> in0("In0"s);
	auto&& in1 = Test_data<std::string, 1>("In1"s);
	Test_data<std::string, 2> in2("In2"s);
	Test_data<std::string, 3> in3("In3"s);

	auto&& ins = tuple<
		int,
		Test_data<std::string, 0>,
		Test_data<std::string, 1>&&,
		bool,
		Test_data<std::string, 2>&,
		float,
		const Test_data<std::string, 3>&
	>(5, in0, move(in1), true, in2, 5.5, in3);

	auto&& out = process_though_functor(f, 
		std::forward<remove_reference_t<decltype(ins)>>(ins),
		Test_functor::Input_types{}
		);


	ASSERT_EQ(out, "In0 In3 In2 In1"s);

	ASSERT_EQ(in0, "In0"s);
	ASSERT_EQ(in1, "Moved"s);
	ASSERT_EQ(in2, "In2 got"s);
	ASSERT_EQ(in3, "In3"s);

	ASSERT_EQ(Test_object<0>::default_counter, 0);
	ASSERT_EQ(Test_object<0>::copy_counter, 1);
	ASSERT_EQ(Test_object<0>::move_counter, 1);
	ASSERT_EQ(Test_object<0>::other_counter, 1);

	ASSERT_EQ(Test_object<1>::default_counter, 0);
	ASSERT_EQ(Test_object<1>::copy_counter, 0);
	ASSERT_EQ(Test_object<1>::move_counter, 1);
	ASSERT_EQ(Test_object<1>::other_counter, 1);

	ASSERT_EQ(Test_object<2>::default_counter, 0);
	ASSERT_EQ(Test_object<2>::copy_counter, 0);
	ASSERT_EQ(Test_object<2>::move_counter, 0);
	ASSERT_EQ(Test_object<2>::other_counter, 1);

	ASSERT_EQ(Test_object<3>::default_counter, 0);
	ASSERT_EQ(Test_object<3>::copy_counter, 0);
	ASSERT_EQ(Test_object<3>::move_counter, 0);
	ASSERT_EQ(Test_object<3>::other_counter, 1);
}