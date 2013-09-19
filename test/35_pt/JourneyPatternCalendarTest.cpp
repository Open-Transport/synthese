
/** JourneyPatternCalendarTest class implementation.
	@file JourneyPatternCalendarTest.cpp

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

#include "JourneyPattern.hpp"
#include "DesignatedLinePhysicalStop.hpp"
#include "ScheduledService.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "GeographyModule.h"

#include <boost/test/auto_unit_test.hpp>
#include <boost/date_time/date.hpp>

using namespace synthese::pt;
using namespace synthese::geography;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace std;

BOOST_AUTO_TEST_CASE (testJourneyPatternCalendarScheduledService)
{
	GeographyModule::PreInit();

	{
		JourneyPattern jp;
		StopArea sa(0, true);
		StopPoint sp(0, "sp", &sa);
		DesignatedLinePhysicalStop ls1(0, &jp, 0, true, false,0,&sp);
		DesignatedLinePhysicalStop ls2(0, &jp, 1, false, true,0,&sp);
		jp.addEdge(ls1);
		jp.addEdge(ls2);

		ScheduledService s(0, string(), &jp);
		SchedulesBasedService::Schedules schedules;
		schedules.push_back(time_duration(8, 0, 0));
		schedules.push_back(time_duration(9, 0, 0));
		s.setDataSchedules(schedules, schedules);

		date d(day_clock::local_day());
		s.setActive(d);

		BOOST_CHECK(!jp.isActive(d - days(1)));
		BOOST_CHECK(jp.isActive(d));
		BOOST_CHECK(!jp.isActive(d + days(1)));

		s.updatePathCalendar();

		BOOST_CHECK(!jp.isActive(d - days(1)));
		BOOST_CHECK(jp.isActive(d));
		BOOST_CHECK(!jp.isActive(d + days(1)));
	}

	{
		JourneyPattern jp;
		StopArea sa(0, true);
		StopPoint sp(0, "sp", &sa);
		DesignatedLinePhysicalStop ls1(0, &jp, 0, true, false,0,&sp);
		DesignatedLinePhysicalStop ls2(0, &jp, 1, false, true,0,&sp);
		jp.addEdge(ls1);
		jp.addEdge(ls2);

		ScheduledService s(0, string(), &jp);
		SchedulesBasedService::Schedules schedules;
		schedules.push_back(time_duration(8, 0, 0));
		schedules.push_back(time_duration(25, 0, 0));
		s.setDataSchedules(schedules, schedules);

		date d(day_clock::local_day());
		s.setActive(d);

		BOOST_CHECK(!jp.isActive(d - days(1)));
		BOOST_CHECK(jp.isActive(d));
		BOOST_CHECK(jp.isActive(d + days(1)));
		BOOST_CHECK(!jp.isActive(d + days(2)));

		s.updatePathCalendar();

		BOOST_CHECK(!jp.isActive(d - days(1)));
		BOOST_CHECK(jp.isActive(d));
		BOOST_CHECK(jp.isActive(d + days(1)));
		BOOST_CHECK(!jp.isActive(d + days(2)));
	}

	{ // Bug 8155
		JourneyPattern jp;
		StopArea sa(0, true);
		StopPoint sp(0, "sp", &sa);
		DesignatedLinePhysicalStop ls1(0, &jp, 0, true, false,0,&sp);
		DesignatedLinePhysicalStop ls2(0, &jp, 1, false, true,0,&sp);
		jp.addEdge(ls1);
		jp.addEdge(ls2);

		ScheduledService s(0, string(), &jp);
		SchedulesBasedService::Schedules schedules;
		schedules.push_back(time_duration(26, 0, 0));
		schedules.push_back(time_duration(32, 0, 0));
		s.setDataSchedules(schedules, schedules);

		date d(day_clock::local_day());
		s.setActive(d);

		BOOST_CHECK(!jp.isActive(d - days(1)));
		BOOST_CHECK(!jp.isActive(d));
		BOOST_CHECK(jp.isActive(d + days(1)));
		BOOST_CHECK(!jp.isActive(d + days(2)));

		s.updatePathCalendar();

		BOOST_CHECK(!jp.isActive(d - days(1)));
		BOOST_CHECK(!jp.isActive(d));
		BOOST_CHECK(jp.isActive(d + days(1)));
		BOOST_CHECK(!jp.isActive(d + days(2)));
	}

	{
		JourneyPattern jp;
		StopArea sa(0, true);
		StopPoint sp(0, "sp", &sa);
		DesignatedLinePhysicalStop ls1(0, &jp, 0, true, false,0,&sp);
		DesignatedLinePhysicalStop ls2(0, &jp, 1, false, true,0,&sp);
		jp.addEdge(ls1);
		jp.addEdge(ls2);

		ScheduledService s(0, string(), &jp);
		SchedulesBasedService::Schedules schedules;
		schedules.push_back(time_duration(26, 0, 0));
		schedules.push_back(time_duration(49, 0, 0));
		s.setDataSchedules(schedules, schedules);

		date d(day_clock::local_day());
		s.setActive(d);

		BOOST_CHECK(!jp.isActive(d - days(1)));
		BOOST_CHECK(!jp.isActive(d));
		BOOST_CHECK(jp.isActive(d + days(1)));
		BOOST_CHECK(jp.isActive(d + days(2)));
		BOOST_CHECK(!jp.isActive(d + days(3)));

		s.updatePathCalendar();

		BOOST_CHECK(!jp.isActive(d - days(1)));
		BOOST_CHECK(!jp.isActive(d));
		BOOST_CHECK(jp.isActive(d + days(1)));
		BOOST_CHECK(jp.isActive(d + days(2)));
		BOOST_CHECK(!jp.isActive(d + days(3)));
	}
}
