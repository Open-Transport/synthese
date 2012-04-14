
/** ProjectionTest class implementation.
	@file ProjectionTest.cpp

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
#include "PTTimeSlotRoutePlanner.h"
#include "RoutePlanner.h"
#include "PTModule.h"
#include "FreeDRTArea.hpp"

#include <boost/test/auto_unit_test.hpp>

using namespace synthese::pt_journey_planner;
using namespace synthese::algorithm;
using namespace synthese::graph;
using namespace synthese::util;
using namespace synthese::pt;

BOOST_AUTO_TEST_CASE (RoutePlannerTest)
{
	ScopedCoordinatesSystemUser scopedCoordinatesSystemUser;
	ScopedRegistrable<FreeDRTArea> scopedFreeDRTAreaRegistrable;

	#include "RoutePlannerTestData.hpp"

	AlgorithmLogger logger;
	AccessParameters::AllowedPathClasses pc;
	AccessParameters a(
		USER_PEDESTRIAN, false, false, 1000, boost::posix_time::minutes(23), 1.11, 10, pc
	);
	ptime tomorrow(day_clock::local_day(), minutes(0));
	tomorrow += days(1);
	ptime next_day(tomorrow);
	next_day += days(1);

	{ // 05 -> 99
		VertexAccessMap ovam(static_cast<Place&>(place05).getVertexAccessMap(a, PTModule::GRAPH_ID, 0));
		VertexAccessMap dvam(static_cast<Place&>(place99).getVertexAccessMap(a, PTModule::GRAPH_ID, 0));

		RoutePlanner r(
			ovam,
			dvam,
			DEPARTURE_FIRST,
			a,
			boost::optional<boost::posix_time::time_duration>(),
			tomorrow,
			next_day,
			next_day,
			PTModule::GRAPH_ID,
			PTModule::GRAPH_ID,
			100,
			false,
			logger
		);

		Journey j(r.run());

		BOOST_CHECK_EQUAL(j.size(), 1);
		if(j.size() >= 1)
		{
			BOOST_CHECK_EQUAL(
				boost::posix_time::to_simple_string(j.getJourneyLeg(0).getDepartureDateTime()),
				boost::posix_time::to_simple_string(ptime(tomorrow.date(), time_duration(7,0,0)))
			);
			BOOST_CHECK_EQUAL(
				boost::posix_time::to_simple_string(j.getJourneyLeg(0).getArrivalDateTime()),
				boost::posix_time::to_simple_string(ptime(tomorrow.date(), time_duration(7,15,0)))
			);
		}
	}

	{ // 93 -> 99
		VertexAccessMap ovam(static_cast<Place&>(place93).getVertexAccessMap(a, PTModule::GRAPH_ID, 0));
		VertexAccessMap dvam(static_cast<Place&>(place99).getVertexAccessMap(a, PTModule::GRAPH_ID, 0));

		ptime departure(tomorrow.date(), time_duration(7,6,0));

		RoutePlanner r(
			ovam,
			dvam,
			DEPARTURE_FIRST,
			a,
			boost::optional<boost::posix_time::time_duration>(),
			departure,
			next_day,
			next_day,
			PTModule::GRAPH_ID,
			PTModule::GRAPH_ID,
			100,
			false,
			logger
		);

		Journey j(r.run());

		BOOST_REQUIRE_EQUAL(j.size(), 2);
		BOOST_CHECK_EQUAL(
			boost::posix_time::to_simple_string(j.getJourneyLeg(0).getDepartureDateTime()),
			boost::posix_time::to_simple_string(ptime(tomorrow.date(), time_duration(9,0,0)))
		);
		BOOST_CHECK_EQUAL(
			boost::posix_time::to_simple_string(j.getJourneyLeg(0).getArrivalDateTime()),
			boost::posix_time::to_simple_string(ptime(tomorrow.date(), time_duration(9,20,0)))
		);
		BOOST_CHECK_EQUAL(
			boost::posix_time::to_simple_string(j.getJourneyLeg(1).getDepartureDateTime()),
			boost::posix_time::to_simple_string(ptime(tomorrow.date(), time_duration(10,5,0)))
		);
		BOOST_CHECK_EQUAL(
			boost::posix_time::to_simple_string(j.getJourneyLeg(1).getArrivalDateTime()),
			boost::posix_time::to_simple_string(ptime(tomorrow.date(), time_duration(10,20,0)))
		);
	}

	{ // 93 -> 07 20:00
		VertexAccessMap ovam(static_cast<Place&>(place93).getVertexAccessMap(a, PTModule::GRAPH_ID, 0));
		VertexAccessMap dvam(static_cast<Place&>(place07).getVertexAccessMap(a, PTModule::GRAPH_ID, 0));

		ptime departure(tomorrow.date(), time_duration(20,0,0));

		RoutePlanner r(
			ovam,
			dvam,
			DEPARTURE_FIRST,
			a,
			boost::optional<boost::posix_time::time_duration>(),
			departure,
			next_day,
			next_day,
			PTModule::GRAPH_ID,
			PTModule::GRAPH_ID,
			100,
			false,
			logger
		);

		Journey j(r.run());

		BOOST_REQUIRE_EQUAL(j.size(), 1);
		BOOST_CHECK_EQUAL(
			boost::posix_time::to_simple_string(j.getJourneyLeg(0).getDepartureDateTime()),
			boost::posix_time::to_simple_string(ptime(tomorrow.date(), time_duration(20,20,0)))
		);
		BOOST_CHECK_EQUAL(
			boost::posix_time::to_simple_string(j.getJourneyLeg(0).getArrivalDateTime()),
			boost::posix_time::to_simple_string(ptime(tomorrow.date(), time_duration(21,20,0)))
		);
		BOOST_CHECK_EQUAL(j.getContinuousServiceRange().total_seconds(), 0);
	}

	{ // 93 -> 07 21:00
		VertexAccessMap ovam(static_cast<Place&>(place93).getVertexAccessMap(a, PTModule::GRAPH_ID, 0));
		VertexAccessMap dvam(static_cast<Place&>(place07).getVertexAccessMap(a, PTModule::GRAPH_ID, 0));

		ptime departure(tomorrow.date(), time_duration(21,0,0));

		RoutePlanner r(
			ovam,
			dvam,
			DEPARTURE_FIRST,
			a,
			boost::optional<boost::posix_time::time_duration>(),
			departure,
			next_day,
			next_day,
			PTModule::GRAPH_ID,
			PTModule::GRAPH_ID,
			100,
			false,
			logger
		);

		Journey j(r.run());

		BOOST_REQUIRE_EQUAL(j.size(), 2);
		BOOST_CHECK_EQUAL(
			boost::posix_time::to_simple_string(j.getJourneyLeg(0).getDepartureDateTime()),
			boost::posix_time::to_simple_string(ptime(tomorrow.date(), time_duration(21,40,0)))
		);
		BOOST_CHECK_EQUAL(
			boost::posix_time::to_simple_string(j.getJourneyLeg(0).getArrivalDateTime()),
			boost::posix_time::to_simple_string(ptime(tomorrow.date(), time_duration(21,42,0)))
		);
		BOOST_CHECK_EQUAL(
			boost::posix_time::to_simple_string(j.getJourneyLeg(1).getDepartureDateTime()),
			boost::posix_time::to_simple_string(ptime(tomorrow.date(), time_duration(21,50,0)))
		);
		BOOST_CHECK_EQUAL(
			boost::posix_time::to_simple_string(j.getJourneyLeg(1).getArrivalDateTime()),
			boost::posix_time::to_simple_string(ptime(tomorrow.date(), time_duration(22,0,0)))
		);
		BOOST_CHECK_EQUAL(j.getContinuousServiceRange().total_seconds() / 60, 30);
	}

	{ // 93 -> 07 22:00
		VertexAccessMap ovam(static_cast<Place&>(place93).getVertexAccessMap(a, PTModule::GRAPH_ID, 0));
		VertexAccessMap dvam(static_cast<Place&>(place07).getVertexAccessMap(a, PTModule::GRAPH_ID, 0));

		ptime departure(tomorrow.date(), time_duration(22,0,0));

		RoutePlanner r(
			ovam,
			dvam,
			DEPARTURE_FIRST,
			a,
			boost::optional<boost::posix_time::time_duration>(),
			departure,
			next_day,
			next_day,
			PTModule::GRAPH_ID,
			PTModule::GRAPH_ID,
			100,
			false,
			logger
		);

		Journey j(r.run());

		BOOST_REQUIRE_EQUAL(j.size(), 2);
		BOOST_CHECK_EQUAL(
			boost::posix_time::to_simple_string(j.getJourneyLeg(0).getDepartureDateTime()),
			boost::posix_time::to_simple_string(ptime(tomorrow.date(), time_duration(22,0,0)))
		);
		BOOST_CHECK_EQUAL(
			boost::posix_time::to_simple_string(j.getJourneyLeg(0).getArrivalDateTime()),
			boost::posix_time::to_simple_string(ptime(tomorrow.date(), time_duration(22,2,0)))
		);
		BOOST_CHECK_EQUAL(
			boost::posix_time::to_simple_string(j.getJourneyLeg(1).getDepartureDateTime()),
			boost::posix_time::to_simple_string(ptime(tomorrow.date(), time_duration(22,10,0)))
		);
		BOOST_CHECK_EQUAL(
			boost::posix_time::to_simple_string(j.getJourneyLeg(1).getArrivalDateTime()),
			boost::posix_time::to_simple_string(ptime(tomorrow.date(), time_duration(22,20,0)))
		);
		BOOST_CHECK_EQUAL(j.getContinuousServiceRange().total_seconds() / 60, 10);
	}

	{ // 47th street -> 99

		PTTimeSlotRoutePlanner r(
			&rp47,
			&place99,
			tomorrow,
			next_day,
			tomorrow,
			next_day,
			boost::optional<std::size_t>(),
			a,
			DEPARTURE_FIRST,
			false,
			logger
		);
		PTRoutePlannerResult result(r.run());

		BOOST_CHECK_EQUAL(result.getJourneys().size(), 5);

		if(result.getJourneys().size() >= 1)
		{
			const Journey& journey(result.getJourneys().at(0));
			BOOST_CHECK_EQUAL(journey.size(), 2);
			BOOST_CHECK_EQUAL(journey.getContinuousServiceRange(), minutes(0));
			if(journey.size() >= 1)
			{
				const ServicePointer& leg(journey.getJourneyLeg(0));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), c88.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getKey(), ro41.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(7) + minutes(10) - minutes(12)); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), c74.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getKey(), ro41.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(7) + minutes(10) - seconds(44)); 
			}
			if(journey.size() >= 2)
			{
				const ServicePointer& leg(journey.getJourneyLeg(1));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), ps75.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getKey(), li92.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(7) + minutes(10)); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), ps85.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getKey(), li92.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(7) + minutes(15)); 
			}
		}

		if(result.getJourneys().size() >= 2)
		{
			const Journey& journey(result.getJourneys().at(1));
			BOOST_CHECK_EQUAL(journey.size(), 2);
			BOOST_CHECK_EQUAL(journey.getContinuousServiceRange(), minutes(0));
			if(journey.size() >= 1)
			{
				const ServicePointer& leg(journey.getJourneyLeg(0));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), c88.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getKey(), ro41.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(10) + minutes(5) - minutes(12)); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), c74.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getKey(), ro41.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(10) + minutes(5) - seconds(44)); 
			}
			if(journey.size() >= 2)
			{
				const ServicePointer& leg(journey.getJourneyLeg(1));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), ps75.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getKey(), li95.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(10) + minutes(5)); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), ps84.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getKey(), li95.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(10) + minutes(20)); 
			}
		}

		if(result.getJourneys().size() >= 3)
		{
			const Journey& journey(result.getJourneys().at(2));
			BOOST_CHECK_EQUAL(journey.size(), 2);
			BOOST_CHECK_EQUAL(journey.getContinuousServiceRange(), minutes(0));
			if(journey.size() >= 1)
			{
				const ServicePointer& leg(journey.getJourneyLeg(0));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), c88.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getKey(), ro41.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(12) + minutes(5) - minutes(12)); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), c74.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getKey(), ro41.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(12) + minutes(5) - seconds(44)); 
			}
			if(journey.size() >= 2)
			{
				const ServicePointer& leg(journey.getJourneyLeg(1));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), ps75.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getKey(), li95.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(12) + minutes(5)); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), ps84.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getKey(), li95.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(12) + minutes(10)); 
			}
		}

		if(result.getJourneys().size() >= 4)
		{
			const Journey& journey(result.getJourneys().at(3));
			BOOST_CHECK_EQUAL(journey.size(), 2);
			BOOST_CHECK_EQUAL(journey.getContinuousServiceRange(), minutes(60));
			if(journey.size() >= 1)
			{
				const ServicePointer& leg(journey.getJourneyLeg(0));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), c88.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getKey(), ro41.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(19) + minutes(30) - minutes(12)); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), c74.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getKey(), ro41.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(19) + minutes(30) - seconds(44)); 
			}
			if(journey.size() >= 2)
			{
				const ServicePointer& leg(journey.getJourneyLeg(1));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), ps75.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getKey(), li92.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(19) + minutes(30)); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), ps85.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getKey(), li92.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(19) + minutes(54)); 
			}
		}

		if(result.getJourneys().size() >= 5)
		{
			const Journey& journey(result.getJourneys().at(4));
			BOOST_CHECK_EQUAL(journey.size(), 2);
			BOOST_CHECK_EQUAL(journey.getContinuousServiceRange(), minutes(0));
			if(journey.size() >= 1)
			{
				const ServicePointer& leg(journey.getJourneyLeg(0));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), c88.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getKey(), ro41.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(22) + minutes(10) - minutes(12)); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), c74.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getKey(), ro41.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(22) + minutes(10) - seconds(44)); 
			}
			if(journey.size() >= 2)
			{
				const ServicePointer& leg(journey.getJourneyLeg(1));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), ps75.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getKey(), li92.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(22) + minutes(10)); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), ps85.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getKey(), li92.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(22) + minutes(25)); 
			}
		}
	}

	{ // 94 -> 99

		PTTimeSlotRoutePlanner r(
			&place94,
			&place99,
			tomorrow,
			next_day,
			tomorrow,
			next_day,
			boost::optional<std::size_t>(),
			a,
			DEPARTURE_FIRST,
			false,
			logger
		);
		PTRoutePlannerResult result(r.run());

		BOOST_CHECK_EQUAL(result.getJourneys().size(), 5);
	}

	{ // 98 -> 99

		PTTimeSlotRoutePlanner r(
			&place98,
			&place99,
			tomorrow,
			next_day,
			tomorrow,
			next_day,
			boost::optional<std::size_t>(),
			a,
			DEPARTURE_FIRST,
			false,
			logger
		);
		PTRoutePlannerResult result(r.run());

		BOOST_CHECK_EQUAL(result.getJourneys().size(), 3);
	}
}
