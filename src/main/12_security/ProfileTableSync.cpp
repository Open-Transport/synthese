
#include "01_util/Conversion.h"
#include "01_util/Log.h"

#include "02_db/SQLiteResult.h"

#include "12_security/SecurityModule.h"
#include "12_security/ProfileTableSync.h"
#include "12_security/Profile.h"

namespace synthese
{
	using namespace util;
	using namespace db;

	namespace security
	{
		const std::string ProfileTableSync::TABLE_NAME = "t027_profiles";
		const std::string ProfileTableSync::TABLE_COL_ID = "id";
		const std::string ProfileTableSync::TABLE_COL_NAME = "name";
		const std::string ProfileTableSync::TABLE_COL_PARENT_ID = "parent";
		const std::string ProfileTableSync::TABLE_COL_RIGHTS_STRING = "rights";

		ProfileTableSync::ProfileTableSync()
			: SQLiteTableSync(TABLE_NAME, true, true)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(TABLE_COL_NAME, "TEXT", true);
			addTableColumn(TABLE_COL_PARENT_ID, "INTEGER", true);
			addTableColumn(TABLE_COL_RIGHTS_STRING, "TEXT", true);
		}

		void ProfileTableSync::loadProfile(Profile* profile, const db::SQLiteResult& rows, int rowId/*=0*/ )
		{
			profile->setName(rows.getColumn(rowId, TABLE_COL_NAME));
			profile->setParent(Conversion::ToLongLong(rows.getColumn(rowId, TABLE_COL_PARENT_ID)));
			profile->setRights(rows.getColumn(rowId, TABLE_COL_RIGHTS_STRING));
		}

		void ProfileTableSync::rowsAdded( const db::SQLiteThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{
			for (int i = 0; i < rows.getNbRows(); ++i)
			{
				Profile* profile = NULL;
				try
				{
					profile = new Profile(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID)));
					loadProfile(profile, rows, i);
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
	}
}
