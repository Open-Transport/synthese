
/** TransportWebsiteModule class implementation.
	@file TransportWebsiteModule.cpp

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

#include "TransportWebsiteModule.h"

#include "Env.h"
#include "GraphConstants.h"
#include "Request.h"

using namespace std;
using namespace boost;


namespace synthese
{
	using namespace util;
	using namespace graph;
	using namespace server;
	using namespace pt_website;

	template<> const std::string util::FactorableTemplate<ModuleClass,TransportWebsiteModule>::FACTORY_KEY("36_places_list");

	namespace server
	{
		template<> const string ModuleClassTemplate<TransportWebsiteModule>::NAME("Site web transport public");

		template<> void ModuleClassTemplate<TransportWebsiteModule>::PreInit()
		{
		}

		template<> void ModuleClassTemplate<TransportWebsiteModule>::Init()
		{
		}

		template<> void ModuleClassTemplate<TransportWebsiteModule>::Start()
		{
		}

		template<> void ModuleClassTemplate<TransportWebsiteModule>::End()
		{
		}



		template<> void ModuleClassTemplate<TransportWebsiteModule>::InitThread(
			
			){
		}



		template<> void ModuleClassTemplate<TransportWebsiteModule>::CloseThread(
			
			){
		}
	}

	namespace pt_website
	{
		TransportWebsiteModule::UserClassNames TransportWebsiteModule::GetAccessibilityNames()
		{
			UserClassNames result;
			result.push_back(make_pair(USER_PEDESTRIAN, "Piéton"));
			result.push_back(make_pair(USER_HANDICAPPED, "PMR"));
			result.push_back(make_pair(USER_BIKE, "Vélo"));
			return result;
		}
	}
}
