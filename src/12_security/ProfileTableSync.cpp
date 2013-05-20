/** ProfileTableSync class implementation.
	@file ProfileTableSync.cpp

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

#include "ProfileTableSync.h"

#include "Conversion.h"
#include "Right.h"
#include "Profile.h"
#include "DBModule.h"
#include "DBResult.hpp"
#include "DBException.hpp"
#include "ReplaceQuery.h"
#include "01_util/Log.h"
#include "Factory.h"
#include "UserTableSync.h"
#include "SecurityRight.h"
#include "SecurityLog.h"
#include "Session.h"

#include <sstream>

#include <boost/tokenizer.hpp>


using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace db;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,ProfileTableSync>::FACTORY_KEY("12.01 Profile");
	}

	namespace security
	{
		const string ProfileTableSync::RIGHT_SEPARATOR = "|";
		const string ProfileTableSync::RIGHT_VALUE_SEPARATOR = ",";

		const string ProfileTableSync::TABLE_COL_NAME = "name";
		const string ProfileTableSync::TABLE_COL_PARENT_ID = "parent";
		const string ProfileTableSync::TABLE_COL_RIGHTS_STRING = "rights";
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<ProfileTableSync>::TABLE(
				"t027_profiles"
				);

		template<> const Field DBTableSyncTemplate<ProfileTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(ProfileTableSync::TABLE_COL_NAME, SQL_TEXT),
			Field(ProfileTableSync::TABLE_COL_PARENT_ID, SQL_INTEGER),
			Field(ProfileTableSync::TABLE_COL_RIGHTS_STRING, SQL_TEXT),
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<ProfileTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		template<>
		void OldLoadSavePolicy<ProfileTableSync,Profile>::Load(
			Profile* profile,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			profile->setName(rows->getText ( ProfileTableSync::TABLE_COL_NAME));
			ProfileTableSync::setRightsFromString(profile, rows->getText ( ProfileTableSync::TABLE_COL_RIGHTS_STRING));

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				RegistryKeyType id(rows->getLongLong(ProfileTableSync::TABLE_COL_PARENT_ID));
				if(id > 0 && id != profile->getKey())
				{
					try
					{
						profile->setParent(ProfileTableSync::Get(id, env, linkLevel).get());
					}
					catch(ObjectNotFoundException<Profile>& e)
					{
						Log::GetInstance().warn("Data corrupted in " + ProfileTableSync::TABLE.NAME + "/" + ProfileTableSync::TABLE_COL_PARENT_ID, e);
					}
				}
			}
		}



		template<>
		void OldLoadSavePolicy<ProfileTableSync,Profile>::Save(
			Profile* profile,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<ProfileTableSync> query(*profile);
			query.addField(profile->getName());
			query.addField(profile->getParent() == NULL ? RegistryKeyType(0) : profile->getParent()->getKey());
			query.addField(ProfileTableSync::getRightsString(profile));
			query.execute(transaction);
		}



		template<>
		void OldLoadSavePolicy<ProfileTableSync,Profile>::Unlink(Profile* profile)
		{
			profile->setParent(NULL);
		}



		template<> bool DBTableSyncTemplate<ProfileTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			Env env;

			// Search of child profiles
			ProfileTableSync::SearchResult profiles(
				ProfileTableSync::Search(env, object_id, 0, 1, FIELDS_ONLY_LOAD_LEVEL)
			);
			if(!profiles.empty())
			{
				return false;
			}

			// Search of users
			UserTableSync::Search(
				env,
				optional<string>(),
				optional<string>(),
				optional<string>(),
				optional<string>(),
				object_id,
				boost::logic::indeterminate,
				boost::logic::indeterminate,
				optional<RegistryKeyType>(),
				0, 1,
				false, false, false, false,
				FIELDS_ONLY_LOAD_LEVEL
			);
			if (!env.getRegistry<User>().empty())
			{
				return false;
			}

			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<SecurityRight>(DELETE_RIGHT);
		}



		template<> void DBTableSyncTemplate<ProfileTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<ProfileTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<ProfileTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			Env env;
			boost::shared_ptr<const Profile> profile(ProfileTableSync::Get(id, env));
			SecurityLog::addProfileAdmin(session->getUser().get(), profile.get(), "Suppression de " + profile->getName());
		}

	}

	namespace security
	{
		ProfileTableSync::SearchResult ProfileTableSync::Search(
			Env& env,
			string name
			, string right
			, int first /*= 0*/
			, boost::optional<std::size_t> number  /*= -1*/
			, bool orderByName
			, bool raisingOrder,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE 1 ";
			if (!name.empty())
				query << " AND " << TABLE_COL_NAME << " LIKE " << Conversion::ToDBString(name);
			if (!right.empty())
				query << " AND " << TABLE_COL_RIGHTS_STRING << " LIKE " << Conversion::ToDBString(right);
			if (orderByName)
				query << " ORDER BY " << TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (number)
				query << " LIMIT " << (*number + 1);
			if (first > 0)
				query << " OFFSET " << first;

			return LoadFromQuery(query.str(), env, linkLevel);
		}



		ProfileTableSync::SearchResult ProfileTableSync::Search(
			Env& env,
			RegistryKeyType parentId,
			int first /*= 0*/,
			boost::optional<std::size_t> number , /*= -1*/
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE "
				<< TABLE_COL_PARENT_ID << "=" << parentId;
			if (number)
				query << " LIMIT " << (*number + 1);
			if (first > 0)
				query << " OFFSET " << first;

			return LoadFromQuery(query.str(), env, linkLevel);
		}



		string ProfileTableSync::getRightsString(const Profile* p)
		{
			stringstream s;

			for (RightsVector::const_iterator it = p->getRights().begin(); it != p->getRights().end(); ++it)
			{
				boost::shared_ptr<const Right> right = it->second;
				if (it != p->getRights().begin())
					s	<< RIGHT_SEPARATOR;
				s	<< right->getFactoryKey()
					<< RIGHT_VALUE_SEPARATOR << right->getParameter()
					<< RIGHT_VALUE_SEPARATOR << (static_cast<int>(right->getPrivateRightLevel()))
					<< RIGHT_VALUE_SEPARATOR << (static_cast<int>(right->getPublicRightLevel()))
					;
			}
			return s.str();
		}



		void ProfileTableSync::setRightsFromString(Profile* profile, const string& text )
		{
			typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
			boost::char_separator<char> sep(RIGHT_SEPARATOR.c_str ());

			// CLeaning the profile
			profile->cleanRights();

			// Parsing
			tokenizer parametersTokens (text, sep);
			for (tokenizer::iterator parameterToken = parametersTokens.begin();
				parameterToken != parametersTokens.end (); ++ parameterToken)
			{
				tokenizer valuesToken(*parameterToken, boost::char_separator<char>(RIGHT_VALUE_SEPARATOR.c_str()));
				tokenizer::iterator it = valuesToken.begin();

				try
				{
					boost::shared_ptr<Right> right(Factory<Right>::create(*it));

					++it;
					right->setParameter(*it);

					++it;
					right->setPrivateLevel(static_cast<RightLevel>(lexical_cast<int>(*it)));

					++it;
					right->setPublicLevel(static_cast<RightLevel>(lexical_cast<int>(*it)));

					profile->addRight(right);
				}
				catch (FactoryException<Right> e)
				{
					continue;
				}
			}
		}
	}
}
