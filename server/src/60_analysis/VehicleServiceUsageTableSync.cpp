
//////////////////////////////////////////////////////////////////////////
///	VehicleServiceUsageTableSync class implementation.
///	@file VehicleServiceUsageTableSync.cpp
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

#include "VehicleServiceUsageTableSync.hpp"

#include "DataSourceLinksField.hpp"
#include "OperationUnit.hpp"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "VehicleService.hpp"
#include "PTOperationModule.hpp"
#include "ImportableTableSync.hpp"

#include <boost/foreach.hpp>
#include <boost/algorithm/string/split.hpp>
#include <sstream>

using namespace std;
using namespace boost;
using namespace boost::algorithm;


namespace synthese
{
	using namespace analysis;
	using namespace db;
	using namespace util;
	using namespace pt_operation;
	using namespace pt;
	using namespace impex;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,VehicleServiceUsageTableSync>::FACTORY_KEY("60.01 Vehicle service usages");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<VehicleServiceUsageTableSync>::TABLE(
			"t117_vehicle_service_usages"
		);



		template<> const Field DBTableSyncTemplate<VehicleServiceUsageTableSync>::_FIELDS[]=
		{
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<VehicleServiceUsageTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(
					Date::FIELD.name.c_str(),
					VehicleService::FIELD.name.c_str(),
					""
			)	);
			return r;
		}



		template<> bool DBTableSyncTemplate<VehicleServiceUsageTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO Check tue user rights
			return true;
		}



		template<> void DBTableSyncTemplate<VehicleServiceUsageTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<VehicleServiceUsageTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<VehicleServiceUsageTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}



	namespace analysis
	{
		VehicleServiceUsageTableSync::SearchResult VehicleServiceUsageTableSync::Search(
			util::Env& env,
			boost::gregorian::date day,
			boost::optional<pt_operation::VehicleService&> vehicleService,
			size_t first /*= 0*/,
			bool orderByDay,
			bool raisingOrder,
			optional<size_t> number /*= boost::optional<std::size_t>()*/,
			util::LinkLevel linkLevel
		){
			SelectQuery<VehicleServiceUsageTableSync> query;
			if(!day.is_not_a_date())
			{
				query.addWhereField(Date::FIELD.name, to_simple_string(day));
			}
			if(vehicleService)
			{
				query.addWhereField(VehicleService::FIELD.name, vehicleService->get<Key>());
			}
			if(orderByDay)
			{
				query.addOrderField(Date::FIELD.name, raisingOrder);
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
