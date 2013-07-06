
/** Vehicle class implementation.
	@file Vehicle.cpp

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

#include "Vehicle.hpp"

#include "ParametersMap.h"

using namespace std;

namespace synthese
{
	using namespace util;

	namespace util
	{
		template<>
		const std::string Registry<vehicle::Vehicle>::KEY("Vehicle");
	}

	namespace vehicle
	{
		const string Vehicle::DATA_NAME = "name";
		const string Vehicle::DATA_NUMBER = "number";
		const string Vehicle::DATA_PICTURE = "picture";
		const string Vehicle::DATA_SEATS = "seats";
		const string Vehicle::DATA_VEHICLE_ID = "vehicle_id";



		Vehicle::Vehicle(RegistryKeyType id):
			Registrable(id),
			_available(true)
		{}



		void Vehicle::toParametersMap(
			util::ParametersMap& pm,
			bool withAdditionalParameters,
			boost::logic::tribool withFiles,
			std::string prefix
		) const	{
			pm.insert(DATA_NAME, getName());
			pm.insert(DATA_NUMBER, getNumber());
			pm.insert(DATA_PICTURE, getPicture());
			pm.insert(DATA_SEATS, getSeats().size());
			pm.insert(DATA_VEHICLE_ID, getKey());
		}
}	}
