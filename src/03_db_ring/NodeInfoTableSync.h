
/** NodeInfoTableSync class header.
	@file NodeInfoTableSync.h

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


#ifndef SYNTHESE_DBRING_NODEINFOTABLESYNC_H
#define SYNTHESE_DBRING_NODEINFOTABLESYNC_H


#include "02_db/SQLiteDirectTableSyncTemplate.h"
#include "03_db_ring/NodeInfo.h"


namespace synthese
{

namespace dbring
{



/** .
	@ingroup m101
*/
class NodeInfoTableSync : public db::SQLiteDirectTableSyncTemplate<NodeInfoTableSync,NodeInfo>
{


public:

    static const std::string TABLE_COL_NODEID;
    static const std::string TABLE_COL_RINGID;
    static const std::string TABLE_COL_HOST;
    static const std::string TABLE_COL_PORT;

    NodeInfoTableSync ();
    ~NodeInfoTableSync ();


    virtual void rowsAdded (db::SQLite* sqlite, 
			    db::SQLiteSync* sync,
			    const db::SQLiteResultSPtr& rows);

    virtual void rowsUpdated (db::SQLite* sqlite, 
			      db::SQLiteSync* sync, 
			      const db::SQLiteResultSPtr& rows);

    virtual void rowsRemoved (db::SQLite* sqlite, 
			      db::SQLiteSync* sync,
			      const db::SQLiteResultSPtr& rows);


};



}
}



#endif



