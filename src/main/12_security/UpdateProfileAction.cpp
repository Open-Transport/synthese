
/** UpdateProfileAction class implementation.
	@file UpdateProfileAction.cpp

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

#include "12_security/UpdateProfileAction.h"
#include "12_security/Profile.h"
#include "12_security/SecurityModule.h"
#include "12_security/ProfileTableSync.h"


#include "30_server/ActionException.h"
#include "30_server/Request.h"

using namespace std;

namespace synthese
{
	using namespace server;
	
	namespace security
	{
		const string UpdateProfileAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "name";


		ParametersMap UpdateProfileAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(make_pair(PARAMETER_NAME, _name));
			return map;
		}

		void UpdateProfileAction::_setFromParametersMap(const ParametersMap& map)
		{
			ParametersMap::const_iterator it;

			// Profile
			try
			{
				_profile = SecurityModule::getProfiles().get(_request->getObjectId());
			}
			catch (Profile::RegistryKeyException e)
			{
				throw ActionException("Profile not found");
			}

			// Name
			it = map.find(PARAMETER_NAME);
			if (it == map.end())
				throw ActionException("Name not specified");
			_name = it->second;

			// Name unicity
			vector<Profile*> existingProfiles = ProfileTableSync::search(_name,"",0,1);
			if (!existingProfiles.empty())
				throw ActionException("Le nom choisi est déjà pris par un autre profil. Veuillez entrer un autre nom.");

		}

		void UpdateProfileAction::run()
		{
			_profile->setName(_name);
			ProfileTableSync::save(_profile);
		}

		UpdateProfileAction::UpdateProfileAction()
			: Action()
			, _profile(NULL)
		{
	
		}
	}
}
