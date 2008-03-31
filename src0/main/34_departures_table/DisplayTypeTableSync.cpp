
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

#include "DisplayTypeTableSync.h"

#include <sstream>

#include "01_util/Conversion.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLite.h"
#include "02_db/SQLiteException.h"

#include "11_interfaces/InterfaceTableSync.h"

#include "01_util/Conversion.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace departurestable;
	using namespace interfaces;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,DisplayTypeTableSync>::FACTORY_KEY("34.00 Display Types");
	}

	namespace db
	{
		template<> const string SQLiteTableSyncTemplate<DisplayTypeTableSync>::TABLE_NAME = "t036_display_types";
		template<> const int SQLiteTableSyncTemplate<DisplayTypeTableSync>::TABLE_ID = 36;
		template<> const bool SQLiteTableSyncTemplate<DisplayTypeTableSync>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteDirectTableSyncTemplate<DisplayTypeTableSync,DisplayType>::load(DisplayType* object, const db::SQLiteResultSPtr& rows )
		{
			object->setKey(rows->getLongLong (TABLE_COL_ID));
			object->setName(rows->getText ( DisplayTypeTableSync::TABLE_COL_NAME));
			object->setRowNumber(rows->getInt ( DisplayTypeTableSync::TABLE_COL_ROWS_NUMBER));
			object->setMaxStopsNumber(rows->getInt ( DisplayTypeTableSync::COL_MAX_STOPS_NUMBER));
		}


		template<> void SQLiteDirectTableSyncTemplate<DisplayTypeTableSync,DisplayType>::_link(DisplayType* object, const db::SQLiteResultSPtr& rows, GetSource temporary)
		{
			uid id(rows->getLongLong ( DisplayTypeTableSync::TABLE_COL_INTERFACE_ID));

			if (Interface::Contains(id))
				object->setInterface(InterfaceTableSync::Get(id, object, false, GET_AUTO));
		}


		template<> void SQLiteDirectTableSyncTemplate<DisplayTypeTableSync,DisplayType>::_unlink(DisplayType* obj)
		{
			obj->setInterface(NULL);
		}

    

		template<> void SQLiteDirectTableSyncTemplate<DisplayTypeTableSync,DisplayType>::save(DisplayType* object)
		{
			SQLite* sqlite = DBModule::GetSQLite();
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
			: SQLiteRegistryTableSyncTemplate<DisplayTypeTableSync,DisplayType>()
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(TABLE_COL_NAME, "TEXT", true);
			addTableColumn(TABLE_COL_INTERFACE_ID, "INTEGER", true);
			addTableColumn(TABLE_COL_ROWS_NUMBER, "INTEGER", true);
			addTableColumn(COL_MAX_STOPS_NUMBER, "INTEGER", true);
		}

		vector<shared_ptr<DisplayType> > DisplayTypeTableSync::search(
			string exactName
			, int first /*= 0*/
			, int number /*= 0*/
			, bool orderByName
			, bool raisingOrder
		){
			SQLite* sqlite = DBModule::GetSQLite();
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
					link(object.get(), rows, GET_TEMPORARY);
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
