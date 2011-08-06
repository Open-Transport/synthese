
/** DepartureBoardsTest class implementation.
	@file ProjectionTest.cpp

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

#include "StandardArrivalDepartureTableGenerator.h"
#include "ForcedDestinationsArrivalDepartureTableGenerator.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "CommercialLine.h"
#include "JourneyPattern.hpp"
#include "DesignatedLinePhysicalStop.hpp"
#include "ScheduledService.h"
#include "DeparturesTableTypes.h"

#include <boost/test/auto_unit_test.hpp>

using namespace synthese::departure_boards;
using namespace synthese::graph;
using namespace synthese::util;
using namespace synthese::pt;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace std;
using namespace synthese;


BOOST_AUTO_TEST_CASE(DepartureBoardsTest)
{
	// Stops
	StopArea a1;
	StopPoint s11(11, "S11", &a1);
	a1.addPhysicalStop(s11);
	StopPoint s12(12, "S12", &a1);
	a1.addPhysicalStop(s12);
	
	StopArea a2;
	StopPoint s2(2, "S2", &a2);
	a2.addPhysicalStop(s2);

	StopArea a3;
	StopPoint s3(3, "S3", &a3);
	a3.addPhysicalStop(s3);

	StopArea a4;
	StopPoint s4(4, "S4", &a4);
	a4.addPhysicalStop(s4);

	StopArea a5;
	StopPoint s5(5, "S5", &a5);
	a5.addPhysicalStop(s5);

	// Dates
	date tomorrow(day_clock::local_day());
	ptime startDateTime(tomorrow, time_duration(8,0,0));
	ptime endDateTime(tomorrow, time_duration(18,0,0));

	// Line
	CommercialLine l;
	JourneyPattern jp1(1, "JP1");
	jp1.setCommercialLine(&l);
	
	DesignatedLinePhysicalStop jp1s1(0, &jp1, 0, true, true, 0, &s11);
	jp1.addEdge(jp1s1);

	DesignatedLinePhysicalStop jp1s2(0, &jp1, 1, true, true, 0, &s2);
	jp1.addEdge(jp1s2);

	DesignatedLinePhysicalStop jp1s3(0, &jp1, 2, true, true, 0, &s3);
	jp1.addEdge(jp1s3);

	DesignatedLinePhysicalStop jp1s4(0, &jp1, 3, true, true, 0, &s4);
	jp1.addEdge(jp1s4);

	ScheduledService jp1ser1(0, "Jp1Ser1", &jp1);
	ScheduledService::Schedules jp1ser1D;
	jp1ser1D.push_back(time_duration(8,15,0));
	jp1ser1D.push_back(time_duration(8,20,0));
	jp1ser1D.push_back(time_duration(8,30,0));
	jp1ser1D.push_back(time_duration(8,40,0));
	jp1ser1.setSchedules(jp1ser1D, jp1ser1D);
	jp1ser1.setActive(tomorrow);
	jp1.addService(jp1ser1, true);

	ScheduledService jp1ser2(0, "Jp1Ser2", &jp1);
	ScheduledService::Schedules jp1ser2D;
	jp1ser2D.push_back(time_duration(9,10,0));
	jp1ser2D.push_back(time_duration(9,11,0));
	jp1ser2D.push_back(time_duration(9,12,0));
	jp1ser2D.push_back(time_duration(9,13,0));
	jp1ser2.setSchedules(jp1ser2D, jp1ser2D);
	jp1ser2.setActive(tomorrow);
	jp1.addService(jp1ser2, true);

	JourneyPattern jp2(2, "JP2");
	jp2.setCommercialLine(&l);

	DesignatedLinePhysicalStop jp2s1(0, &jp2, 0, true, true, 0, &s12);
	jp2.addEdge(jp2s1);

	DesignatedLinePhysicalStop jp2s2(0, &jp2, 1, true, true, 0, &s2);
	jp2.addEdge(jp2s2);

	ScheduledService jp2ser1(0, "Jp2Ser1", &jp2);
	ScheduledService::Schedules jp2ser1D;
	jp2ser1D.push_back(time_duration(8,0,0));
	jp2ser1D.push_back(time_duration(8,1,0));
	jp2ser1.setSchedules(jp2ser1D, jp2ser1D);
	jp2ser1.setActive(tomorrow);
	jp2.addService(jp2ser1, true);

	ScheduledService jp2ser2(0, "Jp2Ser2", &jp2);
	ScheduledService::Schedules jp2ser2D;
	jp2ser2D.push_back(time_duration(8,10,0));
	jp2ser2D.push_back(time_duration(8,11,0));
	jp2ser2.setSchedules(jp2ser2D, jp2ser2D);
	jp2ser2.setActive(tomorrow);
	jp2.addService(jp2ser2, true);

	ScheduledService jp2ser3(0, "Jp2Ser3", &jp2);
	ScheduledService::Schedules jp2ser3D;
	jp2ser3D.push_back(time_duration(8,20,0));
	jp2ser3D.push_back(time_duration(8,21,0));
	jp2ser3.setSchedules(jp2ser3D, jp2ser3D);
	jp2ser3.setActive(tomorrow);
	jp2.addService(jp2ser3, true);

	{	// Test case 1
		// Stops : All stops of a1
		// Size : unlimited
		// Algorithm : standard

		StandardArrivalDepartureTableGenerator::PhysicalStops stops;
		stops.insert(make_pair(s11.getKey(), &s11));
		stops.insert(make_pair(s12.getKey(), &s12));

		StandardArrivalDepartureTableGenerator g(
			stops,
			DISPLAY_DEPARTURES,
			WITH_PASSING,
			LineFilter(),
			DisplayedPlacesList(),
			ForbiddenPlacesList(),
			startDateTime,
			endDateTime
		);
		const ArrivalDepartureList& result(g.generate());

		BOOST_REQUIRE_EQUAL(result.size(), 5);

		ArrivalDepartureList::const_iterator dep(result.begin());
		const ServicePointer& sp1(dep->first);
		BOOST_CHECK_EQUAL(sp1.getService()->getServiceNumber(), jp2ser1.getServiceNumber());

		++dep;
		const ServicePointer& sp2(dep->first);
		BOOST_CHECK_EQUAL(sp2.getService()->getServiceNumber(), jp2ser2.getServiceNumber());

		++dep;
		const ServicePointer& sp3(dep->first);
		BOOST_CHECK_EQUAL(sp3.getService()->getServiceNumber(), jp1ser1.getServiceNumber());

		++dep;
		const ServicePointer& sp4(dep->first);
		BOOST_CHECK_EQUAL(sp4.getService()->getServiceNumber(), jp2ser3.getServiceNumber());

		++dep;
		const ServicePointer& sp5(dep->first);
		BOOST_CHECK_EQUAL(sp5.getService()->getServiceNumber(), jp1ser2.getServiceNumber());
	}

	{	// Test case 2
		// Stops : All stops of a1
		// Size : 3
		// Algorithm : standard

		StandardArrivalDepartureTableGenerator::PhysicalStops stops;
		stops.insert(make_pair(s11.getKey(), &s11));
		stops.insert(make_pair(s12.getKey(), &s12));

		StandardArrivalDepartureTableGenerator g(
			stops,
			DISPLAY_DEPARTURES,
			WITH_PASSING,
			LineFilter(),
			DisplayedPlacesList(),
			ForbiddenPlacesList(),
			startDateTime,
			endDateTime,
			3
		);
		const ArrivalDepartureList& result(g.generate());

		BOOST_REQUIRE_EQUAL(result.size(), 3);

		ArrivalDepartureList::const_iterator dep(result.begin());
		const ServicePointer& sp1(dep->first);
		BOOST_CHECK_EQUAL(sp1.getService()->getServiceNumber(), jp2ser1.getServiceNumber());

		++dep;
		const ServicePointer& sp2(dep->first);
		BOOST_CHECK_EQUAL(sp2.getService()->getServiceNumber(), jp2ser2.getServiceNumber());

		++dep;
		const ServicePointer& sp3(dep->first);
		BOOST_CHECK_EQUAL(sp3.getService()->getServiceNumber(), jp1ser1.getServiceNumber());
	}

	{	// Test case 3
		// Stops : s11 only
		// Size : 3
		// Algorithm : standard

		StandardArrivalDepartureTableGenerator::PhysicalStops stops;
		stops.insert(make_pair(s11.getKey(), &s11));

		StandardArrivalDepartureTableGenerator g(
			stops,
			DISPLAY_DEPARTURES,
			WITH_PASSING,
			LineFilter(),
			DisplayedPlacesList(),
			ForbiddenPlacesList(),
			startDateTime,
			endDateTime,
			3
		);
		const ArrivalDepartureList& result(g.generate());

		BOOST_REQUIRE_EQUAL(result.size(), 2);

		ArrivalDepartureList::const_iterator dep(result.begin());
		const ServicePointer& sp1(dep->first);
		BOOST_CHECK_EQUAL(sp1.getService()->getServiceNumber(), jp1ser1.getServiceNumber());

		++dep;
		const ServicePointer& sp2(dep->first);
		BOOST_CHECK_EQUAL(sp2.getService()->getServiceNumber(), jp1ser2.getServiceNumber());
	}

	{	// Test case 4
		// Stops : all stops
		// Size : 2
		// Algorithm : preselection
		
		ForcedDestinationsArrivalDepartureTableGenerator::PhysicalStops stops;
		stops.insert(make_pair(s11.getKey(), &s11));
		stops.insert(make_pair(s12.getKey(), &s12));

		ForcedDestinationsArrivalDepartureTableGenerator g(
			stops,
			DISPLAY_DEPARTURES,
			WITH_PASSING,
			LineFilter(),
			DisplayedPlacesList(),
			ForbiddenPlacesList(),
			startDateTime,
			endDateTime,
			2,
			ForcedDestinationsArrivalDepartureTableGenerator::ForcedDestinationsSet(),
			hours(2)
		);
		const ArrivalDepartureList& result(g.generate());

		BOOST_REQUIRE_EQUAL(result.size(), 2);

		ArrivalDepartureList::const_iterator dep(result.begin());
		const ServicePointer& sp1(dep->first);
		BOOST_CHECK_EQUAL(sp1.getService()->getServiceNumber(), jp2ser1.getServiceNumber());

		++dep;
		const ServicePointer& sp2(dep->first);
		BOOST_CHECK_EQUAL(sp2.getService()->getServiceNumber(), jp1ser1.getServiceNumber());
	}

}
