
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

#include <sstream>

#include "VehicleServiceTableSync.hpp"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "ScheduledService.h"
#include "ScheduledServiceTableSync.h"
#include "VehicleService.hpp"
#include "PTOperationModule.hpp"
#include "ImportableTableSync.hpp"
#include "DeadRunTableSync.hpp"
#include "DeadRun.hpp"

#include <boost/foreach.hpp>
#include <boost/algorithm/string/split.hpp>

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

	namespace pt_operation
	{
		const string VehicleServiceTableSync::COL_NAME("name");
		const string VehicleServiceTableSync::COL_SERVICES("services");
		const string VehicleServiceTableSync::COL_DATASOURCE_LINKS("datasource_links");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<VehicleServiceTableSync>::TABLE(
			"t077_vehicle_services"
		);



		template<> const DBTableSync::Field DBTableSyncTemplate<VehicleServiceTableSync>::_FIELDS[]=
		{
			DBTableSync::Field(TABLE_COL_ID, SQL_INTEGER),
			DBTableSync::Field(VehicleServiceTableSync::COL_NAME, SQL_TEXT),
			DBTableSync::Field(VehicleServiceTableSync::COL_SERVICES, SQL_TEXT),
			DBTableSync::Field(VehicleServiceTableSync::COL_DATASOURCE_LINKS, SQL_TEXT),
			DBTableSync::Field()
		};



		template<> const DBTableSync::Index DBTableSyncTemplate<VehicleServiceTableSync>::_INDEXES[]=
		{
			DBTableSync::Index()
		};



		template<> void DBDirectTableSyncTemplate<VehicleServiceTableSync,VehicleService>::Load(
			VehicleService* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			// Name
			object->setName(rows->getText(VehicleServiceTableSync::COL_NAME));

			if(linkLevel >= UP_LINKS_LOAD_LEVEL)
			{
				// Data sources and operator codes
				object->setDataSourceLinksWithRegistration(
					ImportableTableSync::GetDataSourceLinksFromSerializedString(
						rows->getText(VehicleServiceTableSync::COL_DATASOURCE_LINKS),
						env
				)	);

				// Services
				object->setServices(VehicleServiceTableSync::UnserializeServices(rows->getText(VehicleServiceTableSync::COL_SERVICES), env, linkLevel));
			}
		}



		template<> void DBDirectTableSyncTemplate<VehicleServiceTableSync,VehicleService>::Save(
			VehicleService* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<VehicleServiceTableSync> query(*object);
			query.addField(object->getName());
			query.addField(VehicleServiceTableSync::SerializeServices(object->getServices()));
			query.addField(
				ImportableTableSync::SerializeDataSourceLinks(
					object->getDataSourceLinks()
			)	);
			query.execute(transaction);
		}



		template<> void DBDirectTableSyncTemplate<VehicleServiceTableSync,VehicleService>::Unlink(
			VehicleService* obj
		){
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
			size_t first /*= 0*/,
			optional<size_t> number /*= boost::optional<std::size_t>()*/,
			bool orderByName,
			bool raisingOrder,
			util::LinkLevel linkLevel
		){
			SelectQuery<VehicleServiceTableSync> query;
			if(name)
			{
				query.addWhereField(COL_NAME, "%"+ *name +"%", ComposedExpression::OP_LIKE);
			}
			if(orderByName)
			{
				query.addOrderField(COL_NAME, raisingOrder);
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



		std::string VehicleServiceTableSync::SerializeServices( const VehicleService::Services& services )
		{
			stringstream servicesStr;
			bool firstService(true);
			BOOST_FOREACH(const VehicleService::Services::value_type& service, services)
			{
				if(firstService)
				{
					firstService = false;
				}
				else
				{
					servicesStr << ",";
				}
				servicesStr << service->getKey();
			}
			return servicesStr.str();
		}



		VehicleService::Services VehicleServiceTableSync::UnserializeServices(
			const std::string& value,
			Env& env,
			LinkLevel linkLevel
		){
			vector<string> servicesStrs;
			if(!value.empty())
			{
				split(servicesStrs, value, is_any_of(","));
			}

			VehicleService::Services services;

			BOOST_FOREACH(const string& serviceId, servicesStrs)
			{
				try
				{
					RegistryKeyType id(lexical_cast<RegistryKeyType>(serviceId));
					if(decodeTableId(id) == ScheduledServiceTableSync::TABLE.ID)
					{
						services.push_back(
							ScheduledServiceTableSync::GetEditable(id, env, linkLevel).get()
						);
					}
					else if(decodeTableId(id) == DeadRunTableSync::TABLE.ID)
					{
						services.push_back(
							DeadRunTableSync::GetEditable(id, env, linkLevel).get()
						);
					}
				}
				catch(ObjectNotFoundException<ScheduledService>&)
				{
					Log::GetInstance().warn("No such service "+ lexical_cast<string>(serviceId));
				}
				catch(ObjectNotFoundException<DeadRun>&)
				{
					Log::GetInstance().warn("No such dead run "+ lexical_cast<string>(serviceId));
				}
			}

			return services;
		}
}	}
