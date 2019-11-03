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


#ifndef TEST_OBJECTS_HPP
#define TEST_OBJECTS_HPP

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

template <int N> constexpr void reset_counters()
{
	Test_object<N>::default_counter = 0;
	Test_object<N>::copy_counter = 0;
	Test_object<N>::move_counter = 0;
	Test_object<N>::destr_counter = 0;
	Test_object<N>::other_counter = 0;
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
	Test_data(Test_data<T, N>&& o) noexcept : Test_object<N>(std::move(o)) { data = std::move(o.data); o.data = "Moved"; }
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


#endif
