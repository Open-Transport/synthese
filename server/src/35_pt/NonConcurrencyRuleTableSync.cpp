
/** NonConcurrencyRuleTableSync class implementation.
	@file NonConcurrencyRuleTableSync.cpp
	@author Hugues Romain
	@date 2008

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

#include "NonConcurrencyRuleTableSync.h"

#include "NonConcurrencyRule.h"
#include "Profile.h"
#include "Session.h"
#include "User.h"
#include "CommercialLineTableSync.h"
#include "CommercialLine.h"
#include "TransportNetworkTableSync.h"
#include "DBModule.h"
#include "DBResult.hpp"
#include "DBException.hpp"
#include "LinkException.h"
#include "ReplaceQuery.h"
#include "TransportNetworkRight.h"

#include <sstream>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace pt;
	using namespace db;
	using namespace util;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,NonConcurrencyRuleTableSync>::FACTORY_KEY("35.25.02 Non concurrency rules");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<NonConcurrencyRuleTableSync>::TABLE(
			"t056_non_concurrency_rules"
		);

		template<> const Field DBTableSyncTemplate<NonConcurrencyRuleTableSync>::_FIELDS[]=
		{
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<NonConcurrencyRuleTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		template<> bool DBTableSyncTemplate<NonConcurrencyRuleTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(DELETE_RIGHT);
		}



		template<> void DBTableSyncTemplate<NonConcurrencyRuleTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<NonConcurrencyRuleTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<NonConcurrencyRuleTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}



	namespace pt
	{
		NonConcurrencyRuleTableSync::SearchResult NonConcurrencyRuleTableSync::Search(
			Env& env,
			optional<RegistryKeyType> hiddenLineId,
			optional<RegistryKeyType> priorityLineId
			, bool hiddenAndPriority
			, int first /*= 0*/
			, boost::optional<std::size_t> number  /*= 0*/
			, bool orderByPriorityLine //= true
			, bool orderByHiddenLine //= false
			, bool orderByDelay //= false
			, bool raisingOrder, //= true
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME;
			if(orderByPriorityLine)
			{
				query <<
					" INNER JOIN " << CommercialLineTableSync::TABLE.NAME << " c ON c." << TABLE_COL_ID << "=" << TABLE.NAME << "." << PriorityLine::FIELD.name
				;
			}
			else if(orderByHiddenLine)
			{
				query <<
					" INNER JOIN " << CommercialLineTableSync::TABLE.NAME << " c ON c." << TABLE_COL_ID << "=" << TABLE.NAME << "." << HiddenLine::FIELD.name
				;
			}
			query
				<< " WHERE 1 ";
			if (priorityLineId || hiddenLineId)
				query << " AND (";
			if (priorityLineId)
				query << PriorityLine::FIELD.name << "=" << *priorityLineId;
			if (priorityLineId && hiddenLineId)
				query << (hiddenAndPriority ? " AND " : " OR ");
			if (hiddenLineId)
				query << HiddenLine::FIELD.name << "=" << *hiddenLineId;
			if (priorityLineId || hiddenLineId)
				query << ")";

			if(orderByPriorityLine || orderByHiddenLine)
			{
				query <<
					" ORDER BY c." << ShortName::FIELD.name << (raisingOrder ? " ASC" : " DESC")
				;
			}
			else if(orderByDelay)
				query << " ORDER BY " << Delay::FIELD.name << (raisingOrder ? " ASC" : " DESC");

			if (number)
			{
				query << " LIMIT " << (*number + 1);
				if (first > 0)
					query << " OFFSET " << first;
			}

			return LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
