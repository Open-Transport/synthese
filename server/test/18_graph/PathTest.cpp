
/** Path unit test.
	@file PathTest.cpp

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

#include "FakeGraphImplementation.hpp"

#pragma GCC diagnostic ignored "-Wsign-compare"
#include <boost/test/auto_unit_test.hpp>
#pragma GCC diagnostic pop

using namespace synthese::util;
using namespace synthese::graph;
using namespace synthese;

BOOST_AUTO_TEST_CASE (testPathsMerge)
{
	FakePathGroup pg;

	FakePath l1(false);
	l1.setPathGroup(&pg);

	FakePath l2(false);
	l2.setPathGroup(&pg);

	FakeHub h1(true);
	FakeHub h2(false);
	FakeHub h3(false);
	FakeHub h4(false);
	FakeHub h5(true);
	FakeHub h6(false);
	FakeHub h7(false);
	FakeHub h8(true);

	FakeVertex s1(&h1);
	FakeVertex s2(&h2);
	FakeVertex s3(&h3);
	FakeVertex s4(&h4);
	FakeVertex s5(&h5);
	FakeVertex s6(&h6);
	FakeVertex s7(&h7);
	FakeVertex s8(&h8);

	FakeEdge e1D(&l1, 0, true, false,0,&s1);
	FakeEdge e2D(&l1, 1, true, false,0,&s2);
	FakeEdge e3AD(&l1, 2, true, true, 0,&s3);
	FakeEdge e4A(&l1, 3, false, true,0,&s4);
	FakeEdge e5D(&l1, 4, true, false,0,&s5);
	FakeEdge e6AD(&l1, 5, true, true,0,&s6);
	s6.addArrivalEdge(&e6AD);
	s6.addDepartureEdge(&e6AD);
	FakeEdge e6bAD(&l2, 0, true, true,0,&s6);
	s6.addArrivalEdge(&e6bAD);
	s6.addDepartureEdge(&e6bAD);
	FakeEdge e7AD(&l2, 1, true, true,0,&s7);
	s7.addArrivalEdge(&e7AD);
	s7.addDepartureEdge(&e7AD);
	FakeEdge e8A(&l2, 2, false, true,0,&s8);
	s8.addArrivalEdge(&e8A);
	FakeEdge* eNULL(NULL);

	l1.addEdge(e1D);
	l1.addEdge(e2D);
	l1.addEdge(e3AD);
	l1.addEdge(e4A);
	l1.addEdge(e5D);
	l1.addEdge(e6AD);
	l2.addEdge(e6bAD);
	l2.addEdge(e7AD);
	l2.addEdge(e8A);

	const FakePath::Edges& edges1 = l1.getEdges();
	const FakePath::Edges& edges2 = l2.getEdges();

	BOOST_CHECK_EQUAL (edges1[0], &e1D);
	BOOST_CHECK_EQUAL (edges1[1], &e2D);
	BOOST_CHECK_EQUAL (edges1[2], &e3AD);
	BOOST_CHECK_EQUAL (edges1[3], &e4A);
	BOOST_CHECK_EQUAL (edges1[4], &e5D);
	BOOST_CHECK_EQUAL (edges1[5], &e6AD);
	BOOST_CHECK_EQUAL (edges2[0], &e6bAD);
	BOOST_CHECK_EQUAL (edges2[1], &e7AD);
	BOOST_CHECK_EQUAL (edges2[2], &e8A);

	BOOST_CHECK_EQUAL (e1D.getFollowingConnectionArrival(), eNULL);
	BOOST_CHECK_EQUAL (e1D.getFollowingArrivalForFineSteppingOnly(), &e3AD);
	BOOST_CHECK_EQUAL (e1D.getPreviousConnectionDeparture(), eNULL);
	BOOST_CHECK_EQUAL (e1D.getPreviousDepartureForFineSteppingOnly(), eNULL);
	BOOST_CHECK_EQUAL (e1D.getPrevious(), eNULL);
	BOOST_CHECK_EQUAL (e1D.getNext(), &e2D);
	BOOST_CHECK_EQUAL (e1D.getParentPath(), &l1);

	BOOST_CHECK_EQUAL (e2D.getFollowingConnectionArrival(), eNULL);
	BOOST_CHECK_EQUAL (e2D.getFollowingArrivalForFineSteppingOnly(), &e3AD);
	BOOST_CHECK_EQUAL (e2D.getPreviousConnectionDeparture(), &e1D);
	BOOST_CHECK_EQUAL (e2D.getPreviousDepartureForFineSteppingOnly(), &e1D);
	BOOST_CHECK_EQUAL (e2D.getPrevious(), &e1D);
	BOOST_CHECK_EQUAL (e2D.getNext(), &e3AD);
	BOOST_CHECK_EQUAL (e2D.getParentPath(), &l1);

	BOOST_CHECK_EQUAL (e3AD.getFollowingConnectionArrival(), eNULL);
	BOOST_CHECK_EQUAL (e3AD.getFollowingArrivalForFineSteppingOnly(), &e4A);
	BOOST_CHECK_EQUAL (e3AD.getPreviousConnectionDeparture(), &e1D);
	BOOST_CHECK_EQUAL (e3AD.getPreviousDepartureForFineSteppingOnly(), &e2D);
	BOOST_CHECK_EQUAL (e3AD.getPrevious(), &e2D);
	BOOST_CHECK_EQUAL (e3AD.getNext(), &e4A);
	BOOST_CHECK_EQUAL (e3AD.getParentPath(), &l1);

	BOOST_CHECK_EQUAL (e4A.getFollowingConnectionArrival(), eNULL);
	BOOST_CHECK_EQUAL (e4A.getFollowingArrivalForFineSteppingOnly(), &e6AD);
	BOOST_CHECK_EQUAL (e4A.getPreviousConnectionDeparture(), &e1D);
	BOOST_CHECK_EQUAL (e4A.getPreviousDepartureForFineSteppingOnly(), &e3AD);
	BOOST_CHECK_EQUAL (e4A.getPrevious(), &e3AD);
	BOOST_CHECK_EQUAL (e4A.getNext(), &e5D);
	BOOST_CHECK_EQUAL (e4A.getParentPath(), &l1);

	BOOST_CHECK_EQUAL (e5D.getFollowingConnectionArrival(), eNULL);
	BOOST_CHECK_EQUAL (e5D.getFollowingArrivalForFineSteppingOnly(), &e6AD);
	BOOST_CHECK_EQUAL (e5D.getPreviousConnectionDeparture(), &e1D);
	BOOST_CHECK_EQUAL (e5D.getPreviousDepartureForFineSteppingOnly(), &e3AD);
	BOOST_CHECK_EQUAL (e5D.getPrevious(), &e4A);
	BOOST_CHECK_EQUAL (e5D.getNext(), &e6AD);
	BOOST_CHECK_EQUAL (e5D.getParentPath(), &l1);

	BOOST_CHECK_EQUAL (e6AD.getFollowingConnectionArrival(), eNULL);
	BOOST_CHECK_EQUAL (e6AD.getFollowingArrivalForFineSteppingOnly(), eNULL);
	BOOST_CHECK_EQUAL (e6AD.getPreviousConnectionDeparture(), &e5D);
	BOOST_CHECK_EQUAL (e6AD.getPreviousDepartureForFineSteppingOnly(), &e5D);
	BOOST_CHECK_EQUAL (e6AD.getPrevious(), &e5D);
	BOOST_CHECK_EQUAL (e6AD.getNext(), eNULL);
	BOOST_CHECK_EQUAL (e6AD.getParentPath(), &l1);


	BOOST_CHECK_EQUAL (e6bAD.getFollowingConnectionArrival(), &e8A);
	BOOST_CHECK_EQUAL (e6bAD.getFollowingArrivalForFineSteppingOnly(), &e7AD);
	BOOST_CHECK_EQUAL (e6bAD.getPreviousConnectionDeparture(), eNULL);
	BOOST_CHECK_EQUAL (e6bAD.getPreviousDepartureForFineSteppingOnly(), eNULL);
	BOOST_CHECK_EQUAL (e6bAD.getPrevious(), eNULL);
	BOOST_CHECK_EQUAL (e6bAD.getNext(), &e7AD);
	BOOST_CHECK_EQUAL (e6bAD.getParentPath(), &l2);

	BOOST_CHECK_EQUAL (e7AD.getFollowingConnectionArrival(), &e8A);
	BOOST_CHECK_EQUAL (e7AD.getFollowingArrivalForFineSteppingOnly(), &e8A);
	BOOST_CHECK_EQUAL (e7AD.getPreviousConnectionDeparture(), eNULL);
	BOOST_CHECK_EQUAL (e7AD.getPreviousDepartureForFineSteppingOnly(), &e6bAD);
	BOOST_CHECK_EQUAL (e7AD.getPrevious(), &e6bAD);
	BOOST_CHECK_EQUAL (e7AD.getNext(), &e8A);
	BOOST_CHECK_EQUAL (e7AD.getParentPath(), &l2);

	BOOST_CHECK_EQUAL (e8A.getFollowingConnectionArrival(), eNULL);
	BOOST_CHECK_EQUAL (e8A.getFollowingArrivalForFineSteppingOnly(), eNULL);
	BOOST_CHECK_EQUAL (e8A.getPreviousConnectionDeparture(), eNULL);
	BOOST_CHECK_EQUAL (e8A.getPreviousDepartureForFineSteppingOnly(), &e7AD);
	BOOST_CHECK_EQUAL (e8A.getPrevious(), &e7AD);
	BOOST_CHECK_EQUAL (e8A.getNext(), eNULL);
	BOOST_CHECK_EQUAL (e8A.getParentPath(), &l2);

	// MERGE

//	l1.merge(l2);
/* What follows depends on the merge() method which is not available anymore
	const FakePath::Edges& edges3 = l1.getEdges();

	BOOST_CHECK_EQUAL (edges3[0], &e1D);
	BOOST_CHECK_EQUAL (edges3[1], &e2D);
	BOOST_CHECK_EQUAL (edges3[2], &e3AD);
	BOOST_CHECK_EQUAL (edges3[3], &e4A);
	BOOST_CHECK_EQUAL (edges3[4], &e5D);
	BOOST_CHECK_EQUAL (edges3[5], &e6AD);
//	BOOST_CHECK_EQUAL (edges3[5], &e6bAD);
//	BOOST_CHECK_EQUAL (edges3[6], &e7AD);
//	BOOST_CHECK_EQUAL (edges3[7], &e8A);

//	BOOST_CHECK_EQUAL (e1D.getFollowingConnectionArrival(), &e8A);
	BOOST_CHECK_EQUAL (e1D.getFollowingConnectionArrival(), eNULL);
	BOOST_CHECK_EQUAL (e1D.getFollowingArrivalForFineSteppingOnly(), &e3AD);
	BOOST_CHECK_EQUAL (e1D.getPreviousConnectionDeparture(), eNULL);
	BOOST_CHECK_EQUAL (e1D.getPreviousDepartureForFineSteppingOnly(), eNULL);
	BOOST_CHECK_EQUAL (e1D.getPrevious(), eNULL);
	BOOST_CHECK_EQUAL (e1D.getNext(), &e2D);
	BOOST_CHECK_EQUAL (e1D.getParentPath(), &l1);
	BOOST_CHECK_EQUAL (e1D.getRankInPath(), 0);

//	BOOST_CHECK_EQUAL (e2D.getFollowingConnectionArrival(), &e8A);
	BOOST_CHECK_EQUAL (e2D.getFollowingConnectionArrival(), eNULL);
	BOOST_CHECK_EQUAL (e2D.getFollowingArrivalForFineSteppingOnly(), &e3AD);
	BOOST_CHECK_EQUAL (e2D.getPreviousConnectionDeparture(), &e1D);
	BOOST_CHECK_EQUAL (e2D.getPreviousDepartureForFineSteppingOnly(), &e1D);
	BOOST_CHECK_EQUAL (e2D.getPrevious(), &e1D);
	BOOST_CHECK_EQUAL (e2D.getNext(), &e3AD);
	BOOST_CHECK_EQUAL (e2D.getParentPath(), &l1);
	BOOST_CHECK_EQUAL (e2D.getRankInPath(), 1);

//	BOOST_CHECK_EQUAL (e3AD.getFollowingConnectionArrival(), &e8A);
	BOOST_CHECK_EQUAL (e3AD.getFollowingConnectionArrival(), eNULL);
	BOOST_CHECK_EQUAL (e3AD.getFollowingArrivalForFineSteppingOnly(), &e4A);
	BOOST_CHECK_EQUAL (e3AD.getPreviousConnectionDeparture(), &e1D);
	BOOST_CHECK_EQUAL (e3AD.getPreviousDepartureForFineSteppingOnly(), &e2D);
	BOOST_CHECK_EQUAL (e3AD.getPrevious(), &e2D);
	BOOST_CHECK_EQUAL (e3AD.getNext(), &e4A);
	BOOST_CHECK_EQUAL (e3AD.getParentPath(), &l1);
	BOOST_CHECK_EQUAL (e3AD.getRankInPath(), 2);

//	BOOST_CHECK_EQUAL (e4A.getFollowingConnectionArrival(), &e8A);
	BOOST_CHECK_EQUAL (e4A.getFollowingConnectionArrival(), eNULL);
//	BOOST_CHECK_EQUAL (e4A.getFollowingArrivalForFineSteppingOnly(), &e6bAD);
	BOOST_CHECK_EQUAL (e4A.getFollowingArrivalForFineSteppingOnly(), &e6AD);
	BOOST_CHECK_EQUAL (e4A.getPreviousConnectionDeparture(), &e1D);
	BOOST_CHECK_EQUAL (e4A.getPreviousDepartureForFineSteppingOnly(), &e3AD);
	BOOST_CHECK_EQUAL (e4A.getPrevious(), &e3AD);
	BOOST_CHECK_EQUAL (e4A.getNext(), &e5D);
	BOOST_CHECK_EQUAL (e4A.getParentPath(), &l1);
	BOOST_CHECK_EQUAL (e4A.getRankInPath(), 3);

//	BOOST_CHECK_EQUAL (e5D.getFollowingConnectionArrival(), &e8A);
	BOOST_CHECK_EQUAL (e5D.getFollowingConnectionArrival(), eNULL);
//	BOOST_CHECK_EQUAL (e5D.getFollowingArrivalForFineSteppingOnly(), &e6bAD);
	BOOST_CHECK_EQUAL (e5D.getFollowingArrivalForFineSteppingOnly(), &e6AD);
	BOOST_CHECK_EQUAL (e5D.getPreviousConnectionDeparture(), &e1D);
	BOOST_CHECK_EQUAL (e5D.getPreviousDepartureForFineSteppingOnly(), &e3AD);
	BOOST_CHECK_EQUAL (e5D.getPrevious(), &e4A);
//	BOOST_CHECK_EQUAL (e5D.getNext(), &e6bAD);
	BOOST_CHECK_EQUAL (e5D.getNext(), &e6AD);
	BOOST_CHECK_EQUAL (e5D.getParentPath(), &l1);
	BOOST_CHECK_EQUAL (e5D.getRankInPath(), 4);

	BOOST_CHECK_EQUAL (e6bAD.getFollowingConnectionArrival(), &e8A);
	BOOST_CHECK_EQUAL (e6bAD.getFollowingArrivalForFineSteppingOnly(), &e7AD);
//	BOOST_CHECK_EQUAL (e6bAD.getPreviousConnectionDeparture(), &e5D);
	BOOST_CHECK_EQUAL (e6bAD.getPreviousConnectionDeparture(), eNULL);
//	BOOST_CHECK_EQUAL (e6bAD.getPreviousDepartureForFineSteppingOnly(), &e5D);
	BOOST_CHECK_EQUAL (e6bAD.getPreviousDepartureForFineSteppingOnly(), eNULL);
//	BOOST_CHECK_EQUAL (e6bAD.getPrevious(), &e5D);
	BOOST_CHECK_EQUAL (e6bAD.getPrevious(), eNULL);
	BOOST_CHECK_EQUAL (e6bAD.getNext(), &e7AD);
//	BOOST_CHECK_EQUAL (e6bAD.getParentPath(), &l1);
//	BOOST_CHECK_EQUAL (e6bAD.getRankInPath(), 5);

	BOOST_CHECK_EQUAL (e7AD.getFollowingConnectionArrival(), &e8A);
	BOOST_CHECK_EQUAL (e7AD.getFollowingArrivalForFineSteppingOnly(), &e8A);
//	BOOST_CHECK_EQUAL (e7AD.getPreviousConnectionDeparture(), &e5D);
	BOOST_CHECK_EQUAL (e7AD.getPreviousConnectionDeparture(), eNULL);
	BOOST_CHECK_EQUAL (e7AD.getPreviousDepartureForFineSteppingOnly(), &e6bAD);
	BOOST_CHECK_EQUAL (e7AD.getPrevious(), &e6bAD);
	BOOST_CHECK_EQUAL (e7AD.getNext(), &e8A);
//	BOOST_CHECK_EQUAL (e7AD.getParentPath(), &l1);
//	BOOST_CHECK_EQUAL (e7AD.getRankInPath(), 6);

	BOOST_CHECK_EQUAL (e8A.getFollowingConnectionArrival(), eNULL);
	BOOST_CHECK_EQUAL (e8A.getFollowingArrivalForFineSteppingOnly(), eNULL);
//	BOOST_CHECK_EQUAL (e8A.getPreviousConnectionDeparture(), &e5D);
	BOOST_CHECK_EQUAL (e8A.getPreviousConnectionDeparture(), eNULL);
	BOOST_CHECK_EQUAL (e8A.getPreviousDepartureForFineSteppingOnly(), &e7AD);
	BOOST_CHECK_EQUAL (e8A.getPrevious(), &e7AD);
	BOOST_CHECK_EQUAL (e8A.getNext(), eNULL);
//	BOOST_CHECK_EQUAL (e8A.getParentPath(), &l1);
//	BOOST_CHECK_EQUAL (e8A.getRankInPath(), 7);
*/
}
