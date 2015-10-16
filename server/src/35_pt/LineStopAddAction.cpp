
//////////////////////////////////////////////////////////////////////////
/// LineStopAddAction class implementation.
/// @file LineStopAddAction.cpp
/// @author Hugues Romain
/// @date 2010
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

#include "LineStopAddAction.h"

#include "ActionException.h"
#include "City.h"
#include "CityTableSync.h"
#include "DesignatedLinePhysicalStop.hpp"
#include "Destination.hpp"
#include "DRTArea.hpp"
#include "DRTAreaTableSync.hpp"
#include "GeographyModule.h"
#include "JourneyPatternTableSync.hpp"
#include "LineStopTableSync.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Request.h"
#include "RollingStock.hpp"
#include "Session.h"
#include "StopArea.hpp"
#include "StopAreaTableSync.hpp"
#include "StopPoint.hpp"
#include "StopPointTableSync.hpp"
#include "TransportNetworkRight.h"
#include "User.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace db;
	using namespace graph;
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace geography;

	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::LineStopAddAction>::FACTORY_KEY("LineStopAddAction");
	}

	namespace pt
	{
		const string LineStopAddAction::PARAMETER_CITY_NAME = Action_PARAMETER_PREFIX + "cn";
		const string LineStopAddAction::PARAMETER_STOP_NAME = Action_PARAMETER_PREFIX + "sn";
		const string LineStopAddAction::PARAMETER_RANK = Action_PARAMETER_PREFIX + "rk";
		const string LineStopAddAction::PARAMETER_ROUTE_ID = Action_PARAMETER_PREFIX + "id";
		const string LineStopAddAction::PARAMETER_METRIC_OFFSET = Action_PARAMETER_PREFIX + "mo";
		const string LineStopAddAction::PARAMETER_AREA = Action_PARAMETER_PREFIX + "ar";
		const string LineStopAddAction::PARAMETER_WITH_SCHEDULES = Action_PARAMETER_PREFIX + "with_schedules";
		const string LineStopAddAction::PARAMETER_RESERVATION_NEEDED = Action_PARAMETER_PREFIX + "_reservation_needed";
		const string LineStopAddAction::PARAMETER_DURATION_TO_ADD = Action_PARAMETER_PREFIX + "_duration_to_add";



		ParametersMap LineStopAddAction::getParametersMap() const
		{
			ParametersMap map;
			if(_stop.get())
			{
				map.insert(PARAMETER_CITY_NAME, _stop->getConnectionPlace()->getCity()->getName());
				map.insert(PARAMETER_STOP_NAME, _stop->getConnectionPlace()->getName());
			}
			if(_area.get())
			{
				map.insert(PARAMETER_AREA, _area->getKey());
			}
			if(_route.get())
			{
				map.insert(PARAMETER_ROUTE_ID, _route->getKey());
			}
			map.insert(PARAMETER_RANK, _rank);
			map.insert(PARAMETER_WITH_SCHEDULES, _withSchedules);
			map.insert(PARAMETER_RESERVATION_NEEDED, _reservationNeeded);
			if(!_durationToAdd.is_not_a_date_time())
			{
				map.insert(PARAMETER_DURATION_TO_ADD, _durationToAdd.total_seconds() / 60);
			}
			return map;
		}



		void LineStopAddAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Journey pattern
			try
			{
				_route = JourneyPatternTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_ROUTE_ID), *_env);
				LineStopTableSync::Search(*_env, _route->getKey());
			}
			catch(ObjectNotFoundException<JourneyPattern>&)
			{
				throw ActionException("No such journey pattern");
			}

			// Place
			if(	map.getOptional<string>(PARAMETER_CITY_NAME) &&
				map.getOptional<string>(PARAMETER_STOP_NAME)
			){
				GeographyModule::CityList cities(GeographyModule::GuessCity(map.get<string>(PARAMETER_CITY_NAME), 1));
				if(cities.empty())
				{
					throw ActionException("City not found");
				}
				boost::shared_ptr<City> city(CityTableSync::GetEditable(cities.front()->getKey(), *_env));

				const string place(map.get<string>(PARAMETER_STOP_NAME));
				vector<boost::shared_ptr<StopArea> > stops(
					cities.front()->search<StopArea>(place, 1)
				);
				if(stops.empty())
				{
					throw ActionException("Place not found");
				}
				boost::shared_ptr<StopArea> stop(StopAreaTableSync::GetEditable(stops.front()->getKey(), *_env));
				StopPointTableSync::Search(*_env, stop->getKey());

				if(stop->getPhysicalStops().empty())
				{
					throw ActionException("Commercial stop without physical stop");
				}
				_stop = const_pointer_cast<StopPoint>(_env->getSPtr(stop->getPhysicalStops().begin()->second));
			}
			else if(map.getOptional<RegistryKeyType>(PARAMETER_AREA))
			{
				try
				{
					_area = DRTAreaTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_AREA), *_env);
					if(_area->get<Stops>().empty())
					{
						throw ActionException("The DRT area can not be empty");
					}
				}
				catch(ObjectNotFoundException<DRTArea>&)
				{
					throw ActionException("No such area");
				}
			}
			else
			{
				throw ActionException("The place must be specified");
			}

			// Rank
			_rank = map.getOptional<size_t>(PARAMETER_RANK) ?
				map.get<size_t>(PARAMETER_RANK) : _route->getEdges().size();

			// Metric offset
			if(map.getOptional<double>(PARAMETER_METRIC_OFFSET))
			{
				_metricOffset = map.get<double>(PARAMETER_METRIC_OFFSET);
			}
			else
			{
			}

			// With schedules
			if(_rank == 0 || _rank == _route->getEdges().size())
			{
				_withSchedules = true;
				_durationToAdd = minutes(map.getDefault<long>(PARAMETER_DURATION_TO_ADD, 1));
			}
			else
			{
				_withSchedules = map.getDefault<bool>(PARAMETER_WITH_SCHEDULES, true);
			}

			// Reservation needed
			_reservationNeeded = map.getDefault<bool>(PARAMETER_RESERVATION_NEEDED, true);

			// Store the services in case of update
			_scheduledServices = ScheduledServiceTableSync::Search(
				*_env,
				_route->getKey()
			);
			_continuousServices = ContinuousServiceTableSync::Search(
				*_env, 
				_route->getKey()
			);
		}



		void LineStopAddAction::run(
			Request& request
		){
			//stringstream text;
			//::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);
			//_object->setAttribute(_value);

			// Transaction
			DBTransaction transaction;

			// Add the stop
			boost::shared_ptr<LineStop> lineStop(
				new LineStop(
					0,
					_route.get(),
					_rank,
					true,
					true,
					_metricOffset,
					_stop ?
						static_cast<Registrable&>(*_stop) :
						static_cast<Registrable&>(*_area)
			)	);
			lineStop->set<ScheduleInput>(_withSchedules);
			lineStop->set<ReservationNeeded>(_reservationNeeded);
			LineStopTableSync::InsertStop(*lineStop, transaction);

			// Update each service with an additional schedule at the beginning
			if(_rank == 0)
			{
				// Update the path with a fake line stop
				BOOST_FOREACH(LineStop* edge, _route->getLineStops())
				{
					edge->set<RankInPath>(edge->get<RankInPath>()+1);
				}
				lineStop->link(*_env);

				// Update of the schedules of each existing service
				BOOST_FOREACH(const boost::shared_ptr<ScheduledService>& service, _scheduledServices)
				{
					// Departure schedules update
					SchedulesBasedService::Schedules departureSchedules(service->getDataDepartureSchedules());
					time_duration departureSchedule(*departureSchedules.begin());
					departureSchedule -= _durationToAdd;
					departureSchedules.insert(departureSchedules.begin(), departureSchedule);

					// Arrival schedules update
					SchedulesBasedService::Schedules arrivalSchedules(service->getDataArrivalSchedules());
					time_duration arrivalSchedule(*arrivalSchedules.begin());
					arrivalSchedule -= _durationToAdd;
					arrivalSchedules.insert(arrivalSchedules.begin(), arrivalSchedule);

					// Service update
					service->setDataSchedules(departureSchedules, arrivalSchedules);
				}
				BOOST_FOREACH(const boost::shared_ptr<ContinuousService>& service, _continuousServices)
				{
					// Departure schedules update
					SchedulesBasedService::Schedules departureSchedules(service->getDataDepartureSchedules());
					time_duration departureSchedule(*departureSchedules.begin());
					departureSchedule -= _durationToAdd;
					departureSchedules.insert(departureSchedules.begin(), departureSchedule);

					// Arrival schedules update
					SchedulesBasedService::Schedules arrivalSchedules(service->getDataArrivalSchedules());
					time_duration arrivalSchedule(*arrivalSchedules.begin());
					arrivalSchedule -= _durationToAdd;
					arrivalSchedules.insert(arrivalSchedules.begin(), arrivalSchedule);

					// Service update
					service->setDataSchedules(departureSchedules, arrivalSchedules);
				}
			}
			else if(_rank == _route->getLineStops().size())
			{
				// Update the path with a fake line stop
				lineStop->link(*_env);

				// Update of the schedules of each existing service
				BOOST_FOREACH(const boost::shared_ptr<ScheduledService>& service, _scheduledServices)
				{
					// Departure schedules update
					SchedulesBasedService::Schedules departureSchedules(service->getDataDepartureSchedules());
					time_duration departureSchedule(*departureSchedules.rbegin());
					departureSchedule += _durationToAdd;
					departureSchedules.push_back(departureSchedule);

					// Arrival schedules update
					SchedulesBasedService::Schedules arrivalSchedules(service->getDataArrivalSchedules());
					time_duration arrivalSchedule(*arrivalSchedules.rbegin());
					arrivalSchedule += _durationToAdd;
					arrivalSchedules.push_back(arrivalSchedule);

					// Service update
					service->setDataSchedules(departureSchedules, arrivalSchedules);
				}
				BOOST_FOREACH(const boost::shared_ptr<ContinuousService>& service, _continuousServices)
				{
					// Departure schedules update
					SchedulesBasedService::Schedules departureSchedules(service->getDataDepartureSchedules());
					time_duration departureSchedule(*departureSchedules.rbegin());
					departureSchedule += _durationToAdd;
					departureSchedules.push_back(departureSchedule);

					// Arrival schedules update
					SchedulesBasedService::Schedules arrivalSchedules(service->getDataArrivalSchedules());
					time_duration arrivalSchedule(*arrivalSchedules.rbegin());
					arrivalSchedule += _durationToAdd;
					arrivalSchedules.push_back(arrivalSchedule);

					// Service update
					service->setDataSchedules(departureSchedules, arrivalSchedules);
				}
			}

			// Store the services
			BOOST_FOREACH(const boost::shared_ptr<ScheduledService>& service, _scheduledServices)
			{
				ScheduledServiceTableSync::Save(service.get(), transaction);
			}
			BOOST_FOREACH(const boost::shared_ptr<ContinuousService>& cservice, _continuousServices)
			{
				ContinuousServiceTableSync::Save(cservice.get(), transaction);
			}

			// Run the transaction
			transaction.run();

			//			::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}



		bool LineStopAddAction::isAuthorized(
			const Session* session
		) const {
			//TODO test if the user has sufficient right level for this commercial line
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE, UNKNOWN_RIGHT_LEVEL, string());
		}



		LineStopAddAction::LineStopAddAction():
			_rank(0),
			_metricOffset(0),
			_withSchedules(true),
			_reservationNeeded(true)
		{}
}	}
