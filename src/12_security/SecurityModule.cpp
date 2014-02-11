////////////////////////////////////////////////////////////////////////////////
/// SecurityModule class implementation.
///	@file SecurityModule.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////


#include "SecurityModule.h"
#include "Factory.h"
#include "UtilConstants.h"
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
	using namespace db;
	using namespace util;
	using namespace server;
	using namespace security;

	template<> const string util::FactorableTemplate<ModuleClass,SecurityModule>::FACTORY_KEY("12_security");

	namespace server
	{
		template<> const string ModuleClassTemplate<SecurityModule>::NAME("Sécurité");

		template<> void ModuleClassTemplate<SecurityModule>::PreInit()
		{
			RegisterParameter(SecurityModule::MODULE_PARAMETER_SENDER_EMAIL, "", &SecurityModule::ParameterCallback);
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

			DBTransaction transaction;

			SecurityModule::_rootProfile->setName(SecurityModule::ROOT_PROFILE);
			boost::shared_ptr<Right> r(new GlobalRight);
			r->setPublicLevel(DELETE_RIGHT);
			r->setPrivateLevel(DELETE_RIGHT);
			SecurityModule::_rootProfile->cleanRights();
			SecurityModule::_rootProfile->addRight(r);
			ProfileTableSync::Save(SecurityModule::_rootProfile.get(), transaction);

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
			UserTableSync::Save(SecurityModule::_rootUser.get(), transaction);

			transaction.run();
		}

		template<> void ModuleClassTemplate<SecurityModule>::Start()
		{
		}

		template<> void ModuleClassTemplate<SecurityModule>::End()
		{
		}



		template<> void ModuleClassTemplate<SecurityModule>::InitThread(
		){
		}



		template<> void ModuleClassTemplate<SecurityModule>::CloseThread(
		){
		}
	}

	namespace security
	{
		const std::string SecurityModule::ROOT_PROFILE = "root";
		const std::string SecurityModule::ROOT_RIGHTS = "*,*,100,100";
		const std::string SecurityModule::ROOT_USER = "root";

		const std::string SecurityModule::MODULE_PARAMETER_SENDER_EMAIL("sender_email");

		boost::shared_ptr<User> SecurityModule::_rootUser;
		boost::shared_ptr<Profile> SecurityModule::_rootProfile;
		std::string SecurityModule::_senderEMail;

		boost::shared_ptr<Profile>	_rootProfile;



		SecurityModule::FactoryKeysLabels SecurityModule::getRightsTemplates()
		{
			FactoryKeysLabels m;
			vector<boost::shared_ptr<Right> > rights(Factory<Right>::GetNewCollection());
			BOOST_FOREACH(const boost::shared_ptr<Right> right, rights)
			{
				m.push_back(make_pair(right->getFactoryKey(), right->getFactoryKey()));
			}
			return m;
		}



		SecurityModule::Labels SecurityModule::getProfileLabels(bool withAll, int first/*=0*/, int last/*=-1*/ )
		{
			Labels m;
			if (withAll)
			{
				m.push_back(make_pair(optional<RegistryKeyType>(), "(tous)"));
			}

			Env env;
			ProfileTableSync::SearchResult profiles(
				ProfileTableSync::Search(env)
			);
			BOOST_FOREACH(const boost::shared_ptr<Profile>& profile, profiles)
			{
				m.push_back(make_pair(profile->getKey(), profile->getName()));
			}
			return m;
		}



		SecurityModule::Labels SecurityModule::getUserLabels(bool withAll, int first/*=0*/, int last/*=-1*/ )
		{
			Labels m;
			if (withAll)
			{
				m.push_back(make_pair(optional<RegistryKeyType>(), "(tous)"));
			}

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
			BOOST_FOREACH(const boost::shared_ptr<User>& user, users)
			{
				m.push_back(make_pair(user->getKey(), user->getSurname() + " " + user->getName()));
			}
			return m;
		}



		SecurityModule::FactoryKeysLabels SecurityModule::getRightLabels( bool withAll/*=false*/ )
		{
			FactoryKeysLabels m;
			if (withAll)
				m.push_back(make_pair(optional<string>(), "(toutes)"));
			vector<boost::shared_ptr<Right> > rights(Factory<Right>::GetNewCollection());
			BOOST_FOREACH(const boost::shared_ptr<Right> right, rights)
			{
				m.push_back(make_pair(right->getFactoryKey(), right->getName()));
			}
			return m;
		}

		std::vector<boost::shared_ptr<Profile> > SecurityModule::getSubProfiles(boost::shared_ptr<const Profile> profile )
		{
			vector<boost::shared_ptr<Profile> > v;
			Env env;
			ProfileTableSync::SearchResult profiles(
				ProfileTableSync::Search(
					env,
					profile.get() ? profile->getKey() : RegistryKeyType(0),
					0,
					optional<size_t>(),
					FIELDS_ONLY_LOAD_LEVEL
			)	);
			BOOST_FOREACH(const boost::shared_ptr<Profile>& cprofile, profiles)
			{
				 v.push_back(cprofile);
			}
			return v;
		}

		std::string SecurityModule::getSenderEMail()
		{
			return _senderEMail;
		}


		
		void SecurityModule::ParameterCallback( const std::string& name, const std::string& value )
		{
			if(name == MODULE_PARAMETER_SENDER_EMAIL)
			{
				try
				{
					std::string mail(lexical_cast<std::string>(value));

					if(!mail.empty())
					{
						_senderEMail = mail;
					}
				}
				catch(bad_lexical_cast)
				{
				}
			}
		}
	}
}
