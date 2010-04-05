
/** StandardArrivalDepartureTableGenerator class implementation.
	@file StandardArrivalDepartureTableGenerator.cpp

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

#include "LineStop.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "PhysicalStop.h"
#include "StandardArrivalDepartureTableGenerator.h"
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

	namespace departurestable
	{

		StandardArrivalDepartureTableGenerator::StandardArrivalDepartureTableGenerator(
			const PhysicalStops& physicalStops
			, const DeparturesTableDirection& direction
			, const EndFilter& endfilter
			, const LineFilter& lineFilter
			, const DisplayedPlacesList& displayedPlacesList
			, const ForbiddenPlacesList& forbiddenPlaces,
			const TransferDestinationsList& transfers
			, const ptime& startTime
			, const ptime& endDateTime
			, size_t maxSize
		):	ArrivalDepartureTableGenerator(
			physicalStops, direction, endfilter, lineFilter,
			displayedPlacesList, forbiddenPlaces, transfers, startTime, endDateTime, maxSize
		){
		}



		const ArrivalDepartureList& StandardArrivalDepartureTableGenerator::generate()
		{
			if(_physicalStops.empty()) return _result;

			const PublicTransportStopZoneConnectionPlace::PhysicalStops& physicalStops(_physicalStops.begin()->second->getConnectionPlace()->getPhysicalStops());

			BOOST_FOREACH(PhysicalStops::value_type it, physicalStops)
			{
				BOOST_FOREACH(const Vertex::Edges::value_type& edge, it.second->getDepartureEdges())
				{
					const LineStop* ls = static_cast<const LineStop*>(edge.second);

					if (!_allowedLineStop(ls))
						continue;

					// Loop on services
					ptime departureDateTime = _startDateTime;
					optional<Edge::DepartureServiceIndex::Value> index;
					size_t insertedServices(0);
					while(true)
					{
						ServicePointer servicePointer(
							ls->getNextService(
								USER_PEDESTRIAN,
								departureDateTime
								, _endDateTime
								, false
								, index
						)	);
						if (!servicePointer.getService())
							break;
						++*index;
						departureDateTime = servicePointer.getActualDateTime();
						if(_physicalStops.find(servicePointer.getRealTimeVertex()->getKey()) == _physicalStops.end())
							continue;
						_insert(servicePointer);
						++insertedServices;
						if(insertedServices >= _maxSize) break;
					}		
				}
			}
			return _result;
		}
	}
}
