
/** ScheduleRealTime unit test.
	@file ScheduleRealTimeTest.cpp

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

#include "AccessParameters.h"
#include "AlgorithmLogger.hpp"
#include "AreaGeneratedLineStop.hpp"
#include "CommercialLine.h"
#include "ContinuousService.h"
#include "DynamicRequest.h"
#include "FreeDRTArea.hpp"
#include "GeographyModule.h"
#include "HTTPRequest.hpp"
#include "Hub.h"
#include "LineStop.h"
#include "JourneyPattern.hpp"
#include "NonConcurrencyRule.h"
#include "Path.h"
#include "PTTimeSlotRoutePlanner.h"
#include "ScheduledService.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "TestUtils.hpp"
#include "UtilConstants.h"

#include <iostream>

#include <boost/test/auto_unit_test.hpp>

#define NB_STOP 2

using namespace boost::posix_time;

using namespace synthese;
using namespace synthese::algorithm;
using namespace synthese::geography;
using namespace synthese::graph;
using namespace synthese::impex;
using namespace synthese::pt;
using namespace synthese::pt_journey_planner;
using namespace synthese::server;
using namespace synthese::util;

using namespace std;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost;

// Create, fill and register a scheduled service all in once
class TestScheduledService
{
private:
	boost::shared_ptr<ScheduledService> _scheduledService;
public:
	TestScheduledService(
		RegistryKeyType id,
		string serviceNumber,
		JourneyPattern &jp,
		time_duration startTime
	):	_scheduledService(boost::shared_ptr<ScheduledService>(new ScheduledService(id, serviceNumber, &jp)))
	{
		{
			ScheduledService::Schedules a;
			ScheduledService::Schedules d;

			a.push_back(time_duration(0,0,0));
			d.push_back(startTime);

			for(size_t i = 1; i < jp.getScheduledStopsNumber(); i++)
			{
				startTime += time_duration(0,4,0);
				a.push_back(startTime);
				startTime += time_duration(0,1,0);
				d.push_back(startTime);
			}

			_scheduledService->setDataSchedules(d, a);
		}
		jp.addService(*_scheduledService, true);
		_scheduledService->setActive(day_clock::local_day());
	}

};

// Create, fill and register a continuous service all in once
class TestContinuousService
{
private:
	boost::shared_ptr<ContinuousService> _continuousService;
public:
	TestContinuousService(
		RegistryKeyType id,
		string serviceNumber,
		JourneyPattern &jp,
		time_duration startTime,
		time_duration range
	):	_continuousService(boost::shared_ptr<ContinuousService>(new ContinuousService(id, serviceNumber, &jp)))
	{
		{
			ContinuousService::Schedules a;
			ContinuousService::Schedules d;

			a.push_back(time_duration(0,0,0));
			d.push_back(startTime);

			for(size_t i = 1; i < jp.getScheduledStopsNumber(); i++)
			{
				startTime += time_duration(0,4,0);
				a.push_back(startTime);
				startTime += time_duration(0,1,0);
				d.push_back(startTime);
			}

			_continuousService->setDataSchedules(d, a);
			_continuousService->setRange(range);
			_continuousService->setMaxWaitingTime(time_duration(0,5,0));
		}
		jp.addService(*_continuousService, true);
		_continuousService->setActive(day_clock::local_day());
	}

};

class TestAreaMap
{
private:
	vector<boost::shared_ptr<StopArea> > _stopAreas;
	vector<boost::shared_ptr<StopPoint> > _stopPoints;
	size_t _numberOfStops;
public:
	TestAreaMap(
		size_t numberOfStops
	):	_numberOfStops(numberOfStops)
	{
		for(size_t i=0; i< _numberOfStops; ++i)
		{
			boost::shared_ptr<StopArea> stopArea(new StopArea(i, true));
			_stopAreas.push_back(stopArea);
			_stopPoints.push_back(boost::shared_ptr<StopPoint>(new StopPoint(0, "", &*_stopAreas[i])));
			_stopAreas[i]->addPhysicalStop(*_stopPoints[i]);
		}
	}
	size_t getNumberOfStops()
	{
		return _numberOfStops;
	}
	vector<boost::shared_ptr<StopPoint> > getStopPoints()
	{
		return _stopPoints;
	}
	vector<boost::shared_ptr<StopArea> > getStopAreas()
	{
		return _stopAreas;
	}
};

class TestJourney
{
private:
	vector<boost::shared_ptr<LineStop> > _designatedLinePhysicalStops;
	vector<boost::shared_ptr<StopPoint> > _stopPoints;
	size_t _numberOfStops;
	Env _env;
public:
	TestJourney(JourneyPattern &jp, TestAreaMap &testAreaMap):
		_stopPoints(testAreaMap.getStopPoints()),
		_numberOfStops(testAreaMap.getNumberOfStops())
	{
		for(size_t i=0; i< _numberOfStops; ++i)
		{
			_designatedLinePhysicalStops.push_back(
				boost::shared_ptr<LineStop>(
				new LineStop(i, &jp, i,
				(i < _numberOfStops - 1 ? true : false), // Departure
				(i > 0 ? true : false),  // Arrival
				0, *_stopPoints[i]))
			);
			_designatedLinePhysicalStops[i]->link(_env);;
		}
	}

};

void resultDump(const PTRoutePlannerResult &result)
{
	size_t i(0);
	BOOST_FOREACH(const PTRoutePlannerResult::Journeys::value_type& journey, result.getJourneys())
	{
		const Journey::ServiceUses& jl(journey.getServiceUses());
		const ServicePointer& leg(jl[0]);
		const JourneyPattern* line(dynamic_cast<const JourneyPattern*>(leg.getService()->getPath()));
		cout << "Journey " << i++ << " departure " << journey.getFirstDepartureTime().time_of_day() 
			 << " " <<line->getCommercialLine()->getShortName() << " range " << leg.getServiceRange()
			 << " arrival " << journey.getLastArrivalTime() << endl;
	}	
}

void checkJourneyEquals(size_t i, PTRoutePlannerResult::Journeys& journeys,
						const string& expectedLine, const ptime& expectedTime,
						const time_duration& expectedDuration)
{
	const PTRoutePlannerResult::Journeys::value_type journey(journeys[i]);
	const Journey::ServiceUses& jl(journey.getServiceUses());
	const ServicePointer& leg(jl[0]);
	const JourneyPattern* line(dynamic_cast<const JourneyPattern*>(leg.getService()->getPath()));

	cout << "CheckJourneyEquals " << i << endl;
	BOOST_CHECK_EQUAL(line->getCommercialLine()->getShortName(), expectedLine);

	BOOST_CHECK_EQUAL(to_iso_extended_string(journey.getFirstDepartureTime()), to_iso_extended_string(expectedTime));
	// There is a single leg
	BOOST_CHECK_EQUAL(jl.size(), 1ULL);
	BOOST_CHECK_EQUAL(to_simple_string(leg.getServiceRange()), to_simple_string(expectedDuration));
	BOOST_CHECK_EQUAL(leg.getDepartureEdge()->getKey(), 0ULL);
	
	/// @FIXME In continuous case the ArrivalEdge is correct but not its key
	//cout << "leg.getArrivalEdge()->getKey() = " << leg.getArrivalEdge()->getKey() << endl;
	//BOOST_CHECK_EQUAL(leg.getArrivalEdge()->getKey(), NB_STOP-1);
}
//#if 0
/**
 * We test that scheduled services with a non concurrency rule are
 * properly exculed (or accepted) against scheduled services.
 */
BOOST_AUTO_TEST_CASE (scheduledVSscheduled)
{
	ScopedRegistrable<CommercialLine> scopedCommercialLine;
	ScopedRegistrable<StopArea> scopedStopArea;

	TestAreaMap testAreaMap(NB_STOP);
	
	JourneyPattern jp1;
	CommercialLine cl1;
	cl1.addPath(&jp1);
	cl1.setShortName("CL1");
	jp1.setCommercialLine(&cl1);
	TestJourney tj1(jp1, testAreaMap);

	JourneyPattern jp2;
	CommercialLine cl2;
	cl1.addPath(&jp2);
	cl2.setShortName("CL2");
	jp2.setCommercialLine(&cl2);
	TestJourney tj2(jp2, testAreaMap);
	// Set a non concurrency rule
	NonConcurrencyRule ncr2(123456);
	ncr2.set<Delay>(time_duration(0, 5, 0));
	ncr2.set<PriorityLine>(cl1);
	CommercialLine::NonConcurrencyRules ncrs2;
	ncrs2.insert(&ncr2);
	cl2.setNonConcurrencyRules(ncrs2);
	//

	// The schedules of the priority line CL1
	vector<boost::shared_ptr<TestScheduledService> > tssVector;
	for(time_duration t(0,0,0); t < hours(8); t+=hours(1))
	{
		tssVector.push_back(boost::shared_ptr<TestScheduledService>(new TestScheduledService(0, "", jp1, t)));
	}
	// The schedules of the non priority line CL2
	TestScheduledService tss2(0, "", jp2, time_duration(2,30,0));
	TestScheduledService tss3(0, "", jp2, time_duration(3,0,4));
	TestScheduledService tss4(0, "", jp2, time_duration(4,30,0));
	TestScheduledService tss5(0, "", jp2, time_duration(4,56,0));


	AlgorithmLogger logger;
	AccessParameters::AllowedPathClasses pc;
	AccessParameters ap(
		USER_PEDESTRIAN, false, false, 1000, boost::posix_time::minutes(23), 1.11, 10, pc
	);

	ptime start_time(day_clock::local_day(), minutes(0));
	ptime end_time(day_clock::local_day(), hours(24));

	PTTimeSlotRoutePlanner r(
		testAreaMap.getStopAreas()[0].get(),
		testAreaMap.getStopAreas()[NB_STOP-1].get(),
		start_time,
		end_time,
		start_time,
		end_time,
		boost::optional<std::size_t>(),
		ap,
		DEPARTURE_FIRST,
		false,
		logger
	);
	PTRoutePlannerResult result(r.run());
	resultDump(result);
	BOOST_REQUIRE_EQUAL(result.getJourneys().size(), 10ULL);
	size_t i = 0;
	checkJourneyEquals(i++, result.getJourneys(), "CL1", start_time + time_duration(0, 0, 0), time_duration(0, 0, 0));
	checkJourneyEquals(i++, result.getJourneys(), "CL1", start_time + time_duration(1, 0, 0), time_duration(0, 0, 0));
	checkJourneyEquals(i++, result.getJourneys(), "CL1", start_time + time_duration(2, 0, 0), time_duration(0, 0, 0));
	checkJourneyEquals(i++, result.getJourneys(), "CL2", start_time + time_duration(2,30, 0), time_duration(0, 0, 0));
	checkJourneyEquals(i++, result.getJourneys(), "CL1", start_time + time_duration(3, 0, 0), time_duration(0, 0, 0));
	checkJourneyEquals(i++, result.getJourneys(), "CL1", start_time + time_duration(4, 0, 0), time_duration(0, 0, 0));
	checkJourneyEquals(i++, result.getJourneys(), "CL2", start_time + time_duration(4,30, 0), time_duration(0, 0, 0));
	checkJourneyEquals(i++, result.getJourneys(), "CL1", start_time + time_duration(5, 0, 0), time_duration(0, 0, 0));
	checkJourneyEquals(i++, result.getJourneys(), "CL1", start_time + time_duration(6, 0, 0), time_duration(0, 0, 0));
	checkJourneyEquals(i++, result.getJourneys(), "CL1", start_time + time_duration(7, 0, 0), time_duration(0, 0, 0));

	
	cout << "END" << endl;	
}

/**
 * We test that continuous services with a non concurrency rule are
 * properly accepted, excluled or shortened against scheduled services.
 */
BOOST_AUTO_TEST_CASE (scheduledVScontinuous)
{
	ScopedRegistrable<CommercialLine> scopedCommercialLine;

	TestAreaMap testAreaMap(NB_STOP);
	
	JourneyPattern jp1;
	CommercialLine cl1;
	cl1.addPath(&jp1);
	cl1.setShortName("SCHED1");
	jp1.setCommercialLine(&cl1);
	TestJourney tj1(jp1, testAreaMap);

	JourneyPattern jp2;
	CommercialLine cl2;
	cl1.addPath(&jp2);
	cl2.setShortName("CONTI1");
	jp2.setCommercialLine(&cl2);
	TestJourney tj2(jp2, testAreaMap);
	// Set a non concurrency rule
	NonConcurrencyRule ncr2(123456);
	ncr2.set<Delay>(time_duration(0, 10, 0));
	ncr2.set<PriorityLine>(cl1);
	CommercialLine::NonConcurrencyRules ncrs2;
	ncrs2.insert(&ncr2);
	cl2.setNonConcurrencyRules(ncrs2);
	//

	vector<boost::shared_ptr<TestScheduledService> > tssVector;
	int cptss(0);
	for(time_duration t(0,0,0); t < hours(11); t+=hours(1))
	{
		tssVector.push_back(boost::shared_ptr<TestScheduledService>(new TestScheduledService(cptss, "", jp1, t)));
		cptss++;
	}
	tssVector.push_back(boost::shared_ptr<TestScheduledService>(new TestScheduledService(cptss, "", jp1, time_duration(8, 15, 0))));
	// The schedules of the non priority continuous line CL2
	TestContinuousService tcs2(100, "CONT1", jp2, time_duration(2,30,0), time_duration(1,0,0));
	TestContinuousService tc32(101, "CONT2", jp2, time_duration(4,30,0), time_duration(2,0,0));
	TestContinuousService tc33(102, "CONT3", jp2, time_duration(7,30,0), time_duration(2,0,0));


	AlgorithmLogger logger;
	AccessParameters::AllowedPathClasses pc;
	AccessParameters ap(
		USER_PEDESTRIAN, false, false, 1000, boost::posix_time::minutes(23), 1.11, 10, pc
	);

	ptime start_time(day_clock::local_day(), minutes(0));
	ptime end_time(day_clock::local_day(), hours(24));

	PTTimeSlotRoutePlanner r(
		testAreaMap.getStopAreas()[0].get(),
		testAreaMap.getStopAreas()[NB_STOP-1].get(),
		start_time,
		end_time,
		start_time,
		end_time,
		boost::optional<std::size_t>(),
		ap,
		DEPARTURE_FIRST,
		false,
		logger
	);
	PTRoutePlannerResult result(r.run());
	resultDump(result);
	BOOST_REQUIRE_EQUAL(result.getJourneys().size(), 20);
	size_t i = 0;
	checkJourneyEquals(i++, result.getJourneys(), "SCHED1", start_time + time_duration(0, 0, 0), time_duration(0, 0, 0));
	checkJourneyEquals(i++, result.getJourneys(), "SCHED1", start_time + time_duration(1, 0, 0), time_duration(0, 0, 0));
	checkJourneyEquals(i++, result.getJourneys(), "SCHED1", start_time + time_duration(2, 0, 0), time_duration(0, 0, 0));
	checkJourneyEquals(i++, result.getJourneys(), "CONTI1", start_time + time_duration(2,30, 0), time_duration(0,20, 0));
	checkJourneyEquals(i++, result.getJourneys(), "SCHED1", start_time + time_duration(3, 0, 0), time_duration(0, 0, 0));
	checkJourneyEquals(i++, result.getJourneys(), "CONTI1", start_time + time_duration(3,10, 0), time_duration(0,20, 0));
	checkJourneyEquals(i++, result.getJourneys(), "SCHED1", start_time + time_duration(4, 0, 0), time_duration(0, 0, 0));
	checkJourneyEquals(i++, result.getJourneys(), "CONTI1", start_time + time_duration(4,30, 0), time_duration(0,20, 0));
	checkJourneyEquals(i++, result.getJourneys(), "SCHED1", start_time + time_duration(5, 0, 0), time_duration(0, 0, 0));
	checkJourneyEquals(i++, result.getJourneys(), "CONTI1", start_time + time_duration(5,10, 0), time_duration(0,40, 0));
	checkJourneyEquals(i++, result.getJourneys(), "SCHED1", start_time + time_duration(6, 0, 0), time_duration(0, 0, 0));
	checkJourneyEquals(i++, result.getJourneys(), "CONTI1", start_time + time_duration(6,10, 0), time_duration(0,20, 0));
	checkJourneyEquals(i++, result.getJourneys(), "SCHED1", start_time + time_duration(7, 0, 0), time_duration(0, 0, 0));
	checkJourneyEquals(i++, result.getJourneys(), "CONTI1", start_time + time_duration(7,30, 0), time_duration(0,20, 0));
	checkJourneyEquals(i++, result.getJourneys(), "SCHED1", start_time + time_duration(8, 0, 0), time_duration(0, 0, 0));
	checkJourneyEquals(i++, result.getJourneys(), "SCHED1", start_time + time_duration(8,15, 0), time_duration(0, 0, 0));
	checkJourneyEquals(i++, result.getJourneys(), "CONTI1", start_time + time_duration(8,25, 0), time_duration(0,25, 0));
	checkJourneyEquals(i++, result.getJourneys(), "SCHED1", start_time + time_duration(9, 0, 0), time_duration(0, 0, 0));
	checkJourneyEquals(i++, result.getJourneys(), "CONTI1", start_time + time_duration(9,10, 0), time_duration(0,20, 0));
	checkJourneyEquals(i++, result.getJourneys(), "SCHED1", start_time + time_duration(10, 0, 0), time_duration(0, 0, 0));

}

/**
 * We test that scheduled services with a non concurrency rule are
 * properly accepted or excluled against continuous services.
 */
BOOST_AUTO_TEST_CASE (continuousVSscheduled)
{
	ScopedRegistrable<CommercialLine> scopedCommercialLine;

	TestAreaMap testAreaMap(NB_STOP);
	
	JourneyPattern jp2;
	CommercialLine cl2;
	cl2.addPath(&jp2);
	cl2.setShortName("CONTI1");
	jp2.setCommercialLine(&cl2);
	TestJourney tj2(jp2, testAreaMap);

	JourneyPattern jp1;
	CommercialLine cl1;
	cl2.addPath(&jp1);
	cl1.setShortName("SCHED1");
	jp1.setCommercialLine(&cl1);
	TestJourney tj1(jp1, testAreaMap);
	// Set a non concurrency rule
	NonConcurrencyRule ncr1(123456);
	ncr1.set<Delay>(time_duration(0, 10, 0));
	ncr1.set<PriorityLine>(cl2);
	CommercialLine::NonConcurrencyRules ncrs1;
	ncrs1.insert(&ncr1);
	cl1.setNonConcurrencyRules(ncrs1);
	//

	// The schedules of the priority line SCHED1
	vector<boost::shared_ptr<TestScheduledService> > tssVector;
	for(time_duration t(0,0,0); t < hours(11); t+=hours(1))
	{
		tssVector.push_back(boost::shared_ptr<TestScheduledService>(new TestScheduledService(0, "", jp1, t)));
	}
	tssVector.push_back(boost::shared_ptr<TestScheduledService>(new TestScheduledService(0, "", jp1, time_duration(8, 15, 0))));
	// The schedules of the priority continuous line CONTI1
	TestContinuousService tcs2(0, "CONT1", jp2, time_duration(2,30,0), time_duration(1,0,0));
	TestContinuousService tc32(1, "CONT2", jp2, time_duration(4,30,0), time_duration(2,0,0));
	TestContinuousService tc33(2, "CONT3", jp2, time_duration(7,30,0), time_duration(2,0,0));


	AlgorithmLogger logger;
	AccessParameters::AllowedPathClasses pc;
	AccessParameters ap(
		USER_PEDESTRIAN, false, false, 1000, boost::posix_time::minutes(23), 1.11, 10, pc
	);

	ptime start_time(day_clock::local_day(), minutes(0));
	ptime end_time(day_clock::local_day(), hours(24));

	PTTimeSlotRoutePlanner r(
		testAreaMap.getStopAreas()[0].get(),
		testAreaMap.getStopAreas()[NB_STOP-1].get(),
		start_time,
		end_time,
		start_time,
		end_time,
		boost::optional<std::size_t>(),
		ap,
		DEPARTURE_FIRST,
		false,
		logger
	);
	PTRoutePlannerResult result(r.run());
	resultDump(result);
	BOOST_REQUIRE_EQUAL(result.getJourneys().size(), 9);
	size_t i = 0;
	checkJourneyEquals(i++, result.getJourneys(), "SCHED1", start_time + time_duration(0, 0, 0), time_duration(0, 0, 0));
	checkJourneyEquals(i++, result.getJourneys(), "SCHED1", start_time + time_duration(1, 0, 0), time_duration(0, 0, 0));
	checkJourneyEquals(i++, result.getJourneys(), "SCHED1", start_time + time_duration(2, 0, 0), time_duration(0, 0, 0));
	checkJourneyEquals(i++, result.getJourneys(), "CONTI1", start_time + time_duration(2,30, 0), time_duration(1, 0, 0));
	checkJourneyEquals(i++, result.getJourneys(), "SCHED1", start_time + time_duration(4, 0, 0), time_duration(0, 0, 0));
	checkJourneyEquals(i++, result.getJourneys(), "CONTI1", start_time + time_duration(4,30, 0), time_duration(2, 0, 0));
	checkJourneyEquals(i++, result.getJourneys(), "SCHED1", start_time + time_duration(7, 0, 0), time_duration(0, 0, 0));
	checkJourneyEquals(i++, result.getJourneys(), "CONTI1", start_time + time_duration(7,30, 0), time_duration(2, 0, 0));
	checkJourneyEquals(i++, result.getJourneys(), "SCHED1", start_time + time_duration(10, 0, 0), time_duration(0, 0, 0));

}
//#endif


/**
 * We test that continuous services with a non concurrency rule are
 * properly accepted, excluled or shortened against continuous services.
 */
BOOST_AUTO_TEST_CASE (continuousVScontinuous)
{
	ScopedRegistrable<CommercialLine> scopedCommercialLine;

	TestAreaMap testAreaMap(NB_STOP);
	
	JourneyPattern jp1;
	CommercialLine cl1;
	cl1.addPath(&jp1);
	cl1.setShortName("CONTI1");
	jp1.setCommercialLine(&cl1);
	TestJourney tj1(jp1, testAreaMap);

	JourneyPattern jp2;
	CommercialLine cl2;
	cl2.addPath(&jp2);
	cl2.setShortName("CONTI2");
	jp2.setCommercialLine(&cl2);
	TestJourney tj2(jp2, testAreaMap);
	// Set a non concurrency rule
	NonConcurrencyRule ncr2(123456);
	ncr2.set<Delay>(time_duration(0, 10, 0));
	ncr2.set<PriorityLine>(cl1);
	CommercialLine::NonConcurrencyRules ncrs2;
	ncrs2.insert(&ncr2);
	cl2.setNonConcurrencyRules(ncrs2);
	//

	// The schedules of the priority continuous line CONTI1
	TestContinuousService tcs1(0, "CONTI1/1", jp1, time_duration(0, 0,0), time_duration(2,0,0));
	TestContinuousService tcs2(1, "CONIT1/2", jp1, time_duration(3, 0,0), time_duration(1,0,0));
	// The schedules of the non priority continuous line CONTI2
	TestContinuousService tcs5(2, "CONTI2/1", jp2, time_duration(0, 0,0), time_duration(0,30,0));
	TestContinuousService tcs6(3, "CONTI2/2", jp2, time_duration(1, 0,0), time_duration(4, 0,0));


	AlgorithmLogger logger;
	AccessParameters::AllowedPathClasses pc;
	AccessParameters ap(
		USER_PEDESTRIAN, false, false, 1000, boost::posix_time::minutes(23), 1.11, 10, pc
	);

	ptime start_time(day_clock::local_day(), minutes(0));
	ptime end_time(day_clock::local_day(), hours(24));

	PTTimeSlotRoutePlanner r(
		testAreaMap.getStopAreas()[0].get(),
		testAreaMap.getStopAreas()[NB_STOP-1].get(),
		start_time,
		end_time,
		start_time,
		end_time,
		boost::optional<std::size_t>(),
		ap,
		DEPARTURE_FIRST,
		false,
		logger
	);
	PTRoutePlannerResult result(r.run());
	resultDump(result);
	BOOST_REQUIRE_EQUAL(result.getJourneys().size(), 4);
	size_t i = 0;
	checkJourneyEquals(i++, result.getJourneys(), "CONTI1", start_time + time_duration(0, 0, 0), time_duration(2, 0, 0));
	checkJourneyEquals(i++, result.getJourneys(), "CONTI2", start_time + time_duration(2,10, 0), time_duration(0,40, 0));
	checkJourneyEquals(i++, result.getJourneys(), "CONTI1", start_time + time_duration(3, 0, 0), time_duration(1, 0, 0));
	checkJourneyEquals(i++, result.getJourneys(), "CONTI2", start_time + time_duration(4,10, 0), time_duration(0,50, 0));

}
