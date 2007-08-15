
/** HandicappedComplianceTableSync class implementation.
	@file HandicappedComplianceTableSync.cpp

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

#include "15_env/EnvModule.h"
#include "HandicappedCompliance.h"
#include "HandicappedComplianceTableSync.h"

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
		template<> const std::string SQLiteTableSyncTemplate<HandicappedCompliance>::TABLE_NAME = "t019_handicapped_compliances";
		template<> const int SQLiteTableSyncTemplate<HandicappedCompliance>::TABLE_ID = 19;
		template<> const bool SQLiteTableSyncTemplate<HandicappedCompliance>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<HandicappedCompliance>::load(HandicappedCompliance* cmp, const db::SQLiteResultSPtr& rows )
		{
		    cmp->setKey(rows->getLongLong (TABLE_COL_ID));
		    
		    tribool status = true;
		    int statusInt (rows->getInt (HandicappedComplianceTableSync::COL_STATUS));

		    if (statusInt < 0)
		    {
			status = boost::logic::indeterminate;
		    }
		    else if (statusInt == 0)
		    {
			status = false;
		    }
		    
		    int capacity (
			rows->getInt (HandicappedComplianceTableSync::COL_CAPACITY));
		    
		    cmp->setCompliant (status);
		    cmp->setCapacity (capacity);
		}


		template<> void SQLiteTableSyncTemplate<HandicappedCompliance>::save(HandicappedCompliance* object)
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
		const std::string HandicappedComplianceTableSync::COL_STATUS ("status");
		const std::string HandicappedComplianceTableSync::COL_CAPACITY ("capacity");

		HandicappedComplianceTableSync::HandicappedComplianceTableSync()
			: SQLiteTableSyncTemplate<HandicappedCompliance>(true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn (COL_STATUS, "INTEGER");
			addTableColumn (COL_CAPACITY, "INTEGER");
		}

		void HandicappedComplianceTableSync::rowsAdded(db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows, bool isFirstSync)
		{
			while (rows->next ())
			{
				shared_ptr<HandicappedCompliance> object(new HandicappedCompliance());
				load(object.get(), rows);
				EnvModule::getHandicappedCompliances().add(object);
			}
		}

		void HandicappedComplianceTableSync::rowsUpdated(db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows)
		{
			while (rows->next ())
			{
				uid id = rows->getLongLong (TABLE_COL_ID);
				if (EnvModule::getHandicappedCompliances().contains(id))
				{
					shared_ptr<HandicappedCompliance> object = EnvModule::getHandicappedCompliances().getUpdateable(id);
					load(object.get(), rows);
				}
			}
		}

		void HandicappedComplianceTableSync::rowsRemoved( db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows )
		{
			while (rows->next ())
			{
				uid id = rows->getLongLong (TABLE_COL_ID);
				if (EnvModule::getHandicappedCompliances().contains(id))
				{
					EnvModule::getHandicappedCompliances().remove(id);
				}
			}
		}

		std::vector<shared_ptr<HandicappedCompliance> > HandicappedComplianceTableSync::search(int first /*= 0*/, int number /*= 0*/ )
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
				vector<shared_ptr<HandicappedCompliance> > objects;
				while (rows->next ())
				{
					shared_ptr<HandicappedCompliance> object(new HandicappedCompliance());
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

