
/** MainRoadPart Test implementation.
	@file VertexAccessMapTest.cpp

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

#include "MainRoadPart.hpp"
#include "Crossing.h"
#include "RoadPlace.h"
#include "MainRoadChunk.hpp"
#include "ReverseRoadPart.hpp"

#include <boost/test/auto_unit_test.hpp>

using namespace synthese::road;
using namespace boost;


BOOST_AUTO_TEST_CASE (Edges)
{
	{ // Regular order insertion 0 1 2
		RoadPlace p;

		MainRoadPart r;
		r.setRoadPlace(p);

		BOOST_REQUIRE(r.getReverseRoad());
		BOOST_REQUIRE_EQUAL(r.getRoadPlace()->getPaths().size(), 2);

		Crossing cr1;
		MainRoadChunk c1(0, &cr1, 0, &r, 0);
		r.addRoadChunk(c1);

		BOOST_REQUIRE_EQUAL(r.getEdges().size(), 1);
		BOOST_CHECK_EQUAL(r.getEdges().at(0), &c1);
		BOOST_REQUIRE_EQUAL(r.getReverseRoad()->getEdges().size(), 1);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(0)->getFromVertex(), &cr1);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(0)->getMetricOffset(), 0);

		Crossing cr2;
		MainRoadChunk c2(0, &cr2, 1, &r, 10);
		r.addRoadChunk(c2);

		BOOST_REQUIRE_EQUAL(r.getEdges().size(), 2);
		BOOST_CHECK_EQUAL(r.getEdges().at(0), &c1);
		BOOST_CHECK_EQUAL(r.getEdges().at(1), &c2);
		BOOST_REQUIRE_EQUAL(r.getReverseRoad()->getEdges().size(), 2);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(0)->getFromVertex(), &cr2);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(0)->getRankInPath(), 0);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(0)->getMetricOffset(), -10);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(1)->getFromVertex(), &cr1);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(1)->getRankInPath(), 1);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(1)->getMetricOffset(), 0);

		Crossing cr3;
		MainRoadChunk c3(0, &cr3, 2, &r, 20);
		r.addRoadChunk(c3);

		BOOST_REQUIRE_EQUAL(r.getEdges().size(), 3);
		BOOST_CHECK_EQUAL(r.getEdges().at(0), &c1);
		BOOST_CHECK_EQUAL(r.getEdges().at(1), &c2);
		BOOST_CHECK_EQUAL(r.getEdges().at(2), &c3);
		BOOST_REQUIRE_EQUAL(r.getReverseRoad()->getEdges().size(), 3);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(0)->getFromVertex(), &cr3);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(0)->getRankInPath(), 0);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(0)->getMetricOffset(), -20);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(1)->getFromVertex(), &cr2);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(1)->getRankInPath(), 1);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(1)->getMetricOffset(), -10);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(2)->getFromVertex(), &cr1);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(2)->getRankInPath(), 2);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(2)->getMetricOffset(), 0);
	}

	{ // Reverse order insertion 2 1 0
		RoadPlace p;

		MainRoadPart r;
		r.setRoadPlace(p);

		BOOST_REQUIRE(r.getReverseRoad());
		BOOST_REQUIRE_EQUAL(r.getRoadPlace()->getPaths().size(), 2);

		Crossing cr2;
		MainRoadChunk c2(0, &cr2, 2, &r, 20);
		r.addRoadChunk(c2);

		BOOST_REQUIRE_EQUAL(r.getEdges().size(), 1);
		BOOST_CHECK_EQUAL(r.getEdges().at(0), &c2);
		BOOST_REQUIRE_EQUAL(r.getReverseRoad()->getEdges().size(), 1);
		BOOST_CHECK_EQUAL(r.getEdges().at(0)->getRankInPath(), 2);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(0)->getFromVertex(), &cr2);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(0)->getMetricOffset(), -20);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(0)->getRankInPath(), 0);

		Crossing cr1;
		MainRoadChunk c1(0, &cr1, 1, &r, 10);
		r.addRoadChunk(c1);

		BOOST_REQUIRE_EQUAL(r.getEdges().size(), 2);
		BOOST_CHECK_EQUAL(r.getEdges().at(0), &c1);
		BOOST_CHECK_EQUAL(r.getEdges().at(0)->getRankInPath(), 1);
		BOOST_CHECK_EQUAL(r.getEdges().at(1), &c2);
		BOOST_CHECK_EQUAL(r.getEdges().at(1)->getRankInPath(), 2);
		BOOST_REQUIRE_EQUAL(r.getReverseRoad()->getEdges().size(), 2);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(0)->getFromVertex(), &cr2);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(0)->getRankInPath(), 0);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(1)->getFromVertex(), &cr1);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(1)->getRankInPath(), 1);

		Crossing cr0;
		MainRoadChunk c0(0, &cr0, 0, &r, 0);
		r.addRoadChunk(c0);

		BOOST_REQUIRE_EQUAL(r.getEdges().size(), 3);
		BOOST_CHECK_EQUAL(r.getEdges().at(0), &c0);
		BOOST_CHECK_EQUAL(r.getEdges().at(1), &c1);
		BOOST_CHECK_EQUAL(r.getEdges().at(2), &c2);
		BOOST_REQUIRE_EQUAL(r.getReverseRoad()->getEdges().size(), 3);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(0)->getFromVertex(), &cr2);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(0)->getRankInPath(), 0);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(1)->getFromVertex(), &cr1);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(1)->getRankInPath(), 1);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(2)->getFromVertex(), &cr0);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(2)->getRankInPath(), 2);
	}

	{ // Random order insertion 2 0 1
		RoadPlace p;

		MainRoadPart r;
		r.setRoadPlace(p);

		Crossing cr1;
		MainRoadChunk c1(0, &cr1, 2, &r, 10);
		r.addRoadChunk(c1);

		Crossing cr2;
		MainRoadChunk c2(0, &cr2, 0, &r, 0);
		r.addRoadChunk(c2);

		BOOST_REQUIRE_EQUAL(r.getEdges().size(), 2);
		BOOST_CHECK_EQUAL(r.getEdges().at(0), &c2);
		BOOST_CHECK_EQUAL(r.getEdges().at(1), &c1);
		BOOST_REQUIRE_EQUAL(r.getReverseRoad()->getEdges().size(), 2);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(0)->getFromVertex(), &cr1);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(0)->getRankInPath(), 0);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(1)->getFromVertex(), &cr2);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(1)->getRankInPath(), 2);

		Crossing cr3;
		MainRoadChunk c3(0, &cr3, 1, &r, 5);
		r.addRoadChunk(c3);

		BOOST_REQUIRE_EQUAL(r.getEdges().size(), 3);
		BOOST_CHECK_EQUAL(r.getEdges().at(0), &c2);
		BOOST_CHECK_EQUAL(r.getEdges().at(1), &c3);
		BOOST_CHECK_EQUAL(r.getEdges().at(2), &c1);
		BOOST_REQUIRE_EQUAL(r.getReverseRoad()->getEdges().size(), 3);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(0)->getFromVertex(), &cr1);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(0)->getRankInPath(), 0);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(1)->getFromVertex(), &cr3);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(1)->getRankInPath(), 1);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(2)->getFromVertex(), &cr2);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(2)->getRankInPath(), 2);
	}

	{ // Insertions 0 1 2
		RoadPlace p;

		MainRoadPart r;
		r.setRoadPlace(p);

		Crossing cr2;
		MainRoadChunk c2(0, &cr2, 0, &r, 0);
		r.addRoadChunk(c2);

		BOOST_REQUIRE_EQUAL(r.getEdges().size(), 1);
		BOOST_CHECK_EQUAL(r.getEdges().at(0), &c2);
		BOOST_CHECK_EQUAL(r.getEdges().at(0)->getRankInPath(), 0);
		BOOST_CHECK_EQUAL(r.getEdges().at(0)->getMetricOffset(), 0);
		BOOST_REQUIRE_EQUAL(r.getReverseRoad()->getEdges().size(), 1);
		BOOST_CHECK_EQUAL(static_cast<MainRoadChunk*>(r.getReverseRoad()->getEdges().at(0))->getFromCrossing(), &cr2);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(0)->getMetricOffset(), 0);

		Crossing cr1;
		MainRoadChunk c1(0, &cr1, 0, &r, 0);
		r.insertRoadChunk(c1, 10, 1);

		BOOST_REQUIRE_EQUAL(r.getEdges().size(), 2);
		BOOST_CHECK_EQUAL(r.getEdges().at(0), &c1);
		BOOST_CHECK_EQUAL(r.getEdges().at(0)->getRankInPath(), 0);
		BOOST_CHECK_EQUAL(r.getEdges().at(0)->getMetricOffset(), 0);
		BOOST_CHECK_EQUAL(r.getEdges().at(1), &c2);
		BOOST_CHECK_EQUAL(r.getEdges().at(1)->getRankInPath(), 1);
		BOOST_CHECK_EQUAL(r.getEdges().at(1)->getMetricOffset(), 10);
		BOOST_REQUIRE_EQUAL(r.getReverseRoad()->getEdges().size(), 2);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(0)->getFromVertex(), &cr2);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(0)->getMetricOffset(), -10);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(0)->getRankInPath(), 0);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(1)->getFromVertex(), &cr1);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(1)->getRankInPath(), 1);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(1)->getMetricOffset(), 0);

		Crossing cr0;
		MainRoadChunk c0(0, &cr0, 0, &r, 0);
		r.insertRoadChunk(c0, 20, 1);

		BOOST_REQUIRE_EQUAL(r.getEdges().size(), 3);
		BOOST_CHECK_EQUAL(r.getEdges().at(0), &c0);
		BOOST_CHECK_EQUAL(r.getEdges().at(0)->getRankInPath(), 0);
		BOOST_CHECK_EQUAL(r.getEdges().at(0)->getMetricOffset(), 0);
		BOOST_CHECK_EQUAL(r.getEdges().at(1), &c1);
		BOOST_CHECK_EQUAL(r.getEdges().at(1)->getRankInPath(), 1);
		BOOST_CHECK_EQUAL(r.getEdges().at(1)->getMetricOffset(), 20);
		BOOST_CHECK_EQUAL(r.getEdges().at(2), &c2);
		BOOST_CHECK_EQUAL(r.getEdges().at(2)->getRankInPath(), 2);
		BOOST_CHECK_EQUAL(r.getEdges().at(2)->getMetricOffset(), 30);
		BOOST_REQUIRE_EQUAL(r.getReverseRoad()->getEdges().size(), 3);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(0)->getFromVertex(), &cr2);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(0)->getRankInPath(), 0);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(0)->getMetricOffset(), -30);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(1)->getFromVertex(), &cr1);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(1)->getRankInPath(), 1);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(1)->getMetricOffset(), -20);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(2)->getFromVertex(), &cr0);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(2)->getRankInPath(), 2);
		BOOST_CHECK_EQUAL(r.getReverseRoad()->getEdges().at(2)->getMetricOffset(), 0);
	}
}
