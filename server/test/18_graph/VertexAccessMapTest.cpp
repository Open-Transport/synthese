
/** VertexAccessMap Test implementation.
	@file VertexAccessMapTest.cpp

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

#include "VertexAccessMap.h"
#include "FakeGraphImplementation.hpp"
#include "Journey.h"
#include "ServicePointer.h"

#pragma GCC diagmostic ignored "-Werror=sign-compare"
#include <boost/test/auto_unit_test.hpp>
#pragma GCC diagnostic pop

using namespace synthese::graph;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost;


BOOST_AUTO_TEST_CASE (Intersections)
{
	{ // Simple intersection : a vertex belongs to the both maps without approach time
		VertexAccessMap map1;
		VertexAccessMap map2;
		FakeHub h1;

		BOOST_CHECK(!map1.intersercts(map2));
		BOOST_CHECK(!map2.intersercts(map1));
		BOOST_CHECK(map1.getBestIntersection(map2).empty());
		BOOST_CHECK(map2.getBestIntersection(map1).empty());

		FakeVertex v1(&h1);
		map1.insert(&v1, VertexAccess());

		BOOST_CHECK(!map1.intersercts(map2));
		BOOST_CHECK(!map2.intersercts(map1));
		BOOST_CHECK(map1.getBestIntersection(map2).empty());
		BOOST_CHECK(map2.getBestIntersection(map1).empty());

		FakeVertex v2(&h1);
		map2.insert(&v2, VertexAccess());

		BOOST_CHECK(!map1.intersercts(map2));
		BOOST_CHECK(!map2.intersercts(map1));
		BOOST_CHECK(map1.getBestIntersection(map2).empty());
		BOOST_CHECK(map2.getBestIntersection(map1).empty());

		FakeVertex v3(&h1);
		map1.insert(&v3, VertexAccess());
		map2.insert(&v3, VertexAccess());

		BOOST_CHECK(map1.intersercts(map2));
		BOOST_CHECK(map2.intersercts(map1));
		BOOST_CHECK(map1.getBestIntersection(map2).empty());
		BOOST_CHECK(map2.getBestIntersection(map1).empty());
	}

	{ // Simple approach intersection : a vertex allow to approach to a vertex of the other
		VertexAccessMap map1;
		VertexAccessMap map2;

		FakePath L(false);
		FakeService S;
		S.setPath(&L);
		FakeHub h1;

		FakeVertex v1(&h1);
		map1.insert(&v1, VertexAccess());
		FakeEdge e1;
		e1.setFromVertex(&v1);
		e1.setParentPath(&L);

		FakeVertex v2(&h1);
		map2.insert(&v2, VertexAccess());

		FakeVertex v3(&h1);
		map2.insert(&v3, VertexAccess());
		Journey j3;
		FakeEdge e3;
		e3.setFromVertex(&v3);
		e3.setParentPath(&L);
		ptime d3_0s(date(2000,1,1), minutes(0));
		ptime d3_0e(date(2000,1,1), minutes(5));
		ServicePointer sp3_0(true, false, 0, S, d3_0s);
		sp3_0.setDepartureInformations(e1, d3_0s, d3_0s, *e1.getFromVertex());
		sp3_0.setArrivalInformations(e3, d3_0e, d3_0e, *e3.getFromVertex());
		j3.append(sp3_0);

		// The first map contains a point of the second one, reached in 5 minutes
		map1.insert(&v3, VertexAccess(boost::posix_time::minutes(5), 1000, j3));

		BOOST_CHECK(map1.intersercts(map2));
		BOOST_CHECK(map2.intersercts(map1));
		Journey jt(map1.getBestIntersection(map2));
		BOOST_REQUIRE(!jt.empty());
		ServicePointer su(*jt.getServiceUses().begin());
		BOOST_CHECK_EQUAL(su.getDepartureEdge(), &e1);
		BOOST_CHECK_EQUAL(boost::posix_time::to_simple_string(su.getDepartureDateTime()), boost::posix_time::to_simple_string(d3_0s));
		BOOST_CHECK_EQUAL(su.getArrivalEdge(), &e3);
		BOOST_CHECK_EQUAL(boost::posix_time::to_simple_string(su.getArrivalDateTime()), boost::posix_time::to_simple_string(d3_0e));
	}

	{ // Simple approach intersection : a vertex allow to approach to a vertex of the other
		VertexAccessMap map1;
		VertexAccessMap map2;

		FakePath L(false);
		FakeService S;
		S.setPath(&L);
		FakeHub h1;

		FakeVertex v1(&h1);
		map1.insert(&v1, VertexAccess());

		FakeVertex v2(&h1);
		map2.insert(&v2, VertexAccess());
		FakeEdge e2;
		e2.setFromVertex(&v2);
		e2.setParentPath(&L);

		FakeVertex v3(&h1);
		map1.insert(&v3, VertexAccess());
		Journey j3;
		FakeEdge e3;
		e3.setFromVertex(&v3);
		e3.setParentPath(&L);
		ptime d3_0s(date(2000,1,1), minutes(0));
		ptime d3_0e(date(2000,1,1), minutes(5));
		ServicePointer sp3_0(true, false, 0, S, d3_0s);
		sp3_0.setArrivalInformations(e2, d3_0e, d3_0e, *e2.getFromVertex());
		sp3_0.setDepartureInformations(e3, d3_0s, d3_0s, *e3.getFromVertex());
		j3.append(sp3_0);

		// The second map contains a point of the first one, reached in 5 minutes
		map2.insert(&v3, VertexAccess(boost::posix_time::minutes(5), 1000, j3));

		BOOST_CHECK(map1.intersercts(map2));
		BOOST_CHECK(map2.intersercts(map1));
		Journey jt(map1.getBestIntersection(map2));
		BOOST_REQUIRE(!jt.empty());
		ServicePointer su(*jt.getServiceUses().begin());
		BOOST_CHECK_EQUAL(su.getDepartureEdge(), &e3);
		BOOST_CHECK_EQUAL(boost::posix_time::to_simple_string(su.getDepartureDateTime()), boost::posix_time::to_simple_string(d3_0s));
		BOOST_CHECK_EQUAL(su.getArrivalEdge(), &e2);
		BOOST_CHECK_EQUAL(boost::posix_time::to_simple_string(su.getArrivalDateTime()), boost::posix_time::to_simple_string(d3_0e));
	}

	{ // Double approach intersection : a vertex is reachable from two different vertices from the two maps
		VertexAccessMap map1;
		VertexAccessMap map2;

		FakePath L(false);
		FakeService S;
		S.setPath(&L);
		FakeHub h1;

		FakeVertex v1(&h1);
		map1.insert(&v1, VertexAccess());
		FakeEdge e1;
		e1.setFromVertex(&v1);
		e1.setParentPath(&L);

		FakeVertex v2(&h1);
		map2.insert(&v2, VertexAccess());
		FakeEdge e2;
		e2.setFromVertex(&v2);
		e2.setParentPath(&L);

		FakeVertex v3(&h1);
		FakeEdge e3;
		e3.setFromVertex(&v3);
		e3.setParentPath(&L);

		Journey j3_1;
		ptime d3_1s(date(2000,1,1), minutes(0));
		ptime d3_1e(date(2000,1,1), minutes(5));
		ServicePointer sp3_1(true, false, 0, S, d3_1s);
		sp3_1.setDepartureInformations(e1, d3_1s, d3_1s, *e1.getFromVertex());
		sp3_1.setArrivalInformations(e3, d3_1e, d3_1e, *e3.getFromVertex());
		j3_1.append(sp3_1);

		// The second map contains a point of the first one, reached in 5 minutes
		map1.insert(&v3, VertexAccess(boost::posix_time::minutes(5), 1000, j3_1));

		Journey j3_2;
		ptime d3_2s(date(2000,1,1), minutes(0));
		ptime d3_2e(date(2000,1,1), minutes(5));
		ServicePointer sp3_2(true, false, 0, S, d3_2s);
		sp3_2.setArrivalInformations(e2, d3_2e, d3_2e, *e2.getFromVertex());
		sp3_2.setDepartureInformations(e3, d3_2s, d3_2s, *e3.getFromVertex());
		j3_2.append(sp3_2);

		// The second map contains a point of the first one, reached in 5 minutes
		map2.insert(&v3, VertexAccess(boost::posix_time::minutes(5), 1000, j3_2));

		time_duration shifting_delay(d3_2s - d3_1e);

		BOOST_CHECK(map1.intersercts(map2));
		BOOST_CHECK(map2.intersercts(map1));
		Journey jt(map1.getBestIntersection(map2));
		BOOST_REQUIRE(!jt.empty());

		ServicePointer su(*jt.getServiceUses().begin());
		BOOST_CHECK_EQUAL(su.getDepartureEdge(), &e1);
		BOOST_CHECK_EQUAL(boost::posix_time::to_simple_string(su.getDepartureDateTime()), boost::posix_time::to_simple_string(d3_1s));
		BOOST_CHECK_EQUAL(su.getArrivalEdge(), &e3);
		BOOST_CHECK_EQUAL(boost::posix_time::to_simple_string(su.getArrivalDateTime()), boost::posix_time::to_simple_string(d3_1e));

		BOOST_REQUIRE(jt.getServiceUses().begin() + 1 != jt.getServiceUses().end());
		ServicePointer su2(*(jt.getServiceUses().begin() + 1));
		BOOST_CHECK_EQUAL(su2.getDepartureEdge(), &e3);
		BOOST_CHECK_EQUAL(boost::posix_time::to_simple_string(su2.getDepartureDateTime()), boost::posix_time::to_simple_string(d3_2s - shifting_delay));
		BOOST_CHECK_EQUAL(su2.getArrivalEdge(), &e2);
		BOOST_CHECK_EQUAL(boost::posix_time::to_simple_string(su2.getArrivalDateTime()), boost::posix_time::to_simple_string(d3_2e - shifting_delay));

		BOOST_CHECK(jt.getServiceUses().begin() + 2 == jt.getServiceUses().end());
	}
}
