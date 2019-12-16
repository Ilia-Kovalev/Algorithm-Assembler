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


class Generator_Test :
	public Generates<
		Types_with_policy<Updating_policy::always, int, float, bool>,
		Types_with_policy<Updating_policy::sometimes, double>,
		Types_with_policy<Updating_policy::never, char>
	>
{
public:
	AA_GENERATES_SOMETIMES;

	template<> static int get<int>(Generator_Test&) { return 5; }
	template<> static float get<float>(Generator_Test&) { return 5.5; }
	template<> static bool get<bool>(Generator_Test&) { return true; }
	template<> static double get<double>(Generator_Test&) { return -2.6; }
	template<> static char get<char>(Generator_Test&) { return 'a'; }
	
	template<> bool has_new_data<double>() const override { return false; }
};

TEST(Interfaces, Generator)
{
	Generator_Test gt;

	ASSERT_EQ(gt.get<int>(gt), 5);
	ASSERT_EQ(gt.get<float>(gt), 5.5);
	ASSERT_EQ(gt.get<bool>(gt), true);
	ASSERT_EQ(gt.get<double>(gt), -2.6);
	ASSERT_EQ(gt.get<char>(gt), 'a');

	ASSERT_FALSE(gt.has_new_data<double>());
}


class Transformer_test :
	public Transforms<Types_with_policy<Updating_policy::never, int>>,
	public Transforms<Types_with_policy<Updating_policy::sometimes, double, float>>,
	public Transforms< Types_with_policy<Updating_policy::always, char>>
{
public:
	AA_TRANSFORMS_SOMETIMES;

	void transform(int& in) override { in += 10; }
	void transform(double& in)  override { in *= 2; }
	void transform(float& in)  override { in *= 3; }
	void transform(char& in) override { in = 'b'; }


	template<> bool is_transformation_changed<double>() const override { return true; }
	template<> bool is_transformation_changed<float>() const override { return false; }
};

TEST(Interfaces, Transformer)
{
	Transformer_test tt;

	int i = 5;
	tt.transform(i);
	ASSERT_EQ(i, 15);
	
	double d = 2.5;
	tt.transform(d);
	ASSERT_EQ(d, 5);
	
	float f = 15;
	tt.transform(f);
	ASSERT_EQ(f, 45);

	char c = 'a';
	tt.transform(c);
	ASSERT_EQ(c, 'b');

	ASSERT_TRUE(tt.is_transformation_changed<double>());
	ASSERT_FALSE(tt.is_transformation_changed<float>());
}


class Demadnand_test : public Demands<bool, float, int>
{
public:
	bool b_;
	float f_;
	int i_;

	void set(const bool& b) override { b_ = b; }
	void set(const float& f) override { f_ = f; }
	void set(const int& i) override { i_ = i; }
};

TEST(Interfaces, Demandant)
{
	Demadnand_test dt;

	dt.set(true);
	ASSERT_TRUE(dt.b_);

	dt.set(15.f);
	ASSERT_EQ(dt.f_, 15.f);

	dt.set(22);
	ASSERT_EQ(dt.i_, 22);
}

class Functor_test : public Functor<std::string, int, float, char>
{
public:
	std::string operator()(int i, float f, char c) override { return c + std::to_string(i + f); }
};

TEST(Interfaces, Functor)
{
	Functor_test ft;

	auto str = ft(2, 5.5, 'a');
	ASSERT_EQ(str, "a7.500000");
}

class Functor_source_test : public Functor<int>
{
	int i = 2;
public:
	int operator()() override { return i--; }
	bool is_active() const override { return i > 0; }
};

TEST(Interfaces, Functor_source)
{
	Functor_source_test fst;

	ASSERT_TRUE(fst.is_active());
	ASSERT_EQ(fst(), 2);
	ASSERT_EQ(fst(), 1);
	ASSERT_FALSE(fst.is_active());
}