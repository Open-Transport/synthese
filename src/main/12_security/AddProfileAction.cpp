
/** AddProfileAction class implementation.
	@file AddProfileAction.cpp

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

#include "01_util/Conversion.h"

#include "12_security/Profile.h"
#include "12_security/ProfileTableSync.h"
#include "12_security/AddProfileAction.h"
#include "12_security/SecurityModule.h"
#include "12_security/GlobalRight.h"
#include "12_security/SecurityLog.h"

#include "30_server/ActionException.h"
#include "30_server/Request.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace dblog;
	
	namespace security
	{
		const string AddProfileAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "apan";
		const string AddProfileAction::PARAMETER_TEMPLATE_ID = Action_PARAMETER_PREFIX + "apat";


		ParametersMap AddProfileAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(make_pair(PARAMETER_NAME, _name));
			if (_templateProfile.get())
				map.insert(make_pair(PARAMETER_TEMPLATE_ID, Conversion::ToString(_templateProfile->getKey())));
			return map;
		}

		void AddProfileAction::_setFromParametersMap(const ParametersMap& map)
		{
			ParametersMap::const_iterator it;

			// Name
			it = map.find(PARAMETER_NAME);
			if (it == map.end())
				throw ActionException("Name not specified");
			_name = it->second;

			// Template
			it = map.find(PARAMETER_TEMPLATE_ID);
			if (it != map.end())
			{
				if (!SecurityModule::getProfiles().contains(Conversion::ToLongLong(it->second)))
					throw ActionException("Specified root profile not found.");
				_templateProfile = SecurityModule::getProfiles().get(Conversion::ToLongLong(it->second));
			}

			// Name unicity
			vector<shared_ptr<Profile> > existingProfiles = ProfileTableSync::search(_name,"",0,1);
			if (!existingProfiles.empty())
				throw ActionException("Le nom choisi est déjà pris par un autre profil. Veuillez entrer un autre nom.");
		}

		void AddProfileAction::run()
		{
			shared_ptr<Profile> profile(new Profile);
			profile->setName(_name);
			if (_templateProfile != NULL)
				profile->setParent(_templateProfile->getKey());
			else
			{
				shared_ptr<Right> r = Factory<Right>::create<GlobalRight>();
				r->setPrivateLevel(Right::FORBIDDEN);
				r->setPublicLevel(Right::FORBIDDEN);
				profile->addRight(r);
			}
			ProfileTableSync::save(profile.get());
			_request->setObjectId(profile->getKey());

			// DBLog
			shared_ptr<SecurityLog> log = Factory<DBLog>::create<SecurityLog>();
			log->addProfileAdmin(_request->getUser(), profile, "Création du profil");
		}
	}
}
