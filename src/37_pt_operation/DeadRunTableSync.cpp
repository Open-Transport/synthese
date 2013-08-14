
//////////////////////////////////////////////////////////////////////////
///	DeadRunTableSync class implementation.
///	@file DeadRunTableSync.cpp
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

#include "DeadRunTableSync.hpp"

#include "DataSourceLinksField.hpp"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "Depot.hpp"
#include "DepotTableSync.hpp"
#include "StopPoint.hpp"
#include "StopPointTableSync.hpp"
#include "ImportableTableSync.hpp"
#include "TransportNetworkTableSync.h"

#include <sstream>

using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace pt_operation;
	using namespace pt;
	using namespace impex;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync, DeadRunTableSync>::FACTORY_KEY("37.30 Dead runs");
	}

	namespace pt_operation
	{
		const std::string DeadRunTableSync::COL_NETWORK_ID("network_id");
		const std::string DeadRunTableSync::COL_DEPOT_ID("depot_id");
		const std::string DeadRunTableSync::COL_STOP_ID("stop_id");
		const std::string DeadRunTableSync::COL_DIRECTION("direction");
		const std::string DeadRunTableSync::COL_SCHEDULES("schedules");
		const std::string DeadRunTableSync::COL_DATES("dates");
		const std::string DeadRunTableSync::COL_SERVICE_NUMBER("service_number");
		const std::string DeadRunTableSync::COL_LENGTH("length");
		const std::string DeadRunTableSync::COL_DATASOURCE_LINKS("datasource_links");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<DeadRunTableSync>::TABLE(
			"t080_dead_runs"
		);



		template<> const Field DBTableSyncTemplate<DeadRunTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(DeadRunTableSync::COL_NETWORK_ID, SQL_INTEGER),
			Field(DeadRunTableSync::COL_DEPOT_ID, SQL_INTEGER),
			Field(DeadRunTableSync::COL_STOP_ID, SQL_INTEGER),
			Field(DeadRunTableSync::COL_DIRECTION, SQL_BOOLEAN),
			Field(DeadRunTableSync::COL_SCHEDULES, SQL_TEXT),
			Field(DeadRunTableSync::COL_DATES, SQL_TEXT),
			Field(DeadRunTableSync::COL_SERVICE_NUMBER, SQL_TEXT),
			Field(DeadRunTableSync::COL_LENGTH, SQL_DOUBLE),
			Field(DeadRunTableSync::COL_DATASOURCE_LINKS, SQL_TEXT),
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<DeadRunTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(
					DeadRunTableSync::COL_NETWORK_ID.c_str(),
			"")	);
			return r;
		}



		template<> void OldLoadSavePolicy<DeadRunTableSync, DeadRun>::Load(
			DeadRun* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			// Service number
			object->setServiceNumber(rows->getText(DeadRunTableSync::COL_SERVICE_NUMBER));

			if(linkLevel >= UP_LINKS_LOAD_LEVEL)
			{
				// Network
				object->setTransportNetwork(NULL);
				RegistryKeyType id(rows->getLongLong(DeadRunTableSync::COL_NETWORK_ID));
				if(id > 0) try
				{
					object->setTransportNetwork(
						TransportNetworkTableSync::GetEditable(id, env, linkLevel).get()
					);
				}
				catch(ObjectNotFoundException<TransportNetwork>&)
				{
					Log::GetInstance().warn("No such network "+ lexical_cast<string>(id) +" in Dead run "+ lexical_cast<string>(object->getKey()));
				}

				// Depot, stop point, direction, length
				boost::shared_ptr<Depot> depot;
				boost::shared_ptr<StopPoint> stop;
				RegistryKeyType pid(rows->getLongLong(DeadRunTableSync::COL_DEPOT_ID));
				RegistryKeyType stopId(rows->getLongLong(DeadRunTableSync::COL_STOP_ID));
				try
				{
					if(pid > 0)
					{
						depot = DepotTableSync::GetEditable(pid, env, linkLevel);
					}
					if(stopId > 0)
					{
						stop = StopPointTableSync::GetEditable(stopId, env, linkLevel);
					}
				}
				catch(ObjectNotFoundException<Depot>&)
				{
					Log::GetInstance().warn("No such depot "+ lexical_cast<string>(pid) +" in Dead run "+ lexical_cast<string>(object->getKey()));
				}
				catch(ObjectNotFoundException<StopPoint>&)
				{
					Log::GetInstance().warn("No such stop "+ lexical_cast<string>(stopId) +" in Dead run "+ lexical_cast<string>(object->getKey()));
				}
				if(depot.get() && stop.get())
				{
					object->setRoute(
						*depot,
						*stop,
						rows->getDouble(DeadRunTableSync::COL_LENGTH),
						rows->getBool(DeadRunTableSync::COL_DIRECTION)
					);
				}

				// Data source links
				object->setDataSourceLinksWithoutRegistration(
					ImportableTableSync::GetDataSourceLinksFromSerializedString(
						rows->getText(DeadRunTableSync::COL_DATASOURCE_LINKS),
						env
				)	);
			}

			// Schedules
			SchedulesBasedService::SchedulesPair value(
				SchedulesBasedService::DecodeSchedules(
					rows->get<string>(DeadRunTableSync::COL_SCHEDULES)
			)	);
			object->setSchedules(value.first, value.second, true);

			// Dates
			object->setFromSerializedString(rows->getText(DeadRunTableSync::COL_DATES));
		}



		template<> void OldLoadSavePolicy<DeadRunTableSync, DeadRun>::Save(
			DeadRun* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<DeadRunTableSync> query(*object);

			// Network
			query.addField(object->getTransportNetwork() ? object->getTransportNetwork()->getKey() : RegistryKeyType(0));

			// Depot
			Depot* depot(object->getDepot());
			query.addField(depot ? depot->getKey() : RegistryKeyType(0));

			// Stop
			StopPoint* stop(object->getStop());
			query.addField(stop ? stop->getKey() : RegistryKeyType(0));

			// Direction
			query.addField(object->getFromDepotToStop());

			// Schedules
			query.addField(object->encodeSchedules());

			// Dates
			stringstream datesStr;
			object->serialize(datesStr);
			query.addField(datesStr.str());

			// Service number
			query.addField(object->getServiceNumber());

			// Length
			query.addField(object->isUndefined() ? 0 : object->getEdge(1)->getMetricOffset());

			// Data source links
			query.addField(
				DataSourceLinks::Serialize(
					object->getDataSourceLinks()
			)	);

			query.execute(transaction);
		}



		template<> void OldLoadSavePolicy<DeadRunTableSync, DeadRun>::Unlink(
			DeadRun* obj
		){
		}



		template<> bool DBTableSyncTemplate<DeadRunTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return true;
		}



		template<> void DBTableSyncTemplate<DeadRunTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<DeadRunTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<DeadRunTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
		}

	}



	namespace pt_operation
	{
		DeadRunTableSync::SearchResult DeadRunTableSync::Search(
			util::Env& env,
			optional<RegistryKeyType> networkId,
			size_t first /*= 0*/,
			optional<size_t> number /*= boost::optional<std::size_t>()*/,
			bool orderByName,
			bool raisingOrder,
			util::LinkLevel linkLevel
		){
			SelectQuery<DeadRunTableSync> query;
			if(networkId)
			{
				query.addWhereField(COL_NETWORK_ID, *networkId);
			}
			if (number)
			{
				query.setNumber(*number + 1);
			}
			if (first > 0)
			{
				query.setFirst(first);
			}

			return LoadFromQuery(query.toString(), env, linkLevel);
		}
}	}
