
/** UpdateRecordTableSync class implementation.
	@file UpdateRecordTableSync.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

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

	namespace util
	{
		template<> const std::string FactorableTemplate<SQLiteTableSync,UpdateRecordTableSync>::FACTORY_KEY("1 Update log");
	}

    namespace db
    {
	template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<UpdateRecordTableSync>::TABLE.NAME = "t997_update_log";
	template<> const int SQLiteTableSyncTemplate<UpdateRecordTableSync>::TABLE.ID = 997;
	template<> const bool SQLiteTableSyncTemplate<UpdateRecordTableSync>::HAS_AUTO_INCREMENT = true;


	
	
	template<> void SQLiteDirectTableSyncTemplate<UpdateRecordTableSync,UpdateRecord>::_link(UpdateRecord* obj, const SQLiteResultSPtr& rows, GetSource temporary)
	{
	}


	template<> void SQLiteDirectTableSyncTemplate<UpdateRecordTableSync,UpdateRecord>::_unlink(UpdateRecord* obj)
	{
	}



	template<> void SQLiteDirectTableSyncTemplate<UpdateRecordTableSync,UpdateRecord>::load (UpdateRecord* object, const db::SQLiteResultSPtr& rows)
	{
	    object->setKey (rows->getLongLong (TABLE_COL_ID));
	    object->setTimestamp (rows->getTimestamp (UpdateRecordTableSync::TABLE_COL_TIMESTAMP));
	    object->setEmitterNodeId (rows->getInt ( UpdateRecordTableSync::TABLE_COL_EMITTERNODEID));
	    object->setState ((RecordState) rows->getInt ( UpdateRecordTableSync::TABLE_COL_STATE));

	    // Load compressed SQL if available only
	    if (rows->getColumnIndex (UpdateRecordTableSync::TABLE_COL_SQL) != -1)
	    {
		object->setCompressedSQL (rows->getBlob (UpdateRecordTableSync::TABLE_COL_SQL));
	    }
	}




	template<> void SQLiteDirectTableSyncTemplate<UpdateRecordTableSync,UpdateRecord>::save (UpdateRecord* object)
	{
	    SQLite* sqlite = DBModule::GetSQLite();
	    std::stringstream query;

	    assert (object->getKey() != 0);
	    assert (object->hasCompressedSQL ());

		query << "REPLACE INTO " << TABLE.NAME << " VALUES (:key, :timestamp, :emitter_node_id, :state, :sql)";

	    SQLiteStatementSPtr statement (sqlite->compileStatement (query.str ()));
	    
	    statement->bindParameterLongLong (":key", object->getKey ());
	    statement->bindParameterTimestamp (":timestamp", object->getTimestamp ());
	    statement->bindParameterLongLong (":emitter_node_id", object->getEmitterNodeId ());
	    statement->bindParameterInt (":state", object->getState ());
	    statement->bindParameterBlob (":sql", object->getCompressedSQL ());

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
	    : SQLiteDirectTableSyncTemplate<UpdateRecordTableSync,UpdateRecord> ()
	{
	    addTableColumn (TABLE_COL_ID, "SQL_INTEGER", false);
	    addTableColumn (TABLE_COL_TIMESTAMP, "SQL_TEXT", true);
	    addTableColumn (TABLE_COL_EMITTERNODEID, "SQL_INTEGER", false);
	    addTableColumn (TABLE_COL_STATE, "SQL_INTEGER", true);
	    addTableColumn (TABLE_COL_SQL, "BLOB", false, false);  // Not loaded on callback
	    
	    addTableIndex(TABLE_COL_EMITTERNODEID);
	    addTableIndex(TABLE_COL_TIMESTAMP);
	}
	
	
	
	
	UpdateRecordTableSync::~UpdateRecordTableSync ()
	{
	}
    


	void 
	UpdateRecordTableSync::rowsAdded (SQLite* sqlite, 
					  SQLiteSync* sync,
					  const SQLiteResultSPtr& rows, bool isFirstSync)
	{
	    rowsUpdated (sqlite, sync, rows);
	}
	
	

	void 
	UpdateRecordTableSync::rowsUpdated (SQLite* sqlite, 
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
	UpdateRecordTableSync::rowsRemoved (SQLite* sqlite, 
					    SQLiteSync* sync,
					    const SQLiteResultSPtr& rows)
	{

	}


	void 
	UpdateRecordTableSync::LoadDeltaUpdate (UpdateRecordSet& dest, 
						const boost::posix_time::ptime& clientLastAcknowledgedTimestamp,
						std::set<uid> clientLastPendingIds)
	{
	    // First query only necessary ids.
	    // The delta is composed of:
	    // * All pending records that are not already pending in client update log (known through clientLastPendingIds)
	    // * All acknowledged records that are flagged pending in client update log. Those ones are not fully loaded since the SQL
	    //   compressed part have already been sent.


	    // First part : full load of 
            // - acknowledged/failed records that are not known on client side and after client last acknowledged timestamp
	    // - pending records not known on client side whenever they occured (will be processed as late records on authority)
	    
	    {
		std::stringstream query;
		query << "SELECT * FROM " << TABLE.NAME << " WHERE " ;

		query << "((" << TABLE_COL_TIMESTAMP << " > " << Conversion::ToSQLiteString (to_iso_string (clientLastAcknowledgedTimestamp)) << ") OR ";
		query << "(" << TABLE_COL_STATE << "=" << PENDING << ")) AND";

		query << " (" << TABLE_COL_ID << " NOT IN (";
		for (std::set<uid>::const_iterator it = clientLastPendingIds.begin ();
		     it != clientLastPendingIds.end (); ++it)
		{
		    if (it != clientLastPendingIds.begin ()) query << ",";
		    query << "'" << Conversion::ToString (*it) << "'";
		}
		query << "))" ;
		
		// query << "(" << TABLE_COL_STATE << "=" << PENDING << " OR " << TABLE_COL_STATE << "=" << ACKNOWLEDGED << "))";
		
		//std::cerr << query.str () << std::endl;

		SQLiteResultSPtr result = DBModule::GetSQLite()->execQuery (query.str());
		
		while (result->next ())
		{
		    // std::cerr << "<<<< FULL LOAD " <<std::endl;
		    UpdateRecord* ur = new UpdateRecord ();
		    load (ur, result);
		    dest.insert (UpdateRecordSPtr (ur));
		}
	    }
	    
	    // Second part : partial load (excluded compressed SQL) of acknowledged/failed records that are pending on client side
	    {
		std::stringstream query;
		query << "SELECT " <<
		    TABLE_COL_ID << "," << TABLE_COL_TIMESTAMP << "," << TABLE_COL_EMITTERNODEID << "," << TABLE_COL_STATE
		      << " FROM " << TABLE.NAME << " WHERE (" 
		      << TABLE_COL_TIMESTAMP << " > " << Conversion::ToSQLiteString (to_iso_string (clientLastAcknowledgedTimestamp)) << ") AND (";

		query << "(" << TABLE_COL_ID << " IN (";
		for (std::set<uid>::const_iterator it = clientLastPendingIds.begin ();
		     it != clientLastPendingIds.end (); ++it)
		{
		    if (it != clientLastPendingIds.begin ()) query << ",";
		    query << "'" << Conversion::ToString (*it) << "'";
		}
		query << ")) AND " ;
		
		query << "(" << TABLE_COL_STATE << "=" << ACKNOWLEDGED << " OR " << TABLE_COL_STATE << "=" << FAILED << "))";
		//std::cerr << "..." << query.str () << std::endl;
		
		SQLiteResultSPtr result = DBModule::GetSQLite()->execQuery (query.str());
		
		while (result->next ())
		{
		    // std::cerr << "<<<< PARTIAL LOAD " <<std::endl;
		    UpdateRecord* ur = new UpdateRecord ();
		    load (ur, result);
		    dest.insert (UpdateRecordSPtr (ur));
		}
	    }
	    


	}



	void 
	UpdateRecordTableSync::LoadPendingRecordIds (std::set<uid>& updateRecordIds)
	{
	    std::stringstream query;
	    query << "SELECT " << TABLE_COL_ID << " FROM " << TABLE.NAME << " WHERE " << TABLE_COL_STATE << "=" << PENDING;

	    SQLiteResultSPtr result = DBModule::GetSQLite()->execQuery (query.str());
	    if (result->next ())
	    {
		updateRecordIds.insert (result->getLongLong (TABLE_COL_ID));
	    }
	}



	void 
	UpdateRecordTableSync::LoadPendingRecords (std::vector<UpdateRecordSPtr>& updateRecords, 
						   const boost::posix_time::ptime& lastAcknowledgedTimestamp,
						   bool lateOnes, bool withBlob)
	{
	    std::string selectClause (withBlob ? "*" : TABLE_COL_ID + "," + TABLE_COL_TIMESTAMP + 
				      "," + TABLE_COL_EMITTERNODEID + "," + TABLE_COL_STATE);

	    std::stringstream query;
	    query << "SELECT " << selectClause << " FROM " << TABLE.NAME << " WHERE " << TABLE_COL_STATE << "=" << PENDING << " AND " 
		  << TABLE_COL_TIMESTAMP << (lateOnes ? "<=" : ">") 
		  << Conversion::ToSQLiteString (to_iso_string (lastAcknowledgedTimestamp))
		// << "(SELECT MAX(" << TABLE_COL_TIMESTAMP << ") FROM " << TABLE.NAME << " WHERE " << TABLE_COL_STATE << "=" << ACKNOWLEDGED << ")"
		  << " ORDER BY " << TABLE_COL_TIMESTAMP;
	    
	    SQLiteResultSPtr result = DBModule::GetSQLite()->execQuery (query.str());
	    if (result->next ())
	    {
		UpdateRecord* ur = new UpdateRecord ();
		load (ur, result);
		updateRecords.push_back (UpdateRecordSPtr (ur));
	    }
	}





	
	void 
	UpdateRecordTableSync::ApplyUpdateRecord (const UpdateRecordSPtr& ur, bool overwriteTimestamp)
	{
	    //std::cerr << "......... Applying update record  " << std::endl;
	    UpdateRecordSPtr urp (ur);

	    if (ur->hasCompressedSQL () == false)
	    {
		// Retrieve the corresponding pending record and use it.
		urp = GetEditable (ur->getKey ());

		// If was already acnkwowledged, nothing to do.
		if (urp->getState () == ACKNOWLEDGED) return;
	    }

	    bool failed (false);
	    try
	    {
		std::stringstream compressedSQL;
		std::stringstream decompressedSQL;
		
		// Wrap the update in a unique transaction;
		// so that update record cannot be acknowledged only if it has executed successfully.
		// For this reason, usage of begin is strictly forbidden in an SQL update.
		// (nested transactions are not supported)

		// Note that update record timestamp is overriden with the real acknowledgement timestamp
		// which corresponds to execution order on authority.
		
		decompressedSQL << "BEGIN;" << std::endl;
		
		compressedSQL << urp->getCompressedSQL ();
		Compression::ZlibDecompress (compressedSQL, decompressedSQL);

		// Update state, and always override timestamp with one from record received from authority.
		decompressedSQL << "UPDATE " << TABLE.NAME << " SET " << TABLE_COL_STATE << "=" << ((int) ACKNOWLEDGED)
				<< "," << TABLE_COL_TIMESTAMP << "=" << Conversion::ToSQLiteString (to_iso_string (ur->getTimestamp ()));

		decompressedSQL  << " WHERE " << TABLE_COL_ID << "='" << urp->getKey () << "';";

		decompressedSQL << "END;" << std::endl;
		
		// batch execution, without precompilation!
		DBModule::GetSQLite()->execUpdate (decompressedSQL.str ());
		
		// Log::GetInstance ().info ("Executed : " + Conversion::ToTruncatedString (urp->getSQL ()));
	    }
	    catch (std::exception& e)
	    {
		failed = true;

		Log::GetInstance ().error ("Error while executing SQL statement ", e);
	    }
	    catch (...)
	    {
		failed = true;
	    }

	    if (failed)
	    {
		AbortUpdateRecord (ur, overwriteTimestamp);
	    }
	    
	    //std::cerr << "......... Applying update finished  " << std::endl;
	    
	}




	void 
	UpdateRecordTableSync::AbortUpdateRecord (const UpdateRecordSPtr& ur, bool overwriteTimestamp)
	{
	    std::stringstream query;
	    // Update state, and always override timestamp with one from record received from authority.
	    query << "UPDATE " << TABLE.NAME << " SET " << TABLE_COL_STATE << "=" << FAILED
		  << "," << TABLE_COL_TIMESTAMP << "=" << Conversion::ToSQLiteString (to_iso_string (ur->getTimestamp ()));

	    query  << " WHERE " << TABLE_COL_ID << "='" << ur->getKey () << "';";
	    DBModule::GetSQLite()->execUpdate (query.str ());

	}




	void 
	UpdateRecordTableSync::PostponeUpdateRecord (const UpdateRecordSPtr& ur)
	{
	    boost::posix_time::ptime now (boost::date_time::microsec_clock<ptime>::universal_time ());
	    std::stringstream query;
	    query << "UPDATE " << TABLE.NAME << " SET " << TABLE_COL_TIMESTAMP << "=" << Conversion::ToSQLiteString (to_iso_string (now));
	    query  << " WHERE " << TABLE_COL_ID << "='" << ur->getKey () << "';";
	    DBModule::GetSQLite()->execUpdate (query.str ());
	}




	boost::posix_time::ptime 
	UpdateRecordTableSync::GetLastTimestampWithState (const RecordState& recordState)
	{
	    // TODO : filter failed records!
	    std::stringstream query;

	    query << "SELECT MAX(" << TABLE_COL_TIMESTAMP << ") AS ts FROM " 
		  << TABLE.NAME << " WHERE " << TABLE_COL_STATE << "=" << recordState;

	    SQLiteResultSPtr result = DBModule::GetSQLite()->execQuery (query.str());
	    if (result->next () && (result->getText ("ts") != ""))
	    {
		return result->getTimestamp ("ts");
	    }
	    
	    return min_date_time;
	}




	long 
	UpdateRecordTableSync::GetLastUpdateIndex (NodeId nodeId)
	{
	    std::stringstream query;

	    query << "SELECT COUNT(*) AS lui FROM " << TABLE.NAME <<  " WHERE " 
		  << TABLE_COL_EMITTERNODEID << "=" << nodeId;
	    SQLiteResultSPtr result = DBModule::GetSQLite()->execQuery (query.str());
	    if (result->next () && (result->getText ("lui") != ""))
	    {
		return result->getLong ("lui");
	    }
	    return 0;
	}






/*
	void 
	UpdateRecordTableSync::SelectAllRecordIdsBetween (const boost::posix_time::ptime& startTimestamp,
							  const boost::posix_time::ptime& endTimestamp,
							  std::vector<uid>& result)
	{
	    std::stringstream query;
	    query << "SELECT " << TABLE_COL_ID << " FROM " << TABLE.NAME << " WHERE " 
		  << TABLE_COL_TIMESTAMP << " > " << Conversion::ToSQLiteString (to_iso_string (startTimestamp)) << " AND "
		  << TABLE_COL_TIMESTAMP << " < " << Conversion::ToSQLiteString (to_iso_string (endTimestamp)) ;
	    
	    SQLiteResultSPtr result = DBModule::GetSQLite()->execQuery (query.str());
	    
	    while (result->next ())
	    {
		result.push_back ((uid) result->getLongLong (TABLE_COL_ID));
	    }
	}
*/









}
}

