
/** BestVertexReachesMap class implementation.
	@file BestVertexReachesMap.cpp

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

#include "33_route_planner/BestVertexReachesMap.h"

#include "15_env/ConnectionPlace.h"
#include "15_env/Address.h"
#include "15_env/PhysicalStop.h"
#include "15_env/Vertex.h"
#include "15_env/Edge.h"

#include <assert.h>

using namespace boost;

namespace synthese
{
	using namespace env;
	using namespace time;

	namespace routeplanner
	{

		BestVertexReachesMap::BestVertexReachesMap (const AccessDirection& accessDirection)
			: _accessDirection (accessDirection)
		{
		    
		}



		BestVertexReachesMap::~BestVertexReachesMap ()
		{
		    
		}



		bool 
		BestVertexReachesMap::contains (const synthese::env::Vertex* vertex) const
		{
			return (_bestJourneyLegMap.find (vertex) != _bestJourneyLegMap.end ());
		}






		void 
		BestVertexReachesMap::insert (const ServiceUse& journeyLeg)
		{
			const Vertex* vertex(journeyLeg.getSecondEdge()->getFromVertex());
//			_bestJourneyLegMap[vertex] = journeyLeg;

			insert (vertex, journeyLeg.getSecondActualDateTime());
		}    
		    


		void 
		BestVertexReachesMap::insert (const synthese::env::Vertex* vertex, 
						  const synthese::time::DateTime& dateTime,
						  bool propagateInConnectionPlace)
		{
			TimeMap::iterator itc = _bestTimeMap.find (vertex);
			DateTime bestTime = dateTime;

			if (itc == _bestTimeMap.end ()) 
			{
				_bestTimeMap.insert (std::make_pair (vertex, bestTime));
			}
			else
			{
				// TODO : rename FROM_ORIGIN into TOWARD_ORIGIN
				// TODO : rename TO_DESTINATION into TOWARD_DESTINATION
				if ( (_accessDirection == TO_DESTINATION) && (bestTime < itc->second) ||
					 (_accessDirection == FROM_ORIGIN) && (bestTime > itc->second) )
				{
					itc->second = bestTime;
				}
				else
				{
					bestTime = itc->second;
				}
			}

			if (propagateInConnectionPlace)
			{
			const ConnectionPlace* cp = vertex->getConnectionPlace ();
			assert (cp != 0);

			DateTime bestTimeUpperBound (bestTime);

			// TODO : Could be more accurate (with a per vertex max transfer delay)
			// TODO : Also check special forbidden transfer delays
			if (_accessDirection == TO_DESTINATION)
			{
				bestTimeUpperBound += cp->getMaxTransferDelay ();
			}
			else
			{
				bestTimeUpperBound -= cp->getMaxTransferDelay ();
			}

			
			for (std::vector<const Address*>::const_iterator ita = cp->getAddresses ().begin ();
				 ita != cp->getAddresses ().end (); ++ita)
			{
				insert (*ita, bestTimeUpperBound, false);
			}
			for (PhysicalStopsSet::const_iterator itp = cp->getPhysicalStops ().begin ();
				 itp != cp->getPhysicalStops ().end (); ++itp)
			{
				insert (*itp, bestTimeUpperBound, false);
			}

			}

		}





		const DateTime& 
		BestVertexReachesMap::getBestTime (const Vertex* vertex, 
						   const DateTime& defaultValue) const
		{
			TimeMap::const_iterator itc = 
			_bestTimeMap.find (vertex);
		    
			if (itc != _bestTimeMap.end ())
			{
			return itc->second;
			}

		    
			return defaultValue;

		}

		void 
		BestVertexReachesMap::clear ()
		{
			_bestJourneyLegMap.clear ();
			_bestTimeMap.clear ();
		}




	}
}
