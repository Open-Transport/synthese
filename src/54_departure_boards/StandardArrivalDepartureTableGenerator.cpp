
/** StandardArrivalDepartureTableGenerator class implementation.
	@file StandardArrivalDepartureTableGenerator.cpp

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

#include "StandardArrivalDepartureTableGenerator.h"

#include "AccessParameters.h"
#include "LinePhysicalStop.hpp"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "GraphConstants.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace pt;
	using namespace graph;
	using namespace pt;

	namespace departure_boards
	{

		StandardArrivalDepartureTableGenerator::StandardArrivalDepartureTableGenerator(
			const PhysicalStops& physicalStops,
			const DeparturesTableDirection& direction,
			const EndFilter& endfilter,
			const LineFilter& lineFilter,
			const DisplayedPlacesList& displayedPlacesList,
			const ForbiddenPlacesList& forbiddenPlaces,
			const ptime& startTime,
			const ptime& endDateTime,
			bool allowCanceled,
			optional<size_t> maxSize
		):	ArrivalDepartureTableGenerator(
			physicalStops, direction, endfilter, lineFilter,
			displayedPlacesList, forbiddenPlaces, startTime, endDateTime, allowCanceled, maxSize
		){
		}



		const ArrivalDepartureList& StandardArrivalDepartureTableGenerator::generate()
		{
			// If no stop in the current stop area, return empty result
			if(_physicalStops.empty())
			{
				return _result;
			}

			AccessParameters ap;

			// Loop on the stops
			BOOST_FOREACH(PhysicalStops::value_type it, _physicalStops)
			{
				// Loop on journey patterns calling at the stop
				BOOST_FOREACH(const Vertex::Edges::value_type& edge, it.second->getDepartureEdges())
				{
					if(!dynamic_cast<const LinePhysicalStop*>(edge.second))
					{
						continue;
					}
					const LinePhysicalStop& ls = static_cast<const LinePhysicalStop&>(*edge.second);

					// Checks if the line stop is allowed according to the generator parameters
					if (!_allowedLineStop(ls) || !ls.getFollowingArrivalForFineSteppingOnly())
					{
						continue;
					}

					// Loop on services
					ptime departureDateTime = _startDateTime;
					optional<Edge::DepartureServiceIndex::Value> index;
					size_t insertedServices(0);
					while(true)
					{
						// Tells to the journey pattern for a next service
						ServicePointer servicePointer(
							ls.getNextService(
								ap,
								departureDateTime,
								_endDateTime,
								false,
								index,
								false,
								false,
								_allowCanceled
						)	);

						// If no next service was found, then abort the search in the current journey pattern
						if(	!servicePointer.getService())
						{
							break;
						}

						// Saves local variables
						++*index;
						departureDateTime = servicePointer.getDepartureDateTime();

						// Checks if the stop area is really served and if the served stop is allowed
						if(	_physicalStops.find(servicePointer.getRealTimeDepartureVertex()->getKey()) == _physicalStops.end()
						){
							continue;
						}

						// The departure is kept in the results
						_insert(servicePointer);

						// Checks if the maximal number of results is reached
						++insertedServices;
						if(	_maxSize && insertedServices >= *_maxSize)
						{
							break;
						}
					}
				}
			}
			return _result;
		}
}	}
