////////////////////////////////////////////////////////////////////////////////
/// ContinuousServiceTableSync class implementation.
///	@file ContinuousServiceTableSync.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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

#include "DBException.hpp"
#include "DBModule.h"
#include "DBResult.hpp"
#include "JourneyPattern.hpp"
#include "JourneyPatternTableSync.hpp"
#include "Profile.h"
#include "PTModule.h"
#include "Session.h"
#include "TransportNetworkRight.h"
#include "User.h"
#include "PTUseRuleTableSync.h"
#include "PTUseRule.h"
#include "GraphConstants.h"
#include "ReplaceQuery.h"
#include "LoadException.h"
#include "LineStopTableSync.h"
#include "CalendarLinkTableSync.hpp"
#include "CalendarLink.hpp"

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
	using namespace calendar;

	template<> const string util::FactorableTemplate<DBTableSync,ContinuousServiceTableSync>::FACTORY_KEY("35.60.02 Continuous services");
	template<> const string FactorableTemplate<Fetcher<SchedulesBasedService>, ContinuousServiceTableSync>::FACTORY_KEY("17");
	template<> const string FactorableTemplate<Fetcher<Calendar>, ContinuousServiceTableSync>::FACTORY_KEY("17");

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

		template<> const Field DBTableSyncTemplate<ContinuousServiceTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(ContinuousServiceTableSync::COL_SERVICENUMBER, SQL_TEXT),
			Field(ContinuousServiceTableSync::COL_SCHEDULES, SQL_TEXT),
			Field(ContinuousServiceTableSync::COL_PATHID, SQL_INTEGER),
			Field(ContinuousServiceTableSync::COL_RANGE, SQL_INTEGER),
			Field(ContinuousServiceTableSync::COL_MAXWAITINGTIME, SQL_INTEGER),
			Field(ContinuousServiceTableSync::COL_BIKE_USE_RULE, SQL_INTEGER),
			Field(ContinuousServiceTableSync::COL_HANDICAPPED_USE_RULE, SQL_INTEGER),
			Field(ContinuousServiceTableSync::COL_PEDESTRIAN_USE_RULE, SQL_INTEGER),
			Field(ContinuousServiceTableSync::COL_DATES, SQL_TEXT),
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<ContinuousServiceTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(
					ContinuousServiceTableSync::COL_PATHID.c_str(),
					ContinuousServiceTableSync::COL_SCHEDULES.c_str(),
			"")	);
			return r;
		};

		template<> void OldLoadSavePolicy<ContinuousServiceTableSync,ContinuousService>::Load(
			ContinuousService* cs,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			DBModule::LoadObjects(cs->getLinkedObjectsIds(*rows), env, linkLevel);
			cs->loadFromRecord(*rows, env);
			if(linkLevel > util::FIELDS_ONLY_LOAD_LEVEL)
			{
				cs->link(env, linkLevel == util::ALGORITHMS_OPTIMIZATION_LOAD_LEVEL);
			}
		}



		template<> void OldLoadSavePolicy<ContinuousServiceTableSync,ContinuousService>::Unlink(
			ContinuousService* obj
		){
			obj->unlink();
		}



		template<> void OldLoadSavePolicy<ContinuousServiceTableSync,ContinuousService>::Save(
			ContinuousService* object,
			optional<DBTransaction&> transaction
		){
			// Dates preparation
			stringstream datesStr;
			if(object->getCalendarLinks().empty())
			{
				object->serialize(datesStr);
			}

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
			//TODO test if the user has sufficient right level for this service
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(DELETE_RIGHT, UNKNOWN_RIGHT_LEVEL, string());
		}



		template<> void DBTableSyncTemplate<ContinuousServiceTableSync>::BeforeDelete(
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
				query << " AND l." << JourneyPatternCommercialLine::FIELD.name << "=" << *commercialLineId;
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
