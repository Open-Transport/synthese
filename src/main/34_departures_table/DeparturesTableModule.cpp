
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

#include "01_util/UId.h"

#include "15_env/ConnectionPlace.h"

#include "34_departures_table/DeparturesTableModule.h"
#include "34_departures_table/AdvancedSelectTableSync.h"

using namespace std;

namespace synthese
{
	using namespace env;

	namespace departurestable
	{
		DisplayType::Registry DeparturesTableModule::_displayTypes;
		BroadcastPoint::Registry	DeparturesTableModule::_broadcastPoints;
		DisplayScreen::Registry	DeparturesTableModule::_displayScreens;

		DisplayType::Registry& DeparturesTableModule::getDisplayTypes()
		{
			return _displayTypes;
		}

		BroadcastPoint::Registry& DeparturesTableModule::getBroadcastPoints()
		{
			return _broadcastPoints;
		}

		DisplayScreen::Registry& DeparturesTableModule::getDisplayScreens()
		{
			return _displayScreens;
		}

		std::map<uid, std::string> DeparturesTableModule::getDisplayTypeLabels( bool withAll /*= false*/ )
		{
			map<uid,string> m;
			if (withAll)
				m.insert(make_pair(UNKNOWN_VALUE, "(tous)"));
			for(DisplayType::Registry::const_iterator it = _displayTypes.begin(); it != _displayTypes.end(); ++it)
				m.insert(make_pair(it->first, it->second->getName()));
			return m;
		}

		std::map<uid, std::string> DeparturesTableModule::getPlacesWithBroadcastPointsLabels( bool withAll /*= false*/ )
		{
			map<uid, string> localizations;
			if (withAll)
				localizations.insert(make_pair(UNKNOWN_VALUE, "(tous)"));
			std::vector<ConnectionPlaceWithBroadcastPoint> bpv = searchConnectionPlacesWithBroadcastPoints("", "", AT_LEAST_ONE_BROADCASTPOINT);
			for (vector<ConnectionPlaceWithBroadcastPoint>::const_iterator it = bpv.begin(); it != bpv.end(); ++it)
				localizations.insert(make_pair(it->place->getKey(), it->place->getFullName()));
			return localizations;
		}

		std::map<uid, std::string> DeparturesTableModule::getBroadcastPointLabels( const ConnectionPlace* place, bool withAll /*= false*/ )
		{
			map<uid, string> m;
			if (withAll)
				m.insert(make_pair(UNKNOWN_VALUE, "(tous)"));
			vector<PhysicalStopAndBroadcastPoint> b = getConnectionPlaceBroadcastPointsAndPhysicalStops(place->getKey(), true);
			for (vector<PhysicalStopAndBroadcastPoint>::const_iterator it = b.begin(); it != b.end(); ++it)
				m.insert(make_pair(it->bp->getKey(), it->bp->getName()));
			return m;
		}
	}
}
