/** Alert class implementation.
	@file Alert.cpp

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

#include "Alert.hpp"

#include "Profile.h"
#include "Session.h"
#include "User.h"


using namespace boost;
using namespace std;
using namespace boost::posix_time;

namespace synthese
{

    using namespace regulation;
	using namespace util;

	CLASS_DEFINITION(Alert, "t121_alerts", 121)
    FIELD_DEFINITION_OF_TYPE(Kind, "kind", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(Line, "line_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(Service, "service_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(ExtraData, "extra_data", SQL_TEXT)

	namespace regulation
	{

		Alert::Alert(
			RegistryKeyType id
		):	Registrable(id),
			Object<Alert, AlertSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Kind),
					FIELD_DEFAULT_CONSTRUCTOR(Line),
					FIELD_DEFAULT_CONSTRUCTOR(Service),
					FIELD_DEFAULT_CONSTRUCTOR(ExtraData)
                    )
                )
		{}



        /*
		void Alert::addAdditionalParameters(
			util::ParametersMap& map,
			std::string prefix
		) const	{

			// Service detail
			boost::shared_ptr<ParametersMap> serviceMap(new ParametersMap);
			get<Service>()->toParametersMap(*serviceMap, false);
			map.insert(prefix + TAG_SERVICE, serviceMap);

			// Stop detail
			boost::shared_ptr<ParametersMap> stopMap(new ParametersMap);
			get<Stop>()->toParametersMap(*stopMap, false);
			map.insert(prefix + TAG_STOP_POINT, stopMap);
		}
        */


		Alert::~Alert()
		{

		}


		bool Alert::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool Alert::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool Alert::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::DELETE_RIGHT);
		}
}	}
