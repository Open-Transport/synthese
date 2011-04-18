
/** Route planner test data set.
	@file RoutePlannerTestData.hpp

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

using namespace boost::posix_time;
using namespace boost::gregorian;

using namespace synthese;
using namespace synthese::resa;
using namespace synthese::geography;
using namespace synthese::road;
using namespace synthese::util;
using namespace synthese::graph;
using namespace synthese::impex;
using namespace synthese::pt;
using namespace synthese::db;

using namespace geos::geom;
using namespace boost;

GeographyModule::PreInit();
DBModule::PreInit();

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Cities
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

City city54(1688849860511154,"City54");
City city95(1688849860511295,"City95");
City city38(1688849860530938,"City38");

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Roads
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Crossings
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Coordinate co10(520000.0,1844000.0);
Crossing c10(12103423998558210, shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(co10)));

Coordinate co74(523000.0,1844000.0);
Crossing c74(12103423998558274, shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(co74)));

Coordinate co86(523000.0,1845342.0);
Crossing c86(12103423998558286, shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(co86)));

Coordinate co88(523000.0,1845342.0);
Crossing c88(12103423998558288, shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(co88)));

Coordinate co89(521000.0,1845000.0);
Crossing c89(12103423998558289, shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(co89)));

Coordinate co90(528000.0,1844000.0);
Crossing c90(12103423998558290, shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(co90)));

Coordinate co91(525000.0,1844000.0);
Crossing c91(12103423998558291, shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(co91)));

Coordinate co92(523263.0,1845331.0);
Crossing c92(12103423998558292, shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(co92)));

Coordinate co93(523000.0,1845342.0);
Crossing c93(12103423998558293, shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(co93)));

Coordinate co94(523000.0,1845342.0);
Crossing c94(12103423998558294, shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(co94)));

Coordinate co96(526000.0,1847000.0);
Crossing c96(12103423998558296, shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(co96)));

Coordinate co97(523266.0,1845341.0);
Crossing c97(12103423998558297, shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(co97)));

Coordinate co98(523000.0,1845342.0);
Crossing c98(12103423998558298, shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(co98)));

Coordinate co99(526000.0,1845000.0);
Crossing c99(12103423998558299, shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(co99)));

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Roads
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Road 40

RoadPlace rp40(16888498602639440);
rp40.setName("40");
rp40.setCity(&city54);

MainRoadPart ro40(4222124650659840);
ro40.setRoadPlace(rp40);

MainRoadChunk rc86_40(3940649673949187, &c86, 0, &ro40, 0);
ro40.addRoadChunk(rc86_40);

MainRoadChunk rc89(3940649673949188, &c89, 1, &ro40, 200);
ro40.addRoadChunk(rc89);

// Road 41

RoadPlace rp41(16888498602639441);
rp41.setName("41");
rp41.setCity(&city54);

MainRoadPart ro41(4222124650659841);
ro41.setRoadPlace(rp41);

MainRoadChunk rc88_41(3940649673949188, &c88, 0, &ro41, 0);
ro41.addRoadChunk(rc88_41);

MainRoadChunk rc86_41(3940649673949186, &c86, 1, &ro41, 300);
ro41.addRoadChunk(rc86_41);

MainRoadChunk rc74(3940649673949174, &c74, 2, &ro41, 750);
ro41.addRoadChunk(rc74);

MainRoadChunk rc93_41(3940649673949193, &c93, 3, &ro41, 1700);
ro41.addRoadChunk(rc93_41);


// Road 42

RoadPlace rp42(16888498602639442);
rp42.setName("42");
rp42.setCity(&city38);

MainRoadPart ro42(4222124650659842);
ro42.setRoadPlace(rp42);

MainRoadChunk rc98_42(3940649673949298, &c98, 0, &ro42, 0);
ro42.addRoadChunk(rc98_42);

MainRoadChunk rc97(3940649673949197, &c97, 1, &ro42, 200);
ro42.addRoadChunk(rc97);

MainRoadPart ro44(4222124650659844);
ro44.setRoadPlace(rp42);

MainRoadChunk rc98_44(3940649673949498, &c98, 0, &ro44, 0);
ro44.addRoadChunk(rc98_44);

MainRoadChunk rc93(3940649673949193, &c93, 1, &ro44, 800);
ro44.addRoadChunk(rc93);

MainRoadChunk rc94(3940649673949194, &c94, 2, &ro44, 1600);
ro44.addRoadChunk(rc94);


// Road 43

RoadPlace rp43(16888498602639443);
rp43.setName("43");
rp43.setCity(&city38);

MainRoadPart ro43(4222124650659843);
ro43.setRoadPlace(rp43);

MainRoadChunk rc93_43(3940649673949393, &c93, 0, &ro43, 0);
ro43.addRoadChunk(rc93_43);

MainRoadChunk rc96(3940649673949396, &c96, 1, &ro43, 100);
ro43.addRoadChunk(rc96);

MainRoadChunk rc99(3940649673949199, &c99, 2, &ro43, 150);
ro43.addRoadChunk(rc99);


// Road 45

RoadPlace rp45(16888498602639445);
rp45.setName("45");
rp45.setCity(&city54);

MainRoadPart ro45(4222124650659845);
ro45.setRoadPlace(rp45);

MainRoadChunk rc86_45(3940649673949546, &c86, 0, &ro45, 0);
ro45.addRoadChunk(rc86_45);

MainRoadChunk rc10(3940649673949410, &c10, 1, &ro45, 400);
ro45.addRoadChunk(rc10);


// Road 46

RoadPlace rp46(16888498602639446);
rp46.setName("46");
rp46.setCity(&city38);

MainRoadPart ro46(4222124650659846);
ro46.setRoadPlace(rp46);

MainRoadChunk rc93_46(3940649673949693, &c93, 0, &ro46, 0);
ro46.addRoadChunk(rc93_46);

MainRoadChunk rc91(3940649673949189, &c91, 1, &ro46, 400);
ro46.addRoadChunk(rc91);

MainRoadChunk rc90(3940649673949190, &c90, 2, &ro46, 2000);
ro46.addRoadChunk(rc90);


// Road 47

RoadPlace rp47(16888498602639447);
rp47.setName("47");
rp47.setCity(&city54);

MainRoadPart ro47(4222124650659847);
ro47.setRoadPlace(rp47);

MainRoadChunk rc88_47(3940649673949788, &c88, 0, &ro47, 0);
ro47.addRoadChunk(rc88_47);

MainRoadChunk rc92(3940649673949292, &c92, 1, &ro47, 200);
ro47.addRoadChunk(rc92);


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Places
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Place 93
StopArea place93(1970324837184593,true,minutes(8));
place93.setName("93");
place93.setCity(&city54);
place93.setAllowedConnection(true);
city54.addIncludedPlace(&place93);
place93.addTransferDelay(562949953421389,3377699720880573,minutes(0));

Coordinate cops73(521000.0,1847000.0);
StopPoint ps73(
	3377699720880573,
	"93/73",
	&place93,
	shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops73))
);
ps73.setProjectedPoint(Address(rc89,0));
//ps73.setAllsetCodeBySource("CTP:StopArea:SPOCE87446179");
place93.addPhysicalStop(ps73);


// Place 94
StopArea place94(1970324837184594,false,minutes(8));
place94.setName("94");
place94.setCity(&city54);
place94.setAllowedConnection(true);
place94.addTransferDelay(3377699720880574,3377699720880575,minutes(11));
place94.addTransferDelay(3377699720880575,3377699720880574,minutes(12));
place94.addTransferDelay(3377699720880574,562949953421374,minutes(0));
place94.addTransferDelay(562949953421374,3377699720880574,minutes(0));
place94.addTransferDelay(3377699720880575,562949953421374,minutes(0));
place94.addTransferDelay(562949953421374,3377699720880575,minutes(0));

Coordinate cops75(523000.0,1845000.0);
StopPoint ps75(3377699720880575,"94/75", &place94, shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops75)));
//ps75.setCodeBySource("CTP:StopArea:SPOCE87353573");
ps75.setProjectedPoint(Address(rc74,0));
place94.addPhysicalStop(ps75);

// Place 95
StopArea place95(1970324837184595,false,minutes(8));
place95.setName("95");
place95.setCity(&city95);
place95.setAllowedConnection(false);
place95.addTransferDelay(562949953421397,377699720880576,minutes(0));

Coordinate cops76(523266.0,1845339.0);
StopPoint ps76(3377699720880576,"95/76", &place95, shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops76)));
//ps76.setCodeBySource("CTP:StopArea:SPOCE87611939");
ps76.setProjectedPoint(Address(rc97,0));
place95.addPhysicalStop(ps76);

// Place 96
StopArea place96(1970324837184596,false,minutes(8));
place96.setName("96");
place96.setCity(&city38);
place96.setAllowedConnection(false);

Coordinate cops77(524000.0,1849500.0);
StopPoint ps77(3377699720880577,"96/77", &place96, shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops77)));
place96.addPhysicalStop(ps77);

Coordinate cops78(524000.0,1849500.0);
StopPoint ps78(3377699720880578,"96/78", &place96, shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops78)));
place96.addPhysicalStop(ps78);

// Place 97
StopArea place97(1970324837184597,true,minutes(8));
place97.setName("97");
place97.setCity(&city38);
place97.setAllowedConnection(true);
city38.addIncludedPlace(&place97);
place97.addTransferDelay(562949953421396,3377699720880580,minutes(0));
place97.addTransferDelay(562949953421399,3377699720880579,minutes(0));
place97.addTransferDelay(3377699720880580,562949953421396,minutes(0));
place97.addTransferDelay(3377699720880579,562949953421399,minutes(0));

Coordinate cops79(526000,1845000);
StopPoint ps79(3377699720880579,"97/79", &place97, shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops79)));
//ps79.setCodeBySource("CTP:StopArea:SPOCE87353599");
ps79.setProjectedPoint(Address(rc99, 0));
place97.addPhysicalStop(ps79);

Coordinate cops80(526000.0,1847000.0);
StopPoint ps80(3377699720880580,"97/80", &place97, shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops80)));
ps80.setProjectedPoint(Address(rc96,0));
place97.addPhysicalStop(ps80);


// Place 98
StopArea place98(1970324837184598,false,minutes(8));
place98.setName("98");
place98.setCity(&city38);
place98.setAllowedConnection(true);
/*'562949953421391:3377699720880581:0,562949953421391:3377699720880582:0,3377699720880581:562949953421391:0,3377699720880582:562949953421391:0'*/

Coordinate cops81(525000.0,1844000.0);
StopPoint ps81(3377699720880581,"98/81", &place98, shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops81)));
//ps81.setCodeBySource("CTP:StopArea:SPOCE87353581");
ps81.setProjectedPoint(Address(rc91,0));
place98.addPhysicalStop(ps81);

Coordinate cops82(525000.0,1844000.0);
StopPoint ps82(3377699720880582,"98/82", &place98,shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops82)));
place98.addPhysicalStop(ps82);


// Place 99
StopArea place99(1970324837184599,true,minutes(8));
place99.setName("99");
place99.setCity(&city38);
// connection type 5
city38.addIncludedPlace(&place99);
place99.addTransferDelay(3377699720880585,562949953421390,minutes(0));
place99.addTransferDelay(562949953421390,3377699720880585,minutes(0));

Coordinate cops83(528000.0,1849000.0);
StopPoint ps83(3377699720880583,"99/83", &place99,shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops83)));
//ps83.setCodeBySource("CTP:StopArea:SPOCE87611467");
ps83.setProjectedPoint(Address(rc94,0));
place99.addPhysicalStop(ps83);

Coordinate cops84(528000.0,1845000.0);
StopPoint ps84(3377699720880584,"99/84", &place99,shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops84)));
ps84.setProjectedPoint(Address(rc90,0));
place99.addPhysicalStop(ps84);

Coordinate cops85(528000.0,1847000.0);
StopPoint ps85(3377699720880585,"99/85", &place99,shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops85)));
place99.addPhysicalStop(ps85);


// Place 05
StopArea place05(1970324837184605, true, minutes(8));
place05.setName("05");
place05.setCity(&city54);
place05.setAllowedConnection(true);
place05.addTransferDelay(562949953421410,3377699720880586,minutes(0));

Coordinate cops86(520000,1845000.0);
StopPoint ps86(3377699720880586,"05/86", &place05,shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops86)));
ps86.setProjectedPoint(Address(rc10,0));
place05.addPhysicalStop(ps86);


// Place 06
StopArea place06(1970324837184606, true, minutes(8));
place06.setName("06");
place06.setCity(&city54);
place06.setAllowedConnection(true);

Coordinate cops06(526000,1852000.0);
StopPoint ps06(3377699720880606,"06", &place06,shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops06)));
place06.addPhysicalStop(ps06);

// Place 07
StopArea place07(1970324837184607, true, minutes(8));
place07.setName("07");
place07.setCity(&city54);
place07.setAllowedConnection(true);

Coordinate cops07(523000,1852000.0);
StopPoint ps07(3377699720880607,"07", &place07,shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops07)));
place07.addPhysicalStop(ps07);



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Networks
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TransportNetwork n34(6192449487677434,"Network34");

TransportNetwork n37(6192449487677437,"Network37");


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Use rules
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PTUseRule ur53(2305843009213693953);
ur53.setName("Compulsory reservation");
ur53.setReservationType(PTUseRule::RESERVATION_RULE_MIXED_BY_DEPARTURE_PLACE);
ur53.setOriginIsReference(false);
ur53.setMinDelayMinutes(minutes(10));
ur53.setMinDelayDays(days(0));
ur53.setMaxDelayDays(days(60));
ur53.setHourDeadLine(time_duration(23,59,0));
ur53.setAccessCapacity(1);

PTUseRule ur54(2305843009213693954);
ur54.setName("1 seat only");
ur54.setReservationType(PTUseRule::RESERVATION_RULE_FORBIDDEN);
ur54.setAccessCapacity(1);


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Reservation contact
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ReservationContact rc05(5910974511914805);
rc05.setPhoneExchangeNumber("0800 929 929 (gratuit depuis un poste fixe) ou 05 34 25 33 75");
rc05.setPhoneExchangeOpeningHours("entre 6h30 et 22h30");
rc05.setDescription("Test");

OnlineReservationRule or33(13229323905400833);
or33.setReservationContact(&rc05);
or33.setNeedsPhone(true);
or33.setMaxSeats(1000);
or33.setSenderName("Centrale de reservation RCS");
or33.setSenderEMail("contact@rcsmobility.com");



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RollingStock
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RollingStock rs57(13792273858822157);
rs57.setName("test");


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Lines
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// JourneyPattern 92

CommercialLine cl92(11821949021891592);
cl92.setNetwork(&n34);
cl92.setShortName("92");
cl92.setStyle("vertfonce");
{
	RuleUser::Rules r(RuleUser::GetEmptyRules());
	r[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
	r[USER_HANDICAPPED - USER_CLASS_CODE_OFFSET] = &ur54;
	cl92.setRules(r);
}

JourneyPattern li92(2533274790397692, "92");
li92.setTimetableName("92.1");
li92.setRollingStock(&rs57);
li92.setCommercialLine(&cl92);

DesignatedLinePhysicalStop ls86(2814749767106586, &li92, 0, true, false, 0, &ps86);
li92.addEdge(ls86);

DesignatedLinePhysicalStop ls63(2814749767106563, &li92, 1, true, true, 100, &ps73);
li92.addEdge(ls63);

DesignatedLinePhysicalStop ls61(2814749767106561, &li92, 2, true, true, 5500, &ps75);
li92.addEdge(ls61);

DesignatedLinePhysicalStop ls62(2814749767106562, &li92, 3, true, true, 6400, &ps79);
li92.addEdge(ls62);

DesignatedLinePhysicalStop ls60(2814749767106560, &li92, 4, false, true, 6500, &ps85);
li92.addEdge(ls60);

ScheduledService ss01(4503599627370501, "1", &li92);
{
	ScheduledService::Schedules a;
	ScheduledService::Schedules d;
	a.push_back(time_duration(0,0,0));
	d.push_back(time_duration(7,0,0));

	a.push_back(time_duration(7,5,0));
	d.push_back(time_duration(7,5,0));

	a.push_back(time_duration(7,10,0));
	d.push_back(time_duration(7,10,0));

	a.push_back(time_duration(7,15,0));
	d.push_back(time_duration(7,14,0));

	a.push_back(time_duration(7,15,0));
	d.push_back(time_duration(0,0,0));

	ss01.setSchedules(d,a);
}
ss01.setActive(day_clock::local_day());
ss01.setActive(day_clock::local_day() + days(1));
ss01.setActive(day_clock::local_day() + days(2));
li92.addService(&ss01, true);

ContinuousService cs97(4785074604214097, "", &li92, minutes(60), minutes(5));
{
	ScheduledService::Schedules a;
	ScheduledService::Schedules d;
	a.push_back(time_duration(0,0,0));
	d.push_back(time_duration(19,17,0));

	a.push_back(time_duration(19,22,0));
	d.push_back(time_duration(19,22,0));

	a.push_back(time_duration(19,30,0));
	d.push_back(time_duration(19,30,0));

	a.push_back(time_duration(19,42,0));
	d.push_back(time_duration(19,41,0));

	a.push_back(time_duration(19,49,0));
	d.push_back(time_duration(0,0,0));

	cs97.setSchedules(d, a);
}
cs97.setActive(day_clock::local_day());
cs97.setActive(day_clock::local_day() + days(1));
li92.addService(&cs97, true);

ScheduledService ss02(4503599627370502, "2", &li92);
{
	ScheduledService::Schedules a;
	ScheduledService::Schedules d;
	a.push_back(time_duration(0,0,0));
	d.push_back(time_duration(21,55,0));

	a.push_back(time_duration(22,0,0));
	d.push_back(time_duration(22,0,0));

	a.push_back(time_duration(22,10,0));
	d.push_back(time_duration(22,10,0));

	a.push_back(time_duration(22,20,0));
	d.push_back(time_duration(22,20,0));

	a.push_back(time_duration(22,25,0));
	d.push_back(time_duration(0,0,0));

	ss02.setSchedules(d, a);
}
ss02.setActive(day_clock::local_day());
ss02.setActive(day_clock::local_day() + days(1));
li92.addService(&ss02, true);

// JourneyPattern 93

CommercialLine cl93(11821949021891593);
cl93.setNetwork(&n34);
cl93.setShortName("93");
cl93.setStyle("bleuclair");
{
	RuleUser::Rules r(RuleUser::GetEmptyRules());
	r[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
	cl93.setRules(r);
}

JourneyPattern li93(2533274790397693, "93-2");
li93.setTimetableName("93.2");
li93.setRollingStock(&rs57);
li93.setCommercialLine(&cl93);

DesignatedLinePhysicalStop ls64(2814749767106564, &li93, 0, true, false, 6500, &ps73);
li93.addEdge(ls64);

DesignatedLinePhysicalStop ls65(2814749767106565, &li93, 1, true, true, 12141, &ps81);
{
//	DesignatedLinePhysicalStop::ViaPoints v;
//	v.push_back(new Coordinate(523000,1843000));
//	v.push_back(new Coordinate(525000,1843000));
//	ls65.setViaPoints(v);
}
li93.addEdge(ls65);

DesignatedLinePhysicalStop ls66(2814749767106566, &li93, 2, false, true, 12141, &ps75);
li93.addEdge(ls66);

ScheduledService ss18(4503599627370518, "18", &li93);
{
	ScheduledService::Schedules a;
	ScheduledService::Schedules d;
	a.push_back(time_duration(0,0,0));
	d.push_back(time_duration(7,0,0));

	a.push_back(time_duration(7,3,0));
	d.push_back(time_duration(7,3,0));

	a.push_back(time_duration(7,8,0));
	d.push_back(time_duration(0,0,0));

	ss18.setSchedules(d, a);
}
ss18.setActive(day_clock::local_day());
ss18.setActive(day_clock::local_day() + days(1));
li93.addService(&ss18, true);

ScheduledService ss08(4503599627370508, "8", &li93);
{
	ScheduledService::Schedules a;
	ScheduledService::Schedules d;
	a.push_back(time_duration(0,0,0));
	d.push_back(time_duration(7,10,0));

	a.push_back(time_duration(7,15,0));
	d.push_back(time_duration(7,16,0));

	a.push_back(time_duration(7,20,0));
	d.push_back(time_duration(0,0,0));

	ss08.setSchedules(d, a);
}
ss08.setActive(day_clock::local_day());
ss08.setActive(day_clock::local_day() + days(1));
li93.addService(&ss08, true);

ScheduledService ss07(4503599627370507, "7", &li93);
{
	ScheduledService::Schedules a;
	ScheduledService::Schedules d;
	a.push_back(time_duration(0,0,0));
	d.push_back(time_duration(9,0,0));

	a.push_back(time_duration(9,10,0));
	d.push_back(time_duration(9,11,0));

	a.push_back(time_duration(9,20,0));
	d.push_back(time_duration(0,0,0));

	ss07.setSchedules(d, a);
}
ss07.setActive(day_clock::local_day());
ss07.setActive(day_clock::local_day() + days(1));
li93.addService(&ss07, true);

// JourneyPattern 94

CommercialLine cl94(11821949021891594);
cl94.setNetwork(&n34);
cl94.setShortName("94");
cl94.setStyle("orange");
{
	RuleUser::Rules r(RuleUser::GetEmptyRules());
	r[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = &ur53;
	cl94.setRules(r);
}

JourneyPattern li94(2533274790397694, "94-3");
li94.setTimetableName("94.3");
li94.setRollingStock(&rs57);
li94.setCommercialLine(&cl94);

DesignatedLinePhysicalStop ls67(2814749767106567, &li94, 0, true, false, 14141, &ps83);
li94.addEdge(ls67);

DesignatedLinePhysicalStop ls69(2814749767106569, &li94, 1, true, true, 15141, &ps85);
li94.addEdge(ls69);

DesignatedLinePhysicalStop ls71(2814749767106571, &li94, 2, true, true, 16141, &ps80);
li94.addEdge(ls71);

DesignatedLinePhysicalStop ls70(2814749767106570, &li94, 3, true, true, 17141, &ps76);
li94.addEdge(ls70);

DesignatedLinePhysicalStop ls68(2814749767106568, &li94, 4, true, true, 17641, &ps73);
li94.addEdge(ls68);

DesignatedLinePhysicalStop ls9407(2814749767109407, &li94, 5, false, true, 18641, &ps07);
li94.addEdge(ls9407);

ScheduledService ss11(4503599627370511, "11", &li94);
{
	ScheduledService::Schedules a;
	ScheduledService::Schedules d;
	a.push_back(time_duration(0,0,0));
	d.push_back(time_duration(20,0,0));

	a.push_back(time_duration(20,5,0));
	d.push_back(time_duration(20,6,0));

	a.push_back(time_duration(20,10,0));
	d.push_back(time_duration(20,11,0));

	a.push_back(time_duration(20,15,0));
	d.push_back(time_duration(20,15,0));

	a.push_back(time_duration(20,20,0));
	d.push_back(time_duration(20,20,0));

	a.push_back(time_duration(21,20,0));
	d.push_back(time_duration(0,0,0));

	ss11.setSchedules(d, a);
}
ss11.setActive(day_clock::local_day());
ss11.setActive(day_clock::local_day() + days(1));
li94.addService(&ss11, true);

ScheduledService ss06(4503599627370506, "06", &li94);
{
	ScheduledService::Schedules a;
	ScheduledService::Schedules d;
	a.push_back(time_duration(0,0,0));
	d.push_back(time_duration(21,0,0));

	a.push_back(time_duration(21,5,0));
	d.push_back(time_duration(21,6,0));

	a.push_back(time_duration(21,10,0));
	d.push_back(time_duration(21,11,0));

	a.push_back(time_duration(21,15,0));
	d.push_back(time_duration(21,15,0));

	a.push_back(time_duration(21,20,0));
	d.push_back(time_duration(21,20,0));

	a.push_back(time_duration(22,20,0));
	d.push_back(time_duration(0,0,0));

	ss06.setSchedules(d, a);
}
ss06.setActive(day_clock::local_day());
ss06.setActive(day_clock::local_day() + days(1));
li94.addService(&ss06, true);

// JourneyPattern 95

CommercialLine cl95(11821949021891595);
cl95.setNetwork(&n34);
cl95.setShortName("95");
cl95.setStyle("rouge");
{
	RuleUser::Rules r(RuleUser::GetEmptyRules());
	r[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
	cl95.setRules(r);
}

JourneyPattern li95(2533274790397695, "95-1");
li95.setTimetableName("95.1");
li95.setRollingStock(&rs57);
li95.setCommercialLine(&cl95);

DesignatedLinePhysicalStop ls72(2814749767106572, &li95, 0, true, false, 17641, &ps75);
li95.addEdge(ls72);

DesignatedLinePhysicalStop ls73(2814749767106573, &li95, 1, false, true, 17641, &ps84);
li95.addEdge(ls73);

ScheduledService ss09(4503599627370509, "09", &li95);
{
	ScheduledService::Schedules a;
	ScheduledService::Schedules d;
	a.push_back(time_duration(0,0,0));
	d.push_back(time_duration(10,5,0));

	a.push_back(time_duration(10,20,0));
	d.push_back(time_duration(0,0,0));

	ss09.setSchedules(d, a);
}
ss09.setActive(day_clock::local_day());
ss09.setActive(day_clock::local_day() + days(1));
li95.addService(&ss09, true);

ScheduledService ss10(4503599627370510, "10", &li95);
{
	ScheduledService::Schedules a;
	ScheduledService::Schedules d;
	a.push_back(time_duration(0,0,0));
	d.push_back(time_duration(12,5,0));

	a.push_back(time_duration(12,10,0));
	d.push_back(time_duration(0,0,0));

	ss10.setSchedules(d, a);
}
ss10.setActive(day_clock::local_day());
ss10.setActive(day_clock::local_day() + days(1));
li95.addService(&ss10, true);

// JourneyPattern 96

CommercialLine cl96(11821949021891596);
cl96.setNetwork(&n34);
cl96.setShortName("96");
cl96.setStyle("jaune");
{
	RuleUser::Rules r(RuleUser::GetEmptyRules());
	r[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
	cl96.setRules(r);
}

JourneyPattern li96(2533274790397696, "96-1");
li96.setTimetableName("96.1");
li96.setRollingStock(&rs57);
li96.setCommercialLine(&cl96);

DesignatedLinePhysicalStop ls74(2814749767106574, &li96, 0, true, false, 17641, &ps79);
li96.addEdge(ls74);

DesignatedLinePhysicalStop ls75(2814749767106575, &li96, 1, false, true, 17641, &ps85);
li96.addEdge(ls75);

ScheduledService ss13(4503599627370513, "13", &li96);
{
	ScheduledService::Schedules a;
	ScheduledService::Schedules d;
	a.push_back(time_duration(0,0,0));
	d.push_back(time_duration(19,55,0));

	a.push_back(time_duration(20,5,0));
	d.push_back(time_duration(0,0,0));

	ss13.setSchedules(d, a);
}
ss13.setActive(day_clock::local_day());
ss13.setActive(day_clock::local_day() + days(1));
li96.addService(&ss13, true);

ScheduledService ss14(4503599627370514, "14", &li96);
{
	ScheduledService::Schedules a;
	ScheduledService::Schedules d;
	a.push_back(time_duration(0,0,0));
	d.push_back(time_duration(20,20,0));

	a.push_back(time_duration(20,25,0));
	d.push_back(time_duration(0,0,0));

	ss14.setSchedules(d, a);
}
ss14.setActive(day_clock::local_day());
ss14.setActive(day_clock::local_day() + days(1));
li96.addService(&ss14, true);

// JourneyPattern 97

CommercialLine cl97(11821949021891597);
cl97.setNetwork(&n34);
cl97.setShortName("97");
cl97.setStyle("rose");
{
	RuleUser::Rules r(RuleUser::GetEmptyRules());
	r[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
	cl97.setRules(r);
}

JourneyPattern li97(2533274790397697, "97");
li97.setTimetableName("97");
li97.setRollingStock(&rs57);
li97.setCommercialLine(&cl97);

DesignatedLinePhysicalStop ls9773(2814749767109773, &li97, 0, true, false, 0, &ps73);
li97.addEdge(ls9773);

DesignatedLinePhysicalStop ls9706(2814749767109706, &li97, 1, false, true, 2000, &ps06);
li97.addEdge(ls9706);

ContinuousService cs9701(4503599627379701, "9701", &li97);
{
	ScheduledService::Schedules a;
	ScheduledService::Schedules d;
	a.push_back(time_duration(0,0,0));
	d.push_back(time_duration(21,40,0));

	a.push_back(time_duration(21,42,0));
	d.push_back(time_duration(0,0,0));

	cs9701.setSchedules(d, a);
}
cs9701.setActive(day_clock::local_day());
cs9701.setActive(day_clock::local_day() + days(1));
cs9701.setMaxWaitingTime(minutes(0));
cs9701.setRange(minutes(30));
li97.addService(&cs9701, true);


// JourneyPattern 98

CommercialLine cl98(11821949021891598);
cl98.setNetwork(&n34);
cl98.setShortName("98");
cl98.setStyle("vertclair");
{
	RuleUser::Rules r(RuleUser::GetEmptyRules());
	r[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
	cl98.setRules(r);
}

JourneyPattern li98(2533274790397698, "98");
li98.setTimetableName("98");
li98.setRollingStock(&rs57);
li98.setCommercialLine(&cl98);

DesignatedLinePhysicalStop ls9806(2814749767109806, &li98, 0, true, false, 0, &ps06);
li98.addEdge(ls9806);

DesignatedLinePhysicalStop ls9807(2814749767109807, &li98, 1, false, true, 2000, &ps07);
li98.addEdge(ls9807);

ContinuousService cs9801(4503599627379801, "9801", &li98);
{
	ScheduledService::Schedules a;
	ScheduledService::Schedules d;
	a.push_back(time_duration(0,0,0));
	d.push_back(time_duration(21,50,0));

	a.push_back(time_duration(22,00,0));
	d.push_back(time_duration(0,0,0));

	cs9801.setSchedules(d, a);
}
cs9801.setActive(day_clock::local_day());
cs9801.setActive(day_clock::local_day() + days(1));
cs9801.setMaxWaitingTime(minutes(0));
cs9801.setRange(minutes(30));
li98.addService(&cs9801, true);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Non concurrency rules
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//shared_ptr<NonConcurrencyRule> nc01
//cl93.addConcurrencyRule(cl92.get());

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Data sources
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DataSource ds05(16607023625928705);
ds05.setName("test source");
ds05.setFormat("Trident");
