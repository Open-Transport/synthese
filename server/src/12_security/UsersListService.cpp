
//////////////////////////////////////////////////////////////////////////////////////////
///	UsersListService class implementation.
///	@file UsersListService.cpp
///	@author Hugues Romain
///	@date 2012
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "UsersListService.hpp"

#include "Profile.h"
#include "SecurityRight.h"
#include "Session.h"
#include "RequestException.h"
#include "Request.h"
#include "UserTableSync.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,security::UsersListService>::FACTORY_KEY = "UsersListService";

	namespace security
	{
		const string UsersListService::PARAMETER_LOGIN = "login";
		const string UsersListService::PARAMETER_NAME = "name";
		const string UsersListService::PARAMETER_NUMBER = "number";
		const string UsersListService::PARAMETER_PHONE = "phone";
		const string UsersListService::PARAMETER_SURNAME = "surname";

		const string UsersListService::TAG_USER = "user";



		ParametersMap UsersListService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void UsersListService::_setFromParametersMap(const ParametersMap& map)
		{
			// User id
			RegistryKeyType userId(
				map.getDefault<RegistryKeyType>(Request::PARAMETER_OBJECT_ID, 0)
			);
			if(userId)
			{
				try
				{
					_user = UserTableSync::GetEditable(userId, *_env);
				}
				catch(ObjectNotFoundException<User>&)
				{
					throw RequestException("No such user");
				}
			}

			// Text fields
			_name = map.getDefault<string>(PARAMETER_NAME);
			_surname = map.getDefault<string>(PARAMETER_SURNAME);
			_phone = map.getDefault<string>(PARAMETER_PHONE);
			_login = map.getDefault<string>(PARAMETER_LOGIN);

			// Number
			_number = map.getOptional<size_t>(PARAMETER_NUMBER);
		}



		ParametersMap UsersListService::run(
			std::ostream& stream,
			const Request& request
		) const {

			// Declarations
			ParametersMap result;
			UserTableSync::SearchResult users;

			// Only one specified user
			if(_user.get())
			{
				users.push_back(_user);
			}
			else // User search by criteria
			{
				users = UserTableSync::Search(
					*_env,
					_login.empty() ?
						optional<string>() :
						optional<string>("%"+ _login +"%"),
					_name.empty() ?
						optional<string>() :
						optional<string>("%"+ _name +"%"),
					_surname.empty() ?
						optional<string>() :
						optional<string>("%"+ _surname +"%"),
					_phone.empty() ?
						optional<string>() :
						optional<string>("%"+ _phone +"%"),
					optional<RegistryKeyType>(),
					logic::indeterminate,
					false,
					optional<RegistryKeyType>(),
					0,
					_number,
					false,
					true,
					false,
					true
				);
			}

			// Storage of the result
			BOOST_FOREACH(const boost::shared_ptr<User>& user, users)
			{
				boost::shared_ptr<ParametersMap> userPM(new ParametersMap);
				user->toParametersMap(*userPM, true);
				result.insert(TAG_USER, userPM);
			}

			return result;
		}



		bool UsersListService::isAuthorized(
			const Session* session
		) const {
			return (session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<SecurityRight>(READ)) ||
				(session && session->getUser() && _user && session->getUser()->getKey() == _user->getKey());
		}



		std::string UsersListService::getOutputMimeType() const
		{
			return "text/html";
		}



		UsersListService::UsersListService()
		{
			_env.reset(new Env);
		}
}	}
