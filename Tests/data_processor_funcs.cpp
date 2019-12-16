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
#include "test_objects.hpp"

INI_TEST_OBJECT(0)
INI_TEST_OBJECT(1)
INI_TEST_OBJECT(2)
INI_TEST_OBJECT(3)
INI_TEST_OBJECT(4)


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
	auto out = process_through_functor(f, in);

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
	auto out = process_through_functor(f, in);

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
	auto& out = process_through_functor(f, in);

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
	auto& out = process_through_functor(f, std::forward<Test_data<std::string, 0>>(in));

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
	auto&& out = process_through_functor(f, std::forward<Test_data<std::string, 0>>(in));

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
		const Test_data<string, 3>&,
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

	auto out = process_through_functor(f,
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

TEST(Data_processor_functions, process_data_simple_functors)
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

	F0 f0;
	F1 f1;
	F2 f2;

	auto&& out = process_data(tuple<>(), tuple<>(), f0, f1, f2);

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


TEST(Data_processor_functions, process_data_demandant)
{
	struct F :
		public aa::Functor<int, int>,
		public aa::Demands<std::string>
	{
		std::string s;
		int operator()(int i) override { return i; }
		void set(const std::string& str) override { s = str; }
	};

	F f;
	std::string s = "Origin";
	float d = 0.f;

	process_data(0, std::tuple(s, d), f);

	ASSERT_EQ(s, f.s);
}

TEST(Data_processor_functions, process_data_demandant_several_types)
{
	struct F :
		public aa::Functor<int, int>,
		public aa::Demands<std::string, double>
	{
		std::string s;
		double d = 0;
		int operator()(int i) override { return i; }
		void set(const std::string& str) override { s = str; }
		void set(const double& db) override { d = db; }
	};

	F f;
	std::string s = "Origin";
	float d = 0.f;

	process_data(0, std::tuple(s, d), f);

	ASSERT_EQ(s, f.s);
	ASSERT_EQ(d, f.d);
}


namespace initialize_aux_data_test
{
	struct F1 :
		public aa::Functor<int>,
		public Generates<
		Types_with_policy<Updating_policy::never, int, bool, char>,
		Types_with_policy<Updating_policy::always, float>
		>
	{
	public:
		AA_GENERATES

			int i = 10;
		float f = 5.5f;

		bool gi = false;
		bool gb = false;
		bool gf = false;
		bool gc = false;

		template<>
		static float get<float>(F1& f) { f.gf = true; return f.f; }

		template<>
		static bool get<bool>(F1& f) { f.gb = true; return true; }

		template<>
		static int get<int>(F1& f) { f.gi = true; return f.i; }

		template<>
		static char get<char>(F1& f) { f.gc = true; return 'c'; }

		int operator()() override { return 0; }

		bool is_active() const override { return true; };
	};

	struct F2 :
		public aa::Functor<int, int>,
		public Generates<Types_with_policy<Updating_policy::never, double>>,
		public Transforms<
		Types_with_policy<Updating_policy::never, int>,
		Types_with_policy<Updating_policy::always, char>
		>
	{
	public:
		AA_GENERATES

			char c = '0';

		bool gd = false;
		bool ti = false;
		bool tc = false;

		template<>
		static double get<double>(F2& f) { f.gd = true; return 2.2; }

		void transform(int& i) override { i += 5; ti = true; }
		void transform(char& c) override { tc = true; ++c; }

		int operator()(int) override { return 0; }
	};

	struct F3 :
		public aa::Functor<int, int>,
		public Demands<int, float, char>,
		public Transforms <Types_with_policy<Updating_policy::always, double>>
	{
		int i = 0;
		float f = 0;
		char c = 0;

		bool tc = false;

		void set(const int& i_) override { i = i_; }
		void set(const float& f_) override { f = f_; }
		void set(const char& c_) override { c = c_; }

		void transform(double& d) override { tc = true; d *= 2; }

		int operator()(int) override { return 0; }
	};
}

TEST(Data_processor_functions, get_generated)
{
	using namespace initialize_aux_data_test;

	F1 f;

	auto [i, fl] = get_generated(f, Typelist<int, float>{});

	ASSERT_EQ(i, f.i);
	ASSERT_EQ(fl, f.f);

	ASSERT_TRUE(f.gf);
	ASSERT_TRUE(f.gi);
}

TEST(Data_processor_functions, initialize_const_aux_data)
{
	using namespace initialize_aux_data_test;

	F1 f1;
	F2 f2;
	F3 f3;

	initialize_const_aux_data(std::tuple(), f1, f2, f3);
	ASSERT_FALSE(f1.gb);
	ASSERT_TRUE(f1.gi);
	ASSERT_FALSE(f1.gf);

	ASSERT_FALSE(f2.gd);
	ASSERT_TRUE(f2.ti);

	ASSERT_EQ(f3.i, 15);
	ASSERT_EQ(f3.f, 0);
}


TEST(Data_processor_functions, aux_data_changes_always)
{
	using namespace initialize_aux_data_test;

	F1 f1;
	F2 f2;
	F3 f3;

	process_data(tuple(), tuple(), f1, f2, f3);

	ASSERT_FALSE(f1.gi);
	ASSERT_FALSE(f1.gb);
	ASSERT_TRUE(f1.gc);
	ASSERT_TRUE(f1.gf);

	ASSERT_FALSE(f2.gd);
	ASSERT_FALSE(f2.ti);
	ASSERT_TRUE(f2.tc);

	ASSERT_FALSE(f3.tc);
	ASSERT_EQ(f3.c, 'd');
	ASSERT_EQ(f3.f, 5.5f);
	ASSERT_EQ(f3.i, 0);
}

namespace aux_data_changes_sometimes_simple
{
	struct F1 :
		public Functor<int, int>,
		public Generates<Types_with_policy<Updating_policy::sometimes, int, float>>
	{
		bool bi = true;
		bool bf = true;

		int operator()(int i) { return i; }

		AA_GENERATES_SOMETIMES

			template<> bool has_new_data<int>() const { return bi; }
		template<> bool has_new_data<float>() const { return bf; }

		template<>
		static int get<int>(F1& f) {
			if (f.bi)
			{
				f.bi = false;
				return 10;
			}
			else
				return 5;
		}

		template<>
		static float get<float>(F1& f) { return 1.2f; }
	};

	struct F2 :
		public Functor<int, int>,
		public Demands<int, float>
	{
		int i = 0;
		float f = 0;

		int operator()(int i) { return i; }

		void set(const int& i_) { i = i_; }
		void set(const float& f_) { f = f_; }
	};
}

TEST(Data_processor_functions, aux_data_changes_sometimes_simple)
{
	using namespace aux_data_changes_sometimes_simple;

	F1 f1;
	F2 f2;

	process_data(0, tuple(), f1, f2);

	ASSERT_FALSE(f1.bi);

	ASSERT_EQ(f2.i, 10);
	ASSERT_EQ(f2.f, 1.2f);

	process_data(0, tuple(), f1, f2);

	ASSERT_FALSE(f1.bi);

	ASSERT_EQ(f2.i, 10);
	ASSERT_EQ(f2.f, 1.2f);
}

namespace aux_data_changes_sometimes_simple_with_non_optional
{
	struct F1 :
		public Functor<int, int>,
		public Generates<
			Types_with_policy<Updating_policy::sometimes, int, float>,
			Types_with_policy<Updating_policy::always, char>
		>
	{
		bool bi = true;
		bool bf = true;

		int operator()(int i) { return i; }

		AA_GENERATES_SOMETIMES;

		template<> bool has_new_data<int>() const { return bi; }
		template<> bool has_new_data<float>() const { return bf; }

		template<>
		static int get<int>(F1& f) {
			if (f.bi)
			{
				f.bi = false;
				return 10;
			}
			else
				return 5;
		}

		template<>
		static float get<float>(F1& f) { return 1.2f; }

		template<>
		static char get<char>(F1& f) { return 'a'; }
	};

	struct F2 :
		public Functor<int, int>,
		public Demands<int, float, char>
	{
		int i = 0;
		float f = 0;
		char c = 0;

		int operator()(int i) { return i; }

		void set(const int& i_) { i = i_; }
		void set(const float& f_) { f = f_; }
		void set(const char& c_) { c = c_; }
	};
}

TEST(Data_processor_functions, aux_data_changes_sometimes_simple_with_non_optional)
{
	using namespace aux_data_changes_sometimes_simple_with_non_optional;

	F1 f1;
	F2 f2;

	process_data(0, tuple(), f1, f2);

	ASSERT_FALSE(f1.bi);

	ASSERT_EQ(f2.i, 10);
	ASSERT_EQ(f2.f, 1.2f);
	ASSERT_EQ(f2.c, 'a');

	process_data(0, tuple(), f1, f2);

	ASSERT_FALSE(f1.bi);

	ASSERT_EQ(f2.i, 10);
	ASSERT_EQ(f2.f, 1.2f);
	ASSERT_EQ(f2.c, 'a');
}

namespace aux_data_changes_never_gen_somt_tr
{
	struct F1 :
		public Functor<int, int>,
		public Generates<Types_with_policy<Updating_policy::never, int>>
	{
		int operator()(int i) { return i; }

		AA_GENERATES;

		template<>
		static int get<int>(F1& f) { return 10; }
	};

	struct F2 :
		public Functor<int, int>,
		public Transforms<Types_with_policy<Updating_policy::sometimes, int>>
	{
		int operator()(int i) { return i; }

		AA_TRANSFORMS_SOMETIMES;

		bool nt = true;

		template<>
		bool is_transformation_changed<int>() const { return nt; }

		void transform(int& i)
		{
			if (nt)
				i *= 2;
			else
				i = 0;
			nt = false;
		}
	};

	struct F3 :
		public Functor<int, int>,
		public Demands<int>
	{
		int i = 0;

		int operator()(int i) { return i; }

		void set(const int& i_) { i = i_; }
	};
}

TEST(Data_processor_functions, aux_data_changes_never_gen_somt_tr)
{
	using namespace aux_data_changes_never_gen_somt_tr;

	F1 f1;
	F2 f2;
	F3 f3;

	process_data(0, tuple(), f1, f2, f3);

	ASSERT_FALSE(f2.nt);
	ASSERT_EQ(f3.i, 20);

	process_data(0, tuple(), f1, f2, f3);

	ASSERT_FALSE(f2.nt);
	ASSERT_EQ(f3.i, 20);
}


namespace aux_data_changes_somt_gen_somt_tr
{
	struct F1 :
		public Functor<int, int>,
		public Generates<Types_with_policy<Updating_policy::sometimes, int>>
	{
		bool bi = false;

		int operator()(int i) { return i; }

		AA_GENERATES_SOMETIMES;

		template<>
		static int get<int>(F1& f) 
		{ 
			if (f.bi) {
				f.bi = !f.bi;
				return 10;
			}
			else {
				f.bi = !f.bi;
				return 5;
			}
		}

		template<>
		bool has_new_data<int>() const { return bi; }
	};

	struct F2 :
		public Functor<int, int>,
		public Transforms<Types_with_policy<Updating_policy::sometimes, int>>
	{
		int operator()(int i) { return i; }

		AA_TRANSFORMS_SOMETIMES;

		bool nt = true;

		template<>
		bool is_transformation_changed<int>() const { return nt; }

		void transform(int& i)
		{
			if (nt)
				i *= 2;
			else
				i = 0;
			nt = false;
		}
	};

	struct F3 :
		public Functor<int, int>,
		public Demands<int>
	{
		int i = 0;

		int operator()(int i) { return i; }

		void set(const int& i_) { i = i_; }
	};
}

TEST(Data_processor_functions, aux_data_changes_somt_gen_somt_tr)
{
	using namespace aux_data_changes_somt_gen_somt_tr;

	F1 f1;
	F2 f2;
	F3 f3;

	process_data(0, tuple(), f1, f2, f3);
	ASSERT_EQ(f3.i, 10);

	f2.nt = true;
	process_data(0, tuple(), f1, f2, f3);
	ASSERT_EQ(f3.i, 20);

	process_data(0, tuple(), f1, f2, f3);
	ASSERT_EQ(f3.i, 20);

	f1.bi = true;
	process_data(0, tuple(), f1, f2, f3);
	ASSERT_EQ(f3.i, 0);
}