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

using namespace algorithm_assembler::typelist;


using namespace algorithm_assembler;

class Generator_Test :
	public Generates<Updating_policy::always, int, float, bool>,
	public Generates<Updating_policy::sometimes, double>
{
public:
	AA_GENERATES_SOMETIMES

	template<> int get<int>() { return 5; }
	template<> float get<float>() { return 5.5; }
	template<> bool get<bool>() { return true; }
	template<> double get<double>() { return -2.6; }
	template<> bool has_new_data<double>() override { return false; }
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


