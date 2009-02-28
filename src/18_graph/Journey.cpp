
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
#include "DateTime.h"

#include "01_util/Constants.h"

#include <boost/foreach.hpp>

using namespace boost;

namespace synthese
{
	using namespace time;
	using namespace geometry;

	namespace graph
	{

		Journey::Journey(
			AccessDirection method
		):	_continuousServiceRange (UNKNOWN_VALUE)
			, _effectiveDuration (0)
			, _transportConnectionCount (0)
			, _distance (0)
			, _startApproachDuration(0)
			, _endApproachDuration(0)
			, _endReached(false)
			, _squareDistanceToEnd(UNKNOWN_VALUE)
			, _minSpeedToEnd(UNKNOWN_VALUE)
		{
			_setMethod(method);
		}
		


		Journey::~Journey ()
		{
		}



		int 
		Journey::getJourneyLegCount () const
		{
			return static_cast<int>(_journeyLegs.size());
		}




		const ServiceUse& Journey::getJourneyLeg (int index) const
		{
			return _journeyLegs.at (index);
		}




		const ServiceUse& Journey::getFirstJourneyLeg () const
		{
			return *_journeyLegs.begin();
		}



		const ServiceUse& Journey::getLastJourneyLeg () const
		{
			return *_journeyLegs.rbegin();
		}



		const Edge* Journey::getOrigin(
		) const {
			return getFirstJourneyLeg ().getDepartureEdge();
		}



		const Edge* Journey::getDestination(
		) const {
			return getLastJourneyLeg ().getArrivalEdge();
		}



		DateTime Journey::getDepartureTime () const
		{
			DateTime d(getFirstJourneyLeg ().getDepartureDateTime());
			if (d.isUnknown())
				return d;
			d -= (_method == DEPARTURE_TO_ARRIVAL) ? _startApproachDuration : _endApproachDuration;
			return d;
		}



		DateTime Journey::getArrivalTime () const
		{
			DateTime d(getLastJourneyLeg ().getArrivalDateTime());
			if (d.isUnknown())
				return d;
			d += (_method == DEPARTURE_TO_ARRIVAL) ? _endApproachDuration : _startApproachDuration;
			return d;
		}



		void Journey::_prependServiceUse(const ServiceUse& leg)
		{
			assert(leg.getMethod() == _method);

			_journeyLegs.push_front (leg);
			_effectiveDuration += leg.getDuration ();
			_distance += leg.getDistance ();
			if(	_continuousServiceRange == UNKNOWN_VALUE
			||	_continuousServiceRange > leg.getServiceRange()
			)	_continuousServiceRange = leg.getServiceRange();

			if (!leg.getEdge()->getParentPath()->isRoad())
				++_transportConnectionCount;
		}



		void 
		Journey::prepend (const Journey& journey)
		{
			assert(_method == journey._method);

			for(ServiceUses::const_reverse_iterator it(journey._journeyLegs.rbegin());
				it != journey._journeyLegs.rend();
				++it
			)	_prependServiceUse(*it);

			if (_method == DEPARTURE_TO_ARRIVAL)
				_startApproachDuration = journey._startApproachDuration;
			else
			{
				_endReached = journey._endReached;
				_endApproachDuration = journey._endApproachDuration;
			}
		}



		void Journey::_appendServiceUse(const ServiceUse& leg)
		{
			assert(leg.getMethod() == _method);

			_journeyLegs.push_back (leg);
			_effectiveDuration += leg.getDuration ();
			_distance += leg.getDistance ();
			if(	_continuousServiceRange == UNKNOWN_VALUE
			||	_continuousServiceRange > leg.getServiceRange()
			)	_continuousServiceRange = leg.getServiceRange();

			if (!leg.getEdge()->getParentPath()->isRoad ())
				++_transportConnectionCount;
		}




		void Journey::append (const Journey& journey)
		{
			assert(_method == journey._method);

			for(ServiceUses::const_iterator it(journey._journeyLegs.begin());
				it != journey._journeyLegs.end();
				++it
			)	_appendServiceUse(*it);

			if (_method == DEPARTURE_TO_ARRIVAL)
			{
				_endReached = journey._endReached;
				_endApproachDuration = journey._endApproachDuration;
			}
			else
				_startApproachDuration = journey._startApproachDuration;
		}



		int	Journey::getDuration () const
		{
			if (getDepartureTime ().getHour ().isUnknown () ||
			getArrivalTime ().getHour ().isUnknown ()) return UNKNOWN_VALUE;
		    
			return getArrivalTime () - getDepartureTime ();
		}



		int 
		Journey::getContinuousServiceRange () const
		{
			if (_continuousServiceRange == UNKNOWN_VALUE)
			{
				int continuousServiceRange = UNKNOWN_VALUE;
				for (ServiceUses::const_iterator it = _journeyLegs.begin();	it != _journeyLegs.end(); ++it)
				{
					const ServiceUse& leg(*it);
					if ( (continuousServiceRange == UNKNOWN_VALUE) ||
						(leg.getServiceRange() < continuousServiceRange) )
					{
						continuousServiceRange = leg.getServiceRange();
					}
					if (continuousServiceRange == 0) break;
				}
			}
			return _continuousServiceRange;
		}





		void 
		Journey::setContinuousServiceRange (int continuousServiceRange)
		{
			_continuousServiceRange = continuousServiceRange;
		}



		void 
		Journey::clear ()
		{
			_continuousServiceRange = UNKNOWN_VALUE;
			_effectiveDuration = 0;
			_transportConnectionCount = 0;
			_distance = 0;
			_endApproachDuration = 0;
			_startApproachDuration = 0;
			_endReached = false;
			_journeyLegs.clear();
		}


		const Journey::ServiceUses& Journey::getServiceUses() const
		{
			return _journeyLegs;
		}

		int Journey::getEffectiveDuration() const
		{
			return _effectiveDuration;
		}

		int Journey::getDistance() const
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
			DateTime currentsTime = getEndTime();

			DateTime othersTime = other.getEndTime();

			if ((currentsTime.*_bestTimeStrictOperator)(othersTime))
				return true;
			if (currentsTime != othersTime)
				return false;

			/** </ul><p>Comparison between journey of same duration.</p><ul>
				<li>A shorter journey is best</li>
			*/
			if (getReservationCompliance() == true && other.getReservationCompliance() != true)
				return false;
			if (other.getReservationCompliance() == true && getReservationCompliance() != true)
				return true;

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

// 		bool Journey::verifyAxisConstraints( const Axis* axis ) const
// 		{
// 			// Null Axis is allowed
// 			if (axis == NULL)
// 				return true;
//
// 			// Check if axis is allowed.
// 			if (!axis->isAllowed())
// 				return false;
// 			
// 			// Check if axis is free
// 			if (axis->isFree())
// 				return true;
// 
// 			// Check if current journey is empty
// 			if (_journeyLegs.empty())
// 				return true;
// 
// 			// Check axis against already followed axes
// 			for (ServiceUses::const_iterator it(_journeyLegs.begin()); it != _journeyLegs.end(); ++it)
// 			{
// 				if (it->getEdge()->getParentPath()->getAxis () == axis)
// 					return false;
// 			}
// 			return true;
// 		}

		void Journey::setEndApproachDuration(int duration)
		{
			_endApproachDuration = duration;
		}

		void Journey::shift( int duration, int continuousServiceRange /*= UNKNOWN_VALUE*/ )
		{
			for(ServiceUses::iterator it(_journeyLegs.begin()); it != _journeyLegs.end(); ++it)
			{
				it->shift(duration);
			}
			_continuousServiceRange = (continuousServiceRange == UNKNOWN_VALUE) ? _continuousServiceRange - duration : continuousServiceRange;
		}

		void Journey::setStartApproachDuration( int duration )
		{
			_startApproachDuration = duration;
		}

		const ServiceUse& Journey::getEndServiceUse() const
		{
			return (this->*_endServiceUseGetter)();
		}

		const ServiceUse& Journey::getStartServiceUse() const
		{
			return (this->*_beginServiceUseGetter)();
		}

		void Journey::reverse()
		{
			int duration(_startApproachDuration);
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

		void Journey::setEndReached( bool value )
		{
			_endReached = value;
		}

		void Journey::setSquareDistanceToEnd( const VertexAccessMap& vam )
		{
			if (_endReached)
				_squareDistanceToEnd = 0;
			else
				_squareDistanceToEnd.setFromPoints(vam.getIsobarycenter(), getEndEdge()->getFromVertex()->getPlace()->getPoint());
		}

		const Edge* Journey::getEndEdge() const
		{
			return (this->*_endEdgeGetter)();
		}

		DateTime Journey::getEndTime() const
		{
			return (this->*_endDateTimeGetter)();
		}

		void Journey::setMinSpeedToEnd( const DateTime& dateTime )
		{
			if (_endReached)
				_minSpeedToEnd = 0;
			else
			{
				assert((_method == DEPARTURE_TO_ARRIVAL) ? dateTime > getArrivalTime() : getDepartureTime() > dateTime);

				_minSpeedToEnd = (1000 * _squareDistanceToEnd.getSquareDistance()) / ((_method == DEPARTURE_TO_ARRIVAL) ? dateTime - getArrivalTime() : getDepartureTime() - dateTime);

				_score = _minSpeedToEnd;
				if (_score < 100)
					_score = 100;
				if (getEndEdge()->getFromVertex()->getPlace()->getScore())
					_score /= getEndEdge()->getFromVertex()->getPlace()->getScore();
			}
		}

		Journey::MinSpeed Journey::getMinSpeedToEnd() const
		{
			return _minSpeedToEnd;
		}

		SquareDistance Journey::getSquareDistanceToEnd() const
		{
			return _squareDistanceToEnd;
		}

		bool Journey::getEndReached() const
		{
			return _endReached;
		}

		const time::DateTime::ComparisonOperator& Journey::getBestTimeStrictOperator() const
		{
			return _bestTimeStrictOperator;
		}

		void Journey::_setMethod( AccessDirection method )
		{
			_method = method;
			if (_method == DEPARTURE_TO_ARRIVAL)
			{
				_bestTimeStrictOperator = &DateTime::operator<;
				_endServiceUseGetter = &Journey::getLastJourneyLeg;
				_beginServiceUseGetter = &Journey::getFirstJourneyLeg;
				_endEdgeGetter = &Journey::getDestination;
				_beginEdgeGetter = &Journey::getOrigin;
				_endDateTimeGetter = &Journey::getArrivalTime;
				_beginDateTimeGetter = &Journey::getDepartureTime;
				_serviceUsePusher = &Journey::_appendServiceUse;
				_journeyPusher = &Journey::append;
			}
			else
			{
				_bestTimeStrictOperator = &DateTime::operator>;
				_endServiceUseGetter = &Journey::getFirstJourneyLeg;
				_beginServiceUseGetter = &Journey::getLastJourneyLeg;
				_endEdgeGetter = &Journey::getOrigin;
				_beginEdgeGetter = &Journey::getDestination;
				_endDateTimeGetter = &Journey::getDepartureTime;
				_beginDateTimeGetter = &Journey::getArrivalTime;
				_serviceUsePusher = &Journey::_prependServiceUse;
				_journeyPusher = &Journey::prepend;
			}
		}

		int Journey::getScore() const
		{
			return _score;
		}

		boost::logic::tribool Journey::getReservationCompliance() const
		{
			boost::logic::tribool result(false);
			BOOST_FOREACH(const ServiceUse& su, _journeyLegs)
			{
				const UseRule& rule(su.getUseRule());
				if (rule.getReservationType() == UseRule::RESERVATION_COMPULSORY)
					return true;
				if (rule.getReservationType() == UseRule::RESERVATION_OPTIONAL)
					result = boost::logic::indeterminate;
			}
			return result;
		}

		time::DateTime Journey::getReservationDeadLine() const
		{
			DateTime result(TIME_UNKNOWN);
			boost::logic::tribool compliance(getReservationCompliance());
			BOOST_FOREACH(const ServiceUse& su, _journeyLegs)
			{
				const UseRule& rule(su.getUseRule());
				if(	(	boost::logic::indeterminate(compliance) &&
						rule.getReservationType() == UseRule::RESERVATION_OPTIONAL
					)||(compliance == true &&
						rule.getReservationType() == UseRule::RESERVATION_COMPULSORY
				)	){
					DateTime deadLine(su.getReservationDeadLine());
					if (result.isUnknown() || deadLine < result)
						result = deadLine;
				}
			}
			return result;
		}



		void Journey::push( const ServiceUse& leg )
		{
			(this->*_serviceUsePusher)(leg);
		}



		void Journey::push( const Journey& journey )
		{
			(this->*_journeyPusher)(journey);
		}



		int Journey::getStartApproachDuration() const
		{
			return _startApproachDuration;
		}



		int Journey::getEndApproroachDuration() const
		{
			return _endApproachDuration;
		}
	}
}
