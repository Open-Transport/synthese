
/** NodeInfoTableSync class implementation.
	@file NodeInfoTableSync.cpp

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

#include "03_db_ring/NodeInfoTableSync.h"

#include "02_db/DBModule.h"

#include "03_db_ring/DbRingModule.h"
#include "03_db_ring/Node.h"

#include "01_util/Conversion.h"
#include "01_util/UId.h"

#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/date_time/posix_time/time_parsers.hpp>


#include <set>
#include <sstream>


using namespace boost::posix_time;


namespace synthese
{

    using namespace util;
    using namespace db;
    using namespace dbring;

	namespace util
	{
		template<> const std::string FactorableTemplate<SQLiteTableSync,NodeInfoTableSync>::FACTORY_KEY("0 Node infos");
	}

    namespace db
    {
	template<> const std::string SQLiteTableSyncTemplate<NodeInfoTableSync,NodeInfo>::TABLE_NAME = "t998_nodes_infos";
	template<> const int SQLiteTableSyncTemplate<NodeInfoTableSync,NodeInfo>::TABLE_ID = 998;
	template<> const bool SQLiteTableSyncTemplate<NodeInfoTableSync,NodeInfo>::HAS_AUTO_INCREMENT = true;

	template<> void SQLiteTableSyncTemplate<NodeInfoTableSync,NodeInfo>::load (NodeInfo* object, const db::SQLiteResultSPtr& rows)
	{
	    object->setNodeId (rows->getInt (NodeInfoTableSync::TABLE_COL_NODEID));
	    object->setRingId (rows->getInt (NodeInfoTableSync::TABLE_COL_RINGID));
	    object->setHost (rows->getText (NodeInfoTableSync::TABLE_COL_HOST));
	    object->setPort (rows->getInt (NodeInfoTableSync::TABLE_COL_PORT));
	    object->setAuthority (rows->getBool (NodeInfoTableSync::TABLE_COL_AUTH));
	    object->setState ((NodeState) rows->getInt (NodeInfoTableSync::TABLE_COL_STATE));
	    object->setClock (rows->getLongLong (NodeInfoTableSync::TABLE_COL_CLOCK));
	    object->setLastPendingTimestamp (rows->getTimestamp (NodeInfoTableSync::TABLE_COL_LASTPENDINGTIMESTAMP));
	    object->setLastAcknowledgedTimestamp (rows->getTimestamp (NodeInfoTableSync::TABLE_COL_LASTACKNOWLEDGEDTIMESTAMP));
	}

	template<> void SQLiteTableSyncTemplate<NodeInfoTableSync,NodeInfo>::_link(NodeInfo* obj, const SQLiteResultSPtr& rows, GetSource temporary)
	{

	}

	template<> void SQLiteTableSyncTemplate<NodeInfoTableSync,NodeInfo>::_unlink(NodeInfo* obj)
	{

	}

	template<> void SQLiteTableSyncTemplate<NodeInfoTableSync,NodeInfo>::save (NodeInfo* object)
	{
	    SQLite* sqlite = DBModule::GetSQLite();
	    std::stringstream query;

	    uid key = util::encodeUId (TABLE_ID, 
				 object->getRingId (),
				 object->getNodeId (),
				 object->getNodeId ());

	    query << "REPLACE INTO " << TABLE_NAME << " VALUES ("
		  << key << "," << object->getNodeId () << ","  << object->getRingId () << "," << Conversion::ToSQLiteString (object->getHost ()) << "," 
		  << object->getPort () << "," << object->isAuthority () << "," << object->getState () << "," << object->getClock () 
		  << "," << Conversion::ToSQLiteString (to_iso_string (object->getLastPendingTimestamp ()))    
		  << "," << Conversion::ToSQLiteString (to_iso_string (object->getLastAcknowledgedTimestamp ())) << ")";	    
	    
	    sqlite->execUpdate(query.str());
	}
    }


    namespace dbring
    {
	
	const std::string NodeInfoTableSync::TABLE_COL_NODEID ("nodeid");
	const std::string NodeInfoTableSync::TABLE_COL_RINGID ("ringid");
	const std::string NodeInfoTableSync::TABLE_COL_HOST ("host");
	const std::string NodeInfoTableSync::TABLE_COL_PORT ("port");
	const std::string NodeInfoTableSync::TABLE_COL_AUTH ("authority");
	const std::string NodeInfoTableSync::TABLE_COL_STATE ("state");
	const std::string NodeInfoTableSync::TABLE_COL_CLOCK ("clock");
	const std::string NodeInfoTableSync::TABLE_COL_LASTPENDINGTIMESTAMP ("last_pending_timestamp");
	const std::string NodeInfoTableSync::TABLE_COL_LASTACKNOWLEDGEDTIMESTAMP ("last_acknowledged_timestamp");


	NodeInfoTableSync::NodeInfoTableSync ()
	    : SQLiteTableSyncTemplate<NodeInfoTableSync,NodeInfo> ()
	{
	    // TODO : make host, port, auth updatables.
	    addTableColumn (TABLE_COL_ID, "INTEGER", false);
	    addTableColumn (TABLE_COL_NODEID, "INTEGER", false);
	    addTableColumn (TABLE_COL_RINGID, "INTEGER", false);
	    addTableColumn (TABLE_COL_HOST, "TEXT", false);
	    addTableColumn (TABLE_COL_PORT, "INTEGER", false);
	    addTableColumn (TABLE_COL_AUTH, "BOOLEAN", false);
	    addTableColumn (TABLE_COL_STATE, "INTEGER", true);
	    addTableColumn (TABLE_COL_CLOCK, "INTEGER", true);
	    addTableColumn (TABLE_COL_LASTPENDINGTIMESTAMP, "TEXT", true);
	    addTableColumn (TABLE_COL_LASTACKNOWLEDGEDTIMESTAMP, "TEXT", true);
	    
	    std::vector<std::string> indexColumns;
	    indexColumns.push_back (TABLE_COL_NODEID);
	    indexColumns.push_back (TABLE_COL_RINGID);
	    addTableIndex(indexColumns);
	    addTableIndex(TABLE_COL_LASTPENDINGTIMESTAMP);
	    addTableIndex(TABLE_COL_LASTACKNOWLEDGEDTIMESTAMP);
	    
	}
	
	
	
	
	NodeInfoTableSync::~NodeInfoTableSync ()
	{
	}
    


	void 
	NodeInfoTableSync::rowsAdded (SQLite* sqlite, 
				      SQLiteSync* sync,
				      const SQLiteResultSPtr& rows, bool isFirstSync)
	{
	    rowsUpdated (sqlite, sync, rows);
	}
	
	

	void 
	NodeInfoTableSync::rowsUpdated (SQLite* sqlite, 
					SQLiteSync* sync,
					const SQLiteResultSPtr& rows)
	{
	    NodeInfo info;
	    std::set<int> processedRows;

	    // Check if there is info concerning this node. If there is, 
	    // process it first.
	    int i = 0;
	    while (rows->next ())
	    {
		int nodeId = rows->getInt (NodeInfoTableSync::TABLE_COL_NODEID);
		if (nodeId == DbRingModule::GetNode ()->getId ())
		{
		    load (&info, rows);
		    DbRingModule::GetNode ()->setNodeInfoCallback (info);
		    processedRows.insert (i);
		}
		++i;
	    }

	    i = 0;
	    rows->reset ();
	    while (rows->next ())
	    {
		if (processedRows.find (i) == processedRows.end ())
		{
		    load (&info, rows);
		    DbRingModule::GetNode ()->setNodeInfoCallback (info);
		}
		++i;
	    }
	}
 

	void 
	NodeInfoTableSync::rowsRemoved (SQLite* sqlite, 
					SQLiteSync* sync,
					const SQLiteResultSPtr& rows)
	{

	}



}
}

