
/** ScheduledServiceTableSync class implementation.
	@file ScheduledServiceTableSync.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "DataSourceLinksField.hpp"
#include "LoadException.h"
#include "Path.h"
#include "Profile.h"
#include "PTModule.h"
#include "Session.h"
#include "User.h"
#include "JourneyPatternTableSync.hpp"
#include "PTUseRuleTableSync.h"
#include "PTUseRule.h"
#include "GraphConstants.h"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "LineStopTableSync.h"
#include "TransportNetworkRight.h"
#include "ImportableTableSync.hpp"
#include "CalendarLinkTableSync.hpp"
#include "CalendarLink.hpp"
#include "Vertex.h"
#include "StopPointTableSync.hpp"

#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/algorithm/string/split.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace pt;
	using namespace graph;
	using namespace security;
	using namespace impex;
	using namespace calendar;

	template<> const string util::FactorableTemplate<DBTableSync,ScheduledServiceTableSync>::FACTORY_KEY("35.60.03 Scheduled services");
	template<> const string FactorableTemplate<Fetcher<SchedulesBasedService>, ScheduledServiceTableSync>::FACTORY_KEY("16");
	template<> const string FactorableTemplate<Fetcher<Calendar>, ScheduledServiceTableSync>::FACTORY_KEY("16");

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
		const string ScheduledServiceTableSync::COL_STOPS("stops");
		const string ScheduledServiceTableSync::COL_DATASOURCE_LINKS("datasource_links");
		const string ScheduledServiceTableSync::COL_DATES_TO_FORCE = "dates_to_force";
		const string ScheduledServiceTableSync::COL_DATES_TO_BYPASS = "dates_to_bypass";
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<ScheduledServiceTableSync>::TABLE(
			"t016_scheduled_services"
		);

		template<> const Field DBTableSyncTemplate<ScheduledServiceTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(ScheduledServiceTableSync::COL_SERVICENUMBER, SQL_TEXT),
			Field(ScheduledServiceTableSync::COL_SCHEDULES, SQL_TEXT),
			Field(ScheduledServiceTableSync::COL_PATHID, SQL_INTEGER),
			Field(ScheduledServiceTableSync::COL_BIKECOMPLIANCEID, SQL_INTEGER),
			Field(ScheduledServiceTableSync::COL_HANDICAPPEDCOMPLIANCEID, SQL_INTEGER),
			Field(ScheduledServiceTableSync::COL_PEDESTRIANCOMPLIANCEID, SQL_INTEGER),
			Field(ScheduledServiceTableSync::COL_TEAM, SQL_TEXT),
			Field(ScheduledServiceTableSync::COL_DATES, SQL_TEXT),
			Field(ScheduledServiceTableSync::COL_STOPS, SQL_TEXT),
			Field(ScheduledServiceTableSync::COL_DATASOURCE_LINKS, SQL_TEXT),
			Field(ScheduledServiceTableSync::COL_DATES_TO_FORCE, SQL_TEXT),
			Field(ScheduledServiceTableSync::COL_DATES_TO_BYPASS, SQL_TEXT),
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<ScheduledServiceTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(DBTableSync::Index(ScheduledServiceTableSync::COL_PATHID.c_str(), ScheduledServiceTableSync::COL_SCHEDULES.c_str(), ""));
			return r;
		}

		template<>
		void OldLoadSavePolicy<ScheduledServiceTableSync,ScheduledService>::Load(
			ScheduledService* ss,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			DBModule::LoadObjects(ss->getLinkedObjectsIds(*rows), env, linkLevel);
			ss->loadFromRecord(*rows, env);
			if(linkLevel > util::FIELDS_ONLY_LOAD_LEVEL)
			{
				ss->link(env, linkLevel == util::ALGORITHMS_OPTIMIZATION_LOAD_LEVEL);
			}
		}



		template<> void OldLoadSavePolicy<ScheduledServiceTableSync,ScheduledService>::Unlink(
			ScheduledService* ss
		){
			ss->unlink();
		}



		template<> void OldLoadSavePolicy<ScheduledServiceTableSync,ScheduledService>::Save(
			ScheduledService* object,
			optional<DBTransaction&> transaction
		){
			// Dates preparation
			stringstream datesStr;
			if(object->getCalendarLinks().empty())
			{
				object->serialize(datesStr);
			}

			ReplaceQuery<ScheduledServiceTableSync> query(*object);
			query.addField(object->getServiceNumber());
			query.addField(object->encodeSchedules());
			query.addField(object->getPath() ? object->getPath()->getKey() : 0);
			query.addField(
				object->getRule(USER_BIKE) && dynamic_cast<const PTUseRule*>(object->getRule(USER_BIKE)) ?
				static_cast<const PTUseRule*>(object->getRule(USER_BIKE))->getKey() :
				RegistryKeyType(0)
			);

			if (object->getRule(USER_HANDICAPPED) && dynamic_cast<const PTUseRule*>(object->getRule(USER_HANDICAPPED)))
			{
				query.addField(static_cast<const PTUseRule*>(object->getRule(USER_HANDICAPPED))->getKey());
			}
			else
			{
				query.addField(RegistryKeyType(0));
			}

			query.addField(
				object->getRule(USER_PEDESTRIAN) && dynamic_cast<const PTUseRule*>(object->getRule(USER_PEDESTRIAN)) ?
				static_cast<const PTUseRule*>(object->getRule(USER_PEDESTRIAN))->getKey() :
				RegistryKeyType(0)
			);
			query.addField(object->getTeam());
			query.addField(datesStr.str());
			query.addField(object->encodeStops());
			query.addField(
				DataSourceLinks::Serialize(
					object->getDataSourceLinks()
			)	);

			// Dates to force
			{
				stringstream s;
				bool first(true);
				BOOST_FOREACH(const date& d, object->getDatesToForce())
				{
					if(first)
					{
						first = false;
					}
					else
					{
						s << ",";
					}
					s << to_iso_extended_string(d);
				}
				query.addField(s.str());
			}

			// Dates to bypass
			{
				stringstream s;
				bool first(true);
				BOOST_FOREACH(const date& d, object->getDatesToBypass())
				{
					if(first)
					{
						first = false;
					}
					else
					{
						s << ",";
					}
					s << to_iso_extended_string(d);
				}
				query.addField(s.str());
			}

			query.execute(transaction);
		}



		template<> bool DBTableSyncTemplate<ScheduledServiceTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO test if the user has sufficient right level for this service
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(DELETE_RIGHT, UNKNOWN_RIGHT_LEVEL, string());
		}



		template<> void DBTableSyncTemplate<ScheduledServiceTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
			Env env;
			CalendarLinkTableSync::SearchResult calendarLinks(CalendarLinkTableSync::Search(env, id));
			BOOST_FOREACH(const CalendarLinkTableSync::SearchResult::value_type& calendarLink, calendarLinks)
			{
				CalendarLinkTableSync::Remove(NULL, calendarLink->getKey(), transaction, false);
			}
		}



		template<> void DBTableSyncTemplate<ScheduledServiceTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<ScheduledServiceTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
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
			SelectQuery<ScheduledServiceTableSync> query;
			if (lineId)
			{
				query.addWhereField(ScheduledServiceTableSync::COL_PATHID, *lineId);
			}
			if (commercialLineId)
			{
				query.addWhere(
					ComposedExpression::Get(
						SubQueryExpression::Get(
							string("SELECT b."+ JourneyPatternCommercialLine::FIELD.name +" FROM "+ JourneyPatternTableSync::TABLE.NAME +" AS b WHERE b."+ TABLE_COL_ID +"="+ TABLE.NAME +"."+ COL_PATHID)
						), ComposedExpression::OP_EQ,
						ValueExpression<RegistryKeyType>::Get(*commercialLineId)
					)
				);
			}
		//	if (dataSourceId)
		//	{
		//		query.addWhereFieldOther<JourneyPatternTableSync>(JourneyPatternTableSync::COL_DATASOURCE_ID, *dataSourceId);
		//	}
			if(serviceNumber)
			{
				query.addWhereField(COL_SERVICENUMBER, *serviceNumber);
			}
			if(hideOldServices)
			{
				ptime now(second_clock::local_time());
				now -= hours(1);
				time_duration snow(now.time_of_day());
				if(snow <= time_duration(3,0,0))
				{
					snow += hours(24);
				}
				//query.addWhereField(ScheduledServiceTableSync::COL_SCHEDULES,"00:00:00#"+ SchedulesBasedService::EncodeSchedule(snow), ComposedExpression::OP_SUPEQ);
				query.addWhere(
					ComposedExpression::Get(
						ComposedExpression::Get(
							FieldExpression::Get(TABLE.NAME, COL_SCHEDULES),
							ComposedExpression::OP_SUPEQ,
							ValueExpression<string>::Get(SchedulesBasedService::EncodeSchedule(snow))
						),
						ComposedExpression::OP_OR,
						ComposedExpression::Get(
							ComposedExpression::Get(
								FieldExpression::Get(TABLE.NAME, COL_SCHEDULES),
								ComposedExpression::OP_LIKE,
								ValueExpression<string>::Get("00:00:00#%")
							),
							ComposedExpression::OP_AND,
							ComposedExpression::Get(
								FieldExpression::Get(TABLE.NAME, COL_SCHEDULES),
								ComposedExpression::OP_SUPEQ,
								ValueExpression<string>::Get("00:00:00#"+ SchedulesBasedService::EncodeSchedule(snow))
							)
						)
					)
				);
			}
			if (orderByOriginTime)
			{
				query.addOrderField(COL_SCHEDULES, raisingOrder);
			}

			if (number)
			{
				query.setNumber(*number + 1);
				if (first > 0)
					query.setFirst(first);
			}

			return LoadFromQuery(query, env, linkLevel);
		}
}	}
