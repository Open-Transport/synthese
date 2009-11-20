
/** ScheduledService class implementation.
	@file ScheduledService.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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
#include "Path.h"
#include "Edge.h"
#include "Registry.h"
#include "GraphConstants.h"

#include "Line.h"
#include "CommercialLine.h"
#include "PhysicalStop.h"
#include "NonConcurrencyRule.h"
#include "PublicTransportStopZoneConnectionPlace.h"

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace util;
	using namespace time;
	using namespace graph;

	namespace util
	{
		template<> const string Registry<env::ScheduledService>::KEY("ScheduledService");
	}

	namespace env
	{

		ScheduledService::ScheduledService(
			RegistryKeyType id,
			string serviceNumber,
			Path* path
		):	Registrable(id)
			, NonPermanentService(serviceNumber, path)
		{}



		ScheduledService::~ScheduledService ()
		{}



		    
		bool 
		ScheduledService::isContinuous () const
		{
			return false;
		}


		void ScheduledService::setPath(Path* path )
		{
			Service::setPath(path);
		}


		bool operator==(const ScheduledService& first, const ScheduledService& second)
		{
			return
				first.getPath() == second.getPath() &&
				first.getServiceNumber() == second.getServiceNumber() &&
				first.getDepartureSchedules(false) == second.getDepartureSchedules(false) &&
				first.getArrivalSchedules(false) == second.getArrivalSchedules(false)
			;
		};



		ServicePointer ScheduledService::getFromPresenceTime(
			bool RTData,
			AccessDirection method,
			UserClassCode userClass
			, const Edge* edge
			, const time::DateTime& presenceDateTime
			, bool controlIfTheServiceIsReachable
			, bool inverted
		) const {

			// Initializations
			ServicePointer ptr(RTData,method, userClass, edge);
			ptr.setService(this);
			DateTime actualTime(presenceDateTime);
			Schedule schedule;
			int edgeIndex(edge->getRankInPath());

			// Actual time
			if (method == DEPARTURE_TO_ARRIVAL)
			{
				schedule = _departureSchedules.at(edgeIndex);
				if (presenceDateTime.getHour() > schedule.getHour())
					return ServicePointer(RTData,DEPARTURE_TO_ARRIVAL, userClass);
			}
			if (method == ARRIVAL_TO_DEPARTURE)
			{
				schedule = _arrivalSchedules.at(edgeIndex);
				if (presenceDateTime.getHour() < schedule.getHour())
					return ServicePointer(RTData,ARRIVAL_TO_DEPARTURE, userClass);
			}
			actualTime.setHour(schedule.getHour());
			ptr.setActualTime(actualTime);
			
			// Origin departure time
			DateTime originDateTime(actualTime);
			int duration = schedule - _departureSchedules.at(0);
			originDateTime -= duration;
			ptr.setOriginDateTime(originDateTime);

			// Date control
			if (!isActive(date(originDateTime.getDate().getYear(),originDateTime.getDate().getMonth(),originDateTime.getDate().getDay())))
				return ServicePointer(RTData, method, userClass);

			// Reservation control
			if(	controlIfTheServiceIsReachable &&
				ptr.isUseRuleCompliant() == UseRule::RUN_NOT_POSSIBLE
			){
				return ServicePointer(RTData, method, userClass);
			}		

			return ptr;
		}

		time::DateTime ScheduledService::getLeaveTime(
			const ServicePointer& servicePointer
			, const Edge* edge
		) const	{
			int edgeIndex(edge->getRankInPath());
			Schedule schedule(
				(servicePointer.getMethod() == DEPARTURE_TO_ARRIVAL)
				? getArrivalSchedules(servicePointer.getRTData()).at(edgeIndex)
				: getDepartureSchedules(servicePointer.getRTData()).at(edgeIndex)
				);
			DateTime actualDateTime(servicePointer.getOriginDateTime());
			actualDateTime += (schedule - _departureSchedules.at(0));
			return actualDateTime;
		}

		void ScheduledService::setDepartureSchedules( const Schedules& schedules )
		{
			_departureSchedules = schedules;
			_RTDepartureSchedules = schedules;
		}

		void ScheduledService::setArrivalSchedules( const Schedules& schedules )
		{
			_arrivalSchedules = schedules;
			_RTArrivalSchedules = schedules;
		}

		Schedule ScheduledService::getDepartureSchedule(bool RTData, size_t rank) const
		{
			return getDepartureSchedules(RTData).at(rank);
		}

		Schedule ScheduledService::getDepartureBeginScheduleToIndex(bool RTData, size_t rankInPath) const
		{
			return getDepartureSchedules(RTData).at(rankInPath);
		}

		Schedule ScheduledService::getDepartureEndScheduleToIndex(bool RTData, size_t rankInPath) const
		{
			return getDepartureSchedules(RTData).at(rankInPath);
		}

		Schedule ScheduledService::getArrivalBeginScheduleToIndex(bool RTData, size_t rankInPath) const
		{
			return getArrivalSchedules(RTData).at(rankInPath);
		}

		Schedule ScheduledService::getArrivalEndScheduleToIndex(bool RTData, size_t rankInPath) const
		{
			return getArrivalSchedules(RTData).at(rankInPath);
		}

		const time::Schedule& ScheduledService::getLastArrivalSchedule(bool RTData) const
		{
			Schedules::const_iterator it(getArrivalSchedules(RTData).end() - 1);
			return *it;
		}

		const time::Schedule& ScheduledService::getLastDepartureSchedule(bool RTData) const
		{
			for (Path::Edges::const_reverse_iterator it(getPath()->getEdges().rbegin()); it != getPath()->getEdges().rend(); ++it)
				if ((*it)->isDeparture())
					return getDepartureSchedules(RTData).at((*it)->getRankInPath());
			assert(false);
			return getDepartureSchedules(RTData).at(0);
		}

		void ScheduledService::setTeam( const std::string& team )
		{
			_team = team;
		}

		std::string ScheduledService::getTeam() const
		{
			return _team;
		}
		
		const ScheduledService::Schedules& ScheduledService::getDepartureSchedules(bool RTData) const
		{
			return RTData ? _RTDepartureSchedules : _departureSchedules;
		}



		const ScheduledService::Schedules& ScheduledService::getArrivalSchedules(bool RTData) const
		{
			return RTData ? _RTArrivalSchedules : _arrivalSchedules;
		}

		graph::UseRule::ReservationAvailabilityType ScheduledService::getReservationAbility(
			const Date& date
		) const {
			// Pedestrian
			const Path::Edges& edges(getPath()->getEdges());
			for(Path::Edges::const_reverse_iterator it(edges.rbegin()); it != edges.rend(); ++it)
			{
				if((*it)->isDeparture())
				{
					ServicePointer p(getFromPresenceTime(
						false,
						DEPARTURE_TO_ARRIVAL,
						USER_PEDESTRIAN,
						*it,
						DateTime(
							date,
							getDepartureSchedule(false, (*it)->getRankInPath())
						),
						false,
						false
					)	);
					if(!p.getService()) return UseRule::RESERVATION_FORBIDDEN;
					return getUseRule(USER_PEDESTRIAN).getReservationAvailability(p);
				}
			}
			assert(false);
			return UseRule::RESERVATION_FORBIDDEN;
		}
		
		
		time::DateTime ScheduledService::getReservationDeadLine(
			const Date& date
		) const {
			// Pedestrian
			const Path::Edges& edges(getPath()->getEdges());
			for(Path::Edges::const_reverse_iterator it(edges.rbegin()); it != edges.rend(); ++it)
			{
				if((*it)->isDeparture())
				{
					ServicePointer p(getFromPresenceTime(
						false,
						DEPARTURE_TO_ARRIVAL,
						USER_PEDESTRIAN,
						*it,
						DateTime(
							date,
							getDepartureSchedule(false, (*it)->getRankInPath())
						),
						false,
						false
					)	);
					return getUseRule(USER_PEDESTRIAN).getReservationDeadLine(
						p.getOriginDateTime(),
						p.getActualDateTime()
					);
				}
			}
			assert(false);
			return DateTime(TIME_UNKNOWN);
		}



		bool ScheduledService::nonConcurrencyRuleOK(
			const time::Date& date,
			const Edge& departureEdge,
			const Edge& arrivalEdge,
			UserClassCode userClass
		) const {
			const CommercialLine* line(getRoute()->getCommercialLine());
			if(line->getNonConcurrencyRules().empty()) return true;

			boost::recursive_mutex::scoped_lock serviceLock(_nonConcurrencyCacheMutex);

			_NonConcurrencyCache::const_iterator it(
				_nonConcurrencyCache.find(
					_NonConcurrencyCache::key_type(
						departureEdge.getRankInPath(),
						arrivalEdge.getRankInPath(),
						userClass,
						gregorian::date(date.getYear(), date.getMonth(), date.getDay())
			)	)	);
			if(it != _nonConcurrencyCache.end()) return it->second;

			recursive_mutex::scoped_lock lineLock(line->getNonConcurrencyRulesMutex());

			const CommercialLine::NonConcurrencyRules& rules(line->getNonConcurrencyRules());
			const PublicTransportStopZoneConnectionPlace::PhysicalStops& startStops(
				static_cast<const PhysicalStop*>(departureEdge.getFromVertex())->getConnectionPlace()->getPhysicalStops()
			);
			const Hub* arrivalHub(
				arrivalEdge.getFromVertex()->getHub()
			);

			typedef graph::Edge* (graph::Edge::*PtrEdgeStep) () const;
			PtrEdgeStep step(
				arrivalHub->isConnectionPossible()
				? (&Edge::getFollowingConnectionArrival)
				: (&Edge::getFollowingArrivalForFineSteppingOnly)
			);


			BOOST_FOREACH(const NonConcurrencyRule* rule, rules)
			{
				CommercialLine* priorityLine(rule->getPriorityLine());
				const CommercialLine::Paths& paths(priorityLine->getPaths());
				DateTime minStartTime(date, getDepartureSchedule(false, departureEdge.getRankInPath()));
				minStartTime -= rule->getDelay().minutes();
				DateTime maxStartTime(date, getDepartureSchedule(false, departureEdge.getRankInPath()));
				maxStartTime += rule->getDelay().minutes();

				// Loop on all vertices of the starting place
				BOOST_FOREACH(const PublicTransportStopZoneConnectionPlace::PhysicalStops::value_type& itStartStop, startStops)
				{
					// Loop on all non concurrent paths
					BOOST_FOREACH(const Path* path, paths)
					{
						if(path == getPath()) continue;

						const Vertex::Edges& departureEdges(itStartStop.second->getDepartureEdges());
						Vertex::Edges::const_iterator its(departureEdges.find(path));
						if(its == departureEdges.end()) continue;

						const Edge& startEdge(*its->second);
						// Search a service at the time of the possible

						optional<Edge::DepartureServiceIndex::Value> minServiceIndex;
						ServicePointer serviceInstance(
							startEdge.getNextService(
								userClass,
								minStartTime,
								maxStartTime,
								true,
								minServiceIndex
						)	);
						// If no service, advance to the next path
						if (!serviceInstance.getService()) continue;

						// Path traversal
						for (const Edge* endEdge = (startEdge.*step) ();
							endEdge != NULL; endEdge = (endEdge->*step) ())
						{
							// Found eligible arrival place
							if(endEdge->getHub() == arrivalHub)
							{
								_nonConcurrencyCache.insert(
									make_pair(
										_NonConcurrencyCache::key_type(
											departureEdge.getRankInPath(),
											arrivalEdge.getRankInPath(),
											userClass,
											gregorian::date(date.getYear(), date.getMonth(), date.getDay())
										), false
								)	);
								return false;
							}
						}
					}
				}
			}

			_nonConcurrencyCache.insert(
				make_pair(
					_NonConcurrencyCache::key_type(
						departureEdge.getRankInPath(),
						arrivalEdge.getRankInPath(),
						userClass,
						gregorian::date(date.getYear(), date.getMonth(), date.getDay())
					), true
			)	);
			return true;
		}



		const Line* ScheduledService::getRoute() const
		{
			return static_cast<const Line*>(getPath());
		}



		void ScheduledService::clearNonConcurrencyCache() const
		{
			recursive_mutex::scoped_lock serviceLock(_nonConcurrencyCacheMutex);
			_nonConcurrencyCache.clear();
		}
	}
}
