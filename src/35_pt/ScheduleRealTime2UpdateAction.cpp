
/** ScheduleRealTime2UpdateAction class implementation.
	@file ScheduleRealTime2UpdateAction.cpp
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
#include "ScheduleRealTime2UpdateAction.h"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "Env.h"
#include "ScheduledService.h"
#include "SchedulesBasedService.h"
#include "LineStop.h"
#include "StopPoint.hpp"
#include "DataSourceTableSync.h"
#include "JourneyPattern.hpp"
#include "StandardArrivalDepartureTableGenerator.h"
#include "ArrivalDepartureTableGenerator.h"

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
	using namespace departure_boards;

	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::ScheduleRealTime2UpdateAction>::FACTORY_KEY("ScheduleRealTime2UpdateAction");
	}

	namespace pt
	{
		const string ScheduleRealTime2UpdateAction::PARAMETER_LINE_STOP_RANK = Action_PARAMETER_PREFIX + "ls";
		const string ScheduleRealTime2UpdateAction::PARAMETER_SERVICE_ID = Action_PARAMETER_PREFIX + "se";
		const string ScheduleRealTime2UpdateAction::PARAMETER_SERVICE_DATASOURCE_ID = Action_PARAMETER_PREFIX + "ds";
		const string ScheduleRealTime2UpdateAction::PARAMETER_DEPARTURE_TIME = Action_PARAMETER_PREFIX + "dt";
		const string ScheduleRealTime2UpdateAction::PARAMETER_ARRIVAL_TIME = Action_PARAMETER_PREFIX + "at";


		ScheduleRealTime2UpdateAction::ScheduleRealTime2UpdateAction():
			_departureTime(not_a_date_time),
			_arrivalTime(not_a_date_time)
		{}



		ParametersMap ScheduleRealTime2UpdateAction::getParametersMap() const
		{
			ParametersMap map;
			// This mode is deprecated
			return map;
		}



		void ScheduleRealTime2UpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			if(map.getDefault<RegistryKeyType>(PARAMETER_SERVICE_DATASOURCE_ID, 0))
			{
				try
				{
					shared_ptr<const DataSource> dataSource(
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
				throw ActionException("Missing line stop rank");
			}

			if(_lineStopRank >= _service->getArrivalSchedules(false).size())
			{
				throw ActionException("Inconsistent linestop rank");
			}

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



		void ScheduleRealTime2UpdateAction::run(Request& request)
		{
			bool atArrival;
			// Calc the lateness
			if(!_departureTime.is_not_a_date_time())
			{
				atArrival = false;
			} else if(!_arrivalTime.is_not_a_date_time())
			{
				atArrival = true;
			} else
			{
				// Nothing to do
				return;
			}

			posix_time::time_duration ourTimeRef(
				atArrival 
				? _service->getArrivalBeginScheduleToIndex(true, _lineStopRank) 
				: _service->getDepartureBeginScheduleToIndex(true, _lineStopRank)
			);

			posix_time::time_duration lateDuration(
				atArrival 
				? _arrivalTime - ourTimeRef
				: _departureTime - ourTimeRef
			);

			ptime now(second_clock::local_time());

			//
			// Resync the all services that are between the old sceduled date and the
			// new one for the given service and line stop.
			//
			const StopPoint *sp(static_cast<const StopPoint*>(_service->getRealTimeVertex(_lineStopRank)));
			ArrivalDepartureTableGenerator::PhysicalStops ps;
			ps.insert(make_pair(sp->getKey(), sp));
			DeparturesTableDirection di(atArrival ? DISPLAY_ARRIVALS : DISPLAY_DEPARTURES);
			EndFilter ef(WITH_PASSING);
			LineFilter lf;
			DisplayedPlacesList dp;
			ForbiddenPlacesList fp;
			StandardArrivalDepartureTableGenerator tdg(
				ps,
				di,
				ef,
				lf,
				dp,
				fp,
				ptime(now.date(), ourTimeRef),
				ptime(now.date(), (atArrival ? _arrivalTime : _departureTime)),
				false
			);

			BOOST_FOREACH(const ArrivalDepartureList::value_type& itService, tdg.generate())
			{
				const SchedulesBasedService *nextService (dynamic_cast<const SchedulesBasedService*>(itService.first.getService()));

				posix_time::time_duration theirTimeRef(
					atArrival
					? nextService->getArrivalBeginScheduleToIndex(true, _lineStopRank) 
					: nextService->getDepartureBeginScheduleToIndex(true, _lineStopRank)
				);

				// Reschedule this service to the new given time 
				const_cast<SchedulesBasedService*>(nextService)->applyRealTimeLateDuration(
					_lineStopRank,
					ourTimeRef + lateDuration - theirTimeRef,
					atArrival,
					!atArrival,
					true
				);
			}
		}


		bool ScheduleRealTime2UpdateAction::isAuthorized(const Session* session
		) const {
#if 1
			return true;
#else // See https://extranet.rcsmobility.com/issues/16043
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
#endif
		}



		void ScheduleRealTime2UpdateAction::setService(
			boost::shared_ptr<const ScheduledService> service
		){
			_service = const_pointer_cast<ScheduledService>(service);
		}



		void ScheduleRealTime2UpdateAction::setLineStopRank( std::size_t value )
		{
			_lineStopRank = value;
		}
}	}
