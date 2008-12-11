
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

#include "UId.h"

#include "PublicTransportStopZoneConnectionPlace.h"
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
	using namespace env;
	using namespace security;

	namespace util
	{
		template<> const std::string util::FactorableTemplate<ModuleClass, departurestable::DeparturesTableModule>::FACTORY_KEY("54_departures_table");
	}

	namespace departurestable
	{
		std::vector<pair<uid, std::string> > DeparturesTableModule::getDisplayTypeLabels( bool withAll /*= false*/ )
		{
			vector<pair<uid, string> > m;
			if (withAll)
				m.push_back(make_pair(UNKNOWN_VALUE, "(tous)"));
			Env env;
			DisplayTypeTableSync::Search(env);
			BOOST_FOREACH(shared_ptr<DisplayType> displayType, env.getRegistry<DisplayType>())
			{
				m.push_back(make_pair(displayType->getKey(), displayType->getName()));
			}
			return m;
		}

		std::vector<pair<uid, std::string> > DeparturesTableModule::getPlacesWithBroadcastPointsLabels(
			const security::RightsOfSameClassMap& rights 
			, bool totalControl 
			, RightLevel neededLevel
			, bool withAll /*= false*/
		){
			vector<pair<uid, string> > localizations;
			if (withAll)
				localizations.push_back(make_pair(UNKNOWN_VALUE, "(tous)"));
			std::vector<shared_ptr<ConnectionPlaceWithBroadcastPoint> > bpv = searchConnectionPlacesWithBroadcastPoints(rights, totalControl, neededLevel, string(), string(), AT_LEAST_ONE_BROADCASTPOINT);
			for (vector<shared_ptr<ConnectionPlaceWithBroadcastPoint> >::const_iterator it = bpv.begin(); it != bpv.end(); ++it)
			{
				const shared_ptr<ConnectionPlaceWithBroadcastPoint>& con = *it;
				localizations.push_back(make_pair(con->place->getKey(), con->cityName + " " + con->place->getName()));
			}
			return localizations;
		}

		std::vector<pair<uid, std::string> > DeparturesTableModule::getCommercialLineWithBroadcastLabels( bool withAll /*= false*/ )
		{
			vector<pair<uid, string> > m;
			if (withAll)
				m.push_back(make_pair(UNKNOWN_VALUE, "(toutes)"));
			vector<shared_ptr<const CommercialLine> > c = getCommercialLineWithBroadcastPoints();
			for (vector<shared_ptr<const CommercialLine> >::const_iterator it = c.begin(); it != c.end(); ++it)
				m.push_back(make_pair((*it)->getKey(), (*it)->getShortName()));
			return m;
		}

		void DeparturesTableModule::initialize()
		{

		}

		std::string DeparturesTableModule::getName() const
		{
			return "SAI Tableaux de départs";
		}
	}
}
