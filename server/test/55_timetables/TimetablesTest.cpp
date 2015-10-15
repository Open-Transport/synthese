
/** TimetablesTest class implementation.
	@file TimetablesTest.cpp

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

#include "TimetablesTestData.inc.hpp"

#include <boost/test/auto_unit_test.hpp>

#include "CalendarTemplate.h"
#include "CalendarTemplateElement.h"
#include "Env.h"
#include "Timetable.h"

using namespace synthese::timetables;
using namespace synthese::pt;

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace synthese::calendar;
using namespace synthese::timetables;
using namespace synthese::util;
using namespace boost::gregorian;

using namespace synthese;
using namespace synthese::resa;
using namespace synthese::geography;
using namespace synthese::graph;
using namespace synthese::impex;
using namespace synthese::pt;
using namespace synthese::db;
using namespace synthese::vehicle;

using namespace geos::geom;

BOOST_AUTO_TEST_CASE (TimetablesTest)
{
	ScopedCoordinatesSystemUser scopedCoordinatesSystemUser;
	ScopedRegistrable<JourneyPattern> scopedJourneyPattern;
	ScopedRegistrable<CommercialLine> scopedCommercialLine;
	Env env;


	GeographyModule::PreInit();

	#ifndef SAVE
	#define SAVE(_class, object) do {} while(0)
	#endif

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Cities
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	City city54(1688849860511154ULL,"City54");
	SAVE(City, city54);
	City city95(1688849860511295ULL,"City95");
	SAVE(City, city95);
	City city38(1688849860530938ULL,"City38");
	SAVE(City, city38);



	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Stops
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Place 93
	StopArea place93(1970324837184593ULL,true,minutes(8));
	place93.setName("93");
	place93.setCity(&city54);
	city54.addIncludedPlace(place93);

	Coordinate cops931(521000.0,1847000.0);
	StopPoint ps931(
		3377699720880931ULL,
		"93/1",
		&place93,
		boost::shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops931))
	);
	SAVE(StopPoint, ps931);
	place93.addPhysicalStop(ps931);
	SAVE(StopArea, place93);


	// Place 94
	StopArea place94(1970324837184594ULL,false,minutes(8));
	place94.setName("94");
	place94.setCity(&city54);
	city54.addIncludedPlace(place94);

	Coordinate cops941(523000.0,1845000.0);
	StopPoint ps941(3377699720880941ULL, "94/1", &place94, boost::shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops931)));
	SAVE(StopPoint, ps941);
	place94.addPhysicalStop(ps941);
	SAVE(StopArea, place94);

	// Place 95
	StopArea place95(1970324837184595ULL,false,minutes(8));
	place95.setName("95");
	place95.setCity(&city95);
	city95.addIncludedPlace(place95);

	Coordinate cops951(523266.0,1845339.0);
	StopPoint ps951(3377699720880951ULL, "95/1", &place95, boost::shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops951)));
	SAVE(StopPoint, ps951);
	place95.addPhysicalStop(ps951);
	SAVE(StopArea, place95);

	// Place 96
	StopArea place96(1970324837184596ULL,false,minutes(8));
	place96.setName("96");
	place96.setCity(&city38);
	city38.addIncludedPlace(place96);

	Coordinate cops961(524000.0,1849500.0);
	StopPoint ps961(3377699720880961ULL, "96/1", &place96, boost::shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops961)));
	SAVE(StopPoint, ps961);
	place96.addPhysicalStop(ps961);
	SAVE(StopArea, place96);

	// Place 97
	StopArea place97(1970324837184597ULL,true,minutes(8));
	place97.setName("97");
	place97.setCity(&city38);
	city38.addIncludedPlace(place97);

	Coordinate cops971(526000,1845000);
	StopPoint ps971(3377699720880971ULL, "97/1", &place97, boost::shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops971)));
	SAVE(StopPoint, ps971);
	place97.addPhysicalStop(ps971);


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Networks
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	TransportNetwork n34(6192449487677434ULL,"Network34");
	SAVE(TransportNetwork, n34);



	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Use rules
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	PTUseRule ur53(2305843009213693953ULL);
	ur53.setName("Compulsory reservation");
	ur53.setReservationType(PTUseRule::RESERVATION_RULE_COMPULSORY);
	ur53.setOriginIsReference(false);
	ur53.setMinDelayMinutes(minutes(10));
	ur53.setMinDelayDays(days(0));
	ur53.setMaxDelayDays(days(60));
	ur53.setHourDeadLine(time_duration(23,59,0));
	ur53.setAccessCapacity(1);
	SAVE(PTUseRule, ur53);

	PTUseRule ur54(2305843009213693954ULL);
	ur54.setName("1 seat only");
	ur54.setReservationType(PTUseRule::RESERVATION_RULE_FORBIDDEN);
	ur54.setAccessCapacity(1);
	SAVE(PTUseRule, ur54);


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Reservation contact
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	ReservationContact rc05(5910974511914805ULL);
	rc05.set<PhoneExchangeNumber>("0800 929 929 (gratuit depuis un poste fixe) ou 05 34 25 33 75");
	rc05.set<PhoneExchangeOpeningHours>("entre 6h30 et 22h30");
	rc05.set<Description>("Test");
	SAVE(ReservationContact, rc05);

	OnlineReservationRule or33(13229323905400833ULL);
	or33.setReservationContact(&rc05);
	or33.setNeedsPhone(true);
	or33.setMaxSeats(1000);
	or33.setSenderName("Centrale de reservation RCS");
	or33.setSenderEMail("contact@rcsmobility.com");
	SAVE(OnlineReservationRule, or33);


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// RollingStock
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	RollingStock rs57(13792273858822157ULL);
	rs57.set<Name>("test");
	SAVE(RollingStock, rs57);


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Lines
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// JourneyPattern 92

	CommercialLine cl92(11821949021891592ULL);
	cl92.setParent(n34);
	cl92.setShortName("92");
	cl92.setStyle("vertfonce");
	{
		RuleUser::Rules r(RuleUser::GetEmptyRules());
		r[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
		r[USER_HANDICAPPED - USER_CLASS_CODE_OFFSET] = &ur54;
		cl92.setRules(r);
	}
	SAVE(CommercialLine, cl92);

	boost::shared_ptr<JourneyPattern> li92(new JourneyPattern(2533274790397692ULL, "92"));
	env.add(li92);
	li92->setTimetableName("92.1");
	li92->setRollingStock(&rs57);
	li92->setCommercialLine(&cl92);

	LineStop ls86(2814749767106586ULL, li92.get(), 0, true, false, 0, ps931);
	SAVE(LineStop, ls86);
	ls86.link(env);

	LineStop ls63(2814749767106563ULL, li92.get(), 1, true, true, 100, ps941);
	SAVE(LineStop, ls63);
	ls63.link(env);

	LineStop ls61(2814749767106561ULL, li92.get(), 2, true, true, 5500, ps951);
	SAVE(LineStop, ls61);
	ls61.link(env);

	LineStop ls62(2814749767106562ULL, li92.get(), 3, true, true, 6400, ps961);
	SAVE(LineStop, ls62);
	ls62.link(env);

	LineStop ls60(2814749767106560ULL, li92.get(), 4, false, true, 6500, ps971);
	SAVE(LineStop, ls60);
	ls60.link(env);

	vector<ScheduledService*> ss;

	ScheduledService ss01(4503599627370501ULL, "1", li92.get());
	{
		ScheduledService::Schedules a;
		ScheduledService::Schedules d;
		a.push_back(time_duration(0,0,0));
		d.push_back(time_duration(7,0,0));

		a.push_back(time_duration(7,5,0));
		d.push_back(time_duration(7,5,0));

		a.push_back(time_duration(7,10,0));
		d.push_back(time_duration(7,9,0));

		a.push_back(time_duration(7,15,0));
		d.push_back(time_duration(7,14,0));

		a.push_back(time_duration(7,20,0));
		d.push_back(time_duration(0,0,0));

		ss01.setDataSchedules(d,a);
	}
	ss01.setActive(day_clock::local_day());
	ss01.setActive(day_clock::local_day() + days(1));
	ss01.setActive(day_clock::local_day() + days(2));
	SAVE(ScheduledService, ss01);
	li92->addService(ss01, true);
	ss.push_back(&ss01);


	CalendarTemplate baseCalendarAllDays;
	CalendarTemplateElement element;
	element.setMinDate(day_clock::local_day());
	element.setMaxDate(day_clock::local_day() + days(2));
	element.setRank(0);
	element.setOperation(CalendarTemplateElement::ADD);
	element.setCalendar(&baseCalendarAllDays);
	baseCalendarAllDays.addElement(element);

	Timetable tt1;
	TimetableGenerator::Rows rows;
	TimetableRow row0;
	row0.setRank(0);
	row0.setPlace(&place93);
	rows.push_back(row0);
	TimetableRow row1;
	row1.setRank(1);
	row1.setPlace(&place94);
	rows.push_back(row1);
	TimetableRow row2;
	row2.setRank(2);
	row2.setPlace(&place95);
	rows.push_back(row2);
	TimetableRow row3;
	row3.setRank(3);
	row3.setPlace(&place96);
	rows.push_back(row3);
	TimetableRow row4;
	row4.setRank(4);
	row4.setPlace(&place97);
	rows.push_back(row4);
	tt1.setRows(rows);

	BOOST_CHECK(tt1.isGenerable());

	tt1.set<BaseCalendar>(baseCalendarAllDays);
	BOOST_CHECK(tt1.isGenerable());
	tt1.setContentType(Timetable::CALENDAR);

	{
		std::auto_ptr<TimetableGenerator> generator(
			tt1.getGenerator(
			env
			)	);
		boost::shared_ptr<TimetableResult::Warnings> warnings(new TimetableResult::Warnings);

		TimetableResult result(
			generator->build(
			false,
			warnings
			)	);

		BOOST_CHECK_EQUAL(result.getColumns().size(), 1ULL);
		if(result.getColumns().size() > 0)
		{
			const TimetableColumn& column(*result.getColumns().begin());
			for(size_t i(0); i<column.getContent().size(); ++i)
			{
				BOOST_CHECK_EQUAL(
					to_simple_string(column.getContent().at(i).second),
					to_simple_string(i+1 == column.getContent().size() ? ss01.getArrivalSchedule(false, i) : ss01.getDepartureSchedule(false, i))
				);
			}
		}
	}


	// A A A A sequence
	ScheduledService ss02(4503599627370502ULL, "2", li92.get());
	{
		ScheduledService::Schedules a;
		ScheduledService::Schedules d;
		a.push_back(time_duration(0,0,0));
		d.push_back(time_duration(8,0,0));

		a.push_back(time_duration(8,5,0));
		d.push_back(time_duration(8,5,0));

		a.push_back(time_duration(8,10,0));
		d.push_back(time_duration(8,9,0));

		a.push_back(time_duration(8,15,0));
		d.push_back(time_duration(8,14,0));

		a.push_back(time_duration(8,20,0));
		d.push_back(time_duration(0,0,0));

		ss02.setDataSchedules(d,a);
	}
	ss02.setActive(day_clock::local_day());
	ss02.setActive(day_clock::local_day() + days(1));
	ss02.setActive(day_clock::local_day() + days(2));
	SAVE(ScheduledService, ss02);
	li92->addService(ss02, true);
	ss.push_back(&ss02);


	ScheduledService ss03(4503599627370503ULL, "3", li92.get());
	{
		ScheduledService::Schedules a;
		ScheduledService::Schedules d;
		a.push_back(time_duration(0,0,0));
		d.push_back(time_duration(9,0,0));

		a.push_back(time_duration(9,5,0));
		d.push_back(time_duration(9,5,0));

		a.push_back(time_duration(9,10,0));
		d.push_back(time_duration(9,9,0));

		a.push_back(time_duration(9,15,0));
		d.push_back(time_duration(9,14,0));

		a.push_back(time_duration(9,20,0));
		d.push_back(time_duration(0,0,0));

		ss03.setDataSchedules(d,a);
	}
	ss03.setActive(day_clock::local_day());
	ss03.setActive(day_clock::local_day() + days(1));
	ss03.setActive(day_clock::local_day() + days(2));
	SAVE(ScheduledService, ss03);
	li92->addService(ss03, true);
	ss.push_back(&ss03);

	ScheduledService ss04(45035996273705054ULL, "4", li92.get());
	{
		ScheduledService::Schedules a;
		ScheduledService::Schedules d;
		a.push_back(time_duration(0,0,0));
		d.push_back(time_duration(10,0,0));

		a.push_back(time_duration(10,5,0));
		d.push_back(time_duration(10,5,0));

		a.push_back(time_duration(10,10,0));
		d.push_back(time_duration(10,9,0));

		a.push_back(time_duration(10,15,0));
		d.push_back(time_duration(10,14,0));

		a.push_back(time_duration(10,20,0));
		d.push_back(time_duration(0,0,0));

		ss04.setDataSchedules(d,a);
	}
	ss04.setActive(day_clock::local_day());
	ss04.setActive(day_clock::local_day() + days(1));
	ss04.setActive(day_clock::local_day() + days(2));
	SAVE(ScheduledService, ss04);
	li92->addService(ss04, true);
	ss.push_back(&ss04);

	{ // Without compression
		std::auto_ptr<TimetableGenerator> generator(
			tt1.getGenerator(
				env
		)	);
		boost::shared_ptr<TimetableResult::Warnings> warnings(new TimetableResult::Warnings);

		TimetableResult result(
			generator->build(
				false,
				warnings
		)	);

		BOOST_CHECK_EQUAL(result.getColumns().size(), ss.size());
		for(size_t c(0); c<result.getColumns().size(); ++c)
		{
			const TimetableColumn& column(*(result.getColumns().begin() + c));
			for(size_t i(0); i<column.getContent().size(); ++i)
			{
				BOOST_CHECK_EQUAL(
					to_simple_string(column.getContent().at(i).second),
					to_simple_string(i+1 == column.getContent().size() ? ss[c]->getArrivalSchedule(false, i) : ss[c]->getDepartureSchedule(false, i))
				);
			}
		}
	}

	{ // With compression : A x 4
		tt1.set<Compression>(true);
		std::auto_ptr<TimetableGenerator> generator(
			tt1.getGenerator(
				env
		)	);
		boost::shared_ptr<TimetableResult::Warnings> warnings(new TimetableResult::Warnings);

		TimetableResult result(
			generator->build(
				false,
				warnings
		)	);

		BOOST_CHECK_EQUAL(result.getColumns().size(), 1ULL);
		if(result.getColumns().size() > 0)
		{
			const TimetableColumn& column(*result.getColumns().begin());
			BOOST_CHECK(column.isCompression());
			if(column.isCompression())
			{
				BOOST_CHECK_EQUAL(column.getCompressionRank(), 0);
				BOOST_CHECK_EQUAL(column.getCompressionRepeated(), 3);
			}
			for(size_t i(0); i<column.getContent().size(); ++i)
			{
				BOOST_CHECK_EQUAL(
					to_simple_string(column.getContent().at(i).second),
					to_simple_string(i+1 == column.getContent().size() ? ss01.getArrivalSchedule(false, i) : ss01.getDepartureSchedule(false, i))
				);
			}
		}
	}
	

	// A B A B A B A B A sequence -> AB x 4 + A

	ScheduledService ss05(4503599627370505ULL, "5", li92.get());
	{
		ScheduledService::Schedules a;
		ScheduledService::Schedules d;
		a.push_back(time_duration(0,0,0));
		d.push_back(time_duration(7,30,0));

		a.push_back(time_duration(7,35,0));
		d.push_back(time_duration(7,35,0));

		a.push_back(time_duration(7,40,0));
		d.push_back(time_duration(7,39,0));

		a.push_back(time_duration(7,45,0));
		d.push_back(time_duration(7,44,0));

		a.push_back(time_duration(7,50,0));
		d.push_back(time_duration(0,0,0));

		ss05.setDataSchedules(d,a);
	}
	ss05.setActive(day_clock::local_day());
	ss05.setActive(day_clock::local_day() + days(1));
	ss05.setActive(day_clock::local_day() + days(2));
	SAVE(ScheduledService, ss05);
	li92->addService(ss05, true);

	ScheduledService ss06(4503599627370506ULL, "6", li92.get());
	{
		ScheduledService::Schedules a;
		ScheduledService::Schedules d;
		a.push_back(time_duration(0,0,0));
		d.push_back(time_duration(8,30,0));

		a.push_back(time_duration(8,35,0));
		d.push_back(time_duration(8,35,0));

		a.push_back(time_duration(8,40,0));
		d.push_back(time_duration(8,39,0));

		a.push_back(time_duration(8,45,0));
		d.push_back(time_duration(8,44,0));

		a.push_back(time_duration(8,50,0));
		d.push_back(time_duration(0,0,0));

		ss06.setDataSchedules(d,a);
	}
	ss06.setActive(day_clock::local_day());
	ss06.setActive(day_clock::local_day() + days(1));
	ss06.setActive(day_clock::local_day() + days(2));
	SAVE(ScheduledService, ss06);
	li92->addService(ss06, true);

	ScheduledService ss07(4503599627370506ULL, "7", li92.get());
	{
		ScheduledService::Schedules a;
		ScheduledService::Schedules d;
		a.push_back(time_duration(0,0,0));
		d.push_back(time_duration(9,30,0));

		a.push_back(time_duration(9,35,0));
		d.push_back(time_duration(9,35,0));

		a.push_back(time_duration(9,40,0));
		d.push_back(time_duration(9,39,0));

		a.push_back(time_duration(9,45,0));
		d.push_back(time_duration(9,44,0));

		a.push_back(time_duration(9,50,0));
		d.push_back(time_duration(0,0,0));

		ss07.setDataSchedules(d,a);
	}
	ss07.setActive(day_clock::local_day());
	ss07.setActive(day_clock::local_day() + days(1));
	ss07.setActive(day_clock::local_day() + days(2));
	SAVE(ScheduledService, ss07);
	li92->addService(ss07, true);


	ScheduledService ss08(45035996273705058ULL, "8", li92.get());
	{
		ScheduledService::Schedules a;
		ScheduledService::Schedules d;
		a.push_back(time_duration(0,0,0));
		d.push_back(time_duration(10,30,0));

		a.push_back(time_duration(10,35,0));
		d.push_back(time_duration(10,35,0));

		a.push_back(time_duration(10,40,0));
		d.push_back(time_duration(10,39,0));

		a.push_back(time_duration(10,45,0));
		d.push_back(time_duration(10,44,0));

		a.push_back(time_duration(10,50,0));
		d.push_back(time_duration(0,0,0));

		ss08.setDataSchedules(d,a);
	}
	ss08.setActive(day_clock::local_day());
	ss08.setActive(day_clock::local_day() + days(1));
	ss08.setActive(day_clock::local_day() + days(2));
	SAVE(ScheduledService, ss08);
	li92->addService(ss08, true);


	ScheduledService ss09(45035996273705059ULL, "9", li92.get());
	{
		ScheduledService::Schedules a;
		ScheduledService::Schedules d;
		a.push_back(time_duration(0,0,0));
		d.push_back(time_duration(11,0,0));

		a.push_back(time_duration(11,5,0));
		d.push_back(time_duration(11,5,0));

		a.push_back(time_duration(11,10,0));
		d.push_back(time_duration(11,9,0));

		a.push_back(time_duration(11,15,0));
		d.push_back(time_duration(11,14,0));

		a.push_back(time_duration(11,20,0));
		d.push_back(time_duration(0,0,0));

		ss09.setDataSchedules(d,a);
	}
	ss09.setActive(day_clock::local_day());
	ss09.setActive(day_clock::local_day() + days(1));
	ss09.setActive(day_clock::local_day() + days(2));
	SAVE(ScheduledService, ss09);
	li92->addService(ss09, true);
	ss.push_back(&ss09);


	{
		std::auto_ptr<TimetableGenerator> generator(
			tt1.getGenerator(
				env
		)	);
		boost::shared_ptr<TimetableResult::Warnings> warnings(new TimetableResult::Warnings);

		TimetableResult result(
			generator->build(
				false,
				warnings
		)	);

		BOOST_CHECK_EQUAL(result.getColumns().size(), 3);
		if(result.getColumns().size() >= 1)
		{
			const TimetableColumn& column(*result.getColumns().begin());
			BOOST_CHECK(column.isCompression());
			if(column.isCompression())
			{
				BOOST_CHECK_EQUAL(column.getCompressionRank(), 0);
				BOOST_CHECK_EQUAL(column.getCompressionRepeated(), 3);
			}
			for(size_t i(0); i<column.getContent().size(); ++i)
			{
				BOOST_CHECK_EQUAL(
					to_simple_string(column.getContent().at(i).second),
					to_simple_string(i+1 == column.getContent().size() ? ss01.getArrivalSchedule(false, i) : ss01.getDepartureSchedule(false, i))
				);
			}
		}
		if(result.getColumns().size() >= 2)
		{
			const TimetableColumn& column(*(result.getColumns().begin() + 1));
			BOOST_CHECK(column.isCompression());
			if(column.isCompression())
			{
				BOOST_CHECK_EQUAL(column.getCompressionRank(), 1);
				BOOST_CHECK_EQUAL(column.getCompressionRepeated(), 3);
			}
			for(size_t i(0); i<column.getContent().size(); ++i)
			{
				BOOST_CHECK_EQUAL(
					to_simple_string(column.getContent().at(i).second),
					to_simple_string(i+1 == column.getContent().size() ? ss05.getArrivalSchedule(false, i) : ss05.getDepartureSchedule(false, i))
				);
			}
		}
		if(result.getColumns().size() >= 3)
		{
			const TimetableColumn& column(*(result.getColumns().begin() + 2));
			BOOST_CHECK(!column.isCompression());
			for(size_t i(0); i<column.getContent().size(); ++i)
			{
				BOOST_CHECK_EQUAL(
					to_simple_string(column.getContent().at(i).second),
					to_simple_string(i+1 == column.getContent().size() ? ss09.getArrivalSchedule(false, i) : ss09.getDepartureSchedule(false, i))
				);
			}
		}
	}

	// A B C A B C A B C A B C A sequence -> ABC x 4 + A

	ScheduledService ss10(4503599627370510ULL, "10", li92.get());
	{
		ScheduledService::Schedules a;
		ScheduledService::Schedules d;
		a.push_back(time_duration(0,0,0));
		d.push_back(time_duration(7,40,0));

		a.push_back(time_duration(7,45,0));
		d.push_back(time_duration(7,45,0));

		a.push_back(time_duration(7,50,0));
		d.push_back(time_duration(7,49,0));

		a.push_back(time_duration(7,55,0));
		d.push_back(time_duration(7,54,0));

		a.push_back(time_duration(8,00,0));
		d.push_back(time_duration(0,0,0));

		ss10.setDataSchedules(d,a);
	}
	ss10.setActive(day_clock::local_day());
	ss10.setActive(day_clock::local_day() + days(1));
	ss10.setActive(day_clock::local_day() + days(2));
	SAVE(ScheduledService, ss10);
	li92->addService(ss10, true);

	ScheduledService ss11(4503599627370511ULL, "11", li92.get());
	{
		ScheduledService::Schedules a;
		ScheduledService::Schedules d;
		a.push_back(time_duration(0,0,0));
		d.push_back(time_duration(8,40,0));

		a.push_back(time_duration(8,45,0));
		d.push_back(time_duration(8,45,0));

		a.push_back(time_duration(8,50,0));
		d.push_back(time_duration(8,49,0));

		a.push_back(time_duration(8,55,0));
		d.push_back(time_duration(8,54,0));

		a.push_back(time_duration(9,00,0));
		d.push_back(time_duration(0,0,0));

		ss11.setDataSchedules(d,a);
	}
	ss11.setActive(day_clock::local_day());
	ss11.setActive(day_clock::local_day() + days(1));
	ss11.setActive(day_clock::local_day() + days(2));
	SAVE(ScheduledService, ss11);
	li92->addService(ss11, true);

	ScheduledService ss13(4503599627370513ULL, "13", li92.get());
	{
		ScheduledService::Schedules a;
		ScheduledService::Schedules d;
		a.push_back(time_duration(0,0,0));
		d.push_back(time_duration(9,40,0));

		a.push_back(time_duration(9,45,0));
		d.push_back(time_duration(9,45,0));

		a.push_back(time_duration(9,50,0));
		d.push_back(time_duration(9,49,0));

		a.push_back(time_duration(9,55,0));
		d.push_back(time_duration(9,54,0));

		a.push_back(time_duration(10,0,0));
		d.push_back(time_duration(0,0,0));

		ss13.setDataSchedules(d,a);
	}
	ss13.setActive(day_clock::local_day());
	ss13.setActive(day_clock::local_day() + days(1));
	ss13.setActive(day_clock::local_day() + days(2));
	SAVE(ScheduledService, ss13);
	li92->addService(ss13, true);


	ScheduledService ss12(4503599627370512ULL, "12", li92.get());
	{
		ScheduledService::Schedules a;
		ScheduledService::Schedules d;
		a.push_back(time_duration(0,0,0));
		d.push_back(time_duration(10,40,0));

		a.push_back(time_duration(10,45,0));
		d.push_back(time_duration(10,45,0));

		a.push_back(time_duration(10,50,0));
		d.push_back(time_duration(10,49,0));

		a.push_back(time_duration(10,55,0));
		d.push_back(time_duration(10,54,0));

		a.push_back(time_duration(11,0,0));
		d.push_back(time_duration(0,0,0));

		ss12.setDataSchedules(d,a);
	}
	ss12.setActive(day_clock::local_day());
	ss12.setActive(day_clock::local_day() + days(1));
	ss12.setActive(day_clock::local_day() + days(2));
	SAVE(ScheduledService, ss12);
	li92->addService(ss12, true);



	{
		std::auto_ptr<TimetableGenerator> generator(
			tt1.getGenerator(
				env
		)	);
		boost::shared_ptr<TimetableResult::Warnings> warnings(new TimetableResult::Warnings);

		TimetableResult result(
			generator->build(
				false,
				warnings
		)	);

		BOOST_CHECK_EQUAL(result.getColumns().size(), 4);
		if(result.getColumns().size() >= 1)
		{
			const TimetableColumn& column(*result.getColumns().begin());
			BOOST_CHECK(column.isCompression());
			if(column.isCompression())
			{
				BOOST_CHECK_EQUAL(column.getCompressionRank(), 0);
				BOOST_CHECK_EQUAL(column.getCompressionRepeated(), 3);
			}
			for(size_t i(0); i<column.getContent().size(); ++i)
			{
				BOOST_CHECK_EQUAL(
					to_simple_string(column.getContent().at(i).second),
					to_simple_string(i+1 == column.getContent().size() ? ss01.getArrivalSchedule(false, i) : ss01.getDepartureSchedule(false, i))
				);
			}
		}
		if(result.getColumns().size() >= 2)
		{
			const TimetableColumn& column(*(result.getColumns().begin() + 1));
			BOOST_CHECK(column.isCompression());
			if(column.isCompression())
			{
				BOOST_CHECK_EQUAL(column.getCompressionRank(), 1);
				BOOST_CHECK_EQUAL(column.getCompressionRepeated(), 3);
			}
			for(size_t i(0); i<column.getContent().size(); ++i)
			{
				BOOST_CHECK_EQUAL(
					to_simple_string(column.getContent().at(i).second),
					to_simple_string(i+1 == column.getContent().size() ? ss05.getArrivalSchedule(false, i) : ss05.getDepartureSchedule(false, i))
				);
			}
		}
		if(result.getColumns().size() >= 3)
		{
			const TimetableColumn& column(*(result.getColumns().begin() + 2));
			BOOST_CHECK(column.isCompression());
			if(column.isCompression())
			{
				BOOST_CHECK_EQUAL(column.getCompressionRank(), 2);
				BOOST_CHECK_EQUAL(column.getCompressionRepeated(), 3);
			}
			for(size_t i(0); i<column.getContent().size(); ++i)
			{
				BOOST_CHECK_EQUAL(
					to_simple_string(column.getContent().at(i).second),
					to_simple_string(i+1 == column.getContent().size() ? ss10.getArrivalSchedule(false, i) : ss10.getDepartureSchedule(false, i))
				);
			}
		}
		if(result.getColumns().size() >= 4)
		{
			const TimetableColumn& column(*(result.getColumns().begin() + 3));
			BOOST_CHECK(!column.isCompression());
			for(size_t i(0); i<column.getContent().size(); ++i)
			{
				BOOST_CHECK_EQUAL(
					to_simple_string(column.getContent().at(i).second),
					to_simple_string(i+1 == column.getContent().size() ? ss09.getArrivalSchedule(false, i) : ss09.getDepartureSchedule(false, i))
				);
			}
		}
	}

}
