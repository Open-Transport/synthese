
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

#include "PTModuleRegister.cpp"
#include "JourneyPattern.hpp"
#include "DesignatedLinePhysicalStop.hpp"
#include "Path.h"
#include "Hub.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "GeographyModule.h"
#include "DRTArea.hpp"
#include "LineArea.hpp"
#include "AreaGeneratedLineStop.hpp"
#include "CommercialLine.h"
#include "ScheduledService.h"
#include "ContinuousService.h"
#include "DynamicRequest.h"
#include "HTTPRequest.hpp"

#include "UtilConstants.h"

#include <iostream>

#include <boost/test/auto_unit_test.hpp>

using namespace synthese::util;
using namespace synthese::pt;
using namespace synthese::geography;
using namespace synthese::graph;
using namespace synthese::server;
using namespace synthese;

using namespace std;
using namespace boost::posix_time;
using namespace boost::gregorian;

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

			_scheduledService->setSchedules(d, a, false /* non interpolated */);
		}
		jp.addService(*_scheduledService, true);
		_scheduledService->setActive(day_clock::local_day());
		Env::GetOfficialEnv().getEditableRegistry<ScheduledService>().add(_scheduledService);
	}

	void checkRealArrivalSchedule(size_t stopIndex, time_duration startTime)
	{
		for(size_t i = stopIndex ; i < _scheduledService->getArrivalSchedules(true).size() ; ++i)
		{
			BOOST_CHECK(_scheduledService->getArrivalSchedules(true)[i] == startTime);
			startTime += time_duration(0,5,0);
		}
	}

	void checkRealDepartureSchedule(size_t stopIndex, time_duration startTime)
	{
		for(size_t i = stopIndex ; i < _scheduledService->getDepartureSchedules(true).size() ; ++i)
		{
			BOOST_CHECK(_scheduledService->getDepartureSchedules(true)[i] == startTime);
			startTime += time_duration(0,5,0);
		}
	}

	void dump()
	{
		cout << "Service " << _scheduledService->getServiceNumber() << endl;
		cout << "  Normal Arrival Time" << endl << "  ";
		BOOST_FOREACH(time_duration td, _scheduledService->getArrivalSchedules(false))
		{
			cout << td << " ";
		}
		cout << endl;
		cout << "  Normal Departure Time" << endl << "  ";
		BOOST_FOREACH(time_duration td, _scheduledService->getDepartureSchedules(false))
		{
			cout << td << " ";
		}
		cout << endl;
		cout << "  Real Arrival Time" << endl << "  ";
		BOOST_FOREACH(time_duration td, _scheduledService->getArrivalSchedules(true))
		{
			cout << td << " ";
		}
		cout << endl;
		cout << "  Real Departure Time" << endl << "  ";
		BOOST_FOREACH(time_duration td, _scheduledService->getDepartureSchedules(true))
		{
			cout << td << " ";
		}
		cout << endl;
	}
};

BOOST_AUTO_TEST_CASE (test1)
{

	synthese::pt::moduleRegister();

	JourneyPattern jp;
	CommercialLine cl;
	jp.setCommercialLine(&cl);

	StopArea p1(0, true);
	StopArea p2(0, false);
	StopArea p3(0, false);
	StopArea p4(0, false);
	StopArea p5(0, true);
	StopArea p6(0, true);
	StopArea p7(0, false);
	StopArea p8(0, false);

	StopPoint s1(0, "s1", &p1);
	StopPoint s2(0, "s1", &p2);
	StopPoint s3(0, "s1", &p3);
	StopPoint s4(0, "s1", &p4);
	StopPoint s5(0, "s1", &p5);
	StopPoint s6(0, "s1", &p6);
	StopPoint s7(0, "s1", &p7);
	StopPoint s8(0, "s1", &p8);

	p1.addPhysicalStop(s1);
	p2.addPhysicalStop(s2);
	p3.addPhysicalStop(s3);
	p4.addPhysicalStop(s4);
	p5.addPhysicalStop(s5);
	p6.addPhysicalStop(s6);
	p7.addPhysicalStop(s7);
	p8.addPhysicalStop(s8);

	DesignatedLinePhysicalStop l1AD(0, &jp, 0, true, true, 0, &s1);
	DesignatedLinePhysicalStop l2AD(0, &jp, 1, true, true, 0, &s2);
	DesignatedLinePhysicalStop l3AD(0, &jp, 2, true, true, 0, &s3);
	DesignatedLinePhysicalStop l4AD(0, &jp, 3, true, true, 0, &s4);
	DesignatedLinePhysicalStop l5AD(0, &jp, 4, true, true, 0, &s5);
	DesignatedLinePhysicalStop l6AD(0, &jp, 5, true, true, 0, &s6);
	DesignatedLinePhysicalStop l7AD(0, &jp, 6, true, true, 0, &s7);
	DesignatedLinePhysicalStop l8AD(0, &jp, 7, true, true, 0, &s8);

	s1.addDepartureEdge(&l1AD);
	s2.addDepartureEdge(&l2AD);
	s3.addDepartureEdge(&l3AD);
	s4.addDepartureEdge(&l4AD);
	s5.addDepartureEdge(&l5AD);
	s6.addDepartureEdge(&l6AD);
	s7.addDepartureEdge(&l7AD);
	s8.addDepartureEdge(&l8AD);

	s1.addArrivalEdge(&l1AD);
	s2.addArrivalEdge(&l2AD);
	s3.addArrivalEdge(&l3AD);
	s4.addArrivalEdge(&l4AD);
	s5.addArrivalEdge(&l5AD);
	s6.addArrivalEdge(&l6AD);
	s7.addArrivalEdge(&l7AD);
	s8.addArrivalEdge(&l8AD);

	jp.addEdge(l1AD);
	jp.addEdge(l2AD);
	jp.addEdge(l3AD);
	jp.addEdge(l4AD);
	jp.addEdge(l5AD);
	jp.addEdge(l6AD);
	jp.addEdge(l7AD);
	jp.addEdge(l8AD);

	TestScheduledService tss1(4503599627370501ULL, "1", jp, time_duration(1,0,0));
	TestScheduledService tss2(4503599627370502ULL, "2", jp, time_duration(2,0,0));
	TestScheduledService tss3(4503599627370503ULL, "3", jp, time_duration(3,0,0));
	TestScheduledService tss4(4503599627370504ULL, "4", jp, time_duration(4,0,0));
	TestScheduledService tss5(4503599627370505ULL, "5", jp, time_duration(5,0,0));

	{
		HTTPRequest req;
		req.headers.insert(make_pair("Host", "www.toto.com"));
		req.uri = "?a=ScheduleRealTime2UpdateAction&nr=1&actionParamse=4503599627370501&actionParamls=3&actionParamat=03:20:00";
		req.ipaddr = "127.0.0.1";
		DynamicRequest dr(req);

		stringstream s;
		dr.run(s);
		// TODO Check the result

	}

	tss1.dump();
	tss2.dump();
	tss3.dump();
	tss4.dump();
	tss5.dump();

	tss1.checkRealArrivalSchedule  (3, time_duration(3,20,0));
	// FIXME The first departure time matching the adjusted ArrivalTime is
	//       not adjusted. Here we should check stop 3 for 3,21
	tss1.checkRealDepartureSchedule(4, time_duration(3,26,0));

	tss2.checkRealArrivalSchedule  (3, time_duration(3,20,0));
	tss2.checkRealDepartureSchedule(4, time_duration(3,26,0));

	tss3.checkRealArrivalSchedule  (3, time_duration(3,20,0));
	tss3.checkRealDepartureSchedule(4, time_duration(3,26,0));

	tss4.checkRealArrivalSchedule  (1, time_duration(4,04,0));
	tss4.checkRealDepartureSchedule(0, time_duration(4,00,0));

	tss5.checkRealArrivalSchedule  (1, time_duration(5,04,0));
	tss5.checkRealDepartureSchedule(0, time_duration(5,00,0));

	cout << "END" << endl;	
}


