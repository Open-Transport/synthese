////////////////////////////////////////////////////////////////////////////////
/// ContinuousService class implementation.
///	@file ContinuousService.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "Registry.h"
#include "ContinuousService.h"
#include "Edge.h"

#include "DateTime.h"

using namespace std;

namespace synthese
{
	using namespace time;
	using namespace util;
	using namespace graph;

	namespace util
	{
		template<> const string Registry<env::ContinuousService>::KEY("ContinuousService");
	}

	namespace env
	{
		ContinuousService::ContinuousService(
			RegistryKeyType id,
			string serviceNumber,
			Path* path,
			int range,
			int maxWaitingTime
		)	: Registrable(id)
			, NonPermanentService(serviceNumber, path)
			, _range (range)
			, _maxWaitingTime (maxWaitingTime)
		{	}



		ContinuousService::~ContinuousService ()
		{	}


		    
		int 
		ContinuousService::getMaxWaitingTime () const
		{
			return _maxWaitingTime;
		}




		void 
		ContinuousService::setMaxWaitingTime (int maxWaitingTime)
		{
			_maxWaitingTime = maxWaitingTime;
		}




		int 
		ContinuousService::getRange () const
		{
			return _range;
		}

		void 
		ContinuousService::setRange (int range)
		{
			_range = range;
		}



		bool 
		ContinuousService::isContinuous () const
		{
			return true;
		}



		ServicePointer ContinuousService::getFromPresenceTime(
			AccessDirection method,
			UserClassCode userClass
			, const Edge* edge
			, const time::DateTime& presenceDateTime
			, const time::DateTime& computingTime
			, bool controlIfTheServiceIsReachable
			, bool inverted
		) const	{

			ServicePointer ptr(method, userClass, edge);
			ptr.setService(this);
			Schedule schedule;
			DateTime actualDateTime(presenceDateTime);
			DateTime validityEndTime(presenceDateTime);
			int range;
			int edgeIndex(edge->getRankInPath());
			
			if (method == DEPARTURE_TO_ARRIVAL)
			{
				schedule = _departureSchedules.at(edgeIndex).first;
				if (_departureSchedules.at(edgeIndex).first.getHour() <= _departureSchedules.at(edgeIndex).second.getHour())
				{
					if (presenceDateTime.getHour() > _departureSchedules.at(edgeIndex).second.getHour())
						return ServicePointer(DEPARTURE_TO_ARRIVAL, userClass);
					if (presenceDateTime.getHour() < schedule.getHour())
						actualDateTime.setHour(schedule.getHour());
				}
				else
				{
					if (presenceDateTime.getHour() > _departureSchedules.at(edgeIndex).second.getHour()
						&& presenceDateTime.getHour() < _departureSchedules.at(edgeIndex).first.getHour())
						actualDateTime.setHour(schedule.getHour());
				}
				if (inverted)
				{
					validityEndTime.setHour(_departureSchedules.at(edgeIndex).first.getHour());
					range = actualDateTime - validityEndTime;
				}
				else
				{
					validityEndTime.setHour(_departureSchedules.at(edgeIndex).second.getHour());
					range = validityEndTime - actualDateTime;
				}
			}
			else
			{
				schedule = _arrivalSchedules.at(edgeIndex).first;
				if (_arrivalSchedules.at(edgeIndex).first.getHour() <= _arrivalSchedules.at(edgeIndex).second.getHour())
				{
					if (presenceDateTime.getHour() < _arrivalSchedules.at(edgeIndex).first.getHour())
						return ServicePointer(ARRIVAL_TO_DEPARTURE, userClass);
					if (presenceDateTime.getHour() > _arrivalSchedules.at(edgeIndex).second.getHour())
						actualDateTime.setHour(_arrivalSchedules.at(edgeIndex).second.getHour());
				}
				else
				{
					if (presenceDateTime.getHour() < _arrivalSchedules.at(edgeIndex).second.getHour()
						&& presenceDateTime.getHour() > _arrivalSchedules.at(edgeIndex).first.getHour())
						actualDateTime.setHour(_arrivalSchedules.at(edgeIndex).second.getHour());
				}
				if (inverted)
				{
					validityEndTime.setHour(_arrivalSchedules.at(edgeIndex).second.getHour());
					range = validityEndTime - actualDateTime;
				}
				else
				{
					validityEndTime.setHour(_arrivalSchedules.at(edgeIndex).first.getHour());
					range = actualDateTime - validityEndTime;
				}
			}
			ptr.setActualTime(actualDateTime);
			ptr.setServiceRange(range);

			// Origin departure time
			DateTime originDateTime(actualDateTime);
			int duration = schedule - _departureSchedules.at(0).first;
			originDateTime -= duration;
			ptr.setOriginDateTime(originDateTime);

			// Date control
			if (!isActive(originDateTime.getDate()))
				return ServicePointer(method, userClass);

			// Reservation control
			if (controlIfTheServiceIsReachable)
			{
				if (!ptr.isReservationRuleCompliant(computingTime))
					return ServicePointer(method, userClass);
			}
			else
			{
				/// @todo Implement the reservation control
			}

			return ptr;
		}

		time::DateTime ContinuousService::getLeaveTime(
			const ServicePointer& servicePointer
			, const Edge* edge) const
		{
			int edgeIndex(edge->getRankInPath());
			Schedule schedule(
				(servicePointer.getMethod() == DEPARTURE_TO_ARRIVAL)
				? _arrivalSchedules.at(edgeIndex).first
				: getDepartureSchedule(edgeIndex)
				);
			DateTime actualDateTime(servicePointer.getOriginDateTime());
			actualDateTime += (schedule - getDepartureSchedule());
			return actualDateTime;
		}

		time::Schedule ContinuousService::getDepartureSchedule(int rank) const
		{
			return _departureSchedules.at(rank).first;
		}

		void ContinuousService::setDepartureSchedules( const Schedules& schedules )
		{
			_departureSchedules = schedules;
		}

		void ContinuousService::setArrivalSchedules( const Schedules& schedules )
		{
			_arrivalSchedules = schedules;
		}

		Schedule ContinuousService::getDepartureBeginScheduleToIndex(int rankInPath) const
		{
			return _departureSchedules.at(rankInPath).first;
		}

		Schedule ContinuousService::getDepartureEndScheduleToIndex(int rankInPath) const
		{
			return _departureSchedules.at(rankInPath).second;
		}

		Schedule ContinuousService::getArrivalBeginScheduleToIndex(int rankInPath) const
		{
			return _arrivalSchedules.at(rankInPath).first;
		}

		Schedule ContinuousService::getArrivalEndScheduleToIndex(int rankInPath) const
		{
			return _arrivalSchedules.at(rankInPath).second;
		}

		const time::Schedule& ContinuousService::getLastArrivalSchedule() const
		{
			const Schedules::const_iterator it(_arrivalSchedules.end() - 1);
			return it->second;
		}
	}
}
