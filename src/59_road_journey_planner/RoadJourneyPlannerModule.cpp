
/** RoadJourneyPlannerModule class implementation.
	@file RoadJourneyPlannerModule.cpp
	@author Hugues Romain
	@date 2010
	@since 3.2.0

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

#include "RoadJourneyPlannerModule.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace road_journey_planner;
	using namespace util;

	namespace util
	{
		template<>
		const string FactorableTemplate<ModuleClass,RoadJourneyPlannerModule>::FACTORY_KEY("59_road_journey_planner");
	}

	namespace server
	{
		template<> const string ModuleClassTemplate<RoadJourneyPlannerModule>::NAME("Calcul d'itinéraires routier");

		template<> void ModuleClassTemplate<RoadJourneyPlannerModule>::PreInit()
		{
		}

		template<> void ModuleClassTemplate<RoadJourneyPlannerModule>::Init()
		{
		}

		template<> void ModuleClassTemplate<RoadJourneyPlannerModule>::Start()
		{
		}

		template<> void ModuleClassTemplate<RoadJourneyPlannerModule>::End()
		{
		}



		template<> void ModuleClassTemplate<RoadJourneyPlannerModule>::InitThread(
			
			){
		}



		template<> void ModuleClassTemplate<RoadJourneyPlannerModule>::CloseThread(
			
			){
		}
	}

	namespace road_journey_planner
	{

	}
}


