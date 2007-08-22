
/** PedestrianComplianceTableSync class implementation.
	@file PedestrianComplianceTableSync.cpp

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

#include "PedestrianCompliance.h"
#include "PedestrianComplianceTableSync.h"

using namespace std;
using namespace boost;
using boost::logic::tribool;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace env;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<PedestrianCompliance>::TABLE_NAME = "t018_pedestrian_compliances";
		template<> const int SQLiteTableSyncTemplate<PedestrianCompliance>::TABLE_ID = 18;
		template<> const bool SQLiteTableSyncTemplate<PedestrianCompliance>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<PedestrianCompliance>::load(PedestrianCompliance* cmp, const db::SQLiteResultSPtr& rows )
		{
			cmp->setKey(rows->getLongLong (TABLE_COL_ID));

			tribool status = true;
			int statusInt (
			    rows->getInt (PedestrianComplianceTableSync::COL_STATUS));
			if (statusInt < 0)
			{
				status = boost::logic::indeterminate;
			}
			else if (statusInt == 0)
			{
				status = false;
			}

			int capacity (
				rows->getInt (PedestrianComplianceTableSync::COL_CAPACITY));

			cmp->setCompliant (status);
			cmp->setCapacity (capacity);
		}

		template<> void SQLiteTableSyncTemplate<PedestrianCompliance>::save(PedestrianCompliance* object)
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
		const std::string PedestrianComplianceTableSync::COL_STATUS ("status");
		const std::string PedestrianComplianceTableSync::COL_CAPACITY ("capacity");

		PedestrianComplianceTableSync::PedestrianComplianceTableSync()
			: SQLiteTableSyncTemplate<PedestrianCompliance>(true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn (COL_STATUS, "INTEGER");
			addTableColumn (COL_CAPACITY, "INTEGER");
		}

		void PedestrianComplianceTableSync::rowsAdded(db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows, bool isFirstSync)
		{
			while (rows->next ())
			{
				PedestrianCompliance* object(new PedestrianCompliance());
				load(object, rows);
				object->store();
			}
		}

		void PedestrianComplianceTableSync::rowsUpdated(db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows)
		{
			while (rows->next ())
			{
				uid id = rows->getLongLong (TABLE_COL_ID);
				if (PedestrianCompliance::Contains(id))
				{
					shared_ptr<PedestrianCompliance> object = PedestrianCompliance::GetUpdateable(id);
					load(object.get(), rows);
				}
			}
		}

		void PedestrianComplianceTableSync::rowsRemoved( db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows )
		{
			while (rows->next ())
			{
				uid id = rows->getLongLong (TABLE_COL_ID);
				if (PedestrianCompliance::Contains(id))
				{
					PedestrianCompliance::Remove(id);
				}
			}
		}

		std::vector<shared_ptr<PedestrianCompliance> > PedestrianComplianceTableSync::search(int first /*= 0*/, int number /*= 0*/ )
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
				vector<shared_ptr<PedestrianCompliance> > objects;
				while (rows->next ())
				{
					shared_ptr<PedestrianCompliance> object(new PedestrianCompliance());
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

