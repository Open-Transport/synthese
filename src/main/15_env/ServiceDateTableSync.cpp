
/** ServiceDateTableSync class implementation.
	@file ServiceDateTableSync.cpp

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

#include "04_time/Date.h"

#include "15_env/ServiceDate.h"
#include "15_env/ServiceDateTableSync.h"
#include "15_env/EnvModule.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace env;
	using namespace time;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<ServiceDate>::TABLE_NAME = "t005_service_dates";
		template<> const int SQLiteTableSyncTemplate<ServiceDate>::TABLE_ID = 5;
		template<> const bool SQLiteTableSyncTemplate<ServiceDate>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<ServiceDate>::load(ServiceDate* object, const db::SQLiteResultSPtr& rows )
		{
			object->key = rows->getLongLong (TABLE_COL_ID);
			object->service = EnvModule::fetchService (rows->getLongLong ( ServiceDateTableSync::COL_SERVICEID)).get();
			object->date = Date::FromSQLDate (rows->getText ( ServiceDateTableSync::COL_DATE));
		}

		template<> void SQLiteTableSyncTemplate<ServiceDate>::save(ServiceDate* object)
		{
			SQLiteHandle* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->key <= 0)
				object->key = getId();	
               
			 query
				<< " REPLACE INTO " << TABLE_NAME << " VALUES("
				<< Conversion::ToString(object->key)
				<< "," << Conversion::ToString(object->service->getId())
				<< "," << object->date.toSQLString()
				<< ")";
			sqlite->execUpdate(query.str());
		}

	}

	namespace env
	{
		const std::string ServiceDateTableSync::COL_SERVICEID ("service_id");
		const std::string ServiceDateTableSync::COL_DATE("date");

		ServiceDateTableSync::ServiceDateTableSync()
			: SQLiteTableSyncTemplate<ServiceDate>(true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn (COL_SERVICEID, "INTEGER", false);
			addTableColumn (COL_DATE , "DATE", false);
		}

		void ServiceDateTableSync::rowsAdded(db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows, bool isFirstSync)
		{
		    while (rows->next ())
		    {
			_updateServiceCalendar (rows, true);
		    }
		}
		
		void ServiceDateTableSync::rowsUpdated(db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows)
		{
		}

		void ServiceDateTableSync::rowsRemoved( db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows )
		{
			while (rows->next ())
			{
				_updateServiceCalendar (rows, false);
			}
		}


		void ServiceDateTableSync::_updateServiceCalendar (const SQLiteResultSPtr& rows, bool marked) 
		{
			// Get the corresponding calendar
			uid serviceId = rows->getLongLong (COL_SERVICEID);

			shared_ptr<NonPermanentService> service (EnvModule::fetchService (serviceId));
			assert (service != 0);

			// Mark the date in service calendar
			Date newDate = Date::FromSQLDate (rows->getText (COL_DATE));
			service->getCalendar ().mark (newDate, marked);
			
			if (marked)
			{
			    for (int i = service->getDepartureSchedule().getDaysSinceDeparture(); 
				 i<= service->getLastArrivalSchedule().getDaysSinceDeparture(); ++i)
			    {
				service->getPath()->getCalendar().mark(newDate, marked);
				newDate++;
			    }
			}
			else
			{
			    /// @todo Implement it : 
                            // see in each date if there is at least an other service which runs. If not unmark the date and see the following one.
			}

			//environment.updateMinMaxDatesInUse (newDate, marked);

		}
	}
}
