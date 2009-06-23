/** ProfileTableSync class implementation.
	@file ProfileTableSync.cpp

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

#include "ProfileTableSync.h"
#include "UserTableSyncException.h"
#include "Right.h"
#include "Profile.h"

#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"

#include "Conversion.h"
#include "01_util/Log.h"
#include "Factory.h"

#include <sstream>

#include <boost/tokenizer.hpp>


using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace util;
	using namespace db;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,ProfileTableSync>::FACTORY_KEY("12.01 Profile");
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
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<ProfileTableSync>::TABLE(
				"t027_profiles"
				);

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<ProfileTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(ProfileTableSync::TABLE_COL_NAME, SQL_TEXT),
			SQLiteTableSync::Field(ProfileTableSync::TABLE_COL_PARENT_ID, SQL_INTEGER),
			SQLiteTableSync::Field(ProfileTableSync::TABLE_COL_RIGHTS_STRING, SQL_TEXT),
			SQLiteTableSync::Field()
		};

		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<ProfileTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index()
		};

		template<> void SQLiteDirectTableSyncTemplate<ProfileTableSync,Profile>::Load(
			Profile* profile,
			const db::SQLiteResultSPtr& rows,
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
						Log::GetInstance().warn("Data corrupted in " + TABLE.NAME + "/" + ProfileTableSync::TABLE_COL_PARENT_ID, e);
					}
				}
			}
		}



		template<> void SQLiteDirectTableSyncTemplate<ProfileTableSync,Profile>::Save(Profile* profile )
		{
			try
			{
				SQLite* sqlite = DBModule::GetSQLite();
				if (profile->getKey() <= 0)
					profile->setKey(getId());
				
				stringstream query;
				query
					<< "REPLACE INTO " << TABLE.NAME
					<< " VALUES(" 
					<< Conversion::ToString(profile->getKey())
					<< "," << Conversion::ToSQLiteString(profile->getName())
					<< "," << ((profile == NULL) ? "0" : Conversion::ToString(profile->getKey()))
					<< "," << Conversion::ToSQLiteString(ProfileTableSync::getRightsString(profile))
					<< ")";
				sqlite->execUpdate(query.str());
			}
			catch (SQLiteException& e)
			{
				throw UserTableSyncException("Insert/Update error " + e.getMessage());
			}
			catch (...)
			{
				throw UserTableSyncException("Unknown Insert/Update error");
			}
		}


		template<> void SQLiteDirectTableSyncTemplate<ProfileTableSync,Profile>::Unlink(Profile* profile)
		{
			profile->setParent(NULL);
		}
	}

	namespace security
	{
		ProfileTableSync::ProfileTableSync()
			: db::SQLiteRegistryTableSyncTemplate<ProfileTableSync,Profile>()
		{
		}


		void ProfileTableSync::Search(
			Env& env,
			string name
			, string right
			, int first /*= 0*/
			, int number /*= -1*/ 
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
				query << " AND " << TABLE_COL_NAME << " LIKE " << Conversion::ToSQLiteString(name);
			if (!right.empty())
				query << " AND " << TABLE_COL_RIGHTS_STRING << " LIKE " << Conversion::ToSQLiteString(right);
			if (orderByName)
				query << " ORDER BY " << TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			LoadFromQuery(query.str(), env, linkLevel);
		}



		void ProfileTableSync::Search(
			Env& env,
			RegistryKeyType parentId,
			int first /*= 0*/,
			int number, /*= -1*/
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME					
				<< " WHERE " 
				<< TABLE_COL_PARENT_ID << "=" << parentId;
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			LoadFromQuery(query.str(), env, linkLevel);
		}



		string ProfileTableSync::getRightsString(const Profile* p)
		{
			stringstream s;
			
			for (RightsVector::const_iterator it = p->getRights().begin(); it != p->getRights().end(); ++it)
			{
				shared_ptr<const Right> right = it->second;
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
					shared_ptr<Right> right(Factory<Right>::create(*it));

					++it;
					right->setParameter(*it);

					++it;
					right->setPrivateLevel(static_cast<RightLevel>(Conversion::ToInt(*it)));

					++it;
					right->setPublicLevel(static_cast<RightLevel>(Conversion::ToInt(*it)));

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
