/** Service unit test.
	@file ServiceTest.cpp

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
#include "AllowedUseRule.h"
#include "CommercialLine.h"
#include "ContinuousService.h"
#include "DRTArea.hpp"
#include "JourneyPatternCopy.hpp"
#include "LineArea.hpp"
#include "PermanentService.h"
#include "ScheduledService.h"
#include "GeographyModule.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "DesignatedLinePhysicalStop.hpp"
#include "PermanentService.h"
#include "ServicePointer.h"

#include <iostream>

#include <boost/test/auto_unit_test.hpp>

using namespace synthese::util;
using namespace synthese::pt;
using namespace synthese::geography;
using namespace synthese;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace synthese::graph;


BOOST_AUTO_TEST_CASE (testScheduledService)
{
	GeographyModule::PreInit();

	date today(day_clock::local_day());

	RuleUser::Rules r;
	r.push_back(AllowedUseRule::INSTANCE.get());
	r.push_back(AllowedUseRule::INSTANCE.get());
	r.push_back(AllowedUseRule::INSTANCE.get());
	JourneyPattern l(5678);
	l.setRules(r);

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

	DesignatedLinePhysicalStop l1D(0, &l, 0, true, false,0,&s1, true);
	DesignatedLinePhysicalStop l2D(0, &l, 1, true, false,50,&s2, false);
	DesignatedLinePhysicalStop l3AD(0, &l, 2, true, true,160,&s3, false);
	DesignatedLinePhysicalStop l4A(0, &l, 3, true, true,200,&s4, true);
	DesignatedLinePhysicalStop l5D(0, &l, 4, true, false,250,&s5, false);
	DesignatedLinePhysicalStop l6AD(0, &l, 5, true, true,450,&s6, false);
	DesignatedLinePhysicalStop l7AD(0, &l, 6, false, true,500,&s7, true);
	DesignatedLinePhysicalStop l71A(0, &l, 7, false, true,500,&s7, false);
	DesignatedLinePhysicalStop l8A(0, &l, 8, false, true,500,&s8, true);
	DesignatedLinePhysicalStop* lNULL(NULL);

	l.addEdge(l1D);
	l.addEdge(l2D);
	l.addEdge(l3AD);
	l.addEdge(l4A);
	l.addEdge(l5D);
	l.addEdge(l6AD);
	l.addEdge(l7AD);
	l.addEdge(l71A);
	l.addEdge(l8A);

	ScheduledService s(1234, "1234AB", &l);

	BOOST_CHECK_EQUAL(s.getKey(), 1234);
	BOOST_CHECK_EQUAL(s.getServiceNumber(), "1234AB");
	BOOST_CHECK_EQUAL(s.getPath(), &l);
	BOOST_CHECK_EQUAL(s.isContinuous(), false);

	SchedulesBasedService::Schedules d;
	SchedulesBasedService::Schedules a;

	a.push_back(time_duration(2, 0, 0));
	d.push_back(time_duration(2, 0, 0));

	a.push_back(time_duration(2, 24, 0));
	d.push_back(time_duration(2, 30, 0));

	a.push_back(time_duration(2, 34, 0));
	d.push_back(time_duration(not_a_date_time));

	a.push_back(time_duration(3, 10, 0));
	d.push_back(time_duration(3, 10, 0));

	s.setDataSchedules(d, a);
	s.setActive(today);

	SchedulesBasedService::Schedules id(s.getDepartureSchedules(true, false));
	SchedulesBasedService::Schedules ia(s.getArrivalSchedules(true, false));
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
	BOOST_CHECK_EQUAL(id[4], time_duration(2,36, 0));
	BOOST_CHECK_EQUAL(ia[5], time_duration(2,34, 0));
	BOOST_CHECK_EQUAL(id[5], time_duration(3,3, 0));
	BOOST_CHECK_EQUAL(ia[6], time_duration(2,34, 0)); // Scheduled
	BOOST_CHECK_EQUAL(id[6], time_duration(3,10, 0)); // Scheduled, but should be generated identically as arrival time
	BOOST_CHECK_EQUAL(ia[7], time_duration(2,52, 0));
	BOOST_CHECK_EQUAL(id[7], time_duration(3,10, 0));
	BOOST_CHECK_EQUAL(ia[8], time_duration(3,10, 0)); // Scheduled
	BOOST_CHECK_EQUAL(id[8], time_duration(3,10, 0)); // Scheduled


	// Service pointer
	AccessParameters ap;

	// From departure, before the departure time
	ptime time1(today, time_duration(1,50,0));
	ServicePointer sp1(
		s.getFromPresenceTime(
			ap,
			true,
			false,
			true,
			l3AD,
			time1,
			false,
			false,
			true,
			true
	)	);
	BOOST_CHECK_EQUAL(sp1.getDepartureEdge(), &l3AD);
	BOOST_CHECK(sp1.getArrivalEdge() == NULL);
	BOOST_CHECK_EQUAL(sp1.getRealTimeDepartureVertex(), l3AD.getFromVertex());
	BOOST_CHECK(sp1.getRealTimeArrivalVertex() == NULL);
	BOOST_CHECK_EQUAL(sp1.getService(), &s);
	BOOST_CHECK_EQUAL(sp1.getDepartureDateTime(), ptime(today, id[2]));
	BOOST_CHECK(sp1.getArrivalDateTime().is_not_a_date_time());
	BOOST_CHECK_EQUAL(sp1.getTheoreticalDepartureDateTime(), ptime(today, id[2]));
	BOOST_CHECK(sp1.getTheoreticalArrivalDateTime().is_not_a_date_time());
	BOOST_CHECK_EQUAL(sp1.getOriginDateTime(), ptime(today, id[0]));
	BOOST_CHECK_EQUAL(sp1.getServiceRange(), minutes(0));
	BOOST_CHECK(sp1.getDuration().is_not_a_date_time());

	s.completeServicePointer(sp1, l7AD, ap);
	BOOST_CHECK_EQUAL(sp1.getDepartureEdge(), &l3AD);
	BOOST_CHECK_EQUAL(sp1.getArrivalEdge(), &l7AD);
	BOOST_CHECK_EQUAL(sp1.getRealTimeDepartureVertex(), l3AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp1.getRealTimeArrivalVertex(), l7AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp1.getService(), &s);
	BOOST_CHECK_EQUAL(sp1.getDepartureDateTime(), ptime(today, id[2]));
	BOOST_CHECK_EQUAL(sp1.getArrivalDateTime(), ptime(today, ia[6]));
	BOOST_CHECK_EQUAL(sp1.getTheoreticalDepartureDateTime(), ptime(today, id[2]));
	BOOST_CHECK_EQUAL(sp1.getTheoreticalArrivalDateTime(), ptime(today, ia[6]));
	BOOST_CHECK_EQUAL(sp1.getOriginDateTime(), ptime(today, id[0]));
	BOOST_CHECK_EQUAL(sp1.getServiceRange(), minutes(0));
	BOOST_CHECK_EQUAL(sp1.getDuration(), ia[6] - id[2]);

	// From departure, at the departure time
	ptime time2(today, id[2]);
	ServicePointer sp2(
		s.getFromPresenceTime(
			ap,
			true,
			false,
			true,
			l3AD,
			time2,
			false,
			false,
			true,
			true
	)	);
	BOOST_CHECK_EQUAL(sp2.getDepartureEdge(), &l3AD);
	BOOST_CHECK(sp2.getArrivalEdge() == NULL);
	BOOST_CHECK_EQUAL(sp2.getRealTimeDepartureVertex(), l3AD.getFromVertex());
	BOOST_CHECK(sp2.getRealTimeArrivalVertex() == NULL);
	BOOST_CHECK_EQUAL(sp2.getService(), &s);
	BOOST_CHECK_EQUAL(sp2.getDepartureDateTime(), ptime(today, id[2]));
	BOOST_CHECK(sp2.getArrivalDateTime().is_not_a_date_time());
	BOOST_CHECK_EQUAL(sp2.getTheoreticalDepartureDateTime(), ptime(today, id[2]));
	BOOST_CHECK(sp2.getTheoreticalArrivalDateTime().is_not_a_date_time());
	BOOST_CHECK_EQUAL(sp2.getOriginDateTime(), ptime(today, id[0]));
	BOOST_CHECK_EQUAL(sp2.getServiceRange(), minutes(0));

	s.completeServicePointer(sp2, l7AD, ap);
	BOOST_CHECK_EQUAL(sp2.getDepartureEdge(), &l3AD);
	BOOST_CHECK_EQUAL(sp2.getArrivalEdge(), &l7AD);
	BOOST_CHECK_EQUAL(sp2.getRealTimeDepartureVertex(), l3AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp2.getRealTimeArrivalVertex(), l7AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp2.getService(), &s);
	BOOST_CHECK_EQUAL(sp2.getDepartureDateTime(), ptime(today, id[2]));
	BOOST_CHECK_EQUAL(sp2.getArrivalDateTime(), ptime(today, ia[6]));
	BOOST_CHECK_EQUAL(sp2.getTheoreticalDepartureDateTime(), ptime(today, id[2]));
	BOOST_CHECK_EQUAL(sp2.getTheoreticalArrivalDateTime(), ptime(today, ia[6]));
	BOOST_CHECK_EQUAL(sp2.getOriginDateTime(), ptime(today, id[0]));
	BOOST_CHECK_EQUAL(sp2.getServiceRange(), minutes(0));


	// From departure, after the departure time
	ptime time3(today, id[2] + minutes(1));
	ServicePointer sp3(
		s.getFromPresenceTime(
			ap,
			true,
			false,
			true,
			l3AD,
			time3,
			false,
			false,
			true,
			true
	)	);
	BOOST_CHECK(sp3.getService() == NULL);


	// From arrival, after the departure time
	ptime time4(today, time_duration(4,0,0));
	ServicePointer sp4(
		s.getFromPresenceTime(
			ap,
			true,
			false,
			false,
			l7AD,
			time4,
			false,
			false,
			true,
			true
	)	);
	BOOST_CHECK(sp4.getDepartureEdge() == NULL);
	BOOST_CHECK_EQUAL(sp4.getArrivalEdge(),&l7AD);
	BOOST_CHECK(sp4.getRealTimeDepartureVertex() == NULL);
	BOOST_CHECK_EQUAL(sp4.getRealTimeArrivalVertex(), l7AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp4.getService(), &s);
	BOOST_CHECK(sp4.getDepartureDateTime().is_not_a_date_time());
	BOOST_CHECK_EQUAL(sp4.getArrivalDateTime(), ptime(today, ia[6]));
	BOOST_CHECK(sp4.getTheoreticalDepartureDateTime().is_not_a_date_time());
	BOOST_CHECK_EQUAL(sp4.getTheoreticalArrivalDateTime(), ptime(today, ia[6]));
	BOOST_CHECK_EQUAL(sp4.getOriginDateTime(), ptime(today, id[0]));
	BOOST_CHECK_EQUAL(sp4.getServiceRange(), minutes(0));

	s.completeServicePointer(sp4, l3AD, ap);
	BOOST_CHECK_EQUAL(sp4.getDepartureEdge(), &l3AD);
	BOOST_CHECK_EQUAL(sp4.getArrivalEdge(), &l7AD);
	BOOST_CHECK_EQUAL(sp4.getRealTimeDepartureVertex(), l3AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp4.getRealTimeArrivalVertex(), l7AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp4.getService(), &s);
	BOOST_CHECK_EQUAL(sp4.getDepartureDateTime(), ptime(today, id[2]));
	BOOST_CHECK_EQUAL(sp4.getArrivalDateTime(), ptime(today, ia[6]));
	BOOST_CHECK_EQUAL(sp4.getTheoreticalDepartureDateTime(), ptime(today, id[2]));
	BOOST_CHECK_EQUAL(sp4.getTheoreticalArrivalDateTime(), ptime(today, ia[6]));
	BOOST_CHECK_EQUAL(sp4.getOriginDateTime(), ptime(today, id[0]));
	BOOST_CHECK_EQUAL(sp4.getServiceRange(), minutes(0));


	// From arrival, at the departure time
	ptime time5(today, ia[6]);
	ServicePointer sp5(
		s.getFromPresenceTime(
			ap,
			true,
			false,
			false,
			l7AD,
			time5,
			false,
			false,
			true,
			true
	)	);
	BOOST_CHECK(sp5.getDepartureEdge() == NULL);
	BOOST_CHECK_EQUAL(sp5.getArrivalEdge(),&l7AD);
	BOOST_CHECK(sp5.getRealTimeDepartureVertex() == NULL);
	BOOST_CHECK_EQUAL(sp5.getRealTimeArrivalVertex(), l7AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp5.getService(), &s);
	BOOST_CHECK(sp5.getDepartureDateTime().is_not_a_date_time());
	BOOST_CHECK_EQUAL(sp5.getArrivalDateTime(), ptime(today, ia[6]));
	BOOST_CHECK(sp5.getTheoreticalDepartureDateTime().is_not_a_date_time());
	BOOST_CHECK_EQUAL(sp5.getTheoreticalArrivalDateTime(), ptime(today, ia[6]));
	BOOST_CHECK_EQUAL(sp5.getOriginDateTime(), ptime(today, id[0]));
	BOOST_CHECK_EQUAL(sp5.getServiceRange(), minutes(0));

	s.completeServicePointer(sp5, l3AD, ap);
	BOOST_CHECK_EQUAL(sp5.getDepartureEdge(), &l3AD);
	BOOST_CHECK_EQUAL(sp5.getArrivalEdge(), &l7AD);
	BOOST_CHECK_EQUAL(sp5.getRealTimeDepartureVertex(), l3AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp5.getRealTimeArrivalVertex(), l7AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp5.getService(), &s);
	BOOST_CHECK_EQUAL(sp5.getDepartureDateTime(), ptime(today, id[2]));
	BOOST_CHECK_EQUAL(sp5.getArrivalDateTime(), ptime(today, ia[6]));
	BOOST_CHECK_EQUAL(sp5.getTheoreticalDepartureDateTime(), ptime(today, id[2]));
	BOOST_CHECK_EQUAL(sp5.getTheoreticalArrivalDateTime(), ptime(today, ia[6]));
	BOOST_CHECK_EQUAL(sp5.getOriginDateTime(), ptime(today, id[0]));
	BOOST_CHECK_EQUAL(sp5.getServiceRange(), minutes(0));



	// From arrival, before the departure time
	ptime time6(today, ia[6] - minutes(1));
	ServicePointer sp6(
		s.getFromPresenceTime(
			ap,
			true,
			false,
			false,
			l7AD,
			time6,
			false,
			false,
			true,
			true
	)	);
	BOOST_CHECK(sp6.getService() == NULL);
}

BOOST_AUTO_TEST_CASE (testScheduledServiceRealTime)
{
	GeographyModule::PreInit();

	date today(day_clock::local_day());

	RuleUser::Rules r;
	r.push_back(AllowedUseRule::INSTANCE.get());
	r.push_back(AllowedUseRule::INSTANCE.get());
	r.push_back(AllowedUseRule::INSTANCE.get());
	JourneyPattern l(5678);
	l.setRules(r);

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

	DesignatedLinePhysicalStop l1D(0, &l, 0, true, false,0,&s1, true);
	DesignatedLinePhysicalStop l2D(0, &l, 1, true, false,50,&s2, false);
	DesignatedLinePhysicalStop l3AD(0, &l, 2, true, true,160,&s3, false);
	DesignatedLinePhysicalStop l4A(0, &l, 3, true, true,200,&s4, true);
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
	BOOST_CHECK_EQUAL(s.isContinuous(), false);

	SchedulesBasedService::Schedules d;
	SchedulesBasedService::Schedules a;

	a.push_back(time_duration(2, 0, 0));
	d.push_back(time_duration(2, 0, 0));

	a.push_back(time_duration(2, 24, 0));
	d.push_back(time_duration(2, 28, 0));

	a.push_back(time_duration(2, 31, 0));
	d.push_back(time_duration(2, 31, 0));

	a.push_back(time_duration(3, 10, 0));
	d.push_back(time_duration(3, 10, 0));

	s.setDataSchedules(d, a);
	// Apply a RealTime shift
	s.applyRealTimeShiftDuration(0, time_duration(0, 10, 0), time_duration(0, 10, 0), true);
	s.setActive(today);

	SchedulesBasedService::Schedules id(s.getDepartureSchedules(true, true));
	SchedulesBasedService::Schedules ia(s.getArrivalSchedules(true, true));
	BOOST_CHECK_EQUAL(id.size(), l.getEdges().size());
	BOOST_CHECK_EQUAL(ia[0], time_duration(2,10, 0)); // Scheduled
	BOOST_CHECK_EQUAL(id[0], time_duration(2,10, 0)); // Scheduled
	BOOST_CHECK_EQUAL(ia[1], time_duration(2,16, 0));
	BOOST_CHECK_EQUAL(id[1], time_duration(2,16, 0));
	BOOST_CHECK_EQUAL(ia[2], time_duration(2,30, 0));
	BOOST_CHECK_EQUAL(id[2], time_duration(2,29, 0));
	BOOST_CHECK_EQUAL(ia[3], time_duration(2,34, 0)); // Scheduled
	BOOST_CHECK_EQUAL(id[3], time_duration(2,38, 0)); // Scheduled
	BOOST_CHECK_EQUAL(ia[4], time_duration(2,39, 0));
	BOOST_CHECK_EQUAL(id[4], time_duration(2,38, 0));
	BOOST_CHECK_EQUAL(ia[5], time_duration(2,41, 0));
	BOOST_CHECK_EQUAL(id[5], time_duration(2,40, 0));
	BOOST_CHECK_EQUAL(ia[6], time_duration(2,41, 0)); // Scheduled
	BOOST_CHECK_EQUAL(id[6], time_duration(2,41, 0)); // Scheduled
	BOOST_CHECK_EQUAL(ia[7], time_duration(3,20, 0)); // Scheduled
	BOOST_CHECK_EQUAL(id[7], time_duration(3,20, 0)); // Scheduled


	// Service pointer
	AccessParameters ap;

	// From departure, before the departure time
	ptime time1(today, time_duration(1,50,0));
	ServicePointer sp1(
		s.getFromPresenceTime(
			ap,
			true,
			true,
			true,
			l3AD,
			time1,
			false,
			false,
			true,
			true
	)	);
	BOOST_CHECK_EQUAL(sp1.getDepartureEdge(), &l3AD);
	BOOST_CHECK(sp1.getArrivalEdge() == NULL);
	BOOST_CHECK_EQUAL(sp1.getRealTimeDepartureVertex(), l3AD.getFromVertex());
	BOOST_CHECK(sp1.getRealTimeArrivalVertex() == NULL);
	BOOST_CHECK_EQUAL(sp1.getService(), &s);
	BOOST_CHECK_EQUAL(sp1.getDepartureDateTime(), ptime(today, time_duration(2,29,0)));
	BOOST_CHECK(sp1.getArrivalDateTime().is_not_a_date_time());
	BOOST_CHECK_EQUAL(sp1.getTheoreticalDepartureDateTime(), ptime(today, time_duration(2,19,0)));
	BOOST_CHECK(sp1.getTheoreticalArrivalDateTime().is_not_a_date_time());
	BOOST_CHECK_EQUAL(sp1.getOriginDateTime(), ptime(today, time_duration(2,10,0)));
	BOOST_CHECK_EQUAL(sp1.getServiceRange(), minutes(0));
	BOOST_CHECK(sp1.getDuration().is_not_a_date_time());

	s.completeServicePointer(sp1, l7AD, ap);
	BOOST_CHECK_EQUAL(sp1.getDepartureEdge(), &l3AD);
	BOOST_CHECK_EQUAL(sp1.getArrivalEdge(), &l7AD);
	BOOST_CHECK_EQUAL(sp1.getRealTimeDepartureVertex(), l3AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp1.getRealTimeArrivalVertex(), l7AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp1.getService(), &s);
	BOOST_CHECK_EQUAL(sp1.getDepartureDateTime(), ptime(today, time_duration(2,29,0)));
	BOOST_CHECK_EQUAL(sp1.getArrivalDateTime(), ptime(today, time_duration(2,41,0)));
	BOOST_CHECK_EQUAL(sp1.getTheoreticalDepartureDateTime(), ptime(today, time_duration(2,19,0)));
	BOOST_CHECK_EQUAL(sp1.getTheoreticalArrivalDateTime(), ptime(today, time_duration(2,31,0)));
	BOOST_CHECK_EQUAL(sp1.getOriginDateTime(), ptime(today, time_duration(2,10,0)));
	BOOST_CHECK_EQUAL(sp1.getServiceRange(), minutes(0));
	BOOST_CHECK_EQUAL(sp1.getDuration(), minutes(12));

}

BOOST_AUTO_TEST_CASE (testContinuousService)
{
	GeographyModule::PreInit();

	date today(day_clock::local_day());

	RuleUser::Rules r;
	r.push_back(AllowedUseRule::INSTANCE.get());
	r.push_back(AllowedUseRule::INSTANCE.get());
	r.push_back(AllowedUseRule::INSTANCE.get());
	JourneyPattern l(5678);
	l.setRules(r);

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

	DesignatedLinePhysicalStop l1D(0, &l, 0, true, false,0,&s1, true);
	DesignatedLinePhysicalStop l2D(0, &l, 1, true, false,50,&s2, false);
	DesignatedLinePhysicalStop l3AD(0, &l, 2, true, true,160,&s3, false);
	DesignatedLinePhysicalStop l4A(0, &l, 3, true, true,200,&s4, true);
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

	ContinuousService s(1234, "1234AB", &l, hours(1), minutes(5));

	BOOST_CHECK_EQUAL(s.getKey(), 1234);
	BOOST_CHECK_EQUAL(s.getServiceNumber(), "1234AB");
	BOOST_CHECK_EQUAL(s.getRange(), hours(1));
	BOOST_CHECK_EQUAL(s.getMaxWaitingTime(), minutes(5));
	BOOST_CHECK_EQUAL(s.isContinuous(), true);

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

	s.setDataSchedules(d, a);
	s.setActive(today);

	SchedulesBasedService::Schedules id(s.getDepartureSchedules(true, false));
	SchedulesBasedService::Schedules ia(s.getArrivalSchedules(true, false));
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


	// Service pointer
	AccessParameters ap;

	// From departure, before the departure time
	ptime time1(today, time_duration(1,50,0));
	ServicePointer sp1(
		s.getFromPresenceTime(
			ap,
			true,
			false,
			true,
			l3AD,
			time1,
			false,
			false,
			true,
			true
	)	);
	BOOST_CHECK_EQUAL(sp1.getDepartureEdge(), &l3AD);
	BOOST_CHECK(sp1.getArrivalEdge() == NULL);
	BOOST_CHECK_EQUAL(sp1.getRealTimeDepartureVertex(), l3AD.getFromVertex());
	BOOST_CHECK(sp1.getRealTimeArrivalVertex() == NULL);
	BOOST_CHECK_EQUAL(sp1.getService(), &s);
	BOOST_CHECK_EQUAL(sp1.getDepartureDateTime(), ptime(today, time_duration(2,19,0)));
	BOOST_CHECK(sp1.getArrivalDateTime().is_not_a_date_time());
	BOOST_CHECK_EQUAL(sp1.getTheoreticalDepartureDateTime(), ptime(today, time_duration(2,19,0)));
	BOOST_CHECK(sp1.getTheoreticalArrivalDateTime().is_not_a_date_time());
	BOOST_CHECK_EQUAL(sp1.getOriginDateTime(), ptime(today, time_duration(2,0,0)));
	BOOST_CHECK_EQUAL(sp1.getServiceRange(), hours(1));

	s.completeServicePointer(sp1, l7AD, ap);
	BOOST_CHECK_EQUAL(sp1.getDepartureEdge(), &l3AD);
	BOOST_CHECK_EQUAL(sp1.getArrivalEdge(), &l7AD);
	BOOST_CHECK_EQUAL(sp1.getRealTimeDepartureVertex(), l3AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp1.getRealTimeArrivalVertex(), l7AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp1.getService(), &s);
	BOOST_CHECK_EQUAL(sp1.getDepartureDateTime(), ptime(today, time_duration(2,19,0)));
	BOOST_CHECK_EQUAL(sp1.getArrivalDateTime(), ptime(today, time_duration(2,36,0)));
	BOOST_CHECK_EQUAL(sp1.getTheoreticalDepartureDateTime(), ptime(today, time_duration(2,19,0)));
	BOOST_CHECK_EQUAL(sp1.getTheoreticalArrivalDateTime(), ptime(today, time_duration(2,36,0)));
	BOOST_CHECK_EQUAL(sp1.getOriginDateTime(), ptime(today, time_duration(2,0,0)));
	BOOST_CHECK_EQUAL(sp1.getServiceRange(), hours(1));

	// From departure, at the departure time
	ptime time2(today, time_duration(2,19,0));
	ServicePointer sp2(
		s.getFromPresenceTime(
			ap,
			true,
			false,
			true,
			l3AD,
			time2,
			false,
			false,
			true,
			true
	)	);
	BOOST_CHECK_EQUAL(sp2.getDepartureEdge(), &l3AD);
	BOOST_CHECK(sp2.getArrivalEdge() == NULL);
	BOOST_CHECK_EQUAL(sp2.getRealTimeDepartureVertex(), l3AD.getFromVertex());
	BOOST_CHECK(sp2.getRealTimeArrivalVertex() == NULL);
	BOOST_CHECK_EQUAL(sp2.getService(), &s);
	BOOST_CHECK_EQUAL(sp2.getDepartureDateTime(), ptime(today, time_duration(2,19,0)));
	BOOST_CHECK(sp2.getArrivalDateTime().is_not_a_date_time());
	BOOST_CHECK_EQUAL(sp2.getTheoreticalDepartureDateTime(), ptime(today, time_duration(2,19,0)));
	BOOST_CHECK(sp2.getTheoreticalArrivalDateTime().is_not_a_date_time());
	BOOST_CHECK_EQUAL(sp2.getOriginDateTime(), ptime(today, time_duration(2,0,0)));
	BOOST_CHECK_EQUAL(sp2.getServiceRange(), hours(1));

	s.completeServicePointer(sp2, l7AD, ap);
	BOOST_CHECK_EQUAL(sp2.getDepartureEdge(), &l3AD);
	BOOST_CHECK_EQUAL(sp2.getArrivalEdge(), &l7AD);
	BOOST_CHECK_EQUAL(sp2.getRealTimeDepartureVertex(), l3AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp2.getRealTimeArrivalVertex(), l7AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp2.getService(), &s);
	BOOST_CHECK_EQUAL(sp2.getDepartureDateTime(), ptime(today, time_duration(2,19,0)));
	BOOST_CHECK_EQUAL(sp2.getArrivalDateTime(), ptime(today, time_duration(2,36,0)));
	BOOST_CHECK_EQUAL(sp2.getTheoreticalDepartureDateTime(), ptime(today, time_duration(2,19,0)));
	BOOST_CHECK_EQUAL(sp2.getTheoreticalArrivalDateTime(), ptime(today, time_duration(2,36,0)));
	BOOST_CHECK_EQUAL(sp2.getOriginDateTime(), ptime(today, time_duration(2,0,0)));
	BOOST_CHECK_EQUAL(sp2.getServiceRange(), hours(1));

	// From departure, at the departure time, inverted
	ServicePointer sp2i(
		s.getFromPresenceTime(
			ap,
			true,
			false,
			true,
			l3AD,
			time2,
			false,
			true, // Inverted
			true,
			true
	)	);
	BOOST_CHECK_EQUAL(sp2i.getDepartureEdge(), &l3AD);
	BOOST_CHECK(sp2i.getArrivalEdge() == NULL);
	BOOST_CHECK_EQUAL(sp2i.getRealTimeDepartureVertex(), l3AD.getFromVertex());
	BOOST_CHECK(sp2i.getRealTimeArrivalVertex() == NULL);
	BOOST_CHECK_EQUAL(sp2i.getService(), &s);
	BOOST_CHECK_EQUAL(sp2i.getDepartureDateTime(), ptime(today, time_duration(2,19,0)));
	BOOST_CHECK(sp2i.getArrivalDateTime().is_not_a_date_time());
	BOOST_CHECK_EQUAL(sp2i.getTheoreticalDepartureDateTime(), ptime(today, time_duration(2,19,0)));
	BOOST_CHECK(sp2i.getTheoreticalArrivalDateTime().is_not_a_date_time());
	BOOST_CHECK_EQUAL(sp2i.getOriginDateTime(), ptime(today, time_duration(2,0,0)));
	BOOST_CHECK_EQUAL(sp2i.getServiceRange(), hours(0));

	s.completeServicePointer(sp2i, l7AD, ap);
	BOOST_CHECK_EQUAL(sp2i.getDepartureEdge(), &l3AD);
	BOOST_CHECK_EQUAL(sp2i.getArrivalEdge(), &l7AD);
	BOOST_CHECK_EQUAL(sp2i.getRealTimeDepartureVertex(), l3AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp2i.getRealTimeArrivalVertex(), l7AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp2i.getService(), &s);
	BOOST_CHECK_EQUAL(sp2i.getDepartureDateTime(), ptime(today, time_duration(2,19,0)));
	BOOST_CHECK_EQUAL(sp2i.getArrivalDateTime(), ptime(today, time_duration(2,36,0)));
	BOOST_CHECK_EQUAL(sp2i.getTheoreticalDepartureDateTime(), ptime(today, time_duration(2,19,0)));
	BOOST_CHECK_EQUAL(sp2i.getTheoreticalArrivalDateTime(), ptime(today, time_duration(2,36,0)));
	BOOST_CHECK_EQUAL(sp2i.getOriginDateTime(), ptime(today, time_duration(2,0,0)));
	BOOST_CHECK_EQUAL(sp2i.getServiceRange(), hours(0));


	// From departure, after the departure time, before the end of the range
	ptime time3(today, time_duration(2,20,0));
	ServicePointer sp3(
		s.getFromPresenceTime(
			ap,
			true,
			false,
			true,
			l3AD,
			time3,
			false,
			false,
			true,
			true
	)	);
	BOOST_CHECK_EQUAL(sp3.getDepartureEdge(), &l3AD);
	BOOST_CHECK(sp3.getArrivalEdge() == NULL);
	BOOST_CHECK_EQUAL(sp3.getRealTimeDepartureVertex(), l3AD.getFromVertex());
	BOOST_CHECK(sp3.getRealTimeArrivalVertex() == NULL);
	BOOST_CHECK_EQUAL(sp3.getService(), &s);
	BOOST_CHECK_EQUAL(sp3.getDepartureDateTime(), ptime(today, time_duration(2,20,0)));
	BOOST_CHECK(sp3.getArrivalDateTime().is_not_a_date_time());
	BOOST_CHECK_EQUAL(sp3.getTheoreticalDepartureDateTime(), ptime(today, time_duration(2,20,0)));
	BOOST_CHECK(sp3.getTheoreticalArrivalDateTime().is_not_a_date_time());
	BOOST_CHECK_EQUAL(sp3.getOriginDateTime(), ptime(today, time_duration(2,1,0)));
	BOOST_CHECK_EQUAL(sp3.getServiceRange(), minutes(59));

	s.completeServicePointer(sp3, l7AD, ap);
	BOOST_CHECK_EQUAL(sp3.getDepartureEdge(), &l3AD);
	BOOST_CHECK_EQUAL(sp3.getArrivalEdge(), &l7AD);
	BOOST_CHECK_EQUAL(sp3.getRealTimeDepartureVertex(), l3AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp3.getRealTimeArrivalVertex(), l7AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp3.getService(), &s);
	BOOST_CHECK_EQUAL(sp3.getDepartureDateTime(), ptime(today, time_duration(2,20,0)));
	BOOST_CHECK_EQUAL(sp3.getArrivalDateTime(), ptime(today, time_duration(2,37,0)));
	BOOST_CHECK_EQUAL(sp3.getTheoreticalDepartureDateTime(), ptime(today, time_duration(2,20,0)));
	BOOST_CHECK_EQUAL(sp3.getTheoreticalArrivalDateTime(), ptime(today, time_duration(2,37,0)));
	BOOST_CHECK_EQUAL(sp3.getOriginDateTime(), ptime(today, time_duration(2,1,0)));
	BOOST_CHECK_EQUAL(sp3.getServiceRange(), minutes(59));


	// From departure, after the departure time, before the end of the range
	ServicePointer sp3i(
		s.getFromPresenceTime(
			ap,
			true,
			false,
			true,
			l3AD,
			time3,
			false,
			true, // Inverted
			true,
			true
	)	);
	BOOST_CHECK_EQUAL(sp3i.getDepartureEdge(), &l3AD);
	BOOST_CHECK(sp3i.getArrivalEdge() == NULL);
	BOOST_CHECK_EQUAL(sp3i.getRealTimeDepartureVertex(), l3AD.getFromVertex());
	BOOST_CHECK(sp3i.getRealTimeArrivalVertex() == NULL);
	BOOST_CHECK_EQUAL(sp3i.getService(), &s);
	BOOST_CHECK_EQUAL(sp3i.getDepartureDateTime(), ptime(today, time_duration(2,19,0)));
	BOOST_CHECK(sp3i.getArrivalDateTime().is_not_a_date_time());
	BOOST_CHECK_EQUAL(sp3i.getTheoreticalDepartureDateTime(), ptime(today, time_duration(2,19,0)));
	BOOST_CHECK(sp3i.getTheoreticalArrivalDateTime().is_not_a_date_time());
	BOOST_CHECK_EQUAL(sp3i.getOriginDateTime(), ptime(today, time_duration(2,0,0)));
	BOOST_CHECK_EQUAL(sp3i.getServiceRange(), minutes(1));

	s.completeServicePointer(sp3i, l7AD, ap);
	BOOST_CHECK_EQUAL(sp3i.getDepartureEdge(), &l3AD);
	BOOST_CHECK_EQUAL(sp3i.getArrivalEdge(), &l7AD);
	BOOST_CHECK_EQUAL(sp3i.getRealTimeDepartureVertex(), l3AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp3i.getRealTimeArrivalVertex(), l7AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp3i.getService(), &s);
	BOOST_CHECK_EQUAL(sp3i.getDepartureDateTime(), ptime(today, time_duration(2,19,0)));
	BOOST_CHECK_EQUAL(sp3i.getArrivalDateTime(), ptime(today, time_duration(2,36,0)));
	BOOST_CHECK_EQUAL(sp3i.getTheoreticalDepartureDateTime(), ptime(today, time_duration(2,19,0)));
	BOOST_CHECK_EQUAL(sp3i.getTheoreticalArrivalDateTime(), ptime(today, time_duration(2,36,0)));
	BOOST_CHECK_EQUAL(sp3i.getOriginDateTime(), ptime(today, time_duration(2,0,0)));
	BOOST_CHECK_EQUAL(sp3i.getServiceRange(), minutes(1));


	// From departure, after the departure time, at the end of the range
	ptime time4(today, time_duration(3,19,0));
	ServicePointer sp4(
		s.getFromPresenceTime(
			ap,
			true,
			false,
			true,
			l3AD,
			time4,
			false,
			false,
			true,
			true
	)	);
	BOOST_CHECK_EQUAL(sp4.getDepartureEdge(), &l3AD);
	BOOST_CHECK(sp4.getArrivalEdge() == NULL);
	BOOST_CHECK_EQUAL(sp4.getRealTimeDepartureVertex(), l3AD.getFromVertex());
	BOOST_CHECK(sp4.getRealTimeArrivalVertex() == NULL);
	BOOST_CHECK_EQUAL(sp4.getService(), &s);
	BOOST_CHECK_EQUAL(sp4.getDepartureDateTime(), ptime(today, time_duration(3,19,0)));
	BOOST_CHECK(sp4.getArrivalDateTime().is_not_a_date_time());
	BOOST_CHECK_EQUAL(sp4.getTheoreticalDepartureDateTime(), ptime(today, time_duration(3,19,0)));
	BOOST_CHECK(sp4.getTheoreticalArrivalDateTime().is_not_a_date_time());
	BOOST_CHECK_EQUAL(sp4.getOriginDateTime(), ptime(today, time_duration(3,0,0)));
	BOOST_CHECK_EQUAL(sp4.getServiceRange(), minutes(0));

	s.completeServicePointer(sp4, l7AD, ap);
	BOOST_CHECK_EQUAL(sp4.getDepartureEdge(), &l3AD);
	BOOST_CHECK_EQUAL(sp4.getArrivalEdge(), &l7AD);
	BOOST_CHECK_EQUAL(sp4.getRealTimeDepartureVertex(), l3AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp4.getRealTimeArrivalVertex(), l7AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp4.getService(), &s);
	BOOST_CHECK_EQUAL(sp4.getDepartureDateTime(), ptime(today, time_duration(3,19,0)));
	BOOST_CHECK_EQUAL(sp4.getArrivalDateTime(), ptime(today, time_duration(3,36,0)));
	BOOST_CHECK_EQUAL(sp4.getTheoreticalDepartureDateTime(), ptime(today, time_duration(3,19,0)));
	BOOST_CHECK_EQUAL(sp4.getTheoreticalArrivalDateTime(), ptime(today, time_duration(3,36,0)));
	BOOST_CHECK_EQUAL(sp4.getOriginDateTime(), ptime(today, time_duration(3,0,0)));
	BOOST_CHECK_EQUAL(sp4.getServiceRange(), minutes(0));

	// From departure, after the departure time, at the end of the range, inverted
	ServicePointer sp4i(
		s.getFromPresenceTime(
			ap,
			true,
			false,
			true,
			l3AD,
			time4,
			false,
			true,
			true,
			true
	)	);
	BOOST_CHECK_EQUAL(sp4i.getDepartureEdge(), &l3AD);
	BOOST_CHECK(sp4i.getArrivalEdge() == NULL);
	BOOST_CHECK_EQUAL(sp4i.getRealTimeDepartureVertex(), l3AD.getFromVertex());
	BOOST_CHECK(sp4i.getRealTimeArrivalVertex() == NULL);
	BOOST_CHECK_EQUAL(sp4i.getService(), &s);
	BOOST_CHECK_EQUAL(sp4i.getDepartureDateTime(), ptime(today, time_duration(2,19,0)));
	BOOST_CHECK(sp4i.getArrivalDateTime().is_not_a_date_time());
	BOOST_CHECK_EQUAL(sp4i.getTheoreticalDepartureDateTime(), ptime(today, time_duration(2,19,0)));
	BOOST_CHECK(sp4i.getTheoreticalArrivalDateTime().is_not_a_date_time());
	BOOST_CHECK_EQUAL(sp4i.getOriginDateTime(), ptime(today, time_duration(2,0,0)));
	BOOST_CHECK_EQUAL(sp4i.getServiceRange(), minutes(60));

	s.completeServicePointer(sp4i, l7AD, ap);
	BOOST_CHECK_EQUAL(sp4i.getDepartureEdge(), &l3AD);
	BOOST_CHECK_EQUAL(sp4i.getArrivalEdge(), &l7AD);
	BOOST_CHECK_EQUAL(sp4i.getRealTimeDepartureVertex(), l3AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp4i.getRealTimeArrivalVertex(), l7AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp4i.getService(), &s);
	BOOST_CHECK_EQUAL(sp4i.getDepartureDateTime(), ptime(today, time_duration(2,19,0)));
	BOOST_CHECK_EQUAL(sp4i.getArrivalDateTime(), ptime(today, time_duration(2,36,0)));
	BOOST_CHECK_EQUAL(sp4i.getTheoreticalDepartureDateTime(), ptime(today, time_duration(2,19,0)));
	BOOST_CHECK_EQUAL(sp4i.getTheoreticalArrivalDateTime(), ptime(today, time_duration(2,36,0)));
	BOOST_CHECK_EQUAL(sp4i.getOriginDateTime(), ptime(today, time_duration(2,0,0)));
	BOOST_CHECK_EQUAL(sp4i.getServiceRange(), minutes(60));


	// From departure, after the end of the range
	ptime time5(today, time_duration(3,20,0));
	ServicePointer sp5(
		s.getFromPresenceTime(
			ap,
			true,
			false,
			true,
			l3AD,
			time5,
			false,
			false,
			true,
			true
	)	);
	BOOST_CHECK(sp5.getService() == NULL);


	// From arrival, after the end of the range time
	ptime time6(today, time_duration(3,40,0));
	ServicePointer sp6(
		s.getFromPresenceTime(
			ap,
			true,
			false,
			false,
			l7AD,
			time6,
			false,
			false,
			true,
			true
	)	);
	BOOST_CHECK(sp6.getDepartureEdge() == NULL);
	BOOST_CHECK_EQUAL(sp6.getArrivalEdge(),&l7AD);
	BOOST_CHECK(sp6.getRealTimeDepartureVertex() == NULL);
	BOOST_CHECK_EQUAL(sp6.getRealTimeArrivalVertex(), l7AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp6.getService(), &s);
	BOOST_CHECK(sp6.getDepartureDateTime().is_not_a_date_time());
	BOOST_CHECK_EQUAL(sp6.getArrivalDateTime(), ptime(today, time_duration(2,36,0)));
	BOOST_CHECK(sp6.getTheoreticalDepartureDateTime().is_not_a_date_time());
	BOOST_CHECK_EQUAL(sp6.getTheoreticalArrivalDateTime(), ptime(today, time_duration(2,36,0)));
	BOOST_CHECK_EQUAL(sp6.getOriginDateTime(), ptime(today, time_duration(2,0,0)));
	BOOST_CHECK_EQUAL(sp6.getServiceRange(), minutes(60));

	s.completeServicePointer(sp6, l3AD, ap);
	BOOST_CHECK_EQUAL(sp6.getDepartureEdge(), &l3AD);
	BOOST_CHECK_EQUAL(sp6.getArrivalEdge(), &l7AD);
	BOOST_CHECK_EQUAL(sp6.getRealTimeDepartureVertex(), l3AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp6.getRealTimeArrivalVertex(), l7AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp6.getService(), &s);
	BOOST_CHECK_EQUAL(sp6.getDepartureDateTime(), ptime(today, time_duration(2,19,0)));
	BOOST_CHECK_EQUAL(sp6.getArrivalDateTime(), ptime(today, time_duration(2,36,0)));
	BOOST_CHECK_EQUAL(sp6.getTheoreticalDepartureDateTime(), ptime(today, time_duration(2,19,0)));
	BOOST_CHECK_EQUAL(sp6.getTheoreticalArrivalDateTime(), ptime(today, time_duration(2,36,0)));
	BOOST_CHECK_EQUAL(sp6.getOriginDateTime(), ptime(today, time_duration(2,0,0)));
	BOOST_CHECK_EQUAL(sp6.getServiceRange(), minutes(60));


	// From arrival, at the end of the range time
	ptime time7(today, time_duration(3,36,0));
	ServicePointer sp7(
		s.getFromPresenceTime(
			ap,
			true,
			false,
			false,
			l7AD,
			time7,
			false,
			false,
			true,
			true
	)	);
	BOOST_CHECK(sp7.getDepartureEdge() == NULL);
	BOOST_CHECK_EQUAL(sp7.getArrivalEdge(),&l7AD);
	BOOST_CHECK(sp7.getRealTimeDepartureVertex() == NULL);
	BOOST_CHECK_EQUAL(sp7.getRealTimeArrivalVertex(), l7AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp7.getService(), &s);
	BOOST_CHECK(sp7.getDepartureDateTime().is_not_a_date_time());
	BOOST_CHECK_EQUAL(sp7.getArrivalDateTime(), ptime(today, time_duration(2,36,0)));
	BOOST_CHECK(sp7.getTheoreticalDepartureDateTime().is_not_a_date_time());
	BOOST_CHECK_EQUAL(sp7.getTheoreticalArrivalDateTime(), ptime(today, time_duration(2,36,0)));
	BOOST_CHECK_EQUAL(sp7.getOriginDateTime(), ptime(today, time_duration(2,0,0)));
	BOOST_CHECK_EQUAL(sp7.getServiceRange(), minutes(60));

	s.completeServicePointer(sp7, l3AD, ap);
	BOOST_CHECK_EQUAL(sp7.getDepartureEdge(), &l3AD);
	BOOST_CHECK_EQUAL(sp7.getArrivalEdge(), &l7AD);
	BOOST_CHECK_EQUAL(sp7.getRealTimeDepartureVertex(), l3AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp7.getRealTimeArrivalVertex(), l7AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp7.getService(), &s);
	BOOST_CHECK_EQUAL(sp7.getDepartureDateTime(), ptime(today, time_duration(2,19,0)));
	BOOST_CHECK_EQUAL(sp7.getArrivalDateTime(), ptime(today, time_duration(2,36,0)));
	BOOST_CHECK_EQUAL(sp7.getTheoreticalDepartureDateTime(), ptime(today, time_duration(2,19,0)));
	BOOST_CHECK_EQUAL(sp7.getTheoreticalArrivalDateTime(), ptime(today, time_duration(2,36,0)));
	BOOST_CHECK_EQUAL(sp7.getOriginDateTime(), ptime(today, time_duration(2,0,0)));
	BOOST_CHECK_EQUAL(sp7.getServiceRange(), minutes(60));




	// From arrival, in the range
	ptime time8(today, time_duration(2,50,0));
	ServicePointer sp8(
		s.getFromPresenceTime(
			ap,
			true,
			false,
			false,
			l7AD,
			time8,
			false,
			false,
			true,
			true
	)	);
	BOOST_CHECK(sp8.getDepartureEdge() == NULL);
	BOOST_CHECK_EQUAL(sp8.getArrivalEdge(),&l7AD);
	BOOST_CHECK(sp8.getRealTimeDepartureVertex() == NULL);
	BOOST_CHECK_EQUAL(sp8.getRealTimeArrivalVertex(), l7AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp8.getService(), &s);
	BOOST_CHECK(sp8.getDepartureDateTime().is_not_a_date_time());
	BOOST_CHECK_EQUAL(sp8.getArrivalDateTime(), ptime(today, time_duration(2,36,0)));
	BOOST_CHECK(sp8.getTheoreticalDepartureDateTime().is_not_a_date_time());
	BOOST_CHECK_EQUAL(sp8.getTheoreticalArrivalDateTime(), ptime(today, time_duration(2,36,0)));
	BOOST_CHECK_EQUAL(sp8.getOriginDateTime(), ptime(today, time_duration(2,0,0)));
	BOOST_CHECK_EQUAL(sp8.getServiceRange(), minutes(14));

	s.completeServicePointer(sp8, l3AD, ap);
	BOOST_CHECK_EQUAL(sp8.getDepartureEdge(), &l3AD);
	BOOST_CHECK_EQUAL(sp8.getArrivalEdge(), &l7AD);
	BOOST_CHECK_EQUAL(sp8.getRealTimeDepartureVertex(), l3AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp8.getRealTimeArrivalVertex(), l7AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp8.getService(), &s);
	BOOST_CHECK_EQUAL(sp8.getDepartureDateTime(), ptime(today, time_duration(2,19,0)));
	BOOST_CHECK_EQUAL(sp8.getArrivalDateTime(), ptime(today, time_duration(2,36,0)));
	BOOST_CHECK_EQUAL(sp8.getTheoreticalDepartureDateTime(), ptime(today, time_duration(2,19,0)));
	BOOST_CHECK_EQUAL(sp8.getTheoreticalArrivalDateTime(), ptime(today, time_duration(2,36,0)));
	BOOST_CHECK_EQUAL(sp8.getOriginDateTime(), ptime(today, time_duration(2,0,0)));
	BOOST_CHECK_EQUAL(sp8.getServiceRange(), minutes(14));
}


BOOST_AUTO_TEST_CASE (testPermanentService)
{
	GeographyModule::PreInit();

	date today(day_clock::local_day());

	RuleUser::Rules r;
	r.push_back(AllowedUseRule::INSTANCE.get());
	r.push_back(AllowedUseRule::INSTANCE.get());
	r.push_back(AllowedUseRule::INSTANCE.get());
	JourneyPattern l(5678);
	l.setRules(r);

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

	PermanentService s(1234, &l, minutes(15));

	BOOST_CHECK_EQUAL(s.getKey(), 1234);


	// Service pointer
	AccessParameters ap;

	// From departure
	ptime time1(today, time_duration(1,50,0));
	ServicePointer sp1(
		s.getFromPresenceTime(
			ap,
			true,
			false,
			true,
			l3AD,
			time1,
			false,
			false,
			true,
			true
	)	);
	BOOST_CHECK_EQUAL(sp1.getDepartureEdge(), &l3AD);
	BOOST_CHECK(sp1.getArrivalEdge() == NULL);
	BOOST_CHECK_EQUAL(sp1.getRealTimeDepartureVertex(), l3AD.getFromVertex());
	BOOST_CHECK(sp1.getRealTimeArrivalVertex() == NULL);
	BOOST_CHECK_EQUAL(sp1.getService(), &s);
	BOOST_CHECK_EQUAL(sp1.getDepartureDateTime(), ptime(today, time_duration(1,50,0)));
	BOOST_CHECK(sp1.getArrivalDateTime().is_not_a_date_time());
	BOOST_CHECK_EQUAL(sp1.getTheoreticalDepartureDateTime(), ptime(today, time_duration(1,50,0)));
	BOOST_CHECK(sp1.getTheoreticalArrivalDateTime().is_not_a_date_time());
	BOOST_CHECK_EQUAL(sp1.getOriginDateTime(), ptime(today, time_duration(1,50,0)));
	BOOST_CHECK_EQUAL(sp1.getServiceRange(), hours(24));

	s.completeServicePointer(sp1, l7AD, ap);
	BOOST_CHECK_EQUAL(sp1.getDepartureEdge(), &l3AD);
	BOOST_CHECK_EQUAL(sp1.getArrivalEdge(), &l7AD);
	BOOST_CHECK_EQUAL(sp1.getRealTimeDepartureVertex(), l3AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp1.getRealTimeArrivalVertex(), l7AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp1.getService(), &s);
	BOOST_CHECK_EQUAL(sp1.getDepartureDateTime(), ptime(today, time_duration(1,50,0)));
	BOOST_CHECK_EQUAL(sp1.getArrivalDateTime(), ptime(today, time_duration(2,5,0)));
	BOOST_CHECK_EQUAL(sp1.getTheoreticalDepartureDateTime(), ptime(today, time_duration(1,50,0)));
	BOOST_CHECK_EQUAL(sp1.getTheoreticalArrivalDateTime(), ptime(today, time_duration(2,5,0)));
	BOOST_CHECK_EQUAL(sp1.getOriginDateTime(), ptime(today, time_duration(1,50,0)));
	BOOST_CHECK_EQUAL(sp1.getServiceRange(), hours(24));

	// From departure inverted
	ServicePointer sp1i(
		s.getFromPresenceTime(
			ap,
			true,
			false,
			true,
			l3AD,
			time1,
			false,
			true,
			true,
			true
	)	);
	BOOST_CHECK_EQUAL(sp1i.getDepartureEdge(), &l3AD);
	BOOST_CHECK(sp1i.getArrivalEdge() == NULL);
	BOOST_CHECK_EQUAL(sp1i.getRealTimeDepartureVertex(), l3AD.getFromVertex());
	BOOST_CHECK(sp1i.getRealTimeArrivalVertex() == NULL);
	BOOST_CHECK_EQUAL(sp1i.getService(), &s);
	BOOST_CHECK_EQUAL(sp1i.getDepartureDateTime(), ptime(today - days(1), time_duration(1,50,0)));
	BOOST_CHECK(sp1i.getArrivalDateTime().is_not_a_date_time());
	BOOST_CHECK_EQUAL(sp1i.getTheoreticalDepartureDateTime(), ptime(today - days(1), time_duration(1,50,0)));
	BOOST_CHECK(sp1i.getTheoreticalArrivalDateTime().is_not_a_date_time());
	BOOST_CHECK_EQUAL(sp1i.getOriginDateTime(), ptime(today - days(1), time_duration(1,50,0)));
	BOOST_CHECK_EQUAL(sp1i.getServiceRange(), hours(24));

	s.completeServicePointer(sp1i, l7AD, ap);
	BOOST_CHECK_EQUAL(sp1i.getDepartureEdge(), &l3AD);
	BOOST_CHECK_EQUAL(sp1i.getArrivalEdge(), &l7AD);
	BOOST_CHECK_EQUAL(sp1i.getRealTimeDepartureVertex(), l3AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp1i.getRealTimeArrivalVertex(), l7AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp1i.getService(), &s);
	BOOST_CHECK_EQUAL(sp1i.getDepartureDateTime(), ptime(today - days(1), time_duration(1,50,0)));
	BOOST_CHECK_EQUAL(sp1i.getArrivalDateTime(), ptime(today - days(1), time_duration(2,5,0)));
	BOOST_CHECK_EQUAL(sp1i.getTheoreticalDepartureDateTime(), ptime(today - days(1), time_duration(1,50,0)));
	BOOST_CHECK_EQUAL(sp1i.getTheoreticalArrivalDateTime(), ptime(today - days(1), time_duration(2,5,0)));
	BOOST_CHECK_EQUAL(sp1i.getOriginDateTime(), ptime(today - days(1), time_duration(1,50,0)));
	BOOST_CHECK_EQUAL(sp1i.getServiceRange(), hours(24));

	// From arrival
	ptime time7(today, time_duration(3,36,0));
	ServicePointer sp7(
		s.getFromPresenceTime(
			ap,
			true,
			false,
			false,
			l7AD,
			time7,
			false,
			false,
			true,
			true
	)	);
	BOOST_CHECK(sp7.getDepartureEdge() == NULL);
	BOOST_CHECK_EQUAL(sp7.getArrivalEdge(),&l7AD);
	BOOST_CHECK(sp7.getRealTimeDepartureVertex() == NULL);
	BOOST_CHECK_EQUAL(sp7.getRealTimeArrivalVertex(), l7AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp7.getService(), &s);
	BOOST_CHECK(sp7.getDepartureDateTime().is_not_a_date_time());
	BOOST_CHECK_EQUAL(sp7.getArrivalDateTime(), ptime(today - days(1), time_duration(3,36,0)));
	BOOST_CHECK(sp7.getTheoreticalDepartureDateTime().is_not_a_date_time());
	BOOST_CHECK_EQUAL(sp7.getTheoreticalArrivalDateTime(), ptime(today - days(1), time_duration(3,36,0)));
	BOOST_CHECK_EQUAL(sp7.getOriginDateTime(), ptime(today - days(1), time_duration(3,21,0)));
	BOOST_CHECK_EQUAL(sp7.getServiceRange(), hours(24));

	s.completeServicePointer(sp7, l3AD, ap);
	BOOST_CHECK_EQUAL(sp7.getDepartureEdge(), &l3AD);
	BOOST_CHECK_EQUAL(sp7.getArrivalEdge(), &l7AD);
	BOOST_CHECK_EQUAL(sp7.getRealTimeDepartureVertex(), l3AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp7.getRealTimeArrivalVertex(), l7AD.getFromVertex());
	BOOST_CHECK_EQUAL(sp7.getService(), &s);
	BOOST_CHECK_EQUAL(sp7.getDepartureDateTime(), ptime(today - days(1), time_duration(3,21,0)));
	BOOST_CHECK_EQUAL(sp7.getArrivalDateTime(), ptime(today - days(1), time_duration(3,36,0)));
	BOOST_CHECK_EQUAL(sp7.getTheoreticalDepartureDateTime(), ptime(today - days(1), time_duration(3,21,0)));
	BOOST_CHECK_EQUAL(sp7.getTheoreticalArrivalDateTime(), ptime(today - days(1), time_duration(3,36,0)));
	BOOST_CHECK_EQUAL(sp7.getOriginDateTime(), ptime(today - days(1), time_duration(3,21,0)));
	BOOST_CHECK_EQUAL(sp7.getServiceRange(), hours(24));
}

BOOST_AUTO_TEST_CASE (testServcesIndices)
{
	Env env;
	GeographyModule::PreInit();

	date today(day_clock::local_day());

	RuleUser::Rules r;
	r.push_back(AllowedUseRule::INSTANCE.get());
	r.push_back(AllowedUseRule::INSTANCE.get());
	r.push_back(AllowedUseRule::INSTANCE.get());
	CommercialLine line;
	JourneyPattern l(5678);
	l.setCommercialLine(&line);
	l.setRules(r);

	StopArea p1(0, true);
	StopArea p2(0, false);
	StopArea p3(0, false);
	StopArea p4(0, false);
	
	DRTArea a23;
	{
		Stops::Type stops;
		stops.insert(&p2);
		stops.insert(&p3);
		a23.set<Stops>(stops);
	}

	StopPoint s1(0, "s1", &p1);
	s1.link(env, true);
	StopPoint s2(0, "s1", &p2);
	s2.link(env, true);
	StopPoint s3(0, "s1", &p3);
	s3.link(env, true);
	StopPoint s4(0, "s1", &p4);
	s4.link(env, true);

	DesignatedLinePhysicalStop l1D(0, &l, 0, true, false,0,&s1, true);
	l1D.link(env, true);

	LineArea l23AD(0, &l, 1, true, true,50,&a23, false);
	l23AD.link(env, true);

	DesignatedLinePhysicalStop l4A(0, &l, 2, false, true,200,&s4, true);
	l4A.link(env, true);

	DesignatedLinePhysicalStop* lNULL(NULL);

	SchedulesBasedService::Schedules d;
	SchedulesBasedService::Schedules a;

	a.push_back(time_duration(2, 0, 0));
	d.push_back(time_duration(2, 0, 0));

	a.push_back(time_duration(2, 30, 0));
	d.push_back(time_duration(2, 25, 0));

	a.push_back(time_duration(2, 35, 0));
	d.push_back(time_duration(2, 35, 0));

	ScheduledService s(1234, "1234AB", &l);

	BOOST_CHECK_EQUAL(s.getKey(), 1234);
	BOOST_CHECK_EQUAL(s.getServiceNumber(), "1234AB");
	BOOST_CHECK_EQUAL(s.getPath(), &l);
	BOOST_CHECK_EQUAL(s.isContinuous(), false);

	s.setDataSchedules(d, a);
	s.setActive(today);

	s.link(env, true);

	BOOST_CHECK_EQUAL(l.getServices().size(), 1);
	if(l.getServices().size() == 1)
	{
		BOOST_CHECK_EQUAL(*l.getServices().begin(), &s);
	}

	BOOST_CHECK(l1D._getServiceIndexUpdateNeeded(false));
	BOOST_CHECK(l1D._getServiceIndexUpdateNeeded(true));
	BOOST_CHECK(l23AD.getSubEdges().at(0)->_getServiceIndexUpdateNeeded(false));
	BOOST_CHECK(l23AD.getSubEdges().at(0)->_getServiceIndexUpdateNeeded(true));
	BOOST_CHECK(l23AD.getSubEdges().at(1)->_getServiceIndexUpdateNeeded(false));
	BOOST_CHECK(l23AD.getSubEdges().at(1)->_getServiceIndexUpdateNeeded(true));
	BOOST_CHECK(l4A._getServiceIndexUpdateNeeded(false));
	BOOST_CHECK(l4A._getServiceIndexUpdateNeeded(true));

	AccessParameters ap;
	ptime time7(today, time_duration(1,36,0));
	ptime tomorrow(time7 + days(1));
	boost::optional<Edge::DepartureServiceIndex::Value> lastIndex;
	ServicePointer sp7(
		l1D.getNextService(
			ap,
			time7,
			tomorrow,
			true,
			lastIndex
	)	);
	
	BOOST_CHECK(l1D._getServiceIndexUpdateNeeded(false));
	BOOST_CHECK(!l1D._getServiceIndexUpdateNeeded(true)); // Only RT because the search was for today
	BOOST_CHECK(l23AD.getSubEdges().at(0)->_getServiceIndexUpdateNeeded(false));
	BOOST_CHECK(l23AD.getSubEdges().at(0)->_getServiceIndexUpdateNeeded(true));
	BOOST_CHECK(l23AD.getSubEdges().at(1)->_getServiceIndexUpdateNeeded(false));
	BOOST_CHECK(l23AD.getSubEdges().at(1)->_getServiceIndexUpdateNeeded(true));
	BOOST_CHECK(l4A._getServiceIndexUpdateNeeded(false));
	BOOST_CHECK(l4A._getServiceIndexUpdateNeeded(true));

	tomorrow += days(1);
	ptime after_tomorrow(time7 + days(3));
	ServicePointer sp8(
		l1D.getNextService(
			ap,
			tomorrow,
			after_tomorrow,
			true,
			lastIndex
	)	);

	BOOST_CHECK(!l1D._getServiceIndexUpdateNeeded(false)); // Now planned index too, because the second search was for tomorrow
	BOOST_CHECK(!l1D._getServiceIndexUpdateNeeded(true));
	BOOST_CHECK(l23AD.getSubEdges().at(0)->_getServiceIndexUpdateNeeded(false));
	BOOST_CHECK(l23AD.getSubEdges().at(0)->_getServiceIndexUpdateNeeded(true));
	BOOST_CHECK(l23AD.getSubEdges().at(1)->_getServiceIndexUpdateNeeded(false));
	BOOST_CHECK(l23AD.getSubEdges().at(1)->_getServiceIndexUpdateNeeded(true));
	BOOST_CHECK(l4A._getServiceIndexUpdateNeeded(false));
	BOOST_CHECK(l4A._getServiceIndexUpdateNeeded(true));

	ServicePointer sp7b(
		l23AD.getSubEdges().at(0)->getNextService(
			ap,
			time7,
			tomorrow,
			true,
			lastIndex
	)	);

	BOOST_CHECK(!l1D._getServiceIndexUpdateNeeded(false));
	BOOST_CHECK(!l1D._getServiceIndexUpdateNeeded(true));
	BOOST_CHECK(l23AD.getSubEdges().at(0)->_getServiceIndexUpdateNeeded(false));
	BOOST_CHECK(!l23AD.getSubEdges().at(0)->_getServiceIndexUpdateNeeded(true)); // Because of last call
	BOOST_CHECK(l23AD.getSubEdges().at(1)->_getServiceIndexUpdateNeeded(false));
	BOOST_CHECK(l23AD.getSubEdges().at(1)->_getServiceIndexUpdateNeeded(true));
	BOOST_CHECK(l4A._getServiceIndexUpdateNeeded(false));
	BOOST_CHECK(l4A._getServiceIndexUpdateNeeded(true));

	s.unlink();

	BOOST_CHECK(l1D._getServiceIndexUpdateNeeded(false));
	BOOST_CHECK(l1D._getServiceIndexUpdateNeeded(true));
	BOOST_CHECK(l23AD.getSubEdges().at(0)->_getServiceIndexUpdateNeeded(false));
	BOOST_CHECK(l23AD.getSubEdges().at(0)->_getServiceIndexUpdateNeeded(true));
	BOOST_CHECK(l23AD.getSubEdges().at(1)->_getServiceIndexUpdateNeeded(false));
	BOOST_CHECK(l23AD.getSubEdges().at(1)->_getServiceIndexUpdateNeeded(true));
	BOOST_CHECK(l4A._getServiceIndexUpdateNeeded(false));
	BOOST_CHECK(l4A._getServiceIndexUpdateNeeded(true));
}

BOOST_AUTO_TEST_CASE (testSubline)
{
	GeographyModule::PreInit();

	date today(day_clock::local_day());

	CommercialLine cl;
	JourneyPattern l(5678);
	l.setCommercialLine(&cl);
	l.link(Env::GetOfficialEnv(), true);

	BOOST_CHECK_EQUAL(cl.getPaths().size(), 1);
	if(cl.getPaths().size() == 1)
	{
		BOOST_CHECK_EQUAL(*cl.getPaths().begin(), &l);
	}

	RuleUser::Rules r;
	r.push_back(AllowedUseRule::INSTANCE.get());
	r.push_back(AllowedUseRule::INSTANCE.get());
	r.push_back(AllowedUseRule::INSTANCE.get());
	l.setRules(r);

	StopArea p1(0, true);
	StopArea p2(0, false);
	StopArea p3(0, false);
	StopArea p4(0, false);
	StopArea p5(0, true);
	StopArea p6(0, true);
	StopArea p7(0, false);
	StopArea p8(0, false);

	StopPoint sp1(0, "s1", &p1);
	sp1.link(Env::GetOfficialEnv(), true);
	StopPoint sp2(0, "s1", &p2);
	sp2.link(Env::GetOfficialEnv(), true);
	StopPoint sp3(0, "s1", &p3);
	sp3.link(Env::GetOfficialEnv(), true);
	StopPoint sp4(0, "s1", &p4);
	sp4.link(Env::GetOfficialEnv(), true);
	StopPoint sp5(0, "s1", &p5);
	sp5.link(Env::GetOfficialEnv(), true);
	StopPoint sp6(0, "s1", &p6);
	sp6.link(Env::GetOfficialEnv(), true);
	StopPoint sp7(0, "s1", &p7);
	sp7.link(Env::GetOfficialEnv(), true);
	StopPoint sp8(0, "s1", &p8);
	sp8.link(Env::GetOfficialEnv(), true);

	DesignatedLinePhysicalStop l1D(0, &l, 0, true, false,0,&sp1, true);
	DesignatedLinePhysicalStop l2D(0, &l, 1, true, false,50,&sp2, false);
	DesignatedLinePhysicalStop l3AD(0, &l, 2, true, true,160,&sp3, false);
	DesignatedLinePhysicalStop l4A(0, &l, 3, true, true,200,&sp4, true);
	DesignatedLinePhysicalStop l5D(0, &l, 4, true, false,250,&sp5, false);
	DesignatedLinePhysicalStop l6AD(0, &l, 5, true, true,450,&sp6, false);
	DesignatedLinePhysicalStop l7AD(0, &l, 6, false, true,500,&sp7, true);
	DesignatedLinePhysicalStop l71A(0, &l, 7, false, true,500,&sp7, false);
	DesignatedLinePhysicalStop l8A(0, &l, 8, false, true,500,&sp8, true);
	DesignatedLinePhysicalStop* lNULL(NULL);

	l1D.link(Env::GetOfficialEnv(), true);
	l2D.link(Env::GetOfficialEnv(), true);
	l3AD.link(Env::GetOfficialEnv(), true);
	l4A.link(Env::GetOfficialEnv(), true);
	l5D.link(Env::GetOfficialEnv(), true);
	l6AD.link(Env::GetOfficialEnv(), true);
	l7AD.link(Env::GetOfficialEnv(), true);
	l71A.link(Env::GetOfficialEnv(), true);
	l8A.link(Env::GetOfficialEnv(), true);

	ScheduledService s1(1234, "1234AB", &l);
	ScheduledService s2(1235, "1235AB", &l);

	BOOST_CHECK_EQUAL(s1.getPath(), &l);
	BOOST_CHECK_EQUAL(s2.getPath(), &l);

	SchedulesBasedService::Schedules d;
	SchedulesBasedService::Schedules a;

	a.push_back(time_duration(2, 0, 0));
	d.push_back(time_duration(2, 0, 0));

	a.push_back(time_duration(2, 24, 0));
	d.push_back(time_duration(2, 30, 0));

	a.push_back(time_duration(2, 34, 0));
	d.push_back(time_duration(not_a_date_time));

	a.push_back(time_duration(3, 10, 0));
	d.push_back(time_duration(3, 10, 0));

	s1.setDataSchedules(d, a);
	s1.setActive(today);
	s2.setDataSchedules(d, a);
	s2.setActive(today);

	BOOST_CHECK(!l.isActive(today));

	// The service #1 is now linked to the environment
	s1.link(Env::GetOfficialEnv(), true);

	// The path has no subline
	BOOST_CHECK(l.getSubLines().empty());

	// Path of s1 has not changed
	BOOST_CHECK_EQUAL(s1.getPath(), &l);
	BOOST_CHECK_EQUAL(s2.getPath(), &l);

	// The service is registered in the path
	BOOST_CHECK_EQUAL(l.getServices().size(), 1);
	if(l.getServices().size() == 1)
	{
		BOOST_CHECK_EQUAL(*l.getServices().begin(), &s1);
	}
	BOOST_CHECK(l.isActive(today));

	// The service is registered in the line
	BOOST_CHECK_EQUAL(l.getCommercialLine()->getServices().size(), 1);
	if(l.getCommercialLine()->getServices().size() == 1)
	{
		BOOST_CHECK_EQUAL(*l.getCommercialLine()->getServices().begin(), &s1);
	}
	BOOST_CHECK_EQUAL(l.getCommercialLine()->getServices(s1.getServiceNumber()).size(), 1);
	if(l.getCommercialLine()->getServices(s1.getServiceNumber()).size() == 1)
	{
		BOOST_CHECK_EQUAL(*l.getCommercialLine()->getServices(s1.getServiceNumber()).begin(), &s1);
	}
	BOOST_CHECK(l.getCommercialLine()->getServices(s2.getServiceNumber()).empty());


	// The service #2 is now linked to the environment
	s2.link(Env::GetOfficialEnv(), true);

	// The path has a subline
	BOOST_CHECK_EQUAL(l.getSubLines().size(), 1);
	JourneyPatternCopy* sl(*l.getSubLines().begin());
	BOOST_CHECK_EQUAL(sl->getCommercialLine(), l.getCommercialLine());
	BOOST_CHECK_EQUAL(sl->getEdges().size(), l.getEdges().size());
	BOOST_CHECK_EQUAL(cl.getPaths().size(), 2);
	BOOST_CHECK(cl.getPaths().find(sl) != cl.getPaths().end());

	// Path of s2 is the new subline
	BOOST_CHECK_EQUAL(s1.getPath(), &l);
	BOOST_CHECK_EQUAL(s2.getPath(), sl);

	// The service is registered in the path
	BOOST_CHECK_EQUAL(l.getServices().size(), 1);
	BOOST_CHECK_EQUAL(sl->getServices().size(), 1);
	if(l.getServices().size() == 1)
	{
		BOOST_CHECK_EQUAL(*l.getServices().begin(), &s1);
	}
	if(sl->getServices().size() == 1)
	{
		BOOST_CHECK_EQUAL(*sl->getServices().begin(), &s2);
	}
	BOOST_CHECK(l.isActive(today));
	BOOST_CHECK(sl->isActive(today));

	// The service is registered in the line
	BOOST_CHECK_EQUAL(l.getCommercialLine()->getServices().size(), 2);
	BOOST_CHECK_EQUAL(l.getCommercialLine()->getServices(s1.getServiceNumber()).size(), 1);
	if(l.getCommercialLine()->getServices(s1.getServiceNumber()).size() == 1)
	{
		BOOST_CHECK_EQUAL(*l.getCommercialLine()->getServices(s1.getServiceNumber()).begin(), &s1);
	}
	BOOST_CHECK_EQUAL(l.getCommercialLine()->getServices(s2.getServiceNumber()).size(), 1);
	if(l.getCommercialLine()->getServices(s2.getServiceNumber()).size() == 1)
	{
		BOOST_CHECK_EQUAL(*l.getCommercialLine()->getServices(s2.getServiceNumber()).begin(), &s2);
	}

}
