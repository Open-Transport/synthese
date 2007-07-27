
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

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace env;
	using namespace time;

	namespace routeplanner
	{

		BestVertexReachesMap::BestVertexReachesMap ()
		{
		    
		}



		BestVertexReachesMap::~BestVertexReachesMap ()
		{
		    
		}



		bool BestVertexReachesMap::contains (const Vertex* vertex) const
		{
			return (_bestJourneyLegMap.find (vertex) != _bestJourneyLegMap.end ());
		}



		void BestVertexReachesMap::insert (const ServiceUse& journeyLeg)
		{
			insert (
				journeyLeg.getSecondEdge()->getFromVertex()
				, journeyLeg.getSecondActualDateTime()
				);
		}    
		    


		void 
		BestVertexReachesMap::insert (
			const synthese::env::Vertex* vertex
			, const synthese::time::DateTime& bestTime
			, bool propagateInConnectionPlace
		){
			TimeMap::iterator itc = _bestTimeMap.find (vertex);
			
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
					return;
			}

			if (propagateInConnectionPlace && vertex->isConnectionAllowed())
			{
				const ConnectionPlace* cp(vertex->getConnectionPlace());
				assert (cp != 0);

				if (vertex->isAddress())
				{
					const Addresses& ads(cp->getAddresses());
					for (Addresses::const_iterator ita(ads.begin()); ita != ads.end(); ++ita)
					{
						DateTime bestTimeAtAddress(bestTime);
						if (_accessDirection == TO_DESTINATION)
						{
							int transferDelay(cp->getTransferDelay(vertex, *ita));
							if (transferDelay == ConnectionPlace::FORBIDDEN_TRANSFER_DELAY)
								continue;
							bestTimeAtAddress += transferDelay;
						}
						else
						{
							int transferDelay(cp->getTransferDelay(*ita, vertex));
							if (transferDelay == ConnectionPlace::FORBIDDEN_TRANSFER_DELAY)
								continue;
							bestTimeAtAddress -= transferDelay;
						}
						insert (*ita, bestTimeAtAddress, false);
					}
				}
				else
				{
					const PhysicalStops& ps(cp->getPhysicalStops());
					for (PhysicalStops::const_iterator itp(ps.begin()); itp != ps.end(); ++itp)
					{
						DateTime bestTimeAtStop(bestTime);
						if (_accessDirection == TO_DESTINATION)
						{
							int transferDelay(cp->getTransferDelay(vertex, *itp));
							if (transferDelay == ConnectionPlace::FORBIDDEN_TRANSFER_DELAY)
								continue;
							bestTimeAtStop += transferDelay;
						}
						else
						{
							int transferDelay(cp->getTransferDelay(*itp, vertex));
							if (transferDelay == ConnectionPlace::FORBIDDEN_TRANSFER_DELAY)
								continue;
							bestTimeAtStop -= transferDelay;
						}
						insert (*itp, bestTimeAtStop, false);
					}
				}
			}
		}



		const DateTime& BestVertexReachesMap::getBestTime(
			const Vertex* vertex
			, const DateTime& defaultValue
		) const {
			TimeMap::const_iterator itc = 
			_bestTimeMap.find (vertex);
		    
			if (itc != _bestTimeMap.end ())
			{
				return itc->second;
			}
		    
			return defaultValue;
		}



		void BestVertexReachesMap::clear (const AccessDirection& accessDirection)
		{
			_bestJourneyLegMap.clear ();
			_bestTimeMap.clear ();
			_accessDirection = accessDirection;
		}
	}
}
