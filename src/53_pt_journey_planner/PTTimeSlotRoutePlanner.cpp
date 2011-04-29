
/** PTTimeSlotRoutePlanner class implementation.
	@file PTTimeSlotRoutePlanner.cpp

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

#include "PTTimeSlotRoutePlanner.h"
#include "PTModule.h"
#include "RoadModule.h"
#include "Place.h"
#include "Log.h"
#include "JourneysResult.h"
#include "VertexAccessMap.h"
#include "IntegralSearcher.h"
#include "StopPoint.hpp"
#include "NamedPlace.h"

#include <sstream>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace pt;
	using namespace road;
	using namespace graph;
	using namespace geography;
	using namespace algorithm;
	using namespace util;
	using namespace pt;

	namespace pt_journey_planner
	{
		PTTimeSlotRoutePlanner::PTTimeSlotRoutePlanner(
			const geography::Place* origin,
			const geography::Place* destination,
			const ptime& lowerDepartureTime,
			const ptime& higherDepartureTime,
			const ptime& lowerArrivalTime,
			const ptime& higherArrivalTime,
			const boost::optional<std::size_t> maxSolutionsNumber,
			const graph::AccessParameters accessParameters,
			const PlanningOrder planningOrder,
			bool ignoreReservation,
			std::ostream* logStream
		):	TimeSlotRoutePlanner(
				origin->getVertexAccessMap(
					accessParameters, PTModule::GRAPH_ID, RoadModule::GRAPH_ID, 0
				),
				destination->getVertexAccessMap(
					accessParameters, PTModule::GRAPH_ID, RoadModule::GRAPH_ID, 0
				),
				lowerDepartureTime, higherDepartureTime,
				lowerArrivalTime, higherArrivalTime,
				PTModule::GRAPH_ID,
				PTModule::GRAPH_ID,
				optional<posix_time::time_duration>(),
				maxSolutionsNumber,
				accessParameters,
				planningOrder,
				100,
				ignoreReservation,
				logStream
			),
			_departurePlace(origin),
			_arrivalPlace(destination)
		{
		}


		VertexAccessMap PTTimeSlotRoutePlanner::_extendToPhysicalStops(
			const VertexAccessMap& vam,
			const VertexAccessMap& destinationVam,
			PlanningPhase direction
		) const {

			VertexAccessMap result;

			// Create origin vam from integral search on roads
			JourneysResult resultJourneys(
				direction == DEPARTURE_TO_ARRIVAL ?
				getLowestDepartureTime() :
				getHighestArrivalTime(),
				direction
			);
			VertexAccessMap emptyMap;
			BestVertexReachesMap bvrmd(
				direction,
				vam,
				emptyMap
			); // was optim=true

			ptime highestArrivalTime(direction == DEPARTURE_TO_ARRIVAL ? getHighestArrivalTime() : getLowestDepartureTime());
			IntegralSearcher iso(
				direction,
				_accessParameters,
				PTModule::GRAPH_ID,
				false,
				RoadModule::GRAPH_ID,
				resultJourneys,
				bvrmd,
				destinationVam,
				direction == DEPARTURE_TO_ARRIVAL ? getLowestDepartureTime() : getHighestArrivalTime(),
				direction == DEPARTURE_TO_ARRIVAL ?
					getLowestDepartureTime() + (getHighestArrivalTime() - getHighestDepartureTime()) :
					getHighestArrivalTime() - (getLowestArrivalTime() - getLowestDepartureTime()),
				highestArrivalTime,
				false,
				false,
				_accessParameters.getMaxApproachTime(),
				_accessParameters.getApproachSpeed(),
				_logStream
			);
			iso.integralSearch(vam, optional<size_t>(), optional<posix_time::time_duration>());

			// Include physical stops from originVam into result of integral search
			// (cos not taken into account in returned journey vector).
			BOOST_FOREACH(const VertexAccessMap::VamMap::value_type& itps, vam.getMap())
			{
				const Vertex* vertex(itps.first);

				if(	vertex->getGraphType() == PTModule::GRAPH_ID
				){
					result.insert(vertex, itps.second);
				}

				VertexAccessMap vam2;
				vertex->getHub()->getVertexAccessMap(
					vam2,
					PTModule::GRAPH_ID,
					*vertex,
					direction == DEPARTURE_TO_ARRIVAL
				);
				BOOST_FOREACH(const VertexAccessMap::VamMap::value_type& it, vam2.getMap())
				{
					result.insert(
						it.first,
						VertexAccess(
							itps.second.approachTime +
							(	direction == DEPARTURE_TO_ARRIVAL ?
								vertex->getHub()->getTransferDelay(*vertex, *it.first) :
								vertex->getHub()->getTransferDelay(*it.first, *vertex)
							),
							itps.second.approachDistance,
							itps.second.approachJourney
					)	);
				}
			}


			Journey candidate;
			BOOST_FOREACH(const JourneysResult::ResultSet::value_type& it, resultJourneys.getJourneys())
			{
				JourneysResult::ResultSet::key_type oj(it.first);

				// Store each reached physical stop with full approach time addition :
				//	- approach time in departure place
				//	- duration of the approach journey
				//	- transfer delay between approach journey end address and physical stop
				posix_time::time_duration commonApproachTime(
					vam.getVertexAccess(
						direction == DEPARTURE_TO_ARRIVAL ?
						oj->getOrigin()->getFromVertex() :
						oj->getDestination()->getFromVertex()
					).approachTime + minutes(ceil(oj->getDuration().total_seconds() / double(60)))
				);
				double commonApproachDistance(
					vam.getVertexAccess(
						direction == DEPARTURE_TO_ARRIVAL ?
						oj->getOrigin()->getFromVertex() :
						oj->getDestination()->getFromVertex()
					).approachDistance + oj->getDistance ()
				);
				VertexAccessMap vam2;
				const Hub* cp(
					(	direction == DEPARTURE_TO_ARRIVAL ?
						oj->getDestination() :
						oj->getOrigin()
					)->getHub()
				);
				const Vertex& v(
					*(	direction == DEPARTURE_TO_ARRIVAL ?
						oj->getDestination() :
						oj->getOrigin()
					)->getFromVertex()
				);
				cp->getVertexAccessMap(
					vam2,
					PTModule::GRAPH_ID,
					v,
					direction == DEPARTURE_TO_ARRIVAL
				);
				BOOST_FOREACH(const VertexAccessMap::VamMap::value_type& it, vam2.getMap())
				{
					result.insert(
						it.first,
						VertexAccess(
							commonApproachTime + (
								(&v == it.first) ?
								posix_time::seconds(0) :
								(
									direction == DEPARTURE_TO_ARRIVAL ?
									cp->getTransferDelay(v, *it.first) :
									cp->getTransferDelay(*it.first, v)
							)	),
							commonApproachDistance,
							*oj
					)	);
				}
			}

			return result;
		}


		PTRoutePlannerResult PTTimeSlotRoutePlanner::run() const
		{
			if(	_logStream
			){
				*_logStream << "<h2>Origin access map calculation</h2>";
			}

			TimeSlotRoutePlanner::Result result;

			// Control if departure and arrival VAMs has contains at least one vertex
			if(	_originVam.getMap().empty() ||
				_destinationVam.getMap().empty()
			){
				return PTRoutePlannerResult(_departurePlace, _arrivalPlace, false, result);
			}

			// Control if the departure and arrival places are the same
			if(	_originVam.intersercts(_destinationVam)
			){
				return PTRoutePlannerResult(_departurePlace, _arrivalPlace, true, result);
			}

			// Search stops around the departure and arrival places using the road network
			VertexAccessMap ovam(_extendToPhysicalStops(_originVam, _destinationVam, DEPARTURE_TO_ARRIVAL));
			VertexAccessMap dvam(_extendToPhysicalStops(_destinationVam, _originVam, ARRIVAL_TO_DEPARTURE));

			// Log the result of road approaches calculation
			if(	_logStream
			){
				*_logStream << "<h3>Origins</h3><table class=\"adminresults\"><tr><th>Connection Place</th><th>Physical Stop</th><th>Dst.</th><th>Time</th></tr>";

				BOOST_FOREACH(VertexAccessMap::VamMap::value_type it, ovam.getMap())
				{
					*_logStream	<<
						"<tr><td>" <<
						dynamic_cast<const NamedPlace*>(it.first->getHub())->getFullName() <<
						"</td><td>" << static_cast<const StopPoint*>(it.first)->getName() <<
						"</td><td>" << it.second.approachDistance <<
						"</td><td>" << it.second.approachTime.total_seconds() / 60 <<
						"</td></tr>"
						;
				}
				*_logStream << "</table>";

				*_logStream << "<h2>Destination access map calculation</h2>";

				*_logStream << "<h3>Destinations</h3><table class=\"adminresults\"><tr><th>Connection Place</th><th>Physical Stop</th><th>Dst.</th><th>Time</th></tr>";

				BOOST_FOREACH(VertexAccessMap::VamMap::value_type it, dvam.getMap())
				{
					*_logStream	<<
						"<tr><td>" <<
						dynamic_cast<const NamedPlace*>(it.first->getHub())->getFullName() <<
						"</td><td>" <<
						static_cast<const StopPoint* const>(it.first)->getName() <<
						"</td><td>" <<
						it.second.approachDistance <<
						"</td><td>" <<
						it.second.approachTime.total_seconds() / 60 <<
						"</td></tr>"
						;
				}
				*_logStream << "</table>";
			}

			// Handle of the case of possible full road approach
			if(	ovam.intersercts(dvam)
			){
				Journey resultJourney(ovam.getBestIntersection(dvam));
				ptime departureTime(resultJourney.getFirstDepartureTime());
				if(departureTime.time_of_day().seconds())
				{
					resultJourney.shift(seconds(60 - departureTime.time_of_day().seconds()));
				}
				resultJourney.shift(
					getLowestDepartureTime() - resultJourney.getFirstDepartureTime()
				);
				resultJourney.forceContinuousServiceRange(hours(24));

				result.push_back(resultJourney);
			}

			if(result.empty())
			{
				TimeSlotRoutePlanner r(
					ovam,
					dvam,
					getLowestDepartureTime(),
					getHighestDepartureTime(),
					getLowestArrivalTime(),
					getHighestArrivalTime(),
					_whatToSearch,
					_graphToUse,
					_maxDuration,
					_maxSolutionsNumber,
					_accessParameters,
					_planningOrder,
					100,
					_ignoreReservation,
					_logStream
				);
				return PTRoutePlannerResult(
					_departurePlace,
					_arrivalPlace,
					false,
					r.run()
				);
			}
			else
			{
				TimeSlotRoutePlanner r(
					ovam,
					dvam,
					result.front(),
					_whatToSearch,
					_graphToUse,
					_maxDuration,
					_maxSolutionsNumber,
					_accessParameters,
					_planningOrder,
					100,
					_ignoreReservation,
					_logStream
				);
				return PTRoutePlannerResult(
					_departurePlace,
					_arrivalPlace,
					false,
					r.run()
				);
			}
		}
	}
}
