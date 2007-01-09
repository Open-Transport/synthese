
#include <sstream>

#include "01_util/Conversion.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"
#include "02_db/SQLiteException.h"

#include "12_security/SecurityModule.h"
#include "12_security/UserTableSync.h"
#include "12_security/User.h"
#include "12_security/UserTableSyncException.h"

using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace security;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<User>::TABLE_NAME = "t026_users";
		template<> const int SQLiteTableSyncTemplate<User>::TABLE_ID = 26;
		template<> const bool SQLiteTableSyncTemplate<User>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<User>::load(User* user, const db::SQLiteResult& rows, int rowId)
		{
			user->setKey(Conversion::ToLongLong(rows.getColumn(rowId, TABLE_COL_ID)));
			user->setPassword(rows.getColumn(rowId, UserTableSync::TABLE_COL_PASSWORD));
			user->setName(rows.getColumn(rowId, UserTableSync::TABLE_COL_NAME));
			user->setSurname(rows.getColumn(rowId, UserTableSync::TABLE_COL_SURNAME));
			user->setLogin(rows.getColumn(rowId, UserTableSync::TABLE_COL_LOGIN));
			user->_address = rows.getColumn(rowId, UserTableSync::TABLE_COL_ADDRESS);
			user->_postCode = rows.getColumn(rowId, UserTableSync::TABLE_COL_POST_CODE);
			user->_cityText = rows.getColumn(rowId, UserTableSync::TABLE_COL_CITY_TEXT);
			user->_cityId = Conversion::ToLongLong(rows.getColumn(rowId, UserTableSync::TABLE_COL_CITY_ID));
			user->_country = rows.getColumn(rowId, UserTableSync::TABLE_COL_COUNTRY);
			user->_email = rows.getColumn(rowId, UserTableSync::TABLE_COL_EMAIL);
			user->_phone = rows.getColumn(rowId, UserTableSync::TABLE_COL_PHONE);
			try
			{
				user->setProfile(SecurityModule::getProfiles().get(Conversion::ToLongLong(rows.getColumn(rowId, UserTableSync::TABLE_COL_PROFILE_ID))));
			}
			catch (Profile::RegistryKeyException e)
			{
				throw UserTableSyncException("Bad profile "+ rows.getColumn(rowId, UserTableSync::TABLE_COL_PROFILE_ID));
			}
		}

		template<> void SQLiteTableSyncTemplate<User>::save(User* user )
		{
			try
			{
				const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
				stringstream query;
				if (user->getKey() != 0)
				{
					query
						<< "UPDATE " << TABLE_NAME
						<< " SET "
						<< UserTableSync::TABLE_COL_NAME << "=" << Conversion::ToSQLiteString(user->getName())
						<< "," << UserTableSync::TABLE_COL_SURNAME << "=" << Conversion::ToSQLiteString(user->getSurname())
						<< "," << UserTableSync::TABLE_COL_LOGIN << "=" << Conversion::ToSQLiteString(user->getLogin())
						<< "," << UserTableSync::TABLE_COL_PASSWORD << "=" << Conversion::ToSQLiteString(user->getPassword())
						<< "," << UserTableSync::TABLE_COL_PROFILE_ID << "=" << Conversion::ToString(user->getProfile()->getKey())
						<< "," << UserTableSync::TABLE_COL_ADDRESS << "=" << Conversion::ToSQLiteString(user->_address)
						<< "," << UserTableSync::TABLE_COL_POST_CODE << "=" << Conversion::ToSQLiteString(user->_postCode)
						<< "," << UserTableSync::TABLE_COL_CITY_TEXT << "=" << Conversion::ToSQLiteString(user->_cityText)
						<< "," << UserTableSync::TABLE_COL_CITY_ID << "=" << Conversion::ToString(user->_cityId)
						<< "," << UserTableSync::TABLE_COL_COUNTRY << "=" << Conversion::ToSQLiteString(user->_country)
						<< "," << UserTableSync::TABLE_COL_EMAIL << "=" << Conversion::ToSQLiteString(user->_email)
						<< "," << UserTableSync::TABLE_COL_PHONE << "=" << Conversion::ToSQLiteString(user->_phone)
						<< " WHERE " << TABLE_COL_ID << "=" << Conversion::ToString(user->getKey());
					
				}
				else // INSERT
				{
					/// @todo Implement control of the fields
					user->setKey(getId(1,1));	/// @todo handle grid id
					query
						<< "INSERT INTO " << TABLE_NAME
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
						<< ")";
				}
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
		const std::string UserTableSync::TABLE_COL_ID = "id";
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

		UserTableSync::UserTableSync()
			: db::SQLiteTableSyncTemplate<User> ( TABLE_NAME, true, true, TRIGGERS_ENABLED_CLAUSE)
		{
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
		}


		void UserTableSync::rowsUpdated( const SQLiteQueueThreadExec* sqlite,  SQLiteSync* sync, const SQLiteResult& rows )
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				/// @todo implementation
			}
		}


		void UserTableSync::rowsAdded( const SQLiteQueueThreadExec* sqlite,  SQLiteSync* sync, const SQLiteResult& rows )
		{
		}


		void UserTableSync::rowsRemoved( const SQLiteQueueThreadExec* sqlite,  SQLiteSync* sync, const SQLiteResult& rows )
		{
			/// @todo implementation
		}

		User* UserTableSync::getUserFromLogin(const std::string& login )
		{
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			std::stringstream query;
			query
				<< "SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE " << TABLE_COL_LOGIN << "=" << Conversion::ToSQLiteString(login);
			try
			{
				db::SQLiteResult rows = sqlite->execQuery(query.str());
				if (rows.getNbRows() <= 0)
					throw UserTableSyncException("User "+ login + " not found in database.");
				User* user = new User;
				load(user, rows);
				return user;
			}
			catch (SQLiteException e)
			{
				throw UserTableSyncException(e.getMessage());
			}
		}

		std::vector<User*> UserTableSync::search(const std::string& login, const std::string name, uid profileId, int first /*= 0*/, int number /*= 0*/ )
		{
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE " << TABLE_COL_LOGIN << " LIKE '%" << Conversion::ToSQLiteString(login, false) << "%'"
				<< " AND " << TABLE_COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(name, false) << "%'";
			if (profileId > 0)
				query << " AND " << TABLE_COL_PROFILE_ID << "=" << Conversion::ToString(profileId);
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);
			
			try
			{
				SQLiteResult result = sqlite->execQuery(query.str());
				vector<User*> users;
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					User* user = new User;
					load(user, result, i);
					users.push_back(user);
				}
				return users;
			}
			catch(SQLiteException& e)
			{
				throw UserTableSyncException(e.getMessage());
			}
		}
	}
}



