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

using namespace algorithm_assembler;

class Generator_Test :
	public Generates<Updating_policy::always, int, float, bool>,
	public Generates<Updating_policy::sometimes, double>
{
public:
	AA_GENERATES_SOMETIMES;

	template<> int get<int>() const override { return 5; }
	template<> float get<float>() const override { return 5.5; }
	template<> bool get<bool>() const override { return true; }
	template<> double get<double>() const override { return -2.6; }

	template<> bool has_new_data<double>() const override { return false; }
};

TEST(Interfaces, Generator)
{
	Generator_Test gt;

	ASSERT_EQ(gt.get<int>(), 5);
	ASSERT_EQ(gt.get<float>(), 5.5);
	ASSERT_EQ(gt.get<bool>(), true);
	ASSERT_EQ(gt.get<double>(), -2.6);

	ASSERT_FALSE(gt.has_new_data<double>());
}


class Transformer_test :
	public Transforms<Updating_policy::never, int>,
	public Transforms<Updating_policy::sometimes, double, float>
{
public:
	AA_TRANSFORMS_SOMETIMES;

	void transform(int& in) const override { in += 10; }
	void transform(double& in) const override { in *= 2; }
	void transform(float& in) const override { in *= 3; }

	template<> bool transformation_changed<double>() const override { return true; }
	template<> bool transformation_changed<float>() const override { return false; }
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

	ASSERT_TRUE(tt.transformation_changed<double>());
	ASSERT_FALSE(tt.transformation_changed<float>());
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
	ASSERT_TRUE(dt.b_, true);

	dt.set(15.f);
	ASSERT_TRUE(dt.f_, 15.f);

	dt.set(22);
	ASSERT_TRUE(dt.i_, 22);
}