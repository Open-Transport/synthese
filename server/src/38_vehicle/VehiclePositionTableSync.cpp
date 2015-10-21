
//////////////////////////////////////////////////////////////////////////
///	VehiclePositionTableSync class implementation.
///	@file VehiclePositionTableSync.cpp
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

#include "VehiclePositionTableSync.hpp"

#include "PtimeField.hpp"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "VehicleTableSync.hpp"
#include "StopPointTableSync.hpp"
#include "ScheduledServiceTableSync.h"
#include "UpdateQuery.hpp"
#include "DepotTableSync.hpp"
#include "StopArea.hpp"
#include "PTUseRule.h"


#include <geos/geom/Point.h>
#include <sstream>

using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace vehicle;
	using namespace pt;
	using namespace pt_operation;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,VehiclePositionTableSync>::FACTORY_KEY("37.20 Vehicle positions");
	}

	namespace vehicle
	{
		const std::string VehiclePositionTableSync::COL_STATUS("status");
		const std::string VehiclePositionTableSync::COL_VEHICLE_ID("vehicle_id");
		const std::string VehiclePositionTableSync::COL_TIME("time");
		const std::string VehiclePositionTableSync::COL_METER_OFFSET("meter_offset");
		const std::string VehiclePositionTableSync::COL_STOP_POINT_ID("stop_point_id");
		const std::string VehiclePositionTableSync::COL_COMMENT("comment");
		const std::string VehiclePositionTableSync::COL_SERVICE_ID("service_id");
		const std::string VehiclePositionTableSync::COL_RANK_IN_PATH("rank_in_path");
		const std::string VehiclePositionTableSync::COL_PASSENGERS("passengers");
		const std::string VehiclePositionTableSync::COL_IN_STOP_AREA = "in_stop_area";
		const std::string VehiclePositionTableSync::COL_STOP_FOUND_TIME = "stop_found_time";
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<VehiclePositionTableSync>::TABLE(
			"t072_vehicle_positions"
		);



		template<> const Field DBTableSyncTemplate<VehiclePositionTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(VehiclePositionTableSync::COL_STATUS, SQL_INTEGER),
			Field(VehiclePositionTableSync::COL_VEHICLE_ID, SQL_INTEGER),
			Field(VehiclePositionTableSync::COL_TIME, SQL_TEXT),
			Field(VehiclePositionTableSync::COL_METER_OFFSET, SQL_DOUBLE),
			Field(VehiclePositionTableSync::COL_STOP_POINT_ID, SQL_INTEGER),
			Field(VehiclePositionTableSync::COL_COMMENT, SQL_TEXT),
			Field(VehiclePositionTableSync::COL_SERVICE_ID, SQL_INTEGER),
			Field(VehiclePositionTableSync::COL_RANK_IN_PATH, SQL_INTEGER),
			Field(VehiclePositionTableSync::COL_PASSENGERS, SQL_INTEGER),
			Field(VehiclePositionTableSync::COL_IN_STOP_AREA, SQL_BOOLEAN),
			Field(VehiclePositionTableSync::COL_STOP_FOUND_TIME, SQL_TEXT),
			Field(TABLE_COL_GEOMETRY, SQL_GEOM_POINT),
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<VehiclePositionTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(
					VehiclePositionTableSync::COL_VEHICLE_ID.c_str(),
					VehiclePositionTableSync::COL_TIME.c_str(),
			"")	);
			return r;
		}



		template<> void OldLoadSavePolicy<VehiclePositionTableSync,VehiclePosition>::Load(
			VehiclePosition* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			object->setStatus(static_cast<VehiclePosition::Status>(rows->getInt(VehiclePositionTableSync::COL_STATUS)));
			object->setTime(rows->getDateTime(VehiclePositionTableSync::COL_TIME));
			object->setMeterOffset(static_cast<VehiclePosition::Meters>(rows->getDouble(VehiclePositionTableSync::COL_METER_OFFSET)));
			object->setComment(rows->getText(VehiclePositionTableSync::COL_COMMENT));
			object->setRankInPath(rows->getOptionalUnsignedInt(VehiclePositionTableSync::COL_RANK_IN_PATH));
			object->setPassangers(rows->getInt(VehiclePositionTableSync::COL_PASSENGERS));
			object->setInStopArea(rows->getBool(VehiclePositionTableSync::COL_IN_STOP_AREA));
			object->setNextStopFoundTime(rows->getDateTime(VehiclePositionTableSync::COL_STOP_FOUND_TIME));


			string pointsStr(rows->getText(TABLE_COL_GEOMETRY));
			if(pointsStr.empty())
			{
				object->setGeometry(boost::shared_ptr<Point>());
			}
			else
			{
				object->setGeometry(
					dynamic_pointer_cast<Point,Geometry>(rows->getGeometryFromWKT(TABLE_COL_GEOMETRY))
				);
			}

			if(linkLevel >= UP_LINKS_LOAD_LEVEL)
			{
				RegistryKeyType pid(rows->getLongLong(VehiclePositionTableSync::COL_VEHICLE_ID));
				if(pid > 0)
				{
					try
					{
						object->setVehicle(VehicleTableSync::GetEditable(pid, env, linkLevel).get());
					}
					catch(ObjectNotFoundException<Vehicle>&)
					{
						Log::GetInstance().warn("No such vehicle "+ lexical_cast<string>(pid) +" in VehiclePosition "+ lexical_cast<string>(object->getKey()));
					}
				}

				RegistryKeyType sid(rows->getLongLong(VehiclePositionTableSync::COL_STOP_POINT_ID));
				if(sid > 0)
				{
					try
					{
						RegistryTableType tableId(decodeTableId(sid));
						if(tableId == StopPointTableSync::TABLE.ID)
						{
							object->setStopPoint(StopPointTableSync::GetEditable(sid, env, linkLevel).get());
						}
						else if(tableId == DepotTableSync::TABLE.ID)
						{
							object->setDepot(DepotTableSync::GetEditable(sid, env, linkLevel).get());
						}
					}
					catch(ObjectNotFoundException<StopPoint>&)
					{
						Log::GetInstance().warn("No such stop point "+ lexical_cast<string>(sid) +" in VehiclePosition "+ lexical_cast<string>(object->getKey()));
					}
					catch(ObjectNotFoundException<Depot>&)
					{
						Log::GetInstance().warn("No such depot "+ lexical_cast<string>(sid) +" in VehiclePosition "+ lexical_cast<string>(object->getKey()));
					}
				}

				RegistryKeyType cid(rows->getLongLong(VehiclePositionTableSync::COL_SERVICE_ID));
				if(cid > 0)
				{
					try
					{
						object->setService(ScheduledServiceTableSync::GetEditable(cid, env, linkLevel).get());
					}
					catch(ObjectNotFoundException<ScheduledService>&)
					{
						Log::GetInstance().warn("No such stop point "+ lexical_cast<string>(cid) +" in VehiclePosition "+ lexical_cast<string>(object->getKey()));
					}
				}
			}
		}



		template<> void OldLoadSavePolicy<VehiclePositionTableSync,VehiclePosition>::Save(
			VehiclePosition* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<VehiclePositionTableSync> query(*object);
			query.addField(static_cast<int>(object->getStatus()));
			query.addField(object->getVehicle() ? object->getVehicle()->getKey() : RegistryKeyType(0));
			query.addFrameworkField<PtimeField>(object->getTime());
			query.addField(object->getMeterOffset());
			if(object->getStopPoint())
			{
				query.addField(object->getStopPoint()->getKey());
			}
			else if(object->getDepot())
			{
				query.addField(object->getDepot()->getKey());
			}
			else
			{
				query.addFieldNull();
			}
			query.addField(object->getComment());
			query.addField(object->getService() ? object->getService()->getKey() : RegistryKeyType(0));
			query.addField(object->getRankInPath() ? lexical_cast<string>(*object->getRankInPath()) : string());
			query.addField(object->getPassengers());
			query.addField(object->getInStopArea());
			query.addFrameworkField<PtimeField>(object->getNextStopFoundTime());
			if(object->hasGeometry())
			{
				query.addField(static_pointer_cast<Geometry,Point>(object->getGeometry()));
			}
			else
			{
				query.addFieldNull();
			}
			query.execute(transaction);
		}



		template<> void OldLoadSavePolicy<VehiclePositionTableSync,VehiclePosition>::Unlink(
			VehiclePosition* obj
		){
			obj->setVehicle(NULL);
			obj->setStopPoint(NULL);
			obj->setService(NULL);
			obj->setDepot(NULL);
		}



		template<> bool DBTableSyncTemplate<VehiclePositionTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO Check the user rights
			return true;
		}



		template<> void DBTableSyncTemplate<VehiclePositionTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<VehiclePositionTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<VehiclePositionTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}



	namespace vehicle
	{
		VehiclePositionTableSync::SearchResult VehiclePositionTableSync::Search(
			util::Env& env,
			boost::optional<util::RegistryKeyType> vehicleId,
			boost::optional<boost::posix_time::ptime> startDate,
			boost::optional<boost::posix_time::ptime> endDate,
			size_t first /*= 0*/,
			optional<size_t> number /*= boost::optional<std::size_t>()*/,
			bool orderByDate,
			bool raisingOrder,
			util::LinkLevel linkLevel
		){
			SelectQuery<VehiclePositionTableSync> query;
			if(startDate)
			{
				query.addWhereField(COL_TIME, *startDate, ComposedExpression::OP_SUPEQ);
			}
			if(endDate)
			{
				query.addWhereField(COL_TIME, *endDate, ComposedExpression::OP_INFEQ);
			}
			if(vehicleId)
			{
				query.addWhereField(COL_VEHICLE_ID, *vehicleId);
			}
			if(orderByDate)
			{
				query.addOrderField(COL_TIME, raisingOrder);
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



		void VehiclePositionTableSync::ChangePassengers(
			const VehiclePosition& startPosition,
			const VehiclePosition& endPosition,
			std::size_t passengersToAdd,
			std::size_t passengersToRemove,
			boost::optional<db::DBTransaction&> transaction
		){
			UpdateQuery<VehiclePositionTableSync> query;
			query.addUpdateField(
				COL_PASSENGERS,
				RawSQL(
					COL_PASSENGERS+ "+" + lexical_cast<string>(passengersToAdd) + "-" + lexical_cast<string>(passengersToRemove)
			)	);
			query.addWhereField(COL_VEHICLE_ID, startPosition.getVehicle()->getKey());
			query.addWhereField(COL_TIME, startPosition.getTime(), ComposedExpression::OP_SUPEQ);
			query.addWhereField(COL_TIME, endPosition.getTime(), ComposedExpression::OP_INF);
			query.execute(transaction);
		}
	}
}
