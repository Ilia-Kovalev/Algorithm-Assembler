//
// pch.h
// Header for standard system include files.
//

#pragma once

#include "gtest/gtest.h"

#include <typeinfo>
#include <type_traits>

#include <algorithm_assembler/interfaces.hpp>
#include <algorithm_assembler/detail/data_processor_funcs.hpp>
#include <algorithm_assembler/utils/heterogeneous_container_functions.hpp>
#include <algorithm_assembler/utils/typelist.hpp>
#include <algorithm_assembler/data_processor.hpp>

#define PRINT_TYPE(type) EXPECT_TRUE(false) << typeid(type).name()
#define GET_TYPE(type) typeid(type).name()

using namespace algorithm_assembler::utils;
using namespace algorithm_assembler;
using namespace algorithm_assembler::detail;
namespace aa = algorithm_assembler;
using namespace std;