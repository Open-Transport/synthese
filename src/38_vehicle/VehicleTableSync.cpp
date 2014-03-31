
//////////////////////////////////////////////////////////////////////////
///	VehicleTableSync class implementation.
///	@file VehicleTableSync.cpp
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

#include "VehicleTableSync.hpp"

#include "DataSourceLinksField.hpp"
#include "ImportableTableSync.hpp"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "Vehicle.hpp"

#include <sstream>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/split.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;

namespace synthese
{
	using namespace db;
	using namespace impex;
	using namespace pt;
	using namespace util;
	using namespace vehicle;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,VehicleTableSync>::FACTORY_KEY("37.01 Vehicles");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<VehicleTableSync>::TABLE(
			"t069_vehicles"
		);



		template<> const Field DBTableSyncTemplate<VehicleTableSync>::_FIELDS[]=
		{
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<VehicleTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		template<> bool DBTableSyncTemplate<VehicleTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO Check tue user rights
			return true;
		}



		template<> void DBTableSyncTemplate<VehicleTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<VehicleTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<VehicleTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}



	namespace vehicle
	{
		VehicleTableSync::SearchResult VehicleTableSync::Search(
			util::Env& env,
			boost::optional<std::string> name,
			boost::optional<std::string> vehicleNumber,
			boost::optional<std::string> registration,
			size_t first /*= 0*/,
			optional<size_t> number /*= boost::optional<std::size_t>()*/,
			bool orderByName,
			bool orderByNumber,
			bool orderByRegistration,
			bool raisingOrder,
			util::LinkLevel linkLevel
		){
			SelectQuery<VehicleTableSync> query;
			if(name)
			{
				query.addWhereField(Name::FIELD.name, "%"+ *name +"%", ComposedExpression::OP_LIKE);
			}
			if(vehicleNumber)
			{
				query.addWhereField(Number::FIELD.name, "%"+ *vehicleNumber +"%", ComposedExpression::OP_LIKE);
			}
			if(registration)
			{
				query.addWhereField(RegistrationNumber::FIELD.name, "%"+ *registration +"%", ComposedExpression::OP_LIKE);
			}
			if(orderByName)
			{
				query.addOrderField(Name::FIELD.name, raisingOrder);
			}
			else if(orderByNumber)
			{
				query.addOrderField(Number::FIELD.name, raisingOrder);
			}
			else if(orderByRegistration)
			{
				query.addOrderField(RegistrationNumber::FIELD.name, raisingOrder);
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



		VehicleTableSync::VehiclesList VehicleTableSync::GetVehiclesList(
			util::Env& env,
			optional<string> noVehicleLabel
		){
			VehiclesList result;
			if(noVehicleLabel)
			{
				result.push_back(make_pair(optional<RegistryKeyType>(0), *noVehicleLabel));
			}
			SearchResult vehicles(Search(env));
			BOOST_FOREACH(const boost::shared_ptr<Vehicle>& vehicle, vehicles)
			{
				result.push_back(make_pair(optional<RegistryKeyType>(vehicle->getKey()), vehicle->getName()));
			}
			return result;
		}
}	}
