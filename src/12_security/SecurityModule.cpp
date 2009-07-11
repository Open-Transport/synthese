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
	using namespace server;
	using namespace security;

	template<> const string util::FactorableTemplate<ModuleClass,SecurityModule>::FACTORY_KEY("12_security");
	
	namespace server
	{
		template<> const string ModuleClassTemplate<SecurityModule>::NAME("Sécurité");
		
		template<> void ModuleClassTemplate<SecurityModule>::PreInit()
		{
		}
		
		template<> void ModuleClassTemplate<SecurityModule>::Init()
		{
			Env env;
			ProfileTableSync::SearchResult rootProfiles(
				ProfileTableSync::Search(env, SecurityModule::ROOT_PROFILE)
			);
			if (rootProfiles.empty())
				SecurityModule::_rootProfile.reset(new Profile);
			else
				SecurityModule::_rootProfile = rootProfiles.front();
	
			SecurityModule::_rootProfile->setName(SecurityModule::ROOT_PROFILE);
			shared_ptr<Right> r(new GlobalRight);
			r->setPublicLevel(DELETE_RIGHT);
			r->setPrivateLevel(DELETE_RIGHT);
			SecurityModule::_rootProfile->cleanRights();
			SecurityModule::_rootProfile->addRight(r);
			ProfileTableSync::Save(SecurityModule::_rootProfile.get());

			UserTableSync::SearchResult rootUsers(
				UserTableSync::Search(
					env,
					SecurityModule::ROOT_USER,
					SecurityModule::ROOT_USER,
					optional<string>(),
					optional<string>(),
					SecurityModule::_rootProfile->getKey()
			)	);
			if (rootUsers.empty())
			{
				SecurityModule::_rootUser.reset(new User);
				SecurityModule::_rootUser->setLogin(SecurityModule::ROOT_USER);
				SecurityModule::_rootUser->setPassword(SecurityModule::ROOT_USER);
			}
			else
				SecurityModule::_rootUser = rootUsers.front();
			SecurityModule::_rootUser->setName(SecurityModule::ROOT_USER);
			SecurityModule::_rootUser->setProfile(SecurityModule::_rootProfile.get());
			SecurityModule::_rootUser->setConnectionAllowed(true);
			UserTableSync::Save(SecurityModule::_rootUser.get());
		}
		
		template<> void ModuleClassTemplate<SecurityModule>::End()
		{
		}
	}

	namespace security
	{
		const std::string SecurityModule::ROOT_PROFILE = "root";
		const std::string SecurityModule::ROOT_RIGHTS = "*,*,100,100";
		const std::string SecurityModule::ROOT_USER = "root";

		shared_ptr<User> SecurityModule::_rootUser;
		shared_ptr<Profile> SecurityModule::_rootProfile;

		shared_ptr<Profile>	_rootProfile;



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
			ProfileTableSync::SearchResult profiles(
				ProfileTableSync::Search(env)
			);
			BOOST_FOREACH(shared_ptr<Profile> profile, profiles)
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
			UserTableSync::SearchResult users(
				UserTableSync::Search(
					env,
					optional<string>(),
					optional<string>(),
					optional<string>(),
					optional<string>(),
					optional<RegistryKeyType>(),
					false
			)	);
			BOOST_FOREACH(shared_ptr<User> user, users)
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
			ProfileTableSync::SearchResult profiles(
				ProfileTableSync::Search(
					env, profile.get() ? profile->getKey() : RegistryKeyType(0),
					0,
					UNKNOWN_VALUE,
					FIELDS_ONLY_LOAD_LEVEL
			)	);
			BOOST_FOREACH(shared_ptr<Profile> cprofile, profiles)
			{
				 v.push_back(cprofile);
			}
			return v;
		}
	}
}
