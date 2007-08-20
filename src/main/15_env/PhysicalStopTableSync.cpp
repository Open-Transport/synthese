
/** PhysicalStopTableSync class implementation.
    @file PhysicalStopTableSync.cpp

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

#include "PhysicalStopTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"

#include "15_env/EnvModule.h"
#include "15_env/PhysicalStop.h"
#include "15_env/PublicTransportStopZoneConnectionPlace.h"

#include <sqlite/sqlite3.h>
#include <assert.h>

using namespace std;
using namespace boost;

namespace synthese
{
    using namespace db;
    using namespace env;
    using namespace util;

    namespace db
    {
	template<> const std::string SQLiteTableSyncTemplate<PhysicalStop>::TABLE_NAME = "t012_physical_stops";
	template<> const int SQLiteTableSyncTemplate<PhysicalStop>::TABLE_ID = 12;
	template<> const bool SQLiteTableSyncTemplate<PhysicalStop>::HAS_AUTO_INCREMENT = true;

	/** Does not update the place */
	template<> void SQLiteTableSyncTemplate<PhysicalStop>::load(PhysicalStop* object, const db::SQLiteResultSPtr& rows )
	{
	    object->setKey(rows->getLongLong (TABLE_COL_ID));
	    object->setName(rows->getText ( PhysicalStopTableSync::COL_NAME));
	    object->setXY (rows->getDouble ( PhysicalStopTableSync::COL_X), rows->getDouble ( PhysicalStopTableSync::COL_Y));
	    object->setOperatorCode(rows->getText ( PhysicalStopTableSync::COL_OPERATOR_CODE));
	}

	template<> void SQLiteTableSyncTemplate<PhysicalStop>::save(PhysicalStop* object)
	{
	    /// @todo Implementation
	}
    }

    namespace env
    {
	const std::string PhysicalStopTableSync::COL_NAME = "name";
	const std::string PhysicalStopTableSync::COL_PLACEID = "place_id";
	const std::string PhysicalStopTableSync::COL_X = "x";
	const std::string PhysicalStopTableSync::COL_Y = "y";
	const string PhysicalStopTableSync::COL_OPERATOR_CODE("operator_code");



	PhysicalStopTableSync::PhysicalStopTableSync ()
	    : SQLiteTableSyncTemplate<PhysicalStop>(true, false, db::TRIGGERS_ENABLED_CLAUSE)
	{
	    addTableColumn (TABLE_COL_ID, "INTEGER", true);
	    addTableColumn (COL_NAME, "TEXT", true);
	    addTableColumn (COL_PLACEID, "INTEGER", false);
	    addTableColumn (COL_X, "DOUBLE", true);
	    addTableColumn (COL_Y, "DOUBLE", true);
	    addTableColumn(COL_OPERATOR_CODE, "TEXT", true);

	    addTableIndex(COL_PLACEID);
	}



	PhysicalStopTableSync::~PhysicalStopTableSync ()
	{

	}

		    


	void PhysicalStopTableSync::rowsAdded(db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows, bool isFirstSync)
	{
	    while (rows->next ())
	    {
			
		uid id = rows->getLongLong (TABLE_COL_ID);
			
		if (EnvModule::getPhysicalStops ().contains (id)) return;
			
		shared_ptr<PublicTransportStopZoneConnectionPlace> place = EnvModule::getPublicTransportStopZones().getUpdateable(rows->getLongLong (COL_PLACEID));
		    
	    shared_ptr<PhysicalStop> ps(
		new PhysicalStop (
		    id,
		    rows->getText (COL_NAME),
		    place.get(),
		    rows->getDouble (COL_X),
		    rows->getDouble (COL_Y)
		    ))
		;
		    
	    place->addPhysicalStop(ps.get());
	    ps->setOperatorCode(rows->getText (COL_OPERATOR_CODE));
		    
	    EnvModule::getPhysicalStops ().add (ps);
	}
    }



    void PhysicalStopTableSync::rowsUpdated(db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows)
    {
	while (rows->next ())
	{
	    uid id = rows->getLongLong ( TABLE_COL_ID);
	    shared_ptr<PhysicalStop> ps = EnvModule::getPhysicalStops ().getUpdateable(id);

	    load(ps.get(), rows);

	}
    }



    void PhysicalStopTableSync::rowsRemoved( db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows )
    {
	while (rows->next ())
	{
	    uid id = rows->getLongLong ( TABLE_COL_ID);
	    EnvModule::getPhysicalStops ().remove (id);
	    /// @todo Handle the link between place and stop
	}
    }
}
}
