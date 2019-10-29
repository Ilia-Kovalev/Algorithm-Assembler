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

#include <iostream>
#include <cassert>

#include "algorithm_assembler/Interfaces.hpp"

namespace interfaces 
{
	using namespace algorithm_assembler;

	class Generator_Test : 
		public Generates<Updating_policy::always, int, float, bool>,
		public Generates<Updating_policy::sometimes, double>
	{
	public:
		template <typename T>  T get();
		template <typename T>  bool has_new_data();

		template<> int get<int>() { return 5; }
		template<> float get<float>() { return 5.5; }
		template<> bool get<bool>() { return true; }
		template<> double get<double>() { return -2.6; }
		template<> bool has_new_data<double>() override { return false; }
	};

	void generator()
	{
		Generator_Test gt;

		assert(gt.get<int>() == 5);
		assert(gt.get<float>() == 5.5);
		assert(gt.get<bool>() == true);
		assert(gt.get<double>() == -2.6);

		assert(gt.has_new_data<double>() == false);
	}
}
