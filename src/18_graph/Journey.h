
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
			typedef std::deque<ServiceUse>	ServiceUses;
			typedef unsigned int Score;
			typedef unsigned int Distance;

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
				Distance					_distanceToEnd;
				Score						_score;
				boost::logic::tribool		_similarity;		
			//@}

			//! @name Query cache
			//@{
				mutable boost::posix_time::time_duration	_continuousServiceRange;
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
				boost::posix_time::time_duration			getContinuousServiceRange () const;
				bool		getEndReached() const;
				const time::DateTime::ComparisonOperator& getBestTimeStrictOperator() const;
				Score			getScore()	const;
				boost::posix_time::time_duration getStartApproachDuration()	const;
				boost::posix_time::time_duration getEndApproachDuration()	const;
				Distance getDistanceToEnd() const;
				boost::logic::tribool getSimilarity() const;
			//@}

			//! @name Setters
			//@{
				void setContinuousServiceRange (boost::posix_time::time_duration continuousServiceRange);
				void setStartApproachDuration(boost::posix_time::time_duration duration);
				void setEndApproachDuration(boost::posix_time::time_duration duration);
				void setEndIsReached(bool value);
				void setDistanceToEnd(Distance value);
				void setScore(Score value);
				void setSimilarity(boost::logic::tribool value);
			//@}

			//! @name Orientation relative methods
			//@{
				const ServiceUse& getEndServiceUse() const;
				const ServiceUse& getStartServiceUse() const;
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
					boost::posix_time::time_duration continuousServiceRange = boost::posix_time::not_a_date_time
				);
				void reverse();
			//@}

//			Journey& operator = (const Journey& ref);


		};


	}
}

#endif
