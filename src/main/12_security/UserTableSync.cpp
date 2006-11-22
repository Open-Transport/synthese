
#include <sstream>

#include "01_util/Conversion.h"

#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"
#include "02_db/SQLiteException.h"

#include "12_security/SecurityModule.h"
#include "12_security/UserTableSync.h"
#include "12_security/User.h"
#include "12_security/UserTableSyncException.h"


namespace synthese
{
	using namespace db;
	using namespace util;

	namespace security
	{
		const std::string UserTableSync::TABLE_NAME = "t026_users";
		const std::string UserTableSync::TABLE_COL_ID = "id";
		const std::string UserTableSync::TABLE_COL_NAME = "name";
		const std::string UserTableSync::TABLE_COL_SURNAME = "surname";
		const std::string UserTableSync::TABLE_COL_LOGIN = "login";
		const std::string UserTableSync::TABLE_COL_PASSWORD = "password";
		const std::string UserTableSync::TABLE_COL_PROFILE_ID = "profile_id";

		UserTableSync::UserTableSync()
			: db::SQLiteTableSync ( TABLE_NAME, true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(TABLE_COL_NAME, "TEXT", true);
			addTableColumn(TABLE_COL_SURNAME, "TEXT", true);
			addTableColumn(TABLE_COL_LOGIN, "TEXT", true);
			addTableColumn(TABLE_COL_PASSWORD, "TEXT", true);
			addTableColumn(TABLE_COL_PROFILE_ID, "INTEGER", true);
		}


		void UserTableSync::rowsUpdated( const SQLiteThreadExec* sqlite,  SQLiteSync* sync, const SQLiteResult& rows )
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				/// @todo implementation
			}
		}


		void UserTableSync::rowsAdded( const SQLiteThreadExec* sqlite,  SQLiteSync* sync, const SQLiteResult& rows )
		{
		}


		void UserTableSync::rowsRemoved( const SQLiteThreadExec* sqlite,  SQLiteSync* sync, const SQLiteResult& rows )
		{
			/// @todo implementation
		}


		User* UserTableSync::getUser( const db::SQLiteThreadExec* sqlite, uid id )
		{
			std::stringstream query;
			query
				<< "SELECT * "
				<< "FROM " << TABLE_NAME
				<< "WHERE " << TABLE_COL_ID << "=" << Conversion::ToString(id);
			try
			{
				db::SQLiteResult rows = sqlite->execQuery(query.str());
				if (rows.getNbRows() <= 0)
					throw UserTableSyncException("User "+ Conversion::ToString(id) + " not found in database.");
				User* user = new User;
				loadUser(user, rows);
				return user;
			}
			catch (SQLiteException e)
			{
				throw UserTableSyncException(e.getMessage());
			}
		}

		User* UserTableSync::getUser( const db::SQLiteThreadExec* sqlite, const std::string& login )
		{
			std::stringstream query;
			query
				<< "SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE " << TABLE_COL_LOGIN << "='" << login << "'";	/// @todo Put a string converter to escape special characters
			try
			{
				db::SQLiteResult rows = sqlite->execQuery(query.str());
				if (rows.getNbRows() <= 0)
					throw UserTableSyncException("User "+ login + " not found in database.");
				User* user = new User;
				loadUser(user, rows);
				return user;
			}
			catch (SQLiteException e)
			{
				throw UserTableSyncException(e.getMessage());
			}
		}

		void UserTableSync::loadUser(User* user, const db::SQLiteResult& rows, int rowId)
		{
			user->setId(Conversion::ToLongLong(rows.getColumn(rowId, TABLE_COL_ID)));
			user->setPassword(rows.getColumn(rowId, TABLE_COL_PASSWORD));
			user->setName(rows.getColumn(rowId, TABLE_COL_NAME));
			user->setSurname(rows.getColumn(rowId, TABLE_COL_SURNAME));
			user->setLogin(rows.getColumn(rowId, TABLE_COL_LOGIN));
			try
			{
				user->setProfile(SecurityModule::getProfiles().get(Conversion::ToLongLong(rows.getColumn(rowId, TABLE_COL_PROFILE_ID))));
			}
			catch (Profile::RegistryKeyException e)
			{
				throw UserTableSyncException("Bad profile "+ rows.getColumn(rowId, TABLE_COL_PROFILE_ID));
			}
		}

		void UserTableSync::saveUser( const db::SQLiteThreadExec* sqlite, User* user )
		{
			if (user->getId() != 0)
			{
				// UPDATE
			}
			else
			{
				// INSERT
			}
		}
	}
}


