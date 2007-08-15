#include "03_db_ring/UpdateRecordTableSync.h"

#include "02_db/DBModule.h"

#include "03_db_ring/DbRingModule.h"
#include "03_db_ring/Node.h"

#include "01_util/Conversion.h"
#include "01_util/iostreams/Compression.h"

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

	template<> void SQLiteTableSyncTemplate<UpdateRecord>::load (UpdateRecord* object, const db::SQLiteResultSPtr& rows)
	{
	    object->setKey (rows->getLongLong (TABLE_COL_ID));
	    object->setTimestamp (rows->getTimestamp (UpdateRecordTableSync::TABLE_COL_TIMESTAMP));
	    object->setEmitterNodeId (rows->getInt ( UpdateRecordTableSync::TABLE_COL_EMITTERNODEID));
	    object->setState ((RecordState) rows->getInt ( UpdateRecordTableSync::TABLE_COL_STATE));

	    // Uncompress SQL
	    std::stringstream compressed (rows->getBlob (UpdateRecordTableSync::TABLE_COL_SQL));
	    std::stringstream sql;

	    Compression::ZlibDecompress (compressed, sql);
	    object->setSQL (sql.str ());
	}




	template<> void SQLiteTableSyncTemplate<UpdateRecord>::save (UpdateRecord* object)
	{
	    SQLiteHandle* sqlite = DBModule::GetSQLite();
	    std::stringstream query;

	    assert (object->getKey() != 0);
	    
	    query << "REPLACE INTO " << TABLE_NAME << " VALUES (:key, :timestamp, :emitter_node_id, :state, :sql)";
	    SQLiteStatementSPtr statement (sqlite->compileStatement (query.str ()));
	    
	    statement->bindParameterLongLong (":key", object->getKey ());
	    statement->bindParameterTimestamp (":timestamp", object->getTimestamp ());
	    statement->bindParameterLongLong (":emitter_node_id", object->getEmitterNodeId ());
	    statement->bindParameterInt (":state", object->getState ());

	    // Compress SQL
	    std::stringstream sql (object->getSQL ());
	    std::stringstream compressed;
	    Compression::ZlibCompress (sql, compressed);

	    statement->bindParameterBlob (":sql", compressed.str ());

	    sqlite->execUpdate (statement);

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
	    addTableColumn (TABLE_COL_SQL, "BLOB", false);
	    
	    addTableIndex(TABLE_COL_EMITTERNODEID);
	    addTableIndex(TABLE_COL_TIMESTAMP);
	    
	}
	
	
	
	
	UpdateRecordTableSync::~UpdateRecordTableSync ()
	{
	}
    


	void 
	UpdateRecordTableSync::rowsAdded (SQLiteQueueThreadExec* sqlite, 
					  SQLiteSync* sync,
					  const SQLiteResultSPtr& rows, bool isFirstSync)
	{
	    rowsUpdated (sqlite, sync, rows);
	}
	
	

	void 
	UpdateRecordTableSync::rowsUpdated (SQLiteQueueThreadExec* sqlite, 
					    SQLiteSync* sync,
					    const SQLiteResultSPtr& rows)
	{
	    while (rows->next ())
	    {
		UpdateRecord* ur = new UpdateRecord ();
		load (ur, rows);
		UpdateRecordSPtr urp (ur);
		DbRingModule::GetNode ()->setUpdateRecordCallback (urp);
	    }
	}
 


	void 
	UpdateRecordTableSync::rowsRemoved (SQLiteQueueThreadExec* sqlite, 
					    SQLiteSync* sync,
					    const SQLiteResultSPtr& rows)
	{

	}








	void 
	UpdateRecordTableSync::loadAllAfterTimestamp (UpdateLogSPtr dest, 
						      const boost::posix_time::ptime& timestamp,
						      bool inclusive)
	{
	    std::stringstream query;

	    query << "SELECT * FROM " << TABLE_NAME << " WHERE " 
		  << TABLE_COL_TIMESTAMP << " " << (inclusive ? ">=" : ">") << " " << Conversion::ToSQLiteString (to_iso_string (timestamp));

	    SQLiteResultSPtr result = DBModule::GetSQLite()->execQuery (query.str());
	    
	    while (result->next ())
	    {
		UpdateRecord* ur = new UpdateRecord ();
		load (ur, result);
		dest->setUpdateRecord (UpdateRecordSPtr (ur));
	    }
	}




	boost::posix_time::ptime 
	UpdateRecordTableSync::getLastPendingTimestamp ()
	{
	    std::stringstream query;

	    query << "SELECT MAX(" << TABLE_COL_TIMESTAMP << ") AS ts FROM " << TABLE_NAME ;
	    SQLiteResultSPtr result = DBModule::GetSQLite()->execQuery (query.str());
	    if (result->next () && (result->getText ("ts") != ""))
	    {
		return result->getTimestamp ("ts");
	    }
	    
	    return min_date_time;
	}




	long 
	UpdateRecordTableSync::getLastUpdateIndex (NodeId nodeId)
	{
	    std::stringstream query;

	    query << "SELECT COUNT(*) AS lui FROM " << TABLE_NAME <<  " WHERE " 
		  << TABLE_COL_EMITTERNODEID << "=" << nodeId;
	    SQLiteResultSPtr result = DBModule::GetSQLite()->execQuery (query.str());
	    if (result->next () && (result->getText ("lui") != ""))
	    {
		return result->getLong ("lui");
	    }
	    return 0;
	}



}
}

