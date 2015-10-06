
/** VehicleCall class implementation.
	@file VehicleCall.cpp

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

#include "VehicleCall.hpp"

#include "ImportableTableSync.hpp"
#include "Profile.h"
#include "User.h"

using namespace boost;
using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace vehicle;
	using namespace util;

	CLASS_DEFINITION(VehicleCall, "t119_vehicle_calls", 119)
	FIELD_DEFINITION_OF_TYPE(Vehicle, "vehicle_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(CallTime, "call_time", SQL_DATETIME)
	FIELD_DEFINITION_OF_TYPE(Priority, "priority", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(ClosureTime, "closure_time", SQL_DATETIME)



	namespace vehicle
	{
		VehicleCall::VehicleCall(
			RegistryKeyType id
		):	Registrable(id),
			Object<VehicleCall, VehicleCallSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Vehicle),
					FIELD_DEFAULT_CONSTRUCTOR(CallTime),
					FIELD_DEFAULT_CONSTRUCTOR(Priority),
					FIELD_DEFAULT_CONSTRUCTOR(ClosureTime)
			)	)
		{}



		void VehicleCall::addAdditionalParameters(
			util::ParametersMap& map,
			std::string prefix /*= std::string() */
		) const	{
		}



		void VehicleCall::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
		}



		void VehicleCall::unlink()
		{
		}



		VehicleCall::~VehicleCall()
		{
			unlink();
		}


		bool VehicleCall::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool VehicleCall::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool VehicleCall::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::DELETE_RIGHT);
		}
}	}
