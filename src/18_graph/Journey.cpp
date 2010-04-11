
/** Journey class implementation.
	@file Journey.cpp

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

#include "Journey.h"
#include "UseRule.h"
#include "Path.h"
#include "Service.h"
#include "Vertex.h"
#include "Edge.h"
#include "VertexAccessMap.h"
#include "Hub.h"

#include "01_util/Constants.h"

#include <limits>
#include <boost/foreach.hpp>

#undef max

using namespace boost;
using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace geometry;

	namespace graph
	{

		Journey::Journey(
		):	_method(UNDEFINED_DIRECTION),
			_startApproachDuration(posix_time::minutes(0)),
			_endApproachDuration(posix_time::minutes(0)),
			_endReached(false),
			_distanceToEnd(UNKNOWN_VALUE),
			_similarity(logic::indeterminate),
			_continuousServiceRange (posix_time::not_a_date_time),
			_effectiveDuration(posix_time::seconds(0)),
			_transportConnectionCount (0),
			_distance (0)
		{
		}



		Journey::Journey(
			const Journey& journey,
			const ServiceUse& serviceUse
		):	_startApproachDuration(journey._startApproachDuration),
			_endApproachDuration(posix_time::minutes(0)),
			_endReached(false),
			_distanceToEnd(UNKNOWN_VALUE),
			_similarity(journey._similarity == false ? false : indeterminate),
			_continuousServiceRange(
				journey.getContinuousServiceRange().is_not_a_date_time() ||	
				journey.getContinuousServiceRange() > serviceUse.getServiceRange() ?
				serviceUse.getServiceRange() :
				journey.getContinuousServiceRange()
			),
			_effectiveDuration(journey._effectiveDuration + serviceUse.getDuration()),
			_transportConnectionCount(
				serviceUse.getEdge()->getParentPath()->isRoad() ?
				journey._transportConnectionCount :
				journey._transportConnectionCount + 1
			),
			_distance(journey._distance + serviceUse.getDistance())
		{
			assert(journey._method == UNDEFINED_DIRECTION || journey._method == serviceUse.getMethod());

			_setMethod(serviceUse.getMethod());
			_append(journey);
			if(_method == DEPARTURE_TO_ARRIVAL)
			{
				_append(serviceUse);
			}
			else
			{
				_prepend(serviceUse);
			}
		}



		Journey::Journey(
			const Journey& journey1,
			const Journey& journey2
		):	_startApproachDuration(journey1._startApproachDuration),
			_endApproachDuration(journey2._endApproachDuration),
			_endReached(journey2._endReached),
			_distanceToEnd(journey2._distanceToEnd),
			_similarity(journey1._similarity == false ? false : logic::indeterminate),
			_continuousServiceRange(
				journey1.getContinuousServiceRange().is_not_a_date_time() ||
				journey1.getContinuousServiceRange() > journey2.getContinuousServiceRange() ?
				journey2.getContinuousServiceRange() :
				journey1.getContinuousServiceRange()
			),
			_effectiveDuration(journey1._effectiveDuration + journey2._effectiveDuration),
			_transportConnectionCount(
				journey1._transportConnectionCount +
				journey2._transportConnectionCount + 1
			),
			_distance(journey1._distance + journey2._distance)
		{
			assert(journey1._method == UNDEFINED_DIRECTION || journey2._method == UNDEFINED_DIRECTION || journey1._method == journey2._method);

			_setMethod(journey1._method == UNDEFINED_DIRECTION ? journey2._method : journey1._method);
			_append(journey1);
			if(_method == DEPARTURE_TO_ARRIVAL)
			{
				_append(journey2);
			}
			else
			{
				_prepend(journey2);
			}
		}



		Journey::~Journey ()
		{
		}



		size_t Journey::size(
		) const	{
			return _journeyLegs.size();
		}




		const ServiceUse& Journey::getJourneyLeg(
			size_t index
		) const	{
			assert(!empty());

			return _journeyLegs.at(index);
		}




		const ServiceUse& Journey::getFirstJourneyLeg () const
		{
			assert(!empty());

			return *_journeyLegs.begin();
		}



		const ServiceUse& Journey::getLastJourneyLeg () const
		{
			assert(!empty());

			return *_journeyLegs.rbegin();
		}



		const Edge* Journey::getOrigin(
		) const {
			assert(!empty());

			return getFirstJourneyLeg ().getDepartureEdge();
		}



		const Edge* Journey::getDestination(
		) const {
			assert(!empty());

			return getLastJourneyLeg ().getArrivalEdge();
		}



		boost::posix_time::ptime Journey::getDepartureTime () const
		{
			boost::posix_time::ptime d(getFirstJourneyLeg ().getDepartureDateTime());
			if (d.is_not_a_date_time())
				return d;
			d -= (_method == DEPARTURE_TO_ARRIVAL) ? _startApproachDuration : _endApproachDuration;
			return d;
		}



		ptime Journey::getArrivalTime () const
		{
			ptime d(getLastJourneyLeg ().getArrivalDateTime());
			if (d.is_not_a_date_time())
				return d;
			d += (_method == DEPARTURE_TO_ARRIVAL) ? _endApproachDuration : _startApproachDuration;
			return d;
		}



		void Journey::_prepend(
			const ServiceUse& leg
		){
			_journeyLegs.push_front(leg);
		}



		void Journey::_append(
			const ServiceUse& leg
		){
			_journeyLegs.push_back(leg);
		}



		void Journey::_prepend(
			const Journey& journey
		){
			for(ServiceUses::const_reverse_iterator it(journey._journeyLegs.rbegin());
				it != journey._journeyLegs.rend();
				++it
			)	_prepend(*it);
		}



		void Journey::_append(
			const Journey& journey
		){
			for(ServiceUses::const_iterator it(journey._journeyLegs.begin());
				it != journey._journeyLegs.end();
				++it
			)	_append(*it);
		}



		posix_time::time_duration Journey::getDuration () const
		{
			if (getDepartureTime().is_not_a_date_time() ||
				getArrivalTime ().is_not_a_date_time()
			) return posix_time::time_duration(not_a_date_time);
		    
			return getArrivalTime() - getDepartureTime();
		}



		posix_time::time_duration Journey::getContinuousServiceRange(
		) const	{
			if (_continuousServiceRange.is_not_a_date_time())
			{
				BOOST_FOREACH(const ServiceUse& leg, _journeyLegs)
				{
					if(	_continuousServiceRange.is_not_a_date_time() ||
						leg.getServiceRange() < _continuousServiceRange
					){
						_continuousServiceRange = leg.getServiceRange();
					}
					if (_continuousServiceRange.total_seconds() == 0) break;
				}
			}
			return _continuousServiceRange;
		}



		void Journey::setContinuousServiceRange(
			posix_time::time_duration continuousServiceRange
		){
			_continuousServiceRange = continuousServiceRange;
		}



		void Journey::clear(
		){
			_continuousServiceRange = posix_time::not_a_date_time;
			_effectiveDuration = posix_time::seconds(0);
			_transportConnectionCount = 0;
			_distance = 0;
			_endApproachDuration = posix_time::seconds(0);
			_startApproachDuration = posix_time::seconds(0);
			_endReached = false;
			_journeyLegs.clear();
			_method = UNDEFINED_DIRECTION;
		}


		const Journey::ServiceUses& Journey::getServiceUses() const
		{
			return _journeyLegs;
		}



		Journey::ServiceUses& Journey::getServiceUses()
		{
			return _journeyLegs;
		}

		posix_time::time_duration Journey::getEffectiveDuration() const
		{
			return _effectiveDuration;
		}

		double Journey::getDistance() const
		{
			return _distance;
		}

		bool Journey::isBestThan( const Journey& other) const
		{
			//! <li>An empty journey cannot be superior to another</li> 
			if (empty())
				return false;

			//! <li>A populated journey is superior to an empty journey</li> 
			if (other.empty())
				return true;

			//! <li>Time comparison</li>
			ptime currentsTime = getEndTime();
			ptime othersTime = other.getEndTime();
			if (currentsTime != othersTime)
			{
				return _method == DEPARTURE_TO_ARRIVAL ? currentsTime < othersTime : othersTime < currentsTime;
			}

			/** </ul><p>Comparison between journey of same duration.</p><ul>
			*/
			/** <li>A journey with less approach duration is best</li> */
			posix_time::time_duration pedestrianDuration1(getStartApproachDuration() + getEndApproachDuration());
			posix_time::time_duration pedestrianDuration2(other.getStartApproachDuration() + other.getEndApproachDuration());

			if (pedestrianDuration1 != pedestrianDuration2)
			{
				return pedestrianDuration1 < pedestrianDuration2;
			}


			/** <li>A journey without compulsor reservation is best</li> */
			if (getReservationCompliance() == true && other.getReservationCompliance() != true)
				return false;
			if (other.getReservationCompliance() == true && getReservationCompliance() != true)
				return true;

			/** <li>A shorter journey is best</li> */
			if (getDuration () != other.getDuration ())
				return getDuration() < other.getDuration();

			//! <li>Less transport connection count is best</li> 
			if (_transportConnectionCount != other._transportConnectionCount)
				return _transportConnectionCount < other._transportConnectionCount;

			//! <li>Un trajet où l'on circule moins longtemps est supérieur à celui-ci (plus de marge de fiabilité pour les correspondaces)</li>
			if (getEffectiveDuration () != other.getEffectiveDuration ())
				return getEffectiveDuration () < other.getEffectiveDuration ();

			return false;
		}



		bool Journey::empty() const
		{
			return _journeyLegs.empty();
		}



		void Journey::shift(
			posix_time::time_duration duration,
			posix_time::time_duration continuousServiceRange 
		){
			for(ServiceUses::iterator it(_journeyLegs.begin()); it != _journeyLegs.end(); ++it)
			{
				it->shift(duration);
			}
			_continuousServiceRange = continuousServiceRange.is_not_a_date_time() ? _continuousServiceRange - duration : continuousServiceRange;
		}



		void Journey::setStartApproachDuration(boost::posix_time::time_duration duration )
		{
			_startApproachDuration = duration;
		}

		const ServiceUse& Journey::getEndServiceUse() const
		{
			assert(_method != UNDEFINED_DIRECTION);

			return (this->*_endServiceUseGetter)();
		}

		const ServiceUse& Journey::getStartServiceUse() const
		{
			assert(_method != UNDEFINED_DIRECTION);

			return (this->*_beginServiceUseGetter)();
		}

		void Journey::reverse()
		{
			if(_method == UNDEFINED_DIRECTION) return;

			boost::posix_time::time_duration duration(_startApproachDuration);
			_startApproachDuration = _endApproachDuration;
			_endApproachDuration = duration;

			for (ServiceUses::iterator it(_journeyLegs.begin()); it != _journeyLegs.end(); ++it)
			{
				assert(it->getMethod() == _method);
				it->reverse();
			}

			_setMethod((_method == DEPARTURE_TO_ARRIVAL) ? ARRIVAL_TO_DEPARTURE : DEPARTURE_TO_ARRIVAL);
		}

		AccessDirection Journey::getMethod() const
		{
			return _method;
		}



		const Edge* Journey::getEndEdge() const
		{
			assert(_method != UNDEFINED_DIRECTION);

			return (this->*_endEdgeGetter)();
		}

		ptime Journey::getEndTime() const
		{
			assert(_method != UNDEFINED_DIRECTION);

			return (this->*_endDateTimeGetter)();
		}



		ptime Journey::getBeginTime() const
		{
			assert(_method != UNDEFINED_DIRECTION);

			return (this->*_beginDateTimeGetter)();
		}



		Journey::Distance Journey::getDistanceToEnd() const
		{
			return _distanceToEnd;
		}

		bool Journey::getEndReached() const
		{
			return _endReached;
		}


		void Journey::_setMethod( AccessDirection method )
		{
			_method = method;

			if(method == UNDEFINED_DIRECTION) return;

			if (_method == DEPARTURE_TO_ARRIVAL)
			{
				_endServiceUseGetter = &Journey::getLastJourneyLeg;
				_beginServiceUseGetter = &Journey::getFirstJourneyLeg;
				_endEdgeGetter = &Journey::getDestination;
				_beginEdgeGetter = &Journey::getOrigin;
				_endDateTimeGetter = &Journey::getArrivalTime;
				_beginDateTimeGetter = &Journey::getDepartureTime;
			}
			else
			{
				_endServiceUseGetter = &Journey::getFirstJourneyLeg;
				_beginServiceUseGetter = &Journey::getLastJourneyLeg;
				_endEdgeGetter = &Journey::getOrigin;
				_beginEdgeGetter = &Journey::getDestination;
				_endDateTimeGetter = &Journey::getDepartureTime;
				_beginDateTimeGetter = &Journey::getArrivalTime;
			}
		}

		Journey::Score Journey::getScore() const
		{
			return _score;
		}



		boost::logic::tribool Journey::getReservationCompliance() const
		{
			boost::logic::tribool result(false);
			BOOST_FOREACH(const ServiceUse& su, _journeyLegs)
			{
				const UseRule::ReservationAvailabilityType& resa(
					su.getUseRule()->getReservationAvailability(su)
				);
				if(resa == UseRule::RESERVATION_COMPULSORY_POSSIBLE)
					return true;
				if(resa == UseRule::RESERVATION_OPTIONAL_POSSIBLE)
					result = boost::logic::indeterminate;
			}
			return result;
		}

		ptime Journey::getReservationDeadLine() const
		{
			ptime result(not_a_date_time);
			boost::logic::tribool compliance(getReservationCompliance());
			BOOST_FOREACH(const ServiceUse& su, _journeyLegs)
			{
				const UseRule::ReservationAvailabilityType& resa(
					su.getUseRule()->getReservationAvailability(su)
				);
				if(	(	boost::logic::indeterminate(compliance) &&
						resa == UseRule::RESERVATION_OPTIONAL_POSSIBLE
					)||(compliance == true &&
						resa == UseRule::RESERVATION_COMPULSORY_POSSIBLE
				)	){
					ptime deadLine(su.getReservationDeadLine());
					if (result.is_not_a_date_time() || deadLine < result)
						result = deadLine;
				}
			}
			return result;
		}



		posix_time::time_duration Journey::getStartApproachDuration() const
		{
			return _startApproachDuration;
		}



		posix_time::time_duration Journey::getEndApproachDuration() const
		{
			return _endApproachDuration;
		}



		void Journey::setEndApproachDuration( boost::posix_time::time_duration duration )
		{
			_endApproachDuration = duration;
		}



		void Journey::setEndIsReached( bool value )
		{
			_endReached = value;
		}



		void Journey::setDistanceToEnd(Distance value )
		{
			_distanceToEnd = value;
		}



		void Journey::setScore( Score value )
		{
			_score = value;
		}



		boost::logic::tribool Journey::getSimilarity() const
		{
			return _similarity;
		}



		void Journey::setSimilarity( boost::logic::tribool value )
		{
			_similarity = value;
		}
	}
}
