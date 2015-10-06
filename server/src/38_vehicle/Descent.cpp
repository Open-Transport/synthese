
/** Descent class implementation.
	@file Descent.cpp

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

#include "Descent.hpp"

#include "ImportableTableSync.hpp"
#include "Profile.h"

using namespace boost;
using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace pt;
	using namespace vehicle;
	using namespace util;

	CLASS_DEFINITION(Descent, "t118_descents", 118)
	FIELD_DEFINITION_OF_TYPE(Service, "service_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(Stop, "stop_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(ActivationTime, "activation_time", SQL_DATETIME)
	FIELD_DEFINITION_OF_TYPE(ActivationUser, "activation_user_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(CancellationTime, "cancellation_time", SQL_DATETIME)
	FIELD_DEFINITION_OF_TYPE(CancellationUser, "cancellation_user_id", SQL_INTEGER)


	namespace vehicle
	{
		const string Descent::TAG_SERVICE = "service";
		const string Descent::TAG_STOP_POINT = "stop_point";

		Descent::Descent(
			RegistryKeyType id
		):	Registrable(id),
			Object<Descent, DescentSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Service),
					FIELD_DEFAULT_CONSTRUCTOR(Stop),
					FIELD_DEFAULT_CONSTRUCTOR(Date),
					FIELD_DEFAULT_CONSTRUCTOR(ActivationTime),
					FIELD_DEFAULT_CONSTRUCTOR(CancellationTime),
					FIELD_DEFAULT_CONSTRUCTOR(ActivationUser),
					FIELD_DEFAULT_CONSTRUCTOR(CancellationUser)
			)	)
		{}



		void Descent::addAdditionalParameters(
			util::ParametersMap& map,
			std::string prefix /*= std::string() */
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



		void Descent::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
		}



		void Descent::unlink()
		{
		}



		Descent::~Descent()
		{
		unlink();
		}


		bool Descent::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool Descent::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool Descent::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::DELETE_RIGHT);
		}
}	}
