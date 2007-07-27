
/** JourneyLegComparator class implementation.
	@file JourneyLegComparator.cpp

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

#include "33_route_planner/JourneyLegComparator.h"

#include "15_env/Edge.h"
#include "15_env/Vertex.h"
#include "15_env/ConnectionPlace.h"
#include "15_env/ServiceUse.h"

#include <assert.h>

using namespace boost;

namespace synthese
{
	using namespace env;

	namespace routeplanner
	{

		JourneyLegComparator::JourneyLegComparator ()
//			: _secondServiceUseAccessor((accessDirection == TO_DESTINATION) ? &Journey::getLastJourneyLeg : &Journey::getFirstJourneyLeg)
//			, _firstServiceUseAccessor((accessDirection == TO_DESTINATION) ? &Journey::getFirstJourneyLeg : &Journey::getLastJourneyLeg)
		{	}
		 

		JourneyLegComparator::~JourneyLegComparator ()
		{	}




		int 
		JourneyLegComparator::operator () (const Journey& j1, const Journey& j2) const
		{
			assert(j1.getMethod() == j2.getMethod());

			if (j1.getMinSpeedToEnd() == j2.getMinSpeedToEnd())
			{
				if (j1.getMethod() == TO_DESTINATION)
					return j1.getEndTime() < j2.getEndTime();
				else
					return j1.getEndTime() > j2.getEndTime();
			}

		//	assert ((jl1->*_edgeAccessor) ()->getFromVertex ()->getConnectionPlace () != 0);
		//	assert ((jl2->*_edgeAccessor) ()->getFromVertex ()->getConnectionPlace () != 0);
/// @todo Reactivate this code
/*			ConnectionPlace::ConnectionType type1 = 
				(jl1->*_edgeAccessor) ()->getFromVertex ()->getConnectionPlace ()
					->getRecommendedConnectionType (jl1->getSquareDistance ());

			ConnectionPlace::ConnectionType type2 = 
				(jl2->*_edgeAccessor) ()->getFromVertex ()->getConnectionPlace ()
					->getRecommendedConnectionType (jl2->getSquareDistance ());
		    
			if (type1 != type2)	return type2 - type1;
*/
			return (j1.getMinSpeedToEnd() < j2.getMinSpeedToEnd());
		}


	}

}

