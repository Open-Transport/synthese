
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

#include <vector>
#include <set>

#include "Registrable.h"
#include "Calendar.h"
#include "RuleUser.h"

namespace synthese
{
	namespace time
	{
		class DateTime;
	}

	namespace geometry
	{
		class Point2D;
	}
	
	namespace graph
	{
		class Service;
		class Edge;
		class PathGroup;

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
			public time::Calendar,
			public virtual util::Registrable
		{
		public:
			typedef std::vector<Edge*> Edges;

		protected:
			PathGroup*		_pathGroup;	//!< Up link : path group
			Edges			_edges; 	//!< Down link 1 : edges
			ServiceSet		_services;	//!< Down link 2 : services
			
			bool			_allDays;	//!< A permanent service is present : the calendar is ignored

			/** Constructor.
			*/
			Path();
			
			virtual const RuleUser* _getParentRuleUser() const;
			
		public:

			virtual ~Path ();


			//! @name Getters
			//@{
				const ServiceSet&	getServices()	const;
				const Edges&		getEdges()		const;
				bool				getAllDays()	const;
			//@}

			//! @name Query methods.
			//@{
				const Service*				getService (int serviceIndex) const;

				const Edge*					getEdge (int index) const;

				virtual bool isRoad () const = 0;
				virtual bool isLine () const = 0;
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
				std::vector<const geometry::Point2D*> getPoints(
					int fromEdgeIndex = 0,
					int toEdgeIndex = -1
				) const;

				Edge*	getLastEdge()	const;
				
				virtual bool isActive(const boost::gregorian::date& date) const;
			//@}
			
			//! @name Update methods.
			//@{

				/** Adds edge at the end of the path.
					@param edge The edge to add
					@param autoShift Shift the following edges if an edge with the same rank already exists, else throw an Exception

					All the pointer links necessary to the graph exploration are created :
						- the links between edges (describing the path)

					The order of addEdge calls can be random : the rankinpath attribute of edge
					is used to insert the new object at the good position.
				*/
				void addEdge(
					Edge* edge,
					bool autoShift = false
				);



				/** Adds a service to a path.
					@param service Service to add
					@param ensureLineTheory
					@author Hugues Romain
					@date 2007

					The method is virtual to avoid subclasses to have a different behavior than 
					the simple add to the services list (see SubLine).
					
					@todo Update dates of the path
				*/
				virtual void addService (Service* service, bool ensureLineTheory);

				void removeService (Service* service);

				void setAllDays(bool value);

		private:

				void markScheduleIndexesUpdateNeeded ();

			//@}
		    
		};
	}
}

#endif 	    
