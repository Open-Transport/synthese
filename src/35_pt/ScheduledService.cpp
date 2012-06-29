
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
#include "CommercialLine.h"
#include "Edge.h"
#include "Path.h"
#include "Registry.h"
#include "GraphConstants.h"
#include "JourneyPattern.hpp"
#include "NonConcurrencyRule.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"

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
			const AccessParameters& accessParameters,
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
			if(	getDeparture && presenceDateTime.time_of_day() > timeOfDay ||
				!getDeparture && presenceDateTime.time_of_day() < timeOfDay
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
			ServicePointer ptr(RTData, accessParameters.getUserClassRank(), *this, originDateTime);

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
					getArrivalSchedules(servicePointer.getRTData())[edgeIndex]
				);

				servicePointer.setArrivalInformations(
					edge,
					servicePointer.getOriginDateTime() + (schedule - _departureSchedules[0]),
					servicePointer.getOriginDateTime() + (getArrivalSchedules(false)[edgeIndex] - _departureSchedules[0]),
					*(servicePointer.getRTData() ? _RTVertices[edgeIndex] : edge.getFromVertex())
				);
			}
			else
			{
				time_duration schedule(
					getDepartureSchedules(servicePointer.getRTData())[edgeIndex]
				);

				servicePointer.setDepartureInformations(
					edge,
					servicePointer.getOriginDateTime() + (schedule - _departureSchedules[0]),
					servicePointer.getOriginDateTime() + (getDepartureSchedules(false)[edgeIndex] - _departureSchedules[0]),
					*(servicePointer.getRTData() ? _RTVertices[edgeIndex] : edge.getFromVertex())
				);
			}
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



		bool ScheduledService::nonConcurrencyRuleOK(
			const date& date,
			const Edge& departureEdge,
			const Edge& arrivalEdge,
			size_t userClassRank
		) const {
			const CommercialLine* line(getRoute()->getCommercialLine());
			if(line->getNonConcurrencyRules().empty()) return true;

			boost::recursive_mutex::scoped_lock serviceLock(_nonConcurrencyCacheMutex);

			_NonConcurrencyCache::const_iterator it(
				_nonConcurrencyCache.find(
					_NonConcurrencyCache::key_type(
						&departureEdge,
						&arrivalEdge,
						userClassRank,
						date
			)	)	);
			if(it != _nonConcurrencyCache.end()) return it->second;

			recursive_mutex::scoped_lock lineLock(line->getNonConcurrencyRulesMutex());

			const CommercialLine::NonConcurrencyRules& rules(line->getNonConcurrencyRules());
			const StopArea::PhysicalStops& startStops(
				static_cast<const StopPoint*>(departureEdge.getFromVertex())->getConnectionPlace()->getPhysicalStops()
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
				BOOST_FOREACH(const StopArea::PhysicalStops::value_type& itStartStop, startStops)
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
							AccessParameters ap(userClassRank + USER_CLASS_CODE_OFFSET);
							optional<Edge::DepartureServiceIndex::Value> minServiceIndex;
							ServicePointer serviceInstance(
								startEdge.getNextService(
									ap,
									minStartTime,
									maxStartTime,
									true,
									minServiceIndex,
									false,
									true
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
												&departureEdge,
												&arrivalEdge,
												userClassRank,
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
						&departureEdge,
						&arrivalEdge,
						userClassRank,
						date
					), true
			)	);
			return true;
		}



		const JourneyPattern* ScheduledService::getRoute() const
		{
			return static_cast<const JourneyPattern*>(getPath());
		}



		void ScheduledService::clearNonConcurrencyCache() const
		{
			recursive_mutex::scoped_lock serviceLock(_nonConcurrencyCacheMutex);
			_nonConcurrencyCache.clear();
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
					originPtr.getOriginDateTime() + (getArrivalSchedules(false)[edge->getRankInPath()] - _departureSchedules[0])
				);

				if(RTdata && !_RTVertices[edge->getRankInPath()])
				{
					continue;
				}

				if(arrivalTime > date)
				{
					edge = edge->getPreviousDepartureForFineSteppingOnly();
					ServicePointer result(RTdata, accessParameters.getUserClassRank(), *this, originPtr.getOriginDateTime());
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
	}
}
