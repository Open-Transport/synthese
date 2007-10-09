
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
#include "02_db/SQLite.h"
#include "02_db/SQLiteException.h"

#include "12_security/Profile.h"
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

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,UserTableSync>::FACTORY_KEY("12.02 User");
	}

	namespace db
	{
		template<> const string SQLiteTableSyncTemplate<UserTableSync,User>::TABLE_NAME = "t026_users";
		template<> const int SQLiteTableSyncTemplate<UserTableSync,User>::TABLE_ID = 26;
		template<> const bool SQLiteTableSyncTemplate<UserTableSync,User>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<UserTableSync,User>::load(User* user, const db::SQLiteResultSPtr& rows)
		{
				user->setKey(rows->getLongLong (TABLE_COL_ID));
				user->setPassword(rows->getText ( UserTableSync::TABLE_COL_PASSWORD));
				user->setName(rows->getText ( UserTableSync::TABLE_COL_NAME));
				user->setSurname(rows->getText ( UserTableSync::TABLE_COL_SURNAME));
				user->setLogin(rows->getText ( UserTableSync::TABLE_COL_LOGIN));
				user->setAddress(rows->getText ( UserTableSync::TABLE_COL_ADDRESS));
				user->setPostCode(rows->getText ( UserTableSync::TABLE_COL_POST_CODE));
				user->setCityText(rows->getText ( UserTableSync::TABLE_COL_CITY_TEXT));
				user->setCityId(rows->getLongLong ( UserTableSync::TABLE_COL_CITY_ID));
				user->setCountry(rows->getText ( UserTableSync::TABLE_COL_COUNTRY));
				user->setEMail(rows->getText ( UserTableSync::TABLE_COL_EMAIL));
				user->setPhone(rows->getText ( UserTableSync::TABLE_COL_PHONE));
				user->setConnectionAllowed(rows->getBool ( UserTableSync::COL_LOGIN_AUTHORIZED));
				user->setBirthDate(Date::FromSQLDate(rows->getText ( UserTableSync::COL_BIRTH_DATE)));
		}



		template<> void SQLiteTableSyncTemplate<UserTableSync,User>::_link(User* obj, const SQLiteResultSPtr& rows, GetSource temporary)
		{
			try
			{
				obj->setProfile(ProfileTableSync::Get(rows->getLongLong ( UserTableSync::TABLE_COL_PROFILE_ID), obj, true, GET_AUTO));
			}
			catch (Profile::RegistryKeyException e)
			{
				throw UserTableSyncException("Bad profile "+ rows->getText ( UserTableSync::TABLE_COL_PROFILE_ID));
			}
		}


		template<> void SQLiteTableSyncTemplate<UserTableSync,User>::_unlink(User* obj)
		{
			obj->setProfile(NULL);
		}



		template<> void SQLiteTableSyncTemplate<UserTableSync,User>::save(User* user )
		{
			try
			{
				SQLite* sqlite = DBModule::GetSQLite();
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
					<< "," << Conversion::ToSQLiteString(user->getAddress())
					<< "," << Conversion::ToSQLiteString(user->getPostCode())
					<< "," << Conversion::ToSQLiteString(user->getCityText())
					<< "," << Conversion::ToString(user->getCityId())
					<< "," << Conversion::ToSQLiteString(user->getCountry())
					<< "," << Conversion::ToSQLiteString(user->getEMail())
					<< "," << Conversion::ToSQLiteString(user->getPhone())
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
		const string UserTableSync::TABLE_COL_NAME = "name";
		const string UserTableSync::TABLE_COL_SURNAME = "surname";
		const string UserTableSync::TABLE_COL_LOGIN = "login";
		const string UserTableSync::TABLE_COL_PASSWORD = "password";
		const string UserTableSync::TABLE_COL_PROFILE_ID = "profile_id";
		const string UserTableSync::TABLE_COL_ADDRESS = "address";
		const string UserTableSync::TABLE_COL_POST_CODE = "post_code";
		const string UserTableSync::TABLE_COL_CITY_TEXT = "city_text";
		const string UserTableSync::TABLE_COL_CITY_ID = "city_id";
		const string UserTableSync::TABLE_COL_COUNTRY = "country";
		const string UserTableSync::TABLE_COL_EMAIL = "email";
		const string UserTableSync::TABLE_COL_PHONE = "phone";
		const string UserTableSync::COL_LOGIN_AUTHORIZED = "auth";
		const string UserTableSync::COL_BIRTH_DATE = "birth_date";

		UserTableSync::UserTableSync()
			: db::SQLiteNoSyncTableSyncTemplate<UserTableSync,User>()
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



		shared_ptr<User> UserTableSync::getUserFromLogin(const string& login )
		{
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
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
				link(user.get(), rows, GET_AUTO);
				return user;
			}
			catch (SQLiteException e)
			{
				throw UserTableSyncException(e.getMessage());
			}
		}



		vector<shared_ptr<User> > UserTableSync::search(
			const string& login
			, const string name
			, shared_ptr<const Profile> profile
			, tribool emptyLogin
			, int first /*= 0*/, int number /*= 0*/
			, bool orderByLogin
			, bool orderByName
			, bool orderByProfileName
			, bool raisingOrder
		){
			SQLite* sqlite = DBModule::GetSQLite();
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
					link(user.get(), rows, GET_AUTO);
					users.push_back(user);
				}
				return users;
			}
			catch(SQLiteException& e)
			{
				throw UserTableSyncException(e.getMessage());
			}
		}



		bool UserTableSync::loginExists( const string& login )
		{
			try
			{
				SQLite* sqlite = DBModule::GetSQLite();
				stringstream query;
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
