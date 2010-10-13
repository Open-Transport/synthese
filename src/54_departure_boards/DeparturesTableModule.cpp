
/** DeparturesTableModule class implementation.
	@file DeparturesTableModule.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#include "StopArea.hpp"
#include "CommercialLine.h"

#include "DeparturesTableModule.h"
#include "AdvancedSelectTableSync.h"
#include "DisplayType.h"
#include "DisplayTypeTableSync.h"
#include "DisplayScreen.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace pt;
	using namespace security;
	using namespace util;
	using namespace departure_boards;
	using namespace server;

	namespace util
	{
		template<> const std::string util::FactorableTemplate<ModuleClass, departure_boards::DeparturesTableModule>::FACTORY_KEY("54_departures_table");
	}
	
	namespace server
	{
		template<> const string ModuleClassTemplate<DeparturesTableModule>::NAME("SAI Tableaux de départs");
		
		template<> void ModuleClassTemplate<DeparturesTableModule>::PreInit()
		{
		}
		
		template<> void ModuleClassTemplate<DeparturesTableModule>::Init()
		{
		}
		
		template<> void ModuleClassTemplate<DeparturesTableModule>::End()
		{
		}
	}

	namespace departure_boards
	{
		DeparturesTableModule::Labels DeparturesTableModule::getDisplayTypeLabels(
			bool withAll, /*= false*/
			bool withNone
		){
			Labels m;
			if (withAll)
			{
				m.push_back(make_pair(optional<RegistryKeyType>(), "(tous)"));
			}
			if (withNone)
			{
				m.push_back(make_pair(0, "(non défini)"));
			}
			DisplayTypeTableSync::SearchResult types(
				DisplayTypeTableSync::Search(Env::GetOfficialEnv())
			);
			BOOST_FOREACH(shared_ptr<DisplayType> displayType, types)
			{
				m.push_back(make_pair(displayType->getKey(), displayType->getName()));
			}
			return m;
		}

		DeparturesTableModule::Labels DeparturesTableModule::getPlacesWithBroadcastPointsLabels(
			const security::RightsOfSameClassMap& rights 
			, bool totalControl 
			, RightLevel neededLevel
			, bool withAll /*= false*/
		){
			Labels localizations;
			if (withAll)
				localizations.push_back(make_pair(optional<RegistryKeyType>(), "(tous)"));
			std::vector<shared_ptr<ConnectionPlaceWithBroadcastPoint> > bpv = searchConnectionPlacesWithBroadcastPoints(
				Env::GetOfficialEnv(),
				rights,
				totalControl,
				neededLevel,
				string(),
				string(),
				AT_LEAST_ONE_BROADCASTPOINT
			);
			BOOST_FOREACH(shared_ptr<ConnectionPlaceWithBroadcastPoint> con, bpv)
			{
				localizations.push_back(make_pair(con->place->getKey(), con->cityName + " " + con->place->getName()));
			}
			return localizations;
		}



		DeparturesTableModule::Labels DeparturesTableModule::getCommercialLineWithBroadcastLabels( bool withAll /*= false*/ )
		{
			Env env;
			Labels m;
			if (withAll)
			{
				m.push_back(make_pair(optional<RegistryKeyType>(), "(toutes)"));
			}
			vector<shared_ptr<const CommercialLine> > c = getCommercialLineWithBroadcastPoints(env);
			BOOST_FOREACH(shared_ptr<const CommercialLine> line, c)
			{
				m.push_back(make_pair(line->getKey(), line->getShortName()));
			}
			return m;
		}
	}
}
