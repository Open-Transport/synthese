
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

#include "33_route_planner/Journey.h"
#include "33_route_planner/JourneyLeg.h"

#include "17_messages/SentAlarm.h"
#include "17_messages/Types.h"

#include "15_env/ReservationRuleComplyer.h"
#include "15_env/ReservationRule.h"
#include "15_env/Path.h"
#include "15_env/Service.h"
#include "15_env/ContinuousService.h"
#include "15_env/ConnectionPlace.h"
#include "15_env/Vertex.h"
#include "15_env/Edge.h"

#include "04_time/DateTime.h"

#include "01_util/Constants.h"

using namespace boost;

namespace synthese
{
	using namespace env;
	using namespace messages;
	using namespace time;

	namespace routeplanner
	{

		Journey::Journey ()
			: _continuousServiceRange (UNKNOWN_VALUE)
			, _effectiveDuration (0)
			, _transportConnectionCount (0)
			, _distance (0)
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




		shared_ptr<JourneyLeg> 
		Journey::getJourneyLeg (int index) const
		{
			return _journeyLegs.at (index);
		}




		shared_ptr<JourneyLeg> Journey::getFirstJourneyLeg () const
		{
			return getJourneyLeg (0);
		}



		shared_ptr<JourneyLeg> 
		Journey::getLastJourneyLeg () const
		{
			return getJourneyLeg (getJourneyLegCount () - 1);
		}



		const synthese::env::Edge* 
		Journey::getOrigin() const
		{
			return getFirstJourneyLeg ()->getOrigin ();
		}



		const synthese::env::Edge* 
		Journey::getDestination() const
		{
			return getLastJourneyLeg ()->getDestination ();
		}



		const synthese::time::DateTime& 
		Journey::getDepartureTime () const
		{
			return getFirstJourneyLeg ()->getDepartureTime ();
		}



		const synthese::time::DateTime& 
		Journey::getArrivalTime () const
		{
			return getLastJourneyLeg ()->getArrivalTime ();
		}






		void 
		Journey::prepend (shared_ptr<JourneyLeg> leg)
		{
			_journeyLegs.push_front (leg);
			_effectiveDuration += leg->getDuration ();
			_distance += leg->getDistance ();

			if (leg->getPath ()->isRoad () == false) ++_transportConnectionCount;

		}



		void 
		Journey::prepend (const Journey& journey)
		{
			for (int i=journey.getJourneyLegCount ()-1; i>= 0; --i)
			{
			prepend (journey.getJourneyLeg (i));
			}
		}





		void 
		Journey::append (shared_ptr<JourneyLeg> leg)
		{
			_journeyLegs.push_back (leg);
			_effectiveDuration += leg->getDuration ();
			_distance += leg->getDistance ();

			if (leg->getPath ()->isRoad () == false) ++_transportConnectionCount;

		}




		void 
		Journey::append (const Journey& journey)
		{
			for (int i=0; i<journey.getJourneyLegCount (); ++i)
			{
			append (journey.getJourneyLeg (i));
			}
		}





		int 
		Journey::getMaxAlarmLevel () const
		{
			synthese::time::DateTime alarmStart, alarmStop, now;
			int maxAlarmLevel = 0;
		    
			for (int i=0; i<getJourneyLegCount (); ++i)
			{
			shared_ptr<JourneyLeg> leg(getJourneyLeg (i));
			bool legIsConnection = (i < getJourneyLegCount ()-1);

				// -- Alarm on origin --
				// Alarm start = first departure
				// Alarm stop = last departure
				alarmStart = leg->getDepartureTime ();
				alarmStop = alarmStart;
			if (leg->getServiceInstance().getService()->isContinuous ()) 
				alarmStop += ((const ContinuousService*) leg->getServiceInstance().getService())->getRange ();
			
/*				if ( leg->getOrigin ()->getFromVertex ()->getConnectionPlace ()
				 ->hasApplicableAlarm (alarmStart, alarmStop)
				 && maxAlarmLevel < leg->getOrigin()->getFromVertex ()->
				 getConnectionPlace ()->getAlarm ()->getLevel () )
					maxAlarmLevel = leg->getOrigin()->getFromVertex ()->getConnectionPlace ()->getAlarm ()->getLevel ();
*/			
			
			// -- Compulsory reservation --
			now.updateDateTime ();
			if ( (leg->getServiceInstance().getService()->getReservationRule ()->isCompliant() == true) &&
				 (leg->getServiceInstance().getService()->isReservationPossible ( now, leg->getDepartureTime ())) &&
				 (maxAlarmLevel < ALARM_LEVEL_WARNING) )
			{
					maxAlarmLevel = ALARM_LEVEL_WARNING;
			}

			// -- Possible reservation --
			now.updateDateTime();
			if ( (leg->getServiceInstance().getService()->getReservationRule ()->isCompliant() == boost::logic::indeterminate) &&
				 (leg->getServiceInstance().getService()->isReservationPossible ( now, leg->getDepartureTime ())) &&
				 (maxAlarmLevel < ALARM_LEVEL_INFO) )
			{
					maxAlarmLevel = ALARM_LEVEL_INFO;
			}


				// -- Service alarm --
				// Alarm start = first departure
				// Alarm stop = last arrival
				alarmStart = leg->getDepartureTime ();
				alarmStop = leg->getArrivalTime ();
			if (leg->getServiceInstance().getService()->isContinuous ()) 
				alarmStop += ((const ContinuousService*) leg->getServiceInstance().getService())->getRange ();

/*				if ( (leg->getService ()->getPath ()->hasApplicableAlarm (alarmStart, alarmStop)) &&
				 (maxAlarmLevel < leg->getService ()->getPath ()->getAlarm ()->getLevel ()) )
			{
					maxAlarmLevel = leg->getService ()->getPath ()->getAlarm ()->getLevel ();
			}
*/			
				// -- Alarm on arrival --
				// Alarm start = first arrival
				// Alarm stop = last arrival if connection, last arrival otherwise
				alarmStart = leg->getArrivalTime ();
				alarmStop = alarmStart;
				if (legIsConnection)
			{
					alarmStop = getJourneyLeg (i+1)->getDepartureTime ();
			}

			if (leg->getServiceInstance().getService()->isContinuous ()) 
				alarmStop += ((const ContinuousService*) leg->getServiceInstance().getService())->getRange ();

/*				if ( (leg->getDestination ()->getFromVertex ()->getConnectionPlace ()->hasApplicableAlarm (alarmStart, alarmStop)) &&
					 (maxAlarmLevel < leg->getDestination()->getFromVertex ()->getConnectionPlace ()->getAlarm ()->getLevel ()) )
			{
				maxAlarmLevel = leg->getDestination()->getFromVertex ()->getConnectionPlace ()->getAlarm ()->getLevel ();
			}
*/			}

			return maxAlarmLevel;
		}



		int
		Journey::getDuration () const
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
				shared_ptr<JourneyLeg> leg(*it);
				if ( (continuousServiceRange == UNKNOWN_VALUE) ||
					 (leg->getContinuousServiceRange () < continuousServiceRange) )
				{
					continuousServiceRange = leg->getContinuousServiceRange ();
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
			_journeyLegs.clear();
		}



		Journey& 
		Journey::operator= (const Journey& ref) 
		{
			clear ();
			_journeyLegs = ref._journeyLegs;
			_continuousServiceRange = ref._continuousServiceRange;
			_effectiveDuration = ref._effectiveDuration;
			_transportConnectionCount = ref._transportConnectionCount;
			_distance = ref._distance;
			return *this;
		}

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

		bool Journey::isBestThan( const Journey& other, const AccessDirection& direction ) const
		{

			//! <li>An empty journey cannot be superior to another</li> 
			if (empty())
				return false;

			//! <li>A populated journey is superior to an empty journey</li> 
			if (other.empty())
				return true;

			//! <li>Time comparison</li>
			DateTime currentsTime = (direction == TO_DESTINATION) 
				? getArrivalTime ()
				: getDepartureTime ();

			DateTime othersTime = (direction == TO_DESTINATION) 
				? other.getArrivalTime ()
				: other.getDepartureTime ();

			bool betterTime = (direction == TO_DESTINATION) 
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
	}
}

