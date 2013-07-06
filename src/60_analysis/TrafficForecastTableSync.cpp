
//////////////////////////////////////////////////////////////////////////
///	TrafficForecastTableSync class implementation.
///	@file TrafficForecastTableSync.cpp
///	@author hromain
///	@date 2013
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

#include "TrafficForecastTableSync.hpp"

#include "SelectQuery.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace analysis;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,TrafficForecastTableSync>::FACTORY_KEY = "60 Traffic forecasts";
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<TrafficForecastTableSync>::TABLE(
			"t113_traffic_forecasts"
		);



		template<> const Field DBTableSyncTemplate<TrafficForecastTableSync>::_FIELDS[] = { Field() }; // Defined by the record



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<TrafficForecastTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		template<> bool DBTableSyncTemplate<TrafficForecastTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return true;
		}



		template<> void DBTableSyncTemplate<TrafficForecastTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<TrafficForecastTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<TrafficForecastTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
		}

	}
	
	
	
	namespace analysis
	{
		TrafficForecastTableSync::SearchResult TrafficForecastTableSync::Search(
			util::Env& env,
			// boost::optional<util::RegistryKeyType> parameterId /*= boost::optional<util::RegistryKeyType>()*/,
			size_t first /*= 0*/,
			optional<size_t> number /*= boost::optional<std::size_t>()*/,
			bool orderByName,
			bool raisingOrder,
			util::LinkLevel linkLevel
		){
			SelectQuery<TrafficForecastTableSync> query;
			// if(parameterId)
			// {
			// 	query.addWhereField(COL_PARENT_ID, *parentFolderId);
			// }
			// if(orderByName)
			// {
			// 	query.addOrderField(COL_NAME, raisingOrder);
			// }
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

