	
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

#include "15_env/Journey.h"

namespace synthese
{
	namespace env
	{
		bool JourneyComparator::operator() (const Journey* j1, const Journey* j2) const
		{
			assert(j1 != NULL);
			assert(j2 != NULL);
			assert(j1->getMethod() == j2->getMethod());
			assert(j1->getSquareDistanceToEnd().getSquareDistance() != UNKNOWN_VALUE);
			assert(j2->getSquareDistanceToEnd().getSquareDistance() != UNKNOWN_VALUE);

			/// @todo Add imitation function (which search to imitate _results.rbegin and _results.rbegin+1

			// Priority 1 : end is reached
			if (j1->getEndReached() != j2->getEndReached())
				return j1->getEndReached();
			
			// Prirority 1b : if both have end reached, the the best time is selected
			if (j1->getEndReached())
			{
				if (j1->getEndTime() != j2->getEndTime())
					return (j1->getEndTime().*(j1->getBestTimeStrictOperator())) (j2->getEndTime());

				// Priority 1c : order between addresses
				return j1 < j2;
			}

			// Priority 2 : min speed to end
			if (j1->getMinSpeedToEnd() != j2->getMinSpeedToEnd())
				return (j1->getMinSpeedToEnd() < j2->getMinSpeedToEnd());

			// Priority 3 : end time
			if (j1->getEndTime() != j2->getEndTime())
				return (j1->getEndTime().*j1->getBestTimeStrictOperator())(j2->getEndTime());

			// Priority 4 : addresses order (to differentiate journeys in all cases)
			return j1 < j2;
		}
	}
}
