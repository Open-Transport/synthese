
/** VAMConverter class implementation.
	@file VAMConverter.cpp

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

#include "VAMConverter.hpp"

#include "AlgorithmModule.h"
#include "AStarShortestPathCalculator.hpp"
#include "Edge.h"
#include "Hub.h"
#include "IntegralSearcher.h"
#include "JourneysResult.h"
#include "Vertex.h"

#include <boost/foreach.hpp>

using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace graph;

	namespace algorithm
	{
		VAMConverter::VAMConverter(
			const graph::AccessParameters& accessParameters,
			const AlgorithmLogger& logger,
			const graph::GraphIdType whatToSearch,
			const graph::GraphIdType graphToUse,
			const boost::posix_time::ptime& lowestDepartureTime,
			const boost::posix_time::ptime& highestDepartureTime,
			const boost::posix_time::ptime& lowestArrivalTime,
			const boost::posix_time::ptime& highestArrivalTime,
			const geography::Place* origin,
			const geography::Place* destination
		):	_accessParameters(accessParameters),
			_logger(logger),
			_whatToSearch(whatToSearch),
			_graphToUse(graphToUse),
			_lowestDepartureTime(lowestDepartureTime),
			_highestDepartureTime(highestArrivalTime),
			_lowestArrivalTime(lowestArrivalTime),
			_highestArrivalTime(highestArrivalTime),
			_origin(origin),
			_destination(destination)
		{}



		graph::VertexAccessMap VAMConverter::run(
			const graph::VertexAccessMap& vam,
			const graph::VertexAccessMap& destinationVam,
			algorithm::PlanningPhase direction
		) const	{

			VertexAccessMap result;

			if(AlgorithmModule::GetUseAStarForPhysicalStopsExtender())
			{
				AStarShortestPathCalculator asspc(
					_origin,
					_destination,
					(direction == DEPARTURE_TO_ARRIVAL ? _lowestDepartureTime : _highestArrivalTime),
					_accessParameters,
					direction
				);

				result = asspc.roadPlanningToClosePhysicalStops(vam, destinationVam);
			}
			else
			{
				// Create origin vam from integral search on roads
				JourneysResult resultJourneys(
					direction == DEPARTURE_TO_ARRIVAL ?
					_lowestDepartureTime :
					_highestArrivalTime,
					direction
				);
				VertexAccessMap emptyMap;
				BestVertexReachesMap bvrmd(
					direction,
					vam,
					emptyMap,
					Vertex::GetMaxIndex()
				); // was optim=true

				ptime highestArrivalTime(direction == DEPARTURE_TO_ARRIVAL ? _highestArrivalTime : _lowestDepartureTime);
				IntegralSearcher iso(
					direction,
					_accessParameters,
					_whatToSearch,
					false,
					_graphToUse,
					resultJourneys,
					bvrmd,
					destinationVam,
					direction == DEPARTURE_TO_ARRIVAL ? _lowestDepartureTime : _highestArrivalTime,
					direction == DEPARTURE_TO_ARRIVAL ? _highestDepartureTime : _lowestArrivalTime,
					highestArrivalTime,
					direction == DEPARTURE_TO_ARRIVAL ? false : true,
					false,
					_accessParameters.getMaxApproachTime(),
					_accessParameters.getApproachSpeed(),
					false,
					_logger
				);
				iso.integralSearch(vam, optional<size_t>(), optional<time_duration>());

				// Include physical stops from originVam into result of integral search
				// (cos not taken into account in returned journey vector).
				BOOST_FOREACH(const VertexAccessMap::VamMap::value_type& itps, vam.getMap())
				{
					const Vertex* vertex(itps.first);

					if(	vertex->getGraphType() == _whatToSearch
					){
						result.insert(vertex, itps.second);
					}

					VertexAccessMap vam2;
					vertex->getHub()->getVertexAccessMap(
						vam2,
						_whatToSearch,
						*vertex,
						direction == DEPARTURE_TO_ARRIVAL
					);
					BOOST_FOREACH(const VertexAccessMap::VamMap::value_type& it, vam2.getMap())
					{
						double approachDistance = it.second.approachDistance + itps.second.approachDistance;

						if (approachDistance <=	_accessParameters.getMaxApproachDistance())
						{
							boost::posix_time::time_duration approachTime =
								it.second.approachTime + itps.second.approachTime +
								(	direction == DEPARTURE_TO_ARRIVAL ?
									vertex->getHub()->getTransferDelay(*vertex, *it.first) :
									vertex->getHub()->getTransferDelay(*it.first, *vertex)
								);
							Journey approachJourney(itps.second.approachJourney);
							if(DEPARTURE_TO_ARRIVAL == direction)
							{
								approachJourney.append(it.second.approachJourney);
							}
							else
							{
								approachJourney.prepend(it.second.approachJourney);
							}

							result.insert(it.first, VertexAccess(approachTime, approachDistance, approachJourney));
						}
					}
				}


				Journey candidate;
				BOOST_FOREACH(const JourneysResult::ResultSet::value_type& it, resultJourneys.getJourneys())
				{
					JourneysResult::ResultSet::key_type oj(it.first);
					Vertex* startVertex = (direction == DEPARTURE_TO_ARRIVAL) ?
						oj->getOrigin()->getFromVertex() :
						oj->getDestination()->getFromVertex();
					VertexAccess startVertexAccess = vam.getVertexAccess(startVertex);

					// Store each reached physical stop with full approach time addition :
					//	- approach time in departure place
					//	- duration of the approach journey
					//	- transfer delay between approach journey end address and physical stop
					posix_time::time_duration commonApproachTime(
						startVertexAccess.approachTime + minutes(static_cast<long>(ceil(oj->getDuration(false).total_seconds() / double(60))))
					);
					double commonApproachDistance(startVertexAccess.approachDistance + oj->getDistance());

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
						_whatToSearch,
						v,
						direction == DEPARTURE_TO_ARRIVAL
					);
					BOOST_FOREACH(const VertexAccessMap::VamMap::value_type& it, vam2.getMap())
					{
						double approachDistance = it.second.approachDistance + commonApproachDistance;

						if (approachDistance <= _accessParameters.getMaxApproachDistance())
						{
							boost::posix_time::time_duration approachTime =
								commonApproachTime +
								(
									(&v == it.first) ?
									seconds(0) :
									(
										direction == DEPARTURE_TO_ARRIVAL ?
										cp->getTransferDelay(v, *it.first) :
										cp->getTransferDelay(*it.first, v)
									)
								);
							Journey approachJourney(startVertexAccess.approachJourney);

							if(DEPARTURE_TO_ARRIVAL == direction)
							{
								approachJourney.append(*oj);
							}
							else
							{
								approachJourney.prepend(*oj);
							}

							result.insert(it.first, VertexAccess(approachTime, approachDistance, approachJourney));
						}
					}
				}
			}

			return result;

		}
}	}
