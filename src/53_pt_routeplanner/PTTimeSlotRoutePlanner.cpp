
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
#include "JourneyComparator.h"
#include "IntegralSearcher.h"
#include "PhysicalStop.h"

#include <sstream>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace pt;
	using namespace road;
	using namespace graph;
	using namespace geography;
	using namespace algorithm;
	using namespace util;
	using namespace time;
	using namespace env;

	namespace ptrouteplanner
	{
		PTTimeSlotRoutePlanner::PTTimeSlotRoutePlanner(
			const geography::Place* origin,
			const geography::Place* destination,
			const time::DateTime& lowerDepartureTime,
			const time::DateTime& higherDepartureTime,
			const time::DateTime& lowerArrivalTime,
			const time::DateTime& higherArrivalTime,
			const boost::optional<std::size_t> maxSolutionsNumber,
			const graph::AccessParameters accessParameters,
			const PlanningOrder planningOrder
		):	TimeSlotRoutePlanner(
				origin->getVertexAccessMap(DEPARTURE_TO_ARRIVAL, accessParameters, PTModule::GRAPH_ID, RoadModule::GRAPH_ID, 0),
				destination->getVertexAccessMap(ARRIVAL_TO_DEPARTURE, accessParameters, PTModule::GRAPH_ID, RoadModule::GRAPH_ID, 0),
				lowerDepartureTime, higherDepartureTime,
				lowerArrivalTime, higherArrivalTime,
				PTModule::GRAPH_ID,
				PTModule::GRAPH_ID,
				optional<posix_time::time_duration>(),
				maxSolutionsNumber,
				accessParameters,
				planningOrder
			),
			_departurePlace(origin),
			_arrivalPlace(destination)
		{
		}



		PTRoutePlannerResult PTTimeSlotRoutePlanner::run()
		{
#ifdef DEBUG			// Log
			if(	Log::GetInstance().getLevel() <= Log::LEVEL_TRACE
				|| _logLevel <= Log::LEVEL_TRACE
				){
					stringstream s;
					s << "<h2>Origin access map calculation</h2>";

					if (Log::GetInstance().getLevel() <= Log::LEVEL_TRACE)
						Log::GetInstance().trace(s.str());
					if (_logLevel <= Log::LEVEL_TRACE && _logStream)
						*_logStream << s.str();
			}
#endif
			TimeSlotRoutePlanner::Result result;

			// Control if departure and arrival VAMs has contains at least one vertex
			// or if the departure and arrival places are the same
			if(	_originVam.getMap().empty() ||
				_destinationVam.getMap().empty() ||
				_originVam.intersercts(_destinationVam)
			){
				return PTRoutePlannerResult(_departurePlace, _arrivalPlace, true, result);
			}

			// Create origin vam from integral search on roads
			JourneysResult<JourneyComparator> originJourneys(getLowestDepartureTime());
			BestVertexReachesMap bvrmd(DEPARTURE_TO_ARRIVAL, _originVam); // was optim=true
			DateTime highestArrivalTime(getHighestArrivalTime());
			IntegralSearcher iso(
				DEPARTURE_TO_ARRIVAL
				, _accessParameters
				, PTModule::GRAPH_ID
				, RoadModule::GRAPH_ID
				, originJourneys
				, bvrmd
				, _destinationVam,
				getLowestDepartureTime(),
				getHighestDepartureTime(),
				highestArrivalTime,
				false,
				boost::posix_time::minutes(_accessParameters.getMaxApproachTime())
				, _logStream
				, _logLevel
			);
			iso.integralSearch(_originVam, optional<size_t>());

			VertexAccessMap ovam;
			// Include physical stops from originVam into result of integral search
			// (cos not taken into account in returned journey vector).
			ovam.mergeWithFilter(_originVam, PTModule::GRAPH_ID);

			Journey candidate;
			BOOST_FOREACH(const JourneysResult<JourneyComparator>::ResultSet::value_type& it, originJourneys.getJourneys())
			{
				JourneysResult<JourneyComparator>::ResultSet::key_type oj(it.first);

				// Store each reached physical stop with full approach time addition :
				//	- approach time in departure place
				//	- duration of the approach journey
				//	- transfer delay between approach journey end address and physical stop
				posix_time::time_duration commonApproachTime(
					_originVam.getVertexAccess(oj->getOrigin()->getFromVertex()).approachTime
					+ oj->getDuration ()
					);
				double commonApproachDistance(
					_originVam.getVertexAccess(oj->getOrigin()->getFromVertex()).approachDistance
					+ oj->getDistance ()
				);
				VertexAccessMap vam;
				const Hub* cp(oj->getDestination()->getHub());
				const Vertex& v(*oj->getDestination()->getFromVertex());
				cp->getVertexAccessMap(
					vam
					, DEPARTURE_TO_ARRIVAL
					, PTModule::GRAPH_ID
					, v
				);
				BOOST_FOREACH(const VertexAccessMap::VamMap::value_type& it, vam.getMap())
				{
					if (_destinationVam.contains(it.first))
						continue;

					ovam.insert(
						it.first
						, VertexAccess(
							commonApproachTime + cp->getTransferDelay(v, *it.first)
							, commonApproachDistance
							, *oj
						)
					);
				}

				// Store the journey as a candidate if it goes directly to the destination
				if(	_destinationVam.contains(oj->getDestination()->getFromVertex()) &&
					(	!candidate.empty() ||
					oj->isBestThan(candidate)
				)	){
					candidate = *oj;
				}
			}

			// If a candidate was elected, store it in the result array
			if (!candidate.empty())
			{
				result.push_back(candidate);
			}

#ifdef DEBUG			// Log
			if(	Log::GetInstance().getLevel() <= Log::LEVEL_TRACE
				|| _logLevel <= Log::LEVEL_TRACE
			){
				stringstream s;
				s << "<h3>Origins</h3><table class=\"adminresults\"><tr><th>Connection Place</th><th>Physical Stop</th><th>Dst.</th><th>Time</th></tr>";

				BOOST_FOREACH(VertexAccessMap::VamMap::value_type it, ovam.getMap())
				{
					s	<<
						"<tr><td>" <<
						dynamic_cast<const NamedPlace*>(it.first->getHub())->getFullName() <<
						"</td><td>" << static_cast<const PhysicalStop*>(it.first)->getName() <<
						"</td><td>" << it.second.approachDistance <<
						"</td><td>" << it.second.approachTime.total_seconds() / 60 <<
						"</td></tr>"
						;
				}
				s << "</table>";

				if (Log::GetInstance().getLevel() <= Log::LEVEL_TRACE)
					Log::GetInstance().trace(s.str());
				if (_logLevel <= Log::LEVEL_TRACE && _logStream)
					*_logStream << s.str();
			}

			// Log
			if(	Log::GetInstance().getLevel() <= Log::LEVEL_TRACE
				|| _logLevel <= Log::LEVEL_TRACE
				){
					stringstream s;
					s << "<h2>Destination access map calculation</h2>";

					if (Log::GetInstance().getLevel() <= Log::LEVEL_TRACE)
						Log::GetInstance().trace(s.str());
					if (_logLevel <= Log::LEVEL_TRACE && _logStream)
						*_logStream << s.str();
			}
#endif
			// Create destination vam from integral search on roads
			JourneysResult<JourneyComparator> destinationJourneys(getHighestArrivalTime());
			BestVertexReachesMap bvrmo(DEPARTURE_TO_ARRIVAL, _destinationVam); // was optim=true
			DateTime lowestDepartureTime(getLowestDepartureTime());
			IntegralSearcher isd(
				ARRIVAL_TO_DEPARTURE
				, _accessParameters
				, PTModule::GRAPH_ID
				, RoadModule::GRAPH_ID
				, destinationJourneys
				, bvrmo
				, _originVam,
				getHighestArrivalTime(),
				getLowestArrivalTime(),
				lowestDepartureTime,
				true,
				posix_time::minutes(_accessParameters.getMaxApproachTime())
				, _logStream
				, _logLevel
			);
			isd.integralSearch(_destinationVam, optional<size_t>());

			VertexAccessMap dvam;
			// Include physical stops from destinationVam into result of integral search
			// (cos not taken into account in returned journey vector).
			dvam.mergeWithFilter(_destinationVam, PTModule::GRAPH_ID);

			BOOST_FOREACH(const JourneysResult<JourneyComparator>::ResultSet::value_type& it, destinationJourneys.getJourneys())
			{
				JourneysResult<JourneyComparator>::ResultSet::key_type dj(it.first);

				// Store each reached physical stop with full approach time addition :
				//	- approach time in destination place
				//	- duration of the approach journey
				//	- transfer delay between approach journey end address and physical stop
				posix_time::time_duration commonApproachTime(
					_destinationVam.getVertexAccess(
					dj->getDestination()->getFromVertex()
					).approachTime	+ dj->getDuration ()
				);
				double commonApproachDistance(
					_destinationVam.getVertexAccess(
					dj->getDestination()->getFromVertex()
					).approachDistance + dj->getDistance ()
				);
				VertexAccessMap vam;
				const Hub* cp(dj->getOrigin()->getHub());
				const Vertex& v(*dj->getOrigin()->getFromVertex());
				cp->getVertexAccessMap(
					vam,
					ARRIVAL_TO_DEPARTURE,
					PTModule::GRAPH_ID,
					v
				);
				BOOST_FOREACH(const VertexAccessMap::VamMap::value_type& it, vam.getMap())
				{
					if (!_originVam.contains(it.first))
						dvam.insert(
							it.first,
							VertexAccess(
							commonApproachTime + cp->getTransferDelay(*it.first, v),
							commonApproachDistance,
							*dj
						)
					);
				}
			}

#ifdef DEBUG			// Log
			if(	Log::GetInstance().getLevel() <= Log::LEVEL_TRACE
				|| _logLevel <= Log::LEVEL_TRACE
				){
					stringstream s;
					s << "<h3>Destinations</h3><table class=\"adminresults\"><tr><th>Connection Place</th><th>Physical Stop</th><th>Dst.</th><th>Time</th></tr>";

					BOOST_FOREACH(VertexAccessMap::VamMap::value_type it, dvam.getMap())
					{
						s	<<
							"<tr><td>" <<
							dynamic_cast<const NamedPlace*>(it.first->getHub())->getFullName() <<
							"</td><td>" <<
							static_cast<const PhysicalStop* const>(it.first)->getName() <<
							"</td><td>" <<
							it.second.approachDistance <<
							"</td><td>" <<
							it.second.approachTime.total_seconds() / 60 <<
							"</td></tr>"
							;
					}
					s << "</table>";

					if (Log::GetInstance().getLevel() <= Log::LEVEL_TRACE)
						Log::GetInstance().trace(s.str());
					if (_logLevel <= Log::LEVEL_TRACE && _logStream)
						*_logStream << s.str();
			}
#endif
// 			if (result.empty())
// 			{
// 				_previousContinuousServiceDuration = posix_time::minutes(0);
// 			}
// 			else
// 			{
// 				shared_ptr<Journey> journey(_result.front());
// 				_previousContinuousServiceDuration = journey->getDuration();
// 				_previousContinuousServiceLastDeparture = journey->getDepartureTime();
// 				_previousContinuousServiceLastDeparture += journey->getContinuousServiceRange();
// 			}
// TODO prepare continuous service

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
				_logStream,
				_logLevel
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
