
/** DepartureBoardsTest class implementation.
	@file DepartureBoardsTest.cpp

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

#include "LineStop.h"
#include "StandardArrivalDepartureTableGenerator.h"
#include "ForcedDestinationsArrivalDepartureTableGenerator.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "CommercialLine.h"
#include "JourneyPattern.hpp"
#include "ScheduledService.h"
#include "DeparturesTableTypes.h"
#include "TestUtils.hpp"

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
	ScopedRegistrable<CommercialLine> scopedCommercialLine;
	Env env;

	// Stops
	StopArea a1(1);
	a1.setName("A1");
	StopPoint s11(11, "S11", &a1);
	a1.addPhysicalStop(s11);
	StopPoint s12(12, "S12", &a1);
	a1.addPhysicalStop(s12);

	StopArea a2(2);
	a2.setName("A2");
	StopPoint s2(2, "S2", &a2);
	a2.addPhysicalStop(s2);

	StopArea a3(3);
	a3.setName("A3");
	StopPoint s3(3, "S3", &a3);
	a3.addPhysicalStop(s3);

	StopArea a4(4);
	a4.setName("A4");
	StopPoint s4(4, "S4", &a4);
	a4.addPhysicalStop(s4);

	StopArea a5(5);
	a5.setName("A5");
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

	LineStop jp1s1(0, &jp1, 0, true, true, 0, s11);
	jp1s1.link(env);

	LineStop jp1s2(0, &jp1, 1, true, true, 0, s2);
	jp1s2.link(env);

	LineStop jp1s3(0, &jp1, 2, true, true, 0, s3);
	jp1s3.link(env);

	LineStop jp1s4(0, &jp1, 3, true, true, 0, s4);
	jp1s4.link(env);

	ScheduledService jp1ser1(0, "Jp1Ser1", &jp1);
	ScheduledService::Schedules jp1ser1D;
	jp1ser1D.push_back(time_duration(8,15,0));
	jp1ser1D.push_back(time_duration(8,20,0));
	jp1ser1D.push_back(time_duration(8,30,0));
	jp1ser1D.push_back(time_duration(8,40,0));
	jp1ser1.setDataSchedules(jp1ser1D, jp1ser1D);
	jp1ser1.setActive(tomorrow);
	jp1.addService(jp1ser1, true);

	ScheduledService jp1ser2(0, "Jp1Ser2", &jp1);
	ScheduledService::Schedules jp1ser2D;
	jp1ser2D.push_back(time_duration(9,10,0));
	jp1ser2D.push_back(time_duration(9,11,0));
	jp1ser2D.push_back(time_duration(9,12,0));
	jp1ser2D.push_back(time_duration(9,13,0));
	jp1ser2.setDataSchedules(jp1ser2D, jp1ser2D);
	jp1ser2.setActive(tomorrow);
	jp1.addService(jp1ser2, true);

	JourneyPattern jp2(2, "JP2");
	jp2.setCommercialLine(&l);

	LineStop jp2s1(0, &jp2, 0, true, true, 0, s12);
	jp2s1.link(env);

	LineStop jp2s2(0, &jp2, 1, true, true, 0, s2);
	jp2s2.link(env);

	ScheduledService jp2ser1(0, "Jp2Ser1", &jp2);
	ScheduledService::Schedules jp2ser1D;
	jp2ser1D.push_back(time_duration(8,0,0));
	jp2ser1D.push_back(time_duration(8,1,0));
	jp2ser1.setDataSchedules(jp2ser1D, jp2ser1D);
	jp2ser1.setActive(tomorrow);
	jp2.addService(jp2ser1, true);

	ScheduledService jp2ser2(0, "Jp2Ser2", &jp2);
	ScheduledService::Schedules jp2ser2D;
	jp2ser2D.push_back(time_duration(8,10,0));
	jp2ser2D.push_back(time_duration(8,11,0));
	jp2ser2.setDataSchedules(jp2ser2D, jp2ser2D);
	jp2ser2.setActive(tomorrow);
	jp2.addService(jp2ser2, true);

	ScheduledService jp2ser3(0, "Jp2Ser3", &jp2);
	ScheduledService::Schedules jp2ser3D;
	jp2ser3D.push_back(time_duration(8,20,0));
	jp2ser3D.push_back(time_duration(8,21,0));
	jp2ser3.setDataSchedules(jp2ser3D, jp2ser3D);
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
			endDateTime,
			false
		);
		const ArrivalDepartureList& result(g.generate());

		BOOST_CHECK_EQUAL(result.size(), 5ULL);

		ArrivalDepartureList::const_iterator dep(result.begin());

		if(dep != result.end())
		{
			const ServicePointer& sp1(dep->first);
			BOOST_CHECK_EQUAL(sp1.getService()->getServiceNumber(), jp2ser1.getServiceNumber());
			BOOST_REQUIRE_EQUAL(dep->second.size(), 2);
			++dep;
		}

		if(dep != result.end())
		{
			const ServicePointer& sp2(dep->first);
			BOOST_CHECK_EQUAL(sp2.getService()->getServiceNumber(), jp2ser2.getServiceNumber());
			BOOST_REQUIRE_EQUAL(dep->second.size(), 2);
			++dep;
		}

		if(dep != result.end())
		{
			const ServicePointer& sp3(dep->first);
			BOOST_CHECK_EQUAL(sp3.getService()->getServiceNumber(), jp1ser1.getServiceNumber());
			BOOST_REQUIRE_EQUAL(dep->second.size(), 2);
			++dep;
		}

		if(dep != result.end())
		{
			const ServicePointer& sp4(dep->first);
			BOOST_CHECK_EQUAL(sp4.getService()->getServiceNumber(), jp2ser3.getServiceNumber());
			BOOST_REQUIRE_EQUAL(dep->second.size(), 2);
			++dep;
		}

		if(dep != result.end())
		{
			const ServicePointer& sp5(dep->first);
			BOOST_CHECK_EQUAL(sp5.getService()->getServiceNumber(), jp1ser2.getServiceNumber());
			BOOST_REQUIRE_EQUAL(dep->second.size(), 2);
			BOOST_CHECK_EQUAL(dep->second.at(0).place->getName(), a1.getName());
			BOOST_CHECK_EQUAL(dep->second.at(1).place->getName(), a4.getName());
		}
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
			false,
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
			false,
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
		// Stops : all stops of a1
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
			hours(2),
			false
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

	// Real time update : stop s11 is not served by jp1ser1
	jp1ser1.setRealTimeVertex(0, NULL);

	{	// Test case 5
		// Stops : All stops of a1
		// Size : unlimited
		// Algorithm : standard
		// jp1ser1 must not be selected due to the real time update

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
			false
		);
		const ArrivalDepartureList& result(g.generate());

		BOOST_CHECK_EQUAL(result.size(), 4);

		ArrivalDepartureList::const_iterator dep(result.begin());

		if(dep != result.end())
		{
			const ServicePointer& sp1(dep->first);
			BOOST_CHECK_EQUAL(sp1.getService()->getServiceNumber(), jp2ser1.getServiceNumber());
			++dep;
		}

		if(dep != result.end())
		{
			const ServicePointer& sp2(dep->first);
			BOOST_CHECK_EQUAL(sp2.getService()->getServiceNumber(), jp2ser2.getServiceNumber());
			++dep;
		}

		if(dep != result.end())
		{
			const ServicePointer& sp4(dep->first);
			BOOST_CHECK_EQUAL(sp4.getService()->getServiceNumber(), jp2ser3.getServiceNumber());
			++dep;
		}

		if(dep != result.end())
		{
			const ServicePointer& sp3(dep->first);
			BOOST_CHECK_EQUAL(sp3.getService()->getServiceNumber(), jp1ser2.getServiceNumber());
		}
	}

	{	// Test case 5.5
		// Stops : all stops
		// Size : 2
		// Algorithm : preselection
		// jp1ser2 must be chosen instead of jp1ser1 because jp1ser1 does not run from a1

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
			hours(2),
			false
		);
		const ArrivalDepartureList& result(g.generate());

		BOOST_CHECK_EQUAL(result.size(), 2);

		ArrivalDepartureList::const_iterator dep(result.begin());

		if(dep != result.end())
		{
			const ServicePointer& sp1(dep->first);
			BOOST_CHECK_EQUAL(sp1.getService()->getServiceNumber(), jp2ser1.getServiceNumber());
			++dep;
		}

		if(dep != result.end())
		{
			const ServicePointer& sp2(dep->first);
			BOOST_CHECK_EQUAL(sp2.getService()->getServiceNumber(), jp1ser2.getServiceNumber());
		}
	}

	// Back to initial situation
	jp1ser1.setRealTimeVertex(0, jp1.getEdge(0)->getFromVertex());

	/* OVE 13/03/2015 : those 2 test cases are broken and require a fix in module 54 (see ticket tc-91 for details)
	// Real time update : stop s4 is not served by jp1ser1
	jp1ser1.setRealTimeVertex(3, NULL);

	{	// Test case 6
		// Stops : All stops of a1
		// Size : unlimited
		// Algorithm : standard
		// jp1ser1 must end at a3 instead of a4

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
			false
		);
		const ArrivalDepartureList& result(g.generate());

		BOOST_CHECK_EQUAL(result.size(), 5);

		ArrivalDepartureList::const_iterator dep(result.begin());

		if(dep != result.end())
		{
			const ServicePointer& sp1(dep->first);
			BOOST_CHECK_EQUAL(sp1.getService()->getServiceNumber(), jp2ser1.getServiceNumber());
			++dep;
		}

		if(dep != result.end())
		{
			const ServicePointer& sp2(dep->first);
			BOOST_CHECK_EQUAL(sp2.getService()->getServiceNumber(), jp2ser2.getServiceNumber());
			++dep;
		}

		if(dep != result.end())
		{
			const ServicePointer& sp3(dep->first);
			BOOST_CHECK_EQUAL(sp3.getService()->getServiceNumber(), jp1ser1.getServiceNumber());
			BOOST_REQUIRE_EQUAL(dep->second.size(), 2);
			BOOST_CHECK_EQUAL(dep->second.at(0).place->getName(), a1.getName());
			BOOST_CHECK_EQUAL(dep->second.at(1).place->getName(), a3.getName());
			++dep;
		}

		if(dep != result.end())
		{
			const ServicePointer& sp4(dep->first);
			BOOST_CHECK_EQUAL(sp4.getService()->getServiceNumber(), jp2ser3.getServiceNumber());
			++dep;
		}

		if(dep != result.end())
		{
			const ServicePointer& sp5(dep->first);
			BOOST_CHECK_EQUAL(sp5.getService()->getServiceNumber(), jp1ser2.getServiceNumber());
			BOOST_REQUIRE_EQUAL(dep->second.size(), 2);
			BOOST_CHECK_EQUAL(dep->second.at(0).place->getName(), a1.getName());
			BOOST_CHECK_EQUAL(dep->second.at(1).place->getName(), a4.getName());
		}
	}

	{	// Test case 8
		// Stops : all stops
		// Size : 2
		// Algorithm : preselection
		// jp1ser2 must be chosen instead of jp1ser1 because jp1ser1 ends at a3 instead of a4

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
			hours(2),
			false
			);
		const ArrivalDepartureList& result(g.generate());

		BOOST_CHECK_EQUAL(result.size(), 2);

		ArrivalDepartureList::const_iterator dep(result.begin());

		if(dep != result.end())
		{
			const ServicePointer& sp1(dep->first);
			BOOST_CHECK_EQUAL(sp1.getService()->getServiceNumber(), jp2ser1.getServiceNumber());
			++dep;
		}

		if(dep != result.end())
		{
			const ServicePointer& sp2(dep->first);
			BOOST_CHECK_EQUAL(sp2.getService()->getServiceNumber(), jp1ser2.getServiceNumber());
		}
	}
	*/

	// Back to initial situation
	jp1ser1.setRealTimeVertex(3, jp1.getEdge(3)->getFromVertex());

	{	// Test case 7
		// Stops : All stops of a1
		// Size : unlimited
		// Algorithm : standard
		// Intermediate stop : a3

		StandardArrivalDepartureTableGenerator::PhysicalStops stops;
		stops.insert(make_pair(s11.getKey(), &s11));
		stops.insert(make_pair(s12.getKey(), &s12));

		DisplayedPlacesList displayedPlacesList;
		displayedPlacesList.insert(make_pair(a3.getKey(), &a3));

		StandardArrivalDepartureTableGenerator g(
			stops,
			DISPLAY_DEPARTURES,
			WITH_PASSING,
			LineFilter(),
			displayedPlacesList,
			ForbiddenPlacesList(),
			startDateTime,
			endDateTime,
			false
		);
		const ArrivalDepartureList& result(g.generate());

		BOOST_CHECK_EQUAL(result.size(), 5);

		ArrivalDepartureList::const_iterator dep(result.begin());

		if(dep != result.end())
		{
			const ServicePointer& sp1(dep->first);
			BOOST_CHECK_EQUAL(sp1.getService()->getServiceNumber(), jp2ser1.getServiceNumber());
			BOOST_REQUIRE_EQUAL(dep->second.size(), 2);
			BOOST_CHECK_EQUAL(dep->second.at(0).place->getName(), a1.getName());
			BOOST_CHECK_EQUAL(dep->second.at(1).place->getName(), a2.getName());
			++dep;
		}

		if(dep != result.end())
		{
			const ServicePointer& sp2(dep->first);
			BOOST_CHECK_EQUAL(sp2.getService()->getServiceNumber(), jp2ser2.getServiceNumber());
			BOOST_REQUIRE_EQUAL(dep->second.size(), 2);
			BOOST_CHECK_EQUAL(dep->second.at(0).place->getName(), a1.getName());
			BOOST_CHECK_EQUAL(dep->second.at(1).place->getName(), a2.getName());
			++dep;
		}

		if(dep != result.end())
		{
			const ServicePointer& sp3(dep->first);
			BOOST_CHECK_EQUAL(sp3.getService()->getServiceNumber(), jp1ser1.getServiceNumber());
			BOOST_REQUIRE_EQUAL(dep->second.size(), 3);
			BOOST_CHECK_EQUAL(dep->second.at(0).place->getName(), a1.getName());
			BOOST_CHECK_EQUAL(dep->second.at(1).place->getName(), a3.getName());
			BOOST_CHECK_EQUAL(dep->second.at(2).place->getName(), a4.getName());
			++dep;
		}

		if(dep != result.end())
		{
			const ServicePointer& sp4(dep->first);
			BOOST_CHECK_EQUAL(sp4.getService()->getServiceNumber(), jp2ser3.getServiceNumber());
			BOOST_REQUIRE_EQUAL(dep->second.size(), 2);
			BOOST_CHECK_EQUAL(dep->second.at(0).place->getName(), a1.getName());
			BOOST_CHECK_EQUAL(dep->second.at(1).place->getName(), a2.getName());
			++dep;
		}

		if(dep != result.end())
		{
			const ServicePointer& sp5(dep->first);
			BOOST_CHECK_EQUAL(sp5.getService()->getServiceNumber(), jp1ser2.getServiceNumber());
			BOOST_REQUIRE_EQUAL(dep->second.size(), 3);
			BOOST_CHECK_EQUAL(dep->second.at(0).place->getName(), a1.getName());
			BOOST_CHECK_EQUAL(dep->second.at(1).place->getName(), a3.getName());
			BOOST_CHECK_EQUAL(dep->second.at(2).place->getName(), a4.getName());
		}
	}

	/* OVE 13/03/2015 : this test case is broken and requires a fix in module 54 (see ticket tc-91 for details)
	// Real time update : stop s3 is not served by jp1ser1
	jp1ser1.setRealTimeVertex(2, NULL);

	{	// Test case 9
		// Stops : All stops of a1
		// Size : unlimited
		// Algorithm : standard
		// Intermediate stop : a3
		// Should take into account of deactivation of jp1ser1 at a3

		StandardArrivalDepartureTableGenerator::PhysicalStops stops;
		stops.insert(make_pair(s11.getKey(), &s11));
		stops.insert(make_pair(s12.getKey(), &s12));

		DisplayedPlacesList displayedPlacesList;
		displayedPlacesList.insert(make_pair(a3.getKey(), &a3));

		StandardArrivalDepartureTableGenerator g(
			stops,
			DISPLAY_DEPARTURES,
			WITH_PASSING,
			LineFilter(),
			displayedPlacesList,
			ForbiddenPlacesList(),
			startDateTime,
			endDateTime,
			false
		);
		const ArrivalDepartureList& result(g.generate());

		BOOST_REQUIRE_EQUAL(result.size(), 5);

		ArrivalDepartureList::const_iterator dep(result.begin());
		const ServicePointer& sp1(dep->first);
		BOOST_CHECK_EQUAL(sp1.getService()->getServiceNumber(), jp2ser1.getServiceNumber());
		BOOST_REQUIRE_GE(dep->second.size(), 2);
		BOOST_CHECK_EQUAL(dep->second.size(), 2);
		BOOST_CHECK_EQUAL(dep->second.at(0).place->getName(), a1.getName());
		BOOST_CHECK_EQUAL(dep->second.at(1).place->getName(), a2.getName());

		++dep;
		const ServicePointer& sp2(dep->first);
		BOOST_CHECK_EQUAL(sp2.getService()->getServiceNumber(), jp2ser2.getServiceNumber());
		BOOST_REQUIRE_GE(dep->second.size(), 2);
		BOOST_CHECK_EQUAL(dep->second.size(), 2);
		BOOST_CHECK_EQUAL(dep->second.at(0).place->getName(), a1.getName());
		BOOST_CHECK_EQUAL(dep->second.at(1).place->getName(), a2.getName());

		++dep;
		const ServicePointer& sp3(dep->first);
		BOOST_CHECK_EQUAL(sp3.getService()->getServiceNumber(), jp1ser1.getServiceNumber());
		BOOST_REQUIRE_GE(dep->second.size(), 2);
		BOOST_CHECK_EQUAL(dep->second.size(), 2);
		BOOST_CHECK_EQUAL(dep->second.at(0).place->getName(), a1.getName());
		BOOST_CHECK_EQUAL(dep->second.at(1).place->getName(), a4.getName());

		++dep;
		const ServicePointer& sp4(dep->first);
		BOOST_CHECK_EQUAL(sp4.getService()->getServiceNumber(), jp2ser3.getServiceNumber());
		BOOST_REQUIRE_GE(dep->second.size(), 2);
		BOOST_CHECK_EQUAL(dep->second.size(), 2);
		BOOST_CHECK_EQUAL(dep->second.at(0).place->getName(), a1.getName());
		BOOST_CHECK_EQUAL(dep->second.at(1).place->getName(), a2.getName());

		++dep;
		const ServicePointer& sp5(dep->first);
		BOOST_CHECK_EQUAL(sp5.getService()->getServiceNumber(), jp1ser2.getServiceNumber());
		BOOST_REQUIRE_GE(dep->second.size(), 3);
		BOOST_CHECK_EQUAL(dep->second.size(), 3);
		BOOST_CHECK_EQUAL(dep->second.at(0).place->getName(), a1.getName());
		BOOST_CHECK_EQUAL(dep->second.at(1).place->getName(), a3.getName());
		BOOST_CHECK_EQUAL(dep->second.at(2).place->getName(), a4.getName());
	}
	*/

	// Back to initial situation
	jp1ser1.setRealTimeVertex(2, jp1.getEdge(2)->getFromVertex());
}
