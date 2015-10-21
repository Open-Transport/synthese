
//////////////////////////////////////////////////////////////////////////
///	FreeDRTTimeSlotTableSync class implementation.
///	@file FreeDRTTimeSlotTableSync.cpp
///	@author Hugues Romain
///	@date 2011
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include <sstream>

#include "FreeDRTTimeSlotTableSync.hpp"

#include "CalendarLink.hpp"
#include "CalendarLinkTableSync.hpp"
#include "CityTableSync.h"
#include "CommercialLineTableSync.h"
#include "FreeDRTAreaTableSync.hpp"
#include "Profile.h"
#include "PTUseRuleTableSync.h"
#include "ReplaceQuery.h"
#include "RollingStock.hpp"
#include "SelectQuery.hpp"
#include "TransportNetworkRight.h"
#include "User.h"

#include <boost/algorithm/string/split.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;
using namespace boost::posix_time;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace security;
	using namespace pt;
	using namespace geography;
	using namespace graph;
	using namespace calendar;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,FreeDRTTimeSlotTableSync>::FACTORY_KEY("35.81 Free DRT Time slots");
		template<> const string FactorableTemplate<Fetcher<Calendar>, FreeDRTTimeSlotTableSync>::FACTORY_KEY("83");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<FreeDRTTimeSlotTableSync>::TABLE(
			"t083_free_drt_time_slots"
		);



		template<> const Field DBTableSyncTemplate<FreeDRTTimeSlotTableSync>::_FIELDS[]=
		{
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<FreeDRTTimeSlotTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(
					Area::FIELD.name.c_str(),
			"")	);
			return r;
		};



		template<> bool DBTableSyncTemplate<FreeDRTTimeSlotTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return true;
		}



		template<> void DBTableSyncTemplate<FreeDRTTimeSlotTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<FreeDRTTimeSlotTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<FreeDRTTimeSlotTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
		}

	}



	namespace pt
	{
		FreeDRTTimeSlotTableSync::SearchResult FreeDRTTimeSlotTableSync::Search(
			util::Env& env,
			boost::optional<util::RegistryKeyType> areaId,
			size_t first /*= 0*/,
			optional<size_t> number /*= boost::optional<std::size_t>()*/,
			bool orderByServiceNumber,
			bool raisingOrder,
			util::LinkLevel linkLevel
		){
			SelectQuery<FreeDRTTimeSlotTableSync> query;
			if(areaId)
			{
			 	query.addWhereField(Area::FIELD.name, *areaId);
			}
			if(orderByServiceNumber)
			{
			 	query.addOrderField(FreeDRTServiceNumber::FIELD.name, raisingOrder);
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

		bool FreeDRTTimeSlotTableSync::allowList(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(security::READ);
		}
}	}
