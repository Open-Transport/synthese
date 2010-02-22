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

using namespace std;
using namespace boost;
using namespace boost::gregorian;
using namespace boost::posix_time;

namespace synthese
{
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
			boost::posix_time::time_duration range,
			boost::posix_time::time_duration maxWaitingTime
		):	Registrable(id)
			, NonPermanentService(serviceNumber, path)
			, _range (range)
			, _maxWaitingTime (maxWaitingTime)
		{}



		ContinuousService::~ContinuousService ()
		{}


		    
		boost::posix_time::time_duration
		ContinuousService::getMaxWaitingTime () const
		{
			return _maxWaitingTime;
		}




		void 
		ContinuousService::setMaxWaitingTime (boost::posix_time::time_duration maxWaitingTime)
		{
			_maxWaitingTime = maxWaitingTime;
		}




		boost::posix_time::time_duration ContinuousService::getRange () const
		{
			return _range;
		}

		void 
		ContinuousService::setRange (boost::posix_time::time_duration range)
		{
			_range = range;
		}



		bool 
		ContinuousService::isContinuous () const
		{
			return true;
		}



		ServicePointer ContinuousService::getFromPresenceTime(
			bool RTData,
			AccessDirection method,
			UserClassCode userClass
			, const Edge* edge
			, const ptime& presenceDateTime
			, bool controlIfTheServiceIsReachable
			, bool inverted
		) const	{

			ServicePointer ptr(RTData, method, userClass, edge);
			ptr.setService(this);
			time_duration schedule;
			ptime actualDateTime(presenceDateTime);
			posix_time::time_duration range;
			int edgeIndex(edge->getRankInPath());
			
			if (method == DEPARTURE_TO_ARRIVAL)
			{
				schedule = _departureSchedules.at(edgeIndex).first;
				if (GetTimeOfDay(_departureSchedules.at(edgeIndex).first) <= GetTimeOfDay(_departureSchedules.at(edgeIndex).second))
				{
					if (presenceDateTime.time_of_day() > GetTimeOfDay(_departureSchedules.at(edgeIndex).second))
						return ServicePointer(RTData, DEPARTURE_TO_ARRIVAL, userClass);
					if (presenceDateTime.time_of_day() < GetTimeOfDay(schedule))
					{
						actualDateTime = ptime(presenceDateTime.date(), GetTimeOfDay(schedule));
					}
				}
				else
				{
					if (presenceDateTime.time_of_day() > GetTimeOfDay(_departureSchedules.at(edgeIndex).second)
						&& presenceDateTime.time_of_day() < GetTimeOfDay(_departureSchedules.at(edgeIndex).first)
					){
						actualDateTime = ptime(presenceDateTime.date(), GetTimeOfDay(schedule));
					}
				}
				if (inverted)
				{
					ptime validityEndTime(presenceDateTime.date(), _departureSchedules.at(edgeIndex).first);
					range = actualDateTime - validityEndTime;
				}
				else
				{
					ptime validityEndTime(presenceDateTime.date(), _departureSchedules.at(edgeIndex).second);
					range = validityEndTime - actualDateTime;
				}
			}
			else
			{
				schedule = _arrivalSchedules.at(edgeIndex).first;
				if (GetTimeOfDay(_arrivalSchedules.at(edgeIndex).first) <= GetTimeOfDay(_arrivalSchedules.at(edgeIndex).second))
				{
					if (presenceDateTime.time_of_day() < GetTimeOfDay(_arrivalSchedules.at(edgeIndex).first))
						return ServicePointer(RTData, ARRIVAL_TO_DEPARTURE, userClass);
					if (presenceDateTime.time_of_day() > GetTimeOfDay(_arrivalSchedules.at(edgeIndex).second))
					{
						actualDateTime = ptime(presenceDateTime.date(), GetTimeOfDay(_arrivalSchedules.at(edgeIndex).second));
					}
				}
				else
				{
					if (presenceDateTime.time_of_day() < GetTimeOfDay(_arrivalSchedules.at(edgeIndex).second)
						&& presenceDateTime.time_of_day() > GetTimeOfDay(_arrivalSchedules.at(edgeIndex).first)
					){
						actualDateTime = ptime(presenceDateTime.date(), GetTimeOfDay(_arrivalSchedules.at(edgeIndex).second));
					}
				}
				if (inverted)
				{
					ptime validityEndTime(presenceDateTime.date(), _arrivalSchedules.at(edgeIndex).second);
					range = validityEndTime - actualDateTime;
				}
				else
				{
					ptime validityEndTime(presenceDateTime.date(), _arrivalSchedules.at(edgeIndex).first);
					range = actualDateTime - validityEndTime;
				}
			}

			// Origin departure time
			const time_duration& departureSchedule(_departureSchedules.at(0).first);
			ptime originDateTime(actualDateTime - (schedule - departureSchedule));

			ptime calendarDateTime(originDateTime);
			if(departureSchedule >= hours(24))
			{
				calendarDateTime -= days(floor(float(departureSchedule.total_seconds()) / float(86400)));
			}

			// Date control
			if (!isActive(calendarDateTime.date()))
			{
				return ServicePointer(RTData, method, userClass);
			}

			// Saving of the result
			ptr.setActualTime(actualDateTime);
			ptr.setTheoreticalTime(actualDateTime);
			ptr.setServiceRange(range);
			ptr.setOriginDateTime(originDateTime);

			// Reservation control
			if (controlIfTheServiceIsReachable)
			{
				if (ptr.isUseRuleCompliant() == UseRule::RUN_NOT_POSSIBLE)
					return ServicePointer(RTData, method, userClass);
			}
			else
			{
			}

			return ptr;
		}



		ptime ContinuousService::getLeaveTime(
			const ServicePointer& servicePointer,
			const Edge* edge
		) const	{
			int edgeIndex(edge->getRankInPath());
			time_duration schedule(
				servicePointer.getMethod() == DEPARTURE_TO_ARRIVAL ?
				_arrivalSchedules.at(edgeIndex).first :
				getDepartureSchedule(servicePointer.getRTData(), edgeIndex)
			);
			return servicePointer.getOriginDateTime() + (schedule - getDepartureSchedule(servicePointer.getRTData(), 0));
		}



		time_duration ContinuousService::getDepartureSchedule(bool RTData,
			std::size_t rank) const
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

		time_duration ContinuousService::getDepartureBeginScheduleToIndex(bool RTData, size_t rankInPath) const
		{
			return _departureSchedules.at(rankInPath).first;
		}

		time_duration ContinuousService::getDepartureEndScheduleToIndex(bool RTData,
			std::size_t rankInPath) const
		{
			return _departureSchedules.at(rankInPath).second;
		}

		time_duration ContinuousService::getArrivalBeginScheduleToIndex(bool RTData,
			std::size_t rankInPath) const
		{
			return _arrivalSchedules.at(rankInPath).first;
		}

		time_duration ContinuousService::getArrivalEndScheduleToIndex(bool RTData,
			std::size_t rankInPath) const
		{
			return _arrivalSchedules.at(rankInPath).second;
		}

		const time_duration& ContinuousService::getLastArrivalSchedule(bool RTData) const
		{
			const Schedules::const_iterator it(_arrivalSchedules.end() - 1);
			return it->second;
		}
	}
}
