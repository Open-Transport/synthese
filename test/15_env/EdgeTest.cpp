
/** Edge unit test.
	@file EdgeTest.cpp

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

#include "Line.h"
#include "LineStop.h"
#include "Path.h"
#include "Hub.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "PhysicalStop.h"

#include "01_util/Constants.h"

#include <iostream>

#include <boost/test/auto_unit_test.hpp>

using namespace synthese::util;
using namespace synthese::env;
using namespace synthese;

BOOST_AUTO_TEST_CASE (testEdgeOrderedLinking)
{

	Line l;

	PublicTransportStopZoneConnectionPlace p1(UNKNOWN_VALUE, true);
	PublicTransportStopZoneConnectionPlace p2(UNKNOWN_VALUE, false);
	PublicTransportStopZoneConnectionPlace p3(UNKNOWN_VALUE, false);
	PublicTransportStopZoneConnectionPlace p4(UNKNOWN_VALUE, false);
	PublicTransportStopZoneConnectionPlace p5(UNKNOWN_VALUE, true);
	PublicTransportStopZoneConnectionPlace p6(UNKNOWN_VALUE, true);
	PublicTransportStopZoneConnectionPlace p7(UNKNOWN_VALUE, false);
	PublicTransportStopZoneConnectionPlace p8(UNKNOWN_VALUE, false);

	PhysicalStop s1(UNKNOWN_VALUE, "s1", &p1);
	PhysicalStop s2(UNKNOWN_VALUE, "s1", &p2);
	PhysicalStop s3(UNKNOWN_VALUE, "s1", &p3);
	PhysicalStop s4(UNKNOWN_VALUE, "s1", &p4);
	PhysicalStop s5(UNKNOWN_VALUE, "s1", &p5);
	PhysicalStop s6(UNKNOWN_VALUE, "s1", &p6);
	PhysicalStop s7(UNKNOWN_VALUE, "s1", &p7);
	PhysicalStop s8(UNKNOWN_VALUE, "s1", &p8);

	LineStop l1D(0, &l, 0, true, false,0,&s1);
	LineStop l2D(0, &l, 1, true, false,0,&s2);
	LineStop l3AD(0, &l, 2, true, true, 0,&s3);
	LineStop l4A(0, &l, 3, false, true,0,&s4);
	LineStop l5D(0, &l, 4, true, false,0,&s5);
	LineStop l6AD(0, &l, 5, true, true,0,&s6);
	LineStop l7AD(0, &l, 6, true, true,0,&s7);
	LineStop l8A(0, &l, 7, false, true,0,&s8);
	LineStop* lNULL(NULL);

	l.addEdge(&l1D);

	BOOST_CHECK_EQUAL (l1D.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getFollowingArrivalForFineSteppingOnly(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getPreviousConnectionDeparture(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getPreviousDepartureForFineSteppingOnly(), lNULL);

	l.addEdge(&l2D);

	BOOST_CHECK_EQUAL (l1D.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getFollowingArrivalForFineSteppingOnly(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getPreviousConnectionDeparture(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getPreviousDepartureForFineSteppingOnly(), lNULL);

	BOOST_CHECK_EQUAL (l2D.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l2D.getFollowingArrivalForFineSteppingOnly(), lNULL);
	BOOST_CHECK_EQUAL (l2D.getPreviousConnectionDeparture(), &l1D);
	BOOST_CHECK_EQUAL (l2D.getPreviousDepartureForFineSteppingOnly(), &l1D);

	l.addEdge(&l3AD);

	BOOST_CHECK_EQUAL (l1D.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getFollowingArrivalForFineSteppingOnly(), &l3AD);
	BOOST_CHECK_EQUAL (l1D.getPreviousConnectionDeparture(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getPreviousDepartureForFineSteppingOnly(), lNULL);

	BOOST_CHECK_EQUAL (l2D.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l2D.getFollowingArrivalForFineSteppingOnly(), &l3AD);
	BOOST_CHECK_EQUAL (l2D.getPreviousConnectionDeparture(), &l1D);
	BOOST_CHECK_EQUAL (l2D.getPreviousDepartureForFineSteppingOnly(), &l1D);

	BOOST_CHECK_EQUAL (l3AD.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l3AD.getFollowingArrivalForFineSteppingOnly(), lNULL);
	BOOST_CHECK_EQUAL (l3AD.getPreviousConnectionDeparture(), &l1D);
	BOOST_CHECK_EQUAL (l3AD.getPreviousDepartureForFineSteppingOnly(), &l2D);

	l.addEdge(&l4A);

	BOOST_CHECK_EQUAL (l1D.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getFollowingArrivalForFineSteppingOnly(), &l3AD);
	BOOST_CHECK_EQUAL (l1D.getPreviousConnectionDeparture(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getPreviousDepartureForFineSteppingOnly(), lNULL);

	BOOST_CHECK_EQUAL (l2D.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l2D.getFollowingArrivalForFineSteppingOnly(), &l3AD);
	BOOST_CHECK_EQUAL (l2D.getPreviousConnectionDeparture(), &l1D);
	BOOST_CHECK_EQUAL (l2D.getPreviousDepartureForFineSteppingOnly(), &l1D);

	BOOST_CHECK_EQUAL (l3AD.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l3AD.getFollowingArrivalForFineSteppingOnly(), &l4A);
	BOOST_CHECK_EQUAL (l3AD.getPreviousConnectionDeparture(), &l1D);
	BOOST_CHECK_EQUAL (l3AD.getPreviousDepartureForFineSteppingOnly(), &l2D);

	BOOST_CHECK_EQUAL (l4A.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l4A.getFollowingArrivalForFineSteppingOnly(), lNULL);
	BOOST_CHECK_EQUAL (l4A.getPreviousConnectionDeparture(), &l1D);
	BOOST_CHECK_EQUAL (l4A.getPreviousDepartureForFineSteppingOnly(), &l3AD);

	l.addEdge(&l5D);

	BOOST_CHECK_EQUAL (l1D.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getFollowingArrivalForFineSteppingOnly(), &l3AD);
	BOOST_CHECK_EQUAL (l1D.getPreviousConnectionDeparture(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getPreviousDepartureForFineSteppingOnly(), lNULL);

	BOOST_CHECK_EQUAL (l2D.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l2D.getFollowingArrivalForFineSteppingOnly(), &l3AD);
	BOOST_CHECK_EQUAL (l2D.getPreviousConnectionDeparture(), &l1D);
	BOOST_CHECK_EQUAL (l2D.getPreviousDepartureForFineSteppingOnly(), &l1D);

	BOOST_CHECK_EQUAL (l3AD.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l3AD.getFollowingArrivalForFineSteppingOnly(), &l4A);
	BOOST_CHECK_EQUAL (l3AD.getPreviousConnectionDeparture(), &l1D);
	BOOST_CHECK_EQUAL (l3AD.getPreviousDepartureForFineSteppingOnly(), &l2D);

	BOOST_CHECK_EQUAL (l4A.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l4A.getFollowingArrivalForFineSteppingOnly(), lNULL);
	BOOST_CHECK_EQUAL (l4A.getPreviousConnectionDeparture(), &l1D);
	BOOST_CHECK_EQUAL (l4A.getPreviousDepartureForFineSteppingOnly(), &l3AD);

	BOOST_CHECK_EQUAL (l5D.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l5D.getFollowingArrivalForFineSteppingOnly(), lNULL);
	BOOST_CHECK_EQUAL (l5D.getPreviousConnectionDeparture(), &l1D);
	BOOST_CHECK_EQUAL (l5D.getPreviousDepartureForFineSteppingOnly(), &l3AD);

	l.addEdge(&l6AD);

	BOOST_CHECK_EQUAL (l1D.getFollowingConnectionArrival(), &l6AD);
	BOOST_CHECK_EQUAL (l1D.getFollowingArrivalForFineSteppingOnly(), &l3AD);
	BOOST_CHECK_EQUAL (l1D.getPreviousConnectionDeparture(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getPreviousDepartureForFineSteppingOnly(), lNULL);

	BOOST_CHECK_EQUAL (l2D.getFollowingConnectionArrival(), &l6AD);
	BOOST_CHECK_EQUAL (l2D.getFollowingArrivalForFineSteppingOnly(), &l3AD);
	BOOST_CHECK_EQUAL (l2D.getPreviousConnectionDeparture(), &l1D);
	BOOST_CHECK_EQUAL (l2D.getPreviousDepartureForFineSteppingOnly(), &l1D);

	BOOST_CHECK_EQUAL (l3AD.getFollowingConnectionArrival(), &l6AD);
	BOOST_CHECK_EQUAL (l3AD.getFollowingArrivalForFineSteppingOnly(), &l4A);
	BOOST_CHECK_EQUAL (l3AD.getPreviousConnectionDeparture(), &l1D);
	BOOST_CHECK_EQUAL (l3AD.getPreviousDepartureForFineSteppingOnly(), &l2D);

	BOOST_CHECK_EQUAL (l4A.getFollowingConnectionArrival(), &l6AD);
	BOOST_CHECK_EQUAL (l4A.getFollowingArrivalForFineSteppingOnly(), &l6AD);
	BOOST_CHECK_EQUAL (l4A.getPreviousConnectionDeparture(), &l1D);
	BOOST_CHECK_EQUAL (l4A.getPreviousDepartureForFineSteppingOnly(), &l3AD);

	BOOST_CHECK_EQUAL (l5D.getFollowingConnectionArrival(), &l6AD);
	BOOST_CHECK_EQUAL (l5D.getFollowingArrivalForFineSteppingOnly(), &l6AD);
	BOOST_CHECK_EQUAL (l5D.getPreviousConnectionDeparture(), &l1D);
	BOOST_CHECK_EQUAL (l5D.getPreviousDepartureForFineSteppingOnly(), &l3AD);

	BOOST_CHECK_EQUAL (l6AD.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l6AD.getFollowingArrivalForFineSteppingOnly(), lNULL);
	BOOST_CHECK_EQUAL (l6AD.getPreviousConnectionDeparture(), &l5D);
	BOOST_CHECK_EQUAL (l6AD.getPreviousDepartureForFineSteppingOnly(), &l5D);

	l.addEdge(&l7AD);

	BOOST_CHECK_EQUAL (l1D.getFollowingConnectionArrival(), &l6AD);
	BOOST_CHECK_EQUAL (l1D.getFollowingArrivalForFineSteppingOnly(), &l3AD);
	BOOST_CHECK_EQUAL (l1D.getPreviousConnectionDeparture(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getPreviousDepartureForFineSteppingOnly(), lNULL);

	BOOST_CHECK_EQUAL (l2D.getFollowingConnectionArrival(), &l6AD);
	BOOST_CHECK_EQUAL (l2D.getFollowingArrivalForFineSteppingOnly(), &l3AD);
	BOOST_CHECK_EQUAL (l2D.getPreviousConnectionDeparture(), &l1D);
	BOOST_CHECK_EQUAL (l2D.getPreviousDepartureForFineSteppingOnly(), &l1D);

	BOOST_CHECK_EQUAL (l3AD.getFollowingConnectionArrival(), &l6AD);
	BOOST_CHECK_EQUAL (l3AD.getFollowingArrivalForFineSteppingOnly(), &l4A);
	BOOST_CHECK_EQUAL (l3AD.getPreviousConnectionDeparture(), &l1D);
	BOOST_CHECK_EQUAL (l3AD.getPreviousDepartureForFineSteppingOnly(), &l2D);

	BOOST_CHECK_EQUAL (l4A.getFollowingConnectionArrival(), &l6AD);
	BOOST_CHECK_EQUAL (l4A.getFollowingArrivalForFineSteppingOnly(), &l6AD);
	BOOST_CHECK_EQUAL (l4A.getPreviousConnectionDeparture(), &l1D);
	BOOST_CHECK_EQUAL (l4A.getPreviousDepartureForFineSteppingOnly(), &l3AD);

	BOOST_CHECK_EQUAL (l5D.getFollowingConnectionArrival(), &l6AD);
	BOOST_CHECK_EQUAL (l5D.getFollowingArrivalForFineSteppingOnly(), &l6AD);
	BOOST_CHECK_EQUAL (l5D.getPreviousConnectionDeparture(), &l1D);
	BOOST_CHECK_EQUAL (l5D.getPreviousDepartureForFineSteppingOnly(), &l3AD);

	BOOST_CHECK_EQUAL (l6AD.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l6AD.getFollowingArrivalForFineSteppingOnly(), &l7AD);
	BOOST_CHECK_EQUAL (l6AD.getPreviousConnectionDeparture(), &l5D);
	BOOST_CHECK_EQUAL (l6AD.getPreviousDepartureForFineSteppingOnly(), &l5D);

	BOOST_CHECK_EQUAL (l7AD.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l7AD.getFollowingArrivalForFineSteppingOnly(), lNULL);
	BOOST_CHECK_EQUAL (l7AD.getPreviousConnectionDeparture(), &l6AD);
	BOOST_CHECK_EQUAL (l7AD.getPreviousDepartureForFineSteppingOnly(), &l6AD);

	l.addEdge(&l8A);

	Line::Edges edges = l.getEdges();

	BOOST_CHECK_EQUAL (edges[0], &l1D);
	BOOST_CHECK_EQUAL (edges[1], &l2D);
	BOOST_CHECK_EQUAL (edges[2], &l3AD);
	BOOST_CHECK_EQUAL (edges[3], &l4A);
	BOOST_CHECK_EQUAL (edges[4], &l5D);
	BOOST_CHECK_EQUAL (edges[5], &l6AD);
	BOOST_CHECK_EQUAL (edges[6], &l7AD);
	BOOST_CHECK_EQUAL (edges[7], &l8A);

	BOOST_CHECK_EQUAL (l1D.getFollowingConnectionArrival(), &l6AD);
	BOOST_CHECK_EQUAL (l1D.getFollowingArrivalForFineSteppingOnly(), &l3AD);
	BOOST_CHECK_EQUAL (l1D.getPreviousConnectionDeparture(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getPreviousDepartureForFineSteppingOnly(), lNULL);

	BOOST_CHECK_EQUAL (l2D.getFollowingConnectionArrival(), &l6AD);
	BOOST_CHECK_EQUAL (l2D.getFollowingArrivalForFineSteppingOnly(), &l3AD);
	BOOST_CHECK_EQUAL (l2D.getPreviousConnectionDeparture(), &l1D);
	BOOST_CHECK_EQUAL (l2D.getPreviousDepartureForFineSteppingOnly(), &l1D);

	BOOST_CHECK_EQUAL (l3AD.getFollowingConnectionArrival(), &l6AD);
	BOOST_CHECK_EQUAL (l3AD.getFollowingArrivalForFineSteppingOnly(), &l4A);
	BOOST_CHECK_EQUAL (l3AD.getPreviousConnectionDeparture(), &l1D);
	BOOST_CHECK_EQUAL (l3AD.getPreviousDepartureForFineSteppingOnly(), &l2D);

	BOOST_CHECK_EQUAL (l4A.getFollowingConnectionArrival(), &l6AD);
	BOOST_CHECK_EQUAL (l4A.getFollowingArrivalForFineSteppingOnly(), &l6AD);
	BOOST_CHECK_EQUAL (l4A.getPreviousConnectionDeparture(), &l1D);
	BOOST_CHECK_EQUAL (l4A.getPreviousDepartureForFineSteppingOnly(), &l3AD);

	BOOST_CHECK_EQUAL (l5D.getFollowingConnectionArrival(), &l6AD);
	BOOST_CHECK_EQUAL (l5D.getFollowingArrivalForFineSteppingOnly(), &l6AD);
	BOOST_CHECK_EQUAL (l5D.getPreviousConnectionDeparture(), &l1D);
	BOOST_CHECK_EQUAL (l5D.getPreviousDepartureForFineSteppingOnly(), &l3AD);

	BOOST_CHECK_EQUAL (l6AD.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l6AD.getFollowingArrivalForFineSteppingOnly(), &l7AD);
	BOOST_CHECK_EQUAL (l6AD.getPreviousConnectionDeparture(), &l5D);
	BOOST_CHECK_EQUAL (l6AD.getPreviousDepartureForFineSteppingOnly(), &l5D);

	BOOST_CHECK_EQUAL (l7AD.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l7AD.getFollowingArrivalForFineSteppingOnly(), &l8A);
	BOOST_CHECK_EQUAL (l7AD.getPreviousConnectionDeparture(), &l6AD);
	BOOST_CHECK_EQUAL (l7AD.getPreviousDepartureForFineSteppingOnly(), &l6AD);

	BOOST_CHECK_EQUAL (l8A.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l8A.getFollowingArrivalForFineSteppingOnly(), lNULL);
	BOOST_CHECK_EQUAL (l8A.getPreviousConnectionDeparture(), &l6AD);
	BOOST_CHECK_EQUAL (l8A.getPreviousDepartureForFineSteppingOnly(), &l7AD);

}



BOOST_AUTO_TEST_CASE (testEdgeRandomLinking)
{

	Line l;

	PublicTransportStopZoneConnectionPlace p1(UNKNOWN_VALUE, true);
	PublicTransportStopZoneConnectionPlace p2(UNKNOWN_VALUE, false);
	PublicTransportStopZoneConnectionPlace p3(UNKNOWN_VALUE, false);
	PublicTransportStopZoneConnectionPlace p4(UNKNOWN_VALUE, false);
	PublicTransportStopZoneConnectionPlace p5(UNKNOWN_VALUE, true);
	PublicTransportStopZoneConnectionPlace p6(UNKNOWN_VALUE, true);
	PublicTransportStopZoneConnectionPlace p7(UNKNOWN_VALUE, false);
	PublicTransportStopZoneConnectionPlace p8(UNKNOWN_VALUE, false);

	PhysicalStop s1(UNKNOWN_VALUE, "s1", &p1);
	PhysicalStop s2(UNKNOWN_VALUE, "s1", &p2);
	PhysicalStop s3(UNKNOWN_VALUE, "s1", &p3);
	PhysicalStop s4(UNKNOWN_VALUE, "s1", &p4);
	PhysicalStop s5(UNKNOWN_VALUE, "s1", &p5);
	PhysicalStop s6(UNKNOWN_VALUE, "s1", &p6);
	PhysicalStop s7(UNKNOWN_VALUE, "s1", &p7);
	PhysicalStop s8(UNKNOWN_VALUE, "s1", &p8);

	LineStop l1D(0, &l, 0, true, false,0,&s1);
	LineStop l2D(0, &l, 1, true, false,0,&s2);
	LineStop l3AD(0, &l, 2, true, true, 0,&s3);
	LineStop l4A(0, &l, 3, false, true,0,&s4);
	LineStop l5D(0, &l, 4, true, false,0,&s5);
	LineStop l6AD(0, &l, 5, true, true,0,&s6);
	LineStop l7AD(0, &l, 6, true, true,0,&s7);
	LineStop l8A(0, &l, 7, false, true,0,&s8);
	LineStop* lNULL(NULL);

	l.addEdge(&l4A);

	Line::Edges edges = l.getEdges();
	BOOST_CHECK_EQUAL (edges[0], &l4A);

	BOOST_CHECK_EQUAL (l4A.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l4A.getFollowingArrivalForFineSteppingOnly(), lNULL);
	BOOST_CHECK_EQUAL (l4A.getPreviousConnectionDeparture(), lNULL);
	BOOST_CHECK_EQUAL (l4A.getPreviousDepartureForFineSteppingOnly(), lNULL);

	l.addEdge(&l2D);

	edges = l.getEdges();
	BOOST_CHECK_EQUAL (edges[0], &l2D);
	BOOST_CHECK_EQUAL (edges[1], &l4A);

	BOOST_CHECK_EQUAL (l2D.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l2D.getFollowingArrivalForFineSteppingOnly(), &l4A);
	BOOST_CHECK_EQUAL (l2D.getPreviousConnectionDeparture(), lNULL);
	BOOST_CHECK_EQUAL (l2D.getPreviousDepartureForFineSteppingOnly(), lNULL);

	BOOST_CHECK_EQUAL (l4A.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l4A.getFollowingArrivalForFineSteppingOnly(), lNULL);
	BOOST_CHECK_EQUAL (l4A.getPreviousConnectionDeparture(), lNULL);
	BOOST_CHECK_EQUAL (l4A.getPreviousDepartureForFineSteppingOnly(), &l2D);

	l.addEdge(&l7AD);

	edges = l.getEdges();
	BOOST_CHECK_EQUAL (edges[0], &l2D);
	BOOST_CHECK_EQUAL (edges[1], &l4A);
	BOOST_CHECK_EQUAL (edges[2], &l7AD);

	BOOST_CHECK_EQUAL (l2D.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l2D.getFollowingArrivalForFineSteppingOnly(), &l4A);
	BOOST_CHECK_EQUAL (l2D.getPreviousConnectionDeparture(), lNULL);
	BOOST_CHECK_EQUAL (l2D.getPreviousDepartureForFineSteppingOnly(), lNULL);

	BOOST_CHECK_EQUAL (l4A.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l4A.getFollowingArrivalForFineSteppingOnly(), &l7AD);
	BOOST_CHECK_EQUAL (l4A.getPreviousConnectionDeparture(), lNULL);
	BOOST_CHECK_EQUAL (l4A.getPreviousDepartureForFineSteppingOnly(), &l2D);

	BOOST_CHECK_EQUAL (l7AD.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l7AD.getFollowingArrivalForFineSteppingOnly(), lNULL);
	BOOST_CHECK_EQUAL (l7AD.getPreviousConnectionDeparture(), lNULL);
	BOOST_CHECK_EQUAL (l7AD.getPreviousDepartureForFineSteppingOnly(), &l2D);

	l.addEdge(&l5D);

	edges = l.getEdges();
	BOOST_CHECK_EQUAL (edges[0], &l2D);
	BOOST_CHECK_EQUAL (edges[1], &l4A);
	BOOST_CHECK_EQUAL (edges[2], &l5D);
	BOOST_CHECK_EQUAL (edges[3], &l7AD);

	BOOST_CHECK_EQUAL (l2D.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l2D.getFollowingArrivalForFineSteppingOnly(), &l4A);
	BOOST_CHECK_EQUAL (l2D.getPreviousConnectionDeparture(), lNULL);
	BOOST_CHECK_EQUAL (l2D.getPreviousDepartureForFineSteppingOnly(), lNULL);

	BOOST_CHECK_EQUAL (l4A.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l4A.getFollowingArrivalForFineSteppingOnly(), &l7AD);
	BOOST_CHECK_EQUAL (l4A.getPreviousConnectionDeparture(), lNULL);
	BOOST_CHECK_EQUAL (l4A.getPreviousDepartureForFineSteppingOnly(), &l2D);

	BOOST_CHECK_EQUAL (l5D.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l5D.getFollowingArrivalForFineSteppingOnly(), &l7AD);
	BOOST_CHECK_EQUAL (l5D.getPreviousConnectionDeparture(), lNULL);
	BOOST_CHECK_EQUAL (l5D.getPreviousDepartureForFineSteppingOnly(), &l2D);

	BOOST_CHECK_EQUAL (l7AD.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l7AD.getFollowingArrivalForFineSteppingOnly(), lNULL);
	BOOST_CHECK_EQUAL (l7AD.getPreviousConnectionDeparture(), &l5D);
	BOOST_CHECK_EQUAL (l7AD.getPreviousDepartureForFineSteppingOnly(), &l5D);

	l.addEdge(&l6AD);

	edges = l.getEdges();
	BOOST_CHECK_EQUAL (edges[0], &l2D);
	BOOST_CHECK_EQUAL (edges[1], &l4A);
	BOOST_CHECK_EQUAL (edges[2], &l5D);
	BOOST_CHECK_EQUAL (edges[3], &l6AD);
	BOOST_CHECK_EQUAL (edges[4], &l7AD);

	BOOST_CHECK_EQUAL (l2D.getFollowingConnectionArrival(), &l6AD);
	BOOST_CHECK_EQUAL (l2D.getFollowingArrivalForFineSteppingOnly(), &l4A);
	BOOST_CHECK_EQUAL (l2D.getPreviousConnectionDeparture(), lNULL);
	BOOST_CHECK_EQUAL (l2D.getPreviousDepartureForFineSteppingOnly(), lNULL);

	BOOST_CHECK_EQUAL (l4A.getFollowingConnectionArrival(), &l6AD);
	BOOST_CHECK_EQUAL (l4A.getFollowingArrivalForFineSteppingOnly(), &l6AD);
	BOOST_CHECK_EQUAL (l4A.getPreviousConnectionDeparture(), lNULL);
	BOOST_CHECK_EQUAL (l4A.getPreviousDepartureForFineSteppingOnly(), &l2D);

	BOOST_CHECK_EQUAL (l5D.getFollowingConnectionArrival(), &l6AD);
	BOOST_CHECK_EQUAL (l5D.getFollowingArrivalForFineSteppingOnly(), &l6AD);
	BOOST_CHECK_EQUAL (l5D.getPreviousConnectionDeparture(), lNULL);
	BOOST_CHECK_EQUAL (l5D.getPreviousDepartureForFineSteppingOnly(), &l2D);

	BOOST_CHECK_EQUAL (l6AD.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l6AD.getFollowingArrivalForFineSteppingOnly(), &l7AD);
	BOOST_CHECK_EQUAL (l6AD.getPreviousConnectionDeparture(), &l5D);
	BOOST_CHECK_EQUAL (l6AD.getPreviousDepartureForFineSteppingOnly(), &l5D);

	BOOST_CHECK_EQUAL (l7AD.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l7AD.getFollowingArrivalForFineSteppingOnly(), lNULL);
	BOOST_CHECK_EQUAL (l7AD.getPreviousConnectionDeparture(), &l6AD);
	BOOST_CHECK_EQUAL (l7AD.getPreviousDepartureForFineSteppingOnly(), &l6AD);

	l.addEdge(&l1D);
	l.addEdge(&l3AD);
	l.addEdge(&l8A);

	edges = l.getEdges();
	BOOST_CHECK_EQUAL (edges[0], &l1D);
	BOOST_CHECK_EQUAL (edges[1], &l2D);
	BOOST_CHECK_EQUAL (edges[2], &l3AD);
	BOOST_CHECK_EQUAL (edges[3], &l4A);
	BOOST_CHECK_EQUAL (edges[4], &l5D);
	BOOST_CHECK_EQUAL (edges[5], &l6AD);
	BOOST_CHECK_EQUAL (edges[6], &l7AD);
	BOOST_CHECK_EQUAL (edges[7], &l8A);

	BOOST_CHECK_EQUAL (l1D.getFollowingConnectionArrival(), &l6AD);
	BOOST_CHECK_EQUAL (l1D.getFollowingArrivalForFineSteppingOnly(), &l3AD);
	BOOST_CHECK_EQUAL (l1D.getPreviousConnectionDeparture(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getPreviousDepartureForFineSteppingOnly(), lNULL);

	BOOST_CHECK_EQUAL (l2D.getFollowingConnectionArrival(), &l6AD);
	BOOST_CHECK_EQUAL (l2D.getFollowingArrivalForFineSteppingOnly(), &l3AD);
	BOOST_CHECK_EQUAL (l2D.getPreviousConnectionDeparture(), &l1D);
	BOOST_CHECK_EQUAL (l2D.getPreviousDepartureForFineSteppingOnly(), &l1D);

	BOOST_CHECK_EQUAL (l3AD.getFollowingConnectionArrival(), &l6AD);
	BOOST_CHECK_EQUAL (l3AD.getFollowingArrivalForFineSteppingOnly(), &l4A);
	BOOST_CHECK_EQUAL (l3AD.getPreviousConnectionDeparture(), &l1D);
	BOOST_CHECK_EQUAL (l3AD.getPreviousDepartureForFineSteppingOnly(), &l2D);

	BOOST_CHECK_EQUAL (l4A.getFollowingConnectionArrival(), &l6AD);
	BOOST_CHECK_EQUAL (l4A.getFollowingArrivalForFineSteppingOnly(), &l6AD);
	BOOST_CHECK_EQUAL (l4A.getPreviousConnectionDeparture(), &l1D);
	BOOST_CHECK_EQUAL (l4A.getPreviousDepartureForFineSteppingOnly(), &l3AD);

	BOOST_CHECK_EQUAL (l5D.getFollowingConnectionArrival(), &l6AD);
	BOOST_CHECK_EQUAL (l5D.getFollowingArrivalForFineSteppingOnly(), &l6AD);
	BOOST_CHECK_EQUAL (l5D.getPreviousConnectionDeparture(), &l1D);
	BOOST_CHECK_EQUAL (l5D.getPreviousDepartureForFineSteppingOnly(), &l3AD);

	BOOST_CHECK_EQUAL (l6AD.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l6AD.getFollowingArrivalForFineSteppingOnly(), &l7AD);
	BOOST_CHECK_EQUAL (l6AD.getPreviousConnectionDeparture(), &l5D);
	BOOST_CHECK_EQUAL (l6AD.getPreviousDepartureForFineSteppingOnly(), &l5D);

	BOOST_CHECK_EQUAL (l7AD.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l7AD.getFollowingArrivalForFineSteppingOnly(), &l8A);
	BOOST_CHECK_EQUAL (l7AD.getPreviousConnectionDeparture(), &l6AD);
	BOOST_CHECK_EQUAL (l7AD.getPreviousDepartureForFineSteppingOnly(), &l6AD);

	BOOST_CHECK_EQUAL (l8A.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l8A.getFollowingArrivalForFineSteppingOnly(), lNULL);
	BOOST_CHECK_EQUAL (l8A.getPreviousConnectionDeparture(), &l6AD);
	BOOST_CHECK_EQUAL (l8A.getPreviousDepartureForFineSteppingOnly(), &l7AD);

}



