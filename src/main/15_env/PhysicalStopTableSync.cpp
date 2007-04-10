
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
		template<> void SQLiteTableSyncTemplate<PhysicalStop>::load(PhysicalStop* object, const db::SQLiteResult& rows, int rowId/*=0*/ )
		{
			object->setKey(Conversion::ToLongLong(rows.getColumn(rowId, TABLE_COL_ID)));
			object->setName(rows.getColumn(rowId, PhysicalStopTableSync::COL_NAME));
			object->setX (Conversion::ToDouble (rows.getColumn (rowId, PhysicalStopTableSync::COL_X)));
			object->setY (Conversion::ToDouble (rows.getColumn (rowId, PhysicalStopTableSync::COL_Y)));
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



		PhysicalStopTableSync::PhysicalStopTableSync ()
		: SQLiteTableSyncTemplate<PhysicalStop>(TABLE_NAME, true, false, db::TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn (TABLE_COL_ID, "INTEGER", true);
			addTableColumn (COL_NAME, "TEXT", true);
			addTableColumn (COL_PLACEID, "INTEGER", false);
			addTableColumn (COL_X, "DOUBLE", true);
			addTableColumn (COL_Y, "DOUBLE", true);
		}



		PhysicalStopTableSync::~PhysicalStopTableSync ()
		{

		}

		    


		void PhysicalStopTableSync::rowsAdded(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows, bool isFirstSync)
		{
			for (int rowIndex=0; rowIndex<rows.getNbRows(); ++rowIndex)
			{

				uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));

				if (EnvModule::getPhysicalStops ().contains (id)) return;

				shared_ptr<ConnectionPlace> place = EnvModule::getConnectionPlaces().getUpdateable(Conversion::ToLongLong(rows.getColumn(rowIndex, COL_PLACEID)));
			    
				shared_ptr<PhysicalStop> ps(
						new PhysicalStop (
									id,
									rows.getColumn (rowIndex, COL_NAME),
									place.get(),
									Conversion::ToDouble (rows.getColumn (rowIndex, COL_X)),
									Conversion::ToDouble (rows.getColumn (rowIndex, COL_Y))
								))
				;

				place->addPhysicalStop(ps.get());

				EnvModule::getPhysicalStops ().add (ps);
			}
		}



		void PhysicalStopTableSync::rowsUpdated(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows)
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				uid id = Conversion::ToLongLong (rows.getColumn (i, TABLE_COL_ID));
				shared_ptr<PhysicalStop> ps = EnvModule::getPhysicalStops ().getUpdateable(id);
				load(ps.get(), rows, i);
			}
		}



		void PhysicalStopTableSync::rowsRemoved( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				uid id = Conversion::ToLongLong (rows.getColumn (i, TABLE_COL_ID));
				EnvModule::getPhysicalStops ().remove (id);
				/// @todo Handle the link between place and stop
			}
		}
	}
}
