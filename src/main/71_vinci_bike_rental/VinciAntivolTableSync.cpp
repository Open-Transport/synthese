
/** VinciAntivolTableSync class implementation.
	@file VinciAntivolTableSync.cpp

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

#include "VinciAntivol.h"
#include "VinciAntivolTableSync.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace vinci;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<VinciAntivol>::TABLE_NAME = "t043_vinci_locks";
		template<> const int SQLiteTableSyncTemplate<VinciAntivol>::TABLE_ID = 43;
		template<> const bool SQLiteTableSyncTemplate<VinciAntivol>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<VinciAntivol>::load(VinciAntivol* object, const db::SQLiteResult& rows, int rowId/*=0*/ )
		{
			object->setKey(Conversion::ToLongLong(rows.getColumn(rowId, TABLE_COL_ID)));
			object->setMarkedNumber(rows.getColumn(rowId, VinciAntivolTableSync::COL_MARKED_NUMBER));
		}

		template<> void SQLiteTableSyncTemplate<VinciAntivol>::save(VinciAntivol* object)
		{
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());	/// @todo Use grid ID
               
			 query
				<< " REPLACE INTO " << TABLE_NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				<< "," << Conversion::ToSQLiteString(object->getMarkedNumber())
				<< ")";
			sqlite->execUpdate(query.str());
		}

	}

	namespace vinci
	{
		const std::string VinciAntivolTableSync::COL_MARKED_NUMBER = "marked_number";

		VinciAntivolTableSync::VinciAntivolTableSync()
			: SQLiteTableSyncTemplate<VinciAntivol>(true, true, TRIGGERS_ENABLED_CLAUSE, true)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(COL_MARKED_NUMBER, "TEXT", true);
		}

		void VinciAntivolTableSync::rowsAdded(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows, bool isFirstSync)
		{
		}
		
		void VinciAntivolTableSync::rowsUpdated(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows)
		{
		}

		void VinciAntivolTableSync::rowsRemoved( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{
		}

		vector<shared_ptr<VinciAntivol> > VinciAntivolTableSync::search(const std::string markedNumber, int first /*= 0*/, int number /*= 0*/ )
		{
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE " 
				<< COL_MARKED_NUMBER << "=" << Conversion::ToSQLiteString(markedNumber)
				;
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteResult result = sqlite->execQuery(query.str());
				vector<shared_ptr<VinciAntivol> > objects;
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					shared_ptr<VinciAntivol> object(new VinciAntivol());
					load(object.get (), result, i);
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
