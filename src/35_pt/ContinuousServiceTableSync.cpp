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

#include "JourneyPattern.hpp"
#include "JourneyPatternTableSync.hpp"
#include "PTModule.h"
#include "TransportNetworkRight.h"
#include "DBModule.h"
#include "DBResult.hpp"
#include "DBException.hpp"
#include "PTUseRuleTableSync.h"
#include "PTUseRule.h"
#include "GraphConstants.h"
#include "ReplaceQuery.h"
#include "LoadException.h"
#include "LineStopTableSync.h"

#include <set>
#include <sstream>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>


using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace pt;
	using namespace graph;
	using namespace security;

	template<> const string util::FactorableTemplate<DBTableSync,ContinuousServiceTableSync>::FACTORY_KEY("35.60.02 Continuous services");
	template<> const string FactorableTemplate<Fetcher<SchedulesBasedService>, ContinuousServiceTableSync>::FACTORY_KEY("17");
	template<> const string FactorableTemplate<Fetcher<Service>, ContinuousServiceTableSync>::FACTORY_KEY("17");

	namespace pt
	{
		const std::string ContinuousServiceTableSync::COL_SERVICENUMBER ("service_number");
		const std::string ContinuousServiceTableSync::COL_SCHEDULES ("schedules");
		const std::string ContinuousServiceTableSync::COL_PATHID ("path_id");
		const std::string ContinuousServiceTableSync::COL_RANGE ("range");
		const std::string ContinuousServiceTableSync::COL_MAXWAITINGTIME ("max_waiting_time");
		const std::string ContinuousServiceTableSync::COL_BIKE_USE_RULE("bike_compliance_id");
		const std::string ContinuousServiceTableSync::COL_HANDICAPPED_USE_RULE ("handicapped_compliance_id");
		const std::string ContinuousServiceTableSync::COL_PEDESTRIAN_USE_RULE("pedestrian_compliance_id");
		const std::string ContinuousServiceTableSync::COL_DATES("dates");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<ContinuousServiceTableSync>::TABLE(
			"t017_continuous_services"
		);

		template<> const DBTableSync::Field DBTableSyncTemplate<ContinuousServiceTableSync>::_FIELDS[]=
		{
			DBTableSync::Field(TABLE_COL_ID, SQL_INTEGER),
			DBTableSync::Field(ContinuousServiceTableSync::COL_SERVICENUMBER, SQL_TEXT),
			DBTableSync::Field(ContinuousServiceTableSync::COL_SCHEDULES, SQL_TEXT),
			DBTableSync::Field(ContinuousServiceTableSync::COL_PATHID, SQL_INTEGER),
			DBTableSync::Field(ContinuousServiceTableSync::COL_RANGE, SQL_INTEGER),
			DBTableSync::Field(ContinuousServiceTableSync::COL_MAXWAITINGTIME, SQL_INTEGER),
			DBTableSync::Field(ContinuousServiceTableSync::COL_BIKE_USE_RULE, SQL_INTEGER),
			DBTableSync::Field(ContinuousServiceTableSync::COL_HANDICAPPED_USE_RULE, SQL_INTEGER),
			DBTableSync::Field(ContinuousServiceTableSync::COL_PEDESTRIAN_USE_RULE, SQL_INTEGER),
			DBTableSync::Field(ContinuousServiceTableSync::COL_DATES, SQL_TEXT),
			DBTableSync::Field()
		};

		template<> const DBTableSync::Index DBTableSyncTemplate<ContinuousServiceTableSync>::_INDEXES[]=
		{
			DBTableSync::Index(ContinuousServiceTableSync::COL_PATHID.c_str(), ContinuousServiceTableSync::COL_SCHEDULES.c_str(), ""),
			DBTableSync::Index()
		};

		template<> void DBDirectTableSyncTemplate<ContinuousServiceTableSync,ContinuousService>::Load(
			ContinuousService* cs,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			// Service number
			string serviceNumber (rows->getText(ContinuousServiceTableSync::COL_SERVICENUMBER));
			cs->setServiceNumber(serviceNumber);

			// Range
			boost::posix_time::time_duration range (minutes(rows->getInt (ContinuousServiceTableSync::COL_RANGE)));
			cs->setRange(range);

			// Max waiting time
			boost::posix_time::time_duration maxWaitingTime (minutes(rows->getInt (ContinuousServiceTableSync::COL_MAXWAITINGTIME)));
			cs->setMaxWaitingTime(maxWaitingTime);

			// Path
			util::RegistryKeyType pathId(rows->getLongLong(ContinuousServiceTableSync::COL_PATHID));
			Path* path(NULL);

			// Use rules
			RuleUser::Rules rules(RuleUser::GetEmptyRules());

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				// Path
				path = JourneyPatternTableSync::GetEditable(pathId, env, linkLevel).get();
				cs->setPath(path);
				if(path->getEdges().empty())
				{
					LineStopTableSync::Search(env, pathId);
				}

				// Use rules
				util::RegistryKeyType bikeComplianceId(
					rows->getLongLong (ContinuousServiceTableSync::COL_BIKE_USE_RULE)
				);
				if(bikeComplianceId > 0)
				{
					rules[USER_BIKE - USER_CLASS_CODE_OFFSET] = PTUseRuleTableSync::Get(bikeComplianceId, env, linkLevel).get();
				}
				util::RegistryKeyType handicappedComplianceId(
					rows->getLongLong (ContinuousServiceTableSync::COL_HANDICAPPED_USE_RULE)
				);
				if(handicappedComplianceId > 0)
				{
					rules[USER_HANDICAPPED - USER_CLASS_CODE_OFFSET] = PTUseRuleTableSync::Get(handicappedComplianceId, env, linkLevel).get();
				}
				util::RegistryKeyType pedestrianComplianceId(
					rows->getLongLong (ContinuousServiceTableSync::COL_PEDESTRIAN_USE_RULE)
				);
				if(pedestrianComplianceId > 0)
				{
					rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = PTUseRuleTableSync::Get(pedestrianComplianceId, env, linkLevel).get();
				}
			}
			cs->setRules(rules);

			// Schedules
			try
			{
				cs->decodeSchedules(
					rows->getText(ContinuousServiceTableSync::COL_SCHEDULES),
					maxWaitingTime
				);
			}
			catch(...)
			{
				throw LoadException<ContinuousServiceTableSync>(rows, ContinuousServiceTableSync::COL_SCHEDULES, "Inconsistent schedules size");
			}
			if(	cs->getPath() &&
				cs->getPath()->getEdges().size() != cs->getArrivalSchedules(false).size()
			){
				throw LoadException<ContinuousServiceTableSync>(rows, ContinuousServiceTableSync::COL_SCHEDULES, "Inconsistent schedules size : different from path edges number");
			}

			// Calendar
			cs->setFromSerializedString(rows->getText(ContinuousServiceTableSync::COL_DATES));

			// Registration in path
			if(path)
			{
				path->addService(*cs, linkLevel == ALGORITHMS_OPTIMIZATION_LOAD_LEVEL);
				cs->updatePathCalendar();
			}
		}

		template<> void DBDirectTableSyncTemplate<ContinuousServiceTableSync,ContinuousService>::Unlink(
			ContinuousService* obj
		){
			obj->getPath()->removeService(*obj);
		}



		template<> void DBDirectTableSyncTemplate<ContinuousServiceTableSync,ContinuousService>::Save(
			ContinuousService* object,
			optional<DBTransaction&> transaction
		){
			// Dates preparation
			stringstream datesStr;
			object->serialize(datesStr);

			ReplaceQuery<ContinuousServiceTableSync> query(*object);
			query.addField(object->getServiceNumber());
			query.addField(object->encodeSchedules(-object->getMaxWaitingTime()));
			query.addField(object->getPath() ? object->getPath()->getKey() : 0);
			query.addField(object->getRange().total_seconds() / 60);
			query.addField(object->getMaxWaitingTime().total_seconds() / 60);
			query.addField(
				object->getRule(USER_BIKE) && dynamic_cast<const PTUseRule*>(object->getRule(USER_BIKE)) ?
				static_cast<const PTUseRule*>(object->getRule(USER_BIKE))->getKey() :
				RegistryKeyType(0)
			);
			query.addField(
				object->getRule(USER_HANDICAPPED) && dynamic_cast<const PTUseRule*>(object->getRule(USER_HANDICAPPED)) ?
				static_cast<const PTUseRule*>(object->getRule(USER_HANDICAPPED))->getKey() :
				RegistryKeyType(0)
			);
			query.addField(
				object->getRule(USER_PEDESTRIAN) && dynamic_cast<const PTUseRule*>(object->getRule(USER_PEDESTRIAN)) ?
				static_cast<const PTUseRule*>(object->getRule(USER_PEDESTRIAN))->getKey() :
				RegistryKeyType(0)
			);
			query.addField(
				datesStr.str()
			);
			query.execute(transaction);
		}



		template<> bool DBTableSyncTemplate<ContinuousServiceTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(DELETE_RIGHT);
		}



		template<> void DBTableSyncTemplate<ContinuousServiceTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<ContinuousServiceTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<ContinuousServiceTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace pt
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
				query << " INNER JOIN " << JourneyPatternTableSync::TABLE.NAME << " AS l ON l." << TABLE_COL_ID << "=" << COL_PATHID;
			}
			query << " WHERE 1 ";
			if (lineId)
			{
				query << " AND " << COL_PATHID << "=" << *lineId;
			}
			if (commercialLineId)
			{
				query << " AND l." << JourneyPatternTableSync::COL_COMMERCIAL_LINE_ID << "=" << *commercialLineId;
			}
			if (orderByDepartureTime)
			{
				query << " ORDER BY " << COL_SCHEDULES << (raisingOrder ? " ASC" : " DESC");
			}
			if (number)
			{
				query << " LIMIT " << (*number + 1);
				if (first > 0)
				{
					query << " OFFSET " << first;
				}
			}

			return LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
