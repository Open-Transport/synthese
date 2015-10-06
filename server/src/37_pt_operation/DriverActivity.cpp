
/** DriverActivity class implementation.
	@file DriverActivity.cpp

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

#include "DriverActivity.hpp"

#include "Profile.h"
#include "Session.h"
#include "User.h"

using namespace boost::gregorian;
using namespace boost::posix_time;

namespace synthese
{
	using namespace impex;
	using namespace pt_operation;
	using namespace util;

	CLASS_DEFINITION(DriverActivity, "t087_driver_activites", 87)
	FIELD_DEFINITION_OF_OBJECT(DriverActivity, "driver_activity_id", "driver_activity_ids")

	namespace pt_operation
	{
		DriverActivity::DriverActivity(
			RegistryKeyType id
		):	Registrable(id),
			Object<DriverActivity, DriverActivitySchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_DEFAULT_CONSTRUCTOR(impex::DataSourceLinks)
			)	)
		{}


		bool DriverActivity::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool DriverActivity::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool DriverActivity::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::DELETE_RIGHT);
		}
}	}

