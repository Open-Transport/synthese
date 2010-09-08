
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

using namespace synthese::resa;
using namespace synthese::geography;
using namespace synthese::road;
using namespace synthese::util;
using namespace synthese::graph;
using namespace synthese::impex;
using namespace synthese::pt;

using namespace geos::geom;

GeographyModule::PreInit();

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Cities
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

City city54(1688849860511154,"City54");
City city95(1688849860511295,"City95");
City city38(1688849860530938,"City38");

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

StopPoint ps73(3377699720880573,"93/73", &place93,521000.0,1847000.0);
ps73.setCodeBySource("CTP:StopArea:SPOCE87446179");
place93.addPhysicalStop(ps73);

Address a89(562949953421389, &place93,521000.0,1845000.0);
place93.addAddress(&a89);

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

StopPoint ps74(3377699720880574,"94/74", &place94,523000,1844000);
place94.addPhysicalStop(ps74);

StopPoint ps75(3377699720880575,"94/75", &place94,523000.0,1845000.0);
ps75.setCodeBySource("CTP:StopArea:SPOCE87353573");
place94.addPhysicalStop(ps75);

Address a74(562949953421374, &place94,523000.0,1844000.0);
place94.addAddress(&a74);

// Place 95
StopArea place95(1970324837184595,false,minutes(8));
place95.setName("95");
place95.setCity(&city95);
place95.setAllowedConnection(false);
place95.addTransferDelay(562949953421397,377699720880576,minutes(0));

StopPoint ps76(3377699720880576,"95/76", &place95,523266.0,1845339.0);
ps76.setCodeBySource("CTP:StopArea:SPOCE87611939");
place95.addPhysicalStop(ps76);

Address a97(562949953421397, &place95,523266.0,1845341.0);
place95.addAddress(&a97);

// Place 96
StopArea place96(1970324837184596,false,minutes(8));
place96.setName("96");
place96.setCity(&city38);
place96.setAllowedConnection(false);

StopPoint ps77(3377699720880577,"96/77", &place96,524000.0,1849500.0);
place96.addPhysicalStop(ps77);

StopPoint ps78(3377699720880578,"96/78", &place96,524000.0,1849500.0);
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

StopPoint ps79(3377699720880579,"97/79", &place97,526000,1845000);
ps79.setCodeBySource("CTP:StopArea:SPOCE87353599");
place97.addPhysicalStop(ps79);

StopPoint ps80(3377699720880580,"97/80", &place97,526000.0,1847000.0);
place97.addPhysicalStop(ps80);

Address a96(562949953421396, &place97,526000.0,1847000.0);
place97.addAddress(&a96);

Address a99(562949953421399, &place97,526000.0,1845000.0);
place97.addAddress(&a99);

// Place 98
StopArea place98(1970324837184598,false,minutes(8));
place98.setName("98");
place98.setCity(&city38);
place98.setAllowedConnection(true);
/*'562949953421391:3377699720880581:0,562949953421391:3377699720880582:0,3377699720880581:562949953421391:0,3377699720880582:562949953421391:0'*/

StopPoint ps81(3377699720880581,"98/81", &place98,525000.0,1844000.0);
ps81.setCodeBySource("CTP:StopArea:SPOCE87353581");
place98.addPhysicalStop(ps81);

StopPoint ps82(3377699720880582,"98/82", &place98,525000.0,1844000.0);
place98.addPhysicalStop(ps82);

Address a91(562949953421391, &place98,525000.0,1844000.0);
place98.addAddress(&a91);

// Place 99
StopArea place99(1970324837184599,true,minutes(8));
place99.setName("99");
place99.setCity(&city38);
// connection type 5
city38.addIncludedPlace(&place99);
place99.addTransferDelay(3377699720880585,562949953421390,minutes(0));
place99.addTransferDelay(562949953421390,3377699720880585,minutes(0));

StopPoint ps83(3377699720880583,"99/83", &place99,528000.0,1849000.0);
ps83.setCodeBySource("CTP:StopArea:SPOCE87611467");

StopPoint ps84(3377699720880584,"99/84", &place99,528000.0,1845000.0);
place99.addPhysicalStop(ps84);

StopPoint ps85(3377699720880585,"99/85", &place99,528000.0,1847000.0);
place99.addPhysicalStop(ps85);

Address a90(562949953421390, &place99,528000.0,1844000.0);
place99.addAddress(&a90);

Address a94(562949953421394, &place99,523000.0,1845342.0);
place99.addAddress(&a94);

// Place 05
StopArea place05(1970324837184605, true, minutes(8));
place05.setName("05");
place05.setCity(&city54);
place05.setAllowedConnection(true);
place05.addTransferDelay(562949953421410,3377699720880586,minutes(0));

StopPoint ps86(3377699720880586,"05/86", &place05,520000,1845000.0);
place05.addPhysicalStop(ps86);

Address a10(562949953421410, &place05,520000.0,1844000.0);
place05.addAddress(&a10);

// Place 06
StopArea place06(1970324837184606, true, minutes(8));
place06.setName("06");
place06.setCity(&city54);
place06.setAllowedConnection(true);

StopPoint ps06(3377699720880606,"06", &place06,526000,1852000.0);
place06.addPhysicalStop(ps06);

// Place 07
StopArea place07(1970324837184607, true, minutes(8));
place07.setName("07");
place07.setCity(&city54);
place07.setAllowedConnection(true);

StopPoint ps07(3377699720880607,"07", &place07,523000,1852000.0);
place07.addPhysicalStop(ps07);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Crossings
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Crossing c86(12103423998558286);
Address a86(562949953421386,NULL,523000.0,1845342.0);
c86.setAddress(&a86);
a86.setHub(&c86);

Crossing c88(12103423998558288);
Address a88(562949953421388,NULL,523000.0,1845342.0);
c88.setAddress(&a88);
a88.setHub(&c88);

Crossing c98(12103423998558298);
Address a98(562949953421398,NULL,523000.0,1845342.0);
c98.setAddress(&a98);
a98.setHub(&c98);

Crossing c92(12103423998558292);
Address a92(562949953421392,NULL,523263.0,1845331.0);
c92.setAddress(&a92);
a92.setHub(&c92);

Crossing c93(12103423998558293);
Address a93(562949953421393,NULL,523000.0,1845342.0);
c93.setAddress(&a93);
a93.setHub(&c93);


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
li92.setUseInDepartureBoards(true);
li92.setUseInTimetables(true);
li92.setUseInRoutePlanning(true);
li92.setRollingStock(&rs57);
li92.setCommercialLine(&cl92);

LineStop ls86(2814749767106586, &li92, 0, true, false, 0, &ps86);
li92.addEdge(ls86);

LineStop ls63(2814749767106563, &li92, 1, true, true, 100, &ps73);
li92.addEdge(ls63);

LineStop ls61(2814749767106561, &li92, 2, true, true, 5500, &ps75);
li92.addEdge(ls61);

LineStop ls62(2814749767106562, &li92, 3, true, true, 6400, &ps79);
li92.addEdge(ls62);

LineStop ls60(2814749767106560, &li92, 4, false, true, 6500, &ps85);
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

	ss01.setDepartureSchedules(d);
	ss01.setArrivalSchedules(a);
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

	cs97.setArrivalSchedules(a);
	cs97.setDepartureSchedules(d);
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

	ss02.setArrivalSchedules(a);
	ss02.setDepartureSchedules(d);
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
li93.setUseInDepartureBoards(true);
li93.setUseInTimetables(true);
li93.setUseInRoutePlanning(true);
li93.setRollingStock(&rs57);
li93.setCommercialLine(&cl93);

LineStop ls64(2814749767106564, &li93, 0, true, false, 6500, &ps73);
li93.addEdge(ls64);

LineStop ls65(2814749767106565, &li93, 1, true, true, 12141, &ps81);
{
	LineStop::ViaPoints v;
	v.push_back(new Coordinate(523000,1843000));
	v.push_back(new Coordinate(525000,1843000));
	ls65.setViaPoints(v);
}
li93.addEdge(ls65);

LineStop ls66(2814749767106566, &li93, 2, false, true, 12141, &ps75);
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

	ss18.setArrivalSchedules(a);
	ss18.setDepartureSchedules(d);
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

	ss08.setArrivalSchedules(a);
	ss08.setDepartureSchedules(d);
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

	ss07.setArrivalSchedules(a);
	ss07.setDepartureSchedules(d);
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
li94.setUseInDepartureBoards(true);
li94.setUseInTimetables(true);
li94.setUseInRoutePlanning(true);
li94.setRollingStock(&rs57);
li94.setCommercialLine(&cl94);

LineStop ls67(2814749767106567, &li94, 0, true, false, 14141, &ps83);
li94.addEdge(ls67);

LineStop ls69(2814749767106569, &li94, 1, true, true, 15141, &ps85);
li94.addEdge(ls69);

LineStop ls71(2814749767106571, &li94, 2, true, true, 16141, &ps80);
li94.addEdge(ls71);

LineStop ls70(2814749767106570, &li94, 3, true, true, 17141, &ps76);
li94.addEdge(ls70);

LineStop ls68(2814749767106568, &li94, 4, true, true, 17641, &ps73);
li94.addEdge(ls68);

LineStop ls9407(2814749767109407, &li94, 5, false, true, 18641, &ps07);
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

	ss11.setArrivalSchedules(a);
	ss11.setDepartureSchedules(d);
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

	ss06.setArrivalSchedules(a);
	ss06.setDepartureSchedules(d);
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
li95.setUseInDepartureBoards(true);
li95.setUseInTimetables(true);
li95.setUseInRoutePlanning(true);
li95.setRollingStock(&rs57);
li95.setCommercialLine(&cl95);

LineStop ls72(2814749767106572, &li95, 0, true, false, 17641, &ps75);
li95.addEdge(ls72);

LineStop ls73(2814749767106573, &li95, 1, false, true, 17641, &ps84);
li95.addEdge(ls73);

ScheduledService ss09(4503599627370509, "09", &li95);
{
	ScheduledService::Schedules a;
	ScheduledService::Schedules d;
	a.push_back(time_duration(0,0,0));
	d.push_back(time_duration(10,5,0));

	a.push_back(time_duration(10,20,0));
	d.push_back(time_duration(0,0,0));

	ss09.setArrivalSchedules(a);
	ss09.setDepartureSchedules(d);
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

	ss10.setArrivalSchedules(a);
	ss10.setDepartureSchedules(d);
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
li96.setUseInDepartureBoards(true);
li96.setUseInTimetables(true);
li96.setUseInRoutePlanning(true);
li96.setRollingStock(&rs57);
li96.setCommercialLine(&cl96);

LineStop ls74(2814749767106574, &li96, 0, true, false, 17641, &ps79);
li96.addEdge(ls74);

LineStop ls75(2814749767106575, &li96, 1, false, true, 17641, &ps85);
li96.addEdge(ls75);

ScheduledService ss13(4503599627370513, "13", &li96);
{
	ScheduledService::Schedules a;
	ScheduledService::Schedules d;
	a.push_back(time_duration(0,0,0));
	d.push_back(time_duration(19,55,0));

	a.push_back(time_duration(20,5,0));
	d.push_back(time_duration(0,0,0));

	ss13.setArrivalSchedules(a);
	ss13.setDepartureSchedules(d);
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

	ss14.setArrivalSchedules(a);
	ss14.setDepartureSchedules(d);
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
li97.setUseInDepartureBoards(true);
li97.setUseInTimetables(true);
li97.setUseInRoutePlanning(true);
li97.setRollingStock(&rs57);
li97.setCommercialLine(&cl97);

LineStop ls9773(2814749767109773, &li97, 0, true, false, 0, &ps73);
li97.addEdge(ls9773);

LineStop ls9706(2814749767109706, &li97, 1, false, true, 2000, &ps06);
li97.addEdge(ls9706);

ContinuousService cs9701(4503599627379701, "9701", &li97);
{
	ScheduledService::Schedules a;
	ScheduledService::Schedules d;
	a.push_back(time_duration(0,0,0));
	d.push_back(time_duration(21,40,0));

	a.push_back(time_duration(21,42,0));
	d.push_back(time_duration(0,0,0));

	cs9701.setArrivalSchedules(a);
	cs9701.setDepartureSchedules(d);
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
li98.setUseInDepartureBoards(true);
li98.setUseInTimetables(true);
li98.setUseInRoutePlanning(true);
li98.setRollingStock(&rs57);
li98.setCommercialLine(&cl98);

LineStop ls9806(2814749767109806, &li98, 0, true, false, 0, &ps06);
li98.addEdge(ls9806);

LineStop ls9807(2814749767109807, &li98, 1, false, true, 2000, &ps07);
li98.addEdge(ls9807);

ContinuousService cs9801(4503599627379801, "9801", &li98);
{
	ScheduledService::Schedules a;
	ScheduledService::Schedules d;
	a.push_back(time_duration(0,0,0));
	d.push_back(time_duration(21,50,0));

	a.push_back(time_duration(22,00,0));
	d.push_back(time_duration(0,0,0));

	cs9801.setArrivalSchedules(a);
	cs9801.setDepartureSchedules(d);
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
// Roads
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Road 40

RoadPlace rp40(16888498602639440);
rp40.setName("40");
rp40.setCity(&city54);

Road ro40(4222124650659840);
ro40.setRoadPlace(rp40);

RoadChunk rc87(3940649673949187, &a86, 0, &ro40, 0);
ro40.addRoadChunk(rc87);

RoadChunk rc88(3940649673949188, &a89, 1, &ro40, 200);
ro40.addRoadChunk(rc88);

// Road 41

RoadPlace rp41(16888498602639441);
rp41.setName("41");
rp41.setCity(&city54);

Road ro41(4222124650659841);
ro41.setRoadPlace(rp41);

RoadChunk rc84(3940649673949184, &a88, 0, &ro41, 0);
ro41.addRoadChunk(rc84);

RoadChunk rc85(3940649673949185, &a86, 1, &ro41, 300);
ro41.addRoadChunk(rc85);

RoadChunk rc86(3940649673949186, &a74, 2, &ro41, 750);
ro41.addRoadChunk(rc86);

RoadChunk rc75(3940649673949175, &a93, 3, &ro41, 1700);
ro41.addRoadChunk(rc75);


// Road 42

RoadPlace rp42(16888498602639442);
rp42.setName("42");
rp42.setCity(&city38);

Road ro42(4222124650659842);
ro42.setRoadPlace(rp42);

RoadChunk rc97(3940649673949197, &a98, 0, &ro42, 0);
ro42.addRoadChunk(rc97);

RoadChunk rc96(3940649673949196, &a97, 1, &ro42, 200);
ro42.addRoadChunk(rc96);

Road ro44(4222124650659844);
ro44.setRoadPlace(rp42);

RoadChunk rc91(3940649673949191, &a98, 0, &ro44, 0);
ro44.addRoadChunk(rc91);

RoadChunk rc92(3940649673949192, &a93, 1, &ro44, 800);
ro44.addRoadChunk(rc92);

RoadChunk rc94(3940649673949194, &a94, 2, &ro44, 1600);
ro44.addRoadChunk(rc94);


// Road 43

RoadPlace rp43(16888498602639443);
rp43.setName("43");
rp43.setCity(&city38);

Road ro43(4222124650659843);
ro43.setRoadPlace(rp43);

RoadChunk rc00(3940649673949200, &a93, 0, &ro43, 0);
ro43.addRoadChunk(rc00);

RoadChunk rc98(3940649673949198, &a96, 1, &ro43, 100);
ro43.addRoadChunk(rc98);

RoadChunk rc99(3940649673949199, &a99, 2, &ro43, 150);
ro43.addRoadChunk(rc99);


// Road 45

RoadPlace rp45(16888498602639445);
rp45.setName("45");
rp45.setCity(&city54);

Road ro45(4222124650659845);
ro45.setRoadPlace(rp45);

RoadChunk rc11(3940649673949411, &a86, 0, &ro45, 0);
ro45.addRoadChunk(rc11);

RoadChunk rc10(3940649673949410, &a10, 1, &ro45, 400);
ro45.addRoadChunk(rc10);


// Road 46

RoadPlace rp46(16888498602639446);
rp46.setName("46");
rp46.setCity(&city38);

Road ro46(4222124650659846);
ro46.setRoadPlace(rp46);

RoadChunk rc95(3940649673949295, &a93, 0, &ro46, 0);
ro46.addRoadChunk(rc95);

RoadChunk rc89(3940649673949189, &a91, 1, &ro46, 400);
ro46.addRoadChunk(rc89);

RoadChunk rc90(3940649673949190, &a90, 2, &ro46, 2000);
ro46.addRoadChunk(rc90);


// Road 47

RoadPlace rp47(16888498602639447);
rp47.setName("47");
rp47.setCity(&city54);

Road ro47(4222124650659847);
ro47.setRoadPlace(rp47);

RoadChunk rc78(3940649673949278, &a88, 0, &ro47, 0);
ro47.addRoadChunk(rc78);

RoadChunk rc72(3940649673949272, &a92, 1, &ro47, 200);
ro47.addRoadChunk(rc72);


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Data sources
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DataSource ds05(16607023625928705);
ds05.setName("test source");
ds05.setFormat("Trident");
