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

#include <type_traits>
#include <typeinfo>

#include <algorithm_assembler/utils/heterogeneous_container_functions.hpp>
#include <algorithm_assembler/utils/typelist.hpp>

using namespace algorithm_assembler::utils;
using namespace std;

TEST(Container_functions, concatenation) 
{
    using list1 = Typelist<int, bool, double>;
    using list2 = Typelist<float, double>;
    using list3 = Typelist<char>;
    using list4 = Typelist<>;

    static_assert(is_same_v<
        concatenation_t<list1, list2>,
        Typelist<int, bool, double, float, double>
    >);

    static_assert(is_same_v<
        concatenation_t<list1, list3>,
        Typelist<int, bool, double, char>
    >);

    static_assert(is_same_v<
        concatenation_t<list1, list4>,
        list1
    >);

    static_assert(is_same_v<
        concatenation_t<list1, list2, list3>,
        Typelist<int, bool, double, float, double, char>
    >);

    static_assert(is_same_v<
        concatenation_t<list1, list2, list3, list4>,
        Typelist<int, bool, double, float, double, char>
    >);

    static_assert(is_same_v<
        concatenation_t<list4, list1, list2, list3>,
        Typelist<int, bool, double, float, double, char>
    >);
}

TEST(Container_functions, type_at) 
{
    using list1 = Typelist<int, bool, double>;
    using list2 = Typelist<int>;

    static_assert(is_same_v<
        type_at_t<list1, 0>,
        int>);

    static_assert(is_same_v<
        type_at_t<list1, 1>,
        bool>);

    static_assert(is_same_v<
        type_at_t<list1, 2>,
        double>);

    static_assert(is_same_v<
        type_at_t<list2, 0>,
        int>);
}

TEST(Container_functions, push)
{
    using list1 = Typelist<>;
    using list2 = push_front_t<list1, int, double>;
    using list3 = push_back_t<list2, bool>;

    static_assert(is_same_v<
        list3,
        Typelist<int, double, bool>
        >);
}

TEST(Container_functions, contains)
{
    using list1 = Typelist<>;
    using list2 = Typelist<int>;
    using list3 = Typelist<bool, char, int>;

    static_assert(!contains_v<list1, int>);
    static_assert(contains_v<list2, int>);
    static_assert(!contains_v<list2, float>);
    static_assert(contains_v<list3, int>);
    static_assert(contains_v<list3, bool>);
    static_assert(contains_v<list3, char>);
    static_assert(!contains_v<list3, float>);
}


TEST(Container_functions, tail)
{
    using list1 = Typelist<>;
    using list2 = Typelist<int>;
    using list3 = Typelist<bool, char, int>;

    static_assert(is_same_v<
        tail_t<list1>,
        Typelist<>
    >);

    static_assert(is_same_v<
        tail_t<list2>,
        Typelist<>
    >);

    static_assert(is_same_v<
        tail_t<list3>,
        Typelist<char, int>
    >);
}


TEST(Container_functions, remove)
{
    using list1 = Typelist<>;
    using list2 = Typelist<int>;
    using list3 = Typelist<bool, char, int, int>;

    static_assert(is_same_v<
        remove_t<list1, int>,
        Typelist<>
    >);

    static_assert(is_same_v<
        remove_t<list2, int>,
        Typelist<>
    >);

    static_assert(is_same_v<
        remove_t<list2, double>,
        Typelist<int>
    >);

    static_assert(is_same_v<
        remove_t<list3, char>,
        Typelist<bool, int, int>
    >);

    static_assert(is_same_v<
        remove_t<list3, int>,
        Typelist<bool, char>
    >);
}


TEST(Container_functions, unique)
{
    using list = Typelist<int, char, void, void, char>;

    static_assert(is_same_v<
        unique_t<list>,
        Typelist<int, char, void>
    >);
}

TEST(Container_functions, index)
{
    using list = Typelist<int, char, void, void, char>;

    static_assert(index_v<int, list> == 0);
    static_assert(index_v<char, list> == 1);
    static_assert(index_v<void, list> == 2);
}

TEST(Container_functions, map)
{
    using list = Typelist<int&, char&>;

    static_assert(is_same_v<
        map_t<list, std::remove_reference<void>>,
        Typelist<int, char>
    >);
}


TEST(Container_functions, drop_while_type)
{
    using list1 = Typelist<int, char, void, bool, int, void>;
    using list2 = Typelist<int, char, bool, int, void>;

    static_assert(is_same_v<
        drop_while_type_t<list1, void>,
        Typelist<void, bool, int, void>
    >);

    static_assert(is_same_v<
        drop_while_type_t<list2, void>,
        Typelist<void>
    >);
}

TEST(Container_functions, intersection)
{
    using list1 = Typelist<int, char, void, bool, int, void>;
    using list2 = Typelist<int, char, bool, int, void>;
    using list3 = Typelist<int, char, int>;
    using list4 = Typelist<char>;
    using list5 = Typelist<>;

    static_assert(is_same_v<
        intersection_t<list1, list2>,
        Typelist<int, char, void, bool>
    >);

    static_assert(is_same_v<
        intersection_t<list1, list3>,
        Typelist<int, char>
    >);

    static_assert(is_same_v<
        intersection_t<list2, list1>,
        Typelist<int, char, bool, void>
    >);

    static_assert(is_same_v<
        intersection_t<list2, list5>,
        Typelist<>
    >);

    static_assert(is_same_v<
        intersection_t<list3, list4>,
        Typelist<char>
    >);

    static_assert(is_same_v<
        intersection_t<list1, list2, list3>,
        Typelist<int, char>
    >);

    static_assert(is_same_v<
        intersection_t<list1, list2, list3, list5>,
        Typelist<>
    >);

    static_assert(is_same_v<
        intersection_t<list5, list1, list2, list3>,
        Typelist<>
    >);
}
