
//////////////////////////////////////////////////////////////////////////
///	FreeDRTAreaTableSync class implementation.
///	@file FreeDRTAreaTableSync.cpp
///	@author Hugues Romain
///	@date 2011
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "FreeDRTAreaTableSync.hpp"

#include "Profile.h"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "Session.h"
#include "User.h"
#include "CommercialLineTableSync.h"
#include "CityTableSync.h"
#include "StopAreaTableSync.hpp"
#include "RollingStockTableSync.hpp"
#include "FreeDRTTimeSlotTableSync.hpp"
#include "TransportNetworkRight.h"
#include "PTUseRuleTableSync.h"

#include <sstream>
#include <boost/algorithm/string/split.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace security;
	using namespace vehicle;
	using namespace pt;
	using namespace geography;
	using namespace graph;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,FreeDRTAreaTableSync>::FACTORY_KEY("35.80 Free DRT Areas");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<FreeDRTAreaTableSync>::TABLE(
			"t082_free_drt_areas"
		);



		template<> const Field DBTableSyncTemplate<FreeDRTAreaTableSync>::_FIELDS[]=
		{
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<FreeDRTAreaTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(
					FreeDRTAreaCommercialLine::FIELD.name.c_str(),
			"")	);
			return r;
		}



		template<> bool DBTableSyncTemplate<FreeDRTAreaTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(DELETE_RIGHT);
		}



		template<> void DBTableSyncTemplate<FreeDRTAreaTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
			// Services deletion
			Env env;
			FreeDRTTimeSlotTableSync::SearchResult sservices(FreeDRTTimeSlotTableSync::Search(env, id));
			BOOST_FOREACH(const FreeDRTTimeSlotTableSync::SearchResult::value_type& sservice, sservices)
			{
				FreeDRTTimeSlotTableSync::Remove(NULL, sservice->getKey(), transaction, false);
			}
		}



		template<> void DBTableSyncTemplate<FreeDRTAreaTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<FreeDRTAreaTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
		}

	}



	namespace pt
	{
		FreeDRTAreaTableSync::SearchResult FreeDRTAreaTableSync::Search(
			util::Env& env,
			boost::optional<util::RegistryKeyType> lineId,
			size_t first /*= 0*/,
			optional<size_t> number /*= boost::optional<std::size_t>()*/,
			bool orderByName,
			bool raisingOrder,
			util::LinkLevel linkLevel
		){
			SelectQuery<FreeDRTAreaTableSync> query;
			if(lineId)
			{
			 	query.addWhereField(FreeDRTAreaCommercialLine::FIELD.name, *lineId);
			}
			if(orderByName)
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



		bool FreeDRTAreaTableSync::allowList(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(security::READ);
		}
}	}
