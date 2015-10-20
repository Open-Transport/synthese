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

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<ContinuousServiceTableSync>::TABLE(
			"t017_continuous_services"
		);

		template<> const Field DBTableSyncTemplate<ContinuousServiceTableSync>::_FIELDS[]=
		{
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<ContinuousServiceTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(
					ServicePath::FIELD.name.c_str(),
					ServiceSchedules::FIELD.name.c_str(),
			"")	);
			return r;
		};



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
				query << " INNER JOIN " << JourneyPatternTableSync::TABLE.NAME << " AS l ON l." << TABLE_COL_ID << "=" << ServicePath::FIELD.name;
			}
			query << " WHERE 1 ";
			if (lineId)
			{
				query << " AND " << ServicePath::FIELD.name << "=" << *lineId;
			}
			if (commercialLineId)
			{
				query << " AND l." << JourneyPatternCommercialLine::FIELD.name << "=" << *commercialLineId;
			}
			if (orderByDepartureTime)
			{
				query << " ORDER BY " << ServiceSchedules::FIELD.name << (raisingOrder ? " ASC" : " DESC");
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

		bool ContinuousServiceTableSync::allowList(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(security::READ);
		}
	}
}
