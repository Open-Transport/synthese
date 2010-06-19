
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
#include "PublicTransportStopZoneConnectionPlace.h"
#include "PhysicalStop.h"
#include "LineStop.h"
#include "Journey.h"
#include "Line.h"
#include "PermanentService.h"
#include "Line.h"
#include "Road.h"
#include "RoadChunk.h"
#include "Crossing.h"
#include "Address.h"

#include <boost/test/auto_unit_test.hpp>

using namespace synthese::pt;
using namespace synthese::road;
using namespace synthese::graph;
using namespace synthese::ptrouteplanner;

BOOST_AUTO_TEST_CASE (placesListOrder_OrderingTests)
{
	Line L;
	PermanentService S(0, &L, boost::posix_time::minutes(5));

	PublicTransportStopZoneConnectionPlace CA;
	CA.setName("A");
	PhysicalStop PA;
	PA.setHub(&CA);
	LineStop A;
	A.setLine(&L);
	A.setPhysicalStop(&PA);
	A.setRankInPath(0);

	PublicTransportStopZoneConnectionPlace CB;
	CB.setName("B");
	PhysicalStop PB;
	PB.setHub(&CB);
	LineStop B;
	B.setLine(&L);
	B.setPhysicalStop(&PB);
	B.setRankInPath(1);

	PublicTransportStopZoneConnectionPlace CC;
	CC.setName("C");
	PhysicalStop PC;
	PC.setHub(&CC);
	LineStop C;
	C.setLine(&L);
	C.setPhysicalStop(&PC);
	C.setRankInPath(2);

	PublicTransportStopZoneConnectionPlace CD;
	CD.setName("D");
	PhysicalStop PD;
	PD.setHub(&CD);
	LineStop D;
	D.setLine(&L);
	D.setPhysicalStop(&PD);
	D.setRankInPath(3);

	PublicTransportStopZoneConnectionPlace CE;
	CE.setName("E");
	PhysicalStop PE;
	PE.setHub(&CE);
	LineStop E;
	E.setLine(&L);
	E.setPhysicalStop(&PE);
	E.setRankInPath(4);

	PublicTransportStopZoneConnectionPlace CF;
	CF.setName("F");
	PhysicalStop PF;
	PF.setHub(&CF);
	LineStop F;
	F.setLine(&L);
	F.setPhysicalStop(&PF);
	F.setRankInPath(5);

	Journey j0;
	ServicePointer sp0_0(false, DEPARTURE_TO_ARRIVAL, 0, &A);
	sp0_0.setService(&S);
	ServiceUse s0_0(sp0_0,&B);
	j0 = Journey(j0, s0_0);

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
	ServicePointer sp1_0(false, DEPARTURE_TO_ARRIVAL, 0, &A);
	sp1_0.setService(&S);
	ServiceUse s1_0(sp1_0,&C);
	j1 = Journey(j1, s1_0);
	ServicePointer sp1_1(false, DEPARTURE_TO_ARRIVAL, 0, &C);
	sp1_1.setService(&S);
	ServiceUse s1_1(sp1_1,&B);
	j1 = Journey(j1, s1_1);
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
	ServicePointer sp2_0(false, DEPARTURE_TO_ARRIVAL, 0, &A);
	sp2_0.setService(&S);
	ServiceUse s2_0(sp2_0,&D);
	j2 = Journey(j2, s2_0);
	ServicePointer sp2_1(false, DEPARTURE_TO_ARRIVAL, 0, &D);
	sp2_1.setService(&S);
	ServiceUse s2_1(sp2_1,&B);
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
	ServicePointer sp3_0(false, DEPARTURE_TO_ARRIVAL, 0, &A);
	sp3_0.setService(&S);
	ServiceUse s3_0(sp3_0,&C);
	j3 = Journey(j3, s3_0);
	ServicePointer sp3_1(false, DEPARTURE_TO_ARRIVAL, 0, &C);
	sp3_1.setService(&S);
	ServiceUse s3_1(sp3_1,&D);
	j3 = Journey(j3, s3_1);
	ServicePointer sp3_2(false, DEPARTURE_TO_ARRIVAL, 0, &D);
	sp3_2.setService(&S);
	ServiceUse s3_2(sp3_2,&B);
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
	ServicePointer sp4_0(false, DEPARTURE_TO_ARRIVAL, 0, &A);
	sp4_0.setService(&S);
	ServiceUse s4_0(sp4_0,&E);
	j4 = Journey(j4, s4_0);
	ServicePointer sp4_1(false, DEPARTURE_TO_ARRIVAL, 0, &E);
	sp4_1.setService(&S);
	ServiceUse s4_1(sp4_1,&F);
	j4 = Journey(j4, s4_1);
	ServicePointer sp4_2(false, DEPARTURE_TO_ARRIVAL, 0, &F);
	sp4_2.setService(&S);
	ServiceUse s4_2(sp4_2,&B);
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
	ServicePointer sp5_0(false, DEPARTURE_TO_ARRIVAL, 0, &A);
	sp5_0.setService(&S);
	ServiceUse s5_0(sp5_0,&C);
	j5 = Journey(j5, s5_0);
	ServicePointer sp5_1(false, DEPARTURE_TO_ARRIVAL, 0, &C);
	sp5_1.setService(&S);
	ServiceUse s5_1(sp5_1,&D);
	j5 = Journey(j5, s5_1);
	ServicePointer sp5_2(false, DEPARTURE_TO_ARRIVAL, 0, &D);
	sp5_2.setService(&S);
	ServiceUse s5_2(sp5_2,&E);
	j5 = Journey(j5, s5_2);
	ServicePointer sp5_3(false, DEPARTURE_TO_ARRIVAL, 0, &E);
	sp5_3.setService(&S);
	ServiceUse s5_3(sp5_3,&F);
	j5 = Journey(j5, s5_3);
	ServicePointer sp5_4(false, DEPARTURE_TO_ARRIVAL, 0, &F);
	sp5_4.setService(&S);
	ServiceUse s5_4(sp5_4,&B);
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
	ServicePointer sp6_0(false, DEPARTURE_TO_ARRIVAL, 0, &A);
	sp6_0.setService(&S);
	ServiceUse s6_0(sp6_0,&D);
	j6 = Journey(j6, s6_0);
	ServicePointer sp6_1(false, DEPARTURE_TO_ARRIVAL, 0, &D);
	sp6_1.setService(&S);
	ServiceUse s6_1(sp6_1,&C);
	j6 = Journey(j6, s6_1);
	ServicePointer sp6_2(false, DEPARTURE_TO_ARRIVAL, 0, &C);
	sp6_2.setService(&S);
	ServiceUse s6_2(sp6_2,&B);
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
	Line L;
	PermanentService S(0, &L, boost::posix_time::minutes(5));

	PublicTransportStopZoneConnectionPlace CA;
	CA.setName("A");
	PhysicalStop PA;
	PA.setHub(&CA);
	LineStop A;
	A.setLine(&L);
	A.setPhysicalStop(&PA);
	A.setRankInPath(0);

	PublicTransportStopZoneConnectionPlace CB;
	CB.setName("B");
	PhysicalStop PB;
	PB.setHub(&CB);
	LineStop B;
	B.setLine(&L);
	B.setPhysicalStop(&PB);
	B.setRankInPath(1);

	PublicTransportStopZoneConnectionPlace CC;
	CC.setName("C");
	PhysicalStop PC;
	PC.setHub(&CC);
	LineStop C;
	C.setLine(&L);
	C.setPhysicalStop(&PC);
	C.setRankInPath(2);

	PublicTransportStopZoneConnectionPlace CD;
	CD.setName("D");
	PhysicalStop PD;
	PD.setHub(&CD);
	LineStop D;
	D.setLine(&L);
	D.setPhysicalStop(&PD);
	D.setRankInPath(3);

	PublicTransportStopZoneConnectionPlace CE;
	CE.setName("E");
	PhysicalStop PE;
	PE.setHub(&CE);
	LineStop E;
	E.setLine(&L);
	E.setPhysicalStop(&PE);
	E.setRankInPath(4);

	PublicTransportStopZoneConnectionPlace CF;
	CF.setName("F");
	PhysicalStop PF;
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
	ServicePointer sp0_0(false, DEPARTURE_TO_ARRIVAL, 0, &A);
	sp0_0.setService(&Sr);
	ServiceUse s0_0(sp0_0,&Ch0);
	j0 = Journey(j0, s0_0);
	ServicePointer sp0_1(false, DEPARTURE_TO_ARRIVAL, 0, &Ch0);
	sp0_1.setService(&Sr);
	ServiceUse s0_1(sp0_0,&Ch1);
	j0 = Journey(j0, s0_1);
	ServicePointer sp0_2(false, DEPARTURE_TO_ARRIVAL, 0, &Ch1);
	sp0_2.setService(&Sr);
	ServiceUse s0_2(sp0_2,&C);
	j0 = Journey(j0, s0_2);
	ServicePointer sp0_3(false, DEPARTURE_TO_ARRIVAL, 0, &C);
	sp0_3.setService(&S);
	ServiceUse s0_3(sp0_3,&B);
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
	ServicePointer sp1_0(false, DEPARTURE_TO_ARRIVAL, 0, &A);
	sp1_0.setService(&S);
	ServiceUse s1_0(sp1_0,&E);
	j1 = Journey(j1, s1_0);
	ServicePointer sp1_1(false, DEPARTURE_TO_ARRIVAL, 0, &E);
	sp1_1.setService(&Sr);
	ServiceUse s1_1(sp1_1,&Ch2);
	j1 = Journey(j1, s1_1);
	ServicePointer sp1_2(false, DEPARTURE_TO_ARRIVAL, 0, &Ch2);
	sp1_2.setService(&Sr);
	ServiceUse s1_2(sp1_2,&B);
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
	ServicePointer sp2_0(false, DEPARTURE_TO_ARRIVAL, 0, &Ch0);
	sp2_0.setService(&Sr);
	ServiceUse s2_0(sp2_0,&D);
	j2 = Journey(j2, s2_0);
	ServicePointer sp2_1(false, DEPARTURE_TO_ARRIVAL, 0, &D);
	sp2_1.setService(&S);
	ServiceUse s2_1(sp2_1,&B);
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
	ServicePointer sp3_0(false, DEPARTURE_TO_ARRIVAL, 0, &A);
	sp3_0.setService(&S);
	ServiceUse s3_0(sp3_0,&F);
	j3 = Journey(j3, s3_0);
	ServicePointer sp3_1(false, DEPARTURE_TO_ARRIVAL, 0, &F);
	sp3_1.setService(&Sr);
	ServiceUse s3_1(sp3_1,&Ch2);
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
	ServicePointer sp4_0(false, DEPARTURE_TO_ARRIVAL, 0, &Ch0);
	sp4_0.setService(&Sr);
	ServiceUse s4_0(sp4_0,&F);
	j4 = Journey(j4, s4_0);
	ServicePointer sp4_1(false, DEPARTURE_TO_ARRIVAL, 0, &F);
	sp4_1.setService(&Sr);
	ServiceUse s4_1(sp4_1,&Ch2);
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
	ServicePointer sp5_0(false, DEPARTURE_TO_ARRIVAL, 0, &Ch0);
	sp5_0.setService(&Sr);
	ServiceUse s5_0(sp5_0,&Ch1);
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
	Line L;
	PermanentService S(0, &L, boost::posix_time::minutes(5));

	PublicTransportStopZoneConnectionPlace CA;
	CA.setName("A");
	PhysicalStop PA;
	PA.setHub(&CA);
	LineStop A;
	A.setLine(&L);
	A.setPhysicalStop(&PA);
	A.setRankInPath(0);

	PublicTransportStopZoneConnectionPlace CB;
	CB.setName("B");
	PhysicalStop PB;
	PB.setHub(&CB);
	LineStop B;
	B.setLine(&L);
	B.setPhysicalStop(&PB);
	B.setRankInPath(1);

	PublicTransportStopZoneConnectionPlace CC;
	CC.setName("C");
	PhysicalStop PC;
	PC.setHub(&CC);
	LineStop C;
	C.setLine(&L);
	C.setPhysicalStop(&PC);
	C.setRankInPath(2);

	PublicTransportStopZoneConnectionPlace CD;
	CD.setName("D");
	PhysicalStop PD;
	PD.setHub(&CD);
	LineStop D;
	D.setLine(&L);
	D.setPhysicalStop(&PD);
	D.setRankInPath(3);

	PublicTransportStopZoneConnectionPlace CE;
	CE.setName("E");
	PhysicalStop PE;
	PE.setHub(&CE);
	LineStop E;
	E.setLine(&L);
	E.setPhysicalStop(&PE);
	E.setRankInPath(4);

	PublicTransportStopZoneConnectionPlace CF;
	CF.setName("F");
	PhysicalStop PF;
	PF.setHub(&CF);
	LineStop F;
	F.setLine(&L);
	F.setPhysicalStop(&PF);
	F.setRankInPath(5);

	{
		Journey j0;
		ServicePointer sp0_0(false, DEPARTURE_TO_ARRIVAL, 0, &D);
		sp0_0.setService(&S);
		ServiceUse s0_0(sp0_0,&C);
		j0 = Journey(j0, s0_0);
		ServicePointer sp0_1(false, DEPARTURE_TO_ARRIVAL, 0, &C);
		sp0_1.setService(&S);
		ServiceUse s0_1(sp0_1,&B);
		j0 = Journey(j0, s0_1);

		Journey j1;
		ServicePointer sp1_0(false, DEPARTURE_TO_ARRIVAL, 0, &E);
		sp1_0.setService(&S);
		ServiceUse s1_0(sp1_0,&C);
		j1 = Journey(j1, s1_0);
		ServicePointer sp1_1(false, DEPARTURE_TO_ARRIVAL, 0, &C);
		sp1_1.setService(&S);
		ServiceUse s1_1(sp1_1,&B);
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
		ServicePointer sp0_0(false, DEPARTURE_TO_ARRIVAL, 0, &A);
		sp0_0.setService(&S);
		ServiceUse s0_0(sp0_0,&C);
		j0 = Journey(j0, s0_0);
		ServicePointer sp0_1(false, DEPARTURE_TO_ARRIVAL, 0, &C);
		sp0_1.setService(&S);
		ServiceUse s0_1(sp0_1,&D);
		j0 = Journey(j0, s0_1);

		Journey j1;
		ServicePointer sp1_0(false, DEPARTURE_TO_ARRIVAL, 0, &A);
		sp1_0.setService(&S);
		ServiceUse s1_0(sp1_0,&C);
		j1 = Journey(j1, s1_0);
		ServicePointer sp1_1(false, DEPARTURE_TO_ARRIVAL, 0, &C);
		sp1_1.setService(&S);
		ServiceUse s1_1(sp1_1,&E);
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
		ServicePointer sp0_0(false, DEPARTURE_TO_ARRIVAL, 0, &D);
		sp0_0.setService(&S);
		ServiceUse s0_0(sp0_0,&E);
		j0 = Journey(j0, s0_0);

		Journey j1;
		ServicePointer sp1_0(false, DEPARTURE_TO_ARRIVAL, 0, &C);
		sp1_0.setService(&S);
		ServiceUse s1_0(sp1_0,&F);
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
		ServicePointer sp0_0(false, DEPARTURE_TO_ARRIVAL, 0, &A);
		sp0_0.setService(&S);
		ServiceUse s0_0(sp0_0,&B);
		j0 = Journey(j0, s0_0);

		Journey j1;
		ServicePointer sp1_0(false, DEPARTURE_TO_ARRIVAL, 0, &C);
		sp1_0.setService(&S);
		ServiceUse s1_0(sp1_0,&F);
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
		ServicePointer sp0_0(false, DEPARTURE_TO_ARRIVAL, 0, &D);
		sp0_0.setService(&S);
		ServiceUse s0_0(sp0_0,&F);
		j0 = Journey(j0, s0_0);

		Journey j1;
		ServicePointer sp1_0(false, DEPARTURE_TO_ARRIVAL, 0, &A);
		sp1_0.setService(&S);
		ServiceUse s1_0(sp1_0,&B);
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