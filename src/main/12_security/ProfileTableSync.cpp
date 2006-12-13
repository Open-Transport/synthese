
#include <sstream>

#include "01_util/Conversion.h"
#include "01_util/Log.h"

#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"
#include "02_db/SQLiteException.h"

#include "12_security/SecurityModule.h"
#include "12_security/ProfileTableSync.h"
#include "12_security/UserTableSyncException.h"
#include "12_security/Profile.h"

using namespace std;

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
			profile->setRights(rows.getColumn(rowId, ProfileTableSync::TABLE_COL_RIGHTS_STRING));
		}

		template<> void SQLiteTableSyncTemplate<Profile>::save( const db::SQLiteThreadExec* sqlite, Profile* profile )
		{
			try
			{
				if (profile->getKey() != 0)
				{
					// UPDATE
				}
				else // INSERT
				{
					/// @todo Implement control of the fields
					profile->setKey(getId(1,1));	/// @todo handle grid id
					stringstream query;
					query
						<< "INSERT INTO " << TABLE_NAME
						<< " VALUES(" 
						<< Conversion::ToString(profile->getKey())
						<< "," << Conversion::ToSQLiteString(profile->getName())
						<< "," << Conversion::ToString(profile->getParentId())
						<< "," << Conversion::ToSQLiteString(profile->getRightsString())
						<< ")";
					sqlite->execUpdate(query.str());
				}
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
		const std::string ProfileTableSync::TABLE_COL_ID = "id";
		const std::string ProfileTableSync::TABLE_COL_NAME = "name";
		const std::string ProfileTableSync::TABLE_COL_PARENT_ID = "parent";
		const std::string ProfileTableSync::TABLE_COL_RIGHTS_STRING = "rights";

		ProfileTableSync::ProfileTableSync()
			: db::SQLiteTableSyncTemplate<Profile>(TABLE_NAME, true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(TABLE_COL_NAME, "TEXT", true);
			addTableColumn(TABLE_COL_PARENT_ID, "INTEGER", true);
			addTableColumn(TABLE_COL_RIGHTS_STRING, "TEXT", true);
		}

		void ProfileTableSync::rowsAdded( const db::SQLiteThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{
			for (int i = 0; i < rows.getNbRows(); ++i)
			{
				Profile* profile = NULL;
				try
				{
					profile = new Profile(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID)));
					load(profile, rows, i);
					SecurityModule::getProfiles().add(profile);					
				}
				catch (Exception e)
				{
					delete profile;
					Log::GetInstance().warn("Profile load exception", e);
					continue;
				}
			}
		}

		void ProfileTableSync::rowsUpdated( const db::SQLiteThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{
			/// @todo Implementation
		}

		void ProfileTableSync::rowsRemoved( const db::SQLiteThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{
			/// @todo Implementation
		}

		std::vector<Profile*> ProfileTableSync::searchProfiles( const db::SQLiteThreadExec* sqlite , const std::string name , int first /*= 0*/, int number /*= 0*/ )
		{
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE " << TABLE_COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(name, false) << "%'";
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteResult result = sqlite->execQuery(query.str());
				vector<Profile*> profiles;
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					Profile* profile = new Profile;
					load(profile, result, i);
					profiles.push_back(profile);
				}
				return profiles;
			}
			catch(SQLiteException& e)
			{
				throw UserTableSyncException(e.getMessage());
			}
		}
	}
}

