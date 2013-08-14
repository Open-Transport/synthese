
/** ScheduledService class implementation.
	@file ScheduledService.cpp

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

#include "ScheduledService.h"

#include "AccessParameters.h"
#include "CalendarLink.hpp"
#include "CommercialLine.h"
#include "Edge.h"
#include "Path.h"
#include "Registry.h"
#include "GraphConstants.h"
#include "ImportableTableSync.hpp"
#include "JourneyPatternTableSync.hpp"
#include "LineStopTableSync.h"
#include "NonConcurrencyRule.h"
#include "PTUseRuleTableSync.h"
#include "ScheduledServiceTableSync.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"

using namespace std;
using namespace boost;
using namespace boost::gregorian;
using namespace boost::posix_time;

namespace synthese
{
	using namespace calendar;
	using namespace impex;
	using namespace util;
	using namespace graph;
	using namespace pt;

	namespace util
	{
		template<> const string Registry<pt::ScheduledService>::KEY("ScheduledService");
	}

	namespace pt
	{

		ScheduledService::ScheduledService(
			RegistryKeyType id,
			string serviceNumber,
			Path* path
		):	Registrable(id),
			SchedulesBasedService(serviceNumber, path)
		{
		}



		ScheduledService::~ScheduledService ()
		{}



		bool ScheduledService::isContinuous () const
		{
			return false;
		}



		ServicePointer ScheduledService::getFromPresenceTime(
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
		) const {

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

			// Initializations
			size_t edgeIndex(edge.getRankInPath());

			// Check of real time vertex
			if(	RTData && !allowCanceled && !_RTVertices[edgeIndex])
			{
				return ServicePointer();
			}

			// Actual time
			const time_duration& thSchedule(getDeparture ? _departureSchedules[edgeIndex] : _arrivalSchedules[edgeIndex]);
			const time_duration& rtSchedule(getDeparture ? _RTDepartureSchedules[edgeIndex] : _RTArrivalSchedules[edgeIndex]);
			const time_duration& schedule(RTData ? rtSchedule : thSchedule);
			const time_duration timeOfDay(GetTimeOfDay(schedule));
			if(	(getDeparture && presenceDateTime.time_of_day() > timeOfDay) ||
				(!getDeparture && presenceDateTime.time_of_day() < timeOfDay)
			){
				return ServicePointer();
			}

			// Initializations
			const time_duration& departureSchedule(RTData ? _RTDepartureSchedules[0] : _departureSchedules[0]);
			ptime actualTime(presenceDateTime.date(), timeOfDay);
			ptime originDateTime(actualTime);
			originDateTime += (departureSchedule - schedule);

			// Check of date
			ptime calendarDateTime(originDateTime);
			if(departureSchedule >= hours(24))
			{
				calendarDateTime -= days(static_cast<long>(floor(float(departureSchedule.total_seconds()) / float(86400))));
			}
			if (!isActive(calendarDateTime.date()))
			{
				return ServicePointer();
			}

			// Saving dates
			ServicePointer ptr(THData, RTData, accessParameters.getUserClassRank(), *this, originDateTime);

			if(getDeparture)
			{
				if(RTData && !_RTVertices[edgeIndex])
				{
					ptr.setDepartureInformations(
						edge,
						actualTime,
						ptime(presenceDateTime.date(), GetTimeOfDay(thSchedule))
					);
				}
				else
				{
					ptr.setDepartureInformations(
						edge,
						actualTime,
						ptime(presenceDateTime.date(), GetTimeOfDay(thSchedule)),
						*(RTData ? _RTVertices[edgeIndex] : edge.getFromVertex())
					);
				}
			}
			else
			{
				if(RTData && !_RTVertices[edgeIndex])
				{
					ptr.setArrivalInformations(
						edge,
						actualTime,
						ptime(presenceDateTime.date(), GetTimeOfDay(thSchedule))
					);
				}
				else
				{
					ptr.setArrivalInformations(
						edge,
						actualTime,
						ptime(presenceDateTime.date(), GetTimeOfDay(thSchedule)),
						*(RTData ? _RTVertices[edgeIndex] : edge.getFromVertex())
					);
				}
			}

			// Reservation check
			if(	checkIfTheServiceIsReachable &&
				ptr.isUseRuleCompliant(ignoreReservation) == UseRule::RUN_NOT_POSSIBLE
			){
				return ServicePointer();
			}

			return ptr;
		}



		void ScheduledService::completeServicePointer(
			ServicePointer& servicePointer,
			const Edge& edge,
			const AccessParameters&
		) const	{

			size_t edgeIndex(edge.getRankInPath());
			if(servicePointer.getArrivalEdge() == NULL)
			{
				time_duration schedule(
					getArrivalSchedules(servicePointer.getTHData(),
										servicePointer.getRTData())[edgeIndex]
				);

				servicePointer.setArrivalInformations(
					edge,
					servicePointer.getOriginDateTime() +
						(schedule - getDepartureSchedule(servicePointer.getRTData(), 0)),
					servicePointer.getOriginDateTime() +
						(getArrivalSchedules(true, false)[edgeIndex] -
						 getDepartureSchedule(servicePointer.getRTData(), 0)),
					*(servicePointer.getRTData() ? _RTVertices[edgeIndex] : edge.getFromVertex())
				);
			}
			else
			{
				time_duration schedule(
					getDepartureSchedules(true, servicePointer.getRTData())[edgeIndex]
				);

				servicePointer.setDepartureInformations(
					edge,
					servicePointer.getOriginDateTime() + (schedule - getDepartureSchedule(servicePointer.getRTData(), 0)),
					servicePointer.getOriginDateTime() +
						(getDepartureSchedules(true, false)[edgeIndex] -
						 getDepartureSchedule(servicePointer.getRTData(), 0)),
					*(servicePointer.getRTData() ? _RTVertices[edgeIndex] : edge.getFromVertex())
				);
			}
		}



		time_duration ScheduledService::getDepartureBeginScheduleToIndex(bool RTData, size_t rankInPath) const
		{
			return getDepartureSchedules(true, RTData)[rankInPath];
		}

		time_duration ScheduledService::getDepartureEndScheduleToIndex(bool RTData, size_t rankInPath) const
		{
			return getDepartureSchedules(true, RTData)[rankInPath];
		}

		time_duration ScheduledService::getArrivalBeginScheduleToIndex(bool RTData, size_t rankInPath) const
		{
			return getArrivalSchedules(true, RTData)[rankInPath];
		}

		time_duration ScheduledService::getArrivalEndScheduleToIndex(bool RTData, size_t rankInPath) const
		{
			return getArrivalSchedules(true, RTData)[rankInPath];
		}



		void ScheduledService::setTeam( const std::string& team )
		{
			_team = team;
		}

		std::string ScheduledService::getTeam() const
		{
			return _team;
		}



		graph::UseRule::ReservationAvailabilityType ScheduledService::getReservationAbility(
			const date& date,
			std::size_t userClassRank
		) const {
			AccessParameters ap(userClassRank + USER_CLASS_CODE_OFFSET);

			const Path::Edges& edges(getPath()->getEdges());
			for(Path::Edges::const_reverse_iterator it(edges.rbegin()); it != edges.rend(); ++it)
			{
				if((*it)->isDeparture())
				{
					ServicePointer p(
						getFromPresenceTime(
							ap,
							true,
							false,
							true,
							**it,
							ptime(date, GetTimeOfDay(getDepartureSchedule(false, (*it)->getRankInPath()))),
							false,
							false,
							false,
							false
					)	);
					if(!p.getService()) return UseRule::RESERVATION_FORBIDDEN;
					return getUseRule(userClassRank).getReservationAvailability(p, false);
				}
			}
			assert(false);
			return UseRule::RESERVATION_FORBIDDEN;
		}


		ptime ScheduledService::getReservationDeadLine(
			const date& date,
			std::size_t userClassRank
		) const {
			AccessParameters ap(userClassRank + USER_CLASS_CODE_OFFSET);
			// Pedestrian
			const Path::Edges& edges(getPath()->getEdges());
			for(Path::Edges::const_reverse_iterator it(edges.rbegin()); it != edges.rend(); ++it)
			{
				if((*it)->isDeparture())
				{
					ServicePointer p(getFromPresenceTime(
						ap,
						true,
						false,
						true,
						**it,
						ptime(date, getDepartureSchedule(false, (*it)->getRankInPath())),
						false,
						false,
						false,
						false
					)	);
					return getUseRule(userClassRank).getReservationDeadLine(
						p.getOriginDateTime(),
						p.getDepartureDateTime()
					);
				}
			}
			assert(false);
			return not_a_date_time;
		}


		ServicePointer ScheduledService::getDeparturePosition(
			bool RTdata,
			const AccessParameters& accessParameters,
			const boost::posix_time::ptime& date
		) const	{

			const Edge* edge(_path->getEdge(0));
			ptime originTime(date.date(), minutes(0));

			ServicePointer originPtr(
				getFromPresenceTime(
					accessParameters,
					true,
					RTdata,
					true,
					*edge,
					originTime,
					false,
					false,
					false,
					false
			)	);

			if(!originPtr.getService())
			{
				return originPtr;
			}

			for(edge = edge->getFollowingArrivalForFineSteppingOnly(); edge; edge = edge->getFollowingArrivalForFineSteppingOnly())
			{
				ptime arrivalTime(
					originPtr.getOriginDateTime() + (getArrivalSchedules(true, false)[edge->getRankInPath()] - _departureSchedules[0])
				);

				if(RTdata && !_RTVertices[edge->getRankInPath()])
				{
					continue;
				}

				if(arrivalTime > date)
				{
					edge = edge->getPreviousDepartureForFineSteppingOnly();
					ServicePointer result(true, RTdata, accessParameters.getUserClassRank(), *this, originPtr.getOriginDateTime());
					result.setDepartureInformations(
						*edge,
						originPtr.getOriginDateTime() + (_RTDepartureSchedules[edge->getRankInPath()] - _departureSchedules[0]),
						originPtr.getOriginDateTime() + (_departureSchedules[edge->getRankInPath()] - _departureSchedules[0]),
						*(RTdata ? _RTVertices[edge->getRankInPath()] : edge->getFromVertex())
					);
					return result;
				}
			}
			return originPtr;
		}



		bool ScheduledService::loadFromRecord( const Record& record, util::Env& env )
		{
			bool result(false);

			// Service number
			if(record.isDefined(ScheduledServiceTableSync::COL_SERVICENUMBER))
			{
				string serviceNumber(
					record.get<string>(ScheduledServiceTableSync::COL_SERVICENUMBER)
				);
				if(serviceNumber != getServiceNumber())
				{
					setServiceNumber(serviceNumber);
					result = true;
				}
			}

			// Team
			if(record.isDefined(ScheduledServiceTableSync::COL_TEAM))
			{
				string value(
					record.get<string>(ScheduledServiceTableSync::COL_TEAM)
				);
				if(value != getTeam())
				{
					setTeam(value);
					result = true;
				}
			}

			// Calendar dates
			if(record.isDefined(ScheduledServiceTableSync::COL_DATES))
			{
				Calendar value;
				value.setFromSerializedString(
					record.get<string>(ScheduledServiceTableSync::COL_DATES)
				);
				if(value != *this)
				{
					copyDates(value);
					result = true;
				}
			}

			// Path
			bool pathUpdated(false);
			if(record.isDefined(ScheduledServiceTableSync::COL_PATHID))
			{
				util::RegistryKeyType pathId(
					record.getDefault<RegistryKeyType>(
						ScheduledServiceTableSync::COL_PATHID,
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
					pathUpdated = true;
				}
			}


			// Physical stops
			if(record.isDefined(ScheduledServiceTableSync::COL_STOPS))
			{
				SchedulesBasedService::ServedVertices value(
					decodeStops(
						record.get<string>(ScheduledServiceTableSync::COL_STOPS),
						env
				)	);
				if(value != getVertices(false))
				{
					setVertices(value);
					result = true;
				}
			}


			// Dates to force
			if(record.isDefined(ScheduledServiceTableSync::COL_DATES_TO_FORCE))
			{
				string datesStr(
					record.get<string>(ScheduledServiceTableSync::COL_DATES_TO_FORCE)
				);
				vector<string> datesVec;
				split(datesVec, datesStr, is_any_of(","), token_compress_on);
				Calendar::DatesSet dates;
				BOOST_FOREACH(const string& dateStr, datesVec)
				{
					if(dateStr.empty())
					{
						continue;
					}
					dates.insert(from_string(dateStr));
				}

				if(dates != getDatesToForce())
				{
					setDatesToForce(dates);
					result = true;
				}
			}

			// Dates to bypass
			if(record.isDefined(ScheduledServiceTableSync::COL_DATES_TO_BYPASS))
			{
				string datesStr(
					record.get<string>(ScheduledServiceTableSync::COL_DATES_TO_BYPASS)
				);
				vector<string> datesVec;
				split(datesVec, datesStr, is_any_of(","), token_compress_on);
				Calendar::DatesSet dates;
				BOOST_FOREACH(const string& dateStr, datesVec)
				{
					if(dateStr.empty())
					{
						continue;
					}
					dates.insert(from_string(dateStr));
				}

				if(dates != getDatesToBypass())
				{
					setDatesToBypass(dates);
					result = true;
				}
			}

//			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
//			{
				// Use rules
				RuleUser::Rules rules(getRules());

				// Use rules
				if(record.isDefined(ScheduledServiceTableSync::COL_BIKECOMPLIANCEID))
				{
					RegistryKeyType bikeComplianceId(
						record.getDefault<RegistryKeyType>(
							ScheduledServiceTableSync::COL_BIKECOMPLIANCEID,
							0
					)	);
					const PTUseRule* value(NULL);
					if(bikeComplianceId > 0)
					{
						 value = PTUseRuleTableSync::Get(bikeComplianceId, env).get();
					}
					rules[USER_BIKE - USER_CLASS_CODE_OFFSET] = value;
				}
				if(record.isDefined(ScheduledServiceTableSync::COL_HANDICAPPEDCOMPLIANCEID))
				{
					RegistryKeyType handicappedComplianceId(
						record.getDefault<RegistryKeyType>(
							ScheduledServiceTableSync::COL_HANDICAPPEDCOMPLIANCEID,
							0
					)	);
					const PTUseRule* value(NULL);
					if(handicappedComplianceId > 0)
					{
						value = PTUseRuleTableSync::Get(handicappedComplianceId, env).get();
					}
					rules[USER_HANDICAPPED - USER_CLASS_CODE_OFFSET] = value;
				}
				if(record.isDefined(ScheduledServiceTableSync::COL_PEDESTRIANCOMPLIANCEID))
				{
					RegistryKeyType pedestrianComplianceId(
						record.getDefault<RegistryKeyType>(
							ScheduledServiceTableSync::COL_PEDESTRIANCOMPLIANCEID,
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
//			}

			// Schedules
			if(record.isDefined(ScheduledServiceTableSync::COL_SCHEDULES))
			{
				try
				{
					SchedulesBasedService::SchedulesPair value(
						SchedulesBasedService::DecodeSchedules(
							record.get<string>(ScheduledServiceTableSync::COL_SCHEDULES)
					)	);
					if(	getPath() &&
						getPath()->getEdges().size() != value.second.size()
					){
						throw Exception("Inconsistent schedules size : different from path edges number");
					}
					if(	value.first != _departureSchedules ||
						value.second != _arrivalSchedules
					){
						setSchedules(
							value.first,
							value.second,
							true
						);
						result = true;
					}
				}
				catch(SchedulesBasedService::BadSchedulesException&)
				{
					throw Exception("Inconsistent schedules size");
				}
			}


			// Registration in path
			if( pathUpdated &&
				getPath() &&
				getPath()->getPathGroup())
			{
				getPath()->addService(
					*this,
					&env == &Env::GetOfficialEnv()
				);
				updatePathCalendar();
			}

			// Registration in the line
//			if(linkLevel == ALGORITHMS_OPTIMIZATION_LOAD_LEVEL)
			{
				if(	pathUpdated &&
					getRoute() &&
					getRoute()->getCommercialLine()
				){
					getRoute()->getCommercialLine()->registerService(*this);
			}	}

			// Data source links (at the end of the load to avoid registration of objects which are removed later by an exception)
			if(record.isDefined(ScheduledServiceTableSync::COL_DATASOURCE_LINKS))
			{
				Importable::DataSourceLinks dsl(
					ImportableTableSync::GetDataSourceLinksFromSerializedString(
						record.get<string>(ScheduledServiceTableSync::COL_DATASOURCE_LINKS),
						env
				)	);
				if(dsl != getDataSourceLinks())	
				{
					if(&env == &Env::GetOfficialEnv())
					{
						setDataSourceLinksWithRegistration(dsl);
					}
					else
					{
						setDataSourceLinksWithoutRegistration(dsl);
					}
					result = true;
				}
			}

			return result;
		}

		synthese::SubObjects ScheduledService::getSubObjects() const
		{
			SubObjects r;
			BOOST_FOREACH(CalendarLink* link, getCalendarLinks())
			{
				r.push_back(link);
			}
			return r;
		}
	}
}
