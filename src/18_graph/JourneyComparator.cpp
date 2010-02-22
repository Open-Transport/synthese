	
/** JourneyComparator class implementation.
	@file JourneyComparator.cpp

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

#include "JourneyComparator.h"
#include "Journey.h"
#include "Edge.h"

#include <assert.h>

using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	namespace graph
	{
		bool JourneyComparator::operator() (shared_ptr<Journey> j1, shared_ptr<Journey> j2) const
		{
			assert(j1 != NULL);
			assert(j2 != NULL);
			assert(j1->getMethod() == j2->getMethod());

			// Priority 1 : end is reached
			if (j1->getEndReached() != j2->getEndReached())
				return j1->getEndReached();
			
			// Prirority 1b : if both have end or same vertex reached, the the best time is selected
			if (j1->getEndEdge()->getFromVertex() == j2->getEndEdge()->getFromVertex() ||
				j1->getEndReached()
			){
				if (j1->getEndTime() != j2->getEndTime())
				{
					return j1->getMethod() == DEPARTURE_TO_ARRIVAL ? j1->getEndTime() < j2->getEndTime() : j2->getEndTime() < j1->getEndTime();
				}

				posix_time::time_duration pedestrianDuration1(j1->getStartApproachDuration() + j1->getEndApproachDuration());
				posix_time::time_duration pedestrianDuration2(j2->getStartApproachDuration() + j2->getEndApproachDuration());

				if (pedestrianDuration1 != pedestrianDuration2)
				{
					return pedestrianDuration1 < pedestrianDuration2;
				}

				if(j1->size() != j2->size())
				{
					return j1->size() < j2->size();
				}

				// Total distance
				double distance1(j1->getDistance());
				double distance2(j2->getDistance());
				if(distance1 != distance2)
				{
					return distance1 < distance2;
				}

				// Effective duration
				time_duration td1(j1->getEffectiveDuration());
				time_duration td2(j2->getEffectiveDuration());
				if(td1 != td2)
				{
					return td1 < td2;
				}

				// Priority 1c : order between addresses
				return j1 < j2;
			}
			if (j1->getScore() != j2->getScore())
				return j1->getScore() < j2->getScore();
			
			// Priority 2 : max distance
			if (j1->getDistanceToEnd() != j2->getDistanceToEnd())
				return j1->getDistanceToEnd() < j2->getDistanceToEnd();

			// Priority 3 : end time
			if (j1->getEndTime() != j2->getEndTime())
			{
				return j1->getMethod() == DEPARTURE_TO_ARRIVAL ? j1->getEndTime() < j2->getEndTime() : j2->getEndTime() < j1->getEndTime();
			}

			// Priority 4 : addresses order (to differentiate journeys in all cases)
			return j1 < j2;
		}
	}
}
