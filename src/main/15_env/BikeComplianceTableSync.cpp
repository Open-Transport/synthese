
/** BikeComplianceTableSync class implementation.
	@file BikeComplianceTableSync.cpp

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
#include "02_db/SQLite.h"
#include "02_db/SQLiteException.h"

#include "15_env/BikeCompliance.h"
#include "15_env/BikeComplianceTableSync.h"

using namespace std;
using boost::logic::tribool;
using boost::shared_ptr;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace env;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<BikeCompliance>::TABLE_NAME = "t020_bike_compliances";
		template<> const int SQLiteTableSyncTemplate<BikeCompliance>::TABLE_ID = 20;
		template<> const bool SQLiteTableSyncTemplate<BikeCompliance>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<BikeCompliance>::load(BikeCompliance* cmp, const db::SQLiteResultSPtr& rows )
		{
		    cmp->setKey(rows->getLongLong (TABLE_COL_ID));

			tribool status = true;
			int statusInt (rows->getInt (BikeComplianceTableSync::COL_STATUS));
			if (statusInt < 0)
			{
				status = boost::logic::indeterminate;
			}
			else if (statusInt == 0)
			{
				status = false;
			}

			int capacity (rows->getInt (BikeComplianceTableSync::COL_CAPACITY));
			
			cmp->setCompliant (status);
			cmp->setCapacity (capacity);
		}


		template<> void SQLiteTableSyncTemplate<BikeCompliance>::save(BikeCompliance* object)
		{
			SQLite* sqlite = DBModule::GetSQLite();
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
		const std::string BikeComplianceTableSync::COL_STATUS ("status");
		const std::string BikeComplianceTableSync::COL_CAPACITY ("capacity");

		BikeComplianceTableSync::BikeComplianceTableSync()
			: SQLiteTableSyncTemplate<BikeCompliance>(true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn (COL_STATUS, "INTEGER");
			addTableColumn (COL_CAPACITY, "INTEGER");
		}

		void BikeComplianceTableSync::rowsAdded(db::SQLite* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows, bool isFirstSync)
		{
			while (rows->next ())
			{
				BikeCompliance* object(new BikeCompliance());
				load(object, rows);
				object->store();
			}
		}


		void BikeComplianceTableSync::rowsUpdated(db::SQLite* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows)
		{
			while (rows->next ())
			{
				uid id = rows->getLongLong (TABLE_COL_ID);
				if (BikeCompliance::Contains(id))
				{
					shared_ptr<BikeCompliance> object = BikeCompliance::GetUpdateable(id);
					load(object.get(), rows);
					object->store();
				}
			}
		}

		void BikeComplianceTableSync::rowsRemoved( db::SQLite* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows )
		{
			while (rows->next ())
			{
				uid id = rows->getLongLong (TABLE_COL_ID);
				if (BikeCompliance::Contains(id))
				{
					BikeCompliance::Remove(id);
				}
			}
		}

		std::vector<shared_ptr<BikeCompliance> > BikeComplianceTableSync::search(int first /*= 0*/, int number /*= 0*/ )
		{
			SQLite* sqlite = DBModule::GetSQLite();
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
				vector<shared_ptr<BikeCompliance> > objects;
				while (rows->next ())
				{
					shared_ptr<BikeCompliance> object(new BikeCompliance());
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

