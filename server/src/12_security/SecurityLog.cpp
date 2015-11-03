
/** SecurityLog class implementation.
	@file SecurityLog.cpp

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

#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

#include "SecurityLog.h"
#include "User.h"
#include "Profile.h"
#include "UserTableSync.h"
#include "ProfileTableSync.h"
#include "DBLogEntry.h"
#include "Request.h"
#include "SecurityRight.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace dblog;
	using namespace util;
	using namespace security;
	using namespace server;

	namespace util
	{
		template<> const std::string FactorableTemplate<DBLog, SecurityLog>::FACTORY_KEY = "security";
	}

	namespace security
	{
		DBLog::ColumnsVector SecurityLog::getColumnNames() const
		{
			ColumnsVector v;
			v.push_back("Type");
			v.push_back("Action");
			return v;
		}



		void SecurityLog::addUserLogin(const User* user )
		{
			DBLogEntry::Content c;
			c.push_back(lexical_cast<string>(LOGIN_ENTRY));
			c.push_back(string());
			c.push_back("Login succeeded");
			_addEntry(FACTORY_KEY, DB_LOG_INFO, c, user, user->getKey());
		}

		void SecurityLog::addUserAdmin(const User* user, const User* subject, const string& text)
		{
			DBLogEntry::Content c;
			c.push_back(lexical_cast<string>(USER_ADMIN_ENTRY));
			c.push_back(string());
			c.push_back(text);
			_addEntry(FACTORY_KEY, DB_LOG_INFO, c, user, subject->getKey());
		}

		void SecurityLog::addProfileAdmin(const User* user, const Profile* subject, const std::string& text )
		{
			DBLogEntry::Content c;
			c.push_back(lexical_cast<string>(PROFILE_ADMIN_ENTRY));
			c.push_back(string());
			c.push_back(text);
			_addEntry(FACTORY_KEY, DB_LOG_INFO, c, user, subject->getKey());
		}



		DBLog::ColumnsVector SecurityLog::parse(
			const DBLogEntry& entry,
			const server::Request& searchRequest
		) const	{
			DBLog::ColumnsVector v;
			const DBLogEntry::Content& content(entry.getContent());

			switch (static_cast<_EntryType>(lexical_cast<int>(content[0])))
			{
			case LOGIN_ENTRY:
				v.push_back("Login");
				v.push_back(content[2]);
				break;

			case USER_ADMIN_ENTRY:
				v.push_back("User Admin");
				v.push_back(content[2]);
				break;

			case PROFILE_ADMIN_ENTRY:
				v.push_back("Profile Admin");
				v.push_back(content[2]);
				break;

			default:
				v.push_back("unknown");
				v.push_back(content[2]);
			}

			return v;
		}

		std::string SecurityLog::getName() const
		{
			return "Journal général de sécurité";
		}



		std::string SecurityLog::getObjectName(
			util::RegistryKeyType id,
			const server::Request& request
		) const {
			Env env;
			try
			{
				if (decodeTableId(id) == UserTableSync::TABLE.ID)
				{
					boost::shared_ptr<const User> user(UserTableSync::Get(id, env, FIELDS_ONLY_LOAD_LEVEL));
					return user->getLogin();
				}
				else if (decodeTableId(id) == ProfileTableSync::TABLE.ID)
				{
					boost::shared_ptr<const Profile> profile(ProfileTableSync::Get(id, env, FIELDS_ONLY_LOAD_LEVEL));
					return profile->getName();
				}
			}
			catch(...)
			{
			}
			return DBLog::getObjectName(id, request);
		}



		void SecurityLog::AddQuitEntry( const User& user )
		{
			DBLogEntry::Content c;
			c.push_back(lexical_cast<string>(QUIT_ENTRY));
			c.push_back(string());
			c.push_back("SYNTHESE exited by user command");
			_addEntry(FACTORY_KEY, DB_LOG_WARNING, c, &user, user.getKey());
		}
	}
}
