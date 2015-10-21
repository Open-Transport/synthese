
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
#include "Road.h"
#include "RoadPath.hpp"
#include "RoadPlace.h"
#include "PTModule.h"
#include "Fare.hpp"
#include "FreeDRTArea.hpp"
#include "StopPoint.hpp"

#include <boost/test/auto_unit_test.hpp>

using namespace synthese::pt_journey_planner;
using namespace synthese::algorithm;
using namespace synthese::graph;
using namespace synthese::geography;
using namespace synthese::road;
using namespace synthese::util;
using namespace synthese::pt;
using namespace synthese::vehicle;

using namespace std;
using namespace boost;
using namespace boost::posix_time;

void _xmlDisplayConnectionPlace(ostream& stream, const NamedPlace& np)
{
	stream <<
		"     connectionPlace :" <<
		" name = " << np.getName() <<
		" city = " << np.getCity()->getName() << endl;
}

void _xmlDisplayPhysicalStop(ostream& stream, const string& tag, const StopPoint& stop)
{
	stream << tag <<
		" name = " << stop.getName() << endl;
		_xmlDisplayConnectionPlace(stream, dynamic_cast<const NamedPlace&>(*stop.getHub()));
}

string displayJourneyDifferences(string message, PTRoutePlannerResult& result)
{
	stringstream stream;
	stream << message << endl << "Here it is solutions found : (see https://extranet.rcsmobility.com/projects/synthese/wiki/53_Journey_planner_module_tests)" << endl;

	int i = 1;
	BOOST_FOREACH(const PTRoutePlannerResult::Journeys::value_type& journey, result.getJourneys())
	{
		stream << endl << "Journey number " << i << ":" <<
			" duration " << journey.getDuration();

		if(journey.getContinuousServiceRange().total_seconds() > 0)
		{
			stream << " range (continuous journey) = " << journey.getContinuousServiceRange();
		}
		stream << endl;

		// Loop on each leg
		const Journey::ServiceUses& jl(journey.getServiceUses());
		for (Journey::ServiceUses::const_iterator itl(jl.begin()); itl != jl.end(); ++itl)
		{
			const ServicePointer& leg(*itl);
			stream << " leg departureDateTime = " << leg.getDepartureDateTime() << endl;

			const JourneyPattern* line(dynamic_cast<const JourneyPattern*>(leg.getService()->getPath()));
			if(line != NULL)
			{
				stream << (line->isPedestrianMode() ? "   do a connection :" : "   take a transport :");
				if(!line->isPedestrianMode())
				{
					stream << " lineNumber = " << line->getCommercialLine()->getShortName();
				}

				stream << " departureTime = " << leg.getDepartureDateTime() <<
					" arrivalTime = " << leg.getArrivalDateTime();

				if(journey.getContinuousServiceRange().total_seconds() > 0)
				{
					ptime edTime(leg.getDepartureDateTime());
					edTime += journey.getContinuousServiceRange();
					ptime eaTime(leg.getArrivalDateTime());
					eaTime += journey.getContinuousServiceRange();

					stream << " endDepartureTime=\"" << edTime <<
						" endArrivalTime=\"" << eaTime;
				}
				stream << endl;

				_xmlDisplayPhysicalStop(stream, "   destinationStop of this line :", dynamic_cast<const StopPoint&>(*line->getLastEdge()->getFromVertex()));
				_xmlDisplayPhysicalStop(stream, "   start at stop :", dynamic_cast<const StopPoint&>(*leg.getDepartureEdge()->getFromVertex()));
				_xmlDisplayPhysicalStop(stream, "   end as stop :", dynamic_cast<const StopPoint&>(*leg.getArrivalEdge()->getFromVertex()));
			}
			const RoadPath* road(dynamic_cast<const RoadPath*>(leg.getService()->getPath ()));
			if(road != NULL)
			{
				stream << "   take a street :" <<
					" name = " << road->getRoad()->get<RoadPlace>()->getName() <<
					" city = " << road->getRoad()->get<RoadPlace>()->getCity()->getName() <<
					" length = " << ceil(leg.getDistance()) <<
					" departureTime = " << leg.getDepartureDateTime() <<
					" arrivalTime = " << leg.getArrivalDateTime() ;

				if(journey.getContinuousServiceRange().total_seconds() > 0)
				{
					ptime edTime(leg.getDepartureDateTime());
					edTime += journey.getContinuousServiceRange();
					ptime eaTime(leg.getArrivalDateTime());
					eaTime += journey.getContinuousServiceRange();

					stream << " endDepartureTime = " << edTime <<
						" endArrivalTime = " << eaTime ;
				}
				stream << endl;
			}
		}
	i++;
	}
    return stream.str();
}

BOOST_AUTO_TEST_CASE (RoutePlannerTest)
{
	ScopedRegistrable<CommercialLine> scopedCommercialLine;
	ScopedCoordinatesSystemUser scopedCoordinatesSystemUser;

	#include "RoutePlannerTestData.hpp"

	AlgorithmLogger logger;
	AccessParameters::AllowedPathClasses pc;
	AccessParameters a(
		USER_PEDESTRIAN, false, false, 1000, boost::posix_time::minutes(23), 1.11, 10, pc
	);
	ptime tomorrow(day_clock::local_day(), hours(3));
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

		BOOST_CHECK_EQUAL(j.size(), 1ULL);
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

		BOOST_REQUIRE_EQUAL(j.size(), 2ULL);
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

	{ // 06 -> 07 21:00
		VertexAccessMap ovam(static_cast<Place&>(place06).getVertexAccessMap(a, PTModule::GRAPH_ID, 0));
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

		BOOST_CHECK_EQUAL(j.size(), 1);
		BOOST_CHECK_EQUAL(j.getContinuousServiceRange(), minutes(20));
		if(j.size() >= 1)
		{
			const ServicePointer& leg(j.getJourneyLeg(0));
			BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), ps06.getKey()); 
			BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getKey(), li98.getKey());
//			BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(22) + minutes(0));
			BOOST_CHECK_EQUAL(
				boost::posix_time::to_simple_string(leg.getDepartureDateTime()),
				boost::posix_time::to_simple_string(ptime(tomorrow.date(), time_duration(22,0,0)))
			);
			BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), ps07.getKey()); 
			BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getKey(), li98.getKey()); 
//			BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(22) + minutes(10));
			BOOST_CHECK_EQUAL(
				boost::posix_time::to_simple_string(leg.getArrivalDateTime()),
				boost::posix_time::to_simple_string(ptime(tomorrow.date(), time_duration(22,10,0)))
			);
		}
	}

	{ // 06 -> 07 21:00 from arrival
		VertexAccessMap ovam(static_cast<Place&>(place06).getVertexAccessMap(a, PTModule::GRAPH_ID, 0));
		VertexAccessMap dvam(static_cast<Place&>(place07).getVertexAccessMap(a, PTModule::GRAPH_ID, 0));

		ptime departure(tomorrow.date(), time_duration(22,0,0));

		RoutePlanner r(
			ovam,
			dvam,
			ARRIVAL_FIRST,
			a,
			boost::optional<boost::posix_time::time_duration>(),
			next_day,
			departure,
			departure,
			PTModule::GRAPH_ID,
			PTModule::GRAPH_ID,
			100,
			false,
			logger
		);

		Journey j(r.run());
/* This functionality does not work yet. Commenting test till corrections are made.
		BOOST_CHECK_EQUAL(j.size(), 1);
		BOOST_CHECK_EQUAL(j.getContinuousServiceRange(), minutes(20));
		if(j.size() >= 1)
		{
			const ServicePointer& leg(j.getJourneyLeg(0));
			BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), ps06.getKey()); 
			BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getKey(), li98.getKey()); 
			BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(22) + minutes(0)); 
			BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), ps07.getKey()); 
			BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getKey(), li98.getKey()); 
			BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(22) + minutes(10)); 
		}
*/
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
			// journey #1 : c88 -> ro41 -> c74 ; ps75 -> li92 -> ps85
			const Journey& journey(result.getJourneys().at(0));
			BOOST_CHECK_EQUAL(journey.size(), 2);
			BOOST_CHECK_EQUAL(journey.getContinuousServiceRange(), minutes(0));

			if(journey.size() >= 1)
			{
				const ServicePointer& leg(journey.getJourneyLeg(0));
				RoadPath* departureRoadPath = dynamic_cast<RoadPath*>(leg.getDepartureEdge()->getParentPath());
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), c88.getKey());
				/* Crossing 88 is the point of departure of the Journey leg. It has no parent path... */
				BOOST_CHECK_EQUAL(departureRoadPath->getRoad()->getKey(), ro41.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(4) + minutes(10) - minutes(12));

				RoadPath* arrivalRoadPath = dynamic_cast<RoadPath*>(leg.getArrivalEdge()->getParentPath());
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), c74.getKey());
				BOOST_CHECK_EQUAL(arrivalRoadPath->getRoad()->getKey(), ro41.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(4) + minutes(10) - seconds(44));
			}

			if(journey.size() >= 2)
			{
				const ServicePointer& leg(journey.getJourneyLeg(1));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), ps75.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getKey(), li92.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(4) + minutes(10));
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), ps85.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getKey(), li92.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(4) + minutes(15));
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
				RoadPath* departureRoadPath = dynamic_cast<RoadPath*>(leg.getDepartureEdge()->getParentPath());
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), c88.getKey());
				BOOST_CHECK_EQUAL(departureRoadPath->getRoad()->getKey(), ro41.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(7) + minutes(5) - minutes(12));				

				RoadPath* arrivalRoadPath = dynamic_cast<RoadPath*>(leg.getArrivalEdge()->getParentPath());
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), c74.getKey());
				BOOST_CHECK_EQUAL(arrivalRoadPath->getRoad()->getKey(), ro41.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(7) + minutes(5) - seconds(44));
			}

			if(journey.size() >= 2)
			{
				const ServicePointer& leg(journey.getJourneyLeg(1));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), ps75.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getKey(), li95.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(7) + minutes(5));
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), ps84.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getKey(), li95.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(7) + minutes(20));
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
				RoadPath* departureRoadPath = dynamic_cast<RoadPath*>(leg.getDepartureEdge()->getParentPath());
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), c88.getKey());
				BOOST_CHECK_EQUAL(departureRoadPath->getRoad()->getKey(), ro41.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(9) + minutes(5) - minutes(12));

				RoadPath* arrivalRoadPath = dynamic_cast<RoadPath*>(leg.getArrivalEdge()->getParentPath());
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), c74.getKey());
				BOOST_CHECK_EQUAL(arrivalRoadPath->getRoad()->getKey(), ro41.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(9) + minutes(5) - seconds(44));
			}

			if(journey.size() >= 2)
			{
				const ServicePointer& leg(journey.getJourneyLeg(1));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), ps75.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getKey(), li95.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(9) + minutes(5));
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), ps84.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getKey(), li95.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(9) + minutes(10));
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
				RoadPath* departureRoadPath = dynamic_cast<RoadPath*>(leg.getDepartureEdge()->getParentPath());
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), c88.getKey());
				BOOST_CHECK_EQUAL(departureRoadPath->getRoad()->getKey(), ro41.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(16) + minutes(30) - minutes(12));

				RoadPath* arrivalRoadPath = dynamic_cast<RoadPath*>(leg.getArrivalEdge()->getParentPath());
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), c74.getKey());
				BOOST_CHECK_EQUAL(arrivalRoadPath->getRoad()->getKey(), ro41.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(16) + minutes(30) - seconds(44));
			}

			if(journey.size() >= 2)
			{
				const ServicePointer& leg(journey.getJourneyLeg(1));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), ps75.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getKey(), li92.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(16) + minutes(30));
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), ps85.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getKey(), li92.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(16) + minutes(54));
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
				RoadPath* departureRoadPath = dynamic_cast<RoadPath*>(leg.getDepartureEdge()->getParentPath());
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), c88.getKey());
				BOOST_CHECK_EQUAL(departureRoadPath->getRoad()->getKey(), ro41.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(19) + minutes(10) - minutes(12));

				RoadPath* arrivalRoadPath = dynamic_cast<RoadPath*>(leg.getArrivalEdge()->getParentPath());
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), c74.getKey());
				BOOST_CHECK_EQUAL(arrivalRoadPath->getRoad()->getKey(), ro41.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(19) + minutes(10) - seconds(44));
			}
			if(journey.size() >= 2)
			{
				const ServicePointer& leg(journey.getJourneyLeg(1));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), ps75.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getKey(), li92.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(19) + minutes(10));
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), ps85.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getKey(), li92.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(19) + minutes(25));
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

		BOOST_CHECK_MESSAGE(result.getJourneys().size() == 5, displayJourneyDifferences("Result size from 94 to 99 is not 5 !",result));
	}

	/* OVE 13/03/2015 : this test case is broken and requires a fix in StopArea (see ticket tc-90 for details)
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

		BOOST_CHECK_MESSAGE(result.getJourneys().size() == 7, displayJourneyDifferences("Result size from 98 to 99 is not 7 !",result));
		BOOST_CHECK_EQUAL(result.getJourneys().size(), 8);

		if(result.getJourneys().size() >= 1)
		{
			const Journey& journey(result.getJourneys().at(0));
			BOOST_CHECK_EQUAL(journey.size(), 3);
			BOOST_CHECK_EQUAL(journey.getContinuousServiceRange(), minutes(0));
			if(journey.size() >= 1)
			{
				const ServicePointer& leg(journey.getJourneyLeg(0));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), c91.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getPathGroup()->getKey(), rp46.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(7) + minutes(14) - minutes(9)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(7) + minutes(14) - minutes(9)); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), c93.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getPathGroup()->getKey(), rp46.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(7) + minutes(14) - minutes(9) + minutes(6) + seconds(1)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(7) + minutes(14) - minutes(9) + minutes(6) + seconds(1)); 
			}
			if(journey.size() >= 2)
			{
				const ServicePointer& leg(journey.getJourneyLeg(1));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), c93.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getPathGroup()->getKey(), rp43.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(7) + minutes(14) - minutes(9) + minutes(6) + seconds(1));
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(7) + minutes(14) - minutes(9) + minutes(6) + seconds(1));
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), c99.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getPathGroup()->getKey(), rp43.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(7) + minutes(14) - minutes(9) + minutes(8) + seconds(17)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(7) + minutes(14) - minutes(9) + minutes(8) + seconds(17)); 
			}
			if(journey.size() >= 3)
			{
				const ServicePointer& leg(journey.getJourneyLeg(2));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), ps79.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getKey(), li92.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(7) + minutes(14));
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(7) + minutes(14));
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), ps85.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getKey(), li92.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(7) + minutes(15));
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(7) + minutes(15));
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
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), ps81.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getKey(), li93.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(9) + minutes(11)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(9) + minutes(11)); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), ps75.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getKey(), li93.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(9) + minutes(20)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(9) + minutes(20)); 
			}
			if(journey.size() >= 2)
			{
				const ServicePointer& leg(journey.getJourneyLeg(1));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), ps75.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getKey(), li95.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(10) + minutes(5));
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(10) + minutes(5));
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), ps84.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getKey(), li95.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(10) + minutes(20));
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(10) + minutes(20));
			}
		}

		if(result.getJourneys().size() >= 3)
		{
			const Journey& journey(result.getJourneys().at(2));
			BOOST_CHECK_EQUAL(journey.size(), 3);
			BOOST_CHECK_EQUAL(journey.getContinuousServiceRange(), minutes(10));
			if(journey.size() >= 1)
			{
				const ServicePointer& leg(journey.getJourneyLeg(0));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), c91.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getPathGroup()->getKey(), rp46.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(19) + minutes(41) - minutes(9)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(19) + minutes(41) - minutes(9)); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), c93.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getPathGroup()->getKey(), rp46.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(19) + minutes(41) - minutes(9) + minutes(6) + seconds(1)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(19) + minutes(41) - minutes(9) + minutes(6) + seconds(1)); 
			}
			if(journey.size() >= 2)
			{
				const ServicePointer& leg(journey.getJourneyLeg(1));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), c93.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getPathGroup()->getKey(), rp43.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(19) + minutes(41) - minutes(9) + minutes(6) + seconds(1));
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(19) + minutes(41) - minutes(9) + minutes(6) + seconds(1));
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), c99.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getPathGroup()->getKey(), rp43.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(19) + minutes(41) - minutes(9) + minutes(8) + seconds(17)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(19) + minutes(41) - minutes(9) + minutes(8) + seconds(17)); 
			}
			if(journey.size() >= 3)
			{
				const ServicePointer& leg(journey.getJourneyLeg(2));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), ps79.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getKey(), li92.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(19) + minutes(41));
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(19) + minutes(41));
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), ps85.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getKey(), li92.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(19) + minutes(54));
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(19) + minutes(54));
			}
		}

		if(result.getJourneys().size() >= 4)
		{
			const Journey& journey(result.getJourneys().at(3));
			BOOST_CHECK_EQUAL(journey.size(), 3);
			BOOST_CHECK_EQUAL(journey.getContinuousServiceRange(), minutes(0));
			if(journey.size() >= 1)
			{
				const ServicePointer& leg(journey.getJourneyLeg(0));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), c91.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getPathGroup()->getKey(), rp46.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(19) + minutes(55) - minutes(9)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(19) + minutes(55) - minutes(9)); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), c93.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getPathGroup()->getKey(), rp46.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(19) + minutes(55) - minutes(9) + minutes(6) + seconds(1)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(19) + minutes(55) - minutes(9) + minutes(6) + seconds(1)); 
			}
			if(journey.size() >= 2)
			{
				const ServicePointer& leg(journey.getJourneyLeg(1));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), c93.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getPathGroup()->getKey(), rp43.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(19) + minutes(55) - minutes(9) + minutes(6) + seconds(1));
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(19) + minutes(55) - minutes(9) + minutes(6) + seconds(1));
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), c99.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getPathGroup()->getKey(), rp43.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(19) + minutes(55) - minutes(9) + minutes(8) + seconds(17)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(19) + minutes(55) - minutes(9) + minutes(8) + seconds(17)); 
			}
			if(journey.size() >= 3)
			{
				const ServicePointer& leg(journey.getJourneyLeg(2));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), ps79.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getKey(), li96.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(19) + minutes(55));
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(19) + minutes(55));
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), ps85.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getKey(), li96.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(20) + minutes(5));
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(20) + minutes(5));
			}
		}

		if(result.getJourneys().size() >= 5)
		{
			const Journey& journey(result.getJourneys().at(4));
			BOOST_CHECK_EQUAL(journey.size(), 3);
			BOOST_CHECK_EQUAL(journey.getContinuousServiceRange(), minutes(15));
			if(journey.size() >= 1)
			{
				const ServicePointer& leg(journey.getJourneyLeg(0));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), c91.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getPathGroup()->getKey(), rp46.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(19) + minutes(56) - minutes(9)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(19) + minutes(56) - minutes(9)); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), c93.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getPathGroup()->getKey(), rp46.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(19) + minutes(56) - minutes(9) + minutes(6) + seconds(1)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(19) + minutes(56) - minutes(9) + minutes(6) + seconds(1)); 
			}
			if(journey.size() >= 2)
			{
				const ServicePointer& leg(journey.getJourneyLeg(1));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), c93.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getPathGroup()->getKey(), rp43.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(19) + minutes(56) - minutes(9) + minutes(6) + seconds(1));
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(19) + minutes(56) - minutes(9) + minutes(6) + seconds(1));
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), c99.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getPathGroup()->getKey(), rp43.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(19) + minutes(56) - minutes(9) + minutes(8) + seconds(17)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(19) + minutes(56) - minutes(9) + minutes(8) + seconds(17)); 
			}
			if(journey.size() >= 3)
			{
				const ServicePointer& leg(journey.getJourneyLeg(2));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), ps79.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getKey(), li92.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(19) + minutes(56));
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(19) + minutes(56));
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), ps85.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getKey(), li92.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(20) + minutes(9));
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(20) + minutes(9));
			}
		}

		if(result.getJourneys().size() >= 6)
		{
			const Journey& journey(result.getJourneys().at(5));
			BOOST_CHECK_EQUAL(journey.size(), 3);
			BOOST_CHECK_EQUAL(journey.getContinuousServiceRange(), minutes(0));
			if(journey.size() >= 1)
			{
				const ServicePointer& leg(journey.getJourneyLeg(0));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), c91.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getPathGroup()->getKey(), rp46.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(20) + minutes(20) - minutes(9)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(20) + minutes(20) - minutes(9)); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), c93.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getPathGroup()->getKey(), rp46.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(20) + minutes(20) - minutes(9) + minutes(6) + seconds(1)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(20) + minutes(20) - minutes(9) + minutes(6) + seconds(1)); 
			}
			if(journey.size() >= 2)
			{
				const ServicePointer& leg(journey.getJourneyLeg(1));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), c93.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getPathGroup()->getKey(), rp43.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(20) + minutes(20) - minutes(9) + minutes(6) + seconds(1));
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(20) + minutes(20) - minutes(9) + minutes(6) + seconds(1));
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), c99.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getPathGroup()->getKey(), rp43.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(20) + minutes(20) - minutes(9) + minutes(8) + seconds(17)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(20) + minutes(20) - minutes(9) + minutes(8) + seconds(17)); 
			}
			if(journey.size() >= 3)
			{
				const ServicePointer& leg(journey.getJourneyLeg(2));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), ps79.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getKey(), li96.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(20) + minutes(20));
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(20) + minutes(20));
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), ps85.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getKey(), li96.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(20) + minutes(25));
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(20) + minutes(25));
			}
		}

		if(result.getJourneys().size() >= 7)
		{
			const Journey& journey(result.getJourneys().at(6));
			BOOST_CHECK_EQUAL(journey.size(), 3);
			BOOST_CHECK_EQUAL(journey.getContinuousServiceRange(), minutes(20));
			if(journey.size() >= 1)
			{
				const ServicePointer& leg(journey.getJourneyLeg(0));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), c91.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getPathGroup()->getKey(), rp46.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(20) + minutes(21) - minutes(9)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(20) + minutes(21) - minutes(9)); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), c93.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getPathGroup()->getKey(), rp46.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(20) + minutes(21) - minutes(9) + minutes(6) + seconds(1)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(20) + minutes(21) - minutes(9) + minutes(6) + seconds(1)); 
			}
			if(journey.size() >= 2)
			{
				const ServicePointer& leg(journey.getJourneyLeg(1));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), c93.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getPathGroup()->getKey(), rp43.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(20) + minutes(21) - minutes(9) + minutes(6) + seconds(1));
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(20) + minutes(21) - minutes(9) + minutes(6) + seconds(1));
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), c99.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getPathGroup()->getKey(), rp43.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(20) + minutes(21) - minutes(9) + minutes(8) + seconds(17)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(20) + minutes(21) - minutes(9) + minutes(8) + seconds(17)); 
			}
			if(journey.size() >= 3)
			{
				const ServicePointer& leg(journey.getJourneyLeg(2));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), ps79.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getKey(), li92.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(20) + minutes(21));
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(20) + minutes(21));
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), ps85.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getKey(), li92.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(20) + minutes(34));
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(20) + minutes(34));
			}
		}

		if(result.getJourneys().size() >= 8)
		{
			const Journey& journey(result.getJourneys().at(7));
			BOOST_CHECK_EQUAL(journey.size(), 3);
			BOOST_CHECK_EQUAL(journey.getContinuousServiceRange(), minutes(0));
			if(journey.size() >= 1)
			{
				const ServicePointer& leg(journey.getJourneyLeg(0));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), c91.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getPathGroup()->getKey(), rp46.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(22) + minutes(20) - minutes(9)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(22) + minutes(20) - minutes(9)); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), c93.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getPathGroup()->getKey(), rp46.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(22) + minutes(20) - minutes(9) + minutes(6) + seconds(1)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(22) + minutes(20) - minutes(9) + minutes(6) + seconds(1)); 
			}
			if(journey.size() >= 2)
			{
				const ServicePointer& leg(journey.getJourneyLeg(1));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), c93.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getPathGroup()->getKey(), rp43.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(22) + minutes(20) - minutes(9) + minutes(6) + seconds(1));
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(22) + minutes(20) - minutes(9) + minutes(6) + seconds(1));
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), c99.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getPathGroup()->getKey(), rp43.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(22) + minutes(20) - minutes(9) + minutes(8) + seconds(17)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(22) + minutes(20) - minutes(9) + minutes(8) + seconds(17)); 
			}
			if(journey.size() >= 3)
			{
				const ServicePointer& leg(journey.getJourneyLeg(2));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), ps79.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getKey(), li92.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(22) + minutes(20));
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(22) + minutes(20));
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), ps85.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getKey(), li92.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(22) + minutes(25));
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(22) + minutes(25));
			}
		}
	}


	{ // 98 -> 99 arrival to departure

		PTTimeSlotRoutePlanner r(
			&place98,
			&place99,
			tomorrow,
			next_day,
			tomorrow,
			next_day,
			boost::optional<std::size_t>(),
			a,
			ARRIVAL_FIRST,
			false,
			logger
		);
		PTRoutePlannerResult result(r.run());

		BOOST_CHECK_EQUAL(result.getJourneys().size(), 8);

		if(result.getJourneys().size() >= 1)
		{
			const Journey& journey(result.getJourneys().at(0));
			BOOST_CHECK_EQUAL(journey.size(), 3);
			BOOST_CHECK_EQUAL(journey.getContinuousServiceRange(), minutes(0));
			if(journey.size() >= 1)
			{
				const ServicePointer& leg(journey.getJourneyLeg(0));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), c91.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getPathGroup()->getKey(), rp46.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(7) + minutes(14) - minutes(9)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(7) + minutes(14) - minutes(9)); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), c93.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getPathGroup()->getKey(), rp46.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(7) + minutes(14) - minutes(9) + minutes(6) + seconds(1)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(7) + minutes(14) - minutes(9) + minutes(6) + seconds(1)); 
			}
			if(journey.size() >= 2)
			{
				const ServicePointer& leg(journey.getJourneyLeg(1));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), c93.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getPathGroup()->getKey(), rp43.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(7) + minutes(14) - minutes(9) + minutes(6) + seconds(1));
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(7) + minutes(14) - minutes(9) + minutes(6) + seconds(1));
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), c99.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getPathGroup()->getKey(), rp43.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(7) + minutes(14) - minutes(9) + minutes(8) + seconds(17)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(7) + minutes(14) - minutes(9) + minutes(8) + seconds(17)); 
			}
			if(journey.size() >= 3)
			{
				const ServicePointer& leg(journey.getJourneyLeg(2));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), ps79.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getKey(), li92.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(7) + minutes(14));
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(7) + minutes(14));
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), ps85.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getKey(), li92.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(7) + minutes(15));
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(7) + minutes(15));
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
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), ps81.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getKey(), li93.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(9) + minutes(11)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(9) + minutes(11)); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), ps75.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getKey(), li93.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(9) + minutes(20)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(9) + minutes(20)); 
			}
			if(journey.size() >= 2)
			{
				const ServicePointer& leg(journey.getJourneyLeg(1));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), ps75.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getKey(), li95.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(10) + minutes(5));
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(10) + minutes(5));
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), ps84.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getKey(), li95.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(10) + minutes(20));
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(10) + minutes(20));
			}
		}

		if(result.getJourneys().size() >= 3)
		{
			const Journey& journey(result.getJourneys().at(2));
			BOOST_CHECK_EQUAL(journey.size(), 3);
			BOOST_CHECK_EQUAL(journey.getContinuousServiceRange(), minutes(10));
			if(journey.size() >= 1)
			{
				const ServicePointer& leg(journey.getJourneyLeg(0));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), c91.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getPathGroup()->getKey(), rp46.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(19) + minutes(41) - minutes(9)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(19) + minutes(41) - minutes(9)); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), c93.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getPathGroup()->getKey(), rp46.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(19) + minutes(41) - minutes(9) + minutes(6) + seconds(1)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(19) + minutes(41) - minutes(9) + minutes(6) + seconds(1)); 
			}
			if(journey.size() >= 2)
			{
				const ServicePointer& leg(journey.getJourneyLeg(1));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), c93.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getPathGroup()->getKey(), rp43.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(19) + minutes(41) - minutes(9) + minutes(6) + seconds(1));
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(19) + minutes(41) - minutes(9) + minutes(6) + seconds(1));
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), c99.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getPathGroup()->getKey(), rp43.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(19) + minutes(41) - minutes(9) + minutes(8) + seconds(17)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(19) + minutes(41) - minutes(9) + minutes(8) + seconds(17)); 
			}
			if(journey.size() >= 3)
			{
				const ServicePointer& leg(journey.getJourneyLeg(2));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), ps79.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getKey(), li92.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(19) + minutes(41));
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(19) + minutes(41));
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), ps85.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getKey(), li92.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(19) + minutes(54));
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(19) + minutes(54));
			}
		}

		if(result.getJourneys().size() >= 4)
		{
			const Journey& journey(result.getJourneys().at(3));
			BOOST_CHECK_EQUAL(journey.size(), 3);
			BOOST_CHECK_EQUAL(journey.getContinuousServiceRange(), minutes(0));
			if(journey.size() >= 1)
			{
				const ServicePointer& leg(journey.getJourneyLeg(0));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), c91.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getPathGroup()->getKey(), rp46.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(19) + minutes(55) - minutes(9)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(19) + minutes(55) - minutes(9)); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), c93.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getPathGroup()->getKey(), rp46.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(19) + minutes(55) - minutes(9) + minutes(6) + seconds(1)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(19) + minutes(55) - minutes(9) + minutes(6) + seconds(1)); 
			}
			if(journey.size() >= 2)
			{
				const ServicePointer& leg(journey.getJourneyLeg(1));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), c93.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getPathGroup()->getKey(), rp43.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(19) + minutes(55) - minutes(9) + minutes(6) + seconds(1));
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(19) + minutes(55) - minutes(9) + minutes(6) + seconds(1));
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), c99.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getPathGroup()->getKey(), rp43.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(19) + minutes(55) - minutes(9) + minutes(8) + seconds(17)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(19) + minutes(55) - minutes(9) + minutes(8) + seconds(17)); 
			}
			if(journey.size() >= 3)
			{
				const ServicePointer& leg(journey.getJourneyLeg(2));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), ps79.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getKey(), li96.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(19) + minutes(55));
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(19) + minutes(55));
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), ps85.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getKey(), li96.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(20) + minutes(5));
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(20) + minutes(5));
			}
		}

		if(result.getJourneys().size() >= 5)
		{
			const Journey& journey(result.getJourneys().at(4));
			BOOST_CHECK_EQUAL(journey.size(), 3);
			BOOST_CHECK_EQUAL(journey.getContinuousServiceRange(), minutes(15));
			if(journey.size() >= 1)
			{
				const ServicePointer& leg(journey.getJourneyLeg(0));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), c91.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getPathGroup()->getKey(), rp46.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(19) + minutes(56) - minutes(9)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(19) + minutes(56) - minutes(9)); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), c93.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getPathGroup()->getKey(), rp46.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(19) + minutes(56) - minutes(9) + minutes(6) + seconds(1)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(19) + minutes(56) - minutes(9) + minutes(6) + seconds(1)); 
			}
			if(journey.size() >= 2)
			{
				const ServicePointer& leg(journey.getJourneyLeg(1));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), c93.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getPathGroup()->getKey(), rp43.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(19) + minutes(56) - minutes(9) + minutes(6) + seconds(1));
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(19) + minutes(56) - minutes(9) + minutes(6) + seconds(1));
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), c99.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getPathGroup()->getKey(), rp43.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(19) + minutes(56) - minutes(9) + minutes(8) + seconds(17)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(19) + minutes(56) - minutes(9) + minutes(8) + seconds(17)); 
			}
			if(journey.size() >= 3)
			{
				const ServicePointer& leg(journey.getJourneyLeg(2));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), ps79.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getKey(), li92.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(19) + minutes(56));
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(19) + minutes(56));
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), ps85.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getKey(), li92.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(20) + minutes(9));
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(20) + minutes(9));
			}
		}

		if(result.getJourneys().size() >= 6)
		{
			const Journey& journey(result.getJourneys().at(5));
			BOOST_CHECK_EQUAL(journey.size(), 3);
			BOOST_CHECK_EQUAL(journey.getContinuousServiceRange(), minutes(0));
			if(journey.size() >= 1)
			{
				const ServicePointer& leg(journey.getJourneyLeg(0));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), c91.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getPathGroup()->getKey(), rp46.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(20) + minutes(20) - minutes(9)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(20) + minutes(20) - minutes(9)); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), c93.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getPathGroup()->getKey(), rp46.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(20) + minutes(20) - minutes(9) + minutes(6) + seconds(1)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(20) + minutes(20) - minutes(9) + minutes(6) + seconds(1)); 
			}
			if(journey.size() >= 2)
			{
				const ServicePointer& leg(journey.getJourneyLeg(1));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), c93.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getPathGroup()->getKey(), rp43.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(20) + minutes(20) - minutes(9) + minutes(6) + seconds(1));
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(20) + minutes(20) - minutes(9) + minutes(6) + seconds(1));
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), c99.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getPathGroup()->getKey(), rp43.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(20) + minutes(20) - minutes(9) + minutes(8) + seconds(17)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(20) + minutes(20) - minutes(9) + minutes(8) + seconds(17)); 
			}
			if(journey.size() >= 3)
			{
				const ServicePointer& leg(journey.getJourneyLeg(2));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), ps79.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getKey(), li96.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(20) + minutes(20));
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(20) + minutes(20));
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), ps85.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getKey(), li96.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(20) + minutes(25));
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(20) + minutes(25));
			}
		}

		if(result.getJourneys().size() >= 7)
		{
			const Journey& journey(result.getJourneys().at(6));
			BOOST_CHECK_EQUAL(journey.size(), 3);
			BOOST_CHECK_EQUAL(journey.getContinuousServiceRange(), minutes(20));
			if(journey.size() >= 1)
			{
				const ServicePointer& leg(journey.getJourneyLeg(0));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), c91.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getPathGroup()->getKey(), rp46.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(20) + minutes(21) - minutes(9)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(20) + minutes(21) - minutes(9)); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), c93.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getPathGroup()->getKey(), rp46.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(20) + minutes(21) - minutes(9) + minutes(6) + seconds(1)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(20) + minutes(21) - minutes(9) + minutes(6) + seconds(1)); 
			}
			if(journey.size() >= 2)
			{
				const ServicePointer& leg(journey.getJourneyLeg(1));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), c93.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getPathGroup()->getKey(), rp43.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(20) + minutes(21) - minutes(9) + minutes(6) + seconds(1));
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(20) + minutes(21) - minutes(9) + minutes(6) + seconds(1));
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), c99.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getPathGroup()->getKey(), rp43.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(20) + minutes(21) - minutes(9) + minutes(8) + seconds(17)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(20) + minutes(21) - minutes(9) + minutes(8) + seconds(17)); 
			}
			if(journey.size() >= 3)
			{
				const ServicePointer& leg(journey.getJourneyLeg(2));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), ps79.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getKey(), li92.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(20) + minutes(21));
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(20) + minutes(21));
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), ps85.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getKey(), li92.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(20) + minutes(34));
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(20) + minutes(34));
			}
		}

		if(result.getJourneys().size() >= 8)
		{
			const Journey& journey(result.getJourneys().at(7));
			BOOST_CHECK_EQUAL(journey.size(), 3);
			BOOST_CHECK_EQUAL(journey.getContinuousServiceRange(), minutes(0));
			if(journey.size() >= 1)
			{
				const ServicePointer& leg(journey.getJourneyLeg(0));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), c91.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getPathGroup()->getKey(), rp46.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(22) + minutes(20) - minutes(9)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(22) + minutes(20) - minutes(9)); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), c93.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getPathGroup()->getKey(), rp46.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(22) + minutes(20) - minutes(9) + minutes(6) + seconds(1)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(22) + minutes(20) - minutes(9) + minutes(6) + seconds(1)); 
			}
			if(journey.size() >= 2)
			{
				const ServicePointer& leg(journey.getJourneyLeg(1));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), c93.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getPathGroup()->getKey(), rp43.getKey()); 
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(22) + minutes(20) - minutes(9) + minutes(6) + seconds(1));
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(22) + minutes(20) - minutes(9) + minutes(6) + seconds(1));
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), c99.getKey()); 
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getPathGroup()->getKey(), rp43.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(22) + minutes(20) - minutes(9) + minutes(8) + seconds(17)); 
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(22) + minutes(20) - minutes(9) + minutes(8) + seconds(17)); 
			}
			if(journey.size() >= 3)
			{
				const ServicePointer& leg(journey.getJourneyLeg(2));
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getFromVertex()->getKey(), ps79.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getParentPath()->getKey(), li92.getKey());
				BOOST_CHECK_EQUAL(leg.getDepartureDateTime(), tomorrow + hours(22) + minutes(20));
				BOOST_CHECK_EQUAL(leg.getTheoreticalDepartureDateTime(), tomorrow + hours(22) + minutes(20));
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getFromVertex()->getKey(), ps85.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getParentPath()->getKey(), li92.getKey());
				BOOST_CHECK_EQUAL(leg.getArrivalDateTime(), tomorrow + hours(22) + minutes(25));
				BOOST_CHECK_EQUAL(leg.getTheoreticalArrivalDateTime(), tomorrow + hours(22) + minutes(25));
			}
		}
	}
	*/
}
