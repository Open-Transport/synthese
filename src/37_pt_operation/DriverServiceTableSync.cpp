
//////////////////////////////////////////////////////////////////////////
///	DriverServiceTableSync class implementation.
///	@file DriverServiceTableSync.cpp
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

#include <sstream>

#include "DriverServiceTableSync.hpp"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "ImportableTableSync.hpp"
#include "ScheduledServiceTableSync.h"
#include "DeadRunTableSync.hpp"
#include "DeadRun.hpp"
#include "VehicleServiceTableSync.hpp"

#include <boost/foreach.hpp>
#include <boost/algorithm/string/split.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace security;
	using namespace pt_operation;
	using namespace impex;
	using namespace pt;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,DriverServiceTableSync>::FACTORY_KEY("37.40 Driver services");
	}

	namespace pt_operation
	{
		const string DriverServiceTableSync::COL_SERVICES = "services";
		const string DriverServiceTableSync::COL_DATES = "dates";
		const string DriverServiceTableSync::COL_DATASOURCE_LINKS = "datasource_links";
		const string DriverServiceTableSync::COL_VEHICLE_SERVICE_ID = "vehicle_service_id";
	}
	
	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<DriverServiceTableSync>::TABLE(
			"t081_driver_services"
		);



		template<> const DBTableSync::Field DBTableSyncTemplate<DriverServiceTableSync>::_FIELDS[]=
		{
			DBTableSync::Field(TABLE_COL_ID, SQL_INTEGER),
			DBTableSync::Field(DriverServiceTableSync::COL_SERVICES, SQL_TEXT),
			DBTableSync::Field(DriverServiceTableSync::COL_DATES, SQL_TEXT),
			DBTableSync::Field(DriverServiceTableSync::COL_DATASOURCE_LINKS, SQL_TEXT),
			DBTableSync::Field(DriverServiceTableSync::COL_VEHICLE_SERVICE_ID, SQL_INTEGER),
			DBTableSync::Field()
		};



		template<> const DBTableSync::Index DBTableSyncTemplate<DriverServiceTableSync>::_INDEXES[]=
		{
			DBTableSync::Index()
		};



		template<> void DBDirectTableSyncTemplate<DriverServiceTableSync,DriverService>::Load(
			DriverService* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			// Dates
			object->setFromSerializedString(rows->getText(DriverServiceTableSync::COL_DATES));

			if(linkLevel >= UP_LINKS_LOAD_LEVEL)
			{
				// Services
				object->setServices(DriverServiceTableSync::UnserializeServices(rows->getText(DriverServiceTableSync::COL_SERVICES), env, linkLevel));

				// Data sources and operator codes
				object->setDataSourceLinks(
					ImportableTableSync::GetDataSourceLinksFromSerializedString(
						rows->getText(DriverServiceTableSync::COL_DATASOURCE_LINKS),
						env
					),	true
				);

				// Vehicle service
				RegistryKeyType vehicleServiceId(rows->getLongLong(DriverServiceTableSync::COL_VEHICLE_SERVICE_ID));
				object->setVehicleService(NULL);
				if(vehicleServiceId > 0) try
				{
					shared_ptr<VehicleService> vehicleService(
						VehicleServiceTableSync::GetEditable(vehicleServiceId, env, linkLevel)
					);
					object->setVehicleService(
						vehicleService.get()
					);
					vehicleService->addDriverService(*object);
				}
				catch (ObjectNotFoundException<VehicleService>&)
				{
					Log::GetInstance().warn("No such vehicle service "+ lexical_cast<string>(vehicleServiceId) + " in driver service " + lexical_cast<string>(object->getKey()));
				}
				
			}
		}



		template<> void DBDirectTableSyncTemplate<DriverServiceTableSync,DriverService>::Save(
			DriverService* object,
			optional<DBTransaction&> transaction
		){
			// Dates preparation
			stringstream datesStr;
			object->serialize(datesStr);

			ReplaceQuery<DriverServiceTableSync> query(*object);
			query.addField(DriverServiceTableSync::SerializeServices(object->getServices()));
			query.addField(datesStr.str());
			query.addField(ImportableTableSync::SerializeDataSourceLinks(object->getDataSourceLinks()));
			query.addField(object->getVehicleService() ? object->getVehicleService()->getKey() : RegistryKeyType(0));
			query.execute(transaction);
		}



		template<> void DBDirectTableSyncTemplate<DriverServiceTableSync,DriverService>::Unlink(
			DriverService* obj
		){
			if(obj->getVehicleService())
			{
				obj->getVehicleService()->removeDriverService(*obj);
			}
		}



		template<> bool DBTableSyncTemplate<DriverServiceTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return true;
		}



		template<> void DBTableSyncTemplate<DriverServiceTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<DriverServiceTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<DriverServiceTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
		}

	}
	
	
	
	namespace pt_operation
	{
		DriverServiceTableSync::SearchResult DriverServiceTableSync::Search(
			util::Env& env,
			// boost::optional<util::RegistryKeyType> parameterId /*= boost::optional<util::RegistryKeyType>()*/,
			size_t first /*= 0*/,
			optional<size_t> number /*= boost::optional<std::size_t>()*/,
			bool orderByName,
			bool raisingOrder,
			util::LinkLevel linkLevel
		){
			SelectQuery<DriverServiceTableSync> query;
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



		std::string DriverServiceTableSync::SerializeServices( const DriverService::Services& services )
		{
			stringstream servicesStr;
			bool firstService(true);
			BOOST_FOREACH(const DriverService::Element& service, services)
			{
				if(firstService)
				{
					firstService = false;
				}
				else
				{
					servicesStr << ",";
				}
				servicesStr <<
					service.service->getKey() << ":" <<
					service.startRank << ":" <<
					service.endRank
				;
			}
			return servicesStr.str();
		}



		DriverService::Services DriverServiceTableSync::UnserializeServices( const std::string& value, util::Env& env, util::LinkLevel linkLevel /*= util::UP_LINKS_LOAD_LEVEL */ )
		{
			vector<string> servicesStrs;
			if(!value.empty())
			{
				split(servicesStrs, value, is_any_of(","));
			}

			DriverService::Services services;

			BOOST_FOREACH(const string& elementStr, servicesStrs)
			{
				if(elementStr.empty())
				{
					continue;
				}
				vector<string> elementStrs;
				split(elementStrs, elementStr, is_any_of(":"));

				if(elementStrs.size() != 3)
				{
					continue;
				}

				try
				{
					DriverService::Element element;

					RegistryKeyType id(lexical_cast<RegistryKeyType>(elementStrs[0]));
					if(decodeTableId(id) == ScheduledServiceTableSync::TABLE.ID)
					{
						element.service = ScheduledServiceTableSync::GetEditable(id, env, linkLevel).get();
					}
					else if(decodeTableId(id) == DeadRunTableSync::TABLE.ID)
					{
						element.service = DeadRunTableSync::GetEditable(id, env, linkLevel).get();
					}
					element.startRank = lexical_cast<size_t>(elementStrs[1]);
					element.endRank = lexical_cast<size_t>(elementStrs[2]);
					services.push_back(element);
				}
				catch(ObjectNotFoundException<ScheduledService>&)
				{
					Log::GetInstance().warn("No such service "+ elementStrs[0]);
				}
				catch(ObjectNotFoundException<DeadRun>&)
				{
					Log::GetInstance().warn("No such dead run "+ elementStrs[0]);
				}
			}

			return services;
		}
	}
}

