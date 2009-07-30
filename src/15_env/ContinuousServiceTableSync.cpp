////////////////////////////////////////////////////////////////////////////////
/// ContinuousServiceTableSync class implementation.
///	@file ContinuousServiceTableSync.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "ContinuousServiceTableSync.h"

#include "Line.h"
#include "LineTableSync.h"
#include "EnvModule.h"

#include <sstream>

#include <boost/tokenizer.hpp>

#include "Conversion.h"

#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"
#include "ServiceDate.h"
#include "ServiceDateTableSync.h"
#include "Schedule.h"
#include "PTUseRuleTableSync.h"
#include "PTUseRule.h"
#include "Point2D.h"
#include "GraphConstants.h"

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
	using namespace graph;
	using namespace pt;

	template<> const string util::FactorableTemplate<SQLiteTableSync,ContinuousServiceTableSync>::FACTORY_KEY("15.60.02 Continuous services");
	template<> const string FactorableTemplate<Fetcher<NonPermanentService>, ContinuousServiceTableSync>::FACTORY_KEY("17");

	namespace env
	{
		const std::string ContinuousServiceTableSync::COL_SERVICENUMBER ("service_number");
		const std::string ContinuousServiceTableSync::COL_SCHEDULES ("schedules");
		const std::string ContinuousServiceTableSync::COL_PATHID ("path_id");
		const std::string ContinuousServiceTableSync::COL_RANGE ("range");
		const std::string ContinuousServiceTableSync::COL_MAXWAITINGTIME ("max_waiting_time");
		const std::string ContinuousServiceTableSync::COL_BIKE_USE_RULE("bike_compliance_id");
		const std::string ContinuousServiceTableSync::COL_HANDICAPPED_USE_RULE ("handicapped_compliance_id");
		const std::string ContinuousServiceTableSync::COL_PEDESTRIAN_USE_RULE("pedestrian_compliance_id");
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<ContinuousServiceTableSync>::TABLE(
			"t017_continuous_services"
		);

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<ContinuousServiceTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(ContinuousServiceTableSync::COL_SERVICENUMBER, SQL_TEXT),
			SQLiteTableSync::Field(ContinuousServiceTableSync::COL_SCHEDULES, SQL_TEXT),
			SQLiteTableSync::Field(ContinuousServiceTableSync::COL_PATHID, SQL_INTEGER),
			SQLiteTableSync::Field(ContinuousServiceTableSync::COL_RANGE, SQL_INTEGER),
			SQLiteTableSync::Field(ContinuousServiceTableSync::COL_MAXWAITINGTIME, SQL_INTEGER),
			SQLiteTableSync::Field(ContinuousServiceTableSync::COL_BIKE_USE_RULE, SQL_INTEGER),
			SQLiteTableSync::Field(ContinuousServiceTableSync::COL_HANDICAPPED_USE_RULE, SQL_INTEGER),
			SQLiteTableSync::Field(ContinuousServiceTableSync::COL_PEDESTRIAN_USE_RULE, SQL_INTEGER),
			SQLiteTableSync::Field()
		};
		
		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<ContinuousServiceTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index(ContinuousServiceTableSync::COL_PATHID.c_str(), ContinuousServiceTableSync::COL_SCHEDULES.c_str(), ""),
			SQLiteTableSync::Index()
		};

		template<> void SQLiteDirectTableSyncTemplate<ContinuousServiceTableSync,ContinuousService>::Load(
			ContinuousService* cs,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
		    string serviceNumber (rows->getText(ContinuousServiceTableSync::COL_SERVICENUMBER));
		    int range (rows->getInt (ContinuousServiceTableSync::COL_RANGE));
		    int maxWaitingTime (rows->getInt (ContinuousServiceTableSync::COL_MAXWAITINGTIME));
			uid pathId(rows->getLongLong(ContinuousServiceTableSync::COL_PATHID));

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

			cs->setServiceNumber(serviceNumber);
			cs->setRange(range);
			cs->setMaxWaitingTime(maxWaitingTime);
			cs->setDepartureSchedules(departureSchedules);
			cs->setArrivalSchedules(arrivalSchedules);
			cs->setPathId(pathId);
			cs->clearRules();

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{

				uid pathId (rows->getLongLong (ContinuousServiceTableSync::COL_PATHID));

				Path* path(LineTableSync::GetEditable(pathId, env, linkLevel).get());
				assert (path);
	//			assert (path->getEdges ().size () == arrivalSchedules.size ());

				uid bikeComplianceId(
					rows->getLongLong (ContinuousServiceTableSync::COL_BIKE_USE_RULE)
				);
				uid handicappedComplianceId(
					rows->getLongLong (ContinuousServiceTableSync::COL_HANDICAPPED_USE_RULE)
				);
				uid pedestrianComplianceId(
					rows->getLongLong (ContinuousServiceTableSync::COL_PEDESTRIAN_USE_RULE)
				);

				if(bikeComplianceId > 0)
				{
					cs->addRule(
						USER_BIKE,
						PTUseRuleTableSync::Get(bikeComplianceId, env, linkLevel).get()
					);
				}
				if(handicappedComplianceId > 0)
				{
					cs->addRule(
						USER_HANDICAPPED,
						PTUseRuleTableSync::Get(handicappedComplianceId, env, linkLevel).get()
					);
				}
				if(pedestrianComplianceId > 0)
				{
					cs->addRule(
						USER_PEDESTRIAN,
						PTUseRuleTableSync::Get(pedestrianComplianceId, env, linkLevel).get()
					);
				}

				path->addService (cs, linkLevel == ALGORITHMS_OPTIMIZATION_LOAD_LEVEL);
			}
/*			if (linkLevel == DOWN_LINKS_LOAD_LEVEL || linkLevel == UP_DOWN_LINKS_LOAD_LEVEL)
			{
				ServiceDateTableSync::SetActiveDates(*cs);
			}
*/		}

		template<> void SQLiteDirectTableSyncTemplate<ContinuousServiceTableSync,ContinuousService>::Unlink(
			ContinuousService* obj
		){
			obj->getPath()->removeService(obj);
		}

		template<> void SQLiteDirectTableSyncTemplate<ContinuousServiceTableSync,ContinuousService>::Save(ContinuousService* object)
		{
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());	/// @todo Use grid ID
               
			 query
				<< " REPLACE INTO " << TABLE.NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				/// @todo fill other fields separated by ,
				<< ")";
			sqlite->execUpdate(query.str());
		}

	}

	namespace env
	{
		ContinuousServiceTableSync::SearchResult ContinuousServiceTableSync::Search(
			Env& env,
			boost::optional<util::RegistryKeyType> lineId,
			boost::optional<util::RegistryKeyType> commercialLineId,
			int first /*= 0*/
			, boost::optional<std::size_t> number  /*= 0*/
			, bool orderByDepartureTime
			, bool raisingOrder,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME;
			if (commercialLineId)
			{
				query << " INNER JOIN " << LineTableSync::TABLE.NAME << " AS l ON l." << TABLE_COL_ID << "=" << COL_PATHID;
			}
			query << " WHERE 1 ";
			if (lineId)
			{
				query << " AND " << COL_PATHID << "=" << *lineId;
			}
			if (commercialLineId)
			{
				query << " AND l." << LineTableSync::COL_COMMERCIAL_LINE_ID << "=" << *commercialLineId;
			}
			if (orderByDepartureTime)
				query << " ORDER BY " << COL_SCHEDULES << (raisingOrder ? " ASC" : " DESC");
			if (number)
				query << " LIMIT " << (*number + 1);
			if (first > 0)
				query << " OFFSET " << first;

			return LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
