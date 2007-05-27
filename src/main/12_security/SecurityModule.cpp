
/** SecurityModule class implementation.
	@file SecurityModule.cpp

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

#include "01_util/Factory.h"
#include "01_util/Constants.h"

#include "12_security/ProfileTableSync.h"
#include "12_security/UserTableSync.h"
#include "12_security/User.h"
#include "12_security/Profile.h"
#include "12_security/SecurityModule.h"
#include "12_security/GlobalRight.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace util;

	namespace security
	{
		const std::string SecurityModule::ROOT_PROFILE = "root";
		const std::string SecurityModule::ROOT_RIGHTS = "*,*,100,100";
		const std::string SecurityModule::ROOT_USER = "root";

		Profile::Registry SecurityModule::_profiles;
		shared_ptr<User> SecurityModule::_rootUser;
		shared_ptr<Profile> SecurityModule::_rootProfile;

		shared_ptr<Profile>	_rootProfile;

		void SecurityModule::initialize()
		{
			vector<shared_ptr<Profile> > profiles = ProfileTableSync::search(ROOT_PROFILE);
			if (profiles.size() == 0)
				_rootProfile.reset(new Profile);
			else
				_rootProfile = profiles.front();
	
			_rootProfile->setName(ROOT_PROFILE);
			shared_ptr<Right> r = Factory<Right>::create<GlobalRight>();
			r->setPublicLevel(DELETE);
			r->setPrivateLevel(DELETE);
			_rootProfile->cleanRights();
			_rootProfile->addRight(r);
			ProfileTableSync::save(_rootProfile.get());

			vector<shared_ptr<User> > users = UserTableSync::search(ROOT_USER, ROOT_USER, _rootProfile);
			if (users.size() == 0)
				_rootUser.reset(new User);
			else
				_rootUser = users.front();
			_rootUser->setName(ROOT_USER);
			_rootUser->setLogin(ROOT_USER);
			_rootUser->setPassword(ROOT_USER);
			_rootUser->setProfile(_rootProfile);
			_rootUser->setConnectionAllowed(true);
			UserTableSync::save(_rootUser.get());
		}

		Profile::Registry& SecurityModule::getProfiles()
		{
			return _profiles;
		}

		std::vector<pair<string, string> > SecurityModule::getRightsTemplates()
		{
			vector<pair<string, string> > m;
			for (Factory<Right>::Iterator it = Factory<Right>::begin(); it != Factory<Right>::end(); ++it)
				m.push_back(make_pair(it.getKey(), it.getKey()));
			return m;
		}

		std::vector<pair<uid, std::string> > SecurityModule::getProfileLabels(bool withAll, int first/*=0*/, int last/*=-1*/ )
		{
			vector<pair<uid, string> > m;
			if (withAll)
				m.push_back(make_pair(0, "(tous)"));
			for (Profile::Registry::const_iterator it = _profiles.begin(); it != _profiles.end(); ++it)
				m.push_back(make_pair(it->first, it->second->getName()));
			return m;
		}

		std::vector<pair<uid, std::string> > SecurityModule::getUserLabels(bool withAll, int first/*=0*/, int last/*=-1*/ )
		{
			vector<pair<uid, string> > m;
			if (withAll)
				m.push_back(make_pair(uid(0), "(tous)"));
			vector<shared_ptr<User> > users = UserTableSync::search("","", shared_ptr<const Profile>(), false);
			for (vector<shared_ptr<User> >::iterator it = users.begin(); it != users.end(); ++it)
				m.push_back(make_pair((*it)->getKey(), (*it)->getSurname() + " " + (*it)->getName()));
			return m;
		}

		std::vector<std::pair<std::string, std::string> > SecurityModule::getRightLabels( bool withAll/*=false*/ )
		{
			vector<pair<string, string> > m;
			if (withAll)
				m.push_back(make_pair("", "(toutes)"));
			for (Factory<Right>::Iterator it = Factory<Right>::begin(); it != Factory<Right>::end(); ++it)
				m.push_back(make_pair(it.getKey(), it->getName()));
			return m;
		}

		std::vector<shared_ptr<const Profile> > SecurityModule::getSubProfiles(shared_ptr<const Profile> profile )
		{
			vector<shared_ptr<const Profile> > v;
			for (Profile::Registry::const_iterator it = _profiles.begin(); it != _profiles.end(); ++it)
			{
				if (!profile.get())
				{
					if (!it->second->getParentId())
						v.push_back(it->second);
				}
				else
				{
					 if (it->second->getParentId() == profile->getKey())
						v.push_back(it->second);
				}
			}
			return v;
		}
	}
}
