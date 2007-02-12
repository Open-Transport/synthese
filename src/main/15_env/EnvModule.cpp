
/** EnvModule class implementation.
	@file EnvModule.cpp

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

#include "01_util/Constants.h"
#include "01_util/UId.h"

#include "15_env/EnvModule.h"

using namespace std;

namespace synthese
{
	namespace env
	{
		Environment::Registry			EnvModule::_environments;
		City::Registry					EnvModule::_cities;
		ConnectionPlace::Registry		EnvModule::_connectionPlaces;
		PhysicalStop::Registry			EnvModule::_physicalStops;
		CommercialLine::Registry		EnvModule::_commercialLines;
		Line::Registry					EnvModule::_lines;
		Axis::Registry					EnvModule::_axes;
		Fare::Registry					EnvModule::_fares;
		BikeCompliance::Registry		EnvModule::_bikeCompliances;
		HandicappedCompliance::Registry	EnvModule::_handicappedCompliances;
		PedestrianCompliance::Registry	EnvModule::_pedestrianCompliances;
		ReservationRule::Registry		EnvModule::_reservationRules;

		void EnvModule::initialize()
		{
		}


		Environment::Registry& 
			EnvModule::getEnvironments ()
		{
			return _environments;
		}

		City::Registry& EnvModule::getCities()
		{
			return _cities;
		}

		ConnectionPlace::Registry& EnvModule::getConnectionPlaces()
		{
			return _connectionPlaces;
		}

		PhysicalStop::Registry& EnvModule::getPhysicalStops()
		{
			return _physicalStops;
		}

		EnvModule::~EnvModule()
		{
			_physicalStops.clear ();
			_connectionPlaces.clear ();
			_cities.clear ();
			_commercialLines.clear();
		}

		CommercialLine::Registry& EnvModule::getCommercialLines()
		{
			return _commercialLines;
		}

		Line::Registry& EnvModule::getLines()
		{
			return _lines;
		}

		Axis::Registry& EnvModule::getAxes()
		{
			return _axes;
		}

		Fare::Registry& EnvModule::getFares()
		{
			return _fares;
		}

		BikeCompliance::Registry& EnvModule::getBikeCompliances()
		{
			return _bikeCompliances;
		}

		HandicappedCompliance::Registry& EnvModule::getHandicappedCompliances()
		{
			return _handicappedCompliances;
		}

		PedestrianCompliance::Registry& EnvModule::getPedestrianCompliances()
		{
			return _pedestrianCompliances;
		}

		ReservationRule::Registry& EnvModule::getReservationRules()
		{
			return _reservationRules;
		}


		Path* 
			EnvModule::fetchPath (const uid& id)
		{
			if (_lines.contains (id)) return _lines.get (id);
	//		if (_roads.contains (id)) return _roads.get (id);
			return 0;
		}

		std::map<uid, std::string> EnvModule::getCommercialLineLabels(bool withAll)
		{
			map<uid,string> m;
			if (withAll)
				m.insert(make_pair(UNKNOWN_VALUE, "(toutes)"));
			for(CommercialLine::Registry::const_iterator it = _commercialLines.begin(); it != _commercialLines.end(); ++it)
				m.insert(make_pair(it->first, it->second->getShortName()));
			return m;
		}
	}
}
