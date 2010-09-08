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
		template<> const string Registry<pt::ContinuousService>::KEY("ContinuousService");
	}

	namespace pt
	{
		ContinuousService::ContinuousService(
			RegistryKeyType id,
			string serviceNumber,
			Path* path,
			boost::posix_time::time_duration range,
			boost::posix_time::time_duration maxWaitingTime
		):	Registrable(id),
			SchedulesBasedService(serviceNumber, path)
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
			bool getDeparture,
			size_t userClassRank,
			const Edge& edge,
			const ptime& presenceDateTime,
			bool controlIfTheServiceIsReachable,
			bool inverted
		) const	{

			time_duration schedule;
			ptime actualDateTime(presenceDateTime);
			posix_time::time_duration range;
			size_t edgeIndex(edge.getRankInPath());
			
			if (getDeparture)
			{
				schedule = (RTData ? _RTDepartureSchedules : _departureSchedules).at(edgeIndex);
				time_duration endSchedule(schedule + _range);

				if(	GetTimeOfDay(schedule) <= GetTimeOfDay(endSchedule)
				){
					if (presenceDateTime.time_of_day() > GetTimeOfDay(endSchedule))
					{
						return ServicePointer();
					}
					if (presenceDateTime.time_of_day() < GetTimeOfDay(schedule))
					{
						actualDateTime = ptime(presenceDateTime.date(), GetTimeOfDay(schedule));
					}
				}
				else
				{
					if (presenceDateTime.time_of_day() > GetTimeOfDay(endSchedule)
						&& presenceDateTime.time_of_day() < GetTimeOfDay(schedule)
					){
						actualDateTime = ptime(presenceDateTime.date(), GetTimeOfDay(schedule));
					}
				}

				// Range
				ptime validityEndTime(presenceDateTime.date(), endSchedule);
				range = validityEndTime - actualDateTime;
			}
			else
			{
				schedule = (RTData ? _RTArrivalSchedules : _arrivalSchedules).at(edgeIndex);
				time_duration endSchedule(schedule + _range);
				if (GetTimeOfDay(schedule) <= GetTimeOfDay(endSchedule))
				{
					if (presenceDateTime.time_of_day() < GetTimeOfDay(schedule))
					{
						return ServicePointer();
					}
					if (presenceDateTime.time_of_day() > GetTimeOfDay(endSchedule))
					{
						actualDateTime = ptime(presenceDateTime.date(), GetTimeOfDay(endSchedule));
					}
				}
				else
				{
					if (presenceDateTime.time_of_day() < GetTimeOfDay(endSchedule)
						&& presenceDateTime.time_of_day() > GetTimeOfDay(schedule)
					){
						actualDateTime = ptime(presenceDateTime.date(), GetTimeOfDay(endSchedule));
					}
				}

				// Range
				ptime validityEndTime(presenceDateTime.date(), endSchedule);
				range = validityEndTime - actualDateTime;
			}

			// Origin departure time
			const time_duration& departureSchedule(_departureSchedules.at(0));
			ptime originDateTime(actualDateTime - (schedule - departureSchedule));

			// Date control
			ptime calendarDateTime(originDateTime);
			if(departureSchedule >= hours(24))
			{
				calendarDateTime -= days(floor(float(departureSchedule.total_seconds()) / float(86400)));
			}
			if (!isActive(calendarDateTime.date()))
			{
				return ServicePointer();
			}

			// Saving of the result
			ServicePointer ptr(RTData, userClassRank, *this, originDateTime);
			if(getDeparture)
			{
				ptr.setDepartureInformations(edge, actualDateTime, actualDateTime, *edge.getFromVertex());
			}
			else
			{
				ptr.setArrivalInformations(edge, actualDateTime, actualDateTime, *edge.getFromVertex());
			}
			
			// Reservation control
			if (controlIfTheServiceIsReachable)
			{
				if (ptr.isUseRuleCompliant() == UseRule::RUN_NOT_POSSIBLE)
					return ServicePointer();
			}

			// Range
			ptr.setServiceRange(range);
			return ptr;
		}



		void ContinuousService::completeServicePointer(
			ServicePointer& servicePointer,
			const Edge& edge,
			const AccessParameters&
		) const	{
			size_t edgeIndex(edge.getRankInPath());
			if(servicePointer.getArrivalEdge() == NULL)
			{
				time_duration schedule(
					(servicePointer.getRTData() ? _RTArrivalSchedules : _arrivalSchedules).at(edgeIndex)
				);
				servicePointer.setArrivalInformations(
					edge,
					servicePointer.getOriginDateTime() + (schedule - getDepartureSchedule(servicePointer.getRTData(), 0)),
					servicePointer.getOriginDateTime() + (_arrivalSchedules[edgeIndex] - getDepartureSchedule(servicePointer.getRTData(), 0)),
					*edge.getFromVertex()
				);
			}
			else
			{
				time_duration schedule(
					(servicePointer.getRTData() ? _RTDepartureSchedules : _departureSchedules).at(edgeIndex)
				);
				servicePointer.setDepartureInformations(
					edge,
					servicePointer.getOriginDateTime() + (schedule - getDepartureSchedule(servicePointer.getRTData(), 0)),
					servicePointer.getOriginDateTime() + (_departureSchedules[edgeIndex] - getDepartureSchedule(servicePointer.getRTData(), 0)),
					*edge.getFromVertex()
				);
			}
		}



		time_duration ContinuousService::getDepartureBeginScheduleToIndex(
			bool RTData, size_t rankInPath
		) const	{
			return _departureSchedules.at(rankInPath);
		}



		time_duration ContinuousService::getDepartureEndScheduleToIndex(
			bool RTData,
			std::size_t rankInPath
		) const	{
			return _departureSchedules.at(rankInPath) + _range;
		}



		time_duration ContinuousService::getArrivalBeginScheduleToIndex(
			bool RTData,
			std::size_t rankInPath
		) const	{
			return _arrivalSchedules.at(rankInPath);
		}



		time_duration ContinuousService::getArrivalEndScheduleToIndex(
			bool RTData,
			std::size_t rankInPath
		) const	{
			return _arrivalSchedules.at(rankInPath) + _range;
		}
	}
}
