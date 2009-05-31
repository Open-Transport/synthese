
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
#include "PhysicalStop.h"
#include "StandardArrivalDepartureTableGenerator.h"
#include "GraphConstants.h"

#include <boost/foreach.hpp>

using namespace std;

namespace synthese
{
	using namespace env;
	using namespace time;
	using namespace graph;

	namespace departurestable
	{

		StandardArrivalDepartureTableGenerator::StandardArrivalDepartureTableGenerator(
			const PhysicalStops& physicalStops
			, const DeparturesTableDirection& direction
			, const EndFilter& endfilter
			, const LineFilter& lineFilter
			, const DisplayedPlacesList& displayedPlacesList
			, const ForbiddenPlacesList& forbiddenPlaces
			, const DateTime& startTime
			, const DateTime& endDateTime
			, size_t maxSize
		):	ArrivalDepartureTableGenerator(
			physicalStops, direction, endfilter, lineFilter,
			displayedPlacesList, forbiddenPlaces, startTime, endDateTime, maxSize
		){
		}



		const ArrivalDepartureList& StandardArrivalDepartureTableGenerator::generate()
		{
			BOOST_FOREACH(PhysicalStops::value_type it, _physicalStops)
			{
				BOOST_FOREACH(const Edge* edge, it.second->getDepartureEdges())
				{
					const LineStop* ls = static_cast<const LineStop*>(edge);

					if (!_allowedLineStop(ls))
						continue;

					// Loop on services
					DateTime departureDateTime = _startDateTime;
					int index(UNKNOWN_VALUE);
					while(true)
					{
						ServicePointer servicePointer(
							ls->getNextService(
								USER_PEDESTRIAN,
								departureDateTime
								, _endDateTime
								, _calculationDateTime
								, false
								, index
						)	);
						if (!servicePointer.getService())
							break;
						_insert(servicePointer);
						index = servicePointer.getServiceIndex() + 1;
						departureDateTime = servicePointer.getActualDateTime();
					}		
				}
			}
			return _result;
		}
	}
}
