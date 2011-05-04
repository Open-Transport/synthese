
/** FakeGraphImplementation class header.
	@file FakeGraphImplementation.hpp

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

#include "Vertex.h"
#include "Hub.h"
#include "Path.h"
#include "Service.h"
#include "Edge.h"

#include <boost/date_time/posix_time/posix_time.hpp>

namespace synthese
{
	namespace graph
	{
		class FakeHub:
			public Hub
		{
		public:
			FakeHub() {}
			boost::shared_ptr<geos::geom::Point> p;

			virtual boost::posix_time::time_duration getMinTransferDelay() const { return boost::posix_time::minutes(0); }
			virtual void getVertexAccessMap(VertexAccessMap& result, GraphIdType whatToSearch,const Vertex& vertex, bool vertexIsOrigin ) const {}
			virtual bool isConnectionAllowed(const Vertex& origin, const Vertex& destination) const { return true; }
			virtual boost::posix_time::time_duration getTransferDelay(	const Vertex& origin,const Vertex& destination	) const { return boost::posix_time::minutes(0); }
			virtual HubScore getScore() const { return 0; }
			virtual boost::shared_ptr<geos::geom::Point> getPoint() const { return p; }
			virtual bool containsAnyVertex(GraphIdType graphType) const { return true; }
		};

		class FakeVertex:
			public Vertex
		{
		public:
			FakeVertex(Hub* hub): Vertex(hub, boost::shared_ptr<geos::geom::Point>()), synthese::util::Registrable(0) {}
			virtual GraphIdType getGraphType() const { return 0; }
		};

		class FakePath:
			public Path
		{
		public:
			FakePath() : Path(), synthese::util::Registrable(0) {}
			virtual bool isPedestrianMode() const { return true; }
			virtual bool isActive(const boost::gregorian::date& date) const { return true; }
			virtual const RuleUser* _getParentRuleUser() const {return NULL; }
			virtual std::string getRuleUserName() const { return "Path"; }
		};

		class FakeEdge:
			public Edge
		{
		public:
			FakeEdge() : Edge(), synthese::util::Registrable(0) {}
			virtual bool isDepartureAllowed() const { return true; }
			virtual bool isArrivalAllowed() const {return true;}
		};

		class FakeService:
			public Service
		{
		public:
			FakeService(): Service(), synthese::util::Registrable(0) {}
			virtual void _computeNextRTUpdate() {}
			virtual bool isContinuous () const { return true; }
			virtual boost::posix_time::time_duration getDepartureBeginScheduleToIndex(bool RTData,std::size_t rankInPath) const {return boost::posix_time::minutes(0);}
			virtual boost::posix_time::time_duration getDepartureEndScheduleToIndex(bool RTData,std::size_t rankInPath) const {return boost::posix_time::minutes(0);}
			virtual boost::posix_time::time_duration getArrivalBeginScheduleToIndex(bool RTData,std::size_t rankInPath) const {return boost::posix_time::minutes(0);}
			virtual boost::posix_time::time_duration getArrivalEndScheduleToIndex(bool RTData,std::size_t rankInPath) const {return boost::posix_time::minutes(0);}
			virtual ServicePointer getFromPresenceTime(bool RTData,bool getDeparture,std::size_t userClassRank,const Edge& edge, const boost::posix_time::ptime& presenceDateTime, bool controlIfTheServiceIsReachable, bool inverted, bool ignoreReservation) const {return ServicePointer();}
			virtual void completeServicePointer(synthese::graph::ServicePointer &,const synthese::graph::Edge &,const synthese::graph::AccessParameters &) const {}
			virtual boost::posix_time::ptime getLeaveTime(const ServicePointer& servicePointer, const Edge* edge) const { return boost::posix_time::not_a_date_time; }
			virtual boost::posix_time::time_duration getDepartureSchedule(bool RTData,size_t rank) const {return boost::posix_time::minutes(0); }
		};

	}
}

