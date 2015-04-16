
/** Timetables test data set.
	@file TimetablesTestData.hpp

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

using namespace boost::posix_time;
using namespace boost::gregorian;

using namespace synthese;
using namespace synthese::resa;
using namespace synthese::geography;
using namespace synthese::util;
using namespace synthese::graph;
using namespace synthese::impex;
using namespace synthese::pt;
using namespace synthese::db;
using namespace synthese::vehicle;

using namespace geos::geom;
using namespace boost;

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
	shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops931))
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
StopPoint ps941(3377699720880941ULL,"94/1", &place94, shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops931)));
SAVE(StopPoint, ps941);
place94.addPhysicalStop(ps941);
SAVE(StopArea, place94);

// Place 95
StopArea place95(1970324837184595ULL,false,minutes(8));
place95.setName("95");
place95.setCity(&city95);
city95.addIncludedPlace(place95);

Coordinate cops951(523266.0,1845339.0);
StopPoint ps951(3377699720880951ULL,"95/1", &place95, shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops951)));
SAVE(StopPoint, ps951);
place95.addPhysicalStop(ps951);
SAVE(StopArea, place95);

// Place 96
StopArea place96(1970324837184596ULL,false,minutes(8));
place96.setName("96");
place96.setCity(&city38);
city38.addIncludedPlace(place96);

Coordinate cops961(524000.0,1849500.0);
StopPoint ps961(3377699720880961ULL,"96/1", &place96, shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops961)));
SAVE(StopPoint, ps961);
place96.addPhysicalStop(ps961);
SAVE(StopArea, place96);

// Place 97
StopArea place97(1970324837184597ULL,true,minutes(8));
place97.setName("97");
place97.setCity(&city38);
city38.addIncludedPlace(place97);

Coordinate cops971(526000,1845000);
StopPoint ps971(3377699720880971ULL,"97/1", &place97, shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops971)));
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
rs57.setName("test");
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

DesignatedLinePhysicalStop ls86(2814749767106586ULL, li92.get(), 0, true, false, 0, &ps931);
SAVE(LineStop, ls86);
li92->addEdge(ls86);

DesignatedLinePhysicalStop ls63(2814749767106563ULL, li92.get(), 1, true, true, 100, &ps941);
SAVE(LineStop, ls63);
li92->addEdge(ls63);

DesignatedLinePhysicalStop ls61(2814749767106561ULL, li92.get(), 2, true, true, 5500, &ps951);
SAVE(LineStop, ls61);
li92->addEdge(ls61);

DesignatedLinePhysicalStop ls62(2814749767106562ULL, li92.get(), 3, true, true, 6400, &ps961);
SAVE(LineStop, ls62);
li92->addEdge(ls62);

DesignatedLinePhysicalStop ls60(2814749767106560ULL, li92.get(), 4, false, true, 6500, &ps971);
SAVE(LineStop, ls60);
li92->addEdge(ls60);

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


