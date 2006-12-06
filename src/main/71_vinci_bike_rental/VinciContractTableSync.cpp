
#include <sstream>

#include "02_db/SQLiteResult.h"

#include "12_security/User.h"
#include "12_security/UserTableSync.h"

#include "71_vinci_bike_rental/VinciContract.h"
#include "71_vinci_bike_rental/VinciContractTableSync.h"

using namespace std;

namespace synthese
{
	using namespace db;
	using namespace security;
	using namespace vinci;
	
	namespace db
	{
		const std::string SQLiteTableSyncTemplate<VinciContract>::TABLE_NAME = "t035_vinci_contract";
		const int SQLiteTableSyncTemplate<VinciContract>::TABLE_ID = 35;
		const bool SQLiteTableSyncTemplate<VinciContract>::HAS_AUTO_INCREMENT = true;

		void SQLiteTableSyncTemplate<VinciContract>::load(VinciContract* vc, const SQLiteResult& rows, int rowId)
		{
			vc->setKey(Conversion::ToLongLong(rows.getColumn(rowId, VinciContractTableSync::TABLE_COL_ID)));
			vc->_userId = Conversion::ToLongLong(rows.getColumn(rowId, VinciContractTableSync::TABLE_COL_USER_ID));
		}

		void SQLiteTableSyncTemplate<VinciContract>::save(const SQLiteThreadExec* sqlite, VinciContract* vc)
		{
			stringstream query;
			if (vc->getKey() != 0)
			{	// UPDATE
				query << "UPDATE " << TABLE_NAME << " SET "
					<< VinciContractTableSync::TABLE_COL_USER_ID << "=" << Conversion::ToString(vc->_userId)
					<< " WHERE " << VinciContractTableSync::TABLE_COL_ID << "=" << vc->getKey();
			}
			else
			{	// INSERT
				vc->setKey(getId(0,0)); /// @todo Handle grid ID
				query << "INSERT INTO " << TABLE_NAME << " VALUES("
					<< vc->getKey()
					<< "," << Conversion::ToString(vc->_userId)
					<< ")";
			}
			sqlite->execUpdate(query.str());
		}
	}

	namespace vinci
	{
		const std::string VinciContractTableSync::TABLE_COL_ID = "id";
		const std::string VinciContractTableSync::TABLE_COL_USER_ID = "user_id";
		
		VinciContractTableSync::VinciContractTableSync()
			: SQLiteTableSyncTemplate<VinciContract>(TABLE_NAME, true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(TABLE_COL_USER_ID, "INTEGER", true);
		}

		void VinciContractTableSync::rowsAdded( const db::SQLiteThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{

		}

		void VinciContractTableSync::rowsUpdated( const db::SQLiteThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{

		}

		void VinciContractTableSync::rowsRemoved( const db::SQLiteThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{

		}

		std::vector<VinciContract*> VinciContractTableSync::searchVinciContracts( const db::SQLiteThreadExec* sqlite , std::string name /*= ""*/, std::string surname /*= "" */, int first /*= 0*/, int number /*=-1*/ )
		{
			stringstream query;
			query
				<< "SELECT *"
				<< " FROM "
					<< TABLE_NAME << " AS c "
					<< " INNER JOIN " << UserTableSync::TABLE_NAME << " AS u ON c." << TABLE_COL_ID << "=u." << UserTableSync::TABLE_COL_ID
				<< " WHERE "
					<< "u." << UserTableSync::TABLE_COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(name, false) << "%'"
					<< " AND u." << UserTableSync::TABLE_COL_SURNAME << " LIKE '%" << Conversion::ToSQLiteString(surname, false) << "%'"
				<< " LIMIT " << number << "," << first;
			SQLiteResult result = sqlite->execQuery(query.str());
			vector<VinciContract*> contracts;
			for (int i=0; i<result.getNbRows(); ++i)
			{
				User* user = new User;
				SQLiteTableSyncTemplate<User>::load(user, result, i);
				VinciContract* contract = new VinciContract;
				SQLiteTableSyncTemplate<VinciContract>::load(contract, result, i);
				contract->_user = user;
				contracts.push_back(contract);
			}
			return contracts;
		}
	}
}