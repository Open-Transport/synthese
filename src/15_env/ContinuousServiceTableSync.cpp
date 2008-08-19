
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

#include "Path.h"
#include "LineTableSync.h"
#include "BikeComplianceTableSync.h"
#include "PedestrianComplianceTableSync.h"
#include "HandicappedComplianceTableSync.h"
#include "EnvModule.h"

#include <sstream>

#include <boost/tokenizer.hpp>

#include "Conversion.h"

#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"

#include "Schedule.h"

#include "Point2D.h"

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
		template<> const std::string SQLiteTableSyncTemplate<ContinuousServiceTableSync>::TABLE_NAME = "t017_continuous_services";
		template<> const int SQLiteTableSyncTemplate<ContinuousServiceTableSync>::TABLE_ID = 17;
		template<> const bool SQLiteTableSyncTemplate<ContinuousServiceTableSync>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteDirectTableSyncTemplate<ContinuousServiceTableSync,ContinuousService>::load(ContinuousService* cs, const db::SQLiteResultSPtr& rows )
		{
		    uid id (rows->getLongLong (TABLE_COL_ID));

		    string serviceNumber (rows->getText(ContinuousServiceTableSync::COL_SERVICENUMBER));
		    int range (rows->getInt (ContinuousServiceTableSync::COL_RANGE));
		    int maxWaitingTime (rows->getInt (ContinuousServiceTableSync::COL_MAXWAITINGTIME));

		    string schedules (
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

		template<> void SQLiteDirectTableSyncTemplate<ContinuousServiceTableSync,ContinuousService>::_link(ContinuousService* cs, const SQLiteResultSPtr& rows, GetSource temporary)
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

			cs->setBikeCompliance (BikeCompliance::Get(bikeComplianceId).get());
			cs->setHandicappedCompliance (HandicappedCompliance::Get(handicappedComplianceId).get());
			cs->setPedestrianCompliance (PedestrianCompliance::Get (pedestrianComplianceId).get());

			if (temporary == GET_REGISTRY)
				path->addService (cs);
		}

		template<> void SQLiteDirectTableSyncTemplate<ContinuousServiceTableSync,ContinuousService>::_unlink(ContinuousService* obj)
		{
			obj->getPath()->removeService(obj);
		}

		template<> void SQLiteDirectTableSyncTemplate<ContinuousServiceTableSync,ContinuousService>::save(ContinuousService* object)
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



		std::vector<shared_ptr<ContinuousService> > ContinuousServiceTableSync::search(
			uid lineId
			, int first /*= 0*/
			, int number /*= 0*/
			, bool orderByDepartureTime
			, bool raisingOrder
		){
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE 1 ";
			if (lineId != UNKNOWN_VALUE)
				query << " AND " << COL_PATHID << "=" << lineId;
			if (orderByDepartureTime)
				query << " ORDER BY " << COL_SCHEDULES << (raisingOrder ? " ASC" : " DESC");
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
