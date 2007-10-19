
/** ContinuousServiceTableSync class implementation.
	@file ContinuousServiceTableSync.cpp

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

#include "ContinuousServiceTableSync.h"

#include "15_env/Path.h"
#include "15_env/LineTableSync.h"
#include "15_env/BikeComplianceTableSync.h"
#include "15_env/PedestrianComplianceTableSync.h"
#include "15_env/HandicappedComplianceTableSync.h"
#include "15_env/EnvModule.h"

#include <sstream>

#include <boost/tokenizer.hpp>

#include "01_util/Conversion.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLite.h"
#include "02_db/SQLiteException.h"

#include "04_time/Schedule.h"

#include "06_geometry/Point2D.h"

#include <set>
#include <boost/algorithm/string.hpp>


using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace env;
	using namespace time;

	template<> const string util::FactorableTemplate<SQLiteTableSync,ContinuousServiceTableSync>::FACTORY_KEY("15.60.02 Continuous services");

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<ContinuousServiceTableSync,ContinuousService>::TABLE_NAME = "t017_continuous_services";
		template<> const int SQLiteTableSyncTemplate<ContinuousServiceTableSync,ContinuousService>::TABLE_ID = 17;
		template<> const bool SQLiteTableSyncTemplate<ContinuousServiceTableSync,ContinuousService>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<ContinuousServiceTableSync,ContinuousService>::load(ContinuousService* cs, const db::SQLiteResultSPtr& rows )
		{
		    uid id (rows->getLongLong (TABLE_COL_ID));

		    int serviceNumber (rows->getInt (ContinuousServiceTableSync::COL_SERVICENUMBER));
		    int range (rows->getInt (ContinuousServiceTableSync::COL_RANGE));
		    int maxWaitingTime (rows->getInt (ContinuousServiceTableSync::COL_MAXWAITINGTIME));

		    std::string schedules (
			rows->getText (ContinuousServiceTableSync::COL_SCHEDULES));

		    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
		    
		    // Parse all schedules arrival#departure,arrival#departure...
			boost::char_separator<char> sep1 (",");
			tokenizer schedulesTokens (schedules, sep1);

			ContinuousService::Schedules departureSchedules;
			ContinuousService::Schedules arrivalSchedules;

			for (tokenizer::iterator schedulesIter = schedulesTokens.begin();
				schedulesIter != schedulesTokens.end (); ++schedulesIter)
			{
				std::string arrDep (*schedulesIter);
				size_t sepPos = arrDep.find ("#");
				assert (sepPos != std::string::npos);

				std::string arrivalScheduleStr (arrDep.substr (0, sepPos));
				std::string departureScheduleStr (arrDep.substr (sepPos+1));

				boost::trim (departureScheduleStr);
				boost::trim (arrivalScheduleStr);

				if (departureScheduleStr.empty ())
				{
					assert (arrivalScheduleStr.empty () == false);
					departureScheduleStr = arrivalScheduleStr;
				}
				if (arrivalScheduleStr.empty ())
				{
					assert (departureScheduleStr.empty () == false);
					arrivalScheduleStr = departureScheduleStr;
				}

				Schedule departureSchedule (Schedule::FromString (departureScheduleStr));

				Schedule arrivalSchedule (Schedule::FromString (arrivalScheduleStr));
				arrivalSchedule += maxWaitingTime;

				Schedule endDepartureSchedule(departureSchedule);
				endDepartureSchedule += range;
				Schedule endArrivalSchedule(arrivalSchedule);
				endArrivalSchedule += range;

				departureSchedules.push_back (make_pair(departureSchedule, endDepartureSchedule));
				arrivalSchedules.push_back (make_pair(arrivalSchedule, endArrivalSchedule));
			}

			assert (departureSchedules.size () > 0);
			assert (arrivalSchedules.size () > 0);
			assert (departureSchedules.size () == arrivalSchedules.size ());


			cs->setKey(rows->getLongLong (TABLE_COL_ID));
			cs->setServiceNumber(serviceNumber);
			cs->setRange(range);
			cs->setMaxWaitingTime(maxWaitingTime);
			cs->setDepartureSchedules(departureSchedules);
			cs->setArrivalSchedules(arrivalSchedules);

		}

		template<> void SQLiteTableSyncTemplate<ContinuousServiceTableSync,ContinuousService>::_link(ContinuousService* cs, const SQLiteResultSPtr& rows, GetSource temporary)
		{
			uid pathId (rows->getLongLong (ContinuousServiceTableSync::COL_PATHID));

			shared_ptr<Path> path = EnvModule::fetchPath (pathId);
			assert (path.get());
//			assert (path->getEdges ().size () == arrivalSchedules.size ());

			uid bikeComplianceId (
				rows->getLongLong (ContinuousServiceTableSync::COL_BIKECOMPLIANCEID));

			uid handicappedComplianceId (
				rows->getLongLong (ContinuousServiceTableSync::COL_HANDICAPPEDCOMPLIANCEID));

			uid pedestrianComplianceId (
				rows->getLongLong (ContinuousServiceTableSync::COL_PEDESTRIANCOMPLIANCEID));

			cs->setPath(path.get());
			cs->setBikeCompliance (BikeCompliance::Get(bikeComplianceId).get());
			cs->setHandicappedCompliance (HandicappedCompliance::Get(handicappedComplianceId).get());
			cs->setPedestrianCompliance (PedestrianCompliance::Get (pedestrianComplianceId).get());

			if (temporary == GET_REGISTRY)
				path->addService (cs);
		}

		template<> void SQLiteTableSyncTemplate<ContinuousServiceTableSync,ContinuousService>::_unlink(ContinuousService* obj)
		{
			obj->getPath()->removeService(obj);
		}

		template<> void SQLiteTableSyncTemplate<ContinuousServiceTableSync,ContinuousService>::save(ContinuousService* object)
		{
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());	/// @todo Use grid ID
               
			 query
				<< " REPLACE INTO " << TABLE_NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				/// @todo fill other fields separated by ,
				<< ")";
			sqlite->execUpdate(query.str());
		}

	}

	namespace env
	{
		const std::string ContinuousServiceTableSync::COL_SERVICENUMBER ("service_number");
		const std::string ContinuousServiceTableSync::COL_SCHEDULES ("schedules");
		const std::string ContinuousServiceTableSync::COL_PATHID ("path_id");
		const std::string ContinuousServiceTableSync::COL_RANGE ("range");
		const std::string ContinuousServiceTableSync::COL_MAXWAITINGTIME ("max_waiting_time");
		const std::string ContinuousServiceTableSync::COL_BIKECOMPLIANCEID ("bike_compliance_id");
		const std::string ContinuousServiceTableSync::COL_HANDICAPPEDCOMPLIANCEID ("handicapped_compliance_id");
		const std::string ContinuousServiceTableSync::COL_PEDESTRIANCOMPLIANCEID ("pedestrian_compliance_id");

		ContinuousServiceTableSync::ContinuousServiceTableSync()
			: SQLiteRegistryTableSyncTemplate<ContinuousServiceTableSync,ContinuousService>()
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn (COL_SERVICENUMBER, "TEXT", true);
			addTableColumn (COL_SCHEDULES, "TEXT", true);
			addTableColumn (COL_PATHID, "INTEGER", false);
			addTableColumn (COL_RANGE, "INTEGER", true);
			addTableColumn (COL_MAXWAITINGTIME, "INTEGER", true);
			addTableColumn (COL_BIKECOMPLIANCEID, "INTEGER", true);
			addTableColumn (COL_HANDICAPPEDCOMPLIANCEID, "INTEGER", true);
			addTableColumn (COL_PEDESTRIANCOMPLIANCEID, "INTEGER", true);
		}

/*		void ContinuousServiceTableSync::rowsAdded(db::SQLite* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows, bool isFirstSync)
		{
			// Loop on each added row
			while (rows->next ())
			{
				if (ContinuousService::Contains(rows->getLongLong (TABLE_COL_ID)))
				{
					boost::shared_ptr<ContinuousService> service(ContinuousService::GetUpdateable(rows->getLongLong (TABLE_COL_ID)));
					service->getPath()->removeService(service.get());
					load(service.get(), rows);
				}
				else
				{
					ContinuousService* service(new ContinuousService);
					load(service, rows);
					service->store();
				}
			}
		}
		


		void ContinuousServiceTableSync::rowsUpdated(db::SQLite* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows)
		{
			while (rows->next ())
			{
				uid id = rows->getLongLong (TABLE_COL_ID);
				if (ContinuousService::Contains(id))
				{
					shared_ptr<ContinuousService> object = ContinuousService::GetUpdateable(id);
					object->getPath()->removeService(object.get());
					load(object.get(), rows);
				}
			}
		}

		void ContinuousServiceTableSync::rowsRemoved( db::SQLite* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows )
		{
			while (rows->next ())
			{
				uid id = rows->getLongLong (TABLE_COL_ID);
				if (ContinuousService::Contains(id))
				{
					shared_ptr<ContinuousService> object = ContinuousService::GetUpdateable(id);
					object->getPath()->removeService(object.get());
					ContinuousService::Remove(id);
				}
			}
		}
*/
		std::vector<shared_ptr<ContinuousService> > ContinuousServiceTableSync::search(int first /*= 0*/, int number /*= 0*/ )
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
				vector<shared_ptr<ContinuousService> > objects;
				while (rows->next ())
				{
					shared_ptr<ContinuousService> object(new ContinuousService());
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

		void ContinuousServiceTableSync::afterFirstSync( SQLite* sqlite,  SQLiteSync* sync )
		{
		}
	}
}
