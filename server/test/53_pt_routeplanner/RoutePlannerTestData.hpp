
/** Route planner test data set.
	@file RoutePlannerTestData.hpp

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
using namespace synthese::road;
using namespace synthese::util;
using namespace synthese::graph;
using namespace synthese::impex;
using namespace synthese::pt;
using namespace synthese::db;

using namespace geos::geom;
using namespace boost;

GeographyModule::PreInit();

#ifndef SAVE
#define SAVE(_class, object) do {} while(0)
#endif

Env env;

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
// Roads
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Crossings
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Coordinate co10(520000.0,1844000.0);
Crossing c10(12103423998558210ULL, boost::shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(co10)));
SAVE(Crossing, c10);

Coordinate co74(523000.0,1844000.0);
Crossing c74(12103423998558274ULL, boost::shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(co74)));
SAVE(Crossing, c74);

Coordinate co86(523000.0,1845342.0);
Crossing c86(12103423998558286ULL, boost::shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(co86)));
SAVE(Crossing, c86);

Coordinate co88(523000.0,1845342.0);
Crossing c88(12103423998558288ULL, boost::shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(co88)));
SAVE(Crossing, c88);

Coordinate co89(521000.0,1845000.0);
Crossing c89(12103423998558289ULL, boost::shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(co89)));
SAVE(Crossing, c89);

Coordinate co90(528000.0,1844000.0);
Crossing c90(12103423998558290ULL, boost::shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(co90)));
SAVE(Crossing, c90);

Coordinate co91(525000.0,1844000.0);
Crossing c91(12103423998558291ULL, boost::shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(co91)));
SAVE(Crossing, c91);

Coordinate co92(523263.0,1845331.0);
Crossing c92(12103423998558292ULL, boost::shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(co92)));
SAVE(Crossing, c92);

Coordinate co93(523000.0,1845342.0);
Crossing c93(12103423998558293ULL, boost::shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(co93)));
SAVE(Crossing, c93);

Coordinate co94(523000.0,1845342.0);
Crossing c94(12103423998558294ULL, boost::shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(co94)));
SAVE(Crossing, c94);

Coordinate co96(526000.0,1847000.0);
Crossing c96(12103423998558296ULL, boost::shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(co96)));
SAVE(Crossing, c96);

Coordinate co97(523266.0,1845341.0);
Crossing c97(12103423998558297ULL, boost::shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(co97)));
SAVE(Crossing, c97);

Coordinate co98(523000.0,1845342.0);
Crossing c98(12103423998558298ULL, boost::shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(co98)));
SAVE(Crossing, c98);

Coordinate co99(526000.0,1845000.0);
Crossing c99(12103423998558299ULL, boost::shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(co99)));
SAVE(Crossing, c99);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Roads
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Road 40

RoadPlace rp40(16888498602639440ULL);
rp40.setName("40");
rp40.setCity(&city54);
SAVE(RoadPlace, rp40);

Road ro40(4222124650659840ULL);
ro40.set<RoadPlace>(rp40);
SAVE(Road, ro40);
ro40.link(env);

RoadChunk rc86_40(3940649673949187ULL, &c86, 0, &ro40, 0);
SAVE(RoadChunk, rc86_40);
rc86_40.link(env);

RoadChunk rc89(3940649673949188ULL, &c89, 1, &ro40, 200);
SAVE(RoadChunk, rc89);
rc89.link(env);

// Road 41

RoadPlace rp41(16888498602639441ULL);
rp41.setName("41");
rp41.setCity(&city54);
SAVE(RoadPlace, rp41);

Road ro41(4222124650659841ULL);
ro41.set<RoadPlace>(rp41);
SAVE(Road, ro41);
ro41.link(env);

RoadChunk rc88_41(3940649673949288ULL, &c88, 0, &ro41, 0);
SAVE(RoadChunk, rc88_41);
rc88_41.link(env);

RoadChunk rc86_41(3940649673949186ULL, &c86, 1, &ro41, 300);
SAVE(RoadChunk, rc86_41);
rc86_41.link(env);

RoadChunk rc74(3940649673949174ULL, &c74, 2, &ro41, 750);
SAVE(RoadChunk, rc74);
rc74.link(env);

RoadChunk rc93_41(3940649673949293ULL, &c93, 3, &ro41, 1700);
SAVE(RoadChunk, rc93_41);
rc93_41.link(env);

// Road 42

RoadPlace rp42(16888498602639442ULL);
rp42.setName("42");
rp42.setCity(&city38);
SAVE(RoadPlace, rp42);

Road ro42(4222124650659842ULL);
ro42.set<RoadPlace>(rp42);
SAVE(Road, ro42);
ro42.link(env);

RoadChunk rc98_42(3940649673949298ULL, &c98, 0, &ro42, 0);
SAVE(RoadChunk, rc98_42);
rc98_42.link(env);

RoadChunk rc97(3940649673949197ULL, &c97, 1, &ro42, 200);
SAVE(RoadChunk, rc97);
rc97.link(env);

Road ro44(4222124650659844ULL);
SAVE(Road, ro44);
ro44.set<RoadPlace>(rp42);
ro44.link(env);

RoadChunk rc98_44(3940649673949498ULL, &c98, 0, &ro44, 0);
SAVE(RoadChunk, rc98_44);
rc98_44.link(env);

RoadChunk rc93(3940649673949193ULL, &c93, 1, &ro44, 800);
SAVE(RoadChunk, rc93);
rc93.link(env);

RoadChunk rc94(3940649673949194ULL, &c94, 2, &ro44, 1600);
SAVE(RoadChunk, rc94);
rc94.link(env);


// Road 43

RoadPlace rp43(16888498602639443ULL);
rp43.setName("43");
rp43.setCity(&city38);
SAVE(RoadPlace, rp43);

Road ro43(4222124650659843ULL);
ro43.set<RoadPlace>(rp43);
SAVE(Road, ro43);
ro43.link(env);

RoadChunk rc93_43(3940649673949393ULL, &c93, 0, &ro43, 0);
SAVE(RoadChunk, rc93_43);
rc93_43.link(env);

RoadChunk rc96(3940649673949396ULL, &c96, 1, &ro43, 100);
SAVE(RoadChunk, rc96);
rc96.link(env);

RoadChunk rc99(3940649673949199ULL, &c99, 2, &ro43, 150);
SAVE(RoadChunk, rc99);
rc99.link(env);

// Road 45

RoadPlace rp45(16888498602639445ULL);
rp45.setName("45");
rp45.setCity(&city54);
SAVE(RoadPlace, rp45);

Road ro45(4222124650659845ULL);
ro45.set<RoadPlace>(rp45);
SAVE(Road, ro45);
ro45.link(env);

RoadChunk rc86_45(3940649673949546ULL, &c86, 0, &ro45, 0);
SAVE(RoadChunk, rc86_45);
rc86_45.link(env);

RoadChunk rc10(3940649673949410ULL, &c10, 1, &ro45, 400);
SAVE(RoadChunk, rc10);
rc10.link(env);

// Road 46

RoadPlace rp46(16888498602639446ULL);
rp46.setName("46");
rp46.setCity(&city38);
SAVE(RoadPlace, rp46);

Road ro46(4222124650659846ULL);
ro46.set<RoadPlace>(rp46);
SAVE(Road, ro46);
ro46.link(env);

RoadChunk rc93_46(3940649673949693ULL, &c93, 0, &ro46, 0);
SAVE(RoadChunk, rc93_46);
rc93_46.link(env);

RoadChunk rc91(3940649673949189ULL, &c91, 1, &ro46, 400);
SAVE(RoadChunk, rc91);
rc91.link(env);

RoadChunk rc90(3940649673949190ULL, &c90, 2, &ro46, 2000);
SAVE(RoadChunk, rc90);
rc90.link(env);

// Road 47

RoadPlace rp47(16888498602639447ULL);
rp47.setName("47");
rp47.setCity(&city54);
SAVE(RoadPlace, rp47);

Road ro47(4222124650659847ULL);
ro47.set<RoadPlace>(rp47);
SAVE(Road, ro47);
ro47.link(env);

RoadChunk rc88_47(3940649673949788ULL, &c88, 0, &ro47, 0);
SAVE(RoadChunk, rc88_47);
rc88_47.link(env);

RoadChunk rc92(3940649673949292ULL, &c92, 1, &ro47, 200);
SAVE(RoadChunk, rc92);
rc92.link(env);


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Places
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Place 93
StopArea place93(1970324837184593ULL,true,minutes(8));
place93.setName("93");
place93.setCity(&city54);
place93.setAllowedConnection(true);
city54.addIncludedPlace(place93);
StopArea::TransferDelaysMap delays93;
StopArea::_addTransferDelay(delays93, 562949953421389ULL,3377699720880573ULL,minutes(0));
place93.setTransferDelaysMatrix(delays93);

Coordinate cops73(521000.0,1847000.0);
StopPoint ps73(
	3377699720880573ULL,
	"93/73",
	&place93,
	boost::shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops73))
);
SAVE(StopPoint, ps73);
ps73.setProjectedPoint(Address(rc89,0));
rc89.getFromCrossing()->addReachableVertex(&ps73);
//ps73.setAllsetCodeBySource("CTP:StopArea:SPOCE87446179");
place93.addPhysicalStop(ps73);
SAVE(StopArea, place93);


// Place 94
StopArea place94(1970324837184594ULL,false,minutes(8));
place94.setName("94");
place94.setCity(&city54);
place94.setAllowedConnection(true);
StopArea::TransferDelaysMap delays94;
StopArea::_addTransferDelay(delays94,3377699720880574ULL,3377699720880575ULL,minutes(11));
StopArea::_addTransferDelay(delays94,3377699720880575ULL,3377699720880574ULL,minutes(12));
StopArea::_addTransferDelay(delays94,3377699720880574ULL,562949953421374ULL,minutes(0));
StopArea::_addTransferDelay(delays94,562949953421374ULL,3377699720880574ULL,minutes(0));
StopArea::_addTransferDelay(delays94,3377699720880575ULL,562949953421374ULL,minutes(0));
StopArea::_addTransferDelay(delays94,562949953421374ULL,3377699720880575ULL,minutes(0));
place94.setTransferDelaysMatrix(delays94);

Coordinate cops75(523000.0,1845000.0);
StopPoint ps75(3377699720880575ULL, "94/75", &place94, boost::shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops75)));
//ps75.setCodeBySource("CTP:StopArea:SPOCE87353573");
ps75.setProjectedPoint(Address(rc74,0));
rc74.getFromCrossing()->addReachableVertex(&ps75);
SAVE(StopPoint, ps75);
place94.addPhysicalStop(ps75);
SAVE(StopArea, place94);

// Place 95
StopArea place95(1970324837184595ULL,false,minutes(8));
place95.setName("95");
place95.setCity(&city95);
place95.setAllowedConnection(false);
StopArea::TransferDelaysMap delays95;
StopArea::_addTransferDelay(delays95,562949953421397ULL,377699720880576ULL,minutes(0));
place95.setTransferDelaysMatrix(delays94);

Coordinate cops76(523266.0,1845339.0);
StopPoint ps76(3377699720880576ULL, "95/76", &place95, boost::shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops76)));
//ps76.setCodeBySource("CTP:StopArea:SPOCE87611939");
ps76.setProjectedPoint(Address(rc97,0));
rc97.getFromCrossing()->addReachableVertex(&ps76);
SAVE(StopPoint, ps76);
place95.addPhysicalStop(ps76);
SAVE(StopArea, place95);

// Place 96
StopArea place96(1970324837184596ULL,false,minutes(8));
place96.setName("96");
place96.setCity(&city38);
place96.setAllowedConnection(false);

Coordinate cops77(524000.0,1849500.0);
StopPoint ps77(3377699720880577ULL, "96/77", &place96, boost::shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops77)));
SAVE(StopPoint, ps77);
place96.addPhysicalStop(ps77);

Coordinate cops78(524000.0,1849500.0);
StopPoint ps78(3377699720880578ULL, "96/78", &place96, boost::shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops78)));
SAVE(StopPoint, ps78);
place96.addPhysicalStop(ps78);
SAVE(StopArea, place96);

// Place 97
StopArea place97(1970324837184597ULL,true,minutes(8));
place97.setName("97");
place97.setCity(&city38);
place97.setAllowedConnection(true);
city38.addIncludedPlace(place97);
StopArea::TransferDelaysMap delays97;
StopArea::_addTransferDelay(delays97,562949953421396ULL,3377699720880580ULL,minutes(0));
StopArea::_addTransferDelay(delays97,562949953421399ULL,3377699720880579ULL,minutes(0));
StopArea::_addTransferDelay(delays97,3377699720880580ULL,562949953421396ULL,minutes(0));
StopArea::_addTransferDelay(delays97,3377699720880579ULL,562949953421399ULL,minutes(0));
place97.setTransferDelaysMatrix(delays97);

Coordinate cops79(526000,1845000);
StopPoint ps79(3377699720880579ULL, "97/79", &place97, boost::shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops79)));
//ps79.setCodeBySource("CTP:StopArea:SPOCE87353599");
ps79.setProjectedPoint(Address(rc99, 0));
rc99.getFromCrossing()->addReachableVertex(&ps79);
SAVE(StopPoint, ps79);
place97.addPhysicalStop(ps79);

Coordinate cops80(526000.0,1847000.0);
StopPoint ps80(3377699720880580ULL, "97/80", &place97, boost::shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops80)));
ps80.setProjectedPoint(Address(rc96,0));
rc96.getFromCrossing()->addReachableVertex(&ps80);
SAVE(StopPoint, ps80);
place97.addPhysicalStop(ps80);
SAVE(StopArea, place97);


// Place 98
StopArea place98(1970324837184598ULL,false,minutes(8));
place98.setName("98");
place98.setCity(&city38);
place98.setAllowedConnection(true);
/*'562949953421391:3377699720880581:0,562949953421391:3377699720880582:0,3377699720880581:562949953421391:0,3377699720880582:562949953421391:0'*/

Coordinate cops81(525000.0,1844000.0);
StopPoint ps81(3377699720880581ULL, "98/81", &place98, boost::shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops81)));
//ps81.setCodeBySource("CTP:StopArea:SPOCE87353581");
ps81.setProjectedPoint(Address(rc91,0));
rc91.getFromCrossing()->addReachableVertex(&ps81);
SAVE(StopPoint, ps81);
place98.addPhysicalStop(ps81);

Coordinate cops82(525000.0,1844000.0);
StopPoint ps82(3377699720880582ULL, "98/82", &place98, boost::shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops82)));
SAVE(StopPoint, ps82);
place98.addPhysicalStop(ps82);
SAVE(StopArea, place98);


// Place 99
StopArea place99(1970324837184599ULL,true,minutes(8));
place99.setName("99");
place99.setCity(&city38);
// connection type 5
city38.addIncludedPlace(place99);
StopArea::TransferDelaysMap delays99;
StopArea::_addTransferDelay(delays99,3377699720880585ULL,562949953421390ULL,minutes(0));
StopArea::_addTransferDelay(delays99,562949953421390ULL,3377699720880585ULL,minutes(0));
place99.setTransferDelaysMatrix(delays99);

Coordinate cops83(528000.0,1849000.0);
StopPoint ps83(3377699720880583ULL, "99/83", &place99, boost::shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops83)));
//ps83.setCodeBySource("CTP:StopArea:SPOCE87611467");
ps83.setProjectedPoint(Address(rc94,0));
rc94.getFromCrossing()->addReachableVertex(&ps83);
SAVE(StopPoint, ps83);
place99.addPhysicalStop(ps83);

Coordinate cops84(528000.0,1845000.0);
StopPoint ps84(3377699720880584ULL, "99/84", &place99, boost::shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops84)));
ps84.setProjectedPoint(Address(rc90,0));
rc90.getFromCrossing()->addReachableVertex(&ps84);
SAVE(StopPoint, ps84);
place99.addPhysicalStop(ps84);

Coordinate cops85(528000.0,1847000.0);
StopPoint ps85(3377699720880585ULL, "99/85", &place99, boost::shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops85)));
SAVE(StopPoint, ps85);
place99.addPhysicalStop(ps85);
SAVE(StopArea, place99);


// Place 05
StopArea place05(1970324837184605ULL, true, minutes(8));
place05.setName("05");
place05.setCity(&city54);
place05.setAllowedConnection(true);
StopArea::TransferDelaysMap delays05;
StopArea::_addTransferDelay(delays05,562949953421410ULL,3377699720880586ULL,minutes(0));
place05.setTransferDelaysMatrix(delays05);

Coordinate cops86(520000,1845000.0);
StopPoint ps86(3377699720880586ULL, "05/86", &place05, boost::shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops86)));
ps86.setProjectedPoint(Address(rc10,0));
rc10.getFromCrossing()->addReachableVertex(&ps86);
SAVE(StopPoint, ps86);
place05.addPhysicalStop(ps86);
SAVE(StopArea, place05);


// Place 06
StopArea place06(1970324837184606ULL, true, minutes(8));
place06.setName("06");
place06.setCity(&city54);
place06.setAllowedConnection(true);

Coordinate cops06(526000,1852000.0);
StopPoint ps06(3377699720880606ULL, "06", &place06, boost::shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops06)));
SAVE(StopPoint, ps06);
place06.addPhysicalStop(ps06);
SAVE(StopArea, place06);


// Place 07
StopArea place07(1970324837184607ULL, true, minutes(8));
place07.setName("07");
place07.setCity(&city54);
place07.setAllowedConnection(true);

Coordinate cops07(523000,1852000.0);
StopPoint ps07(3377699720880607ULL, "07", &place07, boost::shared_ptr<Point>(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(cops07)));
SAVE(StopPoint, ps07);
place07.addPhysicalStop(ps07);
SAVE(StopArea, place07);



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Networks
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TransportNetwork n34(6192449487677434ULL,"Network34");
SAVE(TransportNetwork, n34);

TransportNetwork n37(6192449487677437ULL,"Network37");
SAVE(TransportNetwork, n37);


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Use rules
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PTUseRule ur53(2305843009213693953ULL);
ur53.setName("Compulsory reservation");
ur53.setReservationType(pt::RESERVATION_RULE_MIXED_BY_DEPARTURE_PLACE);
ur53.setOriginIsReference(false);
ur53.setMinDelayMinutes(minutes(10));
ur53.setMinDelayDays(days(0));
ur53.setMaxDelayDays(days(60));
ur53.setHourDeadLine(time_duration(23,59,0));
ur53.setAccessCapacity(1);
SAVE(PTUseRule, ur53);

PTUseRule ur54(2305843009213693954ULL);
ur54.setName("1 seat only");
ur54.setReservationType(pt::RESERVATION_RULE_FORBIDDEN);
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

JourneyPattern li92(2533274790397692ULL, "92");
li92.setTimetableName("92.1");
li92.setRollingStock(&rs57);
li92.setCommercialLine(&cl92);

LineStop ls86(2814749767106586ULL, &li92, 0, true, false, 0, ps86);
SAVE(LineStop, ls86);
ls86.link(env);

LineStop ls63(2814749767106563ULL, &li92, 1, true, true, 100, ps73);
SAVE(LineStop, ls63);
ls63.link(env);

LineStop ls61(2814749767106561ULL, &li92, 2, true, true, 5500, ps75);
SAVE(LineStop, ls61);
ls61.link(env);

LineStop ls62(2814749767106562ULL, &li92, 3, true, true, 6400, ps79);
SAVE(LineStop, ls62);
ls62.link(env);

LineStop ls60(2814749767106560ULL, &li92, 4, false, true, 6500, ps85);
SAVE(LineStop, ls60);
ls60.link(env);

ScheduledService ss01(4503599627370501ULL, "1", &li92);
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

	ss01.setDataSchedules(d,a);
}
ss01.setActive(day_clock::local_day());
ss01.setActive(day_clock::local_day() + days(1));
ss01.setActive(day_clock::local_day() + days(2));
SAVE(ScheduledService, ss01);
li92.addService(ss01, true);

ContinuousService cs97(4785074604214097ULL, "", &li92, minutes(60), minutes(5));
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

	cs97.setDataSchedules(d, a);
}
cs97.setActive(day_clock::local_day());
cs97.setActive(day_clock::local_day() + days(1));
SAVE(ContinuousService, cs97);
li92.addService(cs97, true);

ScheduledService ss02(4503599627370502ULL, "2", &li92);
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

	ss02.setDataSchedules(d, a);
}
ss02.setActive(day_clock::local_day());
ss02.setActive(day_clock::local_day() + days(1));
SAVE(ScheduledService, ss02);
li92.addService(ss02, true);
SAVE(JourneyPattern, li92);

// JourneyPattern 93

CommercialLine cl93(11821949021891593ULL);
cl93.setParent(n34);
cl93.setShortName("93");
cl93.setStyle("bleuclair");
{
	RuleUser::Rules r(RuleUser::GetEmptyRules());
	r[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
	cl93.setRules(r);
}
SAVE(CommercialLine, cl93);

JourneyPattern li93(2533274790397693ULL, "93-2");
li93.setTimetableName("93.2");
li93.setRollingStock(&rs57);
li93.setCommercialLine(&cl93);

LineStop ls64(2814749767106564ULL, &li93, 0, true, false, 6500, ps73);
SAVE(LineStop, ls64);
ls64.link(env);

LineStop ls65(2814749767106565ULL, &li93, 1, true, true, 12141, ps81);
SAVE(LineStop, ls65);
{
//	LineStop::ViaPoints v;
//	v.push_back(new Coordinate(523000,1843000));
//	v.push_back(new Coordinate(525000,1843000));
//	ls65.setViaPoints(v);
}
ls65.link(env);

LineStop ls66(2814749767106566ULL, &li93, 2, false, true, 12141, ps75);
SAVE(LineStop, ls66);
ls66.link(env);

ScheduledService ss18(4503599627370518ULL, "18", &li93);
{
	ScheduledService::Schedules a;
	ScheduledService::Schedules d;
	a.push_back(time_duration(0,0,0));
	d.push_back(time_duration(7,0,0));

	a.push_back(time_duration(7,3,0));
	d.push_back(time_duration(7,3,0));

	a.push_back(time_duration(7,8,0));
	d.push_back(time_duration(0,0,0));

	ss18.setDataSchedules(d, a);
}
ss18.setActive(day_clock::local_day());
ss18.setActive(day_clock::local_day() + days(1));
SAVE(ScheduledService, ss18);
li93.addService(ss18, true);

ScheduledService ss08(4503599627370508ULL, "8", &li93);
{
	ScheduledService::Schedules a;
	ScheduledService::Schedules d;
	a.push_back(time_duration(0,0,0));
	d.push_back(time_duration(7,10,0));

	a.push_back(time_duration(7,15,0));
	d.push_back(time_duration(7,16,0));

	a.push_back(time_duration(7,20,0));
	d.push_back(time_duration(0,0,0));

	ss08.setDataSchedules(d, a);
}
ss08.setActive(day_clock::local_day());
ss08.setActive(day_clock::local_day() + days(1));
SAVE(ScheduledService, ss08);
li93.addService(ss08, true);

ScheduledService ss07(4503599627370507ULL, "7", &li93);
{
	ScheduledService::Schedules a;
	ScheduledService::Schedules d;
	a.push_back(time_duration(0,0,0));
	d.push_back(time_duration(9,0,0));

	a.push_back(time_duration(9,10,0));
	d.push_back(time_duration(9,11,0));

	a.push_back(time_duration(9,20,0));
	d.push_back(time_duration(0,0,0));

	ss07.setDataSchedules(d, a);
}
ss07.setActive(day_clock::local_day());
ss07.setActive(day_clock::local_day() + days(1));
SAVE(ScheduledService, ss07);
li93.addService(ss07, true);
SAVE(JourneyPattern, li93);

// JourneyPattern 94

CommercialLine cl94(11821949021891594ULL);
cl94.setParent(n34);
cl94.setShortName("94");
cl94.setStyle("orange");
{
	RuleUser::Rules r(RuleUser::GetEmptyRules());
	r[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = &ur53;
	cl94.setRules(r);
}
SAVE(CommercialLine, cl94);

JourneyPattern li94(2533274790397694ULL, "94-3");
li94.setTimetableName("94.3");
li94.setRollingStock(&rs57);
li94.setCommercialLine(&cl94);

LineStop ls67(2814749767106567ULL, &li94, 0, true, false, 14141, ps83);
SAVE(LineStop, ls67);
ls67.link(env);

LineStop ls69(2814749767106569ULL, &li94, 1, true, true, 15141, ps85);
SAVE(LineStop, ls69);
ls69.link(env);

LineStop ls71(2814749767106571ULL, &li94, 2, true, true, 16141, ps80);
SAVE(LineStop, ls71);
ls71.link(env);

LineStop ls70(2814749767106570ULL, &li94, 3, true, true, 17141, ps76);
SAVE(LineStop, ls70);
ls70.link(env);

LineStop ls68(2814749767106568ULL, &li94, 4, true, true, 17641, ps73);
SAVE(LineStop, ls68);
ls68.link(env);

LineStop ls9407(2814749767109407ULL, &li94, 5, false, true, 18641, ps07);
SAVE(LineStop, ls9407);
ls9407.link(env);

ScheduledService ss11(4503599627370511ULL, "11", &li94);
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

	ss11.setDataSchedules(d, a);
}
ss11.setActive(day_clock::local_day());
ss11.setActive(day_clock::local_day() + days(1));
SAVE(ScheduledService, ss11);
li94.addService(ss11, true);

ScheduledService ss06(4503599627370506ULL, "06", &li94);
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

	ss06.setDataSchedules(d, a);
}
ss06.setActive(day_clock::local_day());
ss06.setActive(day_clock::local_day() + days(1));
SAVE(ScheduledService, ss06);
li94.addService(ss06, true);
SAVE(JourneyPattern, li94);

// JourneyPattern 95

CommercialLine cl95(11821949021891595ULL);
cl95.setParent(n34);
cl95.setShortName("95");
cl95.setStyle("rouge");
{
	RuleUser::Rules r(RuleUser::GetEmptyRules());
	r[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
	cl95.setRules(r);
}
SAVE(CommercialLine, cl95);

JourneyPattern li95(2533274790397695ULL, "95-1");
li95.setTimetableName("95.1");
li95.setRollingStock(&rs57);
li95.setCommercialLine(&cl95);

LineStop ls72(2814749767106572ULL, &li95, 0, true, false, 17641, ps75);
SAVE(LineStop, ls72);
ls72.link(env);

LineStop ls73(2814749767106573ULL, &li95, 1, false, true, 17641, ps84);
SAVE(LineStop, ls73);
ls73.link(env);

ScheduledService ss09(4503599627370509ULL, "09", &li95);
{
	ScheduledService::Schedules a;
	ScheduledService::Schedules d;
	a.push_back(time_duration(0,0,0));
	d.push_back(time_duration(10,5,0));

	a.push_back(time_duration(10,20,0));
	d.push_back(time_duration(0,0,0));

	ss09.setDataSchedules(d, a);
}
ss09.setActive(day_clock::local_day());
ss09.setActive(day_clock::local_day() + days(1));
SAVE(ScheduledService, ss09);
li95.addService(ss09, true);

ScheduledService ss10(4503599627370510ULL, "10", &li95);
{
	ScheduledService::Schedules a;
	ScheduledService::Schedules d;
	a.push_back(time_duration(0,0,0));
	d.push_back(time_duration(12,5,0));

	a.push_back(time_duration(12,10,0));
	d.push_back(time_duration(0,0,0));

	ss10.setDataSchedules(d, a);
}
ss10.setActive(day_clock::local_day());
ss10.setActive(day_clock::local_day() + days(1));
SAVE(ScheduledService, ss10);
li95.addService(ss10, true);
SAVE(JourneyPattern, li95);

// JourneyPattern 96

CommercialLine cl96(11821949021891596ULL);
cl96.setParent(n34);
cl96.setShortName("96");
cl96.setStyle("jaune");
{
	RuleUser::Rules r(RuleUser::GetEmptyRules());
	r[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
	cl96.setRules(r);
}
SAVE(CommercialLine, cl96);

JourneyPattern li96(2533274790397696ULL, "96-1");
li96.setTimetableName("96.1");
li96.setRollingStock(&rs57);
li96.setCommercialLine(&cl96);

LineStop ls74(2814749767106574ULL, &li96, 0, true, false, 17641, ps79);
SAVE(LineStop, ls74);
ls74.link(env);

LineStop ls75(2814749767106575ULL, &li96, 1, false, true, 17641, ps85);
SAVE(LineStop, ls75);
ls75.link(env);

ScheduledService ss13(4503599627370513ULL, "13", &li96);
{
	ScheduledService::Schedules a;
	ScheduledService::Schedules d;
	a.push_back(time_duration(0,0,0));
	d.push_back(time_duration(19,55,0));

	a.push_back(time_duration(20,5,0));
	d.push_back(time_duration(0,0,0));

	ss13.setDataSchedules(d, a);
}
ss13.setActive(day_clock::local_day());
ss13.setActive(day_clock::local_day() + days(1));
SAVE(ScheduledService, ss13);
li96.addService(ss13, true);

ScheduledService ss14(4503599627370514ULL, "14", &li96);
{
	ScheduledService::Schedules a;
	ScheduledService::Schedules d;
	a.push_back(time_duration(0,0,0));
	d.push_back(time_duration(20,20,0));

	a.push_back(time_duration(20,25,0));
	d.push_back(time_duration(0,0,0));

	ss14.setDataSchedules(d, a);
}
ss14.setActive(day_clock::local_day());
ss14.setActive(day_clock::local_day() + days(1));
SAVE(ScheduledService, ss14);
li96.addService(ss14, true);
SAVE(JourneyPattern, li96);

// JourneyPattern 97

CommercialLine cl97(11821949021891597ULL);
cl97.setParent(n34);
cl97.setShortName("97");
cl97.setStyle("rose");
{
	RuleUser::Rules r(RuleUser::GetEmptyRules());
	r[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
	cl97.setRules(r);
}
SAVE(CommercialLine, cl97);

JourneyPattern li97(2533274790397697ULL, "97");
li97.setTimetableName("97");
li97.setRollingStock(&rs57);
li97.setCommercialLine(&cl97);

LineStop ls9773(2814749767109773ULL, &li97, 0, true, false, 0, ps73);
SAVE(LineStop, ls9773);
ls9773.link(env);

LineStop ls9706(2814749767109706ULL, &li97, 1, false, true, 2000, ps06);
SAVE(LineStop, ls9706);
ls9706.link(env);

ContinuousService cs9701(4503599627379701ULL, "9701", &li97);
{
	ScheduledService::Schedules a;
	ScheduledService::Schedules d;
	a.push_back(time_duration(0,0,0));
	d.push_back(time_duration(21,40,0));

	a.push_back(time_duration(21,42,0));
	d.push_back(time_duration(0,0,0));

	cs9701.setDataSchedules(d, a);
}
cs9701.setActive(day_clock::local_day());
cs9701.setActive(day_clock::local_day() + days(1));
cs9701.setMaxWaitingTime(minutes(0));
cs9701.setRange(minutes(30));
SAVE(ContinuousService, cs9701);
li97.addService(cs9701, true);
SAVE(JourneyPattern, li97);

// JourneyPattern 98

CommercialLine cl98(11821949021891598ULL);
cl98.setParent(n34);
cl98.setShortName("98");
cl98.setStyle("vertclair");
{
	RuleUser::Rules r(RuleUser::GetEmptyRules());
	r[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
	cl98.setRules(r);
}
SAVE(CommercialLine, cl98);

JourneyPattern li98(2533274790397698ULL, "98");
li98.setTimetableName("98");
li98.setRollingStock(&rs57);
li98.setCommercialLine(&cl98);

LineStop ls9806(2814749767109806ULL, &li98, 0, true, false, 0, ps06);
SAVE(LineStop, ls9806);
ls9806.link(env);

LineStop ls9807(2814749767109807ULL, &li98, 1, false, true, 2000, ps07);
SAVE(LineStop, ls9807);
ls9807.link(env);

ContinuousService cs9801(4503599627379801ULL, "9801", &li98);
{
	ScheduledService::Schedules a;
	ScheduledService::Schedules d;
	a.push_back(time_duration(0,0,0));
	d.push_back(time_duration(21,50,0));

	a.push_back(time_duration(22,00,0));
	d.push_back(time_duration(0,0,0));

	cs9801.setDataSchedules(d, a);
}
cs9801.setActive(day_clock::local_day());
cs9801.setActive(day_clock::local_day() + days(1));
cs9801.setMaxWaitingTime(minutes(0));
cs9801.setRange(minutes(30));
SAVE(ContinuousService, cs9801);
li98.addService(cs9801, true);
SAVE(JourneyPattern, li98);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Non concurrency rules
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//shared_ptr<NonConcurrencyRule> nc01
//cl93.addConcurrencyRule(cl92.get());

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Data sources
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DataSource ds05(16607023625928705ULL);
ds05.set<Name>("test source");
SAVE(DataSource, ds05);
