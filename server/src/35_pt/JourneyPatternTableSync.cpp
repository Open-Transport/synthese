
/** JourneyPatternTableSync class implementation.
	@file JourneyPatternTableSync.cpp

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

#include "JourneyPatternTableSync.hpp"

#include "DataSourceLinksField.hpp"
#include "Profile.h"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "Session.h"
#include "User.h"
#include "GraphConstants.h"
#include "CommercialLineTableSync.h"
#include "FareTableSync.hpp"
#include "RollingStockTableSync.hpp"
#include "DataSourceTableSync.h"
#include "PTUseRuleTableSync.h"
#include "PTUseRule.h"
#include "ImportableTableSync.hpp"
#include "ScheduledServiceTableSync.h"
#include "ContinuousServiceTableSync.h"
#include "LineStopTableSync.h"
#include "TransportNetworkRight.h"
#include "DestinationTableSync.hpp"

#include <sstream>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace fare;
	using namespace util;
	using namespace impex;
	using namespace graph;
	using namespace pt;
	using namespace security;
	using namespace vehicle;

	template<> const string util::FactorableTemplate<DBTableSync,JourneyPatternTableSync>::FACTORY_KEY(
		"35.30.01 Journey patterns"
	);

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<JourneyPatternTableSync>::TABLE(
			"t009_lines"
		);
		template<> const Field DBTableSyncTemplate<JourneyPatternTableSync>::_FIELDS[]=
		{
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<JourneyPatternTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(DBTableSync::Index(JourneyPatternCommercialLine::FIELD.name.c_str(), ""));
			r.push_back(DBTableSync::Index(LineDataSource::FIELD.name.c_str(), ""));
			return r;
		}



		template<> bool DBTableSyncTemplate<JourneyPatternTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO test if the user has sufficient right level for this journey pattern
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(DELETE_RIGHT, UNKNOWN_RIGHT_LEVEL, string());
		}



		template<> void DBTableSyncTemplate<JourneyPatternTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
			// Services deletion
			Env env;
			ScheduledServiceTableSync::SearchResult sservices(ScheduledServiceTableSync::Search(env, id));
			BOOST_FOREACH(const ScheduledServiceTableSync::SearchResult::value_type& sservice, sservices)
			{
				ScheduledServiceTableSync::Remove(NULL, sservice->getKey(), transaction, false);
			}

			ContinuousServiceTableSync::SearchResult cservices(ContinuousServiceTableSync::Search(env, id));
			BOOST_FOREACH(const ContinuousServiceTableSync::SearchResult::value_type& cservice, cservices)
			{
				ContinuousServiceTableSync::Remove(NULL, cservice->getKey(), transaction, false);
			}

			// LineStops deletion
			LineStopTableSync::SearchResult edges(LineStopTableSync::Search(env, id));
			BOOST_FOREACH(const LineStopTableSync::SearchResult::value_type& edge, edges)
			{
				DBModule::GetDB()->deleteStmt(edge->getKey(), transaction);
			}
		}



		template<> void DBTableSyncTemplate<JourneyPatternTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<JourneyPatternTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace pt
	{
		JourneyPatternTableSync::SearchResult JourneyPatternTableSync::Search(
			Env& env,
			boost::optional<util::RegistryKeyType> commercialLineId,
			int first, /*= 0*/
			boost::optional<std::size_t> number,
			bool orderByName,
			bool raisingOrder,
			LinkLevel linkLevel,
			boost::optional<bool> wayback
		){
			SelectQuery<JourneyPatternTableSync> query;
			if (commercialLineId)
			{
				query.addWhereField(JourneyPatternCommercialLine::FIELD.name, *commercialLineId);
			}
			if(wayback)
			{
				query.addWhereField(WayBack::FIELD.name, *wayback);
			}
			if (orderByName)
			{
				query.addOrderField(SimpleObjectFieldDefinition<Name>::FIELD.name, raisingOrder);
			}
			if (number)
			{
				query.setNumber(*number + 1);
			}
			if (first > 0)
			{
				query.setFirst(first);
			}
			return LoadFromQuery(query, env, linkLevel);
		}
}	}
