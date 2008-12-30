
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

#include "Registry.h"
#include "Conversion.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLite.h"
#include "02_db/SQLiteException.h"

#include "15_env/ServiceDate.h"
#include "15_env/ServiceDateTableSync.h"
#include "15_env/NonPermanentService.h"
#include "15_env/Path.h"
#include "15_env/EnvModule.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace env;
	using namespace time;

	template<> const string util::FactorableTemplate<SQLiteTableSync,env::ServiceDateTableSync>::FACTORY_KEY("15.70.01 Service dates");

	namespace env
	{
		const string ServiceDateTableSync::COL_SERVICEID ("service_id");
		const string ServiceDateTableSync::COL_DATE("date");
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<ServiceDateTableSync>::TABLE(
			"t005_service_dates"
			);

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<ServiceDateTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(ServiceDateTableSync::COL_SERVICEID, SQL_INTEGER, false),
			SQLiteTableSync::Field(ServiceDateTableSync::COL_DATE, SQL_DATE, false),
			SQLiteTableSync::Field()
		};

		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<ServiceDateTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index(ServiceDateTableSync::COL_SERVICEID.c_str(), ServiceDateTableSync::COL_DATE.c_str(), ""),
			SQLiteTableSync::Index()
		};
	}

	namespace env
	{
		ServiceDateTableSync::ServiceDateTableSync()
			: SQLiteTableSyncTemplate<ServiceDateTableSync>()
		{
		}

		void ServiceDateTableSync::rowsAdded(db::SQLite* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows, bool isFirstSync)
		{
		    while (rows->next ())
		    {
			_updateServiceCalendar (rows, true);
		    }
		}
		
		void ServiceDateTableSync::rowsUpdated(db::SQLite* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows)
		{
		}

		void ServiceDateTableSync::rowsRemoved( db::SQLite* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows )
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

			shared_ptr<NonPermanentService> service (EnvModule::FetchEditableService(serviceId));
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

		void ServiceDateTableSync::Save( env::NonPermanentService* service )
		{
//////////////////////////////////////////////////////////////////////////
			/// @todo Loop on each date of the service

/*			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->key <= 0)
				object->key = getId();	

			query
				<< " REPLACE INTO " << TABLE.NAME << " VALUES("
				<< Conversion::ToString(object->key)
				<< "," << Conversion::ToString(object->service->getId())
				<< "," << object->date.toSQLString()
				<< ")";
			sqlite->execUpdate(query.str());
*/
		}



		vector<time::Date> ServiceDateTableSync::GetDatesOfService( uid serviceId )
		{
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;

			query << "SELECT " << COL_DATE << " FROM " << TABLE.NAME << " WHERE " << COL_SERVICEID << "=" << serviceId << " ORDER BY " << COL_DATE;
			
			try
			{
				SQLiteResultSPtr rows = DBModule::GetSQLite()->execQuery(query.str());
				vector<Date> objects;
				while (rows->next ())
				{
					objects.push_back(Date::FromSQLDate(rows->getText(COL_DATE)));
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
