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
#include "CalendarLink.hpp"
#include "CommercialLine.h"
#include "ContinuousServiceTableSync.h"
#include "DBConstants.h"
#include "Destination.hpp"
#include "JourneyPatternTableSync.hpp"
#include "LineStopTableSync.h"
#include "PTUseRuleTableSync.h"
#include "Registry.h"
#include "RollingStock.hpp"

using namespace std;
using namespace boost;
using namespace boost::gregorian;
using namespace boost::posix_time;

namespace synthese
{
	using namespace calendar;
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
		{
			unlink();
		}



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
			bool allowCanceled,
			UseRule::ReservationDelayType reservationRulesDelayType
		) const	{

			// Check of access parameters
			if(!isCompatibleWith(accessParameters))
			{
				return ServicePointer();
			}

			// Check Theoretical and Real Time validity
			if(!THData && RTData && !hasRealTimeData())
			{
				return ServicePointer();
			}

			// Check of real time vertex
			if(	RTData && edge.getRankInPath() < _RTVertices.size() && !_RTVertices[edge.getRankInPath()])
			{
				return ServicePointer();
			}

			time_duration schedule;
			ptime actualDateTime(presenceDateTime);
			posix_time::time_duration range;
			size_t edgeIndex(edge.getRankInPath());

			if (getDeparture)
			{
				schedule = getDepartureSchedule(RTData, edgeIndex);
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
				schedule = getArrivalSchedule(RTData, edgeIndex) + _maxWaitingTime;
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
					if(	presenceDateTime.time_of_day() > GetTimeOfDay(endSchedule) &&
						presenceDateTime.time_of_day() < GetTimeOfDay(schedule)
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
			const time_duration& departureSchedule(getDepartureSchedule(false, 0));
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
					getArrivalSchedule(servicePointer.getRTData(), edgeIndex)
				);
				schedule += _maxWaitingTime;
				servicePointer.setArrivalInformations(
					edge,
					servicePointer.getOriginDateTime() + (schedule - getDepartureSchedule(servicePointer.getRTData(), 0)),
					servicePointer.getOriginDateTime() + (getArrivalSchedule(false, edgeIndex) + _maxWaitingTime - getDepartureSchedule(servicePointer.getRTData(), 0)),
					*edge.getFromVertex()
				);
			}
			else
			{
				time_duration schedule(
					getDepartureSchedule(servicePointer.getRTData(), edgeIndex)
				);
				servicePointer.setDepartureInformations(
					edge,
					servicePointer.getOriginDateTime() + (schedule - getDepartureSchedule(servicePointer.getRTData(), 0)),
					servicePointer.getOriginDateTime() + (getDepartureSchedule(false, edgeIndex) - getDepartureSchedule(servicePointer.getRTData(), 0)),
					*edge.getFromVertex()
				);
			}
		}



		time_duration ContinuousService::getDepartureBeginScheduleToIndex(
			bool RTData, size_t rankInPath
		) const	{
			if(rankInPath == 0 && !RTData)
			{
				return getDataDepartureSchedules()[0];
			}
			return getDepartureSchedule(RTData, rankInPath);
		}



		time_duration ContinuousService::getDepartureEndScheduleToIndex(
			bool RTData,
			std::size_t rankInPath
		) const	{
			if(rankInPath == 0 && !RTData)
			{
				return getDataDepartureSchedules()[0] + _range;
			}
			return getDepartureSchedule(RTData, rankInPath) + _range;
		}



		time_duration ContinuousService::getArrivalBeginScheduleToIndex(
			bool RTData,
			std::size_t rankInPath
		) const	{
			return getArrivalSchedule(RTData, rankInPath) + _maxWaitingTime;
		}



		time_duration ContinuousService::getArrivalEndScheduleToIndex(
			bool RTData,
			std::size_t rankInPath
		) const	{
			return getArrivalSchedule(RTData, rankInPath) + _range + _maxWaitingTime;
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



		bool ContinuousService::loadFromRecord( const Record& record, util::Env& env )
		{
			bool result(false);

			// Service number
			if(record.isDefined(ContinuousServiceTableSync::COL_SERVICENUMBER))
			{
				string serviceNumber(
					record.get<string>(ContinuousServiceTableSync::COL_SERVICENUMBER)
				);
				if(serviceNumber != getServiceNumber())
				{
					setServiceNumber(serviceNumber);
					result = true;
				}
			}

			// Range
			if(record.isDefined(ContinuousServiceTableSync::COL_RANGE))
			{
				boost::posix_time::time_duration range(
					minutes(record.getDefault<long>(ContinuousServiceTableSync::COL_RANGE, 0))
				);
				if(range != getRange())
				{
					setRange(range);
					result = true;
				}
			}

			// Max waiting time
			if(record.isDefined(ContinuousServiceTableSync::COL_MAXWAITINGTIME))
			{
				boost::posix_time::time_duration maxWaitingTime(
					minutes(record.getDefault<long>(ContinuousServiceTableSync::COL_MAXWAITINGTIME, 0))
				);
				if(maxWaitingTime != getMaxWaitingTime())
				{
					setMaxWaitingTime(maxWaitingTime);
					result = true;
				}
			}

			// Calendar dates
			if(record.isDefined(ContinuousServiceTableSync::COL_DATES))
			{
				Calendar value;
				value.setFromSerializedString(
					record.get<string>(ContinuousServiceTableSync::COL_DATES)
				);
				if(value.getMarkedDates() != getMarkedDates())
				{
					copyDates(value);
					result = true;
				}
			}


			// Path
			if(record.isDefined(ContinuousServiceTableSync::COL_PATHID))
			{
				util::RegistryKeyType pathId(
					record.getDefault<RegistryKeyType>(
						ContinuousServiceTableSync::COL_PATHID,
						0
				)	);
				Path* path(
					JourneyPatternTableSync::GetEditable(pathId, env).get()
				);

				if(path != getPath())
				{
					setPath(path);
					if(path->getEdges().empty())
					{
						LineStopTableSync::Search(env, pathId);
					}
					result = true;
				}
			}

			// Use rules
			RuleUser::Rules rules(RuleUser::GetEmptyRules());

//			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				// Use rules
				RuleUser::Rules rules(getRules());

				// Use rules
				if(record.isDefined(ContinuousServiceTableSync::COL_BIKE_USE_RULE))
				{
					RegistryKeyType bikeComplianceId(
						record.getDefault<RegistryKeyType>(
							ContinuousServiceTableSync::COL_BIKE_USE_RULE,
							0
					)	);
					const PTUseRule* value(NULL);
					if(bikeComplianceId > 0)
					{
						 value = PTUseRuleTableSync::Get(bikeComplianceId, env).get();
					}
					rules[USER_BIKE - USER_CLASS_CODE_OFFSET] = value;
				}
				if(record.isDefined(ContinuousServiceTableSync::COL_HANDICAPPED_USE_RULE))
				{
					RegistryKeyType handicappedComplianceId(
						record.getDefault<RegistryKeyType>(
							ContinuousServiceTableSync::COL_HANDICAPPED_USE_RULE,
							0
					)	);
					const PTUseRule* value(NULL);
					if(handicappedComplianceId > 0)
					{
						value = PTUseRuleTableSync::Get(handicappedComplianceId, env).get();
					}
					rules[USER_HANDICAPPED - USER_CLASS_CODE_OFFSET] = value;
				}
				if(record.isDefined(ContinuousServiceTableSync::COL_PEDESTRIAN_USE_RULE))
				{
					RegistryKeyType pedestrianComplianceId(
						record.getDefault<RegistryKeyType>(
							ContinuousServiceTableSync::COL_PEDESTRIAN_USE_RULE,
							0
					)	);
					const PTUseRule* value(NULL);
					if(pedestrianComplianceId > 0)
					{
						value = PTUseRuleTableSync::Get(pedestrianComplianceId, env).get();
					}
					rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = value;
				}
				if(rules != getRules())
				{
					setRules(rules);
					result = true;
				}
			}

			// Schedules
			if(record.isDefined(ContinuousServiceTableSync::COL_SCHEDULES))
			{
				try
				{
					string rawSchedule = record.get<string>(ContinuousServiceTableSync::COL_SCHEDULES);
					SchedulesBasedService::SchedulesPair value(
						SchedulesBasedService::DecodeSchedules(
							rawSchedule,
							_maxWaitingTime
					)	);
					if(	value.first != getDataDepartureSchedules() ||
						value.second != getDataArrivalSchedules()
					){
						result = true;
						setDataSchedules(value.first, value.second);
					}
				}
				catch(SchedulesBasedService::BadSchedulesException&)
				{
					throw Exception("Inconsistent schedules size");
				}
			}

			return result;
		}



		void ContinuousService::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			// Registration in path
			if( getPath()&&
				!getPath()->contains(*this))
			{
				getPath()->addService(
					*this,
					true
				);
				updatePathCalendar();
			}

			// Registration in the line
//			if(linkLevel == ALGORITHMS_OPTIMIZATION_LOAD_LEVEL)
			{
				if(	getRoute() &&
					getRoute()->getCommercialLine()
				){
					getRoute()->getCommercialLine()->registerService(*this);
			}	}

			// Clear cache in case of non detected change in external objects (like path edges number)
			_clearGeneratedSchedules();
		}



		synthese::SubObjects ContinuousService::getSubObjects() const
		{
			SubObjects r;
			BOOST_FOREACH(CalendarLink* link, getCalendarLinks())
			{
				r.push_back(link);
			}
			return r;
		}



		synthese::LinkedObjectsIds ContinuousService::getLinkedObjectsIds( const Record& record ) const
		{
			return LinkedObjectsIds();
		}

		const boost::posix_time::time_duration ContinuousService::getDataLastDepartureSchedule( size_t i ) const
		{
			return getDataFirstDepartureSchedule(i) + _range;
		}

		const boost::posix_time::time_duration ContinuousService::getDataLastArrivalSchedule( size_t i ) const
		{
			return getDataFirstArrivalSchedule(i) + _range + _maxWaitingTime;
		}



		void ContinuousService::unlink()
		{
			// Unregister from the route
			if(getPath())
			{
				getPath()->removeService(*this);
	}

			// Unregister from the line
			if(	getRoute() &&
				getRoute()->getCommercialLine()
			){
				getRoute()->getCommercialLine()->unregisterService(*this);
}
		}
	}
}
