
/** ProjectionTest class implementation.
	@file ProjectionTest.cpp

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

#include "PTRoutePlannerResult.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "LineStop.h"
#include "Journey.h"
#include "JourneyPattern.hpp"
#include "PermanentService.h"
#include "JourneyPattern.hpp"
#include "Road.h"
#include "RoadChunk.h"
#include "Crossing.h"
#include "Address.h"
#include "GeographyModule.h"

#include <boost/test/auto_unit_test.hpp>

using namespace boost::posix_time;

using namespace synthese::pt;
using namespace synthese::road;
using namespace synthese::graph;
using namespace synthese::geography;
using namespace synthese::pt_journey_planner;


BOOST_AUTO_TEST_CASE (placesListOrder_OrderingTests)
{
	GeographyModule::PreInit();

	ptime now(second_clock::local_time());

	JourneyPattern L;
	PermanentService S(0, &L, boost::posix_time::minutes(5));

	StopArea CA;
	CA.setName("A");
	StopPoint PA;
	PA.setHub(&CA);
	LineStop A;
	A.setLine(&L);
	A.setPhysicalStop(&PA);
	A.setRankInPath(0);

	StopArea CB;
	CB.setName("B");
	StopPoint PB;
	PB.setHub(&CB);
	LineStop B;
	B.setLine(&L);
	B.setPhysicalStop(&PB);
	B.setRankInPath(1);

	StopArea CC;
	CC.setName("C");
	StopPoint PC;
	PC.setHub(&CC);
	LineStop C;
	C.setLine(&L);
	C.setPhysicalStop(&PC);
	C.setRankInPath(2);

	StopArea CD;
	CD.setName("D");
	StopPoint PD;
	PD.setHub(&CD);
	LineStop D;
	D.setLine(&L);
	D.setPhysicalStop(&PD);
	D.setRankInPath(3);

	StopArea CE;
	CE.setName("E");
	StopPoint PE;
	PE.setHub(&CE);
	LineStop E;
	E.setLine(&L);
	E.setPhysicalStop(&PE);
	E.setRankInPath(4);

	StopArea CF;
	CF.setName("F");
	StopPoint PF;
	PF.setHub(&CF);
	LineStop F;
	F.setLine(&L);
	F.setPhysicalStop(&PF);
	F.setRankInPath(5);

	Journey j0;
	ServicePointer sp0_0(false, USER_PEDESTRIAN, S, now);
	sp0_0.setDepartureInformations(A, now, now, PA);
	sp0_0.setArrivalInformations(B, now, now, PB);
	j0 = Journey(j0, sp0_0);

	PTRoutePlannerResult::Journeys j;
	j.push_back(j0);

	{
		PTRoutePlannerResult r(&CA, &CB, false, j);
		const PTRoutePlannerResult::PlacesList& l(r.getOrderedPlaces());
		PTRoutePlannerResult::PlacesList::const_iterator itl(l.begin());
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
	ServicePointer sp1_0(false, USER_PEDESTRIAN, S, now);
	sp1_0.setDepartureInformations(A, now, now, PA);
	sp1_0.setArrivalInformations(C, now, now, PC);
	j1 = Journey(j1, sp1_0);
	ServicePointer sp1_1(false, USER_PEDESTRIAN, S, now);
	sp1_1.setDepartureInformations(C, now, now, PC);
	sp1_1.setArrivalInformations(B, now, now, PB);
	j1 = Journey(j1, sp1_1);
	j.push_back(j1);

	{
		PTRoutePlannerResult r(&CA, &CB, false, j);
		const PTRoutePlannerResult::PlacesList& l(r.getOrderedPlaces());
		PTRoutePlannerResult::PlacesList::const_iterator itl(l.begin());
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
	ServicePointer s2_0(false, USER_PEDESTRIAN, S, now);
	s2_0.setDepartureInformations(A, now, now, PA);
	s2_0.setArrivalInformations(D, now, now, PD);
	j2 = Journey(j2, s2_0);
	ServicePointer s2_1(false, USER_PEDESTRIAN, S, now);
	s2_1.setDepartureInformations(D, now, now, PD);
	s2_1.setArrivalInformations(B, now, now, PB);
	j2 = Journey(j2, s2_1);
	j.push_back(j2);

	{
		PTRoutePlannerResult r(&CA, &CB, false, j);
		const PTRoutePlannerResult::PlacesList& l(r.getOrderedPlaces());
		PTRoutePlannerResult::PlacesList::const_iterator itl(l.begin());
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
	ServicePointer s3_0(false, USER_PEDESTRIAN, S, now);
	s3_0.setDepartureInformations(A, now, now, PA);
	s3_0.setArrivalInformations(C, now, now, PC);
	ServicePointer s3_1(false, USER_PEDESTRIAN, S, now);
	s3_1.setDepartureInformations(C, now, now, PC);
	s3_1.setArrivalInformations(D, now, now, PD);
	j3 = Journey(j3, s3_1);
	ServicePointer s3_2(false, USER_PEDESTRIAN, S, now);
	s3_2.setDepartureInformations(D, now, now, PD);
	s3_2.setArrivalInformations(B, now, now, PB);
	j3 = Journey(j3, s3_2);
	j.push_back(j3);

	{
		PTRoutePlannerResult r(&CA, &CB, false, j);
		const PTRoutePlannerResult::PlacesList& l(r.getOrderedPlaces());
		PTRoutePlannerResult::PlacesList::const_iterator itl(l.begin());
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
	ServicePointer s4_0(false, USER_PEDESTRIAN, S, now);
	s4_0.setDepartureInformations(A, now, now, PA);
	s4_0.setArrivalInformations(E, now, now, PE);
	j4 = Journey(j4, s4_0);
	ServicePointer s4_1(false, USER_PEDESTRIAN, S, now);
	s4_1.setDepartureInformations(E, now, now, PE);
	s4_1.setArrivalInformations(F, now, now, PF);
	j4 = Journey(j4, s4_1);
	ServicePointer s4_2(false, USER_PEDESTRIAN, S, now);
	s4_2.setDepartureInformations(F, now, now, PF);
	s4_2.setArrivalInformations(B, now, now, PB);
	j4 = Journey(j4, s4_2);
	j.push_back(j4);

	{
		PTRoutePlannerResult r(&CA, &CB, false, j);
		const PTRoutePlannerResult::PlacesList& l(r.getOrderedPlaces());
		PTRoutePlannerResult::PlacesList::const_iterator itl(l.begin());
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
	ServicePointer s5_0(false, USER_PEDESTRIAN, S, now);
	s5_0.setDepartureInformations(A, now, now, PA);
	s5_0.setArrivalInformations(C, now, now, PC);
	j5 = Journey(j5, s5_0);
	ServicePointer s5_1(false, USER_PEDESTRIAN, S, now);
	s5_1.setDepartureInformations(C, now, now, PC);
	s5_1.setArrivalInformations(D, now, now, PD);
	j5 = Journey(j5, s5_1);
	ServicePointer s5_2(false, USER_PEDESTRIAN, S, now);
	s5_2.setDepartureInformations(D, now, now, PD);
	s5_2.setArrivalInformations(E, now, now, PE);
	j5 = Journey(j5, s5_2);
	ServicePointer s5_3(false, USER_PEDESTRIAN, S, now);
	s5_3.setDepartureInformations(E, now, now, PE);
	s5_3.setArrivalInformations(F, now, now, PF);
	j5 = Journey(j5, s5_3);
	ServicePointer s5_4(false, USER_PEDESTRIAN, S, now);
	s5_4.setDepartureInformations(F, now, now, PF);
	s5_4.setArrivalInformations(B, now, now, PB);
	j5 = Journey(j5, s5_4);
	j.push_back(j5);

	{
		PTRoutePlannerResult r(&CA, &CB, false, j);
		const PTRoutePlannerResult::PlacesList& l(r.getOrderedPlaces());
		PTRoutePlannerResult::PlacesList::const_iterator itl(l.begin());
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
	ServicePointer s6_0(false, USER_PEDESTRIAN, S, now);
	s6_0.setDepartureInformations(A, now, now, PA);
	s6_0.setArrivalInformations(D, now, now, PD);
	j6 = Journey(j6, s6_0);
	ServicePointer s6_1(false, USER_PEDESTRIAN, S, now);
	s6_1.setDepartureInformations(D, now, now, PD);
	s6_1.setArrivalInformations(C, now, now, PC);
	j6 = Journey(j6, s6_1);
	ServicePointer s6_2(false, USER_PEDESTRIAN, S, now);
	s6_2.setDepartureInformations(C, now, now, PC);
	s6_2.setArrivalInformations(B, now, now, PB);
	j6 = Journey(j6, s6_2);
	j.push_back(j6);

	{
		PTRoutePlannerResult r(&CA, &CB, false, j);
		const PTRoutePlannerResult::PlacesList& l(r.getOrderedPlaces());
		PTRoutePlannerResult::PlacesList::const_iterator itl(l.begin());
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
	ptime now(second_clock::local_time());

	JourneyPattern L;
	PermanentService S(0, &L, boost::posix_time::minutes(5));

	StopArea CA;
	CA.setName("A");
	StopPoint PA;
	PA.setHub(&CA);
	LineStop A;
	A.setLine(&L);
	A.setPhysicalStop(&PA);
	A.setRankInPath(0);

	StopArea CB;
	CB.setName("B");
	StopPoint PB;
	PB.setHub(&CB);
	LineStop B;
	B.setLine(&L);
	B.setPhysicalStop(&PB);
	B.setRankInPath(1);

	StopArea CC;
	CC.setName("C");
	StopPoint PC;
	PC.setHub(&CC);
	LineStop C;
	C.setLine(&L);
	C.setPhysicalStop(&PC);
	C.setRankInPath(2);

	StopArea CD;
	CD.setName("D");
	StopPoint PD;
	PD.setHub(&CD);
	LineStop D;
	D.setLine(&L);
	D.setPhysicalStop(&PD);
	D.setRankInPath(3);

	StopArea CE;
	CE.setName("E");
	StopPoint PE;
	PE.setHub(&CE);
	LineStop E;
	E.setLine(&L);
	E.setPhysicalStop(&PE);
	E.setRankInPath(4);

	StopArea CF;
	CF.setName("F");
	StopPoint PF;
	PF.setHub(&CF);
	LineStop F;
	F.setLine(&L);
	F.setPhysicalStop(&PF);
	F.setRankInPath(5);

	Road R;
	PermanentService Sr(0, &R, boost::posix_time::minutes(5));

	Crossing Cr0;
	Address A0;
	A0.setHub(&Cr0);
	
	Crossing Cr1;
	Address A1;
	A1.setHub(&Cr1);
	
	Crossing Cr2;
	Address A2;
	A2.setHub(&Cr2);

	RoadChunk Ch0;
	Ch0.setRoad(&R);
	Ch0.setFromAddress(&A0);
	Ch0.setRankInPath(0);

	RoadChunk Ch1;
	Ch1.setRoad(&R);
	Ch1.setFromAddress(&A1);
	Ch1.setRankInPath(1);

	RoadChunk Ch2;
	Ch2.setRoad(&R);
	Ch2.setFromAddress(&A2);
	Ch2.setRankInPath(2);

	Journey j0;
	ServicePointer s0_0(false, USER_PEDESTRIAN, Sr, now);
	s0_0.setDepartureInformations(A, now, now, PA);
	s0_0.setArrivalInformations(Ch0, now, now, A0);
	j0 = Journey(j0, s0_0);
	ServicePointer s0_1(false, USER_PEDESTRIAN, Sr, now);
	s0_1.setDepartureInformations(Ch0, now, now, A0);
	s0_1.setArrivalInformations(Ch1, now, now, A1);
	j0 = Journey(j0, s0_1);
	ServicePointer s0_2(false, USER_PEDESTRIAN, Sr, now);
	s0_2.setDepartureInformations(Ch1, now, now, A1);
	s0_2.setArrivalInformations(C, now, now, PC);
	j0 = Journey(j0, s0_2);
	ServicePointer s0_3(false, USER_PEDESTRIAN, Sr, now);
	s0_3.setDepartureInformations(C, now, now, PC);
	s0_3.setArrivalInformations(B, now, now, PB);
	j0 = Journey(j0, s0_3);

	PTRoutePlannerResult::Journeys j;
	j.push_back(j0);

	{
		PTRoutePlannerResult r(&CA, &CB, false, j);
		const PTRoutePlannerResult::PlacesList& l(r.getOrderedPlaces());
		PTRoutePlannerResult::PlacesList::const_iterator itl(l.begin());
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
	ServicePointer s1_0(false, USER_PEDESTRIAN, S, now);
	s1_0.setDepartureInformations(A, now, now, PA);
	s1_0.setArrivalInformations(E, now, now, PE);
	j1 = Journey(j1, s1_0);
	ServicePointer s1_1(false, USER_PEDESTRIAN, Sr, now);
	s1_1.setDepartureInformations(E, now, now, PE);
	s1_1.setArrivalInformations(Ch2, now, now, A2);
	j1 = Journey(j1, s1_1);
	ServicePointer s1_2(false, USER_PEDESTRIAN, Sr, now);
	s1_2.setDepartureInformations(Ch2, now, now, A2);
	s1_2.setArrivalInformations(B, now, now, PB);
	j1 = Journey(j1, s1_2);

	j.push_back(j1);

	{
		PTRoutePlannerResult r(&CA, &CB, false, j);
		const PTRoutePlannerResult::PlacesList& l(r.getOrderedPlaces());
		PTRoutePlannerResult::PlacesList::const_iterator itl(l.begin());
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
	ServicePointer s2_0(false, USER_PEDESTRIAN, Sr, now);
	s2_0.setDepartureInformations(Ch0, now, now, A0);
	s2_0.setArrivalInformations(D, now, now, PD);
	j2 = Journey(j2, s2_0);
	ServicePointer s2_1(false, USER_PEDESTRIAN, S, now);
	s2_1.setDepartureInformations(D, now, now, PD);
	s2_1.setArrivalInformations(B, now, now, PB);
	j2 = Journey(j2, s2_1);

	j.push_back(j2);

	{
		PTRoutePlannerResult r(&CA, &CB, false, j);
		const PTRoutePlannerResult::PlacesList& l(r.getOrderedPlaces());
		PTRoutePlannerResult::PlacesList::const_iterator itl(l.begin());
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
	ServicePointer s3_0(false, USER_PEDESTRIAN, S, now);
	s3_0.setDepartureInformations(A, now, now, PA);
	s3_0.setArrivalInformations(F, now, now, PF);
	j3 = Journey(j3, s3_0);
	ServicePointer s3_1(false, USER_PEDESTRIAN, Sr, now);
	s3_1.setDepartureInformations(F, now, now, PF);
	s3_1.setArrivalInformations(Ch2, now, now, A2);
	j3 = Journey(j3, s3_1);

	j.push_back(j3);

	{
		PTRoutePlannerResult r(&CA, &CB, false, j);
		const PTRoutePlannerResult::PlacesList& l(r.getOrderedPlaces());
		PTRoutePlannerResult::PlacesList::const_iterator itl(l.begin());
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
	ServicePointer s4_0(false, USER_PEDESTRIAN, Sr, now);
	s4_0.setDepartureInformations(Ch0, now, now, A0);
	s4_0.setArrivalInformations(F, now, now, PF);
	j4 = Journey(j4, s4_0);
	ServicePointer s4_1(false, USER_PEDESTRIAN, Sr, now);
	s4_1.setDepartureInformations(F, now, now, PF);
	s4_1.setArrivalInformations(Ch2, now, now, A2);
	j4 = Journey(j4, s4_1);

	j.push_back(j4);

	{
		PTRoutePlannerResult r(&CA, &CB, false, j);
		const PTRoutePlannerResult::PlacesList& l(r.getOrderedPlaces());
		PTRoutePlannerResult::PlacesList::const_iterator itl(l.begin());
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
	ServicePointer s5_0(false, USER_PEDESTRIAN, Sr, now);
	s5_0.setDepartureInformations(Ch0, now, now, A0);
	s5_0.setArrivalInformations(Ch1, now, now, A1);
	j5 = Journey(j5, s5_0);

	j.push_back(j5);

	{
		PTRoutePlannerResult r(&CA, &CB, false, j);
		const PTRoutePlannerResult::PlacesList& l(r.getOrderedPlaces());
		PTRoutePlannerResult::PlacesList::const_iterator itl(l.begin());
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
	ptime now(second_clock::local_time());

	JourneyPattern L;
	PermanentService S(0, &L, boost::posix_time::minutes(5));

	StopArea CA;
	CA.setName("A");
	StopPoint PA;
	PA.setHub(&CA);
	LineStop A;
	A.setLine(&L);
	A.setPhysicalStop(&PA);
	A.setRankInPath(0);

	StopArea CB;
	CB.setName("B");
	StopPoint PB;
	PB.setHub(&CB);
	LineStop B;
	B.setLine(&L);
	B.setPhysicalStop(&PB);
	B.setRankInPath(1);

	StopArea CC;
	CC.setName("C");
	StopPoint PC;
	PC.setHub(&CC);
	LineStop C;
	C.setLine(&L);
	C.setPhysicalStop(&PC);
	C.setRankInPath(2);

	StopArea CD;
	CD.setName("D");
	StopPoint PD;
	PD.setHub(&CD);
	LineStop D;
	D.setLine(&L);
	D.setPhysicalStop(&PD);
	D.setRankInPath(3);

	StopArea CE;
	CE.setName("E");
	StopPoint PE;
	PE.setHub(&CE);
	LineStop E;
	E.setLine(&L);
	E.setPhysicalStop(&PE);
	E.setRankInPath(4);

	StopArea CF;
	CF.setName("F");
	StopPoint PF;
	PF.setHub(&CF);
	LineStop F;
	F.setLine(&L);
	F.setPhysicalStop(&PF);
	F.setRankInPath(5);

	{
		Journey j0;
		ServicePointer s0_0(false, USER_PEDESTRIAN, S, now);
		s0_0.setDepartureInformations(D, now, now, PD);
		s0_0.setArrivalInformations(C, now, now, PC);
		j0 = Journey(j0, s0_0);
		ServicePointer s0_1(false, USER_PEDESTRIAN, S, now);
		s0_1.setDepartureInformations(C, now, now, PC);
		s0_1.setArrivalInformations(B, now, now, PB);
		j0 = Journey(j0, s0_1);

		Journey j1;
		ServicePointer s1_0(false, USER_PEDESTRIAN, S, now);
		s1_0.setDepartureInformations(E, now, now, PE);
		s1_0.setArrivalInformations(C, now, now, PC);
		j1 = Journey(j1, s1_0);
		ServicePointer s1_1(false, USER_PEDESTRIAN, S, now);
		s1_1.setDepartureInformations(C, now, now, PC);
		s1_1.setArrivalInformations(B, now, now, PB);
		j1 = Journey(j1, s1_1);

		PTRoutePlannerResult::Journeys j;
		j.push_back(j0);
		j.push_back(j1);
		PTRoutePlannerResult r(&CA, &CB, false, j);
		const PTRoutePlannerResult::PlacesList& l(r.getOrderedPlaces());
		PTRoutePlannerResult::PlacesList::const_iterator itl(l.begin());
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
		ServicePointer s0_0(false, USER_PEDESTRIAN, S, now);
		s0_0.setDepartureInformations(A, now, now, PA);
		s0_0.setArrivalInformations(C, now, now, PC);
		j0 = Journey(j0, s0_0);
		ServicePointer s0_1(false, USER_PEDESTRIAN, S, now);
		s0_1.setDepartureInformations(C, now, now, PC);
		s0_1.setArrivalInformations(D, now, now, PD);
		j0 = Journey(j0, s0_1);

		Journey j1;
		ServicePointer s1_0(false, USER_PEDESTRIAN, S, now);
		s1_0.setDepartureInformations(A, now, now, PA);
		s1_0.setArrivalInformations(C, now, now, PC);
		j1 = Journey(j1, s1_0);
		ServicePointer s1_1(false, USER_PEDESTRIAN, S, now);
		s1_1.setDepartureInformations(A, now, now, PA);
		s1_1.setArrivalInformations(E, now, now, PE);
		j1 = Journey(j1, s1_1);

		PTRoutePlannerResult::Journeys j;
		j.push_back(j0);
		j.push_back(j1);
		PTRoutePlannerResult r(&CA, &CB, false, j);
		const PTRoutePlannerResult::PlacesList& l(r.getOrderedPlaces());
		PTRoutePlannerResult::PlacesList::const_iterator itl(l.begin());
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
		ServicePointer s0_0(false, USER_PEDESTRIAN, S, now);
		s0_0.setDepartureInformations(D, now, now, PD);
		s0_0.setArrivalInformations(E, now, now, PE);
		j0 = Journey(j0, s0_0);

		Journey j1;
		ServicePointer s1_0(false, USER_PEDESTRIAN, S, now);
		s1_0.setDepartureInformations(C, now, now, PC);
		s1_0.setArrivalInformations(F, now, now, PF);
		j1 = Journey(j1, s1_0);

		PTRoutePlannerResult::Journeys j;
		j.push_back(j0);
		j.push_back(j1);
		PTRoutePlannerResult r(&CA, &CB, false, j);
		const PTRoutePlannerResult::PlacesList& l(r.getOrderedPlaces());
		PTRoutePlannerResult::PlacesList::const_iterator itl(l.begin());
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
		ServicePointer s0_0(false, USER_PEDESTRIAN, S, now);
		s0_0.setDepartureInformations(A, now, now, PA);
		s0_0.setArrivalInformations(B, now, now, PB);
		j0 = Journey(j0, s0_0);

		Journey j1;
		ServicePointer s1_0(false, USER_PEDESTRIAN, S, now);
		s1_0.setDepartureInformations(C, now, now, PC);
		s1_0.setArrivalInformations(F, now, now, PF);
		j1 = Journey(j1, s1_0);

		PTRoutePlannerResult::Journeys j;
		j.push_back(j0);
		j.push_back(j1);
		PTRoutePlannerResult r(&CA, &CB, false, j);
		const PTRoutePlannerResult::PlacesList& l(r.getOrderedPlaces());
		PTRoutePlannerResult::PlacesList::const_iterator itl(l.begin());
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CA.getName());
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK(itl->isOrigin);
		BOOST_CHECK(!itl->isDestination);
		BOOST_CHECK_EQUAL(itl->place->getName(), CC.getName());
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

	{ // 7798 The actual destination must always be at the end of the table
		Journey j0;
		ServicePointer s0_0(false, USER_PEDESTRIAN, S, now);
		s0_0.setDepartureInformations(D, now, now, PD);
		s0_0.setArrivalInformations(F, now, now, PF);
		j0 = Journey(j0, s0_0);

		Journey j1;
		ServicePointer s1_0(false, USER_PEDESTRIAN, S, now);
		s1_0.setDepartureInformations(A, now, now, PA);
		s1_0.setArrivalInformations(B, now, now, PB);
		j1 = Journey(j1, s1_0);

		PTRoutePlannerResult::Journeys j;
		j.push_back(j0);
		j.push_back(j1);
		PTRoutePlannerResult r(&CA, &CB, false, j);
		const PTRoutePlannerResult::PlacesList& l(r.getOrderedPlaces());
		PTRoutePlannerResult::PlacesList::const_iterator itl(l.begin());
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
}
