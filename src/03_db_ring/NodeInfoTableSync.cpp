
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
	template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<NodeInfoTableSync>::TABLE.NAME = "t998_nodes_infos";
	template<> const int SQLiteTableSyncTemplate<NodeInfoTableSync>::TABLE.ID = 998;
	template<> const bool SQLiteTableSyncTemplate<NodeInfoTableSync>::HAS_AUTO_INCREMENT = true;

	template<> void SQLiteDirectTableSyncTemplate<NodeInfoTableSync,NodeInfo>::load (NodeInfo* object, const db::SQLiteResultSPtr& rows)
	{
	    object->setNodeId (rows->getInt (NodeInfoTableSync::TABLE_COL_NODEID));
	    object->setRingId (rows->getInt (NodeInfoTableSync::TABLE_COL_RINGID));
	    object->setHost (rows->getText (NodeInfoTableSync::TABLE_COL_HOST));
	    object->setPort (rows->getInt (NodeInfoTableSync::TABLE_COL_PORT));
	}

	template<> void SQLiteDirectTableSyncTemplate<NodeInfoTableSync,NodeInfo>::_link(NodeInfo* obj, const SQLiteResultSPtr& rows, GetSource temporary)
	{

	}

	template<> void SQLiteDirectTableSyncTemplate<NodeInfoTableSync,NodeInfo>::_unlink(NodeInfo* obj)
	{

	}

	template<> void SQLiteDirectTableSyncTemplate<NodeInfoTableSync,NodeInfo>::save (NodeInfo* object)
	{
	    SQLite* sqlite = DBModule::GetSQLite();
	    std::stringstream query;

	    uid key = util::encodeUId (TABLE.ID, 
				 object->getRingId (),
				 object->getNodeId (),
				 object->getNodeId ());

	    query << "REPLACE INTO " << TABLE.NAME << " VALUES ("
		  << key << "," << object->getNodeId () << ","  << object->getRingId () << "," << Conversion::ToSQLiteString (object->getHost ()) << "," 
		  << object->getPort () << ")";	    
	    
	    sqlite->execUpdate(query.str());
	}
    }


    namespace dbring
    {
	
	const std::string NodeInfoTableSync::TABLE_COL_NODEID ("nodeid");
	const std::string NodeInfoTableSync::TABLE_COL_RINGID ("ringid");
	const std::string NodeInfoTableSync::TABLE_COL_HOST ("host");
	const std::string NodeInfoTableSync::TABLE_COL_PORT ("port");


	NodeInfoTableSync::NodeInfoTableSync ()
	    : SQLiteDirectTableSyncTemplate<NodeInfoTableSync,NodeInfo> ()
	{
	    // TODO : make host, port, updatables.
	    addTableColumn (TABLE_COL_ID, "SQL_INTEGER", false);
	    addTableColumn (TABLE_COL_NODEID, "SQL_INTEGER", false);
	    addTableColumn (TABLE_COL_RINGID, "SQL_INTEGER", false);
	    addTableColumn (TABLE_COL_HOST, "SQL_TEXT", false);
	    addTableColumn (TABLE_COL_PORT, "SQL_INTEGER", false);
	    
	    std::vector<std::string> indexColumns;
	    indexColumns.push_back (TABLE_COL_NODEID);
	    indexColumns.push_back (TABLE_COL_RINGID);
	    addTableIndex(indexColumns);
	    
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

