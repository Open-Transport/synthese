//////////////////////////////////////////////////////////////////////////
/// UserTableSync class implementation.
///	@file UserTableSync.cpp
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "UserTableSync.h"

#include "Conversion.h"
#include "DataSourceLinksField.hpp"
#include "DateField.hpp"
#include "ImportableTableSync.hpp"
#include "ReplaceQuery.h"
#include "DBModule.h"
#include "DBResult.hpp"
#include "DBException.hpp"
#include "Language.hpp"
#include "Profile.h"
#include "ProfileTableSync.h"
#include "User.h"
#include "SecurityRight.h"
#include "SecurityLog.h"
#include "Session.h"

#include <sstream>

using namespace std;
using namespace boost;
using namespace boost::logic;
using namespace boost::gregorian;

namespace synthese
{
	using namespace db;
	using namespace impex;
	using namespace util;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,UserTableSync>::FACTORY_KEY("12.02 User");
	}

	namespace security
	{
		const string UserTableSync::TABLE_COL_NAME = "name";
		const string UserTableSync::TABLE_COL_SURNAME = "surname";
		const string UserTableSync::TABLE_COL_LOGIN = "login";
		// TODO: rename column to password_hash, once schema migration is possible.
		const string UserTableSync::TABLE_COL_PASSWORD_HASH = "password";
		const string UserTableSync::TABLE_COL_PROFILE_ID = "profile_id";
		const string UserTableSync::TABLE_COL_ADDRESS = "address";
		const string UserTableSync::TABLE_COL_POST_CODE = "post_code";
		const string UserTableSync::TABLE_COL_CITY_TEXT = "city_text";
		const string UserTableSync::TABLE_COL_CITY_ID = "city_id";
		const string UserTableSync::TABLE_COL_COUNTRY = "country";
		const string UserTableSync::TABLE_COL_EMAIL = "email";
		const string UserTableSync::TABLE_COL_PHONE = "phone";
		const string UserTableSync::TABLE_COL_CREATION_DATE = "creation_date";
		const string UserTableSync::TABLE_COL_CREATOR_ID = "creator_id";
		const string UserTableSync::COL_LOGIN_AUTHORIZED = "auth";
		const string UserTableSync::COL_BIRTH_DATE = "birth_date";
		const string UserTableSync::COL_LANGUAGE = "language";
		const string UserTableSync::COL_DATA_SOURCE_LINKS = "data_source_links";
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<UserTableSync>::TABLE(
			"t026_users", true
		);

		template<> const Field DBTableSyncTemplate<UserTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(UserTableSync::TABLE_COL_NAME, SQL_TEXT),
			Field(UserTableSync::TABLE_COL_SURNAME, SQL_TEXT),
			Field(UserTableSync::TABLE_COL_LOGIN, SQL_TEXT),
			Field(UserTableSync::TABLE_COL_PASSWORD_HASH, SQL_TEXT),
			Field(UserTableSync::TABLE_COL_PROFILE_ID, SQL_INTEGER),
			Field(UserTableSync::TABLE_COL_ADDRESS, SQL_TEXT),
			Field(UserTableSync::TABLE_COL_POST_CODE, SQL_TEXT),
			Field(UserTableSync::TABLE_COL_CITY_TEXT, SQL_TEXT),
			Field(UserTableSync::TABLE_COL_CITY_ID, SQL_INTEGER),
			Field(UserTableSync::TABLE_COL_COUNTRY, SQL_TEXT),
			Field(UserTableSync::TABLE_COL_EMAIL, SQL_TEXT),
			Field(UserTableSync::TABLE_COL_PHONE, SQL_TEXT),
			Field(UserTableSync::TABLE_COL_CREATION_DATE, SQL_DATETIME),
			Field(UserTableSync::TABLE_COL_CREATOR_ID, SQL_INTEGER),
			Field(UserTableSync::COL_LOGIN_AUTHORIZED, SQL_BOOLEAN),
			Field(UserTableSync::COL_BIRTH_DATE, SQL_DATETIME),
			Field(UserTableSync::COL_LANGUAGE, SQL_TEXT),
			Field(UserTableSync::COL_DATA_SOURCE_LINKS, SQL_TEXT),
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<UserTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(DBTableSync::Index(UserTableSync::TABLE_COL_NAME.c_str(), ""));
			r.push_back(DBTableSync::Index(UserTableSync::TABLE_COL_LOGIN.c_str(), ""));
			r.push_back(DBTableSync::Index(UserTableSync::TABLE_COL_PROFILE_ID.c_str(), ""));
			return r;
		}

		template<>
		void OldLoadSavePolicy<UserTableSync,User>::Load(
			User* user,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			user->setPasswordHash(rows->getText ( UserTableSync::TABLE_COL_PASSWORD_HASH));
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
			user->setCreationDate(rows->getDate ( UserTableSync::TABLE_COL_CREATION_DATE));
			user->setCreatorId(rows->getLongLong ( UserTableSync::TABLE_COL_CREATOR_ID));
			user->setConnectionAllowed(rows->getBool ( UserTableSync::COL_LOGIN_AUTHORIZED));
			user->setBirthDate(rows->getDate(UserTableSync::COL_BIRTH_DATE));

			// Language
			user->setLanguage(NULL);
			string langStr(rows->getText(UserTableSync::COL_LANGUAGE));
			if(!langStr.empty()) try
			{
				user->setLanguage(&Language::GetLanguageFromIso639_2Code(langStr));
			}
			catch(Language::LanguageNotFoundException& e)
			{
				Log::GetInstance().warn("Language error in user "+ lexical_cast<string>(user->getKey()), e);
			}

			// Profile
			user->setProfile(NULL);
			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				RegistryKeyType profileId(
					rows->getDefault<RegistryKeyType>(UserTableSync::TABLE_COL_PROFILE_ID, 0)
				);
				if(profileId)
				{
					user->setProfile(
						ProfileTableSync::Get(
							profileId,
							env,
							linkLevel
						).get()
					);
				}
			}

			// Data source links (at the end of the load to avoid registration of objects which are removed later by an exception)
			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				if(&env == &Env::GetOfficialEnv())
				{
					user->setDataSourceLinksWithRegistration(
						ImportableTableSync::GetDataSourceLinksFromSerializedString(
							rows->getText(UserTableSync::COL_DATA_SOURCE_LINKS),
							env
					)	);
				}
				else
				{
					user->setDataSourceLinksWithoutRegistration(
						ImportableTableSync::GetDataSourceLinksFromSerializedString(
							rows->getText(UserTableSync::COL_DATA_SOURCE_LINKS),
							env
					)	);
				}
			}
		}


		template<>
		void OldLoadSavePolicy<UserTableSync,User>::Unlink(
			User* obj
		){
			if(Env::GetOfficialEnv().contains(*obj))
			{
				obj->cleanDataSourceLinks(true);
			}
		}



		template<>
		void OldLoadSavePolicy<UserTableSync,User>::Save(
			User* user,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<UserTableSync> query(*user);
			query.addField(user->getName());
			query.addField(user->getSurname());
			query.addField(user->getLogin());
			query.addField(user->getPasswordHash());
			query.addField(user->getProfile() ? user->getProfile()->getKey() : RegistryKeyType(0));
			query.addField(user->getAddress());
			query.addField(user->getPostCode());
			query.addField(user->getCityText());
			query.addField(user->getCityId());
			query.addField(user->getCountry());
			query.addField(user->getEMail());
			query.addField(user->getPhone());
			query.addFrameworkField<DateField>(user->getCreationDate());
			query.addField(user->getCreatorId());
			query.addField(user->getConnectionAllowed());
			query.addFrameworkField<DateField>(user->getBirthDate());
			query.addField(user->getLanguage() ? user->getLanguage()->getIso639_2Code() : string());
			query.addField(
				DataSourceLinks::Serialize(
					user->getDataSourceLinks()
			)	);
			query.addFieldNull(); // legacy SVN user
			query.addFieldNull(); // legacy SVN password
			query.execute(transaction);
		}


		template<> bool DBTableSyncTemplate<UserTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			try
			{
				Env env;
				UserTableSync::Get(
					object_id,
					env
				);
			}
			catch (ObjectNotFoundException<User>&)
			{
				return false;
			}

			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<SecurityRight>(DELETE_RIGHT);
		}



		template<> void DBTableSyncTemplate<UserTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<UserTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<UserTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			Env env;
			boost::shared_ptr<const User> user(UserTableSync::Get(id, env));
			SecurityLog::addUserAdmin(
				session->getUser().get(),
				user.get(),
				"Suppression de l'utilisateur "+ user->getLogin()
			);
		}
	}

	namespace security
	{
		boost::shared_ptr<User> UserTableSync::getUserFromLogin(const string& login )
		{
			Env& env(Env::GetOfficialEnv());
			DB* db = DBModule::GetDB();
			stringstream query;
			query
				<< "SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE " << TABLE_COL_LOGIN << "=" << Conversion::ToDBString(login);
			try
			{
				db::DBResultSPtr rows = db->execQuery(query.str());
				if (rows->next () == false)
					throw Exception("User "+ login + " not found in database.");

				boost::shared_ptr<User> user (new User(rows->getKey()));
				Load(user.get(), rows, env, UP_LINKS_LOAD_LEVEL);
				return user;
			}
			catch (DBException e)
			{
				throw Exception(e.getMessage());
			}
		}



		boost::shared_ptr<User> UserTableSync::getUserFromMail(const string& mail)
		{
			Env& env(Env::GetOfficialEnv());
			DB* db = DBModule::GetDB();
			stringstream query;
			query
				<< "SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE " << TABLE_COL_EMAIL << "=" << Conversion::ToDBString(mail);
			try
			{
				db::DBResultSPtr rows = db->execQuery(query.str());
				if (rows->next () == false)
					throw Exception("Mail "+ mail + " not found in database.");

				boost::shared_ptr<User> user (new User(rows->getKey()));
				Load(user.get(), rows, env, UP_LINKS_LOAD_LEVEL);
				return user;
			}
			catch (DBException e)
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
				query << " AND t." << TABLE_COL_LOGIN << " LIKE " << Conversion::ToDBString(*login);
			if(name)
				query << " AND t." << TABLE_COL_NAME << " LIKE " << Conversion::ToDBString(*name);
			if(surname)
				query << " AND t." << TABLE_COL_SURNAME << " LIKE " << Conversion::ToDBString(*surname);
			if(phone)
				query << " AND t." << TABLE_COL_PHONE << " LIKE " << Conversion::ToDBString(*phone);
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
				DB* db = DBModule::GetDB();
				stringstream query;
				query
					<< "SELECT " << TABLE_COL_ID
					<< " FROM " << TABLE.NAME
					<< " WHERE " << TABLE_COL_LOGIN << "=" << Conversion::ToDBString(login)
					<< " LIMIT 1 ";

				db::DBResultSPtr rows = db->execQuery(query.str());
				return (rows->next () != false);
			}
			catch (DBException e)
			{
				throw Exception(e.getMessage());
			}
		}
	}
}
