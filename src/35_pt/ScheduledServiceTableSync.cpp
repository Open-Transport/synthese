
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

#include "ScheduledServiceTableSync.h"
#include "LoadException.h"
#include "Path.h"
#include "EnvModule.h"
#include "LineTableSync.h"
#include "CommercialLineTableSync.h"
#include "PTUseRuleTableSync.h"
#include "PTUseRule.h"
#include "GraphConstants.h"
#include <sstream>

#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"
#include "ReplaceQuery.h"

#include <boost/date_time/posix_time/ptime.hpp>

#include <assert.h>
#include <set>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace env;
	using namespace graph;
	using namespace pt;

	template<> const string util::FactorableTemplate<SQLiteTableSync,ScheduledServiceTableSync>::FACTORY_KEY("15.60.03 Scheduled services");
	template<> const string FactorableTemplate<Fetcher<SchedulesBasedService>, ScheduledServiceTableSync>::FACTORY_KEY("16");
	template<> const string FactorableTemplate<Fetcher<NonPermanentService>, ScheduledServiceTableSync>::FACTORY_KEY("16");

	namespace pt
	{
		const string ScheduledServiceTableSync::COL_SERVICENUMBER ("service_number");
		const string ScheduledServiceTableSync::COL_SCHEDULES ("schedules");
		const string ScheduledServiceTableSync::COL_PATHID ("path_id");
		const string ScheduledServiceTableSync::COL_RANKINPATH ("rank_in_path");
		const string ScheduledServiceTableSync::COL_BIKECOMPLIANCEID ("bike_compliance_id");
		const string ScheduledServiceTableSync::COL_HANDICAPPEDCOMPLIANCEID ("handicapped_compliance_id");
		const string ScheduledServiceTableSync::COL_PEDESTRIANCOMPLIANCEID ("pedestrian_compliance_id");
		const string ScheduledServiceTableSync::COL_TEAM("team");
		const string ScheduledServiceTableSync::COL_DATES("dates");
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<ScheduledServiceTableSync>::TABLE(
			"t016_scheduled_services"
		);

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<ScheduledServiceTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(ScheduledServiceTableSync::COL_SERVICENUMBER, SQL_TEXT),
			SQLiteTableSync::Field(ScheduledServiceTableSync::COL_SCHEDULES, SQL_TEXT),
			SQLiteTableSync::Field(ScheduledServiceTableSync::COL_PATHID, SQL_INTEGER, false),
			SQLiteTableSync::Field(ScheduledServiceTableSync::COL_BIKECOMPLIANCEID, SQL_INTEGER),
			SQLiteTableSync::Field(ScheduledServiceTableSync::COL_HANDICAPPEDCOMPLIANCEID, SQL_INTEGER),
			SQLiteTableSync::Field(ScheduledServiceTableSync::COL_PEDESTRIANCOMPLIANCEID, SQL_INTEGER),
			SQLiteTableSync::Field(ScheduledServiceTableSync::COL_TEAM, SQL_TEXT),
			SQLiteTableSync::Field(ScheduledServiceTableSync::COL_DATES, SQL_TEXT),
			SQLiteTableSync::Field()
		};
		
		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<ScheduledServiceTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index(ScheduledServiceTableSync::COL_PATHID.c_str(), ScheduledServiceTableSync::COL_SCHEDULES.c_str(), ""),
			SQLiteTableSync::Index()
		};

		template<> void SQLiteDirectTableSyncTemplate<ScheduledServiceTableSync,ScheduledService>::Load(
			ScheduledService* ss,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
		    string serviceNumber (rows->getText(ScheduledServiceTableSync::COL_SERVICENUMBER));

			uid pathId(rows->getLongLong(ScheduledServiceTableSync::COL_PATHID));

			try
			{
				ss->decodeSchedules(
					rows->getText(ScheduledServiceTableSync::COL_SCHEDULES)
				);
			}
			catch(...)
			{
				throw LoadException<ScheduledServiceTableSync>(rows, ScheduledServiceTableSync::COL_SCHEDULES, "Inconsistent schedules size");
			}

		    ss->setServiceNumber(serviceNumber);
			ss->setTeam(rows->getText(ScheduledServiceTableSync::COL_TEAM));
			ss->setPathId(pathId);
			ss->clearRules();
			ss->setFromSerializedString(rows->getText(ScheduledServiceTableSync::COL_DATES));

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				Path* path = LineTableSync::GetEditable(pathId, env, linkLevel).get();
				
				if(	path->getEdges ().size () != ss->getArrivalSchedules(false).size ()
				){
					throw LoadException<ScheduledServiceTableSync>(rows, ScheduledServiceTableSync::COL_SCHEDULES, "Inconsistent schedules size : different from path edges number");
				}

				uid bikeComplianceId (rows->getLongLong (ScheduledServiceTableSync::COL_BIKECOMPLIANCEID));

				uid handicappedComplianceId (rows->getLongLong (ScheduledServiceTableSync::COL_HANDICAPPEDCOMPLIANCEID));

				uid pedestrianComplianceId (rows->getLongLong (ScheduledServiceTableSync::COL_PEDESTRIANCOMPLIANCEID));


				if(bikeComplianceId > 0)
				{
					ss->addRule(
						USER_BIKE,
						PTUseRuleTableSync::Get(bikeComplianceId, env, linkLevel).get()
					);
				}
				if(handicappedComplianceId > 0)
				{
					ss->addRule(
						USER_HANDICAPPED,
						PTUseRuleTableSync::Get(handicappedComplianceId, env, linkLevel).get()
					);
				}
				if(pedestrianComplianceId > 0)
				{
					ss->addRule(
						USER_PEDESTRIAN,
						PTUseRuleTableSync::Get(pedestrianComplianceId, env, linkLevel).get()
					);
				}

				path->addService(
					ss,
					linkLevel == ALGORITHMS_OPTIMIZATION_LOAD_LEVEL
				);
			}
		}



		template<> void SQLiteDirectTableSyncTemplate<ScheduledServiceTableSync,ScheduledService>::Unlink(
			ScheduledService* ss
		){
			ss->getPath()->removeService(ss);
		}



		template<> void SQLiteDirectTableSyncTemplate<ScheduledServiceTableSync,ScheduledService>::Save(
			ScheduledService* object,
			optional<SQLiteTransaction&> transaction
		){
			// Dates preparation
			stringstream datesStr;
			object->serialize(datesStr);

			ReplaceQuery<ScheduledServiceTableSync> query(*object);
			query.addField(object->getServiceNumber());
			query.addField(object->encodeSchedules());
			query.addField(object->getPathId());
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
			query.addField(object->getTeam());
			query.addField(datesStr.str());
			query.execute(transaction);
		}

	}

	namespace pt
	{
		ScheduledServiceTableSync::SearchResult ScheduledServiceTableSync::Search(
			Env& env,
			optional<RegistryKeyType> lineId,
			optional<RegistryKeyType> commercialLineId,
			optional<RegistryKeyType> dataSourceId,
			optional<string> serviceNumber,
			bool hideOldServices,
			int first, /*= 0*/
			boost::optional<std::size_t> number, /*= 0*/
			bool orderByOriginTime,
			bool raisingOrder,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME;
			if (commercialLineId || dataSourceId)
				query << " INNER JOIN " << LineTableSync::TABLE.NAME << " AS l ON l." << TABLE_COL_ID << "=" << COL_PATHID;
			query << " WHERE 1 ";
			if (lineId)
				query << " AND " << ScheduledServiceTableSync::COL_PATHID << "=" << *lineId;
			if (commercialLineId)
				query << " AND l." << LineTableSync::COL_COMMERCIAL_LINE_ID << "=" << *commercialLineId;
			if (dataSourceId)
				query << " AND l." << LineTableSync::COL_DATASOURCE_ID << "=" << *dataSourceId;
			if(serviceNumber)
				query << " AND " << COL_SERVICENUMBER << "=" << *serviceNumber;
			if(hideOldServices)
			{
				ptime now(second_clock::local_time());
				now -= hours(1);
				time_duration snow(now.time_of_day());
				if(snow <= time_duration(3,0,0))
				{
					snow += hours(24);
				}
				query <<
					" AND " << ScheduledServiceTableSync::COL_SCHEDULES <<
					">='00:00:00#" << SchedulesBasedService::EncodeSchedule(snow) << "'" 
				;
			}
			if (orderByOriginTime)
			{
				query << " ORDER BY " << COL_SCHEDULES << (raisingOrder ? " ASC" : " DESC");
			}

			if (number)
			{
				query << " LIMIT " << (*number + 1);
				if (first > 0)
					query << " OFFSET " << first;
			}

			ScheduledServiceTableSync::SearchResult result(
				LoadFromQuery(query.str(), env, linkLevel)
			);
			
			return result;
}	}	}
