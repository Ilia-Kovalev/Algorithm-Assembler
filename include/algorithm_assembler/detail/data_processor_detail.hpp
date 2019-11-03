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

#ifndef DATA_PROCESSOR_DETAIL_HPP
#define DATA_PROCESSOR_DETAIL_HPP


#include <tuple>

#include "../utils/typelist.hpp"
#include "../interfaces.hpp"
#include "data_processor_funcs.hpp"

namespace algorithm_assembler::detail
{
	template<class... Modules>
	class DP_Modules
	{
	protected:
		std::tuple<Modules...> modules_;
	};

	template<typename In_typelist, typename Out_type, class... Modules> class DP_Functor;

	template<typename... In_types, typename Out_type, class... Modules>
	class DP_Functor<utils::Typelist<In_types...>, Out_type, Modules...> :
		public algorithm_assembler::Functor<Out_type, In_types...>,
		public virtual DP_Modules<Modules...>
	{
	public:
		inline Out_type operator()(In_types... ins) override
		{
			auto ins_tuple&& = std::forward_as_tuple(ins...);

			return process_data(
				std::forward<std::tuple<In_types...>>(ins_tuple), 
				std::get<Modules>(modules_)...
			);
		}
	};





}



#endif
