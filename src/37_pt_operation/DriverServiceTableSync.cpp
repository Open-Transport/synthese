
//////////////////////////////////////////////////////////////////////////
///	DriverServiceTableSync class implementation.
///	@file DriverServiceTableSync.cpp
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

#include "DriverServiceTableSync.hpp"

#include "DataSourceLinksField.hpp"
#include "DriverActivityTableSync.hpp"
#include "OperationUnitTableSync.hpp"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "ImportableTableSync.hpp"
#include "ScheduledServiceTableSync.h"
#include "DeadRunTableSync.hpp"
#include "DeadRun.hpp"
#include "VehicleServiceTableSync.hpp"

#include <sstream>
#include <boost/foreach.hpp>
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
	using namespace pt_operation;
	using namespace impex;
	using namespace pt;
	using namespace calendar;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync, DriverServiceTableSync>::FACTORY_KEY("37.40 Driver services");
		template<> const string FactorableTemplate<Fetcher<Calendar>, DriverServiceTableSync>::FACTORY_KEY("81");
	}

	namespace pt_operation
	{
		const string DriverServiceTableSync::COL_NAME = "name";
		const string DriverServiceTableSync::COL_SERVICES = "services";
		const string DriverServiceTableSync::COL_DATES = "dates";
		const string DriverServiceTableSync::COL_DATASOURCE_LINKS = "datasource_links";
		const string DriverServiceTableSync::COL_OPERATION_UNIT_ID = "operation_unit_id";
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<DriverServiceTableSync>::TABLE(
			"t081_driver_services"
		);



		template<> const Field DBTableSyncTemplate<DriverServiceTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(DriverServiceTableSync::COL_NAME, SQL_TEXT),
			Field(DriverServiceTableSync::COL_SERVICES, SQL_TEXT),
			Field(DriverServiceTableSync::COL_DATES, SQL_TEXT),
			Field(DriverServiceTableSync::COL_DATASOURCE_LINKS, SQL_TEXT),
			Field(DriverServiceTableSync::COL_OPERATION_UNIT_ID, SQL_INTEGER),
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<DriverServiceTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(DBTableSync::Index(DriverServiceTableSync::COL_OPERATION_UNIT_ID.c_str(), ""));
			return r;
		}



		template<> void OldLoadSavePolicy<DriverServiceTableSync,DriverService>::Load(
			DriverService* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			// Dates
			object->setFromSerializedString(rows->getText(DriverServiceTableSync::COL_DATES));

			// Name
			object->setName(rows->getText(DriverServiceTableSync::COL_NAME));

			if(linkLevel >= UP_LINKS_LOAD_LEVEL)
			{
				// Services
				object->setChunks(
					DriverServiceTableSync::UnserializeServices(
						rows->getText(DriverServiceTableSync::COL_SERVICES),
						env,
						linkLevel
				)	);
				BOOST_FOREACH(const DriverService::Chunk& chunk, object->getChunks())
				{
					if(chunk.vehicleService)
					{
						chunk.vehicleService->addDriverServiceChunk(chunk);
					}
				}

				// Data sources and operator codes
				object->setDataSourceLinksWithRegistration(
					ImportableTableSync::GetDataSourceLinksFromSerializedString(
						rows->getText(DriverServiceTableSync::COL_DATASOURCE_LINKS),
						env
				)	);

				// Operation unit
				{
					optional<OperationUnit&> value;
					RegistryKeyType unitId(rows->getLongLong(OperationUnit::FIELD.name));
					if(unitId) try
					{
						value = *OperationUnitTableSync::GetEditable(unitId, env, linkLevel);
					}
					catch(ObjectNotFoundException<OperationUnit>&)
					{
						Log::GetInstance().warn("Bad operation unit "+ lexical_cast<string>(unitId) +" in driver service "+ lexical_cast<string>(object->getKey()));
					}
					object->setOperationUnit(value);
				}
			}
		}



		template<> void OldLoadSavePolicy<DriverServiceTableSync,DriverService>::Save(
			DriverService* object,
			optional<DBTransaction&> transaction
		){
			// Dates preparation
			stringstream datesStr;
			object->serialize(datesStr);

			ReplaceQuery<DriverServiceTableSync> query(*object);
			query.addField(object->getName());
			query.addField(DriverServiceTableSync::SerializeServices(object->getChunks()));
			query.addField(datesStr.str());
			query.addField(
				DataSourceLinks::Serialize(
					object->getDataSourceLinks()
			)	);
			query.addField(
				object->getOperationUnit() ?
				object->getOperationUnit()->getKey() :
				0
			);
			query.execute(transaction);
		}



		template<> void OldLoadSavePolicy<DriverServiceTableSync,DriverService>::Unlink(
			DriverService* obj
		){
			BOOST_FOREACH(const DriverService::Chunk& chunk, obj->getChunks())
			{
				if(chunk.vehicleService)
				{
					chunk.vehicleService->removeDriverServiceChunk(chunk);
				}
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
			boost::optional<std::string> searchName,
			boost::optional<util::RegistryKeyType> searchUnit,
			size_t first /*= 0*/,
			optional<size_t> number /*= boost::optional<std::size_t>()*/,
			bool orderByName,
			bool raisingOrder,
			util::LinkLevel linkLevel
		){
			SelectQuery<DriverServiceTableSync> query;
			if(searchName)
			{
				query.addWhereField(COL_NAME, *searchName, ComposedExpression::OP_LIKE);
			}
			if(searchUnit)
			{
				query.addWhereField(COL_OPERATION_UNIT_ID, *searchUnit);
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



		std::string DriverServiceTableSync::SerializeServices( const DriverService::Chunks& services )
		{
			stringstream servicesStr;
			bool firstService(true);
			BOOST_FOREACH(const DriverService::Chunk& chunk, services)
			{
				bool firstElement(true);
				if(chunk.elements.empty())
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
						"0:0:0:" <<
						(chunk.activity ? chunk.activity->getKey() : RegistryKeyType(0))
					;
					if(	!chunk.driverStartTime.is_not_a_date_time() &&
						!chunk.driverEndTime.is_not_a_date_time()
					){
						servicesStr << ":" << to_simple_string(chunk.driverStartTime);
						servicesStr << ":" << to_simple_string(chunk.driverEndTime);
					}
				}
				else
				{
					BOOST_FOREACH(const DriverService::Chunk::Element& service, chunk.elements)
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
						if(firstElement)
						{
							firstElement = false;
							servicesStr <<
								":" <<
								(chunk.vehicleService ? chunk.vehicleService->getKey() : RegistryKeyType(0))
							;
							if(	!chunk.driverStartTime.is_not_a_date_time() &&
								!chunk.driverEndTime.is_not_a_date_time()
							){
								servicesStr << ":" << to_simple_string(chunk.driverStartTime);
								servicesStr << ":" << to_simple_string(chunk.driverEndTime);
							}
						}
			}	}	}
			return servicesStr.str();
		}



		DriverService::Chunks DriverServiceTableSync::UnserializeServices(
			const std::string& value,
			util::Env& env,
			util::LinkLevel linkLevel /*= util::UP_LINKS_LOAD_LEVEL */
		){
			vector<string> servicesStrs;
			if(!value.empty())
			{
				split(servicesStrs, value, is_any_of(","));
			}

			DriverService::Chunks services;

			DriverService::Chunks::reverse_iterator itServices(services.rend());
			BOOST_FOREACH(const string& elementStr, servicesStrs)
			{
				if(elementStr.empty())
				{
					continue;
				}
				vector<string> elementStrs;
				split(elementStrs, elementStr, is_any_of(":"));

				if(elementStrs.size() < 3)
				{
					continue;
				}

				if(elementStrs.size() >= 4 || itServices == services.rend())
				{
					DriverService::Chunk chunk;
					if(	elementStrs.size() >= 4)
					{
						RegistryKeyType activityVehicleServiceId(
							lexical_cast<RegistryKeyType>(elementStrs[3])
						);
						if(activityVehicleServiceId > 0)
						{
							if(decodeTableId(activityVehicleServiceId) == VehicleServiceTableSync::TABLE.ID) try
							{
								chunk.vehicleService = VehicleServiceTableSync::GetEditable(
									activityVehicleServiceId,
									env,
									linkLevel
								).get();
							}
							catch (ObjectNotFoundException<VehicleService>&)
							{
								Log::GetInstance().warn("No such vehicle service "+ elementStrs[3]);
							}
							else if(decodeTableId(activityVehicleServiceId) == DriverActivityTableSync::TABLE.ID) try
							{
								chunk.activity = DriverActivityTableSync::GetEditable(
									activityVehicleServiceId,
									env,
									linkLevel
								).get();
							}
							catch (ObjectNotFoundException<DriverActivity>&)
							{
								Log::GetInstance().warn("No such activity "+ elementStrs[3]);
							}
					}	}
					if(elementStrs.size() >= 9)
					{
						chunk.driverStartTime =
							hours(lexical_cast<long>(elementStrs[4])) +
							minutes(lexical_cast<long>(elementStrs[5]));
						chunk.driverEndTime =
							hours(lexical_cast<long>(elementStrs[7])) +
							minutes(lexical_cast<long>(elementStrs[8]));
					}
					services.push_back(chunk);
					itServices = services.rbegin();
				}

				try
				{
					if(lexical_cast<RegistryKeyType>(elementStrs[0]) > 0)
					{
						DriverService::Chunk::Element element;

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

						itServices->elements.push_back(element);
				}	}
				catch(ObjectNotFoundException<ScheduledService>&)
				{
					Log::GetInstance().warn("No such service "+ elementStrs[0]);
				}
				catch(ObjectNotFoundException<DeadRun>&)
				{
					Log::GetInstance().warn("No such dead run "+ elementStrs[0]);
				}
				catch(bad_lexical_cast&)
				{
					Log::GetInstance().warn("Inconsistent service id "+ elementStrs[0]);
				}
			}

			return services;
		}
}	}
