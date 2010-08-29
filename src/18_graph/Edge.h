
/** Edge class header.
	@file Edge.h

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

#ifndef SYNTHESE_ENV_EDGE_H
#define SYNTHESE_ENV_EDGE_H

#include <vector>

#include "01_util/Constants.h"
#include "Registrable.h"
#include "GraphTypes.h"
#include "Path.h"

#include <boost/optional.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>

namespace geos
{
	namespace geom
	{
		class LineString;
	}
}

namespace synthese
{
	namespace graph
	{
		class Path;
		class Vertex;
		class ServicePointer;
		class Hub;

		/** Edge abstract base class.

			An edge necessarily belongs to a path.

			For instance :
				- if the edge is a road chunk, its parent path is a road.
				- if the edge is a line stop, its parent path is a line.

			An edge holds links between one vertex (the "from" vertex),
			and several others edges sharing the same path.

			This way, a path can be traversed following different views :
				- departure/arrival view
				- departure/arrival connection view

			An edge is said to be :
				- a departure edge if it is possible to start a move from it;
				- an arrival edge if it is possible to end a move to it;
				- a passage edge if it can be considered both as a departure edge 
				and as an arrival edge.

			@ingroup m18
			@author Marc Jambert, Hugues Romain
		*/
		class Edge
			:	public virtual util::Registrable
		{
		public:
			template<class Iterator>
			class ServiceIndex
			{
			public:
				typedef Iterator Value;

			private:
				Value _value;
				Value _RTValue;

			public:
				Value get(bool RTData) const{ return RTData ? _RTValue : _value; }

				void set(
					bool RTData,
					Value value
				){
					if(RTData) _RTValue = value; else _value = value;
				}
			};

			typedef ServiceIndex<ServiceSet::const_iterator> DepartureServiceIndex;
			typedef ServiceIndex<ServiceSet::const_reverse_iterator> ArrivalServiceIndex;

			typedef std::vector<DepartureServiceIndex> DepartureServiceIndices;
			typedef std::vector<ArrivalServiceIndex> ArrivalServiceIndices;

		protected:
			Vertex*	_fromVertex;
			Path*		_parentPath;		//!< The path the edge belongs
			double			_metricOffset;		//!< Metric offset
		
		private:
			static const std::size_t INDICES_NUMBER;

			std::size_t _rankInPath;		//!< Rank in path.

			Edge* _previousConnectionDeparture;			//!< Previous connection departure edge along path.
			Edge* _previousDepartureForFineSteppingOnly;	//!< Previous departure edge with or without connection
			Edge* _followingConnectionArrival;			//!< Next connection arrival edge along path.
			Edge* _followingArrivalForFineSteppingOnly;	//!< Next arrival edge with or without connection

			boost::shared_ptr<geos::geom::LineString> _geometry;				//!< Intermediate points along the edge (for map drawing)

			mutable DepartureServiceIndices _departureIndex;	//!< First service index by departure hour of day
			mutable ArrivalServiceIndices _arrivalIndex;		//!< First service index by arrival hour of day

			mutable bool _serviceIndexUpdateNeeded;
			mutable bool _RTserviceIndexUpdateNeeded;

			/** Updates service indices.
				@param RTData indicates if real time or theoretical indices must be updated
				@author Hugues Romain
			*/
			void _updateServiceIndex(
				bool RTData
			) const;

			bool _getServiceIndexUpdateNeeded(
				bool RTData
			) const;

		protected:
			Edge(
				Path* parentPath = NULL,
				std::size_t rankInPath = 0,
				Vertex* fromVertex = NULL,
				double metricOffset = 0
			);

			
		public:
			virtual ~Edge ();

			//! @name Setters
			//@{
				void setRankInPath(std::size_t value) { _rankInPath = value; }
				void setParentPath(Path* path) { _parentPath = path; }
				void setPreviousConnectionDeparture(Edge* previousConnectionDeparture) { _previousConnectionDeparture = previousConnectionDeparture; }
				void setPreviousDepartureForFineSteppingOnly (Edge* previousDeparture) { _previousDepartureForFineSteppingOnly = previousDeparture; }
				void setFollowingConnectionArrival(Edge* followingConnectionArrival) {_followingConnectionArrival = followingConnectionArrival; }
				void setFollowingArrivalForFineSteppingOnly(Edge* followingArrival) { _followingArrivalForFineSteppingOnly = followingArrival; }
				void setMetricOffset (double metricOffset) { _metricOffset = metricOffset; }
				void setGeometry(boost::shared_ptr<geos::geom::LineString> value) { _geometry = value; }
				void setFromVertex(Vertex* value) { _fromVertex = value; }
			//@}

			//! @name Getters
			//@{
				Path* getParentPath () const { return _parentPath; }

				/** Returns this edge origin vertex.
				*/
				Vertex* getFromVertex () const { return _fromVertex; }

				/** Returns metric offset of this edge from
				parent path origin vertex.
				*/
				double getMetricOffset () const { return _metricOffset; }

				Edge* getPreviousConnectionDeparture () const { return _previousConnectionDeparture; }
				Edge* getPreviousDepartureForFineSteppingOnly () const { return _previousDepartureForFineSteppingOnly; }
				Edge* getFollowingConnectionArrival () const { return _followingConnectionArrival; }
				Edge* getFollowingArrivalForFineSteppingOnly () const { return _followingArrivalForFineSteppingOnly; }
			    
				const DepartureServiceIndices& getDepartureIndices() const { return _departureIndex; }
				const ArrivalServiceIndices& getArrivalIndices() const { return _arrivalIndex; }

				std::size_t getRankInPath () const { return _rankInPath; }

				boost::shared_ptr<geos::geom::LineString> getStoredGeometry() const { return _geometry; }
			//@}


			//! @name Query methods
			//@{
				virtual bool isDepartureAllowed() const = 0;
				virtual bool isArrivalAllowed() const = 0;
				
				bool isConnectingEdge() const;

				const Hub* getHub() const;
				
				DepartureServiceIndex::Value getDepartureFromIndex(
					bool RTData,
					size_t hour
				) const;

				ArrivalServiceIndex::Value getArrivalFromIndex(
					bool RTData,
					size_t hour
				) const;
				
				bool isArrival() const;
				bool isDeparture() const;



				//////////////////////////////////////////////////////////////////////////
				/// Gets the geometry of the edge.
				/// @return the geometry of the edge including starting and ending points (vertices)
				/// @author Hugues Romain
				/// @since 3.2.0
				/// @date 2010
				//////////////////////////////////////////////////////////////////////////
				/// A geometry is generated if no geometry is stored in the object :
				///	<ul>
				///		<li>If the edge is the last edge, the vertex point is returned</li>
				///		<li>Else a right line string between the vertex and the next one is returned</li>
				/// </ul>
				boost::shared_ptr<geos::geom::Geometry> getGeometry(
				) const;


//				int getBestRunTime (const Edge& other ) const;
			    
				/** Checks consistency of input schedules.
				@param edgeWithPreviousSchedule Previous edge with schedule
				@return true if no problem detected, false otherwise
				*/
//				bool checkSchedule (const Edge* edgeWithPreviousSchedule ) const;



				// TODO : initialisation des filtres de compliance de la ligne pour tenir compte
				// de ceux des services et eviter le parcour total de tous les services

			    
				/** Provides next departure service number (method 1)
					@param departureMoment Presence hour at departure place
					@param maxDepartureMoment Maximum departure hour
					@param controlIfTheServiceIsReachable service selection method :
						- true : the result is a usable service : its departure time must be in the future, and the reservation rules must be followed
						- false : the result is a runnable service : if the reservation on it is compulsory, then there must bu at least one reservation for the service
					@param minNextServiceIndex First index to scan in the services list (optimization)
						- undefined : start at the beginning of the list
					@return Found service instance index or -1 if none was found.
					@retval departureMoment Accurate departure moment. Meaningless if -1 returned.
					@retval minNextServiceIndex Index corresponding to the returned service
				*/
				ServicePointer getNextService (
					std::size_t userClassRank,
					boost::posix_time::ptime departureMoment,
					const boost::posix_time::ptime& maxDepartureMoment,
					bool controlIfTheServiceIsReachable,
					boost::optional<DepartureServiceIndex::Value>& minNextServiceIndex,
					bool inverted = false
				) const;

			    
			 
				/** Provides previous arrival service number
					@param arrivalMoment Presence hour at arrival place
					@param minArrivalMoment Minimum arrival hour
					@param controlIfTheServiceIsReachable service selection method :
						- true : the result is a usable service : its departure time must be in the future, and the reservation rules must be followed
						- false : the result is a runnable service : if the reservation on it is compulsory, then there must bu at least one reservation for the service
					@param maxPreviousServiceIndex First index to scan in the services list (optimization)
						- undefined : start at the end of the list
					@return Found service instance index or -1 if none was found.
					@retval arrivalMoment Accurate departure moment. Meaningless if -1 returned.
					@retval maxPreviousServiceIndex Index corresponding to the returned service
				*/
				ServicePointer getPreviousService(
					std::size_t userClassRank,
					boost::posix_time::ptime arrivalMoment,
					const boost::posix_time::ptime& minArrivalMoment,
					bool controlIfTheServiceIsReachable,
					boost::optional<ArrivalServiceIndex::Value>& maxPreviousServiceIndex,
					bool inverted = false
				) const;
			    
			//@}

			//! @name Update methods
			//@{
				void markServiceIndexUpdateNeeded(bool RTDataOnly) const;
			//@}
		};
	}
}

#endif
