
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
	using namespace graph;
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
		const string VehiclePositionUpdateAction::PARAMETER_IN_STOP_AREA = Action_PARAMETER_PREFIX + "_in_stop_area";
		const string VehiclePositionUpdateAction::PARAMETER_STOP_FOUND_TIME = Action_PARAMETER_PREFIX + "_next_stop_found_time";
		const string VehiclePositionUpdateAction::PARAMETER_SET_NEXT_STOPS = Action_PARAMETER_PREFIX + "_set_next_stops";



		VehiclePositionUpdateAction::VehiclePositionUpdateAction() :
			_setAsCurrentPosition(false),
			_setNextStops(false)
		{}



		ParametersMap VehiclePositionUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_vehicle)
			{
				map.insert(PARAMETER_VEHICLE_ID, _vehicle ? (*_vehicle)->getKey() : RegistryKeyType(0));
			}
			if(_vehiclePosition.get())
			{
				map.insert(PARAMETER_VEHICLE_POSITION_ID, _vehiclePosition->getKey());
			}

			if(_setAsCurrentPosition)
			{
				map.insert(PARAMETER_SET_AS_CURRENT_POSITION, _setAsCurrentPosition);
			}
			
			if(_setNextStops)
			{
				map.insert(PARAMETER_SET_NEXT_STOPS, _setNextStops);
			}

			// Warning : Export other parameters if setters are implemented
			return map;
		}



		void VehiclePositionUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Set as current position
			if(map.isDefined(PARAMETER_SET_AS_CURRENT_POSITION))
			{
				_setAsCurrentPosition = map.get<bool>(PARAMETER_SET_AS_CURRENT_POSITION);
			}

			// The object to update
			if(map.isDefined(PARAMETER_VEHICLE_POSITION_ID))
			{
				try
				{
					_vehiclePosition = VehiclePositionTableSync::GetEditable(
						map.get<RegistryKeyType>(PARAMETER_VEHICLE_POSITION_ID),
						*_env
					);
				}
				catch(ObjectNotFoundException<VehiclePosition>&)
				{
					throw ActionException("No such vehicle position");
				}
			}
			else
			{
				_vehiclePosition.reset(new VehiclePosition);

				if(_setAsCurrentPosition)
				{
					// Copy the values of the current position in the object
					const VehiclePosition &vp = VehicleModule::GetCurrentVehiclePosition();

					_comment = vp.getComment();
					_status = vp.getStatus();
					_time = second_clock::local_time();
					_passengers = vp.getPassengers();
					_inStopArea = vp.getInStopArea();
					_meterOffset = vp.getMeterOffset();
					_rankInPath = vp.getRankInPath();
					_vehicle = vp.getVehicle();
					_stopPoint = vp.getStopPoint();
					_service = vp.getService();
					_depot = vp.getDepot();
				}
			}

			if(map.isDefined(PARAMETER_VEHICLE_ID))
			{
				RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_VEHICLE_ID));
				if(id > 0) try
				{
					_vehicle = Env::GetOfficialEnv().getEditable<Vehicle>(id).get();
				}
				catch(ObjectNotFoundException<Vehicle>&)
				{
					throw ActionException("No such vehicle");
				}
				else
				{
					_vehicle = (Vehicle*) NULL;
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
					_service = Env::GetOfficialEnv().getEditable<ScheduledService>(id).get();
				}
				catch(ObjectNotFoundException<ScheduledService>&)
				{
					throw ActionException("No such service");
				}
				else
				{
					_service = (ScheduledService*) NULL;
				}
			}

			if(map.isDefined(PARAMETER_STOP_POINT_ID))
			{
				RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_STOP_POINT_ID));
				if(id > 0) try
				{
					if(decodeTableId(id) == StopPointTableSync::TABLE.ID)
					{
						_stopPoint = Env::GetOfficialEnv().getEditable<StopPoint>(id).get();
						_depot = (Depot*) NULL;
					}
					else if(decodeTableId(id) == DepotTableSync::TABLE.ID)
					{
						_depot = Env::GetOfficialEnv().getEditable<Depot>(id).get();
						_stopPoint = (StopPoint*) NULL;
					}
					else
					{
						_stopPoint = (StopPoint*) NULL;
						_depot = (Depot*) NULL;
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
					_stopPoint = (StopPoint*) NULL;
					_depot = (Depot*) NULL;
				}
			}

			if(map.isDefined(PARAMETER_COMMENT))
			{
				_comment = map.get<string>(PARAMETER_COMMENT);
			}

			if(map.isDefined(PARAMETER_STATUS))
			{
				_status = static_cast<VehiclePositionStatusEnum>(map.get<int>(PARAMETER_STATUS));
			}

			if(map.isDefined(PARAMETER_METER_OFFSET))
			{
				_meterOffset = map.get<VehiclePosition::Meters>(PARAMETER_METER_OFFSET);
			}

			if(map.isDefined(PARAMETER_TIME))
			{
				_time = time_from_string(map.get<string>(PARAMETER_TIME));
			}
			if(map.isDefined(PARAMETER_STOP_FOUND_TIME))
			{
				_nextStopFoundTime = time_from_string(map.get<string>(PARAMETER_STOP_FOUND_TIME));
			}
			
			// In stop area
			if(map.isDefined(PARAMETER_IN_STOP_AREA))
			{
				_inStopArea = map.get<bool>(PARAMETER_IN_STOP_AREA);
			}
			
			// Set next stops
			if(map.isDefined(PARAMETER_SET_NEXT_STOPS))
			{
				_setNextStops = map.get<bool>(PARAMETER_SET_NEXT_STOPS);
			}
		}



		void VehiclePositionUpdateAction::run(
			Request& request
		){
			VehiclePosition &vp = VehicleModule::GetCurrentVehiclePosition();
			if(_vehicle)
			{
				_vehiclePosition->setVehicle(*_vehicle);
				if(_setAsCurrentPosition)
				{
					vp.setVehicle(_vehiclePosition->getVehicle());
				}
			}
			if(_passengers)
			{
				_vehiclePosition->setPassengers(*_passengers);
				if(_setAsCurrentPosition)
				{
					vp.setPassengers(_vehiclePosition->getPassengers());
				}
			}
			if(_rankInPath)
			{
				_vehiclePosition->setRankInPath(*_rankInPath);
				if(_setAsCurrentPosition)
				{
					vp.setRankInPath(_vehiclePosition->getRankInPath());
				}
			}
			if(_service)
			{
				_vehiclePosition->setService(*_service);
				if(_setAsCurrentPosition)
				{
					vp.setService(_vehiclePosition->getService());
				}
			}
			if(_comment)
			{
				_vehiclePosition->setComment(*_comment);
				if(_setAsCurrentPosition)
				{
					vp.setComment(_vehiclePosition->getComment());
				}
			}
			if(_status)
			{
				_vehiclePosition->setStatus(*_status);
				if(_setAsCurrentPosition)
				{
					vp.setStatus( _vehiclePosition->getStatus());
				}
			}
			if(_meterOffset)
			{
				_vehiclePosition->setMeterOffset(*_meterOffset);
				if(_setAsCurrentPosition)
				{
					vp.setMeterOffset(_vehiclePosition->getMeterOffset());
				}
			}
			if(_time)
			{
				_vehiclePosition->setTime(*_time);
				if(_setAsCurrentPosition)
				{
					vp.setTime(_vehiclePosition->getTime());
				}
			}
			if(_stopPoint)
			{
				_vehiclePosition->setStopPoint(*_stopPoint);
				if(_setAsCurrentPosition)
				{
					vp.setStopPoint(_vehiclePosition->getStopPoint());
				}
			}
			if(_depot)
			{
				_vehiclePosition->setDepot(*_depot);
				if(_setAsCurrentPosition)
				{
					vp.setDepot(_vehiclePosition->getDepot());
				}
			}
			if(_inStopArea)
			{
				_vehiclePosition->setInStopArea(*_inStopArea);
				if(_setAsCurrentPosition)
				{
					vp.setInStopArea(_vehiclePosition->getInStopArea());
				}
			}
			if(_nextStopFoundTime)
			{
				_vehiclePosition->setNextStopFoundTime(*_nextStopFoundTime);
				if(_setAsCurrentPosition)
				{
					vp.setNextStopFoundTime(_vehiclePosition->getNextStopFoundTime());
				}
			}

			if (_setNextStops && _rankInPath)
			{
				Path::Edges allEdges;
				if(VehicleModule::GetCurrentVehiclePosition().getService())
				{
					allEdges = VehicleModule::GetCurrentVehiclePosition().getService()->getPath()->getEdges();
				}
				
				// Update the next stops
				CurrentJourney::NextStops nextStops;
				{
					BOOST_FOREACH(const Path::Edges::value_type& edge, allEdges)
					{
						if(edge->getRankInPath() > *_rankInPath &&
							dynamic_cast<StopPoint*>(edge->getFromVertex()))
						{
							NextStop nextStop;
							nextStop.setStop(static_cast<StopPoint*>(edge->getFromVertex()));
							nextStop.setRank(edge->getRankInPath());
							nextStops.push_back(nextStop);
						}
					}
				}
				VehicleModule::GetCurrentJourney().setNextStops(nextStops);
			}

			VehiclePositionTableSync::Save(_vehiclePosition.get());

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
