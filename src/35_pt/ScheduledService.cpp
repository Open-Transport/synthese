
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
using namespace boost::posix_time;

namespace synthese
{
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



		bool operator==(const ScheduledService& first, const ScheduledService& second)
		{
			return
				first.getPath() == second.getPath() &&
				first.getServiceNumber() == second.getServiceNumber() &&
				first.getDepartureSchedules(false) == second.getDepartureSchedules(false) &&
				first.getArrivalSchedules(false) == second.getArrivalSchedules(false)
			;
		}



		ServicePointer ScheduledService::getFromPresenceTime(
			bool RTData,
			AccessDirection method,
			size_t userClass
			, const Edge* edge
			, const ptime& presenceDateTime
			, bool controlIfTheServiceIsReachable
			, bool inverted
		) const {

			// Initializations
			ServicePointer ptr(RTData,method, userClass, edge);
			ptr.setService(this);
			int edgeIndex(edge->getRankInPath());
			const time_duration& thSchedule(method == DEPARTURE_TO_ARRIVAL ? _departureSchedules[edgeIndex] : _arrivalSchedules[edgeIndex]);
			const time_duration& rtSchedule(method == DEPARTURE_TO_ARRIVAL ? _RTDepartureSchedules[edgeIndex] : _RTArrivalSchedules[edgeIndex]);
			const time_duration& schedule(RTData ? rtSchedule : thSchedule);
			const time_duration timeOfDay(GetTimeOfDay(schedule));

			// Actual time
			if(	method == DEPARTURE_TO_ARRIVAL && presenceDateTime.time_of_day() > timeOfDay ||
				method == ARRIVAL_TO_DEPARTURE && presenceDateTime.time_of_day() < timeOfDay
			){
				return ServicePointer(RTData, method, userClass);
			}
			ptime actualTime(presenceDateTime.date(), timeOfDay);
			const time_duration& departureSchedule(RTData ? _RTDepartureSchedules[0] : _departureSchedules[0]);

			ptime originDateTime(actualTime);
			originDateTime += (departureSchedule - schedule);

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

			// Saving dates
			ptr.setActualTime(actualTime);
			ptr.setOriginDateTime(originDateTime);

			// Reservation control
			if(	controlIfTheServiceIsReachable &&
				ptr.isUseRuleCompliant() == UseRule::RUN_NOT_POSSIBLE
			){
				return ServicePointer(RTData, method, userClass);
			}		

			// Theoretical time
			ptr.setTheoreticalTime(ptime(presenceDateTime.date(), GetTimeOfDay(thSchedule)));

			// Real time edge
			if(RTData)
			{
				ptr.setRealTimeVertex(_RTVertices[edgeIndex]);
			}
			return ptr;
		}



		ptime ScheduledService::getLeaveTime(
			const ServicePointer& servicePointer,
			const Edge* edge
		) const	{
			int edgeIndex(edge->getRankInPath());
			time_duration schedule(
				(servicePointer.getMethod() == DEPARTURE_TO_ARRIVAL)
				? getArrivalSchedules(servicePointer.getRTData())[edgeIndex]
				: getDepartureSchedules(servicePointer.getRTData())[edgeIndex]
			);
			return servicePointer.getOriginDateTime() + (schedule - _departureSchedules[0]);
		}



		time_duration ScheduledService::getDepartureBeginScheduleToIndex(bool RTData, size_t rankInPath) const
		{
			return getDepartureSchedules(RTData)[rankInPath];
		}

		time_duration ScheduledService::getDepartureEndScheduleToIndex(bool RTData, size_t rankInPath) const
		{
			return getDepartureSchedules(RTData)[rankInPath];
		}

		time_duration ScheduledService::getArrivalBeginScheduleToIndex(bool RTData, size_t rankInPath) const
		{
			return getArrivalSchedules(RTData)[rankInPath];
		}

		time_duration ScheduledService::getArrivalEndScheduleToIndex(bool RTData, size_t rankInPath) const
		{
			return getArrivalSchedules(RTData)[rankInPath];
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
			const date& date
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
						ptime(date, GetTimeOfDay(getDepartureSchedule(false, (*it)->getRankInPath()))),
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
		
		
		ptime ScheduledService::getReservationDeadLine(
			const date& date
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
						ptime(date, getDepartureSchedule(false, (*it)->getRankInPath())),
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
			return not_a_date_time;
		}



		bool ScheduledService::nonConcurrencyRuleOK(
			const date& date,
			const Edge& departureEdge,
			const Edge& arrivalEdge,
			size_t userClass
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
						date
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
				ptime minStartTime(date, getDepartureSchedule(false, departureEdge.getRankInPath()));
				minStartTime -= rule->getDelay();
				ptime maxStartTime(date, getDepartureSchedule(false, departureEdge.getRankInPath()));
				maxStartTime += rule->getDelay();

				// Loop on all vertices of the starting place
				BOOST_FOREACH(const PublicTransportStopZoneConnectionPlace::PhysicalStops::value_type& itStartStop, startStops)
				{
					// Loop on all non concurrent paths
					BOOST_FOREACH(const Path* path, paths)
					{
						if(path == getPath()) continue;

						const Vertex::Edges& departureEdges(itStartStop.second->getDepartureEdges());
						pair<Vertex::Edges::const_iterator, Vertex::Edges::const_iterator> range(departureEdges.equal_range(path));
						if(range.first == departureEdges.end() || range.first->first != path)
						{
							continue;
						}

						for(Vertex::Edges::const_iterator its(range.first); its != range.second; ++its)
						{
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
										date
										), false
										)	);
									return false;
								}
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
						date
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
