
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

#include "11_interfaces/InterfaceModule.h"

#include "34_departures_table/DisplayType.h"
#include "34_departures_table/DisplayTypeTableSync.h"
#include "34_departures_table/DeparturesTableModule.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace db;
	using namespace departurestable;
	using namespace interfaces;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<DisplayType>::TABLE_NAME = "t036_display_types";
		template<> const int SQLiteTableSyncTemplate<DisplayType>::TABLE_ID = 36;
		template<> const bool SQLiteTableSyncTemplate<DisplayType>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<DisplayType>::load(DisplayType* object, const db::SQLiteResult& rows, int rowId/*=0*/ )
		{
			object->setKey(Conversion::ToLongLong(rows.getColumn(rowId, TABLE_COL_ID)));
			object->setName(rows.getColumn(rowId, DisplayTypeTableSync::TABLE_COL_NAME));

			if (InterfaceModule::getInterfaces().contains(Conversion::ToLongLong(rows.getColumn(rowId, DisplayTypeTableSync::TABLE_COL_INTERFACE_ID))))
			{
				object->setInterface(InterfaceModule::getInterfaces().get(Conversion::ToLongLong(rows.getColumn(rowId, DisplayTypeTableSync::TABLE_COL_INTERFACE_ID))));
			}

			object->setRowNumber(Conversion::ToInt(rows.getColumn(rowId, DisplayTypeTableSync::TABLE_COL_ROWS_NUMBER)));
		}

		template<> void SQLiteTableSyncTemplate<DisplayType>::save(DisplayType* object)
		{
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() > 0)
			{
				query
					<< "UPDATE " << TABLE_NAME << " SET "
					<< DisplayTypeTableSync::TABLE_COL_NAME << "=" << Conversion::ToSQLiteString(object->getName())
					<< "," << DisplayTypeTableSync::TABLE_COL_INTERFACE_ID << "="  << Conversion::ToString(object->getInterface()->getKey())
					<< "," << DisplayTypeTableSync::TABLE_COL_ROWS_NUMBER << "="  << Conversion::ToString(object->getRowNumber())
					<< " WHERE " << TABLE_COL_ID << "=" << Conversion::ToString(object->getKey());
			}
			else
			{
				object->setKey(getId());
                query
					<< " INSERT INTO " << TABLE_NAME << " VALUES("
					<< Conversion::ToString(object->getKey())
					<< "," << Conversion::ToSQLiteString(object->getName())
					<< "," << Conversion::ToString(object->getInterface()->getKey())
					<< "," << Conversion::ToString(object->getRowNumber())
					<< ")";
			}
			sqlite->execUpdate(query.str());
		}

	}

	namespace departurestable
	{
		const std::string DisplayTypeTableSync::TABLE_COL_NAME = "name";
		const std::string DisplayTypeTableSync::TABLE_COL_INTERFACE_ID = "interface_id";
		const std::string DisplayTypeTableSync::TABLE_COL_ROWS_NUMBER = "rows_number";

		/// @todo Other fields

		DisplayTypeTableSync::DisplayTypeTableSync()
			: SQLiteTableSyncTemplate<DisplayType>(true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(TABLE_COL_NAME, "TEXT", true);
			addTableColumn(TABLE_COL_INTERFACE_ID, "INTEGER", true);
			addTableColumn(TABLE_COL_ROWS_NUMBER, "INTEGER", true);
		}

		void DisplayTypeTableSync::rowsAdded(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows, bool isFirstSync)
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				shared_ptr<DisplayType> object(new DisplayType());
				load(object.get(), rows, i);
				DeparturesTableModule::getDisplayTypes().add(object);
			}
		}

		void DisplayTypeTableSync::rowsUpdated(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows)
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				if (DeparturesTableModule::getDisplayTypes().contains(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID))))
				{
					load(DeparturesTableModule::getDisplayTypes().getUpdateable(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID))).get(), rows, i);
				}
			}
		}

		void DisplayTypeTableSync::rowsRemoved( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				if (DeparturesTableModule::getDisplayTypes().contains(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID))))
				{
					DeparturesTableModule::getDisplayTypes().remove(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID)));
				}
			}
		}

		std::vector<shared_ptr<DisplayType> > DisplayTypeTableSync::search(
			std::string exactName
			, int first /*= 0*/
			, int number /*= 0*/
			, bool orderByName
			, bool raisingOrder
		){
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
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
				SQLiteResult result = sqlite->execQuery(query.str());
				vector<shared_ptr<DisplayType> > objects;
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					shared_ptr<DisplayType> object(new DisplayType);
					load(object.get(), result, i);
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
