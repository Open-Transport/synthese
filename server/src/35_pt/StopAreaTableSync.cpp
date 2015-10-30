////////////////////////////////////////////////////////////////////////////////
/// StopAreaTableSync class implementation.
///	@file StopAreaTableSync.cpp
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

#include "StopAreaTableSync.hpp"

#include "DataSourceLinksField.hpp"
#include "Profile.h"
#include "ReplaceQuery.h"
#include "Session.h"
#include "User.h"
#include "CityTableSync.h"
#include "SelectQuery.hpp"
#include "ImportableTableSync.hpp"
#include "StopPointTableSync.hpp"
#include "TransportNetworkRight.h"
#include "PTModule.h"
#include "PTUseRuleTableSync.h"
#include "AllowedUseRule.h"
#include "ForbiddenUseRule.h"

#include <geos/geom/Point.h>

#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <assert.h>

using namespace std;
using namespace boost;
using namespace boost::algorithm;
using namespace geos::geom;

namespace synthese
{
	using namespace db;
	using namespace pt;
	using namespace util;
	using namespace geography;
	using namespace road;
	using namespace impex;
	using namespace security;
	using namespace graph;

	template<> const string util::FactorableTemplate<DBTableSync,pt::StopAreaTableSync>::FACTORY_KEY("35.40.01 Connection places");
	template<> const string FactorableTemplate<Fetcher<NamedPlace>, StopAreaTableSync>::FACTORY_KEY("7");

	namespace pt
	{
		const string StopAreaTableSync::FORBIDDEN_DELAY_SYMBOL = "F";
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<StopAreaTableSync>::TABLE(
			"t007_connection_places"
		);

		template<> const Field DBTableSyncTemplate<StopAreaTableSync>::_FIELDS[] =
		{
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<StopAreaTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(DBTableSync::Index(pt::CityId::FIELD.name.c_str(), SimpleObjectFieldDefinition<Name>::FIELD.name.c_str(), ""));
			r.push_back(DBTableSync::Index(StopAreaDataSource::FIELD.name.c_str(), ""));
			return r;
		}



		template<> bool DBTableSyncTemplate<StopAreaTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(DELETE_RIGHT);
		}



		template<> void DBTableSyncTemplate<StopAreaTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
			Env env;
			StopPointTableSync::SearchResult stops(StopPointTableSync::Search(env, id));
			BOOST_FOREACH(const StopPointTableSync::SearchResult::value_type& stop, stops)
			{
				StopPointTableSync::Remove(NULL, stop->getKey(), transaction, false);
			}
		}



		template<> void DBTableSyncTemplate<StopAreaTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<StopAreaTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace pt
	{
		StopAreaTableSync::SearchResult StopAreaTableSync::Search(
			Env& env,
			optional<RegistryKeyType> cityId,
			logic::tribool mainConnectionOnly,
			optional<string> creatorIdFilter,
			optional<string> nameFilter,
			optional<string> cityNameFilter,
			bool orderByCityNameAndName /*= true */
			, bool raisingOrder /*= true */
			, int first /*= 0 */
			, int number /*= 0 */,
			LinkLevel linkLevel
		){
			SelectQuery<StopAreaTableSync> query;
			if(orderByCityNameAndName || cityNameFilter)
			{
				query.addTableAndEqualJoin<CityTableSync>(TABLE_COL_ID, pt::CityId::FIELD.name);
			}

			// Filters
			if (cityId)
			{
				query.addWhereField(pt::CityId::FIELD.name, *cityId);
			}
			if (!logic::indeterminate(mainConnectionOnly))
			{
				query.addWhereField(IsCityMainConnection::FIELD.name, mainConnectionOnly);
			}
			if(creatorIdFilter)
			{
				query.addWhereField(StopAreaDataSource::FIELD.name, *creatorIdFilter);
			}
			if(nameFilter)
			{
				query.addWhereField(SimpleObjectFieldDefinition<Name>::FIELD.name, *nameFilter, ComposedExpression::OP_LIKE);
			}
			if(cityNameFilter)
			{
				query.addWhereFieldOther<CityTableSync>(CityTableSync::TABLE_COL_NAME, *cityNameFilter, ComposedExpression::OP_LIKE);
			}

			// Ordering
			if(orderByCityNameAndName)
			{
				query.addOrderFieldOther<CityTableSync>(SimpleObjectFieldDefinition<Name>::FIELD.name, raisingOrder);
				query.addOrderField(SimpleObjectFieldDefinition<Name>::FIELD.name, raisingOrder);
			}
			if (number > 0)
			{
				query.setNumber(number + 1);
				if (first > 0)
				{
					query.setFirst(first);
				}
			}

			return LoadFromQuery(query, env, linkLevel);
		}

	
	
		StopAreaTableSync::SearchResult StopAreaTableSync::SearchSameName(
			StopArea const& stoparea, 
			Env& env,
			LinkLevel linkLevel
		){
			SelectQuery<StopAreaTableSync> query;
			std::stringstream subQuery;

			if(stoparea.getCity())
			{
				subQuery << stoparea.getKey() << " != t007_connection_places.id AND "
					<< "\"" << stoparea.getName() << "\"" << " = t007_connection_places.name AND " 
					<< stoparea.getCity()->getKey() << " = t007_connection_places.city_id";
			}
			else
			{
				subQuery << stoparea.getKey() << " != t007_connection_places.id AND "
				<< "\"" << stoparea.getName() << "\"" << " = t007_connection_places.name"; 
			}
			
			query.addWhere(
				SubQueryExpression::Get(subQuery.str())
			);

			return LoadFromQuery(query, env, linkLevel);
		}

		bool StopAreaTableSync::allowList(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(security::READ);
		}
	}
}
