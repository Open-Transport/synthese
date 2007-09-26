
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

#include "04_time/DateTime.h"
#include "04_time/Schedule.h"


namespace synthese
{
	namespace geometry
	{
		class Point2D;
	}

	namespace env
	{
		class Path;
		class Vertex;
		class AddressablePlace;
		class ConnectionPlace;
		class ServicePointer;

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

			@ingroup m15
		*/
		class Edge
		{
		private:

			bool _isDeparture;				//!< The departure from the vertex is allowed
			bool _isArrival;				//!< The arrival at the vertex is allowed

			const Path*	_parentPath;		//!< The path the edge belongs
			int			_rankInPath;		//!< Rank in path.

			const Edge* _nextInPath;		//!< Next edge in path.

			const Edge* _previousConnectionDeparture;			//!< Previous connection departure edge along path.
			const Edge* _previousDepartureForFineSteppingOnly;	//!< Previous departure edge with or without connection

			const Edge* _followingConnectionArrival;			//!< Next connection arrival edge along path.
			const Edge* _followingArrivalForFineSteppingOnly;	//!< Next arrival edge with or without connection

			std::vector<const geometry::Point2D*> _viaPoints;				//!< Intermediate points along the edge (for map drawing)

			mutable int _departureIndex[24];	//!< First line service index by departure hour of day
			mutable int _arrivalIndex[24];		//!< First line service index by arrival hour of day

			mutable bool _serviceIndexUpdateNeeded;

		protected:
			Edge (bool isDeparture = true, bool isArrival = true,
			const Path* parentPath = NULL, int rankInPath = UNKNOWN_VALUE);

		public:
			virtual ~Edge ();


			//! @name Setters
			//@{
				void	setIsArrival(bool value);
				void	setIsDeparture(bool value);
				void	setRankInPath(int value);
				void	setParentPath(const Path* path);
				void	setNextInPath (const Edge* nextInPath);
				void	setPreviousDeparture ( const Edge* previousDeparture);
				void	setPreviousConnectionDeparture( const Edge* previousConnectionDeparture);
				void	setPreviousDepartureForFineSteppingOnly ( const Edge* previousDeparture);
				void	setFollowingConnectionArrival( const Edge* followingConnectionArrival);
				void	setFollowingArrivalForFineSteppingOnly ( const Edge* followingArrival);
			//@}

			//! @name Getters
			//@{
				const Path* getParentPath () const;

				/** Returns this edge origin vertex.
				*/
				virtual const Vertex* getFromVertex () const = 0;

				/** Returns metric offset of this edge from
				parent path origin vertex.
				*/
				virtual double getMetricOffset () const = 0;


				/** Returns length of this edge, in meters. from
				*/
				double getLength () const;

				const Edge* getNextInPath () const;

				const Edge* getPreviousDeparture () const;

				const Edge* getPreviousConnectionDeparture () const;

				const Edge* getPreviousDepartureForFineSteppingOnly () const;

				const Edge* getFollowingConnectionArrival () const;
			    
				const Edge* getFollowingArrivalForFineSteppingOnly () const;
			    
				/** Gets intermediate points 
				* between this line stop and the next in path.
				*/
				const std::vector<const geometry::Point2D*>& getViaPoints () const;

				int getDepartureFromIndex (int hour) const;
				int getArrivalFromIndex (int hour) const;
			//@}


			//! @name Query methods
			//@{
				const AddressablePlace* getPlace () const;
				const ConnectionPlace* getConnectionPlace () const;
				const ConnectionPlace* getToConnectionPlace() const;

				int getRankInPath () const;

				bool isArrival () const;
				bool isDeparture () const;
			    
				bool isRunning( const synthese::time::DateTime& startMoment, 
						const synthese::time::DateTime& endMoment ) const;

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
					@param calculationMoment Calculation moment for reservation delay checking
					@param controlIfTheServiceIsReachable service selection method :
						- true : the result is a usable service : its departure time must be in the future, and the reservation rules must be followed
						- false : the result is a runnable service : if the reservation on it is compulsory, then there must bu at least one reservation for the service
					@param minNextServiceIndex First index to scan in the services list (optimization)
						- UNKNOWN_VALUE / -1 : start at the beginning of the list
					@return Found service instance index or -1 if none was found.
					@retval departureMoment Accurate departure moment. Meaningless if -1 returned.
				*/
				ServicePointer getNextService (
					time::DateTime departureMoment
					, const time::DateTime& maxDepartureMoment
					, const time::DateTime& calculationMoment
					, bool controlIfTheServiceIsReachable
					, int minNextServiceIndex = UNKNOWN_VALUE					
				) const;

			    
			 
				/** Provides previous arrival service number
					@param arrivalMoment Presence hour at arrival place
					@param minArrivalMoment Minimum arrival hour
					@param calculationMoment Calculation moment for reservation delay checking
					@param controlIfTheServiceIsReachable service selection method :
						- true : the result is a usable service : its departure time must be in the future, and the reservation rules must be followed
						- false : the result is a runnable service : if the reservation on it is compulsory, then there must bu at least one reservation for the service
					@param maxPreviousServiceIndex First index to scan in the services list (optimization)
						- UNKNOWN_VALUE / -1 : start at the end of the list
					@return Found service instance index or -1 if none was found.
					@retval arrivalMoment Accurate departure moment. Meaningless if -1 returned.
				*/
				ServicePointer getPreviousService(
					time::DateTime arrivalMoment
					, const time::DateTime& minArrivalMoment
					, const time::DateTime& calculationMoment
					, bool controlIfTheServiceIsReachable
					, int maxPreviousServiceIndex = UNKNOWN_VALUE
				) const;
			    
			//@}

			//! @name Update methods
			//@{
				void clearViaPoints ();
				void addViaPoint (const geometry::Point2D& viaPoint);
			    
				void markServiceIndexUpdateNeeded();
		private:
				
				void updateServiceIndex () const;
			//@}
	    
		};
	}
}

#endif 	    
