
/** AxisTableSync class implementation.
	@file AxisTableSync.cpp

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

#include "15_env/Axis.h"
#include "15_env/AxisTableSync.h"
#include "15_env/EnvModule.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace env;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<Axis>::TABLE_NAME = "t004_axes";
		template<> const int SQLiteTableSyncTemplate<Axis>::TABLE_ID = 4;
		template<> const bool SQLiteTableSyncTemplate<Axis>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<Axis>::load(Axis* axis, const db::SQLiteResultSPtr& rows)
		{
		    axis->setKey(rows->getLongLong (TABLE_COL_ID));
		    axis->setName (rows->getText (AxisTableSync::COL_NAME));
		    axis->setFree (rows->getBool (AxisTableSync::COL_FREE));
		    axis->setAllowed (rows->getBool (AxisTableSync::COL_ALLOWED));
		}
	    

		template<> void SQLiteTableSyncTemplate<Axis>::save(Axis* object)
		{
			SQLiteHandle* sqlite = DBModule::GetSQLite();
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
				object->setKey(getId());
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
		const std::string AxisTableSync::COL_NAME ("name");
		const std::string AxisTableSync::COL_FREE ("free");
		const std::string AxisTableSync::COL_ALLOWED ("allowed");


		AxisTableSync::AxisTableSync()
			: SQLiteTableSyncTemplate<Axis>(true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn (COL_NAME, "TEXT", true);
			addTableColumn (COL_FREE, "BOOLEAN", true);
			addTableColumn (COL_ALLOWED, "BOOLEAN", true);
		}

		void AxisTableSync::rowsAdded(db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows, bool isFirstSync)
		{
			while (rows->next ())
			{
				Axis* object(new Axis());
				load(object, rows);
				object->store();
			}
		}

		void AxisTableSync::rowsUpdated(db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows)
		{
			while (rows->next ())
			{
				uid lineId = rows->getLongLong (TABLE_COL_ID);
				if (Axis::Contains(lineId))
				{
					shared_ptr<Axis> object = Axis::GetUpdateable(lineId);
					load(object.get(), rows);
					object->store();
				}
			}
		}

		void AxisTableSync::rowsRemoved( db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows )
		{
			while (rows->next ())
			{
				uid lineId = rows->getLongLong (TABLE_COL_ID);
				if (Axis::Contains(lineId))
				{
					Axis::Remove(lineId);
				}
			}
		}

		std::vector<shared_ptr<Axis> > AxisTableSync::search(int first /*= 0*/, int number /*= 0*/ )
		{
			SQLiteHandle* sqlite = DBModule::GetSQLite();
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
				SQLiteResultSPtr rows = sqlite->execQuery(query.str());
				vector<shared_ptr<Axis> > objects;
				while (rows->next ())
				{
					shared_ptr<Axis> object(new Axis());
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

