
/** ProjectionTest class implementation.
	@file ProjectionTest.cpp

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
#include "LineStop.h"
#include "PTRoutePlannerResult.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "LinePhysicalStop.hpp"
#include "Journey.h"
#include "JourneyPattern.hpp"
#include "PermanentService.h"
#include "JourneyPattern.hpp"
#include "Road.h"
#include "RoadPath.hpp"
#include "RoadChunk.h"
#include "RoadChunkEdge.hpp"
#include "Crossing.h"
#include "Address.h"
#include "GeographyModule.h"
#include "TestUtils.hpp"

#include <boost/test/auto_unit_test.hpp>

using namespace boost::posix_time;

using namespace synthese;
using namespace synthese::pt;
using namespace synthese::road;
using namespace synthese::graph;
using namespace synthese::geography;
using namespace synthese::pt_journey_planner;
using namespace synthese::util;

BOOST_AUTO_TEST_CASE (placesListOrder_OrderingTests)
{
	Env env;
	GeographyModule::PreInit();
	ScopedRegistrable<StopArea> scopedStopArea;

	ptime now(second_clock::local_time());

	JourneyPattern L;
	PermanentService S(0, &L, boost::posix_time::minutes(5));

	StopArea CA;
	CA.setName("A");
	StopPoint PA;
	PA.setHub(&CA);
	LineStop A(0, &L, 0);
	A.set<LineNode>(PA);
	A.link(env);

	StopArea CB;
	CB.setName("B");
	StopPoint PB;
	PB.setHub(&CB);
	LineStop B(0, &L, 1);
	B.set<LineNode>(PB);
	B.link(env);

	StopArea CC;
	CC.setName("C");
	StopPoint PC;
	PC.setHub(&CC);
	LineStop C(0, &L, 2);
	C.set<LineNode>(PC);
	C.link(env);

	StopArea CD;
	CD.setName("D");
	StopPoint PD;
	PD.setHub(&CD);
	LineStop D(0, &L, 3);
	D.set<LineNode>(PD);
	D.link(env);

	StopArea CE;
	CE.setName("E");
	StopPoint PE;
	PE.setHub(&CE);
	LineStop E(0, &L, 4);
	E.set<LineNode>(PE);
	E.link(env);

	StopArea CF;
	CF.setName("F");
	StopPoint PF;
	PF.setHub(&CF);
	LineStop F(0, &L, 5);
	F.set<LineNode>(PF);
	F.link(env);

	Journey j0;
	ServicePointer sp0_0(true, false, USER_PEDESTRIAN, S, now);
	sp0_0.setDepartureInformations(**A.getGeneratedLineStops().begin(), now, now, PA);
	sp0_0.setArrivalInformations(**B.getGeneratedLineStops().begin(), now, now, PB);
	j0.append(sp0_0);

	PTRoutePlannerResult::Journeys j;
	j.push_back(j0);

	{
		PTRoutePlannerResult r(&CA, &CB, false, j);
		const PTRoutePlannerResult::PlacesListConfiguration::List& l(r.getOrderedPlaces().getResult());
		PTRoutePlannerResult::PlacesListConfiguration::List::const_iterator itl(l.begin());
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CA.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CB.getName());
		++itl;
		BOOST_CHECK(itl == l.end());
	}

	Journey j1;
	ServicePointer sp1_0(true, false, USER_PEDESTRIAN, S, now);
	sp1_0.setDepartureInformations(**A.getGeneratedLineStops().begin(), now, now, PA);
	sp1_0.setArrivalInformations(**C.getGeneratedLineStops().begin(), now, now, PC);
	j1.append(sp1_0);
	ServicePointer sp1_1(true, false, USER_PEDESTRIAN, S, now);
	sp1_1.setDepartureInformations(**C.getGeneratedLineStops().begin(), now, now, PC);
	sp1_1.setArrivalInformations(**B.getGeneratedLineStops().begin(), now, now, PB);
	j1.append(sp1_1);
	j.push_back(j1);

	{
		PTRoutePlannerResult r(&CA, &CB, false, j);
		const PTRoutePlannerResult::PlacesListConfiguration::List& l(r.getOrderedPlaces().getResult());
		PTRoutePlannerResult::PlacesListConfiguration::List::const_iterator itl(l.begin());
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CA.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CC.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CB.getName());
		++itl;
		BOOST_CHECK(itl == l.end());
	}

	Journey j2;
	ServicePointer s2_0(true, false, USER_PEDESTRIAN, S, now);
	s2_0.setDepartureInformations(**A.getGeneratedLineStops().begin(), now, now, PA);
	s2_0.setArrivalInformations(**D.getGeneratedLineStops().begin(), now, now, PD);
	j2.append(s2_0);
	ServicePointer s2_1(true, false, USER_PEDESTRIAN, S, now);
	s2_1.setDepartureInformations(**D.getGeneratedLineStops().begin(), now, now, PD);
	s2_1.setArrivalInformations(**B.getGeneratedLineStops().begin(), now, now, PB);
	j2.append(s2_1);
	j.push_back(j2);

	{
		PTRoutePlannerResult r(&CA, &CB, false, j);
		const PTRoutePlannerResult::PlacesListConfiguration::List& l(r.getOrderedPlaces().getResult());
		PTRoutePlannerResult::PlacesListConfiguration::List::const_iterator itl(l.begin());
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK_EQUAL(itl->place->getName(), CA.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK_EQUAL(itl->place->getName(), CD.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK_EQUAL(itl->place->getName(), CC.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK_EQUAL(itl->place->getName(), CB.getName());
		++itl;
		BOOST_CHECK(itl == l.end());
	}

	Journey j3;
	ServicePointer s3_0(true, false, USER_PEDESTRIAN, S, now);
	s3_0.setDepartureInformations(**A.getGeneratedLineStops().begin(), now, now, PA);
	s3_0.setArrivalInformations(**C.getGeneratedLineStops().begin(), now, now, PC);
	ServicePointer s3_1(true, false, USER_PEDESTRIAN, S, now);
	s3_1.setDepartureInformations(**C.getGeneratedLineStops().begin(), now, now, PC);
	s3_1.setArrivalInformations(**D.getGeneratedLineStops().begin(), now, now, PD);
	j3.append(s3_1);
	ServicePointer s3_2(true, false, USER_PEDESTRIAN, S, now);
	s3_2.setDepartureInformations(**D.getGeneratedLineStops().begin(), now, now, PD);
	s3_2.setArrivalInformations(**B.getGeneratedLineStops().begin(), now, now, PB);
	j3.append(s3_2);
	j.push_back(j3);

	{
		PTRoutePlannerResult r(&CA, &CB, false, j);
		const PTRoutePlannerResult::PlacesListConfiguration::List& l(r.getOrderedPlaces().getResult());
		PTRoutePlannerResult::PlacesListConfiguration::List::const_iterator itl(l.begin());
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK_EQUAL(itl->place->getName(), CA.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK_EQUAL(itl->place->getName(), CC.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK_EQUAL(itl->place->getName(), CD.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK_EQUAL(itl->place->getName(), CB.getName());
		++itl;
		BOOST_CHECK(itl == l.end());
	}

	Journey j4;
	ServicePointer s4_0(true, false, USER_PEDESTRIAN, S, now);
	s4_0.setDepartureInformations(**A.getGeneratedLineStops().begin(), now, now, PA);
	s4_0.setArrivalInformations(**E.getGeneratedLineStops().begin(), now, now, PE);
	j4.append(s4_0);
	ServicePointer s4_1(true, false, USER_PEDESTRIAN, S, now);
	s4_1.setDepartureInformations(**E.getGeneratedLineStops().begin(), now, now, PE);
	s4_1.setArrivalInformations(**F.getGeneratedLineStops().begin(), now, now, PF);
	j4.append(s4_1);
	ServicePointer s4_2(true, false, USER_PEDESTRIAN, S, now);
	s4_2.setDepartureInformations(**F.getGeneratedLineStops().begin(), now, now, PF);
	s4_2.setArrivalInformations(**B.getGeneratedLineStops().begin(), now, now, PB);
	j4.append(s4_2);
	j.push_back(j4);

	{
		PTRoutePlannerResult r(&CA, &CB, false, j);
		const PTRoutePlannerResult::PlacesListConfiguration::List& l(r.getOrderedPlaces().getResult());
		PTRoutePlannerResult::PlacesListConfiguration::List::const_iterator itl(l.begin());
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK_EQUAL(itl->place->getName(), CA.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK_EQUAL(itl->place->getName(), CE.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK_EQUAL(itl->place->getName(), CF.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK_EQUAL(itl->place->getName(), CC.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK_EQUAL(itl->place->getName(), CD.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK_EQUAL(itl->place->getName(), CB.getName());
		++itl;
		BOOST_CHECK(itl == l.end());
	}

	Journey j5;
	ServicePointer s5_0(true, false, USER_PEDESTRIAN, S, now);
	s5_0.setDepartureInformations(**A.getGeneratedLineStops().begin(), now, now, PA);
	s5_0.setArrivalInformations(**C.getGeneratedLineStops().begin(), now, now, PC);
	j5.append(s5_0);
	ServicePointer s5_1(true, false, USER_PEDESTRIAN, S, now);
	s5_1.setDepartureInformations(**C.getGeneratedLineStops().begin(), now, now, PC);
	s5_1.setArrivalInformations(**D.getGeneratedLineStops().begin(), now, now, PD);
	j5.append(s5_1);
	ServicePointer s5_2(true, false, USER_PEDESTRIAN, S, now);
	s5_2.setDepartureInformations(**D.getGeneratedLineStops().begin(), now, now, PD);
	s5_2.setArrivalInformations(**E.getGeneratedLineStops().begin(), now, now, PE);
	j5.append(s5_2);
	ServicePointer s5_3(true, false, USER_PEDESTRIAN, S, now);
	s5_3.setDepartureInformations(**E.getGeneratedLineStops().begin(), now, now, PE);
	s5_3.setArrivalInformations(**F.getGeneratedLineStops().begin(), now, now, PF);
	j5.append(s5_3);
	ServicePointer s5_4(true, false, USER_PEDESTRIAN, S, now);
	s5_4.setDepartureInformations(**F.getGeneratedLineStops().begin(), now, now, PF);
	s5_4.setArrivalInformations(**B.getGeneratedLineStops().begin(), now, now, PB);
	j5.append(s5_4);
	j.push_back(j5);

	{
		PTRoutePlannerResult r(&CA, &CB, false, j);
		const PTRoutePlannerResult::PlacesListConfiguration::List& l(r.getOrderedPlaces().getResult());
		PTRoutePlannerResult::PlacesListConfiguration::List::const_iterator itl(l.begin());
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK_EQUAL(itl->place->getName(), CA.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK_EQUAL(itl->place->getName(), CC.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK_EQUAL(itl->place->getName(), CD.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK_EQUAL(itl->place->getName(), CE.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK_EQUAL(itl->place->getName(), CF.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK_EQUAL(itl->place->getName(), CB.getName());
		++itl;
		BOOST_CHECK(itl == l.end());
	}

	Journey j6;
	ServicePointer s6_0(true, false, USER_PEDESTRIAN, S, now);
	s6_0.setDepartureInformations(**A.getGeneratedLineStops().begin(), now, now, PA);
	s6_0.setArrivalInformations(**D.getGeneratedLineStops().begin(), now, now, PD);
	j6.append(s6_0);
	ServicePointer s6_1(true, false, USER_PEDESTRIAN, S, now);
	s6_1.setDepartureInformations(**D.getGeneratedLineStops().begin(), now, now, PD);
	s6_1.setArrivalInformations(**C.getGeneratedLineStops().begin(), now, now, PC);
	j6.append(s6_1);
	ServicePointer s6_2(true, false, USER_PEDESTRIAN, S, now);
	s6_2.setDepartureInformations(**C.getGeneratedLineStops().begin(), now, now, PC);
	s6_2.setArrivalInformations(**B.getGeneratedLineStops().begin(), now, now, PB);
	j6.append(s6_2);
	j.push_back(j6);

	{
		PTRoutePlannerResult r(&CA, &CB, false, j);
		const PTRoutePlannerResult::PlacesListConfiguration::List& l(r.getOrderedPlaces().getResult());
		PTRoutePlannerResult::PlacesListConfiguration::List::const_iterator itl(l.begin());
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK_EQUAL(itl->place->getName(), CA.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK_EQUAL(itl->place->getName(), CC.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK_EQUAL(itl->place->getName(), CD.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK_EQUAL(itl->place->getName(), CC.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK_EQUAL(itl->place->getName(), CE.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK_EQUAL(itl->place->getName(), CF.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK_EQUAL(itl->place->getName(), CB.getName());
		++itl;
		BOOST_CHECK(itl == l.end());
	}
}

BOOST_AUTO_TEST_CASE (placesListOrder_RoadChunks)
{
	Env env;
	ptime now(second_clock::local_time());

	JourneyPattern L;
	PermanentService S(0, &L, boost::posix_time::minutes(5));

	StopArea CA;
	CA.setName("A");
	StopPoint PA;
	PA.setHub(&CA);
	LineStop A;
	A.set<Line>(L);
	A.set<LineNode>(PA);
	A.set<RankInPath>(0);
	A.link(env);

	StopArea CB;
	CB.setName("B");
	StopPoint PB;
	PB.setHub(&CB);
	LineStop B;
	B.set<Line>(L);
	B.set<LineNode>(PB);
	B.set<RankInPath>(1);
	B.link(env);

	StopArea CC;
	CC.setName("C");
	StopPoint PC;
	PC.setHub(&CC);
	LineStop C;
	C.set<Line>(L);
	C.set<LineNode>(PC);
	C.set<RankInPath>(2);
	C.link(env);

	StopArea CD;
	CD.setName("D");
	StopPoint PD;
	PD.setHub(&CD);
	LineStop D;
	D.set<Line>(L);
	D.set<LineNode>(PD);
	D.set<RankInPath>(3);
	D.link(env);

	StopArea CE;
	CE.setName("E");
	StopPoint PE;
	PE.setHub(&CE);
	LineStop E;
	E.set<Line>(L);
	E.set<LineNode>(PE);
	E.set<RankInPath>(4);
	E.link(env);

	StopArea CF;
	CF.setName("F");
	StopPoint PF;
	PF.setHub(&CF);
	LineStop F;
	F.set<Line>(L);
	F.set<LineNode>(PF);
	F.set<RankInPath>(5);
	F.link(env);
	
	Road R;
	R.link(env);
	PermanentService Sr(0, &R.getForwardPath(), boost::posix_time::minutes(5));

	Crossing Cr0;
	Crossing Cr1;
	Crossing Cr2;

	RoadChunk Ch0(0, &Cr0, 0, &R);
	Ch0.link(env);
	RoadChunk Ch1(0, &Cr1, 1, &R);
	Ch1.link(env);
	RoadChunk Ch2(0, &Cr2, 2, &R);
	Ch2.link(env);

	Journey j0;
	ServicePointer s0_0(true, false, USER_PEDESTRIAN, Sr, now);
	s0_0.setDepartureInformations(**A.getGeneratedLineStops().begin(), now, now, PA);
	s0_0.setArrivalInformations(Ch0.getForwardEdge(), now, now, Cr0);
	j0.append(s0_0);
	ServicePointer s0_1(true, false, USER_PEDESTRIAN, Sr, now);
	s0_1.setDepartureInformations(Ch0.getForwardEdge(), now, now, Cr0);
	s0_1.setArrivalInformations(Ch1.getForwardEdge(), now, now, Cr1);
	j0.append(s0_1);
	ServicePointer s0_2(true, false, USER_PEDESTRIAN, Sr, now);
	s0_2.setDepartureInformations(Ch1.getForwardEdge(), now, now, Cr1);
	s0_2.setArrivalInformations(**C.getGeneratedLineStops().begin(), now, now, PC);
	j0.append(s0_2);
	ServicePointer s0_3(true, false, USER_PEDESTRIAN, S, now);
	s0_3.setDepartureInformations(**C.getGeneratedLineStops().begin(), now, now, PC);
	s0_3.setArrivalInformations(**B.getGeneratedLineStops().begin(), now, now, PB);
	j0.append(s0_3);

	PTRoutePlannerResult::Journeys j;
	j.push_back(j0);

	{
		PTRoutePlannerResult r(&CA, &CB, false, j);
		const PTRoutePlannerResult::PlacesListConfiguration::List& l(r.getOrderedPlaces().getResult());
		PTRoutePlannerResult::PlacesListConfiguration::List::const_iterator itl(l.begin());
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CA.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CC.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CB.getName());
		++itl;
		BOOST_CHECK(itl == l.end());
	}

	Journey j1;
	ServicePointer s1_0(true, false, USER_PEDESTRIAN, S, now);
	s1_0.setDepartureInformations(**A.getGeneratedLineStops().begin(), now, now, PA);
	s1_0.setArrivalInformations(**E.getGeneratedLineStops().begin(), now, now, PE);
	j1.append(s1_0);
	ServicePointer s1_1(true, false, USER_PEDESTRIAN, Sr, now);
	s1_1.setDepartureInformations(**E.getGeneratedLineStops().begin(), now, now, PE);
	s1_1.setArrivalInformations(Ch2.getForwardEdge(), now, now, Cr2);
	j1.append(s1_1);
	ServicePointer s1_2(true, false, USER_PEDESTRIAN, Sr, now);
	s1_2.setDepartureInformations(Ch2.getForwardEdge(), now, now, Cr2);
	s1_2.setArrivalInformations(**B.getGeneratedLineStops().begin(), now, now, PB);
	j1.append(s1_2);

	j.push_back(j1);

	{
		PTRoutePlannerResult r(&CA, &CB, false, j);
		const PTRoutePlannerResult::PlacesListConfiguration::List& l(r.getOrderedPlaces().getResult());
		PTRoutePlannerResult::PlacesListConfiguration::List::const_iterator itl(l.begin());
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CA.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CE.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CC.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CB.getName());
		++itl;
		BOOST_CHECK(itl == l.end());
	}

	Journey j2;
	ServicePointer s2_0(true, false, USER_PEDESTRIAN, Sr, now);
	s2_0.setDepartureInformations(Ch0.getForwardEdge(), now, now, Cr0);
	s2_0.setArrivalInformations(**D.getGeneratedLineStops().begin(), now, now, PD);
	j2.append(s2_0);
	ServicePointer s2_1(true, false, USER_PEDESTRIAN, S, now);
	s2_1.setDepartureInformations(**D.getGeneratedLineStops().begin(), now, now, PD);
	s2_1.setArrivalInformations(**B.getGeneratedLineStops().begin(), now, now, PB);
	j2.append(s2_1);

	j.push_back(j2);

	{
		PTRoutePlannerResult r(&CA, &CB, false, j);
		const PTRoutePlannerResult::PlacesListConfiguration::List& l(r.getOrderedPlaces().getResult());
		PTRoutePlannerResult::PlacesListConfiguration::List::const_iterator itl(l.begin());
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CA.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CD.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CE.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CC.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CB.getName());
		++itl;
		BOOST_CHECK(itl == l.end());
	}

	Journey j3;
	ServicePointer s3_0(true, false, USER_PEDESTRIAN, S, now);
	s3_0.setDepartureInformations(**A.getGeneratedLineStops().begin(), now, now, PA);
	s3_0.setArrivalInformations(**F.getGeneratedLineStops().begin(), now, now, PF);
	j3.append(s3_0);
	ServicePointer s3_1(true, false, USER_PEDESTRIAN, Sr, now);
	s3_1.setDepartureInformations(**F.getGeneratedLineStops().begin(), now, now, PF);
	s3_1.setArrivalInformations(Ch2.getForwardEdge(), now, now, Cr2);
	j3.append(s3_1);

	j.push_back(j3);

	{
		PTRoutePlannerResult r(&CA, &CB, false, j);
		const PTRoutePlannerResult::PlacesListConfiguration::List& l(r.getOrderedPlaces().getResult());
		PTRoutePlannerResult::PlacesListConfiguration::List::const_iterator itl(l.begin());
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CA.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CF.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CD.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CE.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CC.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CB.getName());
		++itl;
		BOOST_CHECK(itl == l.end());
	}

	Journey j4;
	ServicePointer s4_0(true, false, USER_PEDESTRIAN, Sr, now);
	s4_0.setDepartureInformations(Ch0.getForwardEdge(), now, now, Cr0);
	s4_0.setArrivalInformations(**F.getGeneratedLineStops().begin(), now, now, PF);
	j4.append(s4_0);
	ServicePointer s4_1(true, false, USER_PEDESTRIAN, Sr, now);
	s4_1.setDepartureInformations(**F.getGeneratedLineStops().begin(), now, now, PF);
	s4_1.setArrivalInformations(Ch2.getForwardEdge(), now, now, Cr2);
	j4.append(s4_1);

	j.push_back(j4);

	{
		PTRoutePlannerResult r(&CA, &CB, false, j);
		const PTRoutePlannerResult::PlacesListConfiguration::List& l(r.getOrderedPlaces().getResult());
		PTRoutePlannerResult::PlacesListConfiguration::List::const_iterator itl(l.begin());
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CA.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CF.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CD.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CE.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CC.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CB.getName());
		++itl;
		BOOST_CHECK(itl == l.end());
	}

	Journey j5;
	ServicePointer s5_0(true, false, USER_PEDESTRIAN, Sr, now);
	s5_0.setDepartureInformations(Ch0.getForwardEdge(), now, now, Cr0);
	s5_0.setArrivalInformations(Ch1.getForwardEdge(), now, now, Cr1);
	j5.append(s5_0);

	j.push_back(j5);

	{
		PTRoutePlannerResult r(&CA, &CB, false, j);
		const PTRoutePlannerResult::PlacesListConfiguration::List& l(r.getOrderedPlaces().getResult());
		PTRoutePlannerResult::PlacesListConfiguration::List::const_iterator itl(l.begin());
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CA.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CF.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CD.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CE.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CC.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CB.getName());
		++itl;
		BOOST_CHECK(itl == l.end());
	}
}

BOOST_AUTO_TEST_CASE (placesListOrder_DoubleOriginsDestinationsTest)
{
	Env env;
	ptime now(second_clock::local_time());

	JourneyPattern L;
	PermanentService S(0, &L, boost::posix_time::minutes(5));

	StopArea CA;
	CA.setName("A");
	StopPoint PA;
	PA.setHub(&CA);
	LineStop A;
	A.set<Line>(L);
	A.set<LineNode>(PA);
	A.set<RankInPath>(0);
	A.link(env);

	StopArea CB;
	CB.setName("B");
	StopPoint PB;
	PB.setHub(&CB);
	LineStop B;
	B.set<Line>(L);
	B.set<LineNode>(PB);
	B.set<RankInPath>(1);
	B.link(env);

	StopArea CC;
	CC.setName("C");
	StopPoint PC;
	PC.setHub(&CC);
	LineStop C;
	C.set<Line>(L);
	C.set<LineNode>(PC);
	C.set<RankInPath>(2);
	C.link(env);

	StopArea CD;
	CD.setName("D");
	StopPoint PD;
	PD.setHub(&CD);
	LineStop D;
	D.set<Line>(L);
	D.set<LineNode>(PD);
	D.set<RankInPath>(3);
	D.link(env);

	StopArea CE;
	CE.setName("E");
	StopPoint PE;
	PE.setHub(&CE);
	LineStop E;
	E.set<Line>(L);
	E.set<LineNode>(PE);
	E.set<RankInPath>(4);
	E.link(env);

	StopArea CF;
	CF.setName("F");
	StopPoint PF;
	PF.setHub(&CF);
	LineStop F;
	F.set<Line>(L);
	F.set<LineNode>(PF);
	F.set<RankInPath>(5);
	F.link(env);

	{
		Journey j0;
		ServicePointer s0_0(true, false, USER_PEDESTRIAN, S, now);
		s0_0.setDepartureInformations(**D.getGeneratedLineStops().begin(), now, now, PD);
		s0_0.setArrivalInformations(**C.getGeneratedLineStops().begin(), now, now, PC);
		j0.append(s0_0);
		ServicePointer s0_1(true, false, USER_PEDESTRIAN, S, now);
		s0_1.setDepartureInformations(**C.getGeneratedLineStops().begin(), now, now, PC);
		s0_1.setArrivalInformations(**B.getGeneratedLineStops().begin(), now, now, PB);
		j0.append(s0_1);

		Journey j1;
		ServicePointer s1_0(true, false, USER_PEDESTRIAN, S, now);
		s1_0.setDepartureInformations(**E.getGeneratedLineStops().begin(), now, now, PE);
		s1_0.setArrivalInformations(**C.getGeneratedLineStops().begin(), now, now, PC);
		j1.append(s1_0);
		ServicePointer s1_1(true, false, USER_PEDESTRIAN, S, now);
		s1_1.setDepartureInformations(**C.getGeneratedLineStops().begin(), now, now, PC);
		s1_1.setArrivalInformations(**B.getGeneratedLineStops().begin(), now, now, PB);
		j1.append(s1_1);

		PTRoutePlannerResult::Journeys j;
		j.push_back(j0);
		j.push_back(j1);
		PTRoutePlannerResult r(&CA, &CB, false, j);
		const PTRoutePlannerResult::PlacesListConfiguration::List& l(r.getOrderedPlaces().getResult());
		PTRoutePlannerResult::PlacesListConfiguration::List::const_iterator itl(l.begin());
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CE.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CD.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CC.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CB.getName());
		++itl;
		BOOST_CHECK(itl == l.end());
	}

	{
		Journey j0;
		ServicePointer s0_0(true, false, USER_PEDESTRIAN, S, now);
		s0_0.setDepartureInformations(**A.getGeneratedLineStops().begin(), now, now, PA);
		s0_0.setArrivalInformations(**C.getGeneratedLineStops().begin(), now, now, PC);
		j0.append(s0_0);
		ServicePointer s0_1(true, false, USER_PEDESTRIAN, S, now);
		s0_1.setDepartureInformations(**C.getGeneratedLineStops().begin(), now, now, PC);
		s0_1.setArrivalInformations(**D.getGeneratedLineStops().begin(), now, now, PD);
		j0.append(s0_1);

		Journey j1;
		ServicePointer s1_0(true, false, USER_PEDESTRIAN, S, now);
		s1_0.setDepartureInformations(**A.getGeneratedLineStops().begin(), now, now, PA);
		s1_0.setArrivalInformations(**C.getGeneratedLineStops().begin(), now, now, PC);
		j1.append(s1_0);
		ServicePointer s1_1(true, false, USER_PEDESTRIAN, S, now);
		s1_1.setDepartureInformations(**A.getGeneratedLineStops().begin(), now, now, PA);
		s1_1.setArrivalInformations(**E.getGeneratedLineStops().begin(), now, now, PE);
		j1.append(s1_1);

		PTRoutePlannerResult::Journeys j;
		j.push_back(j0);
		j.push_back(j1);
		PTRoutePlannerResult r(&CA, &CB, false, j);
		const PTRoutePlannerResult::PlacesListConfiguration::List& l(r.getOrderedPlaces().getResult());
		PTRoutePlannerResult::PlacesListConfiguration::List::const_iterator itl(l.begin());
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CA.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CC.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CD.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CE.getName());
		++itl;
		BOOST_CHECK(itl == l.end());
	}

	{
		Journey j0;
		ServicePointer s0_0(true, false, USER_PEDESTRIAN, S, now);
		s0_0.setDepartureInformations(**D.getGeneratedLineStops().begin(), now, now, PD);
		s0_0.setArrivalInformations(**E.getGeneratedLineStops().begin(), now, now, PE);
		j0.append(s0_0);

		Journey j1;
		ServicePointer s1_0(true, false, USER_PEDESTRIAN, S, now);
		s1_0.setDepartureInformations(**C.getGeneratedLineStops().begin(), now, now, PC);
		s1_0.setArrivalInformations(**F.getGeneratedLineStops().begin(), now, now, PF);
		j1.append(s1_0);

		PTRoutePlannerResult::Journeys j;
		j.push_back(j0);
		j.push_back(j1);
		PTRoutePlannerResult r(&CA, &CB, false, j);
		const PTRoutePlannerResult::PlacesListConfiguration::List& l(r.getOrderedPlaces().getResult());
		PTRoutePlannerResult::PlacesListConfiguration::List::const_iterator itl(l.begin());
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CC.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CD.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CE.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CF.getName());
		++itl;
		BOOST_CHECK(itl == l.end());
	}

	{ // 7798 The actual destination must always be at the end of the table
		Journey j0;
		ServicePointer s0_0(true, false, USER_PEDESTRIAN, S, now);
		s0_0.setDepartureInformations(**A.getGeneratedLineStops().begin(), now, now, PA);
		s0_0.setArrivalInformations(**B.getGeneratedLineStops().begin(), now, now, PB);
		j0.append(s0_0);

		Journey j1;
		ServicePointer s1_0(true, false, USER_PEDESTRIAN, S, now);
		s1_0.setDepartureInformations(**C.getGeneratedLineStops().begin(), now, now, PC);
		s1_0.setArrivalInformations(**F.getGeneratedLineStops().begin(), now, now, PF);
		j1.append(s1_0);

		PTRoutePlannerResult::Journeys j;
		j.push_back(j0);
		j.push_back(j1);
		PTRoutePlannerResult r(&CA, &CB, false, j);
		const PTRoutePlannerResult::PlacesListConfiguration::List& l(r.getOrderedPlaces().getResult());
		PTRoutePlannerResult::PlacesListConfiguration::List::const_iterator itl(l.begin());
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CC.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CA.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CB.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CF.getName());
		++itl;
		BOOST_CHECK(itl == l.end());
	}

	{ // 7798 The actual destination must always be at the end of the table
		Journey j0;
		ServicePointer s0_0(true, false, USER_PEDESTRIAN, S, now);
		s0_0.setDepartureInformations(**D.getGeneratedLineStops().begin(), now, now, PD);
		s0_0.setArrivalInformations(**F.getGeneratedLineStops().begin(), now, now, PF);
		j0.append(s0_0);

		Journey j1;
		ServicePointer s1_0(true, false, USER_PEDESTRIAN, S, now);
		s1_0.setDepartureInformations(**A.getGeneratedLineStops().begin(), now, now, PA);
		s1_0.setArrivalInformations(**B.getGeneratedLineStops().begin(), now, now, PB);
		j1.append(s1_0);

		PTRoutePlannerResult::Journeys j;
		j.push_back(j0);
		j.push_back(j1);
		PTRoutePlannerResult r(&CA, &CB, false, j);
		const PTRoutePlannerResult::PlacesListConfiguration::List& l(r.getOrderedPlaces().getResult());
		PTRoutePlannerResult::PlacesListConfiguration::List::const_iterator itl(l.begin());
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CA.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CD.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CF.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CB.getName());
		++itl;
		BOOST_CHECK(itl == l.end());
	}

	{ // 13407 Crash if a destination stop is used for transfer too
		Journey j0;
		ServicePointer s0_0(true, false, USER_PEDESTRIAN, S, now);
		s0_0.setDepartureInformations(**A.getGeneratedLineStops().begin(), now, now, PD);
		s0_0.setArrivalInformations(**B.getGeneratedLineStops().begin(), now, now, PF);
		j0.append(s0_0);

		Journey j1;
		ServicePointer s1_0(true, false, USER_PEDESTRIAN, S, now);
		s1_0.setDepartureInformations(**A.getGeneratedLineStops().begin(), now, now, PA);
		s1_0.setArrivalInformations(**B.getGeneratedLineStops().begin(), now, now, PB);
		j1.append(s1_0);
		ServicePointer s1_1(true, false, USER_PEDESTRIAN, S, now);
		s1_1.setDepartureInformations(**B.getGeneratedLineStops().begin(), now, now, PA);
		s1_1.setArrivalInformations(**C.getGeneratedLineStops().begin(), now, now, PB);
		j1.append(s1_1);

		PTRoutePlannerResult::Journeys j;
		j.push_back(j0);
		j.push_back(j1);
		PTRoutePlannerResult r(&CA, &CB, false, j);
		const PTRoutePlannerResult::PlacesListConfiguration::List& l(r.getOrderedPlaces().getResult());
		PTRoutePlannerResult::PlacesListConfiguration::List::const_iterator itl(l.begin());
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CA.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CB.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(!itl->isOrigin);
		BOOST_CHECK(itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CC.getName());
		++itl;
		BOOST_CHECK(itl == l.end());
	}
}
