
/** Path class header.
	@file Path.h

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

#ifndef SYNTHESE_ENV_PATH_H
#define SYNTHESE_ENV_PATH_H

#include "Registrable.h"
#include "RuleUser.h"

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <vector>
#include <set>

namespace geos
{
	namespace geom
	{
		class Coordinate;
	}
}

namespace synthese
{
	namespace graph
	{
		class Service;
		class Edge;
		class PathGroup;
		class PathClass;

		struct cmpService
		{
		    bool operator() (const Service* s1, const Service* s2) const;
		};

		typedef std::set<Service*, cmpService> ServiceSet;

		/** Path abstract base class.

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
		class Path
		:	public RuleUser,
			public virtual util::Registrable
		{
		public:
			typedef std::vector<Edge*> Edges;

		protected:
			PathGroup*		_pathGroup;	//!< Up link : path group
			PathClass*		_pathClass;	//!< Up link : path class
			Edges			_edges; 	//!< Down link 1 : edges
			ServiceSet		_services;	//!< Down link 2 : services
			
			/** Constructor.
			*/
			Path();
			
		public:

			virtual ~Path ();


			//! @name Getters
			//@{
				const ServiceSet&	getServices()	const { return _services; }
				const Edges&		getEdges()		const { return _edges; }
				PathClass*			getPathClass()	const { return _pathClass; }
			//@}

			//! @name Setters
			//@{
			//@}

			//! @name Services.
			//@{
				virtual const RuleUser* _getParentRuleUser() const;

				const Edge*					getEdge (int index) const;

				Edge*	getLastEdge()	const;

				virtual bool isRoad() const;
				virtual bool isPedestrianMode() const = 0;

				/** Gets all the geographical points linked by the path
					between two of its edges. If no from/to edge
					index is provided, all the edges are considered.
					@param fromEdgeIndex 
					@param toEdgeIndex 

					This includes :
					- vertices (address/physical stops)
					- via points
				*/
				std::vector<const geos::geom::Coordinate*> getPoints(
					int fromEdgeIndex = 0,
					int toEdgeIndex = -1
				) const;

				
				virtual bool isActive(const boost::gregorian::date& date) const = 0;



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
			//@}
			
			//! @name Modifiers.
			//@{
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

					The method is virtual to avoid subclasses to have a different behavior than 
					the simple add to the services list (see JourneyPatternCopy).
					
					@todo Update dates of the path
				*/
				virtual void addService (Service* service, bool ensureLineTheory);

				void removeService (Service* service);



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
				void merge(Path& other);

		private:

				void markScheduleIndexesUpdateNeeded ();

			//@}
		    
		};
	}
}

#endif 	    
