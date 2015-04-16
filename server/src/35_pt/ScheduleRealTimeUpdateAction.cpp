
/** ScheduleRealTimeUpdateAction class implementation.
	@file ScheduleRealTimeUpdateAction.cpp
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

#include "ActionException.h"
#include "ParametersMap.h"
#include "ScheduleRealTimeUpdateAction.h"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "Env.h"
#include "ScheduledService.h"
#include "LineStop.h"
#include "DataSourceTableSync.h"

using namespace std;
using namespace boost;

using namespace boost::posix_time;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace graph;
	using namespace pt;
	using namespace impex;

	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::ScheduleRealTimeUpdateAction>::FACTORY_KEY("ScheduleRealTimeUpdateAction");
	}

	namespace pt
	{
		const string ScheduleRealTimeUpdateAction::PARAMETER_LATE_DURATION_MINUTES = Action_PARAMETER_PREFIX + "la";
		const string ScheduleRealTimeUpdateAction::PARAMETER_LINE_STOP_RANK = Action_PARAMETER_PREFIX + "ls";
		const string ScheduleRealTimeUpdateAction::PARAMETER_LINE_STOP_METRIC_OFFSET = Action_PARAMETER_PREFIX + "mo";
		const string ScheduleRealTimeUpdateAction::PARAMETER_SERVICE_ID = Action_PARAMETER_PREFIX + "se";
		const string ScheduleRealTimeUpdateAction::PARAMETER_AT_ARRIVAL = Action_PARAMETER_PREFIX + "aa";
		const string ScheduleRealTimeUpdateAction::PARAMETER_AT_DEPARTURE = Action_PARAMETER_PREFIX + "ad";
		const string ScheduleRealTimeUpdateAction::PARAMETER_PROPAGATE_CONSTANTLY = Action_PARAMETER_PREFIX + "pc";
		const string ScheduleRealTimeUpdateAction::PARAMETER_SERVICE_DATASOURCE_ID = Action_PARAMETER_PREFIX + "ds";
		const string ScheduleRealTimeUpdateAction::PARAMETER_DEPARTURE_TIME = Action_PARAMETER_PREFIX + "dt";
		const string ScheduleRealTimeUpdateAction::PARAMETER_ARRIVAL_TIME = Action_PARAMETER_PREFIX + "at";


		ScheduleRealTimeUpdateAction::ScheduleRealTimeUpdateAction():
			_lateDuration(not_a_date_time),
			_departureTime(not_a_date_time),
			_arrivalTime(not_a_date_time)
		{}



		ParametersMap ScheduleRealTimeUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_service.get())
			{
				if(_dataSource.get())
				{
					map.insert(PARAMETER_SERVICE_DATASOURCE_ID, _dataSource->getKey());
					map.insert(PARAMETER_SERVICE_ID, _serviceCodeBySource);
				}
				else
				{
					map.insert(PARAMETER_SERVICE_ID, _service->getKey());
				}
			}
			map.insert(PARAMETER_LINE_STOP_RANK, static_cast<int>(_lineStopRank));
			if(!_lateDuration.is_not_a_date_time())
			{
				map.insert(PARAMETER_LATE_DURATION_MINUTES, _lateDuration.total_seconds() / 60);
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



		void ScheduleRealTimeUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			if(map.getDefault<RegistryKeyType>(PARAMETER_SERVICE_DATASOURCE_ID, 0))
			{
				try
				{
					boost::shared_ptr<const DataSource> dataSource(
						Env::GetOfficialEnv().getRegistry<DataSource>().get(map.get<RegistryKeyType>(PARAMETER_SERVICE_DATASOURCE_ID))
					);
					_serviceCodeBySource = map.get<string>(PARAMETER_SERVICE_ID);
					ScheduledService* obj(dataSource->getObjectByCode<ScheduledService>(_serviceCodeBySource));
					if(!obj)
					{
						throw ActionException("No such service");
					}
					_service = Env::GetOfficialEnv().getEditableSPtr(obj);
				}
				catch(ObjectNotFoundException<DataSource>&)
				{
					throw ActionException("No such datasource");
				}
			}
			else try
			{
				_service = Env::GetOfficialEnv().getEditableRegistry<ScheduledService>().getEditable(
					map.get<RegistryKeyType>(PARAMETER_SERVICE_ID)
				);
			}
			catch(ObjectNotFoundException<ScheduledService>)
			{
				throw ActionException("No such service");
			}

			if(map.isDefined(PARAMETER_LINE_STOP_RANK))
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

			if(_lineStopRank >= _service->getArrivalSchedules(true, false).size())
			{
				throw ActionException("Inconsistent linestop rank");
			}

			if(map.isDefined(PARAMETER_LATE_DURATION_MINUTES))
			{
				_lateDuration = posix_time::minutes(map.get<int>(PARAMETER_LATE_DURATION_MINUTES));
				_atArrival = map.getDefault<bool>(PARAMETER_AT_ARRIVAL, true);
				_atDeparture = map.getDefault<bool>(PARAMETER_AT_DEPARTURE, true);
				_propagateConstantly = map.getDefault<bool>(PARAMETER_PROPAGATE_CONSTANTLY, true);
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



		void ScheduleRealTimeUpdateAction::run(Request& request)
		{
			if(!_lateDuration.is_not_a_date_time())
			{
				_service->applyRealTimeShiftDuration(
					_lineStopRank,
					(_atArrival ? _lateDuration : seconds(0)),
					(_atDeparture ? _lateDuration : seconds(0)),
					_propagateConstantly,
					true // Enable timestamping
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



		bool ScheduleRealTimeUpdateAction::isAuthorized(const Session* session
		) const {
#if 1
			return true;
#else // See https://extranet.rcsmobility.com/issues/16043
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
#endif
		}



		void ScheduleRealTimeUpdateAction::setService(
			boost::shared_ptr<const ScheduledService> service
		){
			_service = const_pointer_cast<ScheduledService>(service);
		}



		void ScheduleRealTimeUpdateAction::setLineStopRank( std::size_t value )
		{
			_lineStopRank = value;
		}



		void ScheduleRealTimeUpdateAction::setAtArrival( bool value )
		{
			_atArrival = value;
		}



		void ScheduleRealTimeUpdateAction::setAtDeparture( bool value )
		{
			_atDeparture = value;
		}



		void ScheduleRealTimeUpdateAction::setPropagateConstantly( bool value )
		{
			_propagateConstantly = value;
		}



		void ScheduleRealTimeUpdateAction::setDelay( boost::posix_time::time_duration value )
		{
			_lateDuration = value;
		}
}	}
