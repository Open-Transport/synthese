
/** MainRoadPart Test implementation.
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

#include "Env.h"
#include "Road.h"
#include "RoadPath.hpp"
#include "Crossing.h"
#include "RoadPlace.h"
#include "RoadChunk.h"
#include "RoadChunkEdge.hpp"

#include <boost/test/auto_unit_test.hpp>

using namespace synthese::road;
using namespace synthese::util;
using namespace boost;
using namespace std;


BOOST_AUTO_TEST_CASE (Edges)
{
	Env env;

	{	// Regular order insertion 0 1 2
		RoadPlace p;

		Road r;
		r.get<RoadPlace::Vector>().push_back(&p);
		r.link(env);

		BOOST_REQUIRE_EQUAL(r.getForwardPath().getRoad(), &r);
		BOOST_REQUIRE_EQUAL(r.getReversePath().getRoad(), &r);

		BOOST_REQUIRE_EQUAL(r.getAnyRoadPlace()->getPaths().size(), 2ULL);
		BOOST_CHECK(p.getPaths().find(&r.getForwardPath()) != p.getPaths().end());
		BOOST_CHECK(p.getPaths().find(&r.getReversePath()) != p.getPaths().end());

		BOOST_REQUIRE_EQUAL(p.getRoads().size(), 1);
		BOOST_CHECK(p.getRoads().find(&r) != p.getRoads().end());

		Crossing cr1;
		RoadChunk c1(0, &cr1, 0, &r, 0);
		c1.link(env);

		BOOST_CHECK_EQUAL(c1.getForwardEdge().getFromVertex(), &cr1);
		BOOST_CHECK_EQUAL(c1.getForwardEdge().getRankInPath(), c1.getRankInPath());
		BOOST_CHECK_EQUAL(c1.getForwardEdge().getMetricOffset(), c1.getMetricOffset());
		BOOST_CHECK_EQUAL(c1.getForwardEdge().getRoadChunk(), &c1);
		BOOST_CHECK_EQUAL(c1.getReverseEdge().getFromVertex(), &cr1);
		BOOST_CHECK_EQUAL(c1.getReverseEdge().getRankInPath(), numeric_limits<size_t>::max() - c1.getRankInPath());
		BOOST_CHECK_EQUAL(c1.getReverseEdge().getMetricOffset(), -c1.getMetricOffset());
		BOOST_CHECK_EQUAL(c1.getReverseEdge().getRoadChunk(), &c1);

		BOOST_REQUIRE_EQUAL(r.getForwardPath().getEdges().size(), 1);
		BOOST_CHECK_EQUAL(r.getForwardPath().getEdges().at(0), &c1.getForwardEdge());
		BOOST_REQUIRE_EQUAL(r.getReversePath().getEdges().size(), 1);
		BOOST_CHECK_EQUAL(r.getReversePath().getEdges().at(0), &c1.getReverseEdge());

		Crossing cr2;
		RoadChunk c2(0, &cr2, 1, &r, 10);
		c2.link(env);

		BOOST_CHECK_EQUAL(c1.getForwardEdge().getFromVertex(), &cr1);
		BOOST_CHECK_EQUAL(c1.getForwardEdge().getRankInPath(), c1.getRankInPath());
		BOOST_CHECK_EQUAL(c1.getForwardEdge().getMetricOffset(), c1.getMetricOffset());
		BOOST_CHECK_EQUAL(c1.getForwardEdge().getRoadChunk(), &c1);
		BOOST_CHECK_EQUAL(c1.getReverseEdge().getFromVertex(), &cr1);
		BOOST_CHECK_EQUAL(c1.getReverseEdge().getRankInPath(), numeric_limits<size_t>::max() - c1.getRankInPath());
		BOOST_CHECK_EQUAL(c1.getReverseEdge().getMetricOffset(), -c1.getMetricOffset());
		BOOST_CHECK_EQUAL(c1.getReverseEdge().getRoadChunk(), &c1);

		BOOST_CHECK_EQUAL(c2.getForwardEdge().getFromVertex(), &cr2);
		BOOST_CHECK_EQUAL(c2.getForwardEdge().getRankInPath(), c2.getRankInPath());
		BOOST_CHECK_EQUAL(c2.getForwardEdge().getMetricOffset(), c2.getMetricOffset());
		BOOST_CHECK_EQUAL(c2.getForwardEdge().getRoadChunk(), &c2);
		BOOST_CHECK_EQUAL(c2.getReverseEdge().getFromVertex(), &cr2);
		BOOST_CHECK_EQUAL(c2.getReverseEdge().getRankInPath(), numeric_limits<size_t>::max() - c2.getRankInPath());
		BOOST_CHECK_EQUAL(c2.getReverseEdge().getMetricOffset(), -c2.getMetricOffset());
		BOOST_CHECK_EQUAL(c2.getReverseEdge().getRoadChunk(), &c2);

		BOOST_REQUIRE_EQUAL(r.getForwardPath().getEdges().size(), 2);
		BOOST_CHECK_EQUAL(r.getForwardPath().getEdges().at(0), &c1.getForwardEdge());
		BOOST_CHECK_EQUAL(r.getForwardPath().getEdges().at(1), &c2.getForwardEdge());
		BOOST_REQUIRE_EQUAL(r.getReversePath().getEdges().size(), 2);
		BOOST_CHECK_EQUAL(r.getReversePath().getEdges().at(0), &c2.getReverseEdge());
		BOOST_CHECK_EQUAL(r.getReversePath().getEdges().at(1), &c1.getReverseEdge());


		Crossing cr3;
		RoadChunk c3(0, &cr3, 2, &r, 20);
		c3.link(env);

		BOOST_CHECK_EQUAL(c1.getForwardEdge().getFromVertex(), &cr1);
		BOOST_CHECK_EQUAL(c1.getForwardEdge().getRankInPath(), c1.getRankInPath());
		BOOST_CHECK_EQUAL(c1.getForwardEdge().getMetricOffset(), c1.getMetricOffset());
		BOOST_CHECK_EQUAL(c1.getForwardEdge().getRoadChunk(), &c1);
		BOOST_CHECK_EQUAL(c1.getReverseEdge().getFromVertex(), &cr1);
		BOOST_CHECK_EQUAL(c1.getReverseEdge().getRankInPath(), numeric_limits<size_t>::max() - c1.getRankInPath());
		BOOST_CHECK_EQUAL(c1.getReverseEdge().getMetricOffset(), -c1.getMetricOffset());
		BOOST_CHECK_EQUAL(c1.getReverseEdge().getRoadChunk(), &c1);

		BOOST_CHECK_EQUAL(c2.getForwardEdge().getFromVertex(), &cr2);
		BOOST_CHECK_EQUAL(c2.getForwardEdge().getRankInPath(), c2.getRankInPath());
		BOOST_CHECK_EQUAL(c2.getForwardEdge().getMetricOffset(), c2.getMetricOffset());
		BOOST_CHECK_EQUAL(c2.getForwardEdge().getRoadChunk(), &c2);
		BOOST_CHECK_EQUAL(c2.getReverseEdge().getFromVertex(), &cr2);
		BOOST_CHECK_EQUAL(c2.getReverseEdge().getRankInPath(), numeric_limits<size_t>::max() - c2.getRankInPath());
		BOOST_CHECK_EQUAL(c2.getReverseEdge().getMetricOffset(), -c2.getMetricOffset());
		BOOST_CHECK_EQUAL(c2.getReverseEdge().getRoadChunk(), &c2);

		BOOST_CHECK_EQUAL(c3.getForwardEdge().getFromVertex(), &cr3);
		BOOST_CHECK_EQUAL(c3.getForwardEdge().getRankInPath(), c3.getRankInPath());
		BOOST_CHECK_EQUAL(c3.getForwardEdge().getMetricOffset(), c3.getMetricOffset());
		BOOST_CHECK_EQUAL(c3.getForwardEdge().getRoadChunk(), &c3);
		BOOST_CHECK_EQUAL(c3.getReverseEdge().getFromVertex(), &cr3);
		BOOST_CHECK_EQUAL(c3.getReverseEdge().getRankInPath(), numeric_limits<size_t>::max() - c3.getRankInPath());
		BOOST_CHECK_EQUAL(c3.getReverseEdge().getMetricOffset(), -c3.getMetricOffset());
		BOOST_CHECK_EQUAL(c3.getReverseEdge().getRoadChunk(), &c3);

		BOOST_REQUIRE_EQUAL(r.getForwardPath().getEdges().size(), 3);
		BOOST_CHECK_EQUAL(r.getForwardPath().getEdges().at(0), &c1.getForwardEdge());
		BOOST_CHECK_EQUAL(r.getForwardPath().getEdges().at(1), &c2.getForwardEdge());
		BOOST_CHECK_EQUAL(r.getForwardPath().getEdges().at(2), &c3.getForwardEdge());
		BOOST_REQUIRE_EQUAL(r.getReversePath().getEdges().size(), 3);
		BOOST_CHECK_EQUAL(r.getReversePath().getEdges().at(0), &c3.getReverseEdge());
		BOOST_CHECK_EQUAL(r.getReversePath().getEdges().at(1), &c2.getReverseEdge());
		BOOST_CHECK_EQUAL(r.getReversePath().getEdges().at(2), &c1.getReverseEdge());
	}

	{ // Reverse order insertion 2 1 0
		RoadPlace p;

		Road r;
		r.get<RoadPlace::Vector>().push_back(&p);
		r.link(env);

		Crossing cr2;
		RoadChunk c2(0, &cr2, 2, &r, 20);
		c2.link(env);

		BOOST_CHECK_EQUAL(c2.getForwardEdge().getFromVertex(), &cr2);
		BOOST_CHECK_EQUAL(c2.getForwardEdge().getRankInPath(), c2.getRankInPath());
		BOOST_CHECK_EQUAL(c2.getForwardEdge().getMetricOffset(), c2.getMetricOffset());
		BOOST_CHECK_EQUAL(c2.getForwardEdge().getRoadChunk(), &c2);
		BOOST_CHECK_EQUAL(c2.getReverseEdge().getFromVertex(), &cr2);
		BOOST_CHECK_EQUAL(c2.getReverseEdge().getRankInPath(), numeric_limits<size_t>::max() - c2.getRankInPath());
		BOOST_CHECK_EQUAL(c2.getReverseEdge().getMetricOffset(), -c2.getMetricOffset());
		BOOST_CHECK_EQUAL(c2.getReverseEdge().getRoadChunk(), &c2);

		BOOST_REQUIRE_EQUAL(r.getForwardPath().getEdges().size(), 1);
		BOOST_CHECK_EQUAL(r.getForwardPath().getEdges().at(0), &c2.getForwardEdge());
		BOOST_REQUIRE_EQUAL(r.getReversePath().getEdges().size(), 1);
		BOOST_CHECK_EQUAL(r.getReversePath().getEdges().at(0), &c2.getReverseEdge());

		Crossing cr1;
		RoadChunk c1(0, &cr1, 1, &r, 10);
		c1.link(env);

		BOOST_CHECK_EQUAL(c1.getForwardEdge().getFromVertex(), &cr1);
		BOOST_CHECK_EQUAL(c1.getForwardEdge().getRankInPath(), c1.getRankInPath());
		BOOST_CHECK_EQUAL(c1.getForwardEdge().getMetricOffset(), c1.getMetricOffset());
		BOOST_CHECK_EQUAL(c1.getForwardEdge().getRoadChunk(), &c1);
		BOOST_CHECK_EQUAL(c1.getReverseEdge().getFromVertex(), &cr1);
		BOOST_CHECK_EQUAL(c1.getReverseEdge().getRankInPath(), numeric_limits<size_t>::max() - c1.getRankInPath());
		BOOST_CHECK_EQUAL(c1.getReverseEdge().getMetricOffset(), -c1.getMetricOffset());
		BOOST_CHECK_EQUAL(c1.getReverseEdge().getRoadChunk(), &c1);

		BOOST_CHECK_EQUAL(c2.getForwardEdge().getFromVertex(), &cr2);
		BOOST_CHECK_EQUAL(c2.getForwardEdge().getRankInPath(), c2.getRankInPath());
		BOOST_CHECK_EQUAL(c2.getForwardEdge().getMetricOffset(), c2.getMetricOffset());
		BOOST_CHECK_EQUAL(c2.getForwardEdge().getRoadChunk(), &c2);
		BOOST_CHECK_EQUAL(c2.getReverseEdge().getFromVertex(), &cr2);
		BOOST_CHECK_EQUAL(c2.getReverseEdge().getRankInPath(), numeric_limits<size_t>::max() - c2.getRankInPath());
		BOOST_CHECK_EQUAL(c2.getReverseEdge().getMetricOffset(), -c2.getMetricOffset());
		BOOST_CHECK_EQUAL(c2.getReverseEdge().getRoadChunk(), &c2);

		BOOST_REQUIRE_EQUAL(r.getForwardPath().getEdges().size(), 2);
		BOOST_CHECK_EQUAL(r.getForwardPath().getEdges().at(0), &c1.getForwardEdge());
		BOOST_CHECK_EQUAL(r.getForwardPath().getEdges().at(1), &c2.getForwardEdge());
		BOOST_REQUIRE_EQUAL(r.getReversePath().getEdges().size(), 2);
		BOOST_CHECK_EQUAL(r.getReversePath().getEdges().at(0), &c2.getReverseEdge());
		BOOST_CHECK_EQUAL(r.getReversePath().getEdges().at(1), &c1.getReverseEdge());

		Crossing cr0;
		RoadChunk c0(0, &cr0, 0, &r, 0);
		c0.link(env);

		BOOST_CHECK_EQUAL(c0.getForwardEdge().getFromVertex(), &cr0);
		BOOST_CHECK_EQUAL(c0.getForwardEdge().getRankInPath(), c0.getRankInPath());
		BOOST_CHECK_EQUAL(c0.getForwardEdge().getMetricOffset(), c0.getMetricOffset());
		BOOST_CHECK_EQUAL(c0.getForwardEdge().getRoadChunk(), &c0);
		BOOST_CHECK_EQUAL(c0.getReverseEdge().getFromVertex(), &cr0);
		BOOST_CHECK_EQUAL(c0.getReverseEdge().getRankInPath(), numeric_limits<size_t>::max() - c0.getRankInPath());
		BOOST_CHECK_EQUAL(c0.getReverseEdge().getMetricOffset(), -c0.getMetricOffset());
		BOOST_CHECK_EQUAL(c0.getReverseEdge().getRoadChunk(), &c0);

		BOOST_CHECK_EQUAL(c1.getForwardEdge().getFromVertex(), &cr1);
		BOOST_CHECK_EQUAL(c1.getForwardEdge().getRankInPath(), c1.getRankInPath());
		BOOST_CHECK_EQUAL(c1.getForwardEdge().getMetricOffset(), c1.getMetricOffset());
		BOOST_CHECK_EQUAL(c1.getForwardEdge().getRoadChunk(), &c1);
		BOOST_CHECK_EQUAL(c1.getReverseEdge().getFromVertex(), &cr1);
		BOOST_CHECK_EQUAL(c1.getReverseEdge().getRankInPath(), numeric_limits<size_t>::max() - c1.getRankInPath());
		BOOST_CHECK_EQUAL(c1.getReverseEdge().getMetricOffset(), -c1.getMetricOffset());
		BOOST_CHECK_EQUAL(c1.getReverseEdge().getRoadChunk(), &c1);

		BOOST_CHECK_EQUAL(c2.getForwardEdge().getFromVertex(), &cr2);
		BOOST_CHECK_EQUAL(c2.getForwardEdge().getRankInPath(), c2.getRankInPath());
		BOOST_CHECK_EQUAL(c2.getForwardEdge().getMetricOffset(), c2.getMetricOffset());
		BOOST_CHECK_EQUAL(c2.getForwardEdge().getRoadChunk(), &c2);
		BOOST_CHECK_EQUAL(c2.getReverseEdge().getFromVertex(), &cr2);
		BOOST_CHECK_EQUAL(c2.getReverseEdge().getRankInPath(), numeric_limits<size_t>::max() - c2.getRankInPath());
		BOOST_CHECK_EQUAL(c2.getReverseEdge().getMetricOffset(), -c2.getMetricOffset());
		BOOST_CHECK_EQUAL(c2.getReverseEdge().getRoadChunk(), &c2);

		BOOST_REQUIRE_EQUAL(r.getForwardPath().getEdges().size(), 3);
		BOOST_CHECK_EQUAL(r.getForwardPath().getEdges().at(0), &c0.getForwardEdge());
		BOOST_CHECK_EQUAL(r.getForwardPath().getEdges().at(1), &c1.getForwardEdge());
		BOOST_CHECK_EQUAL(r.getForwardPath().getEdges().at(2), &c2.getForwardEdge());
		BOOST_REQUIRE_EQUAL(r.getReversePath().getEdges().size(), 3);
		BOOST_CHECK_EQUAL(r.getReversePath().getEdges().at(0), &c2.getReverseEdge());
		BOOST_CHECK_EQUAL(r.getReversePath().getEdges().at(1), &c1.getReverseEdge());
		BOOST_CHECK_EQUAL(r.getReversePath().getEdges().at(2), &c0.getReverseEdge());
	}

	{ // Random order insertion 2 0 1
		RoadPlace p;

		Road r;
		r.get<RoadPlace::Vector>().push_back(&p);
		r.link(env);

		Crossing cr1;
		RoadChunk c1(0, &cr1, 2, &r, 10);
		c1.link(env);

		Crossing cr2;
		RoadChunk c2(0, &cr2, 0, &r, 0);
		c2.link(env);

		Crossing cr3;
		RoadChunk c3(0, &cr3, 1, &r, 5);
		c3.link(env);

		BOOST_REQUIRE_EQUAL(r.getForwardPath().getEdges().size(), 3);
		BOOST_CHECK_EQUAL(r.getForwardPath().getEdges().at(0), &c2.getForwardEdge());
		BOOST_CHECK_EQUAL(r.getForwardPath().getEdges().at(1), &c3.getForwardEdge());
		BOOST_CHECK_EQUAL(r.getForwardPath().getEdges().at(2), &c1.getForwardEdge());
		BOOST_REQUIRE_EQUAL(r.getReversePath().getEdges().size(), 3);
		BOOST_CHECK_EQUAL(r.getReversePath().getEdges().at(0), &c1.getReverseEdge());
		BOOST_CHECK_EQUAL(r.getReversePath().getEdges().at(1), &c3.getReverseEdge());
		BOOST_CHECK_EQUAL(r.getReversePath().getEdges().at(2), &c2.getReverseEdge());
	}

	{ // Insertions 0 1 2
		RoadPlace p;

		Road r;
		r.get<RoadPlace::Vector>().push_back(&p);
		r.link(env);

		Crossing cr2;
		RoadChunk c2(0, &cr2, 0, &r, 0);
		c2.link(env);

		Crossing cr1;
		RoadChunk c1(0, &cr1, 0, &r, 0);
		r.insertRoadChunk(c1, 10, 1);
		
		BOOST_CHECK_EQUAL(c1.getForwardEdge().getFromVertex(), &cr1);
		BOOST_CHECK_EQUAL(c1.getForwardEdge().getRankInPath(), c1.getRankInPath());
		BOOST_CHECK_EQUAL(c1.getForwardEdge().getMetricOffset(), c1.getMetricOffset());
		BOOST_CHECK_EQUAL(c1.getForwardEdge().getRoadChunk(), &c1);
		BOOST_CHECK_EQUAL(c1.getReverseEdge().getFromVertex(), &cr1);
		BOOST_CHECK_EQUAL(c1.getReverseEdge().getRankInPath(), numeric_limits<size_t>::max() - c1.getRankInPath());
		BOOST_CHECK_EQUAL(c1.getReverseEdge().getMetricOffset(), -c1.getMetricOffset());
		BOOST_CHECK_EQUAL(c1.getReverseEdge().getRoadChunk(), &c1);

		BOOST_CHECK_EQUAL(c2.getRankInPath(), 1);
		BOOST_CHECK_EQUAL(c2.getMetricOffset(), 10);

		BOOST_CHECK_EQUAL(c2.getForwardEdge().getFromVertex(), &cr2);
		BOOST_CHECK_EQUAL(c2.getForwardEdge().getRankInPath(), c2.getRankInPath());
		BOOST_CHECK_EQUAL(c2.getForwardEdge().getMetricOffset(), c2.getMetricOffset());
		BOOST_CHECK_EQUAL(c2.getForwardEdge().getRoadChunk(), &c2);
		BOOST_CHECK_EQUAL(c2.getReverseEdge().getFromVertex(), &cr2);
		BOOST_CHECK_EQUAL(c2.getReverseEdge().getRankInPath(), numeric_limits<size_t>::max() - c2.getRankInPath());
		BOOST_CHECK_EQUAL(c2.getReverseEdge().getMetricOffset(), -c2.getMetricOffset());
		BOOST_CHECK_EQUAL(c2.getReverseEdge().getRoadChunk(), &c2);

		BOOST_REQUIRE_EQUAL(r.getForwardPath().getEdges().size(), 2);
		BOOST_CHECK_EQUAL(r.getForwardPath().getEdges().at(0), &c1.getForwardEdge());
		BOOST_CHECK_EQUAL(r.getForwardPath().getEdges().at(1), &c2.getForwardEdge());
		BOOST_REQUIRE_EQUAL(r.getReversePath().getEdges().size(), 2);
		BOOST_CHECK_EQUAL(r.getReversePath().getEdges().at(0), &c2.getReverseEdge());
		BOOST_CHECK_EQUAL(r.getReversePath().getEdges().at(1), &c1.getReverseEdge());

		Crossing cr0;
		RoadChunk c0(0, &cr0, 0, &r, 0);
		r.insertRoadChunk(c0, 20, 1);
		
		BOOST_CHECK_EQUAL(c0.getForwardEdge().getFromVertex(), &cr0);
		BOOST_CHECK_EQUAL(c0.getForwardEdge().getRankInPath(), c0.getRankInPath());
		BOOST_CHECK_EQUAL(c0.getForwardEdge().getMetricOffset(), c0.getMetricOffset());
		BOOST_CHECK_EQUAL(c0.getForwardEdge().getRoadChunk(), &c0);
		BOOST_CHECK_EQUAL(c0.getReverseEdge().getFromVertex(), &cr0);
		BOOST_CHECK_EQUAL(c0.getReverseEdge().getRankInPath(), numeric_limits<size_t>::max() - c0.getRankInPath());
		BOOST_CHECK_EQUAL(c0.getReverseEdge().getMetricOffset(), -c0.getMetricOffset());
		BOOST_CHECK_EQUAL(c0.getReverseEdge().getRoadChunk(), &c0);

		BOOST_CHECK_EQUAL(c1.getRankInPath(), 1);
		BOOST_CHECK_EQUAL(c1.getMetricOffset(), 20);

		BOOST_CHECK_EQUAL(c1.getForwardEdge().getFromVertex(), &cr1);
		BOOST_CHECK_EQUAL(c1.getForwardEdge().getRankInPath(), c1.getRankInPath());
		BOOST_CHECK_EQUAL(c1.getForwardEdge().getMetricOffset(), c1.getMetricOffset());
		BOOST_CHECK_EQUAL(c1.getForwardEdge().getRoadChunk(), &c1);
		BOOST_CHECK_EQUAL(c1.getReverseEdge().getFromVertex(), &cr1);
		BOOST_CHECK_EQUAL(c1.getReverseEdge().getRankInPath(), numeric_limits<size_t>::max() - c1.getRankInPath());
		BOOST_CHECK_EQUAL(c1.getReverseEdge().getMetricOffset(), -c1.getMetricOffset());
		BOOST_CHECK_EQUAL(c1.getReverseEdge().getRoadChunk(), &c1);

		BOOST_CHECK_EQUAL(c2.getRankInPath(), 2);
		BOOST_CHECK_EQUAL(c2.getMetricOffset(), 30);

		BOOST_CHECK_EQUAL(c2.getForwardEdge().getFromVertex(), &cr2);
		BOOST_CHECK_EQUAL(c2.getForwardEdge().getRankInPath(), c2.getRankInPath());
		BOOST_CHECK_EQUAL(c2.getForwardEdge().getMetricOffset(), c2.getMetricOffset());
		BOOST_CHECK_EQUAL(c2.getForwardEdge().getRoadChunk(), &c2);
		BOOST_CHECK_EQUAL(c2.getReverseEdge().getFromVertex(), &cr2);
		BOOST_CHECK_EQUAL(c2.getReverseEdge().getRankInPath(), numeric_limits<size_t>::max() - c2.getRankInPath());
		BOOST_CHECK_EQUAL(c2.getReverseEdge().getMetricOffset(), -c2.getMetricOffset());
		BOOST_CHECK_EQUAL(c2.getReverseEdge().getRoadChunk(), &c2);

		BOOST_REQUIRE_EQUAL(r.getForwardPath().getEdges().size(), 3);
		BOOST_CHECK_EQUAL(r.getForwardPath().getEdges().at(0), &c0.getForwardEdge());
		BOOST_CHECK_EQUAL(r.getForwardPath().getEdges().at(1), &c1.getForwardEdge());
		BOOST_CHECK_EQUAL(r.getForwardPath().getEdges().at(2), &c2.getForwardEdge());
		BOOST_REQUIRE_EQUAL(r.getReversePath().getEdges().size(), 3);
		BOOST_CHECK_EQUAL(r.getReversePath().getEdges().at(0), &c2.getReverseEdge());
		BOOST_CHECK_EQUAL(r.getReversePath().getEdges().at(1), &c1.getReverseEdge());
		BOOST_CHECK_EQUAL(r.getReversePath().getEdges().at(2), &c0.getReverseEdge());
	}
}
