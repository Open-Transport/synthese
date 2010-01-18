
/** Journey class header.
	@file Journey.h

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

#ifndef SYNTHESE_ROUTEPLANNER_JOURNEY_H
#define SYNTHESE_ROUTEPLANNER_JOURNEY_H

#include <deque>
#include <boost/logic/tribool.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>

#include "ServiceUse.h"
#include "SquareDistance.h"
#include "DateTime.h"
#include "GraphTypes.h"

namespace synthese
{
	namespace graph
	{
		class Edge;
		class VertexAccessMap;

		/** Journey class.
			@ingroup m35
		*/
		class Journey
		{
		public:
			typedef unsigned int MinSpeed;
			typedef std::deque<ServiceUse>	ServiceUses;

		private:
			typedef const ServiceUse& (Journey::*ServiceUseGetter) () const;
			typedef const Edge* (Journey::*EdgeGetter) () const;
			typedef time::DateTime (Journey::*DateTimeGetter) () const;
			typedef void (Journey::*JourneyPusher) (const Journey& journey);
			typedef void (Journey::*ServiceUsePusher) (const ServiceUse& serviceUse);

			//! @name Content
			//@{
				AccessDirection						_method;
				ServiceUses							_journeyLegs;
				boost::posix_time::time_duration	_startApproachDuration;
				boost::posix_time::time_duration	_endApproachDuration;
			//@}

			//! @name Route planning data
			//@{
				bool						_endReached;
				geometry::SquareDistance	_squareDistanceToEnd;
				MinSpeed					_minSpeedToEnd;
				int							_score;
			//@}

			//! @name Query cache
			//@{
				mutable int		_continuousServiceRange;
				boost::posix_time::time_duration	_effectiveDuration;
				int				_transportConnectionCount;
				double			_distance;
			//@}
			
			//! @name Oriented operators
			//@{
				time::DateTime::ComparisonOperator	_bestTimeStrictOperator;
				ServiceUseGetter					_endServiceUseGetter;
				ServiceUseGetter					_beginServiceUseGetter;
				EdgeGetter							_endEdgeGetter;
				EdgeGetter							_beginEdgeGetter;
				DateTimeGetter						_endDateTimeGetter;
				DateTimeGetter						_beginDateTimeGetter;
			//@}

				void _setMethod(AccessDirection method);
				void _prepend(const ServiceUse& leg);
				void _append(const ServiceUse& leg);
				void _prepend (const Journey& journey);
				void _append (const Journey& journey);


		 public:
			Journey();
			Journey(const Journey& journey, const ServiceUse& serviceUse);
			Journey(const Journey& journey1, const Journey& journey2);
			~Journey();


			//! @name Getters
			//@{
				/** Journey legs getter.
					@return const JourneyLegs& The journey legs
					@author Hugues Romain
					@date 2007					
				*/
				const ServiceUses& getServiceUses() const;
				ServiceUses& getServiceUses();

				AccessDirection getMethod() const;

				/** Returns the effective amount of time spent
					travelling, excluding tranfer delays.
				*/
				boost::posix_time::time_duration getEffectiveDuration () const;
	
				/** Continuous service range of this journey.
					@return Range duration in minutes, or 0 if unique service.
				*/
				int			getContinuousServiceRange () const;
				MinSpeed	getMinSpeedToEnd() const;
				bool		getEndReached() const;
				const time::DateTime::ComparisonOperator& getBestTimeStrictOperator() const;
				int			getScore()	const;
				boost::posix_time::time_duration getStartApproachDuration()	const;
				boost::posix_time::time_duration getEndApproachDuration()	const;
			//@}

			//! @name Setters
			//@{
				void setContinuousServiceRange (int continuousServiceRange);
				geometry::SquareDistance getSquareDistanceToEnd() const;
			//@}

			//! @name Orientation relative methods
			//@{
				const ServiceUse& getEndServiceUse() const;
				const ServiceUse& getStartServiceUse() const;
				void setStartApproachDuration(boost::posix_time::time_duration duration);
			//@}

			//! @name Query methods
			//@{
				bool empty()	const;
				std::size_t size() const;
				
				const ServiceUse& getJourneyLeg (std::size_t index) const;
				const ServiceUse& getFirstJourneyLeg () const;
				const ServiceUse& getLastJourneyLeg () const;
				
				const Edge* getOrigin() const;
				const Edge* getDestination() const;

				const Edge* getEndEdge() const;
				time::DateTime getEndTime() const;
				time::DateTime getBeginTime() const;

				time::DateTime getDepartureTime () const;
				time::DateTime getArrivalTime () const;

				boost::posix_time::time_duration getDuration () const;
				double getDistance () const;
		
		
				/** Detects max alarm level in journey.
			      
					For each journey leg, 4 cases are possible :
				- Alert on origin
				- Service with reservation rule
				- Service alert
				- Alert on destination
			      
				@return Maximum journey alert level.
				*/
				int getMaxAlarmLevel () const;

				/** Comparison between journeys doing the same relation..
					@param other Journey to compare with
					@return true if the current journey is a best choice than the other one
					@warning Journey objects must have same method
				*/
				bool isBestThan(const Journey& other) const;


				boost::logic::tribool	getReservationCompliance() const;
				time::DateTime			getReservationDeadLine() const;
			//@}



			//! @name Update methods
			//@{
				void clear ();

				void shift(
					boost::posix_time::time_duration duration,
					int continuousServiceRange = UNKNOWN_VALUE
				);
				void reverse();

				//////////////////////////////////////////////////////////////////////////
				/// Sets the informations about the position of the journey as a route
				/// planning result.
				/// @param endIsReached indicates that the journey has reached the goal of
				///		a route planning.
				/// @param goal the goal vertex access map
				/// @param bestTimeAtGoal the best time found for an other journey to reach
				///		the goal
				/// Sets :
				///		- route planning informations
				///		- end arrival time
				void setRoutePlanningInformations(
					bool endIsReached,
					const VertexAccessMap& goal,
					const time::DateTime& bestTimeAtGoal
				);

				void setMinSpeedToEnd(
					const time::DateTime& bestTimeAtGoal
				);
			//@}

//			Journey& operator = (const Journey& ref);


		};


	}
}

#endif
