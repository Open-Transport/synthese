//////////////////////////////////////////////////////////////////////////
/// UserTableSync class implementation.
///	@file UserTableSync.cpp
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include <sstream>

#include "ReplaceQuery.h"
#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"

#include "Profile.h"
#include "UserTableSync.h"
#include "ProfileTableSync.h"
#include "User.h"

using namespace std;
using namespace boost;
using namespace boost::logic;
using namespace boost::gregorian;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,UserTableSync>::FACTORY_KEY("12.02 User");
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
		const string UserTableSync::COL_LANGUAGE = "language";
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<UserTableSync>::TABLE(
			"t026_users", true
		);

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<UserTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(UserTableSync::TABLE_COL_NAME, SQL_TEXT),
			SQLiteTableSync::Field(UserTableSync::TABLE_COL_SURNAME, SQL_TEXT),
			SQLiteTableSync::Field(UserTableSync::TABLE_COL_LOGIN, SQL_TEXT),
			SQLiteTableSync::Field(UserTableSync::TABLE_COL_PASSWORD, SQL_TEXT),
			SQLiteTableSync::Field(UserTableSync::TABLE_COL_PROFILE_ID, SQL_INTEGER),
			SQLiteTableSync::Field(UserTableSync::TABLE_COL_ADDRESS, SQL_TEXT),
			SQLiteTableSync::Field(UserTableSync::TABLE_COL_POST_CODE, SQL_TEXT),
			SQLiteTableSync::Field(UserTableSync::TABLE_COL_CITY_TEXT, SQL_TEXT),
			SQLiteTableSync::Field(UserTableSync::TABLE_COL_CITY_ID, SQL_INTEGER),
			SQLiteTableSync::Field(UserTableSync::TABLE_COL_COUNTRY, SQL_TEXT),
			SQLiteTableSync::Field(UserTableSync::TABLE_COL_EMAIL, SQL_TEXT),
			SQLiteTableSync::Field(UserTableSync::TABLE_COL_PHONE, SQL_TEXT),
			SQLiteTableSync::Field(UserTableSync::COL_LOGIN_AUTHORIZED, SQL_INTEGER),
			SQLiteTableSync::Field(UserTableSync::COL_BIRTH_DATE, SQL_TIMESTAMP),
			SQLiteTableSync::Field(UserTableSync::COL_LANGUAGE, SQL_TEXT),
			SQLiteTableSync::Field()
		};

		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<UserTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index(UserTableSync::TABLE_COL_NAME.c_str(), ""),
			SQLiteTableSync::Index(UserTableSync::TABLE_COL_LOGIN.c_str(), ""),
			SQLiteTableSync::Index(UserTableSync::TABLE_COL_PROFILE_ID.c_str(), ""),
			SQLiteTableSync::Index()
		};

		template<> void SQLiteDirectTableSyncTemplate<UserTableSync,User>::Load(
			User* user,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
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
			user->setBirthDate(rows->getDate(UserTableSync::COL_BIRTH_DATE));
			user->setLanguage(rows->getText(UserTableSync::COL_LANGUAGE));
	
			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				user->setProfile(ProfileTableSync::Get(
					rows->getLongLong(UserTableSync::TABLE_COL_PROFILE_ID),
					env,
					linkLevel
				).get());
			}
		}


		template<> void SQLiteDirectTableSyncTemplate<UserTableSync,User>::Unlink(
			User* obj
		){
			obj->setProfile(NULL);
		}



		template<> void SQLiteDirectTableSyncTemplate<UserTableSync,User>::Save(
			User* user,
			optional<SQLiteTransaction&> transaction
		){
			ReplaceQuery<UserTableSync> query(*user);
			query.addField(user->getName());
			query.addField(user->getSurname());
			query.addField(user->getLogin());
			query.addField(user->getPassword());
			query.addField(user->getProfile()->getKey());
			query.addField(user->getAddress());
			query.addField(user->getPostCode());
			query.addField(user->getCityText());
			query.addField(user->getCityId());
			query.addField(user->getCountry());
			query.addField(user->getEMail());
			query.addField(user->getPhone());
			query.addField(user->getConnectionAllowed());
			query.addField(user->getBirthDate());
			query.addField(user->getLanguage());
			query.execute(transaction);
		}
	}
	namespace security
	{
		shared_ptr<User> UserTableSync::getUserFromLogin(const string& login )
		{
			Env& env(Env::GetOfficialEnv());
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< "SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE " << TABLE_COL_LOGIN << "=" << Conversion::ToSQLiteString(login);
			try
			{
				db::SQLiteResultSPtr rows = sqlite->execQuery(query.str());
				if (rows->next () == false)
					throw Exception("User "+ login + " not found in database.");

				shared_ptr<User> user (new User(rows->getKey()));
				Load(user.get(), rows, env, UP_LINKS_LOAD_LEVEL);
				return user;
			}
			catch (SQLiteException e)
			{
				throw Exception(e.getMessage());
			}
		}



		UserTableSync::SearchResult UserTableSync::Search(
			Env& env,
			optional<string> login,
			optional<string> name,
			optional<string> surname,
			optional<string> phone,
			optional<RegistryKeyType> profileId,
			tribool emptyLogin,
			tribool emptyPhone,
			boost::optional<util::RegistryKeyType> differentUserId,
			int first, /*= 0*/
			boost::optional<std::size_t> number, /*= 0*/
			bool orderByLogin,
			bool orderByName,
			bool orderByProfileName,
			bool raisingOrder,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT "
				<< "t.*"
				<< " FROM " << TABLE.NAME << " AS t";
			if (orderByProfileName)
				query << " INNER JOIN " << ProfileTableSync::TABLE.NAME << " AS p ON p." << TABLE_COL_ID << "=t." << TABLE_COL_PROFILE_ID;
			query << " WHERE 1 "; 
			if(login)
				query << " AND t." << TABLE_COL_LOGIN << " LIKE " << Conversion::ToSQLiteString(*login);
			if(name)
				query << " AND t." << TABLE_COL_NAME << " LIKE " << Conversion::ToSQLiteString(*name);
			if(surname)
				query << " AND t." << TABLE_COL_SURNAME << " LIKE " << Conversion::ToSQLiteString(*surname);
			if(phone)
				query << " AND t." << TABLE_COL_PHONE << " LIKE " << Conversion::ToSQLiteString(*phone);
			if(profileId)
				query << " AND " << TABLE_COL_PROFILE_ID << "=" << *profileId;
			if (!indeterminate(emptyLogin))
				query << " AND " << TABLE_COL_LOGIN << (emptyLogin ? "=''" : "!=''");
			if (!indeterminate(emptyPhone))
				query << " AND " << TABLE_COL_PHONE << (emptyPhone ? "=''" : "!=''");
			if(differentUserId)
				query << " AND " << TABLE_COL_ID << "!=" << *differentUserId;

			if (orderByProfileName)
				query << " ORDER BY p." << ProfileTableSync::TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC");
			else if (orderByLogin)
				query <<
					" ORDER BY t." << TABLE_COL_LOGIN << (raisingOrder ? " ASC" : " DESC") << "," <<
					TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC") << "," <<
					TABLE_COL_SURNAME << (raisingOrder ? " ASC" : " DESC")
				;
			else if (orderByName)
				query << " ORDER BY t." << TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC") << ",t." << TABLE_COL_SURNAME << (raisingOrder ? " ASC" : " DESC");
			if (number)
			{
				query << " LIMIT " << (*number + 1);
				if (first > 0)
					query << " OFFSET " << first;
			}

			return LoadFromQuery(query.str(), env, linkLevel);
		}



		bool UserTableSync::loginExists( const string& login )
		{
			try
			{
				SQLite* sqlite = DBModule::GetSQLite();
				stringstream query;
				query
					<< "SELECT " << TABLE_COL_ID
					<< " FROM " << TABLE.NAME
					<< " WHERE " << TABLE_COL_LOGIN << "=" << Conversion::ToSQLiteString(login)
					<< " LIMIT 1 ";
				
				db::SQLiteResultSPtr rows = sqlite->execQuery(query.str());
				return (rows->next () != false);
			}
			catch (SQLiteException e)
			{
				throw Exception(e.getMessage());
			}
		}
	}
}
