
/** RoutePlanningTableGenerator class implementation.
	@file RoutePlanningTableGenerator.cpp

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

#include "RoutePlanningTableGenerator.h"

#include "AlgorithmLogger.hpp"
#include "PTRoutePlannerResult.h"
#include "PTTimeSlotRoutePlanner.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace pt;
	using namespace pt_journey_planner;
	using namespace graph;
	using namespace algorithm;
	using namespace pt;

	namespace departure_boards
	{
		RoutePlanningList RoutePlanningTableGenerator::run()
		{
			// Declarations
			AlgorithmLogger logger;
			RoutePlanningList result;

			// Loop on destinations
			BOOST_FOREACH(const DisplayedPlacesList::value_type& itDestination, _destinations)
			{
				PTTimeSlotRoutePlanner rp(
					&_origin,
					itDestination.second,
					_startDateTime,
					_endDateTime,
					_startDateTime,
					_endDateTime,
					1,
					AccessParameters(
						USER_PEDESTRIAN,
						false,
						false,
						0,
						posix_time::minutes(0),
						67,
						_withTransfer ? 1 : 0
					),
					DEPARTURE_FIRST,
					false,
					logger
				);

				PTRoutePlannerResult solution(rp.run());
				result.insert(
					make_pair(
						itDestination.second,
						solution.getJourneys().empty() ? Journey() : solution.getJourneys().front()
				)	);
			}

			return result;
		}



		RoutePlanningTableGenerator::RoutePlanningTableGenerator(
			const StopArea& origin,
			const DisplayedPlacesList& destinations,
			const ptime& startDateTime,
			const ptime& endDateTime,
			bool withTransfer
		):	_origin(origin),
			_destinations(destinations),
			_startDateTime(startDateTime),
			_endDateTime(endDateTime),
			_withTransfer(withTransfer)
		{
		}
}	}
