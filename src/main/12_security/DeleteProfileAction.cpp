

/** DeleteProfileAction class implementation.
	@file DeleteProfileAction.cpp

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

#include <vector>

#include "01_util/Conversion.h"

#include "12_security/DeleteProfileAction.h"
#include "12_security/SecurityModule.h"
#include "12_security/User.h"
#include "12_security/UserTableSync.h"
#include "12_security/Profile.h"
#include "12_security/ProfileTableSync.h"

#include "30_server/ActionException.h"


using namespace std;

namespace synthese
{
	using namespace server;
	
	namespace security
	{
		DeleteProfileAction::DeleteProfileAction()
			: Action()
			, _profile(NULL)
		{}

		Request::ParametersMap DeleteProfileAction::getParametersMap() const
		{
			Request::ParametersMap map;
			return map;
		}

		void DeleteProfileAction::setFromParametersMap(Request::ParametersMap& map)
		{
			try
			{
				_profile = SecurityModule::getProfiles().get(_request->getObjectId());
			}
			catch (Profile::RegistryKeyException e)
			{
				throw ActionException("Profile not found");
			}

			vector<User*> users = UserTableSync::search("","",_profile->getKey(),0, 1);
			if (users.size() > 0)
				throw ActionException("At least one user belongs to the specified profile. The deletion is forbidden.");
		}

		void DeleteProfileAction::run()
		{
			if (_profile != NULL)
				ProfileTableSync::remove(_profile->getKey());
		}
	}
}
