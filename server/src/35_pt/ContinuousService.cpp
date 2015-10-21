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
#include "Fare.hpp"
#include "JourneyPatternTableSync.hpp"
#include "LineStopTableSync.h"
#include "Profile.h"
#include "PTUseRuleTableSync.h"
#include "Registry.h"
#include "RollingStock.hpp"
#include "TransportNetworkRight.h"
#include "User.h"

using namespace std;
using namespace boost;
using namespace boost::gregorian;
using namespace boost::posix_time;

namespace synthese
{
	using namespace calendar;
	using namespace db;
	using namespace graph;
	using namespace pt;
	using namespace util;

	CLASS_DEFINITION(ContinuousService, "t017_continuous_services", 17)
	FIELD_DEFINITION_OF_OBJECT(ContinuousService, "continuous_service_id", "continuous_service_ids")

	FIELD_DEFINITION_OF_TYPE(Range, "range", SQL_TIME)
	FIELD_DEFINITION_OF_TYPE(MaxWaitingTime, "max_waiting_time", SQL_TIME)

	namespace pt
	{
		ContinuousService::ContinuousService(
			RegistryKeyType id,
			string serviceNumber,
			Path* path,
			boost::posix_time::time_duration range,
			boost::posix_time::time_duration maxWaitingTime
		):	Registrable(id),
			Object<ContinuousService, ContinuousServiceSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_VALUE_CONSTRUCTOR(ServiceNumber, serviceNumber),
					FIELD_DEFAULT_CONSTRUCTOR(ServiceSchedules),
					FIELD_VALUE_CONSTRUCTOR(ServicePath, path ? boost::optional<JourneyPattern&>(*dynamic_cast<JourneyPattern*>(path)) : boost::none),
					FIELD_VALUE_CONSTRUCTOR(Range, range),
					FIELD_VALUE_CONSTRUCTOR(MaxWaitingTime, maxWaitingTime),
					FIELD_DEFAULT_CONSTRUCTOR(BikeComplianceId),
					FIELD_DEFAULT_CONSTRUCTOR(HandicappedComplianceId),
					FIELD_DEFAULT_CONSTRUCTOR(PedestrianComplianceId),
					FIELD_DEFAULT_CONSTRUCTOR(ServiceDates)
			)	),
			SchedulesBasedService(serviceNumber, path)
		{}



		ContinuousService::~ContinuousService ()
		{
			unlink();
		}



		boost::posix_time::time_duration
		ContinuousService::getMaxWaitingTime () const
		{
			return get<MaxWaitingTime>();
		}



		void
		ContinuousService::setMaxWaitingTime (boost::posix_time::time_duration maxWaitingTime)
		{
			set<MaxWaitingTime>(maxWaitingTime);
		}



		boost::posix_time::time_duration ContinuousService::getRange () const
		{
			return get<Range>();
		}



		void
		ContinuousService::setRange (boost::posix_time::time_duration range)
		{
			set<Range>(range);
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
				time_duration endSchedule(schedule + get<Range>());

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
				schedule = getArrivalSchedule(RTData, edgeIndex) + get<MaxWaitingTime>();
				time_duration endSchedule(schedule + get<Range>());
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
					range = get<Range>() - range;
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
				schedule += get<MaxWaitingTime>();
				servicePointer.setArrivalInformations(
					edge,
					servicePointer.getOriginDateTime() + (schedule - getDepartureSchedule(servicePointer.getRTData(), 0)),
					servicePointer.getOriginDateTime() + (getArrivalSchedule(false, edgeIndex) + get<MaxWaitingTime>() - getDepartureSchedule(servicePointer.getRTData(), 0)),
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
				return getDataDepartureSchedules()[0] + get<Range>();
			}
			return getDepartureSchedule(RTData, rankInPath) + get<Range>();
		}



		time_duration ContinuousService::getArrivalBeginScheduleToIndex(
			bool RTData,
			std::size_t rankInPath
		) const	{
			return getArrivalSchedule(RTData, rankInPath) + get<MaxWaitingTime>();
		}



		time_duration ContinuousService::getArrivalEndScheduleToIndex(
			bool RTData,
			std::size_t rankInPath
		) const	{
			return getArrivalSchedule(RTData, rankInPath) + get<Range>() + get<MaxWaitingTime>();
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
			map.insert(ServiceNumber::FIELD.name, getServiceNumber());
			map.insert(ServiceSchedules::FIELD.name, encodeSchedules(-getMaxWaitingTime()));
			map.insert(
				ServicePath::FIELD.name,
				getPath() ? getPath()->getKey() : 0
			);
			map.insert(
				Range::FIELD.name,
				getRange().total_seconds() / 60
			);
			map.insert(
				MaxWaitingTime::FIELD.name,
				getMaxWaitingTime().total_seconds() / 60
			);

			map.insert(
				BikeComplianceId::FIELD.name,
				(	getRule(USER_BIKE) && dynamic_cast<const PTUseRule*>(getRule(USER_BIKE)) ?
					static_cast<const PTUseRule*>(getRule(USER_BIKE))->getKey() :
					RegistryKeyType(0)
			)	);
			map.insert(
				HandicappedComplianceId::FIELD.name,
				(	getRule(USER_HANDICAPPED) && dynamic_cast<const PTUseRule*>(getRule(USER_HANDICAPPED)) ?
					static_cast<const PTUseRule*>(getRule(USER_HANDICAPPED))->getKey() :
					RegistryKeyType(0)
			)	);
			map.insert(
				PedestrianComplianceId::FIELD.name,
				(	getRule(USER_PEDESTRIAN) && dynamic_cast<const PTUseRule*>(getRule(USER_PEDESTRIAN)) ?
					static_cast<const PTUseRule*>(getRule(USER_PEDESTRIAN))->getKey() :
					RegistryKeyType(0)
			)	);
			map.insert(ServiceDates::FIELD.name, datesStr.str());
		}



		void ContinuousService::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			if (!get<ServiceDates>().empty())
			{
				Calendar valueDates;
				valueDates.setFromSerializedString(
					get<ServiceDates>()
				);
				copyDates(valueDates);
			}

			if (get<ServicePath>())
			{
				setPath(get<ServicePath>().get_ptr());
				if(get<ServicePath>()->getEdges().empty())
				{
					LineStopTableSync::Search(env, get<ServicePath>()->getKey());
				}
			}
			
			// Use rules
			RuleUser::Rules rules(getRules());

			// Bike compliance
			if(get<BikeComplianceId>())
			{
				if(get<BikeComplianceId>() > 0)
				{
					rules[USER_BIKE - USER_CLASS_CODE_OFFSET] = PTUseRuleTableSync::Get(get<BikeComplianceId>(), env).get();
				}
				else
				{
					rules[USER_BIKE - USER_CLASS_CODE_OFFSET] = NULL;
				}
			}
			
			// Handicapped compliance
			if(get<HandicappedComplianceId>())
			{
				if(get<HandicappedComplianceId>() > 0)
				{
					rules[USER_HANDICAPPED - USER_CLASS_CODE_OFFSET] = PTUseRuleTableSync::Get(get<HandicappedComplianceId>(), env).get();
				}
				else
				{
					rules[USER_HANDICAPPED - USER_CLASS_CODE_OFFSET] = NULL;
				}
			}

			// Pedestrian compliance
			if(get<PedestrianComplianceId>())
			{
				if(get<PedestrianComplianceId>() > 0)
				{
					rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = PTUseRuleTableSync::Get(get<PedestrianComplianceId>(), env).get();
				}
				else
				{
					rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = NULL;
				}
			}
			RuleUser::setRules(rules);

			if (!get<ServiceSchedules>().empty())
			{
				try
				{
					string rawSchedule = get<ServiceSchedules>();
					SchedulesBasedService::SchedulesPair value(
						SchedulesBasedService::DecodeSchedules(
							rawSchedule,
							get<MaxWaitingTime>()
					)	);
					setDataSchedules(value.first, value.second);
				}
				catch(SchedulesBasedService::BadSchedulesException&)
				{
					throw Exception("Inconsistent schedules size");
				}
			}

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
			return getDataFirstDepartureSchedule(i) + get<Range>();
		}

		const boost::posix_time::time_duration ContinuousService::getDataLastArrivalSchedule( size_t i ) const
		{
			return getDataFirstArrivalSchedule(i) + get<Range>() + get<MaxWaitingTime>();
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

		void ContinuousService::setRules(const Rules& value)
		{
			RuleUser::setRules(value);
			getRule(USER_BIKE) && dynamic_cast<const PTUseRule*>(getRule(USER_BIKE)) ?
				set<BikeComplianceId>(static_cast<const PTUseRule*>(getRule(USER_BIKE))->getKey()) :
				set<BikeComplianceId>(RegistryKeyType(0));
			getRule(USER_HANDICAPPED) && dynamic_cast<const PTUseRule*>(getRule(USER_HANDICAPPED)) ?
				set<HandicappedComplianceId>(static_cast<const PTUseRule*>(getRule(USER_HANDICAPPED))->getKey()) :
				set<HandicappedComplianceId>(RegistryKeyType(0));
			getRule(USER_PEDESTRIAN) && dynamic_cast<const PTUseRule*>(getRule(USER_PEDESTRIAN)) ?
				set<PedestrianComplianceId>(static_cast<const PTUseRule*>(getRule(USER_PEDESTRIAN))->getKey()) :
				set<PedestrianComplianceId>(RegistryKeyType(0));
		}

		void ContinuousService::setPath(graph::Path* path)
		{
			SchedulesBasedService::setPath(path);
			set<ServicePath>(path
				? boost::optional<JourneyPattern&>(*dynamic_cast<JourneyPattern*>(path))
				: boost::none);
		}

		void ContinuousService::setDataSchedules(
			const Schedules& departureSchedules,
			const Schedules& arrivalSchedules
		)
		{
			SchedulesBasedService::setDataSchedules(departureSchedules, arrivalSchedules);
			set<ServiceSchedules>(encodeSchedules());
		}

		bool ContinuousService::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(security::WRITE);
		}

		bool ContinuousService::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(security::WRITE);
		}

		bool ContinuousService::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(security::DELETE_RIGHT);
		}
	}
}
