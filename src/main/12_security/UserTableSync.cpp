
#include "01_util/Conversion.h"

#include "02_db/SQLiteResult.h"

#include "12_security/SecurityModule.h"
#include "12_security/UserTableSync.h"
#include "12_security/User.h"


namespace synthese
{
	using namespace db;
	using namespace util;

	namespace security
	{
		static const std::string TABLE_NAME = "t026_users";
		static const std::string TABLE_COL_ID = "id";
		static const std::string TABLE_COL_NAME = "name";
		static const std::string TABLE_COL_SURNAME = "surname";
		static const std::string TABLE_COL_PASSWORD = "password";
		static const std::string TABLE_COL_PROFILE_ID = "profile_id";

		UserTableSync::UserTableSync()
			: db::SQLiteTableSync ( TABLE_NAME, true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(TABLE_COL_NAME, "TEXT", true);
			addTableColumn(TABLE_COL_SURNAME, "TEXT", true);
			addTableColumn(TABLE_COL_PASSWORD, "TEXT", true);
			addTableColumn(TABLE_COL_PROFILE_ID, "INTEGER", false);
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
		}

		User* UserTableSync::getUser( const db::SQLiteResult& rows )
		{
			User* user = new User(Conversion::ToLongLong(rows.getColumn(1, TABLE_COL_ID)));
			loadUser(rows, user);
			return user;
		}

		void UserTableSync::loadUser( const db::SQLiteResult& rows, User* user )
		{
			user->setPassword(rows.getColumn(1, TABLE_COL_PASSWORD));
			user->setName(rows.getColumn(1, TABLE_COL_NAME));
			user->setSurname(rows.getColumn(1, TABLE_COL_SURNAME));
			user->setLogin(rows.getColumn(1, TABLE_COL_LOGIN));
			user->setProfile(SecurityModule::getProfiles().get(Conversion::ToLongLong(rows.getColumn(1, TABLE_COL_PROFILE_ID))));
		}

		db::SQLiteResult UserTableSync::fetchUser( uid )
		{
			db::SQLiteResult row;
			return row;
		}
	}
}


