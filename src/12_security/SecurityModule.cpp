////////////////////////////////////////////////////////////////////////////////
/// SecurityModule class implementation.
///	@file SecurityModule.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////


#include "SecurityModule.h"
#include "Factory.h"
#include "01_util/Constants.h"
#include "ProfileTableSync.h"
#include "UserTableSync.h"
#include "User.h"
#include "Profile.h"
#include "GlobalRight.h"

#include <vector>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;

	template<> const string util::FactorableTemplate<ModuleClass, security::SecurityModule>::FACTORY_KEY("12_security");

	namespace security
	{
		const std::string SecurityModule::ROOT_PROFILE = "root";
		const std::string SecurityModule::ROOT_RIGHTS = "*,*,100,100";
		const std::string SecurityModule::ROOT_USER = "root";

		shared_ptr<User> SecurityModule::_rootUser;
		shared_ptr<Profile> SecurityModule::_rootProfile;

		shared_ptr<Profile>	_rootProfile;

		void SecurityModule::initialize()
		{
			Env env;
			ProfileTableSync::Search(env, ROOT_PROFILE);
			if (env.getRegistry<Profile>().empty())
				_rootProfile.reset(new Profile);
			else
				_rootProfile = env.getEditableRegistry<Profile>().front();
	
			_rootProfile->setName(ROOT_PROFILE);
			shared_ptr<Right> r(new GlobalRight);
			r->setPublicLevel(DELETE_RIGHT);
			r->setPrivateLevel(DELETE_RIGHT);
			_rootProfile->cleanRights();
			_rootProfile->addRight(r);
			ProfileTableSync::Save(_rootProfile.get());

			UserTableSync::Search(env, ROOT_USER, ROOT_USER, "%","%", _rootProfile->getKey());
			if (env.getRegistry<User>().empty())
			{
				_rootUser.reset(new User);
				_rootUser->setLogin(ROOT_USER);
				_rootUser->setPassword(ROOT_USER);
			}
			else
				_rootUser = env.getEditableRegistry<User>().front();
			_rootUser->setName(ROOT_USER);
			_rootUser->setProfile(_rootProfile.get());
			_rootUser->setConnectionAllowed(true);
			UserTableSync::Save(_rootUser.get());
		}



		std::vector<pair<string, string> > SecurityModule::getRightsTemplates()
		{
			vector<pair<string, string> > m;
			vector<shared_ptr<Right> > rights(Factory<Right>::GetNewCollection());
			BOOST_FOREACH(const shared_ptr<Right> right, rights)
			{
				m.push_back(make_pair(right->getFactoryKey(), right->getFactoryKey()));
			}
			return m;
		}

		std::vector<pair<uid, std::string> > SecurityModule::getProfileLabels(bool withAll, int first/*=0*/, int last/*=-1*/ )
		{
			vector<pair<uid, string> > m;
			if (withAll)
				m.push_back(make_pair(UNKNOWN_VALUE, "(tous)"));
			
			Env env;
			ProfileTableSync::Search(env);
			BOOST_FOREACH(shared_ptr<Profile> profile, env.getRegistry<Profile>())
			{
				m.push_back(make_pair(profile->getKey(), profile->getName()));
			}
			return m;
		}

		std::vector<pair<uid, std::string> > SecurityModule::getUserLabels(bool withAll, int first/*=0*/, int last/*=-1*/ )
		{
			vector<pair<uid, string> > m;
			if (withAll)
				m.push_back(make_pair(uid(UNKNOWN_VALUE), "(tous)"));

			Env env;
			UserTableSync::Search(env, "%","%","%","%",UNKNOWN_VALUE, false);
			BOOST_FOREACH(shared_ptr<User> user, env.getRegistry<User>())
			{
				m.push_back(make_pair(user->getKey(), user->getSurname() + " " + user->getName()));
			}
			return m;
		}

		std::vector<std::pair<std::string, std::string> > SecurityModule::getRightLabels( bool withAll/*=false*/ )
		{
			vector<pair<string, string> > m;
			if (withAll)
				m.push_back(make_pair("", "(toutes)"));
			vector<shared_ptr<Right> > rights(Factory<Right>::GetNewCollection());
			BOOST_FOREACH(const shared_ptr<Right> right, rights)
			{
				m.push_back(make_pair(right->getFactoryKey(), right->getName()));
			}
			return m;
		}

		std::vector<shared_ptr<Profile> > SecurityModule::getSubProfiles(shared_ptr<const Profile> profile )
		{
			vector<shared_ptr<Profile> > v;
			Env env;
			ProfileTableSync::Search(env, profile);
			BOOST_FOREACH(shared_ptr<Profile> cprofile, env.getRegistry<Profile>())
			{
				 v.push_back(cprofile);
			}
			return v;
		}

		std::string SecurityModule::getName() const
		{
			return "Sécurité";
		}
	}
}
