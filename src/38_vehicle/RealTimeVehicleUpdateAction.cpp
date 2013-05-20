
/** RealTimeVehicleUpdateAction class implementation.
	@file RealTimeVehicleUpdateAction.cpp
	@author Hugues
	@date 2009

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

#include "RealTimeVehicleUpdateAction.hpp"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Request.h"
#include "Env.h"
#include "ScheduledService.h"
#include "LineStop.h"
#include "DataSourceTableSync.h"
#include "VehicleService.hpp"
#include "DriverService.hpp"

using namespace std;
using namespace boost;

using namespace boost::posix_time;

namespace synthese
{
	using namespace graph;
	using namespace impex;
	using namespace pt;
	using namespace pt_operation;
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace vehicle;

	namespace util
	{
		template<>
		const string FactorableTemplate<Action, RealTimeVehicleUpdateAction>::FACTORY_KEY("RealTimeVehicleUpdate");
	}

	namespace vehicle
	{
		const string RealTimeVehicleUpdateAction::PARAMETER_VEHICLE_SERVICE_DATASOURCE_ID = Action_PARAMETER_PREFIX + "sv_ds_id";
		const string RealTimeVehicleUpdateAction::PARAMETER_VEHICLE_SERVICE_ID = Action_PARAMETER_PREFIX + "sv_id";
		const string RealTimeVehicleUpdateAction::PARAMETER_VEHICLE_SERVICE_SERVICE_RANK = Action_PARAMETER_PREFIX + "sv_s_rk";
		const string RealTimeVehicleUpdateAction::PARAMETER_VEHICLE_SERVICE_SERVICE_OR_DRIVER_EXCHANGE_RANK = Action_PARAMETER_PREFIX + "sv_sd_rk";
		const string RealTimeVehicleUpdateAction::PARAMETER_LATE_DURATION_SECONDS = Action_PARAMETER_PREFIX + "la";

		const string RealTimeVehicleUpdateAction::PARAMETER_LINE_STOP_RANK = Action_PARAMETER_PREFIX + "ls";
		const string RealTimeVehicleUpdateAction::PARAMETER_LINE_STOP_METRIC_OFFSET = Action_PARAMETER_PREFIX + "mo";
		const string RealTimeVehicleUpdateAction::PARAMETER_AT_ARRIVAL = Action_PARAMETER_PREFIX + "aa";
		const string RealTimeVehicleUpdateAction::PARAMETER_AT_DEPARTURE = Action_PARAMETER_PREFIX + "ad";
		const string RealTimeVehicleUpdateAction::PARAMETER_PROPAGATE_CONSTANTLY = Action_PARAMETER_PREFIX + "pc";
		const string RealTimeVehicleUpdateAction::PARAMETER_DEPARTURE_TIME = Action_PARAMETER_PREFIX + "dt";
		const string RealTimeVehicleUpdateAction::PARAMETER_ARRIVAL_TIME = Action_PARAMETER_PREFIX + "at";


		RealTimeVehicleUpdateAction::RealTimeVehicleUpdateAction():
			_service(NULL),
			_lineStopRank(0),
			_lateDuration(not_a_date_time),
			_departureTime(not_a_date_time),
			_arrivalTime(not_a_date_time)
		{}



		ParametersMap RealTimeVehicleUpdateAction::getParametersMap() const
		{
			ParametersMap map;
/*			if(_service.get())
			{
				if(_dataSource.get())
				{
					map.insert(PARAMETER_SERVICE_DATASOURCE_ID, _dataSource->getKey());
					map.insert(PARAMETER_SERVICE_ID, _service->getCodeBySource(*_dataSource));
				}
				else
				{
					map.insert(PARAMETER_SERVICE_ID, _service->getKey());
				}
			}
*/			map.insert(PARAMETER_LINE_STOP_RANK, static_cast<int>(_lineStopRank));
			if(!_lateDuration.is_not_a_date_time())
			{
				map.insert(PARAMETER_LATE_DURATION_SECONDS, _lateDuration.total_seconds());
			}
			if(!_departureTime.is_not_a_date_time())
			{
				map.insert(PARAMETER_DEPARTURE_TIME, to_simple_string(_departureTime));
			}
			if(!_arrivalTime.is_not_a_date_time())
			{
				map.insert(PARAMETER_ARRIVAL_TIME, to_simple_string(_arrivalTime));
			}
			map.insert(PARAMETER_AT_ARRIVAL, _atArrival);
			map.insert(PARAMETER_AT_DEPARTURE, _atDeparture);
			map.insert(PARAMETER_PROPAGATE_CONSTANTLY, _propagateConstantly);
			return map;
		}



		void RealTimeVehicleUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			ptime now(second_clock::local_time());

			if(map.getDefault<RegistryKeyType>(PARAMETER_VEHICLE_SERVICE_DATASOURCE_ID, 0))
			{
				try
				{
					boost::shared_ptr<const DataSource> dataSource(
						Env::GetOfficialEnv().getRegistry<DataSource>().get(map.get<RegistryKeyType>(PARAMETER_VEHICLE_SERVICE_DATASOURCE_ID))
					);
					VehicleService* obj(dataSource->getObjectByCode<VehicleService>(map.get<string>(PARAMETER_VEHICLE_SERVICE_ID)));
					if(!obj)
					{
						throw ActionException("No such vehicle service");
					}
					boost::shared_ptr<VehicleService> vehicleService(
						Env::GetOfficialEnv().getEditableSPtr(obj)
					);

					// Service by rank in vehicle service
					if(map.isDefined(PARAMETER_VEHICLE_SERVICE_SERVICE_RANK))
					{
						size_t rank(map.get<size_t>(PARAMETER_VEHICLE_SERVICE_SERVICE_RANK));

						size_t curRank(0);
						BOOST_FOREACH(SchedulesBasedService* service, vehicleService->getServices())
						{
							if(service->isActive(now.date()))
							{
								if(curRank == rank)
								{
									_service = dynamic_cast<ScheduledService*>(service);
									break;
								}
								++curRank;
							}
						}
						if(!_service)
						{
							throw ActionException("No such service");
						}
					} // Service by rank in driver services sequence
					else if(map.isDefined(PARAMETER_VEHICLE_SERVICE_SERVICE_OR_DRIVER_EXCHANGE_RANK))
					{
						size_t rank(map.get<size_t>(PARAMETER_VEHICLE_SERVICE_SERVICE_OR_DRIVER_EXCHANGE_RANK));

						size_t curRank(0);
						BOOST_FOREACH(const DriverService::Chunk* chunk, vehicleService->getDriverServiceChunks())
						{
							if(!chunk->driverService->isActive(now.date()))
							{
								continue;
							}

							BOOST_FOREACH(const DriverService::Chunk::Element& service, chunk->elements)
							{
								if(curRank == rank)
								{
									_service = dynamic_cast<ScheduledService*>(service.service);
									break;
								}
								++curRank;
							}
							if(_service)
							{
								break;
							}
						}
						if(!_service)
						{
							throw ActionException("No such service");
						}
					}
				}
				catch(ObjectNotFoundException<DataSource>&)
				{
					throw ActionException("No such datasource");
				}
			}
/*			else try
			{
				_service = Env::GetOfficialEnv().getEditableRegistry<ScheduledService>().getEditable(
					map.get<RegistryKeyType>(PARAMETER_SERVICE_ID)
				);
			}
			catch(ObjectNotFoundException<ScheduledService>)
			{
				throw ActionException("No such service");
			}
*/
/*			if(map.isDefined(PARAMETER_LINE_STOP_RANK))
			{
				_lineStopRank = map.get<size_t>(PARAMETER_LINE_STOP_RANK);
			}
			else
			{
				MetricOffset offset(map.get<MetricOffset>(PARAMETER_LINE_STOP_METRIC_OFFSET));
				try
				{
					_lineStopRank = _service->getPath()->getEdgeRankAtOffset(offset);
				}
				catch (Path::InvalidOffsetException& e)
				{
					throw ActionException(e.getMessage());
				}
			}

			if(_lineStopRank >= _service->getArrivalSchedules(false).size())
			{
				throw ActionException("Inconsistent linestop rank");
			}
*/
			if(map.isDefined(PARAMETER_LATE_DURATION_SECONDS))
			{
				_lateDuration = posix_time::seconds(map.get<int>(PARAMETER_LATE_DURATION_SECONDS));
				_atArrival = map.getDefault<bool>(PARAMETER_AT_ARRIVAL, true);
				_atDeparture = map.getDefault<bool>(PARAMETER_AT_DEPARTURE, true);
				_propagateConstantly = map.getDefault<bool>(PARAMETER_PROPAGATE_CONSTANTLY, true);

				ptime plannedTime(now);
				plannedTime -= _lateDuration;

				for(_lineStopRank = 0; _lineStopRank < _service->getPath()->getEdges().size(); ++_lineStopRank)
				{
					if(	_service->getArrivalSchedule(false, _lineStopRank) >= plannedTime.time_of_day())
					{
						break;
					}
				}

				if(_lineStopRank == _service->getPath()->getEdges().size())
				{
					throw ActionException("This service does not run right now.");
				}
			}
			else
			{
				if(map.isDefined(PARAMETER_DEPARTURE_TIME))
				{
					_departureTime = duration_from_string(map.get<string>(PARAMETER_DEPARTURE_TIME));
					// Detection of bad encoding of schedules after midnight
					if(_service->getDepartureSchedule(false, _lineStopRank) - _departureTime > hours(12))
					{
						_departureTime += hours(24);
					}
				}
				if(map.isDefined(PARAMETER_ARRIVAL_TIME))
				{
					_arrivalTime = duration_from_string(map.get<string>(PARAMETER_ARRIVAL_TIME));
					// Detection of bad encoding of schedules after midnight
					if(_service->getArrivalSchedule(false, _lineStopRank) - _arrivalTime > hours(12))
					{
						_arrivalTime += hours(24);
					}
				}
			}
		}



		void RealTimeVehicleUpdateAction::run(Request& request)
		{
			if(!_lateDuration.is_not_a_date_time())
			{
				_service->applyRealTimeShiftDuration(
					_lineStopRank,
					(_atArrival ? _lateDuration : seconds(0)),
					(_atDeparture ? _lateDuration : seconds(0)),
					_propagateConstantly,
					false // Disable timestamping
				);
			}
			else
			{
				_service->setRealTimeSchedules(
					_lineStopRank,
					_departureTime,
					_arrivalTime
				);
			}
		}



		bool RealTimeVehicleUpdateAction::isAuthorized(const Session* session
		) const {
			return true;
		}



		void RealTimeVehicleUpdateAction::setService(
			const ScheduledService* service
		){
			_service = const_cast<ScheduledService*>(service);
		}



		void RealTimeVehicleUpdateAction::setLineStopRank( std::size_t value )
		{
			_lineStopRank = value;
		}



		void RealTimeVehicleUpdateAction::setAtArrival( bool value )
		{
			_atArrival = value;
		}



		void RealTimeVehicleUpdateAction::setAtDeparture( bool value )
		{
			_atDeparture = value;
		}



		void RealTimeVehicleUpdateAction::setPropagateConstantly( bool value )
		{
			_propagateConstantly = value;
		}



		void RealTimeVehicleUpdateAction::setDelay( boost::posix_time::time_duration value )
		{
			_lateDuration = value;
		}
}	}
