
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

#include "JourneyPattern.hpp"
#include "DesignatedLinePhysicalStop.hpp"
#include "Path.h"
#include "Hub.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "GeographyModule.h"
#include "DRTArea.hpp"
#include "LineArea.hpp"
#include "AreaGeneratedLineStop.hpp"

#include "UtilConstants.h"

#include <iostream>

#include <boost/test/auto_unit_test.hpp>

using namespace synthese::util;
using namespace synthese::pt;
using namespace synthese::geography;
using namespace synthese::graph;
using namespace synthese;

BOOST_AUTO_TEST_CASE (testEdgeOrderedLinking)
{
	GeographyModule::PreInit();

	JourneyPattern l;

	StopArea p1(UNKNOWN_VALUE, true);
	StopArea p2(UNKNOWN_VALUE, false);
	StopArea p3(UNKNOWN_VALUE, false);
	StopArea p4(UNKNOWN_VALUE, false);
	StopArea p5(UNKNOWN_VALUE, true);
	StopArea p6(UNKNOWN_VALUE, true);
	StopArea p7(UNKNOWN_VALUE, false);
	StopArea p8(UNKNOWN_VALUE, false);

	StopPoint s1(UNKNOWN_VALUE, "s1", &p1);
	StopPoint s2(UNKNOWN_VALUE, "s1", &p2);
	StopPoint s3(UNKNOWN_VALUE, "s1", &p3);
	StopPoint s4(UNKNOWN_VALUE, "s1", &p4);
	StopPoint s5(UNKNOWN_VALUE, "s1", &p5);
	StopPoint s6(UNKNOWN_VALUE, "s1", &p6);
	StopPoint s7(UNKNOWN_VALUE, "s1", &p7);
	StopPoint s8(UNKNOWN_VALUE, "s1", &p8);

	DesignatedLinePhysicalStop l1D(0, &l, 0, true, false,0,&s1);
	DesignatedLinePhysicalStop l2D(0, &l, 1, true, false,0,&s2);
	DesignatedLinePhysicalStop l3AD(0, &l, 2, true, true, 0,&s3);
	DesignatedLinePhysicalStop l4A(0, &l, 3, false, true,0,&s4);
	DesignatedLinePhysicalStop l5D(0, &l, 4, true, false,0,&s5);
	DesignatedLinePhysicalStop l6AD(0, &l, 5, true, true,0,&s6);
	DesignatedLinePhysicalStop l7AD(0, &l, 6, true, true,0,&s7);
	DesignatedLinePhysicalStop l8A(0, &l, 7, false, true,0,&s8);
	DesignatedLinePhysicalStop* lNULL(NULL);

	l.addEdge(l1D);

	BOOST_CHECK_EQUAL (l1D.getPrevious(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getNext(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getFollowingArrivalForFineSteppingOnly(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getPreviousConnectionDeparture(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getPreviousDepartureForFineSteppingOnly(), lNULL);

	l.addEdge(l2D);

	BOOST_CHECK_EQUAL (l1D.getPrevious(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getNext(), &l2D);
	BOOST_CHECK_EQUAL (l1D.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getFollowingArrivalForFineSteppingOnly(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getPreviousConnectionDeparture(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getPreviousDepartureForFineSteppingOnly(), lNULL);

	BOOST_CHECK_EQUAL (l2D.getPrevious(), &l1D);
	BOOST_CHECK_EQUAL (l2D.getNext(), lNULL);
	BOOST_CHECK_EQUAL (l2D.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l2D.getFollowingArrivalForFineSteppingOnly(), lNULL);
	BOOST_CHECK_EQUAL (l2D.getPreviousConnectionDeparture(), &l1D);
	BOOST_CHECK_EQUAL (l2D.getPreviousDepartureForFineSteppingOnly(), &l1D);

	l.addEdge(l3AD);

	BOOST_CHECK_EQUAL (l1D.getPrevious(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getNext(), &l2D);
	BOOST_CHECK_EQUAL (l1D.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getFollowingArrivalForFineSteppingOnly(), &l3AD);
	BOOST_CHECK_EQUAL (l1D.getPreviousConnectionDeparture(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getPreviousDepartureForFineSteppingOnly(), lNULL);

	BOOST_CHECK_EQUAL (l2D.getPrevious(), &l1D);
	BOOST_CHECK_EQUAL (l2D.getNext(), &l3AD);
	BOOST_CHECK_EQUAL (l2D.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l2D.getFollowingArrivalForFineSteppingOnly(), &l3AD);
	BOOST_CHECK_EQUAL (l2D.getPreviousConnectionDeparture(), &l1D);
	BOOST_CHECK_EQUAL (l2D.getPreviousDepartureForFineSteppingOnly(), &l1D);

	BOOST_CHECK_EQUAL (l3AD.getPrevious(), &l2D);
	BOOST_CHECK_EQUAL (l3AD.getNext(), lNULL);
	BOOST_CHECK_EQUAL (l3AD.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l3AD.getFollowingArrivalForFineSteppingOnly(), lNULL);
	BOOST_CHECK_EQUAL (l3AD.getPreviousConnectionDeparture(), &l1D);
	BOOST_CHECK_EQUAL (l3AD.getPreviousDepartureForFineSteppingOnly(), &l2D);

	l.addEdge(l4A);

	BOOST_CHECK_EQUAL (l1D.getPrevious(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getNext(), &l2D);
	BOOST_CHECK_EQUAL (l1D.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getFollowingArrivalForFineSteppingOnly(), &l3AD);
	BOOST_CHECK_EQUAL (l1D.getPreviousConnectionDeparture(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getPreviousDepartureForFineSteppingOnly(), lNULL);

	BOOST_CHECK_EQUAL (l2D.getPrevious(), &l1D);
	BOOST_CHECK_EQUAL (l2D.getNext(), &l3AD);
	BOOST_CHECK_EQUAL (l2D.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l2D.getFollowingArrivalForFineSteppingOnly(), &l3AD);
	BOOST_CHECK_EQUAL (l2D.getPreviousConnectionDeparture(), &l1D);
	BOOST_CHECK_EQUAL (l2D.getPreviousDepartureForFineSteppingOnly(), &l1D);

	BOOST_CHECK_EQUAL (l3AD.getPrevious(), &l2D);
	BOOST_CHECK_EQUAL (l3AD.getNext(), &l4A);
	BOOST_CHECK_EQUAL (l3AD.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l3AD.getFollowingArrivalForFineSteppingOnly(), &l4A);
	BOOST_CHECK_EQUAL (l3AD.getPreviousConnectionDeparture(), &l1D);
	BOOST_CHECK_EQUAL (l3AD.getPreviousDepartureForFineSteppingOnly(), &l2D);

	BOOST_CHECK_EQUAL (l4A.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l4A.getFollowingArrivalForFineSteppingOnly(), lNULL);
	BOOST_CHECK_EQUAL (l4A.getPreviousConnectionDeparture(), &l1D);
	BOOST_CHECK_EQUAL (l4A.getPreviousDepartureForFineSteppingOnly(), &l3AD);

	l.addEdge(l5D);

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

	l.addEdge(l6AD);

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

	l.addEdge(l7AD);

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

	l.addEdge(l8A);

	JourneyPattern::Edges edges = l.getEdges();

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

	JourneyPattern l;

	StopArea p1(UNKNOWN_VALUE, true);
	StopArea p2(UNKNOWN_VALUE, false);
	StopArea p3(UNKNOWN_VALUE, false);
	StopArea p4(UNKNOWN_VALUE, false);
	StopArea p5(UNKNOWN_VALUE, true);
	StopArea p6(UNKNOWN_VALUE, true);
	StopArea p7(UNKNOWN_VALUE, false);
	StopArea p8(UNKNOWN_VALUE, false);

	StopPoint s1(UNKNOWN_VALUE, "s1", &p1);
	StopPoint s2(UNKNOWN_VALUE, "s1", &p2);
	StopPoint s3(UNKNOWN_VALUE, "s1", &p3);
	StopPoint s4(UNKNOWN_VALUE, "s1", &p4);
	StopPoint s5(UNKNOWN_VALUE, "s1", &p5);
	StopPoint s6(UNKNOWN_VALUE, "s1", &p6);
	StopPoint s7(UNKNOWN_VALUE, "s1", &p7);
	StopPoint s8(UNKNOWN_VALUE, "s1", &p8);

	DesignatedLinePhysicalStop l1D(0, &l, 0, true, false,0,&s1);
	DesignatedLinePhysicalStop l2D(0, &l, 1, true, false,0,&s2);
	DesignatedLinePhysicalStop l3AD(0, &l, 2, true, true, 0,&s3);
	DesignatedLinePhysicalStop l4A(0, &l, 3, false, true,0,&s4);
	DesignatedLinePhysicalStop l5D(0, &l, 4, true, false,0,&s5);
	DesignatedLinePhysicalStop l6AD(0, &l, 5, true, true,0,&s6);
	DesignatedLinePhysicalStop l7AD(0, &l, 6, true, true,0,&s7);
	DesignatedLinePhysicalStop l8A(0, &l, 7, false, true,0,&s8);
	DesignatedLinePhysicalStop* lNULL(NULL);

	l.addEdge(l4A);

	JourneyPattern::Edges edges = l.getEdges();
	BOOST_CHECK_EQUAL (edges[0], &l4A);

	BOOST_CHECK_EQUAL (l4A.getPrevious(), lNULL);
	BOOST_CHECK_EQUAL (l4A.getNext(), lNULL);
	BOOST_CHECK_EQUAL (l4A.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l4A.getFollowingArrivalForFineSteppingOnly(), lNULL);
	BOOST_CHECK_EQUAL (l4A.getPreviousConnectionDeparture(), lNULL);
	BOOST_CHECK_EQUAL (l4A.getPreviousDepartureForFineSteppingOnly(), lNULL);

	l.addEdge(l2D);

	edges = l.getEdges();
	BOOST_CHECK_EQUAL (edges[0], &l2D);
	BOOST_CHECK_EQUAL (edges[1], &l4A);

	BOOST_CHECK_EQUAL (l2D.getPrevious(), lNULL);
	BOOST_CHECK_EQUAL (l2D.getNext(), &l4A);
	BOOST_CHECK_EQUAL (l2D.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l2D.getFollowingArrivalForFineSteppingOnly(), &l4A);
	BOOST_CHECK_EQUAL (l2D.getPreviousConnectionDeparture(), lNULL);
	BOOST_CHECK_EQUAL (l2D.getPreviousDepartureForFineSteppingOnly(), lNULL);

	BOOST_CHECK_EQUAL (l4A.getPrevious(), &l2D);
	BOOST_CHECK_EQUAL (l4A.getNext(), lNULL);
	BOOST_CHECK_EQUAL (l4A.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l4A.getFollowingArrivalForFineSteppingOnly(), lNULL);
	BOOST_CHECK_EQUAL (l4A.getPreviousConnectionDeparture(), lNULL);
	BOOST_CHECK_EQUAL (l4A.getPreviousDepartureForFineSteppingOnly(), &l2D);

	l.addEdge(l7AD);

	edges = l.getEdges();
	BOOST_CHECK_EQUAL (edges[0], &l2D);
	BOOST_CHECK_EQUAL (edges[1], &l4A);
	BOOST_CHECK_EQUAL (edges[2], &l7AD);

	BOOST_CHECK_EQUAL (l2D.getPrevious(), lNULL);
	BOOST_CHECK_EQUAL (l2D.getNext(), &l4A);
	BOOST_CHECK_EQUAL (l2D.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l2D.getFollowingArrivalForFineSteppingOnly(), &l4A);
	BOOST_CHECK_EQUAL (l2D.getPreviousConnectionDeparture(), lNULL);
	BOOST_CHECK_EQUAL (l2D.getPreviousDepartureForFineSteppingOnly(), lNULL);

	BOOST_CHECK_EQUAL (l4A.getPrevious(), &l2D);
	BOOST_CHECK_EQUAL (l4A.getNext(), &l7AD);
	BOOST_CHECK_EQUAL (l4A.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l4A.getFollowingArrivalForFineSteppingOnly(), &l7AD);
	BOOST_CHECK_EQUAL (l4A.getPreviousConnectionDeparture(), lNULL);
	BOOST_CHECK_EQUAL (l4A.getPreviousDepartureForFineSteppingOnly(), &l2D);

	BOOST_CHECK_EQUAL (l7AD.getPrevious(), &l4A);
	BOOST_CHECK_EQUAL (l7AD.getNext(), lNULL);
	BOOST_CHECK_EQUAL (l7AD.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l7AD.getFollowingArrivalForFineSteppingOnly(), lNULL);
	BOOST_CHECK_EQUAL (l7AD.getPreviousConnectionDeparture(), lNULL);
	BOOST_CHECK_EQUAL (l7AD.getPreviousDepartureForFineSteppingOnly(), &l2D);

	l.addEdge(l5D);

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

	l.addEdge(l6AD);

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

	l.addEdge(l1D);
	l.addEdge(l3AD);
	l.addEdge(l8A);

	edges = l.getEdges();
	BOOST_CHECK_EQUAL (edges[0], &l1D);
	BOOST_CHECK_EQUAL (edges[1], &l2D);
	BOOST_CHECK_EQUAL (edges[2], &l3AD);
	BOOST_CHECK_EQUAL (edges[3], &l4A);
	BOOST_CHECK_EQUAL (edges[4], &l5D);
	BOOST_CHECK_EQUAL (edges[5], &l6AD);
	BOOST_CHECK_EQUAL (edges[6], &l7AD);
	BOOST_CHECK_EQUAL (edges[7], &l8A);

	BOOST_CHECK_EQUAL (l1D.getPrevious(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getNext(), &l2D);
	BOOST_CHECK_EQUAL (l1D.getFollowingConnectionArrival(), &l6AD);
	BOOST_CHECK_EQUAL (l1D.getFollowingArrivalForFineSteppingOnly(), &l3AD);
	BOOST_CHECK_EQUAL (l1D.getPreviousConnectionDeparture(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getPreviousDepartureForFineSteppingOnly(), lNULL);

	BOOST_CHECK_EQUAL (l2D.getPrevious(), &l1D);
	BOOST_CHECK_EQUAL (l2D.getNext(), &l3AD);
	BOOST_CHECK_EQUAL (l2D.getFollowingConnectionArrival(), &l6AD);
	BOOST_CHECK_EQUAL (l2D.getFollowingArrivalForFineSteppingOnly(), &l3AD);
	BOOST_CHECK_EQUAL (l2D.getPreviousConnectionDeparture(), &l1D);
	BOOST_CHECK_EQUAL (l2D.getPreviousDepartureForFineSteppingOnly(), &l1D);

	BOOST_CHECK_EQUAL (l3AD.getPrevious(), &l2D);
	BOOST_CHECK_EQUAL (l3AD.getNext(), &l4A);
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


BOOST_AUTO_TEST_CASE (testDRTAreaLinking)
{
	JourneyPattern l;

	StopArea p1(UNKNOWN_VALUE, true);
	StopArea p2(UNKNOWN_VALUE, false);
	StopArea p3(UNKNOWN_VALUE, false);
	StopArea p4(UNKNOWN_VALUE, false);
	StopArea p5(UNKNOWN_VALUE, true);
	StopArea p6(UNKNOWN_VALUE, true);
	StopArea p7(UNKNOWN_VALUE, false);
	StopArea p8(UNKNOWN_VALUE, false);

	StopPoint s1(1, "s1", &p1);
	p1.addPhysicalStop(s1);
	StopPoint s2(2, "s2", &p2);
	p2.addPhysicalStop(s2);
	StopPoint s3(3, "s3", &p3);
	p3.addPhysicalStop(s3);
	StopPoint s41(41, "s41", &p4);
	p4.addPhysicalStop(s41);
	StopPoint s42(42, "s42", &p4);
	p4.addPhysicalStop(s42);
	StopPoint s5(5, "s5", &p5);
	p5.addPhysicalStop(s5);
	StopPoint s61(61, "s61", &p6);
	p6.addPhysicalStop(s61);
	StopPoint s62(62, "s62", &p6);
	p6.addPhysicalStop(s62);
	StopPoint s7(7, "s7", &p7);
	p7.addPhysicalStop(s7);
	StopPoint s8(8, "s8", &p8);
	p8.addPhysicalStop(s8);

	DRTArea::Stops ds1;
	ds1.insert(&p3);
	ds1.insert(&p4);
	ds1.insert(&p5);
	ds1.insert(&p6);
	DRTArea d1(0, "DRT1", ds1);

	BOOST_CHECK_EQUAL (d1.getStops().size(), 4);

	DRTArea::Stops ds2;
	ds2.insert(&p7);
	ds2.insert(&p8);
	DRTArea d2(0, "DRT2", ds2);

	DesignatedLinePhysicalStop l1D(0, &l, 0, true, false,0,&s1);
	DesignatedLinePhysicalStop l2D(0, &l, 1, true, false,0,&s2);
	LineArea ld1AD(0, &l, 2, true, true, 0,&d1, false);
	LineArea ld2DA(0, &l, 3, true, true,0,&d2, true);
	DesignatedLinePhysicalStop* lNULL(NULL);

	BOOST_CHECK_EQUAL (ld1AD.getSubEdges().size(), 12);
	const AreaGeneratedLineStop& ld1AD0(static_cast<const AreaGeneratedLineStop&>(*ld1AD.getSubEdges().at(0)));
	const AreaGeneratedLineStop& ld1AD1(static_cast<const AreaGeneratedLineStop&>(*ld1AD.getSubEdges().at(1)));
	const AreaGeneratedLineStop& ld1AD2(static_cast<const AreaGeneratedLineStop&>(*ld1AD.getSubEdges().at(2)));
	const AreaGeneratedLineStop& ld1AD3(static_cast<const AreaGeneratedLineStop&>(*ld1AD.getSubEdges().at(3)));

	const Edge* firstEdgeWithTransfer(NULL);
	BOOST_FOREACH(const Edge* edge, ld1AD.getSubEdges())
	{
		if(static_cast<const AreaGeneratedLineStop&>(*edge).getPhysicalStop()->getConnectionPlace()->getAllowedConnection())
		{
			firstEdgeWithTransfer = static_cast<const AreaGeneratedLineStop*>(edge);
			break;
		}
	}

	l.addEdge(l1D);

	BOOST_CHECK_EQUAL (l1D.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getFollowingArrivalForFineSteppingOnly(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getPreviousConnectionDeparture(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getPreviousDepartureForFineSteppingOnly(), lNULL);

	l.addEdge(l2D);

	BOOST_CHECK_EQUAL (l1D.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getFollowingArrivalForFineSteppingOnly(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getPreviousConnectionDeparture(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getPreviousDepartureForFineSteppingOnly(), lNULL);

	BOOST_CHECK_EQUAL (l2D.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l2D.getFollowingArrivalForFineSteppingOnly(), lNULL);
	BOOST_CHECK_EQUAL (l2D.getPreviousConnectionDeparture(), &l1D);
	BOOST_CHECK_EQUAL (l2D.getPreviousDepartureForFineSteppingOnly(), &l1D);

	l.addEdge(ld1AD);

	BOOST_CHECK_EQUAL (l1D.getPrevious(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getNext(), &l2D);
	BOOST_CHECK_EQUAL (l1D.getFollowingConnectionArrival(), firstEdgeWithTransfer);
	BOOST_CHECK_EQUAL (l1D.getFollowingArrivalForFineSteppingOnly(), ld1AD.getSubEdges().at(0));
	BOOST_CHECK_EQUAL (l1D.getPreviousConnectionDeparture(), lNULL);
	BOOST_CHECK_EQUAL (l1D.getPreviousDepartureForFineSteppingOnly(), lNULL);

	BOOST_CHECK_EQUAL (l2D.getPrevious(), &l1D);
	BOOST_CHECK_EQUAL (l2D.getNext(), ld1AD.getSubEdges().at(0));
	BOOST_CHECK_EQUAL (l2D.getFollowingConnectionArrival(), firstEdgeWithTransfer);
	BOOST_CHECK_EQUAL (l2D.getFollowingArrivalForFineSteppingOnly(), ld1AD.getSubEdges().at(0));
	BOOST_CHECK_EQUAL (l2D.getPreviousConnectionDeparture(), &l1D);
	BOOST_CHECK_EQUAL (l2D.getPreviousDepartureForFineSteppingOnly(), &l1D);

	BOOST_CHECK_EQUAL (ld1AD.getSubEdges().at(0)->getPrevious(), &l2D);
	BOOST_CHECK_EQUAL (ld1AD.getSubEdges().at(0)->getNext(), ld1AD.getSubEdges().at(1));
	BOOST_CHECK_EQUAL (ld1AD.getSubEdges().at(0)->getPreviousConnectionDeparture(), &l1D);
	BOOST_CHECK_EQUAL (ld1AD.getSubEdges().at(0)->getPreviousDepartureForFineSteppingOnly(), &l2D);

	l.addEdge(ld2DA);
}

