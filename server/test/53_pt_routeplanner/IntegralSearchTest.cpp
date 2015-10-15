
/** Integral search test implementation.
	@file IntegralSearchTest.cpp

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

#include "RoutePlannerTestData.inc.hpp"

#include "AlgorithmLogger.hpp"
#include "IntegralSearcher.h"
#include "VertexAccessMap.h"
#include "PTModule.h"
#include "JourneysResult.h"

#include <boost/test/auto_unit_test.hpp>

using namespace synthese::algorithm;
using namespace synthese::graph;
using namespace synthese::util;
using namespace synthese::pt;
using namespace synthese::vehicle;

BOOST_AUTO_TEST_CASE (RoutePlanner)
{
	ScopedRegistrable<CommercialLine> scopedCommercialLine;
	ScopedCoordinatesSystemUser scopedCoordinatesSystemUser;

	#include "RoutePlannerTestData.hpp"

	AccessParameters::AllowedPathClasses pc;
	AccessParameters a(
		USER_PEDESTRIAN, false, false, 1000, minutes(23), 67, 10, pc
	);
	ptime tomorrow(day_clock::local_day(), minutes(0));
	ptime next_day(tomorrow);
	next_day += days(1);

	{ // 05 -> 99
		VertexAccessMap ovam(static_cast<Place&>(place05).getVertexAccessMap(a, PTModule::GRAPH_ID, 0));
		VertexAccessMap dvam(static_cast<Place&>(place99).getVertexAccessMap(a, PTModule::GRAPH_ID, 0));

		JourneysResult	result(tomorrow, DEPARTURE_TO_ARRIVAL);
		BestVertexReachesMap bestVertexReachesMap(
			DEPARTURE_TO_ARRIVAL,
			ovam,
			dvam,
			Vertex::GetMaxIndex()
		);

		AlgorithmLogger logger;
		IntegralSearcher i(
			DEPARTURE_TO_ARRIVAL,
			a,
			PTModule::GRAPH_ID,
			false,
			PTModule::GRAPH_ID,
			result,
			bestVertexReachesMap,
			dvam,
			tomorrow,
			next_day,
			next_day,
			false,
			false,
			boost::optional<boost::posix_time::time_duration>(),
			100,
			false,
			logger
		);

		i.integralSearch(
			ovam,
			boost::optional<std::size_t>(0),
			boost::optional<boost::posix_time::time_duration>()
		);

		BOOST_REQUIRE_EQUAL(result.getJourneys().size(), 4ULL);
	}

	{ // 05 -> 95
		VertexAccessMap ovam(static_cast<Place&>(place05).getVertexAccessMap(a, PTModule::GRAPH_ID, 0));
		VertexAccessMap dvam(static_cast<Place&>(place95).getVertexAccessMap(a, PTModule::GRAPH_ID, 0));
		ptime next_day(tomorrow);
		next_day += days(1);

		JourneysResult	result(tomorrow, DEPARTURE_TO_ARRIVAL);
		BestVertexReachesMap bestVertexReachesMap(
			DEPARTURE_TO_ARRIVAL,
			ovam,
			dvam,
			Vertex::GetMaxIndex()
		);

		AlgorithmLogger logger;
		IntegralSearcher i(
			DEPARTURE_TO_ARRIVAL,
			a,
			PTModule::GRAPH_ID,
			false,
			PTModule::GRAPH_ID,
			result,
			bestVertexReachesMap,
			dvam,
			tomorrow,
			next_day,
			next_day,
			false,
			false,
			boost::optional<boost::posix_time::time_duration>(),
			100,
			false,
			logger
		);

		i.integralSearch(
			ovam,
			boost::optional<std::size_t>(0),
			boost::optional<boost::posix_time::time_duration>()
		);

		BOOST_REQUIRE_EQUAL(result.getJourneys().size(), 4ULL);
	}
}
