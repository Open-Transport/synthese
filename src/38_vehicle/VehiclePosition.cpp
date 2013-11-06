/** VehiclePosition class implementation.
	@file VehiclePosition.cpp

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

#include "VehiclePosition.hpp"

#include "ParametersMap.h"
#include "StopPoint.hpp"

#include <geos/geom/Point.h>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace vehicle;

	namespace util
	{
		template<>
		const std::string Registry<VehiclePosition>::KEY("VehiclePosition");
	}

	namespace vehicle
	{
		const string VehiclePosition::ATTR_X = "x";
		const string VehiclePosition::ATTR_Y = "y";
		const string VehiclePosition::ATTR_METER_OFFSET = "meter_offset";
		const string VehiclePosition::ATTR_STATUS = "status";
		const string VehiclePosition::TAG_STOP = "stop";
		const string VehiclePosition::ATTR_IN_STOP_AREA = "in_stop_area";



		VehiclePosition::VehiclePosition(
			RegistryKeyType id
		):	Registrable(id),
			_status(UNKNOWN_STATUS),
			_vehicle(NULL),
			_time(not_a_date_time),
			_meterOffset(0),
			_stopPoint(NULL),
			_depot(NULL),
			_service(NULL),
			_passengers(0),
			_inStopArea(false)
		{}



		std::string VehiclePosition::GetStatusName( Status value )
		{
			switch(value)
			{
			case TRAINING: return "Formation";
			case DEAD_RUN_DEPOT: return "Transfert dépôt";
			case DEAD_RUN_TRANSFER: return "Transfert inter-ligne";
			case SERVICE: return "Trajet de service";
			case COMMERCIAL: return "Service commercial";
			case NOT_IN_SERVICE: return "Arrêt";
			case OUT_OF_SERVICE: return "Hors service";
			case UNKNOWN_STATUS: return "Inconnu";
			case REFUELING: return "Carburant";
			case TERMINUS_START: return "Départ terminus";
			}
			return string();
		}



		vector<pair<optional<VehiclePosition::Status>, string> > VehiclePosition::GetStatusList()
		{
			vector<pair<optional<Status>,string> > result;
			for(int i(0); i<9; ++i)
			{
				result.push_back(make_pair(optional<Status>(static_cast<Status>(i)), GetStatusName(static_cast<Status>(i))));
			}
			return result;
		}



		void VehiclePosition::toParametersMap(
			util::ParametersMap& pm,
			bool withAdditionalParameters,
			boost::logic::tribool withFiles,
			std::string prefix
		) const	{
			if(hasGeometry())
			{
				pm.insert(ATTR_X, getGeometry()->getX());
				pm.insert(ATTR_Y, getGeometry()->getY());
			}
			pm.insert(ATTR_METER_OFFSET, _meterOffset);
			pm.insert(ATTR_STATUS, _status);
			pm.insert(ATTR_IN_STOP_AREA, _inStopArea);
			if(_stopPoint)
			{
				boost::shared_ptr<ParametersMap> stopPM(new ParametersMap);
				_stopPoint->toParametersMap(*stopPM, false);
				pm.insert(TAG_STOP, stopPM);
			}
		}
}	}
