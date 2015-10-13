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

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<UserTableSync>::TABLE(
			"t026_users", true
		);

		template<> const Field DBTableSyncTemplate<UserTableSync>::_FIELDS[]=
		{
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<UserTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(DBTableSync::Index(Name::FIELD.name.c_str(), ""));
			r.push_back(DBTableSync::Index(Login::FIELD.name.c_str(), ""));
			r.push_back(DBTableSync::Index(UserProfile::FIELD.name.c_str(), ""));
			return r;
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
				<< " WHERE " << Login::FIELD.name << "=" << Conversion::ToDBString(login);
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
				<< " WHERE " << Email::FIELD.name << "=" << Conversion::ToDBString(mail);
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
				query << " INNER JOIN " << ProfileTableSync::TABLE.NAME << " AS p ON p." << TABLE_COL_ID << "=t." << UserProfile::FIELD.name;
			query << " WHERE 1 ";
			if(login)
				query << " AND t." << Login::FIELD.name << " LIKE " << Conversion::ToDBString(*login);
			if(name)
				query << " AND t." << Name::FIELD.name << " LIKE " << Conversion::ToDBString(*name);
			if(surname)
				query << " AND t." << SurName::FIELD.name << " LIKE " << Conversion::ToDBString(*surname);
			if(phone)
				query << " AND t." << Phone::FIELD.name << " LIKE " << Conversion::ToDBString(*phone);
			if(profileId)
				query << " AND " << UserProfile::FIELD.name << "=" << *profileId;
			if (!indeterminate(emptyLogin))
				query << " AND " << Login::FIELD.name << (emptyLogin ? "=''" : "!=''");
			if (!indeterminate(emptyPhone))
				query << " AND " << Phone::FIELD.name << (emptyPhone ? "=''" : "!=''");
			if(differentUserId)
				query << " AND " << TABLE_COL_ID << "!=" << *differentUserId;

			if (orderByProfileName)
				query << " ORDER BY p." << Name::FIELD.name << (raisingOrder ? " ASC" : " DESC");
			else if (orderByLogin)
				query <<
					" ORDER BY t." << Login::FIELD.name << (raisingOrder ? " ASC" : " DESC") << "," <<
					Name::FIELD.name << (raisingOrder ? " ASC" : " DESC") << "," <<
					SurName::FIELD.name << (raisingOrder ? " ASC" : " DESC")
				;
			else if (orderByName)
				query << " ORDER BY t." << Name::FIELD.name << (raisingOrder ? " ASC" : " DESC") << ",t." << SurName::FIELD.name << (raisingOrder ? " ASC" : " DESC");
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
					<< " WHERE " << Login::FIELD.name << "=" << Conversion::ToDBString(login)
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
