
/** RoutePlanningIntermediateJourney class implementation.
	@file RoutePlanningIntermediateJourney.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "RoutePlanningIntermediateJourney.hpp"

#include "Edge.h"
#include "Hub.h"
#include "JourneyTemplates.h"
#include "Service.h"
#include "VertexAccessMap.h"

#include <boost/logic/tribool.hpp>

using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace graph;

	namespace algorithm
	{
		RoutePlanningIntermediateJourney::RoutePlanningIntermediateJourney(
			PlanningPhase phase
		):	_startApproachDuration(posix_time::minutes(0)),
			_endApproachDuration(posix_time::minutes(0)),
			_phase(phase),
			_endReached(false),
			_similarity(logic::indeterminate)
		{
		}



		RoutePlanningIntermediateJourney::RoutePlanningIntermediateJourney(
			const RoutePlanningIntermediateJourney& journey,
			const graph::ServicePointer& serviceUse,
			bool endIsReached,
			const graph::VertexAccessMap& destinationVam,
			graph::Journey::Distance distanceToEnd,
			bool similarity,
			Score score
		):	Journey(journey),
			_startApproachDuration(
				journey._phase == DEPARTURE_TO_ARRIVAL ?
				journey._startApproachDuration :
				(	endIsReached ?
					destinationVam.getVertexAccess(GetEndEdge(journey._phase, serviceUse)->getFromVertex()).approachTime :
					posix_time::minutes(0)
			)	),
			_endApproachDuration(
				journey._phase == DEPARTURE_TO_ARRIVAL ?
				(	endIsReached ?
					destinationVam.getVertexAccess(GetEndEdge(journey._phase, serviceUse)->getFromVertex()).approachTime :
					posix_time::minutes(0)
				):
				journey._endApproachDuration
			),
			_phase(journey._phase),
			_endReached(endIsReached),
			_distanceToEnd(distanceToEnd),
			_score(similarity ? 0 : score),
			_similarity(similarity)
		{
			if(journey._phase == DEPARTURE_TO_ARRIVAL)
			{
				append(serviceUse);
			}
			else
			{
				prepend(serviceUse);
			}
		}



		RoutePlanningIntermediateJourney::RoutePlanningIntermediateJourney(
			const RoutePlanningIntermediateJourney& journey1,
			const RoutePlanningIntermediateJourney& journey2
		):	Journey(journey1),
			_startApproachDuration(journey1._startApproachDuration),
			_endApproachDuration(journey2._endApproachDuration),
			_phase(journey1._phase),
			_endReached(journey2._endReached),
			_distanceToEnd(journey2._distanceToEnd),
			_score(journey2._score),
			_similarity(tribool(indeterminate))
		{
			assert(journey1._phase == journey2._phase);
			_similarity = (journey1._similarity == false ? tribool(false) : tribool(indeterminate));
			if(journey1._phase == DEPARTURE_TO_ARRIVAL)
			{
				append(journey2);
			}
			else
			{
				prepend(journey2);
			}
		}



		RoutePlanningIntermediateJourney::RoutePlanningIntermediateJourney(
			RoutePlanningIntermediateJourney& model,
			PlanningPhase phase
		):	_startApproachDuration(model._startApproachDuration),
			_endApproachDuration(model._endApproachDuration),
			_phase(phase),
			_endReached(model._endReached),
			_distanceToEnd(model._distanceToEnd),
			_score(model._score),
			_similarity(model._similarity)
		{
			append(model);
		}


		const graph::Edge* RoutePlanningIntermediateJourney::GetEndEdge( PlanningPhase phase, const graph::ServicePointer& serviceUse )
		{
			return phase == DEPARTURE_TO_ARRIVAL ? serviceUse.getArrivalEdge() : serviceUse.getDepartureEdge();
		}



		const Edge& RoutePlanningIntermediateJourney::getEndEdge() const
		{
			assert(!_journeyLegs.empty());

			if(_phase == DEPARTURE_TO_ARRIVAL)
			{
				return *(_journeyLegs.end()-1)->getArrivalEdge();
			}
			else
			{
				return *_journeyLegs.begin()->getDepartureEdge();
			}
		}



		const graph::ServicePointer& RoutePlanningIntermediateJourney::getEndServiceUse() const
		{
			assert(!_journeyLegs.empty());

			if(_phase == DEPARTURE_TO_ARRIVAL)
			{
				return *(_journeyLegs.end()-1);
			}
			else
			{
				return *_journeyLegs.begin();
			}
		}



		const boost::posix_time::ptime RoutePlanningIntermediateJourney::getEndTime(
			bool includeApproach
		) const	{
			assert(!_journeyLegs.empty());

			if(_phase == DEPARTURE_TO_ARRIVAL)
			{
				return getFirstArrivalTime(includeApproach);
			}
			else
			{
				return getFirstDepartureTime(includeApproach);
			}
		}



		boost::posix_time::ptime RoutePlanningIntermediateJourney::getBeginTime(
			bool includeApproach /*= true*/
		) const	{
			assert(!_journeyLegs.empty());

			if(_phase == DEPARTURE_TO_ARRIVAL)
			{
				return getFirstDepartureTime(includeApproach);
			}
			else
			{
				return getFirstArrivalTime(includeApproach);
			}
		}

		posix_time::time_duration RoutePlanningIntermediateJourney::getDuration(
			bool includeApproach
		) const	{
			time_duration duration(Journey::getDuration());

			if(includeApproach)
			{
				duration += _startApproachDuration;
				duration += _endApproachDuration;
			}

			return duration;
		}



		ptime RoutePlanningIntermediateJourney::getFirstDepartureTime(
			bool includeApproach
		) const	{
			boost::posix_time::ptime d(Journey::getFirstDepartureTime());
			if (!includeApproach || d.is_not_a_date_time())
				return d;
			d -= _startApproachDuration;
			return d;
		}



		ptime RoutePlanningIntermediateJourney::getLastDepartureTime(
			bool includeApproach
		) const	{
			boost::posix_time::ptime d(Journey::getLastDepartureTime());
			if (!includeApproach || d.is_not_a_date_time())
				return d;
			d -= _startApproachDuration;
			return d;
		}



		ptime RoutePlanningIntermediateJourney::getFirstArrivalTime(
			bool includeApproach
		) const	{
			ptime d(Journey::getFirstArrivalTime());
			if (!includeApproach || d.is_not_a_date_time())
				return d;
			d += _endApproachDuration;
			return d;
		}



		ptime RoutePlanningIntermediateJourney::getLastArrivalTime(
			bool includeApproach
		) const	{
			ptime d(Journey::getLastArrivalTime());
			if (!includeApproach || d.is_not_a_date_time())
				return d;
			d += _endApproachDuration;
			return d;
		}


		bool RoutePlanningIntermediateJourney::operator>(
			const RoutePlanningIntermediateJourney& other
		) const	{
			// RULE-301 RULE-411
			assert(_phase == other._phase);

			// Priority 1 : end is reached
			if (_endReached != other._endReached)
				return _endReached;

			// Prirority 1b : if both have end or same vertex reached, the the best time is selected
			if (getEndEdge().getFromVertex() == other.getEndEdge().getFromVertex() ||
				_endReached
			){
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
					return _phase == DEPARTURE_TO_ARRIVAL ? currentsTime < othersTime : othersTime < currentsTime;
				}

				/** </ul><p>Comparison between journey of same duration.</p><ul>
				*/
				/** <li>A journey with less approach duration is best</li> */
				posix_time::time_duration pedestrianDuration1(_startApproachDuration + _endApproachDuration);
				posix_time::time_duration pedestrianDuration2(other._startApproachDuration + other._endApproachDuration);

				if (pedestrianDuration1 != pedestrianDuration2)
				{
					return pedestrianDuration1 < pedestrianDuration2;
				}


				/** <li>A journey without compulsory reservation is best</li> */
				if (getReservationCompliance(false) == true && other.getReservationCompliance(false) != true)
					return false;
				if (other.getReservationCompliance(false) == true && getReservationCompliance(false) != true)
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

				// Total distance
				double distance1(getDistance());
				double distance2(other.getDistance());
				if(distance1 != distance2)
				{
					return distance1 < distance2;
				}

				// if we can not decide by time neither by distance, we use the other checks
			}

			// Else best bet to reach the destination
			if (_score != other._score)
				return _score < other._score;

			// Priority 2 : max distance
			if (_distanceToEnd != other._distanceToEnd)
				return _distanceToEnd < other._distanceToEnd;

			// Priority 3 : end time
			if (getEndTime() != other.getEndTime())
			{
				return _phase == DEPARTURE_TO_ARRIVAL ? getEndTime() < other.getEndTime() : other.getEndTime() < getEndTime();
			}

			// Priority 4 (to differentiate journeys in all cases) : nonsense : first service key (better than pointer adress for unit tests)
			if (this->getFirstJourneyLeg().getService()->getKey() != other.getFirstJourneyLeg().getService()->getKey())
			{
				return this->getFirstJourneyLeg().getService()->getKey() < other.getFirstJourneyLeg().getService()->getKey();
			}

			// Priority 5 : addresses order (to differentiate journeys in all cases)
			return this < &other;
		}



		void RoutePlanningIntermediateJourney::clear()
		{
			_endReached = false;
			_startApproachDuration = minutes(0);
			_endApproachDuration = minutes(0);
			_distanceToEnd = optional<Journey::Distance>();
			_similarity = indeterminate;
			Journey::clear();
		}

}	}
