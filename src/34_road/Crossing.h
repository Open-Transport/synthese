
/** Crossing class header.
	@file Crossing.h

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

#ifndef SYNTHESE_ENV_CROSSING_H
#define SYNTHESE_ENV_CROSSING_H

#include "Object.hpp"

#include "DataSourceLinksField.hpp"
#include "Hub.h"
#include "Vertex.h"
#include "Registry.h"
#include "Registrable.h"
#include "ImportableTemplate.hpp"
#include "Road.h"
#include "RoadPlace.h"
#include "GeometryField.hpp"

namespace synthese
{
	FIELD_POINTERS_VECTOR(NonReachableRoads, road::Road)
	
	typedef boost::fusion::map<
		FIELD(Key),
		FIELD(impex::DataSourceLinks),
		FIELD(NonReachableRoads),
		FIELD(PointGeometry)
	> CrossingSchema;
	
	namespace road
	{
		class ReachableFromCrossing;

		//////////////////////////////////////////////////////////////////////////
		/// Intersection between at least two roads.
		/// @ingroup m34
		/// @author Marc Jambert, Hugues Romain
		//////////////////////////////////////////////////////////////////////////
		/// Crossing implements both a hub and a vertex which belongs to itself :
		///	<ul>
		///		<li>Vertex : it can support road chunks as a common point intersection
		///		between roads</li>
		///		<li>Hub : it handles directly transfer between roads (by simplified
		///		way : 0 minutes delay, only one vertex (itself), always allowed)</li>
		///	</ul>
		class Crossing:
			public graph::Hub,
			public graph::Vertex,
			public Object<Crossing, CrossingSchema>,
			public virtual util::Registrable,
			public impex::ImportableTemplate<Crossing>
		{
		public:
			static const std::string TAG_NON_REACHABLE_ROADS;
			
			/// Chosen registry class.
			typedef util::Registry<Crossing> Registry;

			typedef std::set<ReachableFromCrossing*> ReachableVertices;
			typedef std::set<std::pair<const Road*, const Road*> > NonReachableRoadFromRoad;

		private:

			ReachableVertices _reachableVertices;
			NonReachableRoadFromRoad _nonReachableRoadFromRoad;

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			/// @param key identifier (default=0)
			/// @param geometry (default unknown) will not be cloned !
			/// @param codeBySource code of the crossing in the data source (default empty)
			/// @param source data source (default NULL)
			Crossing(
				util::RegistryKeyType key = 0,
				boost::shared_ptr<geos::geom::Point> geometry = boost::shared_ptr<geos::geom::Point>(),
				bool withIndexation = true
			);



			//////////////////////////////////////////////////////////////////////////
			/// Destructor.
			virtual ~Crossing ();

			//! @name Modifiers
			//@{
				void addReachableVertex(ReachableFromCrossing* value){ _reachableVertices.insert(value); }
				void removeReachableVertex(ReachableFromCrossing* value){ _reachableVertices.erase(value); }
			//@}

				void addNonReachableRoad(const std::pair<const Road*, const Road*>& pair) { _nonReachableRoadFromRoad.insert(pair); }

				const NonReachableRoadFromRoad getNonReachableRoads() const { return _nonReachableRoadFromRoad; }
				void setNonReachableRoads(const NonReachableRoadFromRoad& value) { _nonReachableRoadFromRoad = value; }

				bool isNonReachableRoad(const Road* from, const Road* to) const;
				
				virtual void addAdditionalParameters(util::ParametersMap& map, std::string prefix /* = std::string */) const;



			//! @name Virtual queries for Hub interface
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Lists the vertices contained in the hub.
				/// @param graphId the graph the vertices must belong to
				virtual Vertices getVertices(
					graph::GraphIdType graphId
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Gets the vertices contained by itself as a hub.
				/// @retval adds itself to the result if the searched graph is road
				/// @param whatToSearch graph to search
				/// @param origin vertex
				/// @param vertexIsOrigin
				virtual void getVertexAccessMap(
					graph::VertexAccessMap& result,
					graph::GraphIdType whatToSearch,
					const graph::Vertex& origin,
					bool vertexIsOrigin
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Tests if connection in the same graph is allowed at this hub.
				/// @param origin origin vertex
				/// @param destination destination vertex
				/// @return always true
				virtual bool isConnectionAllowed(
					const graph::Vertex& origin,
					const graph::Vertex& destination
				) const;



				///////////////////////////////////////////////////////////////
				/// Score getter.
				/// @return always 1 (all the crossing are considered interesting in routing)
				/// @author Hugues Romain
				//////////////////////////////////////////////////////////////////////////
				/// The score of a hub represents its capacity to provide some
				/// connections.
				/// The score range is from 0 to 100.
				/// Several special values are :
				///  - 0 : connection is never possible, jump over the hub in
				///		a routing procedure, except if the hub is near the goal
				///  - 1 : connection is possible, lowest score. This is the
				///		default score of a hub
				///  - 100 : maximum value for a score
				virtual graph::HubScore getScore(
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Tests if the objects contains a vertex of a graph.
				/// @param graphType graph which contained vertex must belong to
				/// @return true if the searched graph is road graph
				/// @author Hugues Romain
				virtual bool containsAnyVertex(graph::GraphIdType graphType) const;



				//////////////////////////////////////////////////////////////////////////
				/// Gets transfer delay.
				/// @param fromVertex origin vertex
				/// @param toVertex desrination vertex
				/// @return always 0 minutes
				virtual boost::posix_time::time_duration getTransferDelay(
					const graph::Vertex& fromVertex,
					const graph::Vertex& toVertex
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Gets the minimal transfer delay of the hub
				/// @return 0 minutes
				virtual boost::posix_time::time_duration getMinTransferDelay() const;



				//////////////////////////////////////////////////////////////////////////
				/// Gets the point representing the hub
				/// @return itself (got from vertex superclass)
				virtual const boost::shared_ptr<geos::geom::Point>& getPoint() const;
			//@}



			//! @name Virtual queries for Vertex interface
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Gets the identifier of the graph which the vertex belongs to.
				/// @return RoadModule::GRAPH_ID
				virtual graph::GraphIdType getGraphType() const;

				virtual std::string getRuleUserName() const;
			//@}
			
			//! @name Modifiers
			//@{
				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();
			//@}
		};
	}
}

#endif
