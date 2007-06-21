#include "03_db_ring/UpdateRecordTableSync.h"

#include "02_db/DBModule.h"

#include "03_db_ring/DbRingModule.h"
#include "03_db_ring/Node.h"

#include "01_util/Conversion.h"

#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/date_time/posix_time/time_parsers.hpp>

#include <sstream>


using namespace boost::posix_time;
using namespace synthese::util;



namespace synthese
{

    using namespace util;
    using namespace db;
    using namespace dbring;

    namespace db
    {
	template<> const std::string SQLiteTableSyncTemplate<UpdateRecord>::TABLE_NAME = "t997_update_log";
	template<> const int SQLiteTableSyncTemplate<UpdateRecord>::TABLE_ID = 997;
	template<> const bool SQLiteTableSyncTemplate<UpdateRecord>::HAS_AUTO_INCREMENT = true;

	template<> void SQLiteTableSyncTemplate<UpdateRecord>::load (UpdateRecord* object, const db::SQLiteResult& rows, int rowId/*=0*/ )
	{
	    object->setKey (Conversion::ToLongLong(rows.getColumn(rowId, TABLE_COL_ID)));
	    object->setTimestamp (from_iso_string (rows.getColumn (rowId, UpdateRecordTableSync::TABLE_COL_TIMESTAMP)));
	    object->setEmitterNodeId (Conversion::ToInt (rows.getColumn (rowId, UpdateRecordTableSync::TABLE_COL_EMITTERNODEID)));
	    object->setState ((RecordState) Conversion::ToInt (rows.getColumn (rowId, UpdateRecordTableSync::TABLE_COL_STATE)));
	    object->setSQL (rows.getColumn (rowId, UpdateRecordTableSync::TABLE_COL_SQL));
	}




	template<> void SQLiteTableSyncTemplate<UpdateRecord>::save (UpdateRecord* object)
	{
	    const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
	    std::stringstream query;

	    assert (object->getKey() != 0);
	    
	    query << "REPLACE INTO " << TABLE_NAME << " VALUES ("
		  << object->getKey () << ","   
		  << Conversion::ToSQLiteString (to_iso_string (object->getTimestamp ())) << "," 
		  << object->getEmitterNodeId () << ","
		  << object->getState () << ","
		  << Conversion::ToSQLiteString (object->getSQL ()) 
		  << ")";	    

	    sqlite->execUpdate(query.str());
	}
    }


    namespace dbring
    {
	const std::string UpdateRecordTableSync::TABLE_COL_TIMESTAMP ("timestamp");
	const std::string UpdateRecordTableSync::TABLE_COL_EMITTERNODEID ("emitter_node_id");
	const std::string UpdateRecordTableSync::TABLE_COL_STATE ("state");
	const std::string UpdateRecordTableSync::TABLE_COL_SQL ("sql");



	UpdateRecordTableSync::UpdateRecordTableSync ()
	    : SQLiteTableSyncTemplate<UpdateRecord> (true, true, db::TRIGGERS_ENABLED_CLAUSE, true)
	{
	    // Note : ignore callbacks on first sync. the update log must be populated only when necessary.
	    
	    addTableColumn (TABLE_COL_ID, "INTEGER", false);
	    addTableColumn (TABLE_COL_TIMESTAMP, "TEXT", false);
	    addTableColumn (TABLE_COL_EMITTERNODEID, "INTEGER", false);
	    addTableColumn (TABLE_COL_STATE, "INTEGER", false);
	    addTableColumn (TABLE_COL_SQL, "TEXT", false);
	    
	    addTableIndex(TABLE_COL_EMITTERNODEID);
	    addTableIndex(TABLE_COL_TIMESTAMP);
	    
	}
	
	
	
	
	UpdateRecordTableSync::~UpdateRecordTableSync ()
	{
	}
    


	void 
	UpdateRecordTableSync::rowsAdded (const SQLiteQueueThreadExec* sqlite, 
					  SQLiteSync* sync,
					  const SQLiteResult& rows, bool isFirstSync)
	{
	    rowsUpdated (sqlite, sync, rows);
	}
	
	

	void 
	UpdateRecordTableSync::rowsUpdated (const SQLiteQueueThreadExec* sqlite, 
					    SQLiteSync* sync,
					    const SQLiteResult& rows)
	{
	    for (int i=0; i<rows.getNbRows (); ++i)
	    {
		UpdateRecord* ur = new UpdateRecord ();
		load (ur, rows, i);
		UpdateRecordSPtr urp (ur);
		DbRingModule::GetNode ()->setUpdateRecordCallback (urp);
	    }
	}
 


	void 
	UpdateRecordTableSync::rowsRemoved (const SQLiteQueueThreadExec* sqlite, 
					    SQLiteSync* sync,
					    const SQLiteResult& rows)
	{

	}








	void 
	UpdateRecordTableSync::loadAllAfterTimestamp (UpdateLogSPtr dest, 
						      const boost::posix_time::ptime& timestamp)
	{
	    std::stringstream query;

	    query << "SELECT * FROM " << TABLE_NAME << " WHERE " 
		  << TABLE_COL_TIMESTAMP << " > " << Conversion::ToSQLiteString (to_iso_string (timestamp));

	    SQLiteResult result = DBModule::GetSQLite()->execQuery (query.str());
	    
	    for (int i=0; i<result.getNbRows (); ++i)
	    {
		UpdateRecord* ur = new UpdateRecord ();
		load (ur, result, i);
		dest->setUpdateRecord (UpdateRecordSPtr (ur));
	    }
	}




	boost::posix_time::ptime 
	UpdateRecordTableSync::getLastPendingTimestamp ()
	{
	    std::stringstream query;

	    query << "SELECT MAX(" << TABLE_COL_TIMESTAMP << ") AS ts FROM " << TABLE_NAME ;
	    SQLiteResult result = DBModule::GetSQLite()->execQuery (query.str());
	    if (result.getNbRows () && (result.getColumn (0, "ts") != ""))
	    {
		std::string ts (result.getColumn (0, "ts"));
		return from_iso_string (ts);
	    }
	    
	    return min_date_time;
	}


	long 
	UpdateRecordTableSync::getLastUpdateIndex (NodeId nodeId)
	{
	    std::stringstream query;

	    query << "SELECT COUNT(*) AS lui FROM " << TABLE_NAME <<  " WHERE " 
		  << TABLE_COL_EMITTERNODEID << "=" << nodeId;
	    SQLiteResult result = DBModule::GetSQLite()->execQuery (query.str());
	    if (result.getNbRows () && (result.getColumn (0, "lui") != ""))
	    {
		std::string lui (result.getColumn (0, "lui"));
		return Conversion::ToLong (lui);
	    }
	    return 0;
	}



}
}

