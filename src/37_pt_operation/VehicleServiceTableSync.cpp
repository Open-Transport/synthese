
//////////////////////////////////////////////////////////////////////////
///	VehicleServiceTableSync class implementation.
///	@file VehicleServiceTableSync.cpp
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

#include "VehicleServiceTableSync.hpp"

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
	using namespace db;
	using namespace util;
	using namespace pt_operation;
	using namespace pt;
	using namespace impex;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,VehicleServiceTableSync>::FACTORY_KEY("37.05 Vehicle services");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<VehicleServiceTableSync>::TABLE(
			"t077_vehicle_services"
		);



		template<> const Field DBTableSyncTemplate<VehicleServiceTableSync>::_FIELDS[]=
		{
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<VehicleServiceTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(DBTableSync::Index(OperationUnit::FIELD.name.c_str(), ""));
			return r;
		}



		template<> bool DBTableSyncTemplate<VehicleServiceTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO Check tue user rights
			return true;
		}



		template<> void DBTableSyncTemplate<VehicleServiceTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<VehicleServiceTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<VehicleServiceTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}



	namespace pt_operation
	{
		VehicleServiceTableSync::SearchResult VehicleServiceTableSync::Search(
			util::Env& env,
			boost::optional<std::string> name,
			boost::optional<util::RegistryKeyType> searchUnit,
			size_t first /*= 0*/,
			optional<size_t> number /*= boost::optional<std::size_t>()*/,
			bool orderByName,
			bool raisingOrder,
			util::LinkLevel linkLevel
		){
			SelectQuery<VehicleServiceTableSync> query;
			if(name)
			{
				query.addWhereField(Name::FIELD.name, "%"+ *name +"%", ComposedExpression::OP_LIKE);
			}
			if(searchUnit)
			{
				query.addWhereField(OperationUnit::FIELD.name, *searchUnit);
			}
			if(orderByName)
			{
				query.addOrderField(Name::FIELD.name, raisingOrder);
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
