
/** VehiclePosition class implementation.
	@file VehiclePosition.cpp

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

#include "VehiclePosition.hpp"

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;

	namespace util
	{
		template<>
		const std::string Registry<pt_operation::VehiclePosition>::KEY("VehiclePosition");
	}

	namespace pt_operation
	{
		VehiclePosition::VehiclePosition(
			RegistryKeyType id
		):	Registrable(id),
			_status(UNKNOWN_STATUS),
			_vehicle(NULL),
			_time(not_a_date_time),
			_meterOffset(0),
			_stopPoint(NULL),
			_service(NULL),
			_passengers(0),
			_depot(NULL)
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
	}
}
