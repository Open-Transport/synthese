
/** ScheduledServiceTableSync class implementation.
	@file ScheduledServiceTableSync.cpp

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

#include "04_time/Schedule.h"

#include "15_env/EnvModule.h"
#include "15_env/ScheduledService.h"
#include "15_env/Path.h"
#include "15_env/Point.h"
#include "15_env/ScheduledServiceTableSync.h"

#include <boost/tokenizer.hpp>
#include <sqlite/sqlite3.h>

#include <assert.h>

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
		template<> const std::string SQLiteTableSyncTemplate<ScheduledService>::TABLE_NAME = "t016_scheduled_services";
		template<> const int SQLiteTableSyncTemplate<ScheduledService>::TABLE_ID = 16;
		template<> const bool SQLiteTableSyncTemplate<ScheduledService>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<ScheduledService>::load(ScheduledService* ss, const db::SQLiteResult& rows, int rowIndex/*=0*/ )
		{


			uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));

			int serviceNumber (Conversion::ToInt (
				rows.getColumn (rowIndex, ScheduledServiceTableSync::COL_SERVICENUMBER)));

			std::string schedules (
				rows.getColumn (rowIndex, ScheduledServiceTableSync::COL_SCHEDULES));

			typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

			// Parse all schedules arrival#departure,arrival#departure...
			boost::char_separator<char> sep1 (",");
			tokenizer schedulesTokens (schedules, sep1);

			std::vector<synthese::time::Schedule> departureSchedules;
			std::vector<synthese::time::Schedule> arrivalSchedules;

			for (tokenizer::iterator schedulesIter = schedulesTokens.begin();
				schedulesIter != schedulesTokens.end (); ++schedulesIter)
			{
				std::string arrDep (*schedulesIter);
				size_t sepPos = arrDep.find ("#");
				assert (sepPos != std::string::npos);

				std::string arrivalScheduleStr (arrDep.substr (0, sepPos));
				std::string departureScheduleStr (arrDep.substr (sepPos+1));

				// unnecessary : boost::trim (departureScheduleStr);
				// unnecessary : boost::trim (arrivalScheduleStr);

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

				departureSchedules.push_back (departureSchedule);
				arrivalSchedules.push_back (arrivalSchedule);
			}

			assert (departureSchedules.size () > 0);
			assert (arrivalSchedules.size () > 0);
			assert (departureSchedules.size () == arrivalSchedules.size ());

			uid pathId (Conversion::ToLongLong (rows.getColumn (rowIndex, ScheduledServiceTableSync::COL_PATHID)));

			shared_ptr<Path> path = EnvModule::fetchPath (pathId);
			assert (path != 0);
			assert (path->getEdges ().size () == arrivalSchedules.size ());

			uid bikeComplianceId (
				Conversion::ToLongLong (rows.getColumn (rowIndex, ScheduledServiceTableSync::COL_BIKECOMPLIANCEID)));

			uid handicappedComplianceId (
				Conversion::ToLongLong (rows.getColumn (rowIndex, ScheduledServiceTableSync::COL_HANDICAPPEDCOMPLIANCEID)));

			uid pedestrianComplianceId (
				Conversion::ToLongLong (rows.getColumn (rowIndex, ScheduledServiceTableSync::COL_PEDESTRIANCOMPLIANCEID)));

			uid reservationRuleId (
				Conversion::ToLongLong (rows.getColumn (rowIndex, ScheduledServiceTableSync::COL_RESERVATIONRULEID)));

			ss->setPath(path.get());
			ss->setDepartureSchedule(departureSchedules.at (0));
			ss->setArrivalSchedule(arrivalSchedules.at(arrivalSchedules.size()-1));
			ss->setServiceNumber(serviceNumber);
			ss->setKey(id);
			ss->setBikeCompliance (EnvModule::getBikeCompliances ().get (bikeComplianceId).get());
			ss->setHandicappedCompliance (EnvModule::getHandicappedCompliances ().get (handicappedComplianceId).get());
			ss->setPedestrianCompliance (EnvModule::getPedestrianCompliances ().get (pedestrianComplianceId).get());
			ss->setReservationRule (EnvModule::getReservationRules ().get (reservationRuleId).get());
			ss->getPath()->addService(ss, departureSchedules, arrivalSchedules);
		}

		template<> void SQLiteTableSyncTemplate<ScheduledService>::save(ScheduledService* object)
		{
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId(1,1));	/// @todo Use grid ID
               
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
		const std::string ScheduledServiceTableSync::COL_SERVICENUMBER ("service_number");
		const std::string ScheduledServiceTableSync::COL_SCHEDULES ("schedules");
		const std::string ScheduledServiceTableSync::COL_PATHID ("path_id");
		const std::string ScheduledServiceTableSync::COL_RANKINPATH ("rank_in_path");
		const std::string ScheduledServiceTableSync::COL_BIKECOMPLIANCEID ("bike_compliance_id");
		const std::string ScheduledServiceTableSync::COL_HANDICAPPEDCOMPLIANCEID ("handicapped_compliance_id");
		const std::string ScheduledServiceTableSync::COL_PEDESTRIANCOMPLIANCEID ("pedestrian_compliance_id");
		const std::string ScheduledServiceTableSync::COL_RESERVATIONRULEID ("reservation_rule_id");

		ScheduledServiceTableSync::ScheduledServiceTableSync()
			: SQLiteTableSyncTemplate<ScheduledService>(TABLE_NAME, true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn (COL_SERVICENUMBER, "TEXT", true);
			addTableColumn (COL_SCHEDULES, "TEXT", true);
			addTableColumn (COL_PATHID, "INTEGER", false);
			addTableColumn (COL_BIKECOMPLIANCEID, "INTEGER", true);
			addTableColumn (COL_HANDICAPPEDCOMPLIANCEID, "INTEGER", true);
			addTableColumn (COL_PEDESTRIANCOMPLIANCEID, "INTEGER", true);
			addTableColumn (COL_RESERVATIONRULEID, "INTEGER", true);
		}

		void ScheduledServiceTableSync::rowsAdded(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows, bool isFirstSync)
		{
			Path* lastPath(NULL);

			// Loop on each added row
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				boost::shared_ptr<ScheduledService> service;
				if (EnvModule::getScheduledServices().contains(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID))))
				{
					service = EnvModule::getScheduledServices().getUpdateable(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID)));
					service->getPath()->removeService(service.get());
					load(service.get(), rows, i);
				}
				else
				{
					service.reset(new ScheduledService);
					load(service.get(), rows, i);
					EnvModule::getScheduledServices().add(service);
				}

				/* At the execution syncs, update the schedules indexes at each path change */
				if (!isFirstSync && service.get() && service->getPath() != lastPath)
				{
					lastPath->updateScheduleIndexes();
					lastPath = service->getPath();
				}
			}

			if (!isFirstSync && lastPath)
				lastPath->updateScheduleIndexes();

		}
		
		void ScheduledServiceTableSync::rowsUpdated(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows)
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				uid id = Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID));
				if (EnvModule::getScheduledServices().contains(id))
				{
					shared_ptr<ScheduledService> object = EnvModule::getScheduledServices().getUpdateable(id);
					object->getPath()->removeService(object.get());
					load(object.get(), rows, i);
				}
			}
		}

		void ScheduledServiceTableSync::rowsRemoved( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				uid id = Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID));
				if (EnvModule::getScheduledServices().contains(id))
				{
					shared_ptr<ScheduledService> object(EnvModule::getScheduledServices().getUpdateable(id));
					object->getPath()->removeService(object.get());
					EnvModule::getScheduledServices().remove(id);
				}
			}
		}

		std::vector<shared_ptr<ScheduledService> > ScheduledServiceTableSync::search(int first /*= 0*/, int number /*= 0*/ )
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
				vector<shared_ptr<ScheduledService> > objects;
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					shared_ptr<ScheduledService> object ( new ScheduledService());
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

		void ScheduledServiceTableSync::afterFirstSync( const SQLiteQueueThreadExec* sqlite,  SQLiteSync* sync )
		{
			// Lines
			for (Line::Registry::const_iterator it = EnvModule::getLines().begin(); it != EnvModule::getLines().end(); it++)
			{
				shared_ptr<Line> line = EnvModule::getLines().getUpdateable(it->first);
				line ->updateScheduleIndexes();
			}

			// Roads
/*			for (Road::Registry::const_iterator it = EnvModule::getRoads().begin(); it != EnvModule::getLines().end(); it++)
			{
				(*it)->updateSchedulesIndexes();
			}
*/
		}
	}
}

