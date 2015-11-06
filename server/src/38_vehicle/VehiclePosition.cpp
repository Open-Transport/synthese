/** VehiclePosition class implementation.
	@file VehiclePosition.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "VehiclePosition.hpp"

#include "CoordinatesSystem.hpp"
#include "Depot.hpp"
#include "DepotTableSync.hpp"
#include "ParametersMap.h"
#include "Profile.h"
#include "PTUseRule.h"
#include "ScheduledService.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "StopPointTableSync.hpp"
#include "User.h"
#include "Vehicle.hpp"

#include <geos/geom/Point.h>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace vehicle;

	CLASS_DEFINITION(VehiclePosition, "t072_vehicle_positions", 72)
	FIELD_DEFINITION_OF_OBJECT(VehiclePosition, "vehicle_position_id", "vehicle_position_ids")

	FIELD_DEFINITION_OF_TYPE(VehiclePositionStatus, "status", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(VehiclePositionVehicle, "vehicle_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(MeterOffset, "meter_offset", SQL_DOUBLE)
	FIELD_DEFINITION_OF_TYPE(StopPointId, "stop_point_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(Comment, "comment", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(VehiclePositionService, "service_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(VehiclePositionRankInPath, "rank_in_path", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(Passengers, "passengers", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(InStopArea, "in_stop_area", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(NextStopFoundTime, "stop_found_time", SQL_TIME)

	namespace vehicle
	{
		const string VehiclePosition::ATTR_X = "x";
		const string VehiclePosition::ATTR_Y = "y";
		const string VehiclePosition::ATTR_METER_OFFSET = "meter_offset";
		const string VehiclePosition::ATTR_STATUS = "status";
		const string VehiclePosition::ATTR_VEHICLE_NUMBER = "vehicle_number";
		const string VehiclePosition::TAG_STOP = "stop";
		const string VehiclePosition::ATTR_IN_STOP_AREA = "in_stop_area";
		const string VehiclePosition::ATTR_STOP_FOUND_TIME = "next_stop_found_time";
		const string VehiclePosition::TAG_SERVICE = "service";



		VehiclePosition::VehiclePosition(
			RegistryKeyType id
		):	Registrable(id),
			Object<VehiclePosition, VehiclePositionSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_VALUE_CONSTRUCTOR(VehiclePositionStatus, UNKNOWN_STATUS),
					FIELD_DEFAULT_CONSTRUCTOR(VehiclePositionVehicle),
					FIELD_VALUE_CONSTRUCTOR(Time, not_a_date_time),
					FIELD_VALUE_CONSTRUCTOR(MeterOffset, 0),
					FIELD_DEFAULT_CONSTRUCTOR(StopPointId),
					FIELD_DEFAULT_CONSTRUCTOR(Comment),
					FIELD_DEFAULT_CONSTRUCTOR(VehiclePositionService),
					FIELD_DEFAULT_CONSTRUCTOR(VehiclePositionRankInPath),
					FIELD_VALUE_CONSTRUCTOR(Passengers, 0),
					FIELD_VALUE_CONSTRUCTOR(InStopArea, false),
					FIELD_VALUE_CONSTRUCTOR(NextStopFoundTime, not_a_date_time),
					FIELD_DEFAULT_CONSTRUCTOR(PointGeometry)
			)	),
			_stopPoint(NULL),
			_depot(NULL)
		{}



		std::string VehiclePosition::GetStatusName( VehiclePositionStatusEnum value )
		{
			switch(value)
			{
			case TRAINING: return "Formation";
			case DEAD_RUN_DEPOT: return "Transfert dépôt";
			case DEAD_RUN_TRANSFER: return "Transfert inter-ligne";
			case SERVICE: return "Trajet de service";
			case COMMERCIAL: return "Service commercial";
			case NOT_IN_SERVICE: return "Arrêt";
			case OUT_OF_SERVICE: return "Hors service";
			case UNKNOWN_STATUS: return "Inconnu";
			case REFUELING: return "Carburant";
			case TERMINUS_START: return "Départ terminus";
			}
			return string();
		}



		vector<pair<optional<VehiclePositionStatusEnum>, string> > VehiclePosition::GetStatusList()
		{
			vector<pair<optional<VehiclePositionStatusEnum>,string> > result;
			for(int i(0); i<9; ++i)
			{
				result.push_back(make_pair(optional<VehiclePositionStatusEnum>(static_cast<VehiclePositionStatusEnum>(i)), GetStatusName(static_cast<VehiclePositionStatusEnum>(i))));
			}
			return result;
		}



		void VehiclePosition::toParametersMap(
			util::ParametersMap& pm,
			bool withAdditionalParameters,
			boost::logic::tribool withFiles,
			std::string prefix
		) const	{

			if(hasGeometry())
			{
				// Fetch the srid if any
				CoordinatesSystem::SRID srid = pm.getDefault<CoordinatesSystem::SRID>(Vehicle::PARAMETER_SRID,0);

				// Convert the geometry into the given coordinate system, if specified
				if (srid)
				{
					boost::shared_ptr<geos::geom::Geometry> geom = CoordinatesSystem::GetCoordinatesSystem(srid).convertGeometry(static_cast<geos::geom::Geometry&>(*getGeometry()));
					pm.insert(ATTR_X, geom->getCoordinate()->x);
					pm.insert(ATTR_Y, geom->getCoordinate()->y);
				}
				else
				{
					pm.insert(ATTR_X, getGeometry()->getX());
					pm.insert(ATTR_Y, getGeometry()->getY());
				}
			}

			pm.insert(ATTR_METER_OFFSET, getMeterOffset());
			pm.insert(ATTR_STATUS, getStatus());
			pm.insert(ATTR_IN_STOP_AREA, getInStopArea());
			pm.insert(ATTR_VEHICLE_NUMBER, _vehicleNumber);
			pm.insert(ATTR_STOP_FOUND_TIME, getNextStopFoundTime());
			if(_stopPoint)
			{
				boost::shared_ptr<ParametersMap> stopPM(new ParametersMap);
				_stopPoint->toParametersMap(*stopPM, false);
				pm.insert(TAG_STOP, stopPM);
			}
			if(get<VehiclePositionService>())
			{
				boost::shared_ptr<ParametersMap> servicePM(new ParametersMap);
				get<VehiclePositionService>()->toParametersMap(*servicePM, false);
				pm.insert(TAG_SERVICE, servicePM);
			}
		}

		void VehiclePosition::setVehicle(Vehicle* value)
		{
			set<VehiclePositionVehicle>(value
				? boost::optional<Vehicle&>(*value)
				: boost::none);
		}

		void VehiclePosition::setStopPoint(pt::StopPoint* value)
		{
			_stopPoint = value;
			if (value)
			{
				set<StopPointId>(value->getKey());
			}
		}

		void VehiclePosition::setService(pt::ScheduledService* value)
		{
			set<VehiclePositionService>(value
				? boost::optional<pt::ScheduledService&>(*value)
				: boost::none);
		}

		void VehiclePosition::setDepot(pt_operation::Depot* value)
		{
			_depot = value;
			if (value)
			{
				set<StopPointId>(value->getKey());
			}
		}

		void VehiclePosition::setRankInPath(boost::optional<std::size_t> value)
		{
			set<VehiclePositionRankInPath>(value
				? *value
				: 0);
		}

		Vehicle* VehiclePosition::getVehicle() const
		{
			if (get<VehiclePositionVehicle>())
			{
				return get<VehiclePositionVehicle>().get_ptr();
			}

			return NULL;
		}

		pt::ScheduledService* VehiclePosition::getService() const
		{
			if (get<VehiclePositionService>())
			{
				return get<VehiclePositionService>().get_ptr();
			}

			return NULL;
		}

		boost::optional<std::size_t> VehiclePosition::getRankInPath() const
		{
			if (get<VehiclePositionRankInPath>() == 0)
			{
				return boost::optional<std::size_t>();
			}

			return boost::optional<std::size_t>(get<VehiclePositionRankInPath>());
		}

		void VehiclePosition::link(util::Env& env, bool withAlgorithmOptimizations)
		{
			setGeometry(get<PointGeometry>());

			RegistryKeyType sid(get<StopPointId>());
			if(sid > 0)
			{
				try
				{
					RegistryTableType tableId(decodeTableId(sid));
					if(tableId == pt::StopPointTableSync::TABLE.ID)
					{
						_stopPoint = pt::StopPointTableSync::GetEditable(sid, env).get();
					}
					else if(tableId == pt_operation::DepotTableSync::TABLE.ID)
					{
						_depot = pt_operation::DepotTableSync::GetEditable(sid, env).get();
					}
				}
				catch(ObjectNotFoundException<pt::StopPoint>&)
				{
					Log::GetInstance().warn("No such stop point "+ lexical_cast<string>(sid) +" in VehiclePosition "+ lexical_cast<string>(getKey()));
				}
				catch(ObjectNotFoundException<pt_operation::Depot>&)
				{
					Log::GetInstance().warn("No such depot "+ lexical_cast<string>(sid) +" in VehiclePosition "+ lexical_cast<string>(getKey()));
				}
			}
		}

		bool VehiclePosition::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool VehiclePosition::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool VehiclePosition::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::DELETE_RIGHT);
		}
}	}
