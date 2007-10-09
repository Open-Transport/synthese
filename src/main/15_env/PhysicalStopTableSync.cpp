
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
#include "02_db/SQLite.h"

#include "15_env/ConnectionPlaceTableSync.h"

#include <sqlite/sqlite3.h>
#include <assert.h>

using namespace std;
using namespace boost;

namespace synthese
{
    using namespace db;
    using namespace env;
    using namespace util;

	template<> const string util::FactorableTemplate<SQLiteTableSync,PhysicalStopTableSync>::FACTORY_KEY("15.55.01 Physical stops");

    namespace db
    {
		template<> const std::string SQLiteTableSyncTemplate<PhysicalStopTableSync,PhysicalStop>::TABLE_NAME = "t012_physical_stops";
		template<> const int SQLiteTableSyncTemplate<PhysicalStopTableSync,PhysicalStop>::TABLE_ID = 12;
		template<> const bool SQLiteTableSyncTemplate<PhysicalStopTableSync,PhysicalStop>::HAS_AUTO_INCREMENT = true;

		/** Does not update the place */
		template<> void SQLiteTableSyncTemplate<PhysicalStopTableSync,PhysicalStop>::load(PhysicalStop* object, const db::SQLiteResultSPtr& rows )
		{
			object->setKey(rows->getLongLong (TABLE_COL_ID));
			object->setName(rows->getText ( PhysicalStopTableSync::COL_NAME));
			object->setXY (rows->getDouble ( PhysicalStopTableSync::COL_X), rows->getDouble ( PhysicalStopTableSync::COL_Y));
			object->setOperatorCode(rows->getText ( PhysicalStopTableSync::COL_OPERATOR_CODE));
		}

		template<> void SQLiteTableSyncTemplate<PhysicalStopTableSync,PhysicalStop>::_link(PhysicalStop* obj, const SQLiteResultSPtr& rows, GetSource temporary)
		{
			PublicTransportStopZoneConnectionPlace* place = ConnectionPlaceTableSync::GetUpdateable(rows->getLongLong (PhysicalStopTableSync::COL_PLACEID), obj, temporary);
			obj->setPlace(place);

			place->addPhysicalStop(obj);
		}

		template<> void SQLiteTableSyncTemplate<PhysicalStopTableSync,PhysicalStop>::_unlink(PhysicalStop* obj)
		{
			PublicTransportStopZoneConnectionPlace* place = const_cast<PublicTransportStopZoneConnectionPlace*>(obj->getConnectionPlace());
/// @todo	place->removePhysicalStop(obj);

			obj->setPlace(NULL);
		}

		template<> void SQLiteTableSyncTemplate<PhysicalStopTableSync,PhysicalStop>::save(PhysicalStop* object)
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
			: SQLiteRegistryTableSyncTemplate<PhysicalStopTableSync,PhysicalStop>()
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

				/*


		void PhysicalStopTableSync::rowsAdded(db::SQLite* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows, bool isFirstSync)
		{
			while (rows->next ())
			{
				
			uid id = rows->getLongLong (TABLE_COL_ID);
				
			if (PhysicalStop::Contains (id)) return;
				
			    
			PhysicalStop* ps(
			new PhysicalStop (
				id,
				rows->getText (COL_NAME),
				rows->getDouble (COL_X),
				rows->getDouble (COL_Y)
				))
			;
			    
			place->addPhysicalStop(ps);
			ps->setOperatorCode(rows->getText (COL_OPERATOR_CODE));
			    
			ps->store();
		}
		}



		void PhysicalStopTableSync::rowsUpdated(db::SQLite* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows)
		{
		while (rows->next ())
		{
			uid id = rows->getLongLong ( TABLE_COL_ID);
			shared_ptr<PhysicalStop> ps = PhysicalStop::GetUpdateable(id);

			load(ps.get(), rows);

		}
		}



		void PhysicalStopTableSync::rowsRemoved( db::SQLite* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows )
		{
		while (rows->next ())
		{
			uid id = rows->getLongLong ( TABLE_COL_ID);
			PhysicalStop::Remove (id);
			/// @todo Handle the link between place and stop
		}
		}*/
	}
}
