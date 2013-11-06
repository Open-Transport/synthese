
//////////////////////////////////////////////////////////////////////////
/// VehiclePositionUpdateAction class implementation.
/// @file VehiclePositionUpdateAction.cpp
/// @author Hugues Romain
/// @date 2011
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

#include "ActionException.h"
#include "ParametersMap.h"
#include "VehiclePositionUpdateAction.hpp"
#include "VehiclePositionTableSync.hpp"
#include "VehicleTableSync.hpp"
#include "ScheduledServiceTableSync.h"
#include "StopPointTableSync.hpp"
#include "Request.h"
#include "DepotTableSync.hpp"
#include "VehicleModule.hpp"

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace pt;
	using namespace pt_operation;
	using namespace security;
	using namespace server;
	using namespace util;
	using namespace vehicle;

	namespace util
	{
		template<> const string FactorableTemplate<Action, VehiclePositionUpdateAction>::FACTORY_KEY("VehiclePositionUpdateAction");
	}

	namespace vehicle
	{
		const string VehiclePositionUpdateAction::PARAMETER_BEFORE_ID = Action_PARAMETER_PREFIX + "bf";
		const string VehiclePositionUpdateAction::PARAMETER_COMMENT = Action_PARAMETER_PREFIX + "co";
		const string VehiclePositionUpdateAction::PARAMETER_METER_OFFSET = Action_PARAMETER_PREFIX + "mo";
		const string VehiclePositionUpdateAction::PARAMETER_PASSENGERS = Action_PARAMETER_PREFIX + "pa";
		const string VehiclePositionUpdateAction::PARAMETER_RANK_IN_PATH = Action_PARAMETER_PREFIX + "rk";
		const string VehiclePositionUpdateAction::PARAMETER_SERVICE_ID = Action_PARAMETER_PREFIX + "si";
		const string VehiclePositionUpdateAction::PARAMETER_STATUS = Action_PARAMETER_PREFIX + "st";
		const string VehiclePositionUpdateAction::PARAMETER_STOP_POINT_ID = Action_PARAMETER_PREFIX + "sp";
		const string VehiclePositionUpdateAction::PARAMETER_TIME = Action_PARAMETER_PREFIX + "ti";
		const string VehiclePositionUpdateAction::PARAMETER_VEHICLE_ID = Action_PARAMETER_PREFIX + "ve";
		const string VehiclePositionUpdateAction::PARAMETER_VEHICLE_POSITION_ID = Action_PARAMETER_PREFIX + "vi";
		const string VehiclePositionUpdateAction::PARAMETER_SET_AS_CURRENT_POSITION = Action_PARAMETER_PREFIX + "_set_as_current_position";

		VehiclePositionUpdateAction::VehiclePositionUpdateAction() :
			_setAsCurrentPosition(false)
		{
		}

		ParametersMap VehiclePositionUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_vehicle)
			{
				map.insert(PARAMETER_VEHICLE_ID, _vehicle->get() ? (*_vehicle)->getKey() : RegistryKeyType(0));
			}
			if(_vehiclePosition.get())
			{
				map.insert(PARAMETER_VEHICLE_POSITION_ID, _vehiclePosition->getKey());
			}

			if(_setAsCurrentPosition)
			{
				map.insert(PARAMETER_SET_AS_CURRENT_POSITION, _setAsCurrentPosition);
			}

			// Warning : Export other parameters if setters are implemented
			return map;
		}



		void VehiclePositionUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			if(map.isDefined(PARAMETER_VEHICLE_POSITION_ID))
			{
				try
				{
					_vehiclePosition = VehiclePositionTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_VEHICLE_POSITION_ID), *_env);
				}
				catch(ObjectNotFoundException<VehiclePosition>&)
				{
					throw ActionException("No such vehicle position");
				}
			}
			else
			{
				_vehiclePosition.reset(new VehiclePosition);
			}

			if(map.isDefined(PARAMETER_VEHICLE_ID))
			{
				RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_VEHICLE_ID));
				if(id > 0) try
				{
					_vehicle = Env::GetOfficialEnv().getEditable<Vehicle>(id);
				}
				catch(ObjectNotFoundException<Vehicle>&)
				{
					throw ActionException("No such vehicle");
				}
				else
				{
					_vehicle = boost::shared_ptr<Vehicle>();
				}
			}

			if(map.isDefined(PARAMETER_PASSENGERS))
			{
				_passengers = map.get<size_t>(PARAMETER_PASSENGERS);
			}

			if(map.isDefined(PARAMETER_RANK_IN_PATH))
			{
				_rankInPath = map.getOptional<size_t>(PARAMETER_RANK_IN_PATH);
			}

			if(map.isDefined(PARAMETER_SERVICE_ID))
			{
				RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_SERVICE_ID));
				if(id > 0) try
				{
					_service = Env::GetOfficialEnv().getEditable<ScheduledService>(id);
				}
				catch(ObjectNotFoundException<ScheduledService>&)
				{
					throw ActionException("No such service");
				}
				else
				{
					_service = boost::shared_ptr<ScheduledService>();
				}
			}

			if(map.isDefined(PARAMETER_STOP_POINT_ID))
			{
				RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_STOP_POINT_ID));
				if(id > 0) try
				{
					if(decodeTableId(id) == StopPointTableSync::TABLE.ID)
					{
						_stopPoint = Env::GetOfficialEnv().getEditable<StopPoint>(id);
						_depot = boost::shared_ptr<Depot>();
					}
					else if(decodeTableId(id) == DepotTableSync::TABLE.ID)
					{
						_depot = Env::GetOfficialEnv().getEditable<Depot>(id);
						_stopPoint = boost::shared_ptr<StopPoint>();
					}
					else
					{
						_stopPoint = boost::shared_ptr<StopPoint>();
						_depot = boost::shared_ptr<Depot>();
					}
				}
				catch(ObjectNotFoundException<StopPoint>&)
				{
					throw ActionException("No such stop");
				}
				catch(ObjectNotFoundException<Depot>&)
				{
					throw ActionException("No such depot");
				}
				else
				{
					_stopPoint = boost::shared_ptr<StopPoint>();
					_depot = boost::shared_ptr<Depot>();
				}
			}

			if(map.isDefined(PARAMETER_COMMENT))
			{
				_comment = map.get<string>(PARAMETER_COMMENT);
			}

			if(map.isDefined(PARAMETER_STATUS))
			{
				_status = static_cast<VehiclePosition::Status>(map.get<int>(PARAMETER_STATUS));
			}

			if(map.isDefined(PARAMETER_METER_OFFSET))
			{
				_meterOffset = map.get<VehiclePosition::Meters>(PARAMETER_METER_OFFSET);
			}

			if(map.isDefined(PARAMETER_TIME))
			{
				_time = time_from_string(map.get<string>(PARAMETER_TIME));
			}

			if(map.isDefined(PARAMETER_SET_AS_CURRENT_POSITION))
			{
				_setAsCurrentPosition = map.get<bool>(PARAMETER_SET_AS_CURRENT_POSITION);
			}
			
		}



		void VehiclePositionUpdateAction::run(
			Request& request
		){
			if(_vehicle)
			{
				_vehiclePosition->setVehicle(_vehicle->get());
			}
			if(_passengers)
			{
				_vehiclePosition->setPassangers(*_passengers);
			}
			if(_rankInPath)
			{
				_vehiclePosition->setRankInPath(*_rankInPath);
			}
			if(_service)
			{
				_vehiclePosition->setService(_service->get());
			}
			if(_comment)
			{
				_vehiclePosition->setComment(*_comment);
			}
			if(_status)
			{
				_vehiclePosition->setStatus(*_status);
			}
			if(_meterOffset)
			{
				_vehiclePosition->setMeterOffset(*_meterOffset);
			}
			if(_time)
			{
				_vehiclePosition->setTime(*_time);
			}
			if(_stopPoint)
			{
				_vehiclePosition->setStopPoint(_stopPoint->get());
			}
			if(_depot)
			{
				_vehiclePosition->setDepot(_depot->get());
			}

			VehiclePositionTableSync::Save(_vehiclePosition.get());

			if(_setAsCurrentPosition)
			{
				VehiclePosition &vp = VehicleModule::GetCurrentVehiclePosition();

				vp.setComment(_vehiclePosition->getComment());
				vp.setStatus( _vehiclePosition->getStatus());
				vp.setTime(_vehiclePosition->getTime());
				vp.setPassangers(_vehiclePosition->getPassengers());
				vp.setInStopArea(_vehiclePosition->getInStopArea());
				vp.setMeterOffset(_vehiclePosition->getMeterOffset());
				vp.setRankInPath(_vehiclePosition->getRankInPath());

				vp.setVehicle(_vehiclePosition->getVehicle());
				vp.setStopPoint(_vehiclePosition->getStopPoint());
				vp.setService(_vehiclePosition->getService());
				vp.setDepot(_vehiclePosition->getDepot());
			}

			if(request.getActionWillCreateObject())
			{
				request.setActionCreatedId(_vehiclePosition->getKey());
			}
		}



		bool VehiclePositionUpdateAction::isAuthorized(
			const Session* session
		) const {
			return true;
//			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<PTOperationRight>(WRITE);
		}
	}
}
