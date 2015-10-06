
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
#include "Profile.h"
#include "Session.h"
#include "User.h"
#include "VehicleModule.hpp"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace vehicle;

	CLASS_DEFINITION(Vehicle, "t069_vehicles", 69)
	FIELD_DEFINITION_OF_TYPE(Number, "number", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(AllowedLines, "allowed_lines", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(Seats, "seats", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(Picture, "picture", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(Available, "available", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(URL, "url", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(RegistrationNumber, "registration_numbers", SQL_TEXT)


	namespace vehicle
	{
		const string Vehicle::TAG_SEAT = "seat";



		Vehicle::Vehicle(
			RegistryKeyType id
		):	Registrable(id),
			Object<Vehicle, VehicleRecord>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_DEFAULT_CONSTRUCTOR(Number),
					FIELD_DEFAULT_CONSTRUCTOR(AllowedLines),
					FIELD_DEFAULT_CONSTRUCTOR(Seats),
					FIELD_DEFAULT_CONSTRUCTOR(Picture),
					FIELD_VALUE_CONSTRUCTOR(Available, true),
					FIELD_DEFAULT_CONSTRUCTOR(URL),
					FIELD_DEFAULT_CONSTRUCTOR(RegistrationNumber),
					FIELD_DEFAULT_CONSTRUCTOR(impex::DataSourceLinks),
					FIELD_DEFAULT_CONSTRUCTOR(inter_synthese::InterSYNTHESESlave)
			)	)
		{}



		void Vehicle::unlink()
		{
			VehicleModule::UnregisterVehicle(*this);
		}



		void Vehicle::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			if(&env == &Env::GetOfficialEnv())
			{
				VehicleModule::RegisterVehicle(*this);
			}
		}



		void Vehicle::addAdditionalParameters( util::ParametersMap& pm, std::string prefix /*= std::string() */ ) const
		{
			// Seats
			BOOST_FOREACH(const string& seat, get<Seats>())
			{
				boost::shared_ptr<ParametersMap> seatPM(new ParametersMap);
				seatPM->insert(TAG_SEAT, seat);
				pm.insert(prefix + TAG_SEAT, seatPM);
			}
		}


		bool Vehicle::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool Vehicle::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool Vehicle::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::DELETE_RIGHT);
		}
}	}
