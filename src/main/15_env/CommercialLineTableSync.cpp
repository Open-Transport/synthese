
/** CommercialLineTableSync class implementation.
	@file CommercialLineTableSync.cpp

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

#include "15_env/CommercialLine.h"
#include "15_env/CommercialLineTableSync.h"
#include "15_env/EnvModule.h"

using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace env;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<CommercialLine>::TABLE_NAME = "t042_commercial_lines";
		template<> const int SQLiteTableSyncTemplate<CommercialLine>::TABLE_ID = 42;
		template<> const bool SQLiteTableSyncTemplate<CommercialLine>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<CommercialLine>::load(CommercialLine* object, const db::SQLiteResult& rows, int rowId/*=0*/ )
		{
			object->setKey(Conversion::ToLongLong(rows.getColumn(rowId, TABLE_COL_ID)));
			object->setName(rows.getColumn(rowId, CommercialLineTableSync::COL_NAME));
			object->setShortName(rows.getColumn(rowId, CommercialLineTableSync::COL_SHORT_NAME));
			object->setLongName(rows.getColumn(rowId, CommercialLineTableSync::COL_LONG_NAME));
			object->setColor(RGBColor(rows.getColumn(rowId, CommercialLineTableSync::COL_COLOR)));
			object->setStyle(rows.getColumn(rowId, CommercialLineTableSync::COL_STYLE));
			object->setImage(rows.getColumn(rowId, CommercialLineTableSync::COL_IMAGE));
		}

		template<> void SQLiteTableSyncTemplate<CommercialLine>::save(CommercialLine* object)
		{
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() > 0)
			{
				query
					<< "UPDATE " << TABLE_NAME << " SET "
					/// @todo fill fields [,]FIELD=VALUE
					<< " WHERE " << TABLE_COL_ID << "=" << Conversion::ToString(object->getKey());
			}
			else
			{
				object->setKey(getId(1,1));
                query
					<< " INSERT INTO " << TABLE_NAME << " VALUES("
					<< Conversion::ToString(object->getKey())
					/// @todo fill other fields separated by ,
					<< ")";
			}
			sqlite->execUpdate(query.str());
		}

	}

	namespace env
	{
		const std::string CommercialLineTableSync::COL_NETWORK_ID ("network_id");
		const std::string CommercialLineTableSync::COL_NAME ("name");
		const std::string CommercialLineTableSync::COL_SHORT_NAME ("short_name");
		const std::string CommercialLineTableSync::COL_LONG_NAME ("long_name");
		const std::string CommercialLineTableSync::COL_COLOR ("color");
		const std::string CommercialLineTableSync::COL_STYLE ("style");
		const std::string CommercialLineTableSync::COL_IMAGE ("image");

		CommercialLineTableSync::CommercialLineTableSync()
			: SQLiteTableSyncTemplate<CommercialLine>(TABLE_NAME, true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(COL_NETWORK_ID, "INTEGER", false);
			addTableColumn(COL_NAME, "TEXT", false);
			addTableColumn(COL_SHORT_NAME, "TEXT", false);
			addTableColumn(COL_LONG_NAME, "TEXT", false);
			addTableColumn(COL_COLOR, "TEXT", false);
			addTableColumn(COL_STYLE, "TEXT", false);
			addTableColumn(COL_IMAGE, "TEXT", false);
		}

		void CommercialLineTableSync::rowsAdded(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows)
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				CommercialLine* object = new CommercialLine();
				load(object, rows, i);
				EnvModule::getCommercialLines().add(object);
			}
		}

		void CommercialLineTableSync::rowsUpdated(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows)
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				CommercialLine* object=EnvModule::getCommercialLines().get(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID)));
				load(object, rows, i);
			}
		}

		void CommercialLineTableSync::rowsRemoved( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				EnvModule::getCommercialLines().remove(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID)));
			}
		}

		std::vector<CommercialLine*> CommercialLineTableSync::search(int first /*= 0*/, int number /*= 0*/ )
		{
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE " 
				/// @todo Fill Where criteria
				// eg << TABLE_COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(name, false) << "%'"
				;
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteResult result = sqlite->execQuery(query.str());
				vector<CommercialLine*> objects;
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					CommercialLine* object = new CommercialLine();
					load(object, result, i);
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
