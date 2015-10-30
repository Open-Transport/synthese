
/** Edge unit test.
	@file EdgeTest.cpp

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

#include "DRTArea.hpp"
#include "JourneyPattern.hpp"
#include "LineStop.h"
#include "Path.h"
#include "Hub.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "GeographyModule.h"
#include "DRTArea.hpp"
#include "DesignatedLinePhysicalStop.hpp"
#include "AreaGeneratedLineStop.hpp"
#include "TestUtils.hpp"

#include "UtilConstants.h"

#include <iostream>

#include <boost/test/auto_unit_test.hpp>

using namespace std;
using namespace synthese::util;
using namespace synthese::pt;
using namespace synthese::geography;
using namespace synthese::graph;
using namespace synthese;

BOOST_AUTO_TEST_CASE (testEdgeOrderedLinking)
{
	GeographyModule::PreInit();
	{
		ScopedRegistrable<StopArea> scopedStopArea;
		Env env;

		JourneyPattern l;

		StopArea p1(0, true);
		StopArea p2(0, false);
		StopArea p3(0, false);
		StopArea p4(0, false);
		StopArea p5(0, true);
		StopArea p6(0, true);
		StopArea p7(0, false);
		StopArea p8(0, false);

		StopPoint s1(0, "s1", &p1);
		StopPoint s2(0, "s1", &p2);
		StopPoint s3(0, "s1", &p3);
		StopPoint s4(0, "s1", &p4);
		StopPoint s5(0, "s1", &p5);
		StopPoint s6(0, "s1", &p6);
		StopPoint s7(0, "s1", &p7);
		StopPoint s8(0, "s1", &p8);

		LineStop ls1D(0, &l, 0, true, false,0,s1);
		LineStop ls2D(0, &l, 1, true, false,0,s2);
		LineStop ls3AD(0, &l, 2, true, true, 0,s3);
		LineStop ls4A(0, &l, 3, false, true,0,s4);
		LineStop ls5D(0, &l, 4, true, false,0,s5);
		LineStop ls6AD(0, &l, 5, true, true,0,s6);
		LineStop ls7AD(0, &l, 6, true, true,0,s7);
		LineStop ls8A(0, &l, 7, false, true,0,s8);
		Edge* lNULL(NULL);

		ls1D.link(env, true);
		BOOST_CHECK_EQUAL(ls1D.getGeneratedLineStops().size(), 1ULL);
		const Edge& l1D(**ls1D.getGeneratedLineStops().begin());
		BOOST_CHECK_EQUAL(l1D.isDepartureAllowed(), true);
		BOOST_CHECK_EQUAL(l1D.isArrivalAllowed(), false);
		BOOST_CHECK(dynamic_cast<const DesignatedLinePhysicalStop*>(&l1D));
		const DesignatedLinePhysicalStop& d1D(dynamic_cast<const DesignatedLinePhysicalStop&>(l1D));
		BOOST_CHECK_EQUAL(d1D.getLineStop(), &ls1D);

		BOOST_CHECK_EQUAL(l1D.getPrevious(), lNULL);
		BOOST_CHECK_EQUAL(l1D.getNext(), lNULL);
		BOOST_CHECK_EQUAL(l1D.getFollowingConnectionArrival(), lNULL);
		BOOST_CHECK_EQUAL(l1D.getFollowingArrivalForFineSteppingOnly(), lNULL);
		BOOST_CHECK_EQUAL(l1D.getPreviousConnectionDeparture(), lNULL);
		BOOST_CHECK_EQUAL(l1D.getPreviousDepartureForFineSteppingOnly(), lNULL);
		BOOST_CHECK_EQUAL(s1.getDepartureEdges().size(), 1);
		if(!s1.getDepartureEdges().empty())
		{
			BOOST_CHECK_EQUAL(s1.getDepartureEdges().begin()->first, &l);
			BOOST_CHECK_EQUAL(s1.getDepartureEdges().begin()->second, &l1D);
		}
		BOOST_CHECK_EQUAL(s1.getArrivalEdges().size(), 0);
		
		ls2D.link(env, true);
		BOOST_CHECK_EQUAL(ls2D.getGeneratedLineStops().size(), 1);
		const Edge& l2D(**ls2D.getGeneratedLineStops().begin());

		BOOST_CHECK_EQUAL (l1D.getPrevious(), lNULL);
		BOOST_CHECK_EQUAL (l1D.getNext(), &l2D);
		BOOST_CHECK_EQUAL (l1D.getFollowingConnectionArrival(), lNULL);
		BOOST_CHECK_EQUAL (l1D.getFollowingArrivalForFineSteppingOnly(), lNULL);
		BOOST_CHECK_EQUAL (l1D.getPreviousConnectionDeparture(), lNULL);
		BOOST_CHECK_EQUAL (l1D.getPreviousDepartureForFineSteppingOnly(), lNULL);
		BOOST_CHECK_EQUAL(s1.getDepartureEdges().size(), 1);
		if(!s1.getDepartureEdges().empty())
		{
			BOOST_CHECK_EQUAL(s1.getDepartureEdges().begin()->first, &l);
			BOOST_CHECK_EQUAL(s1.getDepartureEdges().begin()->second, &l1D);
		}
		BOOST_CHECK_EQUAL(s1.getArrivalEdges().size(), 0);

		BOOST_CHECK_EQUAL (l2D.getPrevious(), &l1D);
		BOOST_CHECK_EQUAL (l2D.getNext(), lNULL);
		BOOST_CHECK_EQUAL (l2D.getFollowingConnectionArrival(), lNULL);
		BOOST_CHECK_EQUAL (l2D.getFollowingArrivalForFineSteppingOnly(), lNULL);
		BOOST_CHECK_EQUAL (l2D.getPreviousConnectionDeparture(), &l1D);
		BOOST_CHECK_EQUAL (l2D.getPreviousDepartureForFineSteppingOnly(), &l1D);
		BOOST_CHECK_EQUAL(s2.getDepartureEdges().size(), 1);
		if(!s2.getDepartureEdges().empty())
		{
			BOOST_CHECK_EQUAL(s2.getDepartureEdges().begin()->first, &l);
			BOOST_CHECK_EQUAL(s2.getDepartureEdges().begin()->second, &l2D);
		}
		BOOST_CHECK_EQUAL(s2.getArrivalEdges().size(), 0);

		ls3AD.link(env, true);
		BOOST_CHECK_EQUAL(ls3AD.getGeneratedLineStops().size(), 1);
		const Edge& l3AD(**ls3AD.getGeneratedLineStops().begin());

		BOOST_CHECK_EQUAL (l1D.getPrevious(), lNULL);
		BOOST_CHECK_EQUAL (l1D.getNext(), &l2D);
		BOOST_CHECK_EQUAL (l1D.getFollowingConnectionArrival(), lNULL);
		BOOST_CHECK_EQUAL (l1D.getFollowingArrivalForFineSteppingOnly(), &l3AD);
		BOOST_CHECK_EQUAL (l1D.getPreviousConnectionDeparture(), lNULL);
		BOOST_CHECK_EQUAL (l1D.getPreviousDepartureForFineSteppingOnly(), lNULL);
		BOOST_CHECK_EQUAL(s1.getDepartureEdges().size(), 1);
		if(!s1.getDepartureEdges().empty())
		{
			BOOST_CHECK_EQUAL(s1.getDepartureEdges().begin()->first, &l);
			BOOST_CHECK_EQUAL(s1.getDepartureEdges().begin()->second, &l1D);
		}
		BOOST_CHECK_EQUAL(s1.getArrivalEdges().size(), 0);

		BOOST_CHECK_EQUAL (l2D.getPrevious(), &l1D);
		BOOST_CHECK_EQUAL (l2D.getNext(), &l3AD);
		BOOST_CHECK_EQUAL (l2D.getFollowingConnectionArrival(), lNULL);
		BOOST_CHECK_EQUAL (l2D.getFollowingArrivalForFineSteppingOnly(), &l3AD);
		BOOST_CHECK_EQUAL (l2D.getPreviousConnectionDeparture(), &l1D);
		BOOST_CHECK_EQUAL (l2D.getPreviousDepartureForFineSteppingOnly(), &l1D);
		BOOST_CHECK_EQUAL(s2.getDepartureEdges().size(), 1);
		if(!s2.getDepartureEdges().empty())
		{
			BOOST_CHECK_EQUAL(s2.getDepartureEdges().begin()->first, &l);
			BOOST_CHECK_EQUAL(s2.getDepartureEdges().begin()->second, &l2D);
		}
		BOOST_CHECK_EQUAL(s2.getArrivalEdges().size(), 0);

		BOOST_CHECK_EQUAL (l3AD.getPrevious(), &l2D);
		BOOST_CHECK_EQUAL (l3AD.getNext(), lNULL);
		BOOST_CHECK_EQUAL (l3AD.getFollowingConnectionArrival(), lNULL);
		BOOST_CHECK_EQUAL (l3AD.getFollowingArrivalForFineSteppingOnly(), lNULL);
		BOOST_CHECK_EQUAL (l3AD.getPreviousConnectionDeparture(), &l1D);
		BOOST_CHECK_EQUAL (l3AD.getPreviousDepartureForFineSteppingOnly(), &l2D);
		BOOST_CHECK_EQUAL(s3.getDepartureEdges().size(), 1);
		if(!s3.getDepartureEdges().empty())
		{
			BOOST_CHECK_EQUAL(s3.getDepartureEdges().begin()->first, &l);
			BOOST_CHECK_EQUAL(s3.getDepartureEdges().begin()->second, &l3AD);
		}
		BOOST_CHECK_EQUAL(s3.getArrivalEdges().size(), 1);
		if(!s3.getArrivalEdges().empty())
		{
			BOOST_CHECK_EQUAL(s3.getArrivalEdges().begin()->first, &l);
			BOOST_CHECK_EQUAL(s3.getArrivalEdges().begin()->second, &l3AD);
		}

		ls4A.link(env, true);
		BOOST_CHECK_EQUAL(ls4A.getGeneratedLineStops().size(), 1);
		const Edge& l4A(**ls4A.getGeneratedLineStops().begin());

		BOOST_CHECK_EQUAL (l1D.getPrevious(), lNULL);
		BOOST_CHECK_EQUAL (l1D.getNext(), &l2D);
		BOOST_CHECK_EQUAL (l1D.getFollowingConnectionArrival(), lNULL);
		BOOST_CHECK_EQUAL (l1D.getFollowingArrivalForFineSteppingOnly(), &l3AD);
		BOOST_CHECK_EQUAL (l1D.getPreviousConnectionDeparture(), lNULL);
		BOOST_CHECK_EQUAL (l1D.getPreviousDepartureForFineSteppingOnly(), lNULL);
		if(!s1.getDepartureEdges().empty())
		{
			BOOST_CHECK_EQUAL(s1.getDepartureEdges().begin()->first, &l);
			BOOST_CHECK_EQUAL(s1.getDepartureEdges().begin()->second, &l1D);
		}
		BOOST_CHECK_EQUAL(s1.getArrivalEdges().size(), 0);

		BOOST_CHECK_EQUAL (l2D.getPrevious(), &l1D);
		BOOST_CHECK_EQUAL (l2D.getNext(), &l3AD);
		BOOST_CHECK_EQUAL (l2D.getFollowingConnectionArrival(), lNULL);
		BOOST_CHECK_EQUAL (l2D.getFollowingArrivalForFineSteppingOnly(), &l3AD);
		BOOST_CHECK_EQUAL (l2D.getPreviousConnectionDeparture(), &l1D);
		BOOST_CHECK_EQUAL (l2D.getPreviousDepartureForFineSteppingOnly(), &l1D);
		BOOST_CHECK_EQUAL(s2.getDepartureEdges().size(), 1);
		if(!s2.getDepartureEdges().empty())
		{
			BOOST_CHECK_EQUAL(s2.getDepartureEdges().begin()->first, &l);
			BOOST_CHECK_EQUAL(s2.getDepartureEdges().begin()->second, &l2D);
		}
		BOOST_CHECK_EQUAL(s2.getArrivalEdges().size(), 0);

		BOOST_CHECK_EQUAL (l3AD.getPrevious(), &l2D);
		BOOST_CHECK_EQUAL (l3AD.getNext(), &l4A);
		BOOST_CHECK_EQUAL (l3AD.getFollowingConnectionArrival(), lNULL);
		BOOST_CHECK_EQUAL (l3AD.getFollowingArrivalForFineSteppingOnly(), &l4A);
		BOOST_CHECK_EQUAL (l3AD.getPreviousConnectionDeparture(), &l1D);
		BOOST_CHECK_EQUAL (l3AD.getPreviousDepartureForFineSteppingOnly(), &l2D);
		BOOST_CHECK_EQUAL(s3.getDepartureEdges().size(), 1);
		if(!s3.getDepartureEdges().empty())
		{
			BOOST_CHECK_EQUAL(s3.getDepartureEdges().begin()->first, &l);
			BOOST_CHECK_EQUAL(s3.getDepartureEdges().begin()->second, &l3AD);
		}
		BOOST_CHECK_EQUAL(s3.getArrivalEdges().size(), 1);
		if(!s3.getArrivalEdges().empty())
		{
			BOOST_CHECK_EQUAL(s3.getArrivalEdges().begin()->first, &l);
			BOOST_CHECK_EQUAL(s3.getArrivalEdges().begin()->second, &l3AD);
		}

		BOOST_CHECK_EQUAL (l4A.getFollowingConnectionArrival(), lNULL);
		BOOST_CHECK_EQUAL (l4A.getFollowingArrivalForFineSteppingOnly(), lNULL);
		BOOST_CHECK_EQUAL (l4A.getPreviousConnectionDeparture(), &l1D);
		BOOST_CHECK_EQUAL (l4A.getPreviousDepartureForFineSteppingOnly(), &l3AD);
		BOOST_CHECK_EQUAL(s4.getDepartureEdges().size(), 0);
		BOOST_CHECK_EQUAL(s4.getArrivalEdges().size(), 1);
		if(!s4.getArrivalEdges().empty())
		{
			BOOST_CHECK_EQUAL(s4.getArrivalEdges().begin()->first, &l);
			BOOST_CHECK_EQUAL(s4.getArrivalEdges().begin()->second, &l4A);
		}

		ls5D.link(env, true);
		BOOST_CHECK_EQUAL(ls5D.getGeneratedLineStops().size(), 1);
		const Edge& l5D(**ls5D.getGeneratedLineStops().begin());

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

		ls6AD.link(env, true);
		BOOST_CHECK_EQUAL(ls6AD.getGeneratedLineStops().size(), 1);
		const Edge& l6AD(**ls6AD.getGeneratedLineStops().begin());

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

		ls7AD.link(env, true);
		BOOST_CHECK_EQUAL(ls7AD.getGeneratedLineStops().size(), 1);
		const Edge& l7AD(**ls7AD.getGeneratedLineStops().begin());

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

		ls8A.link(env, true);
		BOOST_CHECK_EQUAL(ls8A.getGeneratedLineStops().size(), 1);
		const Edge& l8A(**ls8A.getGeneratedLineStops().begin());

		const Path::Edges& edges = l.getEdges();

		BOOST_CHECK_EQUAL (edges[0], &l1D);
		BOOST_CHECK_EQUAL (edges[1], &l2D);
		BOOST_CHECK_EQUAL (edges[2], &l3AD);
		BOOST_CHECK_EQUAL (edges[3], &l4A);
		BOOST_CHECK_EQUAL (edges[4], &l5D);
		BOOST_CHECK_EQUAL (edges[5], &l6AD);
		BOOST_CHECK_EQUAL (edges[6], &l7AD);
		BOOST_CHECK_EQUAL (edges[7], &l8A);

		const JourneyPattern::LineStops& ls = l.getLineStops();
		JourneyPattern::LineStops::const_iterator itls(ls.begin());
		BOOST_CHECK_EQUAL (*itls, &ls1D);
		++itls;
		BOOST_CHECK_EQUAL (*itls, &ls2D);
		++itls;
		BOOST_CHECK_EQUAL (*itls, &ls3AD);
		++itls;
		BOOST_CHECK_EQUAL (*itls, &ls4A);
		++itls;
		BOOST_CHECK_EQUAL (*itls, &ls5D);
		++itls;
		BOOST_CHECK_EQUAL (*itls, &ls6AD);
		++itls;
		BOOST_CHECK_EQUAL (*itls, &ls7AD);
		++itls;
		BOOST_CHECK_EQUAL (*itls, &ls8A);
		++itls;
		BOOST_CHECK(itls == ls.end());

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
	{	// Removal test
		Env env;
		JourneyPattern l;

		StopArea p1(0, true);
		StopArea p2(0, false);
		StopArea p3(0, false);
		StopArea p4(0, false);
		StopArea p5(0, true);
		StopArea p6(0, true);
		StopArea p7(0, false);
		StopArea p8(0, false);

		StopPoint s1(0, "s1", &p1);
		StopPoint s2(0, "s1", &p2);
		StopPoint s3(0, "s1", &p3);
		StopPoint s4(0, "s1", &p4);
		StopPoint s5(0, "s1", &p5);
		StopPoint s6(0, "s1", &p6);
		StopPoint s7(0, "s1", &p7);
		StopPoint s8(0, "s1", &p8);

		LineStop ls1D(0, &l, 0, true, false,0,s1);
		LineStop ls2D(0, &l, 1, true, false,0,s2);
		LineStop ls3AD(0, &l, 2, true, true, 0,s3);
		LineStop ls4A(0, &l, 3, false, true,0,s4);
		LineStop ls5D(0, &l, 4, true, false,0,s5);
		LineStop ls6AD(0, &l, 5, true, true,0,s6);
		LineStop ls7A(0, &l, 6, false, true,0,s7);
		LineStop ls8A(0, &l, 7, false, true,0,s8);
		Edge* lNULL(NULL);

		ls1D.link(env, true);
		ls2D.link(env, true);
		ls3AD.link(env, true);
		ls4A.link(env, true);
		ls5D.link(env, true);
		ls6AD.link(env, true);
		ls7A.link(env, true);
		ls8A.link(env, true);

		const Edge& l1D(**ls1D.getGeneratedLineStops().begin());
		const Edge& l2D(**ls2D.getGeneratedLineStops().begin());
		const Edge& l3AD(**ls3AD.getGeneratedLineStops().begin());
		const Edge& l4A(**ls4A.getGeneratedLineStops().begin());
		const Edge& l5D(**ls5D.getGeneratedLineStops().begin());
		const Edge& l6AD(**ls6AD.getGeneratedLineStops().begin());
		const Edge& l7A(**ls7A.getGeneratedLineStops().begin());
		const Edge& l8A(**ls8A.getGeneratedLineStops().begin());

		{
			const JourneyPattern::Edges& edges = l.getEdges();

			BOOST_CHECK_EQUAL (edges[0], &l1D);
			BOOST_CHECK_EQUAL (edges[1], &l2D);
			BOOST_CHECK_EQUAL (edges[2], &l3AD);
			BOOST_CHECK_EQUAL (edges[3], &l4A);
			BOOST_CHECK_EQUAL (edges[4], &l5D);
			BOOST_CHECK_EQUAL (edges[5], &l6AD);
			BOOST_CHECK_EQUAL (edges[6], &l7A);
			BOOST_CHECK_EQUAL (edges[7], &l8A);
		}

		BOOST_CHECK_EQUAL (l1D.getFollowingConnectionArrival(), &l6AD);
		BOOST_CHECK_EQUAL (l1D.getFollowingArrivalForFineSteppingOnly(), &l3AD);
		BOOST_CHECK_EQUAL (l1D.getPreviousConnectionDeparture(), lNULL);
		BOOST_CHECK_EQUAL (l1D.getPreviousDepartureForFineSteppingOnly(), lNULL);
		BOOST_CHECK_EQUAL (l1D.getPrevious(), lNULL);
		BOOST_CHECK_EQUAL (l1D.getNext(), &l2D);

		BOOST_CHECK_EQUAL (l2D.getFollowingConnectionArrival(), &l6AD);
		BOOST_CHECK_EQUAL (l2D.getFollowingArrivalForFineSteppingOnly(), &l3AD);
		BOOST_CHECK_EQUAL (l2D.getPreviousConnectionDeparture(), &l1D);
		BOOST_CHECK_EQUAL (l2D.getPreviousDepartureForFineSteppingOnly(), &l1D);
		BOOST_CHECK_EQUAL (l2D.getPrevious(), &l1D);
		BOOST_CHECK_EQUAL (l2D.getNext(), &l3AD);

		BOOST_CHECK_EQUAL (l3AD.getFollowingConnectionArrival(), &l6AD);
		BOOST_CHECK_EQUAL (l3AD.getFollowingArrivalForFineSteppingOnly(), &l4A);
		BOOST_CHECK_EQUAL (l3AD.getPreviousConnectionDeparture(), &l1D);
		BOOST_CHECK_EQUAL (l3AD.getPreviousDepartureForFineSteppingOnly(), &l2D);
		BOOST_CHECK_EQUAL (l3AD.getPrevious(), &l2D);
		BOOST_CHECK_EQUAL (l3AD.getNext(), &l4A);

		BOOST_CHECK_EQUAL (l4A.getFollowingConnectionArrival(), &l6AD);
		BOOST_CHECK_EQUAL (l4A.getFollowingArrivalForFineSteppingOnly(), &l6AD);
		BOOST_CHECK_EQUAL (l4A.getPreviousConnectionDeparture(), &l1D);
		BOOST_CHECK_EQUAL (l4A.getPreviousDepartureForFineSteppingOnly(), &l3AD);
		BOOST_CHECK_EQUAL (l4A.getPrevious(), &l3AD);
		BOOST_CHECK_EQUAL (l4A.getNext(), &l5D);

		BOOST_CHECK_EQUAL (l5D.getFollowingConnectionArrival(), &l6AD);
		BOOST_CHECK_EQUAL (l5D.getFollowingArrivalForFineSteppingOnly(), &l6AD);
		BOOST_CHECK_EQUAL (l5D.getPreviousConnectionDeparture(), &l1D);
		BOOST_CHECK_EQUAL (l5D.getPreviousDepartureForFineSteppingOnly(), &l3AD);
		BOOST_CHECK_EQUAL (l5D.getPrevious(), &l4A);
		BOOST_CHECK_EQUAL (l5D.getNext(), &l6AD);

		BOOST_CHECK_EQUAL (l6AD.getFollowingConnectionArrival(), lNULL);
		BOOST_CHECK_EQUAL (l6AD.getFollowingArrivalForFineSteppingOnly(), &l7A);
		BOOST_CHECK_EQUAL (l6AD.getPreviousConnectionDeparture(), &l5D);
		BOOST_CHECK_EQUAL (l6AD.getPreviousDepartureForFineSteppingOnly(), &l5D);
		BOOST_CHECK_EQUAL (l6AD.getPrevious(), &l5D);
		BOOST_CHECK_EQUAL (l6AD.getNext(), &l7A);

		BOOST_CHECK_EQUAL (l7A.getFollowingConnectionArrival(), lNULL);
		BOOST_CHECK_EQUAL (l7A.getFollowingArrivalForFineSteppingOnly(), &l8A);
		BOOST_CHECK_EQUAL (l7A.getPreviousConnectionDeparture(), &l6AD);
		BOOST_CHECK_EQUAL (l7A.getPreviousDepartureForFineSteppingOnly(), &l6AD);
		BOOST_CHECK_EQUAL (l7A.getPrevious(), &l6AD);
		BOOST_CHECK_EQUAL (l7A.getNext(), &l8A);

		BOOST_CHECK_EQUAL (l8A.getFollowingConnectionArrival(), lNULL);
		BOOST_CHECK_EQUAL (l8A.getFollowingArrivalForFineSteppingOnly(), lNULL);
		BOOST_CHECK_EQUAL (l8A.getPreviousConnectionDeparture(), &l6AD);
		BOOST_CHECK_EQUAL (l8A.getPreviousDepartureForFineSteppingOnly(), &l6AD);
		BOOST_CHECK_EQUAL (l8A.getPrevious(), &l7A);
		BOOST_CHECK_EQUAL (l8A.getNext(), lNULL);

		ls7A.unlink();
		BOOST_CHECK_EQUAL(ls7A.getGeneratedLineStops().size(), 0);
		BOOST_CHECK_EQUAL(s7.getArrivalEdges().size(), 0);
		BOOST_CHECK_EQUAL(s7.getDepartureEdges().size(), 0);

		{
			const JourneyPattern::Edges& edges = l.getEdges();

			BOOST_CHECK_EQUAL (edges[0], &l1D);
			BOOST_CHECK_EQUAL (edges[1], &l2D);
			BOOST_CHECK_EQUAL (edges[2], &l3AD);
			BOOST_CHECK_EQUAL (edges[3], &l4A);
			BOOST_CHECK_EQUAL (edges[4], &l5D);
			BOOST_CHECK_EQUAL (edges[5], &l6AD);
			BOOST_CHECK_EQUAL (edges[6], &l8A);
		}

		BOOST_CHECK_EQUAL (l1D.getFollowingConnectionArrival(), &l6AD);
		BOOST_CHECK_EQUAL (l1D.getFollowingArrivalForFineSteppingOnly(), &l3AD);
		BOOST_CHECK_EQUAL (l1D.getPreviousConnectionDeparture(), lNULL);
		BOOST_CHECK_EQUAL (l1D.getPreviousDepartureForFineSteppingOnly(), lNULL);
		BOOST_CHECK_EQUAL (l1D.getPrevious(), lNULL);
		BOOST_CHECK_EQUAL (l1D.getNext(), &l2D);

		BOOST_CHECK_EQUAL (l2D.getFollowingConnectionArrival(), &l6AD);
		BOOST_CHECK_EQUAL (l2D.getFollowingArrivalForFineSteppingOnly(), &l3AD);
		BOOST_CHECK_EQUAL (l2D.getPreviousConnectionDeparture(), &l1D);
		BOOST_CHECK_EQUAL (l2D.getPreviousDepartureForFineSteppingOnly(), &l1D);
		BOOST_CHECK_EQUAL (l2D.getPrevious(), &l1D);
		BOOST_CHECK_EQUAL (l2D.getNext(), &l3AD);

		BOOST_CHECK_EQUAL (l3AD.getFollowingConnectionArrival(), &l6AD);
		BOOST_CHECK_EQUAL (l3AD.getFollowingArrivalForFineSteppingOnly(), &l4A);
		BOOST_CHECK_EQUAL (l3AD.getPreviousConnectionDeparture(), &l1D);
		BOOST_CHECK_EQUAL (l3AD.getPreviousDepartureForFineSteppingOnly(), &l2D);
		BOOST_CHECK_EQUAL (l3AD.getPrevious(), &l2D);
		BOOST_CHECK_EQUAL (l3AD.getNext(), &l4A);

		BOOST_CHECK_EQUAL (l4A.getFollowingConnectionArrival(), &l6AD);
		BOOST_CHECK_EQUAL (l4A.getFollowingArrivalForFineSteppingOnly(), &l6AD);
		BOOST_CHECK_EQUAL (l4A.getPreviousConnectionDeparture(), &l1D);
		BOOST_CHECK_EQUAL (l4A.getPreviousDepartureForFineSteppingOnly(), &l3AD);
		BOOST_CHECK_EQUAL (l4A.getPrevious(), &l3AD);
		BOOST_CHECK_EQUAL (l4A.getNext(), &l5D);

		BOOST_CHECK_EQUAL (l5D.getFollowingConnectionArrival(), &l6AD);
		BOOST_CHECK_EQUAL (l5D.getFollowingArrivalForFineSteppingOnly(), &l6AD);
		BOOST_CHECK_EQUAL (l5D.getPreviousConnectionDeparture(), &l1D);
		BOOST_CHECK_EQUAL (l5D.getPreviousDepartureForFineSteppingOnly(), &l3AD);
		BOOST_CHECK_EQUAL (l5D.getPrevious(), &l4A);
		BOOST_CHECK_EQUAL (l5D.getNext(), &l6AD);

		BOOST_CHECK_EQUAL (l6AD.getFollowingConnectionArrival(), lNULL);
		BOOST_CHECK_EQUAL (l6AD.getFollowingArrivalForFineSteppingOnly(), &l8A);
		BOOST_CHECK_EQUAL (l6AD.getPreviousConnectionDeparture(), &l5D);
		BOOST_CHECK_EQUAL (l6AD.getPreviousDepartureForFineSteppingOnly(), &l5D);
		BOOST_CHECK_EQUAL (l6AD.getPrevious(), &l5D);
		BOOST_CHECK_EQUAL (l6AD.getNext(), &l8A);

		BOOST_CHECK_EQUAL (l8A.getFollowingConnectionArrival(), lNULL);
		BOOST_CHECK_EQUAL (l8A.getFollowingArrivalForFineSteppingOnly(), lNULL);
		BOOST_CHECK_EQUAL (l8A.getPreviousConnectionDeparture(), &l6AD);
		BOOST_CHECK_EQUAL (l8A.getPreviousDepartureForFineSteppingOnly(), &l6AD);
		BOOST_CHECK_EQUAL (l8A.getPrevious(), &l6AD);
		BOOST_CHECK_EQUAL (l8A.getNext(), lNULL);

		ls2D.unlink();

		{
			JourneyPattern::Edges edges = l.getEdges();

			BOOST_CHECK_EQUAL (edges[0], &l1D);
			BOOST_CHECK_EQUAL (edges[1], &l3AD);
			BOOST_CHECK_EQUAL (edges[2], &l4A);
			BOOST_CHECK_EQUAL (edges[3], &l5D);
			BOOST_CHECK_EQUAL (edges[4], &l6AD);
			BOOST_CHECK_EQUAL (edges[5], &l8A);
		}

		BOOST_CHECK_EQUAL (l1D.getFollowingConnectionArrival(), &l6AD);
		BOOST_CHECK_EQUAL (l1D.getFollowingArrivalForFineSteppingOnly(), &l3AD);
		BOOST_CHECK_EQUAL (l1D.getPreviousConnectionDeparture(), lNULL);
		BOOST_CHECK_EQUAL (l1D.getPreviousDepartureForFineSteppingOnly(), lNULL);
		BOOST_CHECK_EQUAL (l1D.getPrevious(), lNULL);
		BOOST_CHECK_EQUAL (l1D.getNext(), &l3AD);

		BOOST_CHECK_EQUAL (l3AD.getFollowingConnectionArrival(), &l6AD);
		BOOST_CHECK_EQUAL (l3AD.getFollowingArrivalForFineSteppingOnly(), &l4A);
		BOOST_CHECK_EQUAL (l3AD.getPreviousConnectionDeparture(), &l1D);
		BOOST_CHECK_EQUAL (l3AD.getPreviousDepartureForFineSteppingOnly(), &l1D);
		BOOST_CHECK_EQUAL (l3AD.getPrevious(), &l1D);
		BOOST_CHECK_EQUAL (l3AD.getNext(), &l4A);

		BOOST_CHECK_EQUAL (l4A.getFollowingConnectionArrival(), &l6AD);
		BOOST_CHECK_EQUAL (l4A.getFollowingArrivalForFineSteppingOnly(), &l6AD);
		BOOST_CHECK_EQUAL (l4A.getPreviousConnectionDeparture(), &l1D);
		BOOST_CHECK_EQUAL (l4A.getPreviousDepartureForFineSteppingOnly(), &l3AD);
		BOOST_CHECK_EQUAL (l4A.getPrevious(), &l3AD);
		BOOST_CHECK_EQUAL (l4A.getNext(), &l5D);

		BOOST_CHECK_EQUAL (l5D.getFollowingConnectionArrival(), &l6AD);
		BOOST_CHECK_EQUAL (l5D.getFollowingArrivalForFineSteppingOnly(), &l6AD);
		BOOST_CHECK_EQUAL (l5D.getPreviousConnectionDeparture(), &l1D);
		BOOST_CHECK_EQUAL (l5D.getPreviousDepartureForFineSteppingOnly(), &l3AD);
		BOOST_CHECK_EQUAL (l5D.getPrevious(), &l4A);
		BOOST_CHECK_EQUAL (l5D.getNext(), &l6AD);

		BOOST_CHECK_EQUAL (l6AD.getFollowingConnectionArrival(), lNULL);
		BOOST_CHECK_EQUAL (l6AD.getFollowingArrivalForFineSteppingOnly(), &l8A);
		BOOST_CHECK_EQUAL (l6AD.getPreviousConnectionDeparture(), &l5D);
		BOOST_CHECK_EQUAL (l6AD.getPreviousDepartureForFineSteppingOnly(), &l5D);
		BOOST_CHECK_EQUAL (l6AD.getPrevious(), &l5D);
		BOOST_CHECK_EQUAL (l6AD.getNext(), &l8A);

		BOOST_CHECK_EQUAL (l8A.getFollowingConnectionArrival(), lNULL);
		BOOST_CHECK_EQUAL (l8A.getFollowingArrivalForFineSteppingOnly(), lNULL);
		BOOST_CHECK_EQUAL (l8A.getPreviousConnectionDeparture(), &l6AD);
		BOOST_CHECK_EQUAL (l8A.getPreviousDepartureForFineSteppingOnly(), &l6AD);
		BOOST_CHECK_EQUAL (l8A.getPrevious(), &l6AD);
		BOOST_CHECK_EQUAL (l8A.getNext(), lNULL);

	}
}



BOOST_AUTO_TEST_CASE (testEdgeRandomLinking)
{
	Env env;
	JourneyPattern l;

	StopArea p1(0, true);
	StopArea p2(0, false);
	StopArea p3(0, false);
	StopArea p4(0, false);
	StopArea p5(0, true);
	StopArea p6(0, true);
	StopArea p7(0, false);
	StopArea p8(0, false);

	StopPoint s1(0, "s1", &p1);
	StopPoint s2(0, "s1", &p2);
	StopPoint s3(0, "s1", &p3);
	StopPoint s4(0, "s1", &p4);
	StopPoint s5(0, "s1", &p5);
	StopPoint s6(0, "s1", &p6);
	StopPoint s7(0, "s1", &p7);
	StopPoint s8(0, "s1", &p8);

	LineStop ls1D(0, &l, 0, true, false,0,s1);
	LineStop ls2D(0, &l, 1, true, false,0,s2);
	LineStop ls3AD(0, &l, 2, true, true, 0,s3);
	LineStop ls4A(0, &l, 3, false, true,0,s4);
	LineStop ls5D(0, &l, 4, true, false,0,s5);
	LineStop ls6AD(0, &l, 5, true, true,0,s6);
	LineStop ls7AD(0, &l, 6, true, true,0,s7);
	LineStop ls8A(0, &l, 7, false, true,0,s8);
	Edge* lNULL(NULL);

	ls4A.link(env, true);
	const Edge& l4A(**ls4A.getGeneratedLineStops().begin());

	JourneyPattern::Edges edges = l.getEdges();
	BOOST_CHECK_EQUAL (edges[0], &l4A);

	BOOST_CHECK_EQUAL (l4A.getPrevious(), lNULL);
	BOOST_CHECK_EQUAL (l4A.getNext(), lNULL);
	BOOST_CHECK_EQUAL (l4A.getFollowingConnectionArrival(), lNULL);
	BOOST_CHECK_EQUAL (l4A.getFollowingArrivalForFineSteppingOnly(), lNULL);
	BOOST_CHECK_EQUAL (l4A.getPreviousConnectionDeparture(), lNULL);
	BOOST_CHECK_EQUAL (l4A.getPreviousDepartureForFineSteppingOnly(), lNULL);

	ls2D.link(env, true);
	const Edge& l2D(**ls2D.getGeneratedLineStops().begin());

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

	ls7AD.link(env, true);
	const Edge& l7AD(**ls7AD.getGeneratedLineStops().begin());

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

	ls5D.link(env, true);
	const Edge& l5D(**ls5D.getGeneratedLineStops().begin());

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

	ls6AD.link(env, true);
	const Edge& l6AD(**ls6AD.getGeneratedLineStops().begin());

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

	ls1D.link(env, true);
	ls3AD.link(env, true);
	ls8A.link(env, true);
	const Edge& l1D(**ls1D.getGeneratedLineStops().begin());
	const Edge& l3AD(**ls3AD.getGeneratedLineStops().begin());
	const Edge& l8A(**ls8A.getGeneratedLineStops().begin());

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


BOOST_AUTO_TEST_CASE (testStopPointDelete)
{
	Env env;

	JourneyPattern l;

	StopArea p1(0, true);
	StopArea p2(0, false);
	StopArea p3(0, false);
	StopArea p4(0, false);
	StopArea p5(0, true);
	StopArea p6(0, true);
	StopArea p7(0, false);
	StopArea p8(0, false);

	boost::shared_ptr<StopPoint> s1(new StopPoint(0, "s1", &p1));
	boost::shared_ptr<StopPoint> s2(new StopPoint(0, "s1", &p2));
	boost::shared_ptr<StopPoint> s3(new StopPoint(0, "s1", &p3));
	StopPoint s4(0, "s1", &p4);
	StopPoint s5(0, "s1", &p5);
	StopPoint s6(0, "s1", &p6);
	StopPoint s7(0, "s1", &p7);
	StopPoint s8(0, "s1", &p8);

	LineStop ls1D(0, &l, 0, true, false,0,*s1);
	LineStop ls2D(0, &l, 1, true, false,0,*s2);
	LineStop ls3AD(0, &l, 2, true, true, 0,*s3);
	LineStop ls4A(0, &l, 3, false, true,0,s4);
	LineStop ls5D(0, &l, 4, true, false,0,s5);
	LineStop ls6AD(0, &l, 5, true, true,0,s6);
	LineStop ls7AD(0, &l, 6, true, true,0,s7);
	LineStop ls8A(0, &l, 7, false, true,0,s8);
	Edge* lNULL(NULL);

	ls4A.link(env, true);
	ls2D.link(env, true);
	ls7AD.link(env, true);
	ls5D.link(env, true);
	ls6AD.link(env, true);
	ls1D.link(env, true);
	ls3AD.link(env, true);
	ls8A.link(env, true);
	
	const Edge& l1D(**ls1D.getGeneratedLineStops().begin());
	const Edge& l2D(**ls2D.getGeneratedLineStops().begin());
	const Edge& l3AD(**ls3AD.getGeneratedLineStops().begin());
	const Edge& l4A(**ls4A.getGeneratedLineStops().begin());
	const Edge& l5D(**ls5D.getGeneratedLineStops().begin());
	const Edge& l6AD(**ls6AD.getGeneratedLineStops().begin());
	const Edge& l7AD(**ls7AD.getGeneratedLineStops().begin());
	const Edge& l8A(**ls8A.getGeneratedLineStops().begin());

	s1.reset();

	BOOST_CHECK(l1D.getFromVertex() == NULL);

	s2.reset();

	BOOST_CHECK(l2D.getFromVertex() == NULL);

	s3.reset();

	BOOST_CHECK(l3AD.getFromVertex() == NULL);

}

BOOST_AUTO_TEST_CASE (testDRTAreaLinking)
{
	{
		Env env;

		JourneyPattern l;

		StopArea p1(0, true);
		StopArea p2(0, false);
		StopArea p3(0, false);
		StopArea p4(0, false);
		StopArea p5(0, true);
		StopArea p6(0, true);
		StopArea p7(0, false);
		StopArea p8(0, false);

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

		Stops::Type ds1;
		ds1.push_back(&p3);
		ds1.push_back(&p4);
		ds1.push_back(&p5);
		ds1.push_back(&p6);
		DRTArea d1(0, "DRT1", ds1);

		BOOST_CHECK_EQUAL (d1.get<Stops>().size(), 4);

		Stops::Type ds2;
		ds2.push_back(&p7);
		ds2.push_back(&p8);
		DRTArea d2(0, "DRT2", ds2);

		LineStop ls1D(0, &l, 0, true, false,0,s1);
		LineStop ls2D(0, &l, 1, true, false,0,s2);
		LineStop lsd1AD(0, &l, 2, true, true, 0,d1);
		lsd1AD.set<InternalService>(false);
		LineStop lsd2DA(0, &l, 3, true, true,0,d2);
		lsd2DA.set<InternalService>(true);
		Edge* lNULL(NULL);

		ls1D.link(env, true);
		const Edge& l1D(**ls1D.getGeneratedLineStops().begin());

		BOOST_CHECK_EQUAL (l1D.getFollowingConnectionArrival(), lNULL);
		BOOST_CHECK_EQUAL (l1D.getFollowingArrivalForFineSteppingOnly(), lNULL);
		BOOST_CHECK_EQUAL (l1D.getPreviousConnectionDeparture(), lNULL);
		BOOST_CHECK_EQUAL (l1D.getPreviousDepartureForFineSteppingOnly(), lNULL);
		{
			pair<Vertex::Edges::const_iterator, Vertex::Edges::const_iterator> it(s1.getDepartureEdges().equal_range(&l));
			BOOST_CHECK_EQUAL(s1.getDepartureEdges().count(&l), 1);
			if(s1.getDepartureEdges().count(&l) == 1)
			{
				BOOST_CHECK_EQUAL(s1.getDepartureEdges().lower_bound(&l)->second, &l1D);
			}
			BOOST_CHECK_EQUAL(s1.getArrivalEdges().count(&l), 0);
		}


		ls2D.link(env, true);
		const Edge& l2D(**ls2D.getGeneratedLineStops().begin());
	
		BOOST_CHECK_EQUAL (l1D.getFollowingConnectionArrival(), lNULL);
		BOOST_CHECK_EQUAL (l1D.getFollowingArrivalForFineSteppingOnly(), lNULL);
		BOOST_CHECK_EQUAL (l1D.getPreviousConnectionDeparture(), lNULL);
		BOOST_CHECK_EQUAL (l1D.getPreviousDepartureForFineSteppingOnly(), lNULL);

		BOOST_CHECK_EQUAL (l2D.getFollowingConnectionArrival(), lNULL);
		BOOST_CHECK_EQUAL (l2D.getFollowingArrivalForFineSteppingOnly(), lNULL);
		BOOST_CHECK_EQUAL (l2D.getPreviousConnectionDeparture(), &l1D);
		BOOST_CHECK_EQUAL (l2D.getPreviousDepartureForFineSteppingOnly(), &l1D);

		lsd1AD.link(env);
		
		const Edge* firstEdgeWithTransfer(NULL);
		BOOST_FOREACH(boost::shared_ptr<LinePhysicalStop> edge, lsd1AD.getGeneratedLineStops())
		{
			BOOST_CHECK(dynamic_cast<const AreaGeneratedLineStop*>(edge.get()));
			if(	static_cast<const AreaGeneratedLineStop&>(*edge).getPhysicalStop()->getConnectionPlace()->getAllowedConnection() &&
				edge->isArrivalAllowed()
			){
				firstEdgeWithTransfer = edge.get();
				break;
			}
		}


		BOOST_CHECK_EQUAL (l1D.getPrevious(), lNULL);
		BOOST_CHECK_EQUAL (l1D.getNext(), &l2D);
		BOOST_CHECK_EQUAL (l1D.getFollowingConnectionArrival(), firstEdgeWithTransfer);
		BOOST_CHECK_EQUAL (l1D.getFollowingArrivalForFineSteppingOnly(), lsd1AD.getGeneratedLineStops().at(0).get());
		BOOST_CHECK_EQUAL (l1D.getPreviousConnectionDeparture(), lNULL);
		BOOST_CHECK_EQUAL (l1D.getPreviousDepartureForFineSteppingOnly(), lNULL);

		BOOST_CHECK_EQUAL (l2D.getPrevious(), &l1D);
		BOOST_CHECK_EQUAL (l2D.getNext(), lsd1AD.getGeneratedLineStops().at(0).get());
		BOOST_CHECK_EQUAL (l2D.getFollowingConnectionArrival(), firstEdgeWithTransfer);
		BOOST_CHECK_EQUAL (l2D.getFollowingArrivalForFineSteppingOnly(), lsd1AD.getGeneratedLineStops().at(0).get());
		BOOST_CHECK_EQUAL (l2D.getPreviousConnectionDeparture(), &l1D);
		BOOST_CHECK_EQUAL (l2D.getPreviousDepartureForFineSteppingOnly(), &l1D);

		BOOST_CHECK_EQUAL (lsd1AD.getGeneratedLineStops().at(0)->getPrevious(), &l2D);
		BOOST_CHECK_EQUAL (lsd1AD.getGeneratedLineStops().at(0)->getNext(), lsd1AD.getGeneratedLineStops().at(1).get());
		BOOST_CHECK_EQUAL (lsd1AD.getGeneratedLineStops().at(0)->getPreviousConnectionDeparture(), &l1D);
		BOOST_CHECK_EQUAL (lsd1AD.getGeneratedLineStops().at(0)->getPreviousDepartureForFineSteppingOnly(), &l2D);

		BOOST_CHECK_EQUAL (lsd1AD.getGeneratedLineStops().size(), 12);
		const AreaGeneratedLineStop& ld1AD0(static_cast<const AreaGeneratedLineStop&>(*lsd1AD.getGeneratedLineStops().at(0)));
		const AreaGeneratedLineStop& ld1AD1(static_cast<const AreaGeneratedLineStop&>(*lsd1AD.getGeneratedLineStops().at(1)));
		const AreaGeneratedLineStop& ld1AD2(static_cast<const AreaGeneratedLineStop&>(*lsd1AD.getGeneratedLineStops().at(2)));
		const AreaGeneratedLineStop& ld1AD3(static_cast<const AreaGeneratedLineStop&>(*lsd1AD.getGeneratedLineStops().at(3)));

		{
			pair<Vertex::Edges::const_iterator, Vertex::Edges::const_iterator> it(s1.getDepartureEdges().equal_range(&l));
			BOOST_CHECK_EQUAL(s1.getDepartureEdges().count(&l), 1);
			if(s1.getDepartureEdges().count(&l) == 1)
			{
				BOOST_CHECK_EQUAL(s1.getDepartureEdges().lower_bound(&l)->second, &l1D);
			}
			BOOST_CHECK_EQUAL(s1.getArrivalEdges().count(&l), 0);
		}

		{
			pair<Vertex::Edges::const_iterator, Vertex::Edges::const_iterator> it(s3.getDepartureEdges().equal_range(&l));
			BOOST_CHECK_EQUAL(s3.getDepartureEdges().count(&l), 1);
			if(s3.getDepartureEdges().count(&l) == 1)
			{
				BOOST_CHECK(dynamic_cast<const AreaGeneratedLineStop*>(s3.getDepartureEdges().lower_bound(&l)->second));
				if(dynamic_cast<const AreaGeneratedLineStop*>(s3.getDepartureEdges().lower_bound(&l)->second))
				{
					BOOST_CHECK_EQUAL(dynamic_cast<const AreaGeneratedLineStop*>(s3.getDepartureEdges().lower_bound(&l)->second)->getLineStop(), &lsd1AD);
				}
			}
			BOOST_CHECK_EQUAL(s3.getArrivalEdges().count(&l), 1);
			{
				BOOST_CHECK(dynamic_cast<const AreaGeneratedLineStop*>(s3.getDepartureEdges().lower_bound(&l)->second));
				if(dynamic_cast<const AreaGeneratedLineStop*>(s3.getArrivalEdges().lower_bound(&l)->second))
				{
					BOOST_CHECK_EQUAL(dynamic_cast<const AreaGeneratedLineStop*>(s3.getArrivalEdges().lower_bound(&l)->second)->getLineStop(), &lsd1AD);
				}
			}
		}

		lsd1AD.unlink();
		BOOST_CHECK_EQUAL(lsd1AD.getGeneratedLineStops().size(), 0);

		BOOST_CHECK_EQUAL(s3.getDepartureEdges().count(&l), 0);
		BOOST_CHECK_EQUAL(s3.getArrivalEdges().count(&l), 0);

		BOOST_CHECK_EQUAL (l1D.getFollowingConnectionArrival(), lNULL);
		BOOST_CHECK_EQUAL (l1D.getFollowingArrivalForFineSteppingOnly(), lNULL);
		BOOST_CHECK_EQUAL (l1D.getPreviousConnectionDeparture(), lNULL);
		BOOST_CHECK_EQUAL (l1D.getPreviousDepartureForFineSteppingOnly(), lNULL);

		BOOST_CHECK_EQUAL (l2D.getFollowingConnectionArrival(), lNULL);
		BOOST_CHECK_EQUAL (l2D.getFollowingArrivalForFineSteppingOnly(), lNULL);
		BOOST_CHECK_EQUAL (l2D.getPreviousConnectionDeparture(), &l1D);
		BOOST_CHECK_EQUAL (l2D.getPreviousDepartureForFineSteppingOnly(), &l1D);

		lsd2DA.link(env);
		lsd1AD.link(env);
	}

	// Area 
	{
		Env env;

		JourneyPattern l;

		StopArea p1(0, true);
		StopArea p2(0, false);
		StopArea p3(0, false);
		StopArea p4(0, false);
		StopArea p5(0, true);
		StopArea p6(0, true);
		StopArea p7(0, false);
		StopArea p8(0, false);

		StopPoint s1(1, "s1", &p1);
		p1.addPhysicalStop(s1);
		StopPoint s2(2, "s2", &p2);
		p2.addPhysicalStop(s2);
		boost::shared_ptr<StopPoint> s3(new StopPoint(3, "s3", &p3));
		p3.addPhysicalStop(*s3);
		boost::shared_ptr<StopPoint> s41(new StopPoint(41, "s41", &p4));
		p4.addPhysicalStop(*s41);
		boost::shared_ptr<StopPoint> s42(new StopPoint(42, "s42", &p4));
		p4.addPhysicalStop(*s42);
		boost::shared_ptr<StopPoint> s5(new StopPoint(5, "s5", &p5));
		p5.addPhysicalStop(*s5);
		StopPoint s61(61, "s61", &p6);
		p6.addPhysicalStop(s61);
		StopPoint s62(62, "s62", &p6);
		p6.addPhysicalStop(s62);
		StopPoint s7(7, "s7", &p7);
		p7.addPhysicalStop(s7);
		StopPoint s8(8, "s8", &p8);
		p8.addPhysicalStop(s8);


		LineStop ls1D(0, &l, 0, true, false,0,s1);
		LineStop ls2AD(0, &l, 1, true, true,0,s2);
		LineStop ls6A(0, &l, 3, false, true,0,s61);
		Edge* lNULL(NULL);

		ls1D.link(env);
		ls2AD.link(env);
		const Edge& l1D(**ls1D.getGeneratedLineStops().begin());
		const Edge& l2AD(**ls2AD.getGeneratedLineStops().begin());


		{
			DRTArea a345;
			Stops::Type stops;
			stops.push_back(&p3);
			stops.push_back(&p4);
			stops.push_back(&p5);
			a345.set<Stops>(stops);
			a345.link(env, true);

			BOOST_CHECK_EQUAL(p3.getDRTAreas().size(), 1);
			if(!p3.getDRTAreas().empty())
			{
				BOOST_CHECK_EQUAL(*p3.getDRTAreas().begin(), &a345);
			}
			BOOST_CHECK_EQUAL(p4.getDRTAreas().size(), 1);
			if(!p4.getDRTAreas().empty())
			{
				BOOST_CHECK_EQUAL(*p4.getDRTAreas().begin(), &a345);
			}
			BOOST_CHECK_EQUAL(p5.getDRTAreas().size(), 1);
			if(!p5.getDRTAreas().empty())
			{
				BOOST_CHECK_EQUAL(*p5.getDRTAreas().begin(), &a345);
			}

			LineStop ls345AD(0, &l, 2, true, true, 0, a345);
			ls345AD.link(env);

			ls6A.link(env);
			const Edge& l6A(**ls6A.getGeneratedLineStops().begin());

			BOOST_CHECK_EQUAL(ls345AD.getGeneratedLineStops().size(), 8);

			ls345AD.unlink();

			BOOST_CHECK_EQUAL (l2AD.getPrevious(), &l1D);
			BOOST_CHECK_EQUAL (l2AD.getNext(), &l6A);
			BOOST_CHECK_EQUAL (l2AD.getFollowingConnectionArrival(), &l6A);
			BOOST_CHECK_EQUAL (l2AD.getFollowingArrivalForFineSteppingOnly(), &l6A);
			BOOST_CHECK_EQUAL (l2AD.getPreviousConnectionDeparture(), &l1D);
			BOOST_CHECK_EQUAL (l2AD.getPreviousDepartureForFineSteppingOnly(), &l1D);

			BOOST_CHECK_EQUAL (l6A.getPrevious(), &l2AD);
			BOOST_CHECK_EQUAL (l6A.getNext(), lNULL);
			BOOST_CHECK_EQUAL (l6A.getFollowingConnectionArrival(), lNULL);
			BOOST_CHECK_EQUAL (l6A.getFollowingArrivalForFineSteppingOnly(), lNULL);
			BOOST_CHECK_EQUAL (l6A.getPreviousConnectionDeparture(), &l1D);
			BOOST_CHECK_EQUAL (l6A.getPreviousDepartureForFineSteppingOnly(), &l2AD);

		}

		BOOST_CHECK_EQUAL(p3.getDRTAreas().size(), 0);
		BOOST_CHECK_EQUAL(p4.getDRTAreas().size(), 0);
		BOOST_CHECK_EQUAL(p5.getDRTAreas().size(), 0);


		{
			DRTArea a345;
			Stops::Type stops;
			stops.push_back(&p3);
			stops.push_back(&p4);
			stops.push_back(&p5);
			a345.set<Stops>(stops);
			a345.link(env, true);

			BOOST_CHECK_EQUAL(p3.getDRTAreas().size(), 1);
			if(!p3.getDRTAreas().empty())
			{
				BOOST_CHECK_EQUAL(*p3.getDRTAreas().begin(), &a345);
			}
			BOOST_CHECK_EQUAL(p4.getDRTAreas().size(), 1);
			if(!p4.getDRTAreas().empty())
			{
				BOOST_CHECK_EQUAL(*p4.getDRTAreas().begin(), &a345);
			}
			BOOST_CHECK_EQUAL(p5.getDRTAreas().size(), 1);
			if(!p5.getDRTAreas().empty())
			{
				BOOST_CHECK_EQUAL(*p5.getDRTAreas().begin(), &a345);
			}

			LineStop ls345AD(0, &l, 2, true, true, 0, a345);
			ls345AD.link(env);

			BOOST_CHECK_EQUAL(ls345AD.getGeneratedLineStops().size(), 8);

			s3.reset();
			s41.reset();
			s42.reset();
			s5.reset();

			BOOST_FOREACH(boost::shared_ptr<LinePhysicalStop> edge, ls345AD.getGeneratedLineStops())
			{
				BOOST_CHECK(edge->getFromVertex() == NULL);
			}

			ls345AD.unlink();
		}
	}
}

