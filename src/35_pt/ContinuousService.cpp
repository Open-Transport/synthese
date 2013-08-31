////////////////////////////////////////////////////////////////////////////////
/// ContinuousService class implementation.
///	@file ContinuousService.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "ContinuousService.h"

#include "AccessParameters.h"
#include "ContinuousServiceTableSync.h"
#include "DBConstants.h"
#include "Edge.h"
#include "JourneyPattern.hpp"
#include "PTUseRule.h"
#include "Registry.h"

using namespace std;
using namespace boost;
using namespace boost::gregorian;
using namespace boost::posix_time;

namespace synthese
{
	using namespace db;
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
			const AccessParameters& accessParameters,
			bool THData,
			bool RTData,
			bool getDeparture,
			const Edge& edge,
			const ptime& presenceDateTime,
			bool checkIfTheServiceIsReachable,
			bool inverted,
			bool ignoreReservation,
			bool allowCanceled
		) const	{

			// Check of access parameters
			if(!isCompatibleWith(accessParameters))
			{
				return ServicePointer();
			}

			// Check Theoretical and Real Time validity
			if(!THData && RTData && !_hasRealTimeData)
			{
				return ServicePointer();
			}

			// Check of real time vertex
			if(	RTData && !_RTVertices[edge.getRankInPath()])
			{
				return ServicePointer();
			}

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
						//If waiting time > 2hours, we are after the endSchedule and not before the schedule
						if (GetTimeOfDay(schedule) - presenceDateTime.time_of_day() < hours(2))
							actualDateTime = ptime(presenceDateTime.date(), GetTimeOfDay(schedule));
						else
							return ServicePointer();
					}
				}

				// Range
				if(inverted)
				{
					ptime validityBeginTime(presenceDateTime.date(), schedule);
					range = actualDateTime - validityBeginTime;
				}
				else
				{
					if(actualDateTime.time_of_day() <= GetTimeOfDay(endSchedule))
					{
						range = GetTimeOfDay(endSchedule) - actualDateTime.time_of_day();
					}
					else
					{
						time_duration beforeMidnight = hours(24) - actualDateTime.time_of_day();
						time_duration afterMidnight = GetTimeOfDay(endSchedule);
						range = beforeMidnight + afterMidnight;
					}
				}
			}
			else
			{
				schedule = (RTData ? _RTArrivalSchedules : _arrivalSchedules).at(edgeIndex) + _maxWaitingTime;
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
					if (presenceDateTime.time_of_day() > GetTimeOfDay(endSchedule)
						&& presenceDateTime.time_of_day() < GetTimeOfDay(schedule)
					){
						// If (reversed) waiting time > 2hours, we are before schedule and not after endSchedule
						if(presenceDateTime.time_of_day() - GetTimeOfDay(endSchedule) > hours(2))
							return ServicePointer();
						else
							actualDateTime = ptime(presenceDateTime.date(), GetTimeOfDay(endSchedule));
					}
				}

				// Range
				if(inverted)
				{
					if(actualDateTime.time_of_day() <= GetTimeOfDay(endSchedule))
					{
						if (GetTimeOfDay(schedule) > GetTimeOfDay(endSchedule))
						{
							time_duration beforeMidnight = hours(24) - GetTimeOfDay(schedule);
							time_duration afterMidnight = actualDateTime.time_of_day();
							range = beforeMidnight + afterMidnight;
						}
						else
						{
							range = actualDateTime.time_of_day() - GetTimeOfDay(schedule);
						}
					}
					else
					{
						range = actualDateTime.time_of_day() - GetTimeOfDay(schedule);
					}
					range = _range - range;
				}
				else
				{
					ptime validityBeginTime(presenceDateTime.date(), schedule);
					range = actualDateTime - validityBeginTime;
				}
			}

			// Origin departure time
			const time_duration& departureSchedule(_departureSchedules.at(0));
			ptime originDateTime(actualDateTime - (schedule - departureSchedule));
			if(	(!getDeparture && !inverted) ||
				(getDeparture && inverted)
			){
				originDateTime -= range;
			}

			// Date check
			ptime calendarDateTime(originDateTime);
			if(departureSchedule >= hours(24))
			{
				calendarDateTime -= days(static_cast<long>(floor(float(departureSchedule.total_seconds()) / float(86400))));
			}
			if (!isActive(calendarDateTime.date()))
			{
				return ServicePointer();
			}

			// Saving of the result
			ServicePointer ptr(THData, RTData, accessParameters.getUserClassRank(), *this, originDateTime);
			if(getDeparture)
			{
				ptime dateTime(actualDateTime);
				if(inverted)
				{
					dateTime -= range;
				}
				ptr.setDepartureInformations(edge, dateTime, dateTime, *edge.getFromVertex());
			}
			else
			{
				ptime dateTime(actualDateTime);
				if(!inverted)
				{
					dateTime -= range;
				}
				ptr.setArrivalInformations(edge, dateTime, dateTime, *edge.getFromVertex());
			}

			// Reservation check
			if(checkIfTheServiceIsReachable)
			{
				if (ptr.isUseRuleCompliant(ignoreReservation) == UseRule::RUN_NOT_POSSIBLE)
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
				schedule += _maxWaitingTime;
				servicePointer.setArrivalInformations(
					edge,
					servicePointer.getOriginDateTime() + (schedule - getDepartureSchedule(servicePointer.getRTData(), 0)),
					servicePointer.getOriginDateTime() + (_arrivalSchedules[edgeIndex] + _maxWaitingTime - getDepartureSchedule(servicePointer.getRTData(), 0)),
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
			return _arrivalSchedules.at(rankInPath) + _maxWaitingTime;
		}



		time_duration ContinuousService::getArrivalEndScheduleToIndex(
			bool RTData,
			std::size_t rankInPath
		) const	{
			return _arrivalSchedules.at(rankInPath) + _range + _maxWaitingTime;
		}



		void ContinuousService::toParametersMap( util::ParametersMap& map, bool withAdditionalParameters, boost::logic::tribool withFiles /*= boost::logic::indeterminate*/, std::string prefix /*= std::string() */ ) const
		{
			Service::toParametersMap(map, withAdditionalParameters, withFiles, prefix);

			// Dates preparation
			stringstream datesStr;
			if(getCalendarLinks().empty())
			{
				serialize(datesStr);
			}

			map.insert(TABLE_COL_ID, getKey());
			map.insert(ContinuousServiceTableSync::COL_SERVICENUMBER, getServiceNumber());
			map.insert(ContinuousServiceTableSync::COL_SCHEDULES, encodeSchedules(-getMaxWaitingTime()));
			map.insert(
				ContinuousServiceTableSync::COL_PATHID, 
				getPath() ? getPath()->getKey() : 0
			);
			map.insert(
				ContinuousServiceTableSync::COL_RANGE,
				getRange().total_seconds() / 60
			);
			map.insert(
				ContinuousServiceTableSync::COL_MAXWAITINGTIME,
				getMaxWaitingTime().total_seconds() / 60
			);

			map.insert(
				ContinuousServiceTableSync::COL_BIKE_USE_RULE,
				(	getRule(USER_BIKE) && dynamic_cast<const PTUseRule*>(getRule(USER_BIKE)) ?
					static_cast<const PTUseRule*>(getRule(USER_BIKE))->getKey() :
					RegistryKeyType(0)
			)	);
			map.insert(
				ContinuousServiceTableSync::COL_HANDICAPPED_USE_RULE,
				(	getRule(USER_HANDICAPPED) && dynamic_cast<const PTUseRule*>(getRule(USER_HANDICAPPED)) ?
					static_cast<const PTUseRule*>(getRule(USER_HANDICAPPED))->getKey() :
					RegistryKeyType(0)
			)	);
			map.insert(
				ContinuousServiceTableSync::COL_PEDESTRIAN_USE_RULE,
				(	getRule(USER_PEDESTRIAN) && dynamic_cast<const PTUseRule*>(getRule(USER_PEDESTRIAN)) ?
					static_cast<const PTUseRule*>(getRule(USER_PEDESTRIAN))->getKey() :
					RegistryKeyType(0)
			)	);
			map.insert(ContinuousServiceTableSync::COL_DATES, datesStr.str());
		}
	}
}
