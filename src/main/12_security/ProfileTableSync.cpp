
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

#include <sstream>

#include <boost/tokenizer.hpp>

#include "01_util/Conversion.h"
#include "01_util/Log.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"
#include "02_db/SQLiteException.h"

#include "12_security/SecurityModule.h"
#include "12_security/ProfileTableSync.h"
#include "12_security/UserTableSyncException.h"
#include "12_security/Right.h"
#include "12_security/Profile.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace util;
	using namespace db;
	using namespace security;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<Profile>::TABLE_NAME = "t027_profiles";
		template<> const int SQLiteTableSyncTemplate<Profile>::TABLE_ID = 27;
		template<> const bool SQLiteTableSyncTemplate<Profile>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<Profile>::load(Profile* profile, const db::SQLiteResult& rows, int rowId/*=0*/ )
		{
			profile->setKey(Conversion::ToLongLong(rows.getColumn(rowId, TABLE_COL_ID)));
			profile->setName(rows.getColumn(rowId, ProfileTableSync::TABLE_COL_NAME));
			profile->setParent(Conversion::ToLongLong(rows.getColumn(rowId, ProfileTableSync::TABLE_COL_PARENT_ID)));
			ProfileTableSync::setRightsFromString(profile, rows.getColumn(rowId, ProfileTableSync::TABLE_COL_RIGHTS_STRING));
		}

		template<> void SQLiteTableSyncTemplate<Profile>::save(Profile* profile )
		{
			try
			{
				const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
				if (profile->getKey() == 0)
					profile->setKey(getId(1,1));	/// @todo handle grid id
				
				stringstream query;
				query
					<< "REPLACE INTO " << TABLE_NAME
					<< " VALUES(" 
					<< Conversion::ToString(profile->getKey())
					<< "," << Conversion::ToSQLiteString(profile->getName())
					<< "," << Conversion::ToString(profile->getParentId())
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



	}

	namespace security
	{
		const std::string ProfileTableSync::RIGHT_SEPARATOR = "|";
		const std::string ProfileTableSync::RIGHT_VALUE_SEPARATOR = ",";

		const std::string ProfileTableSync::TABLE_COL_NAME = "name";
		const std::string ProfileTableSync::TABLE_COL_PARENT_ID = "parent";
		const std::string ProfileTableSync::TABLE_COL_RIGHTS_STRING = "rights";

		ProfileTableSync::ProfileTableSync()
			: db::SQLiteTableSyncTemplate<Profile>(true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(TABLE_COL_NAME, "TEXT", true);
			addTableColumn(TABLE_COL_PARENT_ID, "INTEGER", true);
			addTableColumn(TABLE_COL_RIGHTS_STRING, "TEXT", true);
		}

		void ProfileTableSync::rowsAdded( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows, bool isFirstSync)
		{
			for (int i = 0; i < rows.getNbRows(); ++i)
			{
				if (SecurityModule::getProfiles().contains(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID))))
				{
					load(SecurityModule::getProfiles().getUpdateable(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID))).get(), rows, i);
				}
				else
				{
					shared_ptr<Profile> profile(new Profile(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID))));
					load(profile.get(), rows, i);
					SecurityModule::getProfiles().add(profile);
				}
			}
		}

		void ProfileTableSync::rowsUpdated( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{
			for (int i = 0; i < rows.getNbRows(); ++i)
			{
				if (SecurityModule::getProfiles().contains(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID))))
				{
					load(SecurityModule::getProfiles().getUpdateable(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID))).get(), rows, i);
				}
			}
		}

		void ProfileTableSync::rowsRemoved( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{
			for (int i = 0; i < rows.getNbRows(); ++i)
			{
				if (SecurityModule::getProfiles().contains(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID))))
				{
					SecurityModule::getProfiles().remove(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID)));
				}
			}
		}

		std::vector<shared_ptr<Profile> > ProfileTableSync::search(std::string name, string right, int first /*= 0*/, int number /*= 0*/ 
			, bool orderByName
			, bool raisingOrder	
		){
			/** @todo Handle right filter */
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME					
				<< " WHERE " << TABLE_COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(name, false) << "%'";
			if (orderByName)
				query << " ORDER BY " << TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteResult result = sqlite->execQuery(query.str());
				vector<shared_ptr<Profile> > profiles;
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					shared_ptr<Profile> profile(new Profile);
					load(profile.get(), result, i);
					profiles.push_back(profile);
				}
				return profiles;
			}
			catch(SQLiteException& e)
			{
				throw UserTableSyncException(e.getMessage());
			}
		}

		std::vector<shared_ptr<Profile> > ProfileTableSync::search(
			shared_ptr<const Profile> parent
			, int first /*= 0*/, int number /*= -1*/ )
		{
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME					
				<< " WHERE " 
				<< TABLE_COL_PARENT_ID << "=" << (parent.get() ? Conversion::ToString(parent->getKey()) : "0");
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteResult result = sqlite->execQuery(query.str());
				vector<shared_ptr<Profile> > profiles;
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					shared_ptr<Profile> profile(new Profile);
					load(profile.get(), result, i);
					profiles.push_back(profile);
				}
				return profiles;
			}
			catch(SQLiteException& e)
			{
				throw UserTableSyncException(e.getMessage());
			}
		}
		std::string ProfileTableSync::getRightsString(const Profile* p)
		{
			stringstream s;
			
			for (Profile::RightsVector::const_iterator it = p->getRights().begin(); it != p->getRights().end(); ++it)
			{
				shared_ptr<const Right> right = it->second;
				if (it != p->getRights().begin())
					s	<< RIGHT_SEPARATOR;
				s	<< right->getFactoryKey() 
					<< RIGHT_VALUE_SEPARATOR << right->getParameter()
					<< RIGHT_VALUE_SEPARATOR << ((int) right->getPrivateRightLevel())
					<< RIGHT_VALUE_SEPARATOR << ((int) right->getPublicRightLevel())
					;
			}
			return s.str();
		}

		void ProfileTableSync::setRightsFromString(Profile* profile, const std::string& text )
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
					shared_ptr<Right> right = Factory<Right>::create(*it);

					++it;
					right->setParameter(*it);

					++it;
					right->setPrivateLevel((Right::Level) Conversion::ToInt(*it));

					++it;
					right->setPublicLevel((Right::Level) Conversion::ToInt(*it));

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
