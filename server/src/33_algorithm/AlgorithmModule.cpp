
/** AlgorithmModule class implementation.
	@file AlgorithmModule.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "AlgorithmModule.h"

#include <boost/lexical_cast.hpp>

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace algorithm;

	namespace util
	{
		template<> const string FactorableTemplate<ModuleClass, AlgorithmModule>::FACTORY_KEY = "33_algorithm";
	}

	namespace server
	{
		template<> const string ModuleClassTemplate<AlgorithmModule>::NAME("Algorithmes");

		template<> void ModuleClassTemplate<AlgorithmModule>::PreInit()
		{
			RegisterParameter(AlgorithmModule::MODULE_PARAM_USE_ASTAR_FOR_PHYSICAL_STOPS_EXTENDER, "0", &AlgorithmModule::ParameterCallback);
		}



		template<> void ModuleClassTemplate<AlgorithmModule>::Init()
		{
		}



		template<> void ModuleClassTemplate<AlgorithmModule>::Start()
		{
		}



		template<> void ModuleClassTemplate<AlgorithmModule>::End()
		{
			UnregisterParameter(AlgorithmModule::MODULE_PARAM_USE_ASTAR_FOR_PHYSICAL_STOPS_EXTENDER);
		}



		template<> void ModuleClassTemplate<AlgorithmModule>::InitThread(
			
			){
		}




		template<> void ModuleClassTemplate<AlgorithmModule>::CloseThread(
			
			){
		}
	}

	namespace algorithm
	{
		const string AlgorithmModule::MODULE_PARAM_USE_ASTAR_FOR_PHYSICAL_STOPS_EXTENDER = "astar_for_walk";

		bool AlgorithmModule::_useAStarForPhysicalStopsExtender = false;
		void AlgorithmModule::ParameterCallback(
			const string& name,
			const string& value
		){
			if(name == MODULE_PARAM_USE_ASTAR_FOR_PHYSICAL_STOPS_EXTENDER)
			{
				_useAStarForPhysicalStopsExtender = !value.empty() && boost::lexical_cast<bool>(value);
			}
		}
	}
}
