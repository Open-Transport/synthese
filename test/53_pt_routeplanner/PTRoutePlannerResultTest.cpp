
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

#include <boost/test/auto_unit_test.hpp>

using namespace synthese::pt;
using namespace synthese::graph;
using namespace synthese::ptrouteplanner;

BOOST_AUTO_TEST_CASE (placesListOrder)
{
	Line L;

	PublicTransportStopZoneConnectionPlace CA;
	PhysicalStop PA;
	PA.setHub(&CA);
	LineStop A;
	A.setPhysicalStop(&PA);
	A.setLine(&L);

	PublicTransportStopZoneConnectionPlace CB;
	PhysicalStop PB;
	PB.setHub(&CB);
	LineStop B;
	B.setPhysicalStop(&PB);
	B.setLine(&L);

	PublicTransportStopZoneConnectionPlace CC;
	PhysicalStop PC;
	PC.setHub(&CC);
	LineStop C;
	C.setPhysicalStop(&PC);
	C.setLine(&L);

	PublicTransportStopZoneConnectionPlace CD;
	PhysicalStop PD;
	PD.setHub(&CD);
	LineStop D;
	D.setPhysicalStop(&PD);
	D.setLine(&L);

	Journey j0;
	ServicePointer sp0_0(false, DEPARTURE_TO_ARRIVAL, 0, &A);
	ServiceUse s0_0(sp0_0,&B);
	j0 = Journey(j0, s0_0);

	PTRoutePlannerResult::Journeys j;
	j.push_back(j0);

	{
		PTRoutePlannerResult r(&CA, &CB, false, j);
		const PTRoutePlannerResult::PlacesList& l(r.getOrderedPlaces());
		PTRoutePlannerResult::PlacesList::const_iterator itl(l.begin());
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK_EQUAL(itl->place, &CA);
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK_EQUAL(itl->place, &CB);
		BOOST_CHECK(itl == l.end());
	}

	Journey j1;
	ServicePointer sp1_0(false, DEPARTURE_TO_ARRIVAL, 0, &A);
	ServiceUse s1_0(sp1_0,&C);
	j1 = Journey(j1, s1_0);
	ServicePointer sp1_1(false, DEPARTURE_TO_ARRIVAL, 0, &C);
	ServiceUse s1_1(sp1_1,&B);
	j1 = Journey(j1, s1_1);
	j.push_back(j1);

	{
		PTRoutePlannerResult r(&CA, &CB, false, j);
		const PTRoutePlannerResult::PlacesList& l(r.getOrderedPlaces());
		PTRoutePlannerResult::PlacesList::const_iterator itl(l.begin());
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK_EQUAL(itl->place, &CA);
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK_EQUAL(itl->place, &CC);
		++itl;
		BOOST_REQUIRE(itl != l.end());
		BOOST_CHECK_EQUAL(itl->place, &CB);
		BOOST_CHECK(itl == l.end());
	}

}

