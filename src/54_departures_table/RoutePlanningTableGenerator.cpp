
/** RoutePlanningTableGenerator class implementation.
	@file RoutePlanningTableGenerator.cpp

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

#include "RoutePlanningTableGenerator.h"
#include "RoutePlanner.h"
#include <boost/foreach.hpp>

using namespace std;

namespace synthese
{
	using namespace env;
	using namespace routeplanner;
	using namespace graph;

	namespace departurestable
	{


		RoutePlanningList RoutePlanningTableGenerator::run()
		{
			RoutePlanningList result;

			BOOST_FOREACH(const DisplayedPlacesList::value_type& itDestination, _destinations)
			{
				RoutePlanner rp(
					&_origin,
					itDestination.second,
					AccessParameters(
						USER_PEDESTRIAN,
						false,
						false,
						0,
						0,
						67,
						_withTransfer ? 2 : 1
					),
					ARRIVAL_FIRST,
					_startDateTime,
					_endDateTime,
					1
				);
				
				const RoutePlanner::Result& solution(rp.computeJourneySheetDepartureArrival());
				result.insert(
					make_pair(
						itDestination.second,
						solution.empty() ? Journey() : *solution.front()
				)	);
			}

			return result;
		}

		RoutePlanningTableGenerator::RoutePlanningTableGenerator(
			const PublicTransportStopZoneConnectionPlace& origin,
			const DisplayedPlacesList& destinations,
			const time::DateTime& startDateTime,
			const time::DateTime& endDateTime,
			bool withTransfer
		):	_origin(origin),
			_destinations(destinations),
			_startDateTime(startDateTime),
			_endDateTime(endDateTime),
			_withTransfer(withTransfer)
		{
		}
	}
}
