
/** UserTableSync class implementation.
	@file UserTableSync.cpp

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

#include "01_util/Conversion.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"
#include "02_db/SQLiteException.h"

#include "12_security/SecurityModule.h"
#include "12_security/UserTableSync.h"
#include "12_security/ProfileTableSync.h"
#include "12_security/User.h"
#include "12_security/UserTableSyncException.h"

using namespace std;
using namespace boost::logic;
using boost::shared_ptr;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace security;
	using namespace time;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<User>::TABLE_NAME = "t026_users";
		template<> const int SQLiteTableSyncTemplate<User>::TABLE_ID = 26;
		template<> const bool SQLiteTableSyncTemplate<User>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<User>::load(User* user, const db::SQLiteResultSPtr& rows)
		{
			try
			{
				user->setKey(rows->getLongLong (TABLE_COL_ID));
				user->setPassword(rows->getText ( UserTableSync::TABLE_COL_PASSWORD));
				user->setName(rows->getText ( UserTableSync::TABLE_COL_NAME));
				user->setSurname(rows->getText ( UserTableSync::TABLE_COL_SURNAME));
				user->setLogin(rows->getText ( UserTableSync::TABLE_COL_LOGIN));
				user->_address = rows->getText ( UserTableSync::TABLE_COL_ADDRESS);
				user->_postCode = rows->getText ( UserTableSync::TABLE_COL_POST_CODE);
				user->_cityText = rows->getText ( UserTableSync::TABLE_COL_CITY_TEXT);
				user->_cityId = rows->getLongLong ( UserTableSync::TABLE_COL_CITY_ID);
				user->_country = rows->getText ( UserTableSync::TABLE_COL_COUNTRY);
				user->_email = rows->getText ( UserTableSync::TABLE_COL_EMAIL);
				user->_phone = rows->getText ( UserTableSync::TABLE_COL_PHONE);
				user->setConnectionAllowed(rows->getBool ( UserTableSync::COL_LOGIN_AUTHORIZED));
				user->setProfile(SecurityModule::getProfiles().get(rows->getLongLong ( UserTableSync::TABLE_COL_PROFILE_ID)));
				user->setBirthDate(Date::FromSQLDate(rows->getText ( UserTableSync::COL_BIRTH_DATE)));
			}
			catch (Profile::RegistryKeyException e)
			{
				throw UserTableSyncException("Bad profile "+ rows->getText ( UserTableSync::TABLE_COL_PROFILE_ID));
			}
		}





		template<> void SQLiteTableSyncTemplate<User>::save(User* user )
		{
			try
			{
				SQLiteHandle* sqlite = DBModule::GetSQLite();
				stringstream query;
				if (user->getKey() <= 0)
					user->setKey(getId());
				query
					<< "REPLACE INTO " << TABLE_NAME
					<< " VALUES(" 
					<< Conversion::ToString(user->getKey())
					<< "," << Conversion::ToSQLiteString(user->getName())
					<< "," << Conversion::ToSQLiteString(user->getSurname())
					<< "," << Conversion::ToSQLiteString(user->getLogin())
					<< "," << Conversion::ToSQLiteString(user->getPassword())
					<< "," << Conversion::ToString(user->getProfile()->getKey())
					<< "," << Conversion::ToSQLiteString(user->_address)
					<< "," << Conversion::ToSQLiteString(user->_postCode)
					<< "," << Conversion::ToSQLiteString(user->_cityText)
					<< "," << Conversion::ToString(user->_cityId)
					<< "," << Conversion::ToSQLiteString(user->_country)
					<< "," << Conversion::ToSQLiteString(user->_email)
					<< "," << Conversion::ToSQLiteString(user->_phone)
					<< "," << Conversion::ToString(user->getConnectionAllowed())
					<< "," << user->getBirthDate().toSQLString()
					<< ")";
				sqlite->execUpdate(query.str());
			}
			catch (SQLiteException e)
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
		const std::string UserTableSync::TABLE_COL_NAME = "name";
		const std::string UserTableSync::TABLE_COL_SURNAME = "surname";
		const std::string UserTableSync::TABLE_COL_LOGIN = "login";
		const std::string UserTableSync::TABLE_COL_PASSWORD = "password";
		const std::string UserTableSync::TABLE_COL_PROFILE_ID = "profile_id";
		const std::string UserTableSync::TABLE_COL_ADDRESS = "address";
		const std::string UserTableSync::TABLE_COL_POST_CODE = "post_code";
		const std::string UserTableSync::TABLE_COL_CITY_TEXT = "city_text";
		const std::string UserTableSync::TABLE_COL_CITY_ID = "city_id";
		const std::string UserTableSync::TABLE_COL_COUNTRY = "country";
		const std::string UserTableSync::TABLE_COL_EMAIL = "email";
		const std::string UserTableSync::TABLE_COL_PHONE = "phone";
		const std::string UserTableSync::COL_LOGIN_AUTHORIZED = "auth";
		const std::string UserTableSync::COL_BIRTH_DATE = "birth_date";

		UserTableSync::UserTableSync()
			: db::SQLiteTableSyncTemplate<User> (true, true, TRIGGERS_ENABLED_CLAUSE, true)
		{
			// Columns
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(TABLE_COL_NAME, "TEXT", true);
			addTableColumn(TABLE_COL_SURNAME, "TEXT", true);
			addTableColumn(TABLE_COL_LOGIN, "TEXT", true);
			addTableColumn(TABLE_COL_PASSWORD, "TEXT", true);
			addTableColumn(TABLE_COL_PROFILE_ID, "INTEGER", true);
			addTableColumn(TABLE_COL_ADDRESS, "TEXT", true);
			addTableColumn(TABLE_COL_POST_CODE, "TEXT", true);
			addTableColumn(TABLE_COL_CITY_TEXT, "TEXT", true);
			addTableColumn(TABLE_COL_CITY_ID, "INTEGER", true);
			addTableColumn(TABLE_COL_COUNTRY, "TEXT", true);
			addTableColumn(TABLE_COL_EMAIL, "TEXT", true);
			addTableColumn(TABLE_COL_PHONE, "TEXT", true);
			addTableColumn(COL_LOGIN_AUTHORIZED, "INTEGER");
			addTableColumn(COL_BIRTH_DATE, "TIMESTAMP");

			// Indexes
			addTableIndex(TABLE_COL_NAME);
			addTableIndex(TABLE_COL_LOGIN);
			addTableIndex(TABLE_COL_PROFILE_ID);
		}


		void UserTableSync::rowsUpdated( SQLiteQueueThreadExec* sqlite,  SQLiteSync* sync, const SQLiteResultSPtr& rows )
		{
		}


		void UserTableSync::rowsAdded( SQLiteQueueThreadExec* sqlite,  SQLiteSync* sync, const SQLiteResultSPtr& rows, bool isFirstSync)
		{
		}


		void UserTableSync::rowsRemoved( SQLiteQueueThreadExec* sqlite,  SQLiteSync* sync, const SQLiteResultSPtr& rows )
		{
		}

		shared_ptr<User> UserTableSync::getUserFromLogin(const std::string& login )
		{
			SQLiteHandle* sqlite = DBModule::GetSQLite();
			std::stringstream query;
			query
				<< "SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE " << TABLE_COL_LOGIN << "=" << Conversion::ToSQLiteString(login);
			try
			{
				db::SQLiteResultSPtr rows = sqlite->execQuery(query.str());
				if (rows->next () == false)
					throw UserTableSyncException("User "+ login + " not found in database.");

				shared_ptr<User> user (new User);
				load(user.get(), rows);
				return user;
			}
			catch (SQLiteException e)
			{
				throw UserTableSyncException(e.getMessage());
			}
		}



		std::vector<shared_ptr<User> > UserTableSync::search(
			const std::string& login
			, const std::string name
			, shared_ptr<const Profile> profile
			, tribool emptyLogin
			, int first /*= 0*/, int number /*= 0*/
			, bool orderByLogin
			, bool orderByName
			, bool orderByProfileName
			, bool raisingOrder
		){
			SQLiteHandle* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< " SELECT "
				<< "t.*"
				<< " FROM " << TABLE_NAME << " AS t";
			if (orderByProfileName)
				query << " INNER JOIN " << ProfileTableSync::TABLE_NAME << " AS p ON p." << TABLE_COL_ID << "=t." << TABLE_COL_PROFILE_ID;
			query
				<< " WHERE " 
				<< " t." << TABLE_COL_LOGIN << " LIKE '%" << Conversion::ToSQLiteString(login, false) << "%'"
				<< " AND t." << TABLE_COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(name, false) << "%'";
			if (profile.get())
				query << " AND " << TABLE_COL_PROFILE_ID << "=" << Conversion::ToString(profile->getKey());
			if (emptyLogin != tribool::indeterminate_value)
				query << " AND " << TABLE_COL_LOGIN << (emptyLogin ? "=''" : "!=''");
			if (orderByProfileName)
				query << " ORDER BY p." << ProfileTableSync::TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (orderByLogin)
				query << " ORDER BY t." << TABLE_COL_LOGIN << (raisingOrder ? " ASC" : " DESC");
			if (orderByName)
				query << " ORDER BY t." << TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC") << ",t." << TABLE_COL_SURNAME << (raisingOrder ? " ASC" : " DESC");
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);
			
			try
			{
				SQLiteResultSPtr rows = sqlite->execQuery(query.str());
				vector<shared_ptr<User> > users;
				while (rows->next ())
				{
					shared_ptr<User> user(new User);
					load(user.get(), rows);
					users.push_back(user);
				}
				return users;
			}
			catch(SQLiteException& e)
			{
				throw UserTableSyncException(e.getMessage());
			}
		}



		bool UserTableSync::loginExists( const std::string& login )
		{
			try
			{
				SQLiteHandle* sqlite = DBModule::GetSQLite();
				std::stringstream query;
				query
					<< "SELECT " << TABLE_COL_ID
					<< " FROM " << TABLE_NAME
					<< " WHERE " << TABLE_COL_LOGIN << "=" << Conversion::ToSQLiteString(login)
					<< " LIMIT 1 ";
				
				db::SQLiteResultSPtr rows = sqlite->execQuery(query.str());
				return (rows->next () != false);
			}
			catch (SQLiteException e)
			{
				throw UserTableSyncException(e.getMessage());
			}
		}
	}
}
