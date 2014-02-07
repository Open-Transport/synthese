
/** Path class header.
	@file Path.h

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

#ifndef SYNTHESE_ENV_PATH_H
#define SYNTHESE_ENV_PATH_H

#include "Registrable.h"
#include "RuleUser.h"

#include "ChronologicalServicesCollection.hpp"
#include "GraphTypes.h"
#include "shared_recursive_mutex.hpp"

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <vector>

namespace geos
{
	namespace geom
	{
		class Point;
		class LineString;
	}
}

namespace synthese
{
	namespace graph
	{
		class Service;
		class Edge;
		class Vertex;
		class PathGroup;
		class PathClass;


		/** Path abstract base class.
			@author Marc Jambert

			A path is a sequence of edges.

			A path is associated with a set of services allowing
			to follow this path at certain dates and times.

			It is associated as well with a set of compliances, defining
			which types of entities are able to move along this path. For instance :
				- a bus line is compliant with pedestrians, may be compliant with bikes
				and wheelchairs
				- a road may be compliant only with cars, or only for pedestrians and bikes,
				or for all
				- a ferry line is compliant with cars, bikes, pedestrian
				- ...

			@ingroup m18
		*/
		class Path:
			public RuleUser,
			public virtual util::Registrable
		{
		public:
			typedef std::vector<Edge*> Edges;
			typedef std::map<MetricOffset, std::size_t> RankMap;
			typedef std::vector<boost::shared_ptr<ChronologicalServicesCollection> > ServiceCollections;

		protected:
			PathGroup*		_pathGroup;	//!< Up link : path group
			PathClass*		_pathClass;	//!< Up link : path class
			PathClass*		_pathNetwork;	//!< Up link : path network class
			Edges			_edges; 	//!< Down link 1 : edges
			ServiceCollections		_serviceCollections;	//!< Down link 2 : services
			RankMap			_rankMap;	//!< Saves the first edge at each metric offset

			/** Constructor.
			*/
			Path();

		public:
			/// Use it to protect iteration over the services
			boost::shared_ptr<util::shared_recursive_mutex> sharedServicesMutex;

			virtual ~Path ();


			//! @name Getters
			//@{
				/// @warning Iterating over the service collections must be protected
				/// by using a boost::shared_lock with _sharedServicesMutex.
				
				const Edges&		getEdges()		const { return _edges; }
				Edges&				getEdges()			  { return _edges; }
				PathClass*			getPathClass()	const { return _pathClass; }
				PathClass*          getPathNetwork()  const { return _pathNetwork; }
				PathGroup*			getPathGroup()	const { return _pathGroup; }
				const ServiceCollections& getServiceCollections() const { return _serviceCollections; }
			//@}

			//! @name Setters
			//@{
			//@}

			//! @name Services.
			//@{
				virtual const RuleUser* _getParentRuleUser() const;

				//////////////////////////////////////////////////////////////////////////
				/// Gets the edge at the specified rank.
				/// @param index the rank of the edge (which is not necessarily equal to the index of the object in the _edges vector)
				/// @return the specified edge
				/// @throws Exception if the edge was not found
				const Edge*	getEdge (std::size_t index) const;

				Edge*	getLastEdge()	const;

				virtual bool isRoad() const;
				virtual bool isPedestrianMode() const = 0;



				//////////////////////////////////////////////////////////////////////////
				/// Tests the possibility of finding a running service at a given date.
				/// @param date the date to test
				/// @return true if the path should contain a running service at the date.
				/// @warning this test does not certify that a service is actually running
				/// at the given date. If the response is false, then it is sure that
				/// no service runs at the given date.
				/// @author Hugues Romain
				virtual bool isActive(const boost::gregorian::date& date) const = 0;



				//////////////////////////////////////////////////////////////////////////
				/// builds a geos linestring of all the points linked by the path.
				///	@param fromEdgeIndex first edge to include
				///	@param toEdgeIndex last edge to include (undefined = all edges)
				/// @author Thomas Bonfort, Hugues Romain
				/// @date 2010
				/// @since 3.2.0
				boost::shared_ptr<geos::geom::LineString> getGeometry(
					std::size_t fromEdgeIndex = 0,
					boost::optional<std::size_t> toEdgeIndex = boost::optional<std::size_t>()
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Returns the distance from this path to the given point.
				/// @param pt point
				/// @return the distance from this path to the given point.
				/// @author Thomas Bonfort
				/// @date 2010
				/// @since 3.2.0
				double distance(const geos::geom::Point& pt) const;



				//////////////////////////////////////////////////////////////////////////
				/// Productes a log output if the geometry of the path is unrelevant.
				/// @author Thomas Bonfort
				/// @date 2010
				/// @since 3.2.0
				//////////////////////////////////////////////////////////////////////////
				/// The test consists in comparison between the differences between
				/// metric offsets of the last and the first edges, and the sum of the
				/// length of each geometry
				void validateGeometry() const;



				//////////////////////////////////////////////////////////////////////////
				/// Gets the length of the path.
				/// @return the length of the path
				/// @author Thomas Bonfort
				/// @date 2010
				/// @since 3.2.0
				double length() const;


				//////////////////////////////////////////////////////////////////////////
				/// Compares two paths on the served vertices or hubs.
				/// @param other other path to compare with
				/// @param considerVertices if true the served vertices must be identical, if false the served hubs must be identical
				/// @param considerDepartureArrival if true the departure and arrival authorizations must be the same for each compared edge
				/// @return true if the two paths are identical
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.1.16
				bool sameContent(
					const Path& other,
					bool considerVertices,
					bool considerDepartureArrival
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Tests if the current path includes an other path by comparison on the served vertices or hubs.
				/// @param other other path to compare with
				/// @param considerVertices if true the served vertices must be identical, if false the served hubs must be identical
				/// @return true if the current path offers all relations offered by the second one
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.1.16
				bool includes(const Path& other, bool considerVertices) const;



				//////////////////////////////////////////////////////////////////////////
				/// Returns the edge that contains the point at given distance from start.
				/// @param offset distance along the line, must be in ]0,length()[
				/// @author Thomas Bonfort
				/// @date 2010
				/// @since 3.2.0
				Edge* getEdgeAtOffset(double offset) const;



				class InvalidOffsetException:
					public synthese::Exception
				{
				public:
					InvalidOffsetException(MetricOffset offset, const Path& path);
				};

				//////////////////////////////////////////////////////////////////////////
				/// Returns the rank of the edge recorded at a specific offset
				//////////////////////////////////////////////////////////////////////////
				/// @param offset distance along the path
				/// @return the rank of the edge at the specified offset
				/// @throws InvalidOffsetException if the offset was not found
				/// @author Hugues Romain
				/// @date 2011
				/// @since 3.3.0
				std::size_t getEdgeRankAtOffset(MetricOffset offset) const;



				//////////////////////////////////////////////////////////////////////////
				/// Search in the path for an edge starting at the specified vertex.
				/// @param vertex vertex to search
				/// @return an edge starting at the specified vertex
				/// @throw VertexNotFoundException if the vertex is not served by the path
				/// @warning if the vertex is served several times by the path, then the returned edge is randomly chosen.
				/// @author Hugues Romain
				/// @date 2011
				/// @since 3.3.0
				const Edge& findEdgeByVertex(const Vertex* vertex) const;
				class VertexNotFoundException: public synthese::Exception { public: VertexNotFoundException() : synthese::Exception("Vertex was not found in the path.") {} };


				bool contains(const Service& service) const;
			//@}

			//! @name Modifiers.
			//@{
			protected:

				//////////////////////////////////////////////////////////////////////////
				/// Builds the links between a new edge and the existing edges of the path.
				//////////////////////////////////////////////////////////////////////////
				/// @param previousEdge previous edge before the new one, NULL if the new edge is inserted at first
				/// @param nextEdge next edge after the new one, NULL if the new edge is inserted at the end
				/// @param edge the new edge
				void _linkEdge(
					Edge* previousEdge,
					Edge* nextEdge,
					Edge& edge
				);

			public:
				const Vertex* getDestination () const;
				const Vertex* getOrigin () const;

				//////////////////////////////////////////////////////////////////////////
				/// Inserts an edge in the path.
				///	@param edge The edge to add
				/// @author Hugues Romain
				/// @pre no existing edge in the path must have the same rank than the rank of the
				/// edge to add
				//////////////////////////////////////////////////////////////////////////
				///	All the pointer links necessary to the graph exploration are created :
				///		- the links between edges (describing the path)
				///
				///	The order of addEdge calls can be random : the rankinpath attribute of edge
				///	is used to insert the new object at the good position.
				void addEdge(
					Edge& edge
				);



				//////////////////////////////////////////////////////////////////////////
				/// Removes an edge from the path.
				/// @param edge The edge to remove
				/// @author Hugues Romain
				/// @since 3.2.0
				/// @date 2010
				/// @pre no existing edge in the path must have the same rank than the rank of the
				/// edge to add
				//////////////////////////////////////////////////////////////////////////
				/// The rank of the following edges of the path are not updated.
				void removeEdge(
					Edge& edge
				);



				/** Adds a service to a path.
					@param service Service to add
					@param ensureLineTheory
					@author Hugues Romain
					@date 2007
					@todo Update dates of the path
				*/
				void addService(
					Service& service,
					bool ensureLineTheory
				);



				//////////////////////////////////////////////////////////////////////////
				/// Removes a service from the path.
				/// @param service the service to remove
				/// @author Hugues Romain
				void removeService(
					Service& service
				);


				ServiceSet getAllServices() const;



				//////////////////////////////////////////////////////////////////////////
				/// Merges two paths.
				/// @param other the path to add at the end of the current object
				/// Actions :
				///  - verify if the two paths can be merged (the second one must begin
				///    where the current one ends, and the two paths must belong to the
				///    same PathGroup)
				///  - shift the metric offset in the second path
				///  - change the pointers
				///  - delete the second path in the PathGroup
				/// @author Hugues Romain
				void merge(Path& other);



				//////////////////////////////////////////////////////////////////////////
				/// Reset the schedules indexes.
				/// Use this method when schedules have changed
				/// @param RTDataOnly if true only the real time indexes are reseted
				/// @author Hugues Romain
				void markScheduleIndexesUpdateNeeded(
					const ChronologicalServicesCollection& collection,
					bool RTDataOnly
				) const;

				
				
				//////////////////////////////////////////////////////////////////////////
				/// Reset all the schedules indexes.
				/// Use this method when schedules have changed
				/// @param RTDataOnly if true only the real time indexes are reseted
				/// @author Hugues Romain
				void markAllScheduleIndexesUpdateNeeded(
					bool RTDataOnly
				) const;
			//@}
		};
}	}

#endif
