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

#ifndef DATA_PROCESSOR_HPP
#define DATA_PROCESSOR_HPP

#include "detail/data_processor_detail.hpp"

namespace algorithm_assembler
{
	template<class Module, class... Modules>
	class Data_processor : 
		public detail::DP_Functor<
			typename Module::Input_types,
			typename utils::Typelist<Module, Modules...>::back::Output_type,
			utils::Typelist<Module, Modules...>
		>
	{};
}

#endif