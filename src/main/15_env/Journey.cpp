
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

#include "17_messages/SentAlarm.h"
#include "17_messages/Types.h"

#include "15_env/Journey.h"
#include "15_env/ReservationRuleComplyer.h"
#include "15_env/ReservationRule.h"
#include "15_env/Path.h"
#include "15_env/Service.h"
#include "15_env/ContinuousService.h"
#include "15_env/ConnectionPlace.h"
#include "15_env/Vertex.h"
#include "15_env/Edge.h"
#include "15_env/Axis.h"
#include "15_env/VertexAccessMap.h"

#include "04_time/DateTime.h"

#include "01_util/Constants.h"

using namespace boost;

namespace synthese
{
	using namespace messages;
	using namespace time;

	namespace env
	{

		Journey::Journey (AccessDirection method)
			: _continuousServiceRange (UNKNOWN_VALUE)
			, _effectiveDuration (0)
			, _transportConnectionCount (0)
			, _distance (0)
			, _method(method)
			, _startApproachDuration(0)
			, _endApproachDuration(0)
			, _endReached(false)
			, _squareDistanceToEnd(UNKNOWN_VALUE)
			, _minSpeedToEnd(UNKNOWN_VALUE)
		{
		}
		


		Journey::~Journey ()
		{
		}



		int 
		Journey::getJourneyLegCount () const
		{
			return (int) _journeyLegs.size ();
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



		const synthese::env::Edge* 
		Journey::getOrigin() const
		{
			return getFirstJourneyLeg ().getDepartureEdge();
		}



		const synthese::env::Edge* 
		Journey::getDestination() const
		{
			return getLastJourneyLeg ().getArrivalEdge();
		}



		DateTime Journey::getDepartureTime () const
		{
			DateTime d(getFirstJourneyLeg ().getDepartureDateTime());
			if (d.isUnknown())
				return d;
			d -= (_method == TO_DESTINATION) ? _startApproachDuration : _endApproachDuration;
			return d;
		}



		DateTime Journey::getArrivalTime () const
		{
			DateTime d(getLastJourneyLeg ().getArrivalDateTime());
			if (d.isUnknown())
				return d;
			d += (_method == TO_DESTINATION) ? _endApproachDuration : _startApproachDuration;
			return d;
		}


		void 
		Journey::prepend (const ServiceUse& leg)
		{
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

			for(JourneyLegs::const_reverse_iterator it(journey.getJourneyLegs().rbegin());
				it != journey.getJourneyLegs().rend();
				++it
			)	prepend(*it);

			if (_method == TO_DESTINATION)
				_startApproachDuration = journey._startApproachDuration;
			else
			{
				_endReached = journey._endReached;
				_endApproachDuration = journey._endApproachDuration;
			}
		}





		void 
		Journey::append (const ServiceUse& leg)
		{
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

			for(JourneyLegs::const_iterator it(journey.getJourneyLegs().begin());
				it != journey.getJourneyLegs().end();
				++it
			)	append(*it);

			if (_method == TO_DESTINATION)
			{
				_endReached = journey._endReached;
				_endApproachDuration = journey._endApproachDuration;
			}
			else
				_startApproachDuration = journey._startApproachDuration;
		}





		int 
		Journey::getMaxAlarmLevel () const
		{
			synthese::time::DateTime alarmStart, alarmStop, now;
			int maxAlarmLevel = 0;
		    
			for (JourneyLegs::const_iterator it(_journeyLegs.begin()); it != _journeyLegs.end(); ++it)
			{
				const ServiceUse& leg(*it);
				const Service* service(leg.getService());
				bool legIsConnection = (it < _journeyLegs.end() - 2);

					// -- Alarm on origin --
					// Alarm start = first departure
					// Alarm stop = last departure
					alarmStart = leg.getDepartureDateTime();
					alarmStop = alarmStart;
				if (service->isContinuous ()) 
					alarmStop += static_cast<const ContinuousService*>(service)->getRange ();
				
	/*				if ( leg->getOrigin ()->getFromVertex ()->getConnectionPlace ()
					 ->hasApplicableAlarm (alarmStart, alarmStop)
					 && maxAlarmLevel < leg->getOrigin()->getFromVertex ()->
					 getConnectionPlace ()->getAlarm ()->getLevel () )
						maxAlarmLevel = leg->getOrigin()->getFromVertex ()->getConnectionPlace ()->getAlarm ()->getLevel ();
	*/			
				
				// -- Compulsory reservation --
				now.updateDateTime ();
				if ( (service->getReservationRule ()->isCompliant() == true) &&
					 (service->isReservationPossible ( now, leg.getDepartureDateTime())) &&
					 (maxAlarmLevel < ALARM_LEVEL_WARNING) )
				{
						maxAlarmLevel = ALARM_LEVEL_WARNING;
				}

				// -- Possible reservation --
				now.updateDateTime();
				if ( (service->getReservationRule ()->isCompliant() == boost::logic::indeterminate) &&
					 (service->isReservationPossible ( now, leg.getDepartureDateTime())) &&
					 (maxAlarmLevel < ALARM_LEVEL_INFO) )
				{
						maxAlarmLevel = ALARM_LEVEL_INFO;
				}


					// -- Service alarm --
					// Alarm start = first departure
					// Alarm stop = last arrival
					alarmStart = leg.getDepartureDateTime();
					alarmStop = leg.getArrivalDateTime();
				if (service->isContinuous ()) 
					alarmStop += static_cast<const ContinuousService*>(service)->getRange ();

	/*				if ( (leg->getService ()->getPath ()->hasApplicableAlarm (alarmStart, alarmStop)) &&
					 (maxAlarmLevel < leg->getService ()->getPath ()->getAlarm ()->getLevel ()) )
				{
						maxAlarmLevel = leg->getService ()->getPath ()->getAlarm ()->getLevel ();
				}
	*/			
					// -- Alarm on arrival --
					// Alarm start = first arrival
					// Alarm stop = last arrival if connection, last arrival otherwise
					alarmStart = leg.getArrivalDateTime();
					alarmStop = alarmStart;
					if (legIsConnection)
				{
						alarmStop = (it+1)->getDepartureDateTime ();
				}

				if (service->isContinuous ()) 
					alarmStop += static_cast<const ContinuousService*>(service)->getRange ();

	/*				if ( (leg->getDestination ()->getFromVertex ()->getConnectionPlace ()->hasApplicableAlarm (alarmStart, alarmStop)) &&
						 (maxAlarmLevel < leg->getDestination()->getFromVertex ()->getConnectionPlace ()->getAlarm ()->getLevel ()) )
				{
					maxAlarmLevel = leg->getDestination()->getFromVertex ()->getConnectionPlace ()->getAlarm ()->getLevel ();
				}
	*/			}

				return maxAlarmLevel;
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
			if (_continuousServiceRange != UNKNOWN_VALUE) return _continuousServiceRange;

			int continuousServiceRange = UNKNOWN_VALUE;
			for (JourneyLegs::const_iterator it = _journeyLegs.begin();	it != _journeyLegs.end(); ++it)
			{
				const ServiceUse& leg(*it);
				if ( (continuousServiceRange == UNKNOWN_VALUE) ||
					 (leg.getServiceRange() < continuousServiceRange) )
				{
					continuousServiceRange = leg.getServiceRange();
				}
				if (continuousServiceRange == 0) break;
			}
			return continuousServiceRange;
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



/*		Journey& 
		Journey::operator= (const Journey& ref) 
		{
			clear ();
			_journeyLegs = ref._journeyLegs;
			_continuousServiceRange = ref._continuousServiceRange;
			_effectiveDuration = ref._effectiveDuration;
			_transportConnectionCount = ref._transportConnectionCount;
			_distance = ref._distance;
			_endApproachDuration = ref._endApproachDuration;
			_startApproachDuration = ref._startApproachDuration;
			return *this;
		}
		*/

		const JourneyLegs& Journey::getJourneyLegs() const
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

			bool betterTime = (_method == TO_DESTINATION) 
				? currentsTime < othersTime
				: currentsTime > othersTime; 

			if (betterTime)
				return true;
			if (currentsTime != othersTime)
				return false;

			/** </ul><p>Comparison between journey of same duration.</p><ul>
				<li>A shorter journey is best</li>
			*/
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

		bool Journey::verifyAxisConstraints( const Axis* axis ) const
		{
			// Null Axis is allowed
			if (axis == NULL)
				return true;

			// Check if axis is allowed.
			if (!axis->isAllowed())
				return false;
			
			// Check if axis is free
			if (axis->isFree())
				return true;

			// Check if current journey is empty
			if (_journeyLegs.empty())
				return true;

			// Check axis against already followed axes
			for (JourneyLegs::const_iterator it(_journeyLegs.begin()); it != _journeyLegs.end(); ++it)
			{
				if (it->getEdge()->getParentPath()->getAxis () == axis)
					return false;
			}
			return true;
		}

		void Journey::setEndApproachDuration(int duration)
		{
			_endApproachDuration = duration;
		}

		void Journey::shift( int duration, int continuousServiceRange /*= UNKNOWN_VALUE*/ )
		{
			for(JourneyLegs::iterator it(_journeyLegs.begin()); it != _journeyLegs.end(); ++it)
			{
				it->shift(duration);
			}
			_continuousServiceRange = (continuousServiceRange == UNKNOWN_VALUE) ? _continuousServiceRange - duration : continuousServiceRange;
		}

		void Journey::setStartApproachDuration( int duration )
		{
			_startApproachDuration = duration;
		}

		const env::ServiceUse& Journey::getEndServiceUse() const
		{
			return
				(_method == TO_DESTINATION)
				? getLastJourneyLeg()
				: getFirstJourneyLeg()
			;
		}

		const env::ServiceUse& Journey::getStartServiceUse() const
		{
			return
				(_method == TO_DESTINATION)
				? getFirstJourneyLeg()
				: getLastJourneyLeg()
			;
		}

		void Journey::reverse()
		{
			_method = (_method == TO_DESTINATION) ? FROM_ORIGIN : TO_DESTINATION;
			int duration(_startApproachDuration);
			_startApproachDuration = _endApproachDuration;
			_endApproachDuration = duration;
		}

		synthese::AccessDirection Journey::getMethod() const
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
				_squareDistanceToEnd.setFromPoints(vam.getIsobarycenter(), *getEndEdge()->getFromVertex());
		}

		const Edge* Journey::getEndEdge() const
		{
			return
				(_method == TO_DESTINATION)
				? getDestination()
				: getOrigin()
			;
		}

		time::DateTime Journey::getEndTime() const
		{
			return
				(_method == TO_DESTINATION)
				? getArrivalTime()
				: getDepartureTime()
			;
		}

		void Journey::setMinSpeedToEnd( const time::DateTime& dateTime )
		{
			if (_endReached)
				_minSpeedToEnd = 0;
			else
			{
				assert((_method == TO_DESTINATION) ? dateTime > getArrivalTime() : getDepartureTime() > dateTime);

				_minSpeedToEnd = (1000 * _squareDistanceToEnd.getSquareDistance()) / ((_method == TO_DESTINATION) ? dateTime - getArrivalTime() : getDepartureTime() - dateTime);
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
	}
}

