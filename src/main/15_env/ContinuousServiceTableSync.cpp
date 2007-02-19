
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

#include <sstream>

#include <boost/tokenizer.hpp>

#include "01_util/Conversion.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"
#include "02_db/SQLiteException.h"

#include "04_time/Schedule.h"

#include "15_env/ContinuousService.h"
#include "15_env/Path.h"
#include "15_env/Point.h"
#include "15_env/EnvModule.h"
#include "15_env/ContinuousServiceTableSync.h"

using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace env;
	using namespace time;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<ContinuousService>::TABLE_NAME = "t017_continuous_services";
		template<> const int SQLiteTableSyncTemplate<ContinuousService>::TABLE_ID = 17;
		template<> const bool SQLiteTableSyncTemplate<ContinuousService>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<ContinuousService>::load(ContinuousService* cs, const db::SQLiteResult& rows, int rowIndex )
		{
			uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));

			int serviceNumber (Conversion::ToInt (
				rows.getColumn (rowIndex, ContinuousServiceTableSync::COL_SERVICENUMBER)));

			std::string schedules (
				rows.getColumn (rowIndex, ContinuousServiceTableSync::COL_SCHEDULES));

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

				departureSchedules.push_back (departureSchedule);
				arrivalSchedules.push_back (arrivalSchedule);
			}

			assert (departureSchedules.size () > 0);
			assert (arrivalSchedules.size () > 0);
			assert (departureSchedules.size () == arrivalSchedules.size ());

			uid pathId (Conversion::ToLongLong (rows.getColumn (rowIndex, ContinuousServiceTableSync::COL_PATHID)));

			Path* path = EnvModule::fetchPath (pathId);
			assert (path != 0);
			assert (path->getEdges ().size () == arrivalSchedules.size ());

			int range (Conversion::ToInt (
				rows.getColumn (rowIndex, ContinuousServiceTableSync::COL_RANGE)));

			int maxWaitingTime (Conversion::ToInt (
				rows.getColumn (rowIndex, ContinuousServiceTableSync::COL_MAXWAITINGTIME)));

			uid bikeComplianceId (
				Conversion::ToLongLong (rows.getColumn (rowIndex, ContinuousServiceTableSync::COL_BIKECOMPLIANCEID)));

			uid handicappedComplianceId (
				Conversion::ToLongLong (rows.getColumn (rowIndex, ContinuousServiceTableSync::COL_HANDICAPPEDCOMPLIANCEID)));

			uid pedestrianComplianceId (
				Conversion::ToLongLong (rows.getColumn (rowIndex, ContinuousServiceTableSync::COL_PEDESTRIANCOMPLIANCEID)));

			cs->setKey(Conversion::ToLongLong(rows.getColumn(rowIndex, TABLE_COL_ID)));
			cs->setServiceNumber(serviceNumber);
			cs->setPath(path);
			cs->setDepartureSchedule(departureSchedules.at(0));
			cs->setArrivalSchedule(arrivalSchedules.at(arrivalSchedules.size()-1));
			cs->setRange(range);
			cs->setMaxWaitingTime(maxWaitingTime);
			cs->setBikeCompliance (EnvModule::getBikeCompliances ().get (bikeComplianceId));
			cs->setHandicappedCompliance (EnvModule::getHandicappedCompliances ().get (handicappedComplianceId));
			cs->setPedestrianCompliance (EnvModule::getPedestrianCompliances ().get (pedestrianComplianceId));

			path->addService (cs, departureSchedules, arrivalSchedules);
		}

		template<> void SQLiteTableSyncTemplate<ContinuousService>::save(ContinuousService* object)
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
		const std::string ContinuousServiceTableSync::COL_SERVICENUMBER ("service_number");
		const std::string ContinuousServiceTableSync::COL_SCHEDULES ("schedules");
		const std::string ContinuousServiceTableSync::COL_PATHID ("path_id");
		const std::string ContinuousServiceTableSync::COL_RANGE ("range");
		const std::string ContinuousServiceTableSync::COL_MAXWAITINGTIME ("max_waiting_time");
		const std::string ContinuousServiceTableSync::COL_BIKECOMPLIANCEID ("bike_compliance_id");
		const std::string ContinuousServiceTableSync::COL_HANDICAPPEDCOMPLIANCEID ("handicapped_compliance_id");
		const std::string ContinuousServiceTableSync::COL_PEDESTRIANCOMPLIANCEID ("pedestrian_compliance_id");

		ContinuousServiceTableSync::ContinuousServiceTableSync()
			: SQLiteTableSyncTemplate<ContinuousService>(TABLE_NAME, true, true, TRIGGERS_ENABLED_CLAUSE)
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

		void ContinuousServiceTableSync::rowsAdded(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows)
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				if (EnvModule::getContinuousServices().contains(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID))))
				{
					ContinuousService* object = EnvModule::getContinuousServices().get(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID)));
					object->getPath()->removeService(object);
					load(object, rows, i);
				}
				else
				{
					ContinuousService* object = new ContinuousService;
					load(object, rows, i);
					EnvModule::getContinuousServices().add(object);
				}
			}
		}
		
		void ContinuousServiceTableSync::rowsUpdated(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows)
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				uid id = Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID));
				if (EnvModule::getContinuousServices().contains(id))
				{
					ContinuousService* object = EnvModule::getContinuousServices().get(id);
					object->getPath()->removeService(object);
					load(object, rows, i);
				}
			}
		}

		void ContinuousServiceTableSync::rowsRemoved( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				uid id = Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID));
				if (EnvModule::getContinuousServices().contains(id))
				{
					ContinuousService* object = EnvModule::getContinuousServices().get(id);
					object->getPath()->removeService(object);
					EnvModule::getContinuousServices().remove(id);
				}
			}
		}

		std::vector<ContinuousService*> ContinuousServiceTableSync::search(int first /*= 0*/, int number /*= 0*/ )
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
				vector<ContinuousService*> objects;
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					ContinuousService* object = new ContinuousService();
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
