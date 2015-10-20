
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

#include "Env.h"
#include "PTModuleRegister.cpp"
#include "JourneyPattern.hpp"
#include "LineStop.h"
#include "Path.h"
#include "Hub.h"
#include "ImpExModuleRegister.cpp"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "GeographyModule.h"
#include "AreaGeneratedLineStop.hpp"
#include "CommercialLine.h"
#include "ScheduledService.h"
#include "ContinuousService.h"
#include "DynamicRequest.h"
#include "HTTPRequest.hpp"
#include "InterSYNTHESEModuleRegister.cpp"

#include "UtilConstants.h"

#include <iostream>

#include <boost/test/auto_unit_test.hpp>

#define TEST_SERVICE_DATASOURCE 16607027920896001
#define TEST_STOP_AREA_ID 1970329131942220
#define TEST_STOP_POINT_ID 3377704015495171
#define TEST_LINE_STOP_ID 2814754062076747

using namespace synthese::util;
using namespace synthese::pt;
using namespace synthese::geography;
using namespace synthese::graph;
using namespace synthese::impex;
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
		boost::shared_ptr<DataSource> &ds,
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
		_scheduledService->addCodeBySource(*ds, serviceNumber, true);
		Env::GetOfficialEnv().getEditableRegistry<ScheduledService>().add(_scheduledService);
	}

	void checkRealArrivalSchedule(size_t stopIndex, time_duration startTime)
	{
		for(size_t i = stopIndex ; i < _scheduledService->getArrivalSchedules(true, true).size() ; ++i)
		{
			BOOST_CHECK(_scheduledService->getArrivalSchedules(true, true)[i] == startTime);
			startTime += time_duration(0,5,0);
		}
	}

	void checkRealDepartureSchedule(size_t stopIndex, time_duration startTime)
	{
		for(size_t i = stopIndex ; i < _scheduledService->getDepartureSchedules(true, true).size() - 1 ; ++i)
		{
			BOOST_CHECK(_scheduledService->getDepartureSchedules(true, true)[i] == startTime);
			startTime += time_duration(0,5,0);
		}
	}

	void checkSingleRealArrival(size_t stopIndex, time_duration startTime)
	{
		BOOST_CHECK(_scheduledService->getArrivalSchedules(true, true)[stopIndex] == startTime);
	}

	void checkSingleRealDeparture(size_t stopIndex, time_duration startTime)
	{
		BOOST_CHECK(_scheduledService->getDepartureSchedules(true, true)[stopIndex] == startTime);
	}

	void dump()
	{
		cout << "Service " << _scheduledService->getServiceNumber() << endl;
		cout << "  Normal Arrival Time" << endl << "  ";
		BOOST_FOREACH(time_duration td, _scheduledService->getArrivalSchedules(true, false))
		{
			cout << td << " ";
		}
		cout << endl;
		cout << "  Normal Departure Time" << endl << "  ";
		BOOST_FOREACH(time_duration td, _scheduledService->getDepartureSchedules(true, false))
		{
			cout << td << " ";
		}
		cout << endl;
		cout << "  Real Arrival Time" << endl << "  ";
		BOOST_FOREACH(time_duration td, _scheduledService->getArrivalSchedules(true, true))
		{
			cout << td << " ";
		}
		cout << endl;
		cout << "  Real Departure Time" << endl << "  ";
		BOOST_FOREACH(time_duration td, _scheduledService->getDepartureSchedules(true, true))
		{
			cout << td << " ";
		}
		cout << endl;
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
			boost::shared_ptr<StopArea> stopArea(new StopArea(TEST_STOP_AREA_ID + i, true));
			_stopAreas.push_back(stopArea);
			Env::GetOfficialEnv().getEditableRegistry<StopArea>().add(stopArea);
			_stopPoints.push_back(boost::shared_ptr<StopPoint>(new StopPoint(TEST_STOP_POINT_ID + i, "", &*_stopAreas[i])));
			Env::GetOfficialEnv().getEditableRegistry<StopPoint>().add(_stopPoints[i]);
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
};

class TestJourney
{
private:
	Env _env;
	vector<boost::shared_ptr<LineStop> > _designatedLinePhysicalStops;
	vector<boost::shared_ptr<StopPoint> > _stopPoints;
	size_t _numberOfStops;
public:
	TestJourney(JourneyPattern &jp, TestAreaMap &testAreaMap, int numjp):
		_stopPoints(testAreaMap.getStopPoints()),
		_numberOfStops(testAreaMap.getNumberOfStops())
	{
		for(size_t i=0; i< _numberOfStops; ++i)
		{
			_designatedLinePhysicalStops.push_back(
				boost::shared_ptr<LineStop>(
					new LineStop(
						TEST_LINE_STOP_ID + i + numjp,
						&jp,
						i,
						(i < _numberOfStops - 1 ? true : false), // Departure
						(i > 0 ? true : false),  // Arrival
						0,
						*_stopPoints[i]
				)	)
			);
			Env::GetOfficialEnv().getEditableRegistry<LineStop>().add(_designatedLinePhysicalStops[i]);
			_designatedLinePhysicalStops[i]->link(Env::GetOfficialEnv());
		}
	}

};

void runScheduleRealTimeUpdateService(string sa, string ds, string se1, string at1, string dt1)
{
	HTTPRequest req;
	req.headers.insert(make_pair("Host", "www.toto.com"));
	req.uri = string("?SERVICE=ScheduleRealTimeUpdateService&nr=1") +
		"&sa=" + sa + "&ds=" + ds + "&se1=" + se1 + "&at1=" + at1 + "&dt1=" + dt1;
	req.ipaddr = "127.0.0.1";
	DynamicRequest dr(req);

	stringstream s;
	dr.run(s);
}

BOOST_AUTO_TEST_CASE (test1)
{

	synthese::pt::moduleRegister();
	synthese::impex::moduleRegister();
	synthese::inter_synthese::moduleRegister();

	boost::shared_ptr<DataSource> ds(new DataSource(TEST_SERVICE_DATASOURCE));
	Env::GetOfficialEnv().getEditableRegistry<DataSource>().add(ds);

	TestAreaMap testAreaMap(8);
	
	JourneyPattern* jp = new JourneyPattern(2533279085363335);
	CommercialLine* cl = new CommercialLine(11821953316814849);
	jp->setCommercialLine(cl);
	Env::GetOfficialEnv().getEditableRegistry<JourneyPattern>().add(boost::shared_ptr<JourneyPattern>(jp));
	TestJourney tj1(*jp, testAreaMap, 0);

	JourneyPattern* jp2 = new JourneyPattern(2533279085363336);
	CommercialLine* cl2 = new CommercialLine(11821953316814850);
	jp2->setCommercialLine(cl2);
	Env::GetOfficialEnv().getEditableRegistry<JourneyPattern>().add(boost::shared_ptr<JourneyPattern>(jp2));
	TestJourney tj2(*jp2, testAreaMap, 10);

	TestScheduledService tss1(4503599627370501ULL, "1", *jp, ds,  time_duration(1,0,0));
	TestScheduledService tss2(4503599627370502ULL, "2", *jp2, ds, time_duration(2,0,0));
	TestScheduledService tss3(4503599627370503ULL, "3", *jp, ds,  time_duration(3,0,0));
	TestScheduledService tss4(4503599627370504ULL, "4", *jp2, ds, time_duration(4,0,0));
	TestScheduledService tss5(4503599627370505ULL, "5", *jp, ds,  time_duration(5,0,0));

	runScheduleRealTimeUpdateService("1970329131942223", "16607027920896001", "1", "03:20:00", "03:20:01");

	tss1.dump();
	tss2.dump();
	tss3.dump();
	tss4.dump();
	tss5.dump();

	tss1.checkSingleRealArrival    (3, time_duration(3,20,0));
	tss1.checkRealArrivalSchedule  (4, time_duration(3,24,1));
	tss1.checkRealDepartureSchedule(3, time_duration(3,20,1));

	tss2.checkSingleRealArrival    (3, time_duration(3,20,0));
	tss2.checkRealArrivalSchedule  (4, time_duration(3,24,1));
	tss2.checkRealDepartureSchedule(3, time_duration(3,20,1));

	tss3.checkSingleRealArrival    (3, time_duration(3,20,0));
	tss3.checkRealArrivalSchedule  (4, time_duration(3,24,1));
	tss3.checkRealDepartureSchedule(3, time_duration(3,20,1));

	// These services are too late to be inpacted
	tss4.checkRealArrivalSchedule  (1, time_duration(4,04,0));
	tss4.checkRealDepartureSchedule(0, time_duration(4,00,0));

	tss5.checkRealArrivalSchedule  (1, time_duration(5,04,0));
	tss5.checkRealDepartureSchedule(0, time_duration(5,00,0));

	/// -------
	cout << endl << "Test that the algo keeps the time stamps of requests" << endl;
	runScheduleRealTimeUpdateService("1970329131942222", "16607027920896001", "1", "03:19:00", "03:19:01");

	tss1.dump();
	tss2.dump();
	tss3.dump();
	tss4.dump();
	tss5.dump();

	// This update was done on the given stop
	tss1.checkSingleRealArrival    (2, time_duration(3,19,0));
	tss1.checkSingleRealDeparture  (2, time_duration(3,19,1));
	// The update did not continued because of the timestamp effect
	tss1.checkSingleRealArrival    (3, time_duration(3,20,0));
	tss1.checkRealArrivalSchedule  (4, time_duration(3,24,1));
	tss1.checkRealDepartureSchedule(3, time_duration(3,20,1));

	// This update was done on the given stop
	tss2.checkSingleRealArrival    (2, time_duration(3,19,0));
	tss2.checkSingleRealDeparture  (2, time_duration(3,19,1));
	// Here the update took place
	tss2.checkRealArrivalSchedule  (3, time_duration(3,23,1));
	tss2.checkRealDepartureSchedule(3, time_duration(3,24,1));

	// This update was done on the given stop
	tss3.checkSingleRealArrival    (2, time_duration(3,19,0));
	tss3.checkSingleRealDeparture  (2, time_duration(3,19,1));
	// Here the update took place
	tss3.checkRealArrivalSchedule  (3, time_duration(3,23,1));
	tss3.checkRealDepartureSchedule(3, time_duration(3,24,1));

	// These services are too late to be inpacted
	tss4.checkRealArrivalSchedule  (1, time_duration(4,04,0));
	tss4.checkRealDepartureSchedule(0, time_duration(4,00,0));

	tss5.checkRealArrivalSchedule  (1, time_duration(5,04,0));
	tss5.checkRealDepartureSchedule(0, time_duration(5,00,0));

	Env::GetOfficialEnv().clear();

	cout << "END" << endl;	
}


