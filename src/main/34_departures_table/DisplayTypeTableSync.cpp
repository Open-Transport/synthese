
/** DisplayTypeTableSync class implementation.
	@file DisplayTypeTableSync.cpp

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

#include <sstream>

#include "01_util/Conversion.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"
#include "02_db/SQLiteException.h"

#include "11_interfaces/Interface.h"

#include "34_departures_table/DisplayType.h"
#include "34_departures_table/DisplayTypeTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace departurestable;
	using namespace interfaces;

	namespace db
	{
		template<> const string SQLiteTableSyncTemplate<DisplayType>::TABLE_NAME = "t036_display_types";
		template<> const int SQLiteTableSyncTemplate<DisplayType>::TABLE_ID = 36;
		template<> const bool SQLiteTableSyncTemplate<DisplayType>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<DisplayType>::load(DisplayType* object, const db::SQLiteResultSPtr& rows )
		{
			object->setKey(rows->getLongLong (TABLE_COL_ID));
			object->setName(rows->getText ( DisplayTypeTableSync::TABLE_COL_NAME));

			if (Interface::Contains(rows->getLongLong ( DisplayTypeTableSync::TABLE_COL_INTERFACE_ID)))
			{
			    object->setInterface(Interface::Get(
						     rows->getLongLong ( DisplayTypeTableSync::TABLE_COL_INTERFACE_ID)));
			}

	    object->setRowNumber(rows->getInt ( DisplayTypeTableSync::TABLE_COL_ROWS_NUMBER));
		object->setMaxStopsNumber(rows->getInt ( DisplayTypeTableSync::COL_MAX_STOPS_NUMBER));
	}
    

		template<> void SQLiteTableSyncTemplate<DisplayType>::save(DisplayType* object)
		{
			SQLiteHandle* sqlite = DBModule::GetSQLite();
			if (object->getKey() <= 0)
				object->setKey(getId());
			stringstream query;
			query
				<< "REPLACE INTO " << TABLE_NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				<< "," << Conversion::ToSQLiteString(object->getName())
				<< "," << Conversion::ToString(object->getInterface()->getKey())
				<< "," << Conversion::ToString(object->getRowNumber())
				<< "," << Conversion::ToString(object->getMaxStopsNumber())
				<< ")";
			sqlite->execUpdate(query.str());
		}

	}

	namespace departurestable
	{
		const string DisplayTypeTableSync::TABLE_COL_NAME = "name";
		const string DisplayTypeTableSync::TABLE_COL_INTERFACE_ID = "interface_id";
		const string DisplayTypeTableSync::TABLE_COL_ROWS_NUMBER = "rows_number";
		const string DisplayTypeTableSync::COL_MAX_STOPS_NUMBER("max_stops_number");

		/// @todo Other fields

		DisplayTypeTableSync::DisplayTypeTableSync()
			: SQLiteTableSyncTemplate<DisplayType>(true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(TABLE_COL_NAME, "TEXT", true);
			addTableColumn(TABLE_COL_INTERFACE_ID, "INTEGER", true);
			addTableColumn(TABLE_COL_ROWS_NUMBER, "INTEGER", true);
			addTableColumn(COL_MAX_STOPS_NUMBER, "INTEGER", true);
		}

		void DisplayTypeTableSync::rowsAdded(db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows, bool isFirstSync)
		{
			while (rows->next ())
			{
				if (DisplayType::Contains(rows->getLongLong (TABLE_COL_ID)))
				{
					load(DisplayType::GetUpdateable(rows->getLongLong (TABLE_COL_ID)).get(), rows);
				} else {
					DisplayType* object(new DisplayType());
					load(object, rows);
					object->store();
				}
			}
		}

		void DisplayTypeTableSync::rowsUpdated(db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows)
		{
			while (rows->next ())
			{
				if (DisplayType::Contains(rows->getLongLong (TABLE_COL_ID)))
				{
					load(DisplayType::GetUpdateable(rows->getLongLong (TABLE_COL_ID)).get(), rows);
				}
			}
		}

		void DisplayTypeTableSync::rowsRemoved( db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows )
		{
			while (rows->next ())
			{
				if (DisplayType::Contains(rows->getLongLong (TABLE_COL_ID)))
				{
					DisplayType::Remove(rows->getLongLong (TABLE_COL_ID));
				}
			}
		}

		vector<shared_ptr<DisplayType> > DisplayTypeTableSync::search(
			string exactName
			, int first /*= 0*/
			, int number /*= 0*/
			, bool orderByName
			, bool raisingOrder
		){
			SQLiteHandle* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE 1";
			if (!exactName.empty())
				query << " AND " << TABLE_COL_NAME << "=" << Conversion::ToSQLiteString(exactName);
			if (orderByName)
				query << " ORDER BY " << TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteResultSPtr rows = sqlite->execQuery(query.str());
				vector<shared_ptr<DisplayType> > objects;
				while (rows->next ())
				{
					shared_ptr<DisplayType> object(new DisplayType);
					load(object.get(), rows);
					objects.push_back(object);
				}
				return objects;
			}
			catch(SQLiteException& e)
			{
				throw Exception(e.getMessage());
			}
		}
	}
}
