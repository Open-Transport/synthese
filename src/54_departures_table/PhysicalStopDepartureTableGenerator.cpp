
/** ForcedDestinationsArrivalDepartureTableGenerator class implementation.
	@file ForcedDestinationsArrivalDepartureTableGenerator.cpp

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

#include "PhysicalStopDepartureTableGenerator.h"
#include "PhysicalStop.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "LineStop.h"
#include "Line.h"
#include "ForcedDestinationsArrivalDepartureTableGenerator.h"
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

		PhysicalStopDepartureTableGenerator::PhysicalStopDepartureTableGenerator(
			const PhysicalStops& physicalStops
			, const DeparturesTableDirection& direction
			, const EndFilter& endfilter
			, const LineFilter& lineFilter
			, const DisplayedPlacesList& displayedPlacesList
			, const ForbiddenPlacesList& forbiddenPlaces,
			const TransferDestinationsList& transferDestinations
			, const DateTime& startTime
			, const DateTime& endDateTime
			, size_t maxSize
		):	ArrivalDepartureTableGenerator(
			physicalStops,
			direction,
			endfilter,
			lineFilter,
			displayedPlacesList,
			forbiddenPlaces,
			transferDestinations,
			startTime,
			endDateTime,
			maxSize
		){
		}




		const ArrivalDepartureList& PhysicalStopDepartureTableGenerator::generate()
		{
			typedef map<const PublicTransportStopZoneConnectionPlace*, ArrivalDepartureList::iterator> ReachedDestinationMap;
			ReachedDestinationMap reachedDestination;
			
			BOOST_FOREACH(const PhysicalStops::value_type& it, _physicalStops)
			{
				PhysicalStops fakeList;
				fakeList.insert(it);
				StandardArrivalDepartureTableGenerator standardTable(
					fakeList,
					_direction,
					_endFilter,
					_lineFilter,
					DisplayedPlacesList(),
					_forbiddenPlaces,
					_transferDestinations,
					_startDateTime,
					_endDateTime,
					1
				);
				const ArrivalDepartureList& standardTableResult = standardTable.generate();

				if(standardTableResult.empty())
				{

				}
				else
				{
				}
			}

			return _result;

		}

	}
}
