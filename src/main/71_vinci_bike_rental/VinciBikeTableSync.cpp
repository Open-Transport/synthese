
#include <sstream>

#include "71_vinci_bike_rental/VinciBike.h"
#include "71_vinci_bike_rental/VinciBikeTableSync.h"

using namespace std;

namespace synthese
{
	using namespace db;
	using namespace vinci;

	namespace db
	{
		const std::string SQLiteTableSyncTemplate<VinciBike>::TABLE_NAME = "t032_vinci_bike";
		const int SQLiteTableSyncTemplate<VinciBike>::TABLE_ID = 32;
		const bool SQLiteTableSyncTemplate<VinciBike>::HAS_AUTO_INCREMENT = true;

		void SQLiteTableSyncTemplate<VinciBike>::load(VinciBike* bike, const db::SQLiteResult& rows, int rowId)
		{
			bike->setKey(Conversion::ToLongLong(rows.getColumn(rowId, VinciBikeTableSync::TABLE_COL_ID)));
			bike->_number = rows.getColumn(rowId, VinciBikeTableSync::TABLE_COL_NUMBER);
			bike->_markedNumber = rows.getColumn(rowId, VinciBikeTableSync::TABLE_COL_MARKED_NUMBER);
		}

		void SQLiteTableSyncTemplate<VinciBike>::save(const db::SQLiteThreadExec* sqlite, VinciBike* bike)
		{
			stringstream query;
			if (bike->getKey() != 0)
			{	//UPODATE
				query << "UPDATE " << TABLE_NAME << " SET "
					<< VinciBikeTableSync::TABLE_COL_ID << "=" << Conversion::ToString(bike->getKey())
					<< "," << VinciBikeTableSync::TABLE_COL_NUMBER << "=" << Conversion::ToSQLiteString(bike->_number)
					<< "," << VinciBikeTableSync::TABLE_COL_MARKED_NUMBER << "=" << Conversion::ToSQLiteString(bike->_markedNumber)
					;
			}
			else
			{	// INSERT
				bike->setKey(getId(0,0)); /// @todo Handle grid number
				query << "INSERT INTO " << TABLE_NAME << " VALUES("
					<< Conversion::ToString(bike->getKey())
					<< Conversion::ToSQLiteString(bike->_number)
					<< Conversion::ToSQLiteString(bike->_markedNumber)
					<< ")";
			}
			sqlite->execUpdate(query.str());
		}

	}

	namespace vinci
	{
		const std::string VinciBikeTableSync::TABLE_COL_ID = "id";
		const std::string VinciBikeTableSync::TABLE_COL_NUMBER = "number";
		const std::string VinciBikeTableSync::TABLE_COL_MARKED_NUMBER = "marked_number";

		VinciBikeTableSync::VinciBikeTableSync()
			: db::SQLiteTableSyncTemplate<VinciBike>(TABLE_NAME, true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(TABLE_COL_NUMBER, "TEXT", true);
			addTableColumn(TABLE_COL_MARKED_NUMBER, "TEXT", true);
		}

		VinciBikeTableSync::~VinciBikeTableSync()
		{}
		
		/** Action to do on user creation.
		No action because the users are not permanently loaded in ram.
		*/
		void VinciBikeTableSync::rowsAdded (const db::SQLiteThreadExec* sqlite, 
			db::SQLiteSync* sync,
			const db::SQLiteResult& rows)
		{}

		/** Action to do on user creation.
		Updates the users objects in the opened sessions.
		*/
		void VinciBikeTableSync::rowsUpdated (const db::SQLiteThreadExec* sqlite, 
			db::SQLiteSync* sync,
			const db::SQLiteResult& rows)
		{}

		/** Action to do on user deletion.
		Closes the sessions of the deleted user.
		*/
		void VinciBikeTableSync::rowsRemoved (const db::SQLiteThreadExec* sqlite, 
			db::SQLiteSync* sync,
			const db::SQLiteResult& rows)
		{}


	}
}
