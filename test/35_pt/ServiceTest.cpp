/** Service unit test.
	@file ServiceTest.cpp

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

#include "JourneyPattern.hpp"
#include "ScheduledService.h"
#include "GeographyModule.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "DesignatedLinePhysicalStop.hpp"

#include <iostream>

#include <boost/test/auto_unit_test.hpp>

using namespace synthese::util;
using namespace synthese::pt;
using namespace synthese::geography;
using namespace synthese;
using namespace boost::posix_time;

BOOST_AUTO_TEST_CASE (testStopsWithoutSchedules)
{
	GeographyModule::PreInit();

	JourneyPattern l(5678);

	StopArea p1(UNKNOWN_VALUE, true);
	StopArea p2(UNKNOWN_VALUE, false);
	StopArea p3(UNKNOWN_VALUE, false);
	StopArea p4(UNKNOWN_VALUE, false);
	StopArea p5(UNKNOWN_VALUE, true);
	StopArea p6(UNKNOWN_VALUE, true);
	StopArea p7(UNKNOWN_VALUE, false);
	StopArea p8(UNKNOWN_VALUE, false);

	StopPoint s1(UNKNOWN_VALUE, "s1", &p1);
	StopPoint s2(UNKNOWN_VALUE, "s1", &p2);
	StopPoint s3(UNKNOWN_VALUE, "s1", &p3);
	StopPoint s4(UNKNOWN_VALUE, "s1", &p4);
	StopPoint s5(UNKNOWN_VALUE, "s1", &p5);
	StopPoint s6(UNKNOWN_VALUE, "s1", &p6);
	StopPoint s7(UNKNOWN_VALUE, "s1", &p7);
	StopPoint s8(UNKNOWN_VALUE, "s1", &p8);

	DesignatedLinePhysicalStop l1D(0, &l, 0, true, false,0,&s1, true);
	DesignatedLinePhysicalStop l2D(0, &l, 1, true, false,50,&s2, false);
	DesignatedLinePhysicalStop l3AD(0, &l, 2, true, true,160,&s3, false);
	DesignatedLinePhysicalStop l4A(0, &l, 3, false, true,200,&s4, true);
	DesignatedLinePhysicalStop l5D(0, &l, 4, true, false,250,&s5, false);
	DesignatedLinePhysicalStop l6AD(0, &l, 5, true, true,450,&s6, false);
	DesignatedLinePhysicalStop l7AD(0, &l, 6, true, true,500,&s7, true);
	DesignatedLinePhysicalStop l8A(0, &l, 7, false, true,600,&s8, true);
	DesignatedLinePhysicalStop* lNULL(NULL);

	l.addEdge(l1D);
	l.addEdge(l2D);
	l.addEdge(l3AD);
	l.addEdge(l4A);
	l.addEdge(l5D);
	l.addEdge(l6AD);
	l.addEdge(l7AD);
	l.addEdge(l8A);

	ScheduledService s(1234, "1234AB", &l);

	BOOST_CHECK_EQUAL(s.getKey(), 1234);
	BOOST_CHECK_EQUAL(s.getServiceNumber(), "1234AB");
	BOOST_CHECK_EQUAL(s.getPath(), &l);
	
	SchedulesBasedService::Schedules d;
	SchedulesBasedService::Schedules a;

	a.push_back(time_duration(2, 0, 0));
	d.push_back(time_duration(2, 0, 0));
	
	a.push_back(time_duration(2, 24, 0));
	d.push_back(time_duration(2, 30, 0));
	
	a.push_back(time_duration(2, 31, 0));
	d.push_back(time_duration(2, 31, 0));
	
	a.push_back(time_duration(3, 10, 0));
	d.push_back(time_duration(3, 10, 0));
	
	s.setSchedules(d, a);

	SchedulesBasedService::Schedules id(s.getDepartureSchedules(false));
	SchedulesBasedService::Schedules ia(s.getArrivalSchedules(false));
	BOOST_CHECK_EQUAL(id.size(), l.getEdges().size());
	BOOST_CHECK_EQUAL(ia[0], time_duration(2,0, 0)); // Scheduled
	BOOST_CHECK_EQUAL(id[0], time_duration(2,0, 0)); // Scheduled
	BOOST_CHECK_EQUAL(ia[1], time_duration(2,6, 0));
	BOOST_CHECK_EQUAL(id[1], time_duration(2,6, 0));
	BOOST_CHECK_EQUAL(ia[2], time_duration(2,20, 0));
	BOOST_CHECK_EQUAL(id[2], time_duration(2,19, 0));
	BOOST_CHECK_EQUAL(ia[3], time_duration(2,24, 0)); // Scheduled
	BOOST_CHECK_EQUAL(id[3], time_duration(2,30, 0)); // Scheduled
	BOOST_CHECK_EQUAL(ia[4], time_duration(2,31, 0));
	BOOST_CHECK_EQUAL(id[4], time_duration(2,30, 0));
	BOOST_CHECK_EQUAL(ia[5], time_duration(2,31, 0));
	BOOST_CHECK_EQUAL(id[5], time_duration(2,30, 0));
	BOOST_CHECK_EQUAL(ia[6], time_duration(2,31, 0)); // Scheduled
	BOOST_CHECK_EQUAL(id[6], time_duration(2,31, 0)); // Scheduled
	BOOST_CHECK_EQUAL(ia[7], time_duration(3,10, 0)); // Scheduled
	BOOST_CHECK_EQUAL(id[7], time_duration(3,10, 0)); // Scheduled

}






