
/** VertexAccessMap Test implementation.
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

#include "VertexAccessMap.h"
#include "Journey.h"
#include "Vertex.h"
#include "Hub.h"
#include "Path.h"
#include "ServiceUse.h"
#include "Service.h"
#include "Edge.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/test/auto_unit_test.hpp>

using namespace synthese::graph;
using namespace synthese::geometry;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost;

class FakeHub:
	public Hub
{
public:
	FakeHub() {}
	Point2D p;

	virtual boost::posix_time::time_duration getMinTransferDelay() const { return boost::posix_time::minutes(0); }
	virtual void getVertexAccessMap(VertexAccessMap& result,const AccessDirection& accessDirection,	GraphIdType whatToSearch,const Vertex& vertex	) const {}
	virtual bool isConnectionAllowed(const Vertex& origin, const Vertex& destination) const { return true; }
	virtual boost::posix_time::time_duration getTransferDelay(	const Vertex& origin,const Vertex& destination	) const { return boost::posix_time::minutes(0); }
	virtual HubScore getScore() const { return 0; }
	virtual const Point2D& getPoint() const { return p; }
	virtual bool containsAnyVertex(GraphIdType graphType) const { return true; }
};

class FakeVertex:
	public Vertex
{
public:
	FakeVertex(Hub* hub): Vertex(hub) {}
	virtual GraphIdType getGraphType() const { return 0; }
};

class FakePath:
	public Path
{
public:
	FakePath() : Path() {}
	virtual bool isPedestrianMode() const { return true; }
	virtual bool isActive(const boost::gregorian::date& date) const { return true; }
	virtual const RuleUser* _getParentRuleUser() const {return NULL; }
	virtual std::string getRuleUserName() const { return "Path"; }
};

class FakeEdge:
	public Edge
{
public:
	FakeEdge() : Edge() {}
	virtual bool isDepartureAllowed() const { return true; }
	virtual bool isArrivalAllowed() const {return true;}
};

class FakeService:
	public Service
{
public:
	FakeService(): Service() {}
	virtual void _computeNextRTUpdate() {}
	virtual bool isContinuous () const { return true; }
	virtual boost::posix_time::time_duration getDepartureBeginScheduleToIndex(bool RTData,std::size_t rankInPath) const {return minutes(0);}
	virtual boost::posix_time::time_duration getDepartureEndScheduleToIndex(bool RTData,std::size_t rankInPath) const {return minutes(0);}
	virtual boost::posix_time::time_duration getArrivalBeginScheduleToIndex(bool RTData,std::size_t rankInPath) const {return minutes(0);}
	virtual boost::posix_time::time_duration getArrivalEndScheduleToIndex(bool RTData,std::size_t rankInPath) const {return minutes(0);}
	virtual ServicePointer getFromPresenceTime(bool RTData,AccessDirection method,std::size_t userClassRank,const Edge* edge,const boost::posix_time::ptime& presenceDateTime, bool controlIfTheServiceIsReachable, bool inverted) const {return ServicePointer();}
	virtual boost::posix_time::ptime getLeaveTime(const ServicePointer& servicePointer, const Edge* edge) const { return not_a_date_time; }
	virtual boost::posix_time::time_duration getDepartureSchedule(bool RTData,size_t rank) const {return minutes(0); }
};

BOOST_AUTO_TEST_CASE (Intersections)
{
	{ // Simple intersection : a vertex belongs to the both maps
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

		FakePath L;
		FakeService S;
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
		ServicePointer sp3_0(false, DEPARTURE_TO_ARRIVAL, 0, &e2);
		sp3_0.setService(&S);
		ptime d3_0s(date(2000,1,1), minutes(0));
		sp3_0.setActualTime(d3_0s);
		ptime d3_0e(date(2000,1,1), minutes(5));
		ServiceUse s3_0(sp3_0,&e3,d3_0e);
		j3 = Journey(j3, s3_0);

		map2.insert(&v3, VertexAccess(boost::posix_time::minutes(10), 1000, j3));
		
		BOOST_CHECK(map1.intersercts(map2));
		BOOST_CHECK(map2.intersercts(map1));
		Journey jt(map1.getBestIntersection(map2));
		BOOST_REQUIRE(!jt.empty());
		ServiceUse su(*jt.getServiceUses().begin());
		BOOST_CHECK_EQUAL(su.getDepartureEdge(), &e2);
		BOOST_CHECK_EQUAL(boost::posix_time::to_simple_string(su.getDepartureDateTime()), boost::posix_time::to_simple_string(d3_0s));
		BOOST_CHECK_EQUAL(su.getArrivalEdge(), &e3);
		BOOST_CHECK_EQUAL(boost::posix_time::to_simple_string(su.getArrivalDateTime()), boost::posix_time::to_simple_string(d3_0e));

	}

}
