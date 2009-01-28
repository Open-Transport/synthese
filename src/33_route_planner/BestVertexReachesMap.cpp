
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

#include "BestVertexReachesMap.h"

#include "PublicTransportStopZoneConnectionPlace.h"
#include "Address.h"
#include "PhysicalStop.h"
#include "Vertex.h"
#include "Edge.h"
#include "ServiceUse.h"

#include <assert.h>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace env;
	using namespace time;
	using namespace graph;

	namespace routeplanner
	{

		BestVertexReachesMap::BestVertexReachesMap (
			AccessDirection accessDirection
			, bool optim
		)	: _accessDirection(accessDirection)
		{
		    if (accessDirection == DEPARTURE_TO_ARRIVAL)
			{
				_cleanUpUselessComparison = &DateTime::operator>;
				_comparison = optim ? &DateTime::operator> : &DateTime::operator>=;
				_strictWeakCTimeComparison = &DateTime::operator>=;
			}
			else
			{
				_cleanUpUselessComparison = &DateTime::operator<;
				_comparison = optim ? &DateTime::operator< : &DateTime::operator<=;
				_strictWeakCTimeComparison = &DateTime::operator<=;
			}
		}



		BestVertexReachesMap::~BestVertexReachesMap ()
		{
		    
		}



		bool BestVertexReachesMap::contains (const Vertex* vertex) const
		{
			return (_bestTimeMap.find (vertex) != _bestTimeMap.end ());
		}



		void BestVertexReachesMap::insert (const ServiceUse& journeyLeg)
		{
			insert (
				journeyLeg.getSecondEdge()->getFromVertex()
				, journeyLeg.getSecondActualDateTime()
				);
		}    
		    


		void BestVertexReachesMap::insert (
			const Vertex* vertex
			, const DateTime& bestTime
			, bool propagateInConnectionPlace
		){
			TimeMap::iterator itc = _bestTimeMap.find (vertex);
			
			if (itc == _bestTimeMap.end ()) 
			{
				_bestTimeMap.insert (std::make_pair (vertex, bestTime));
			}
			else
			{
				if((bestTime.*_comparison)(itc->second))
					return;
				itc->second = bestTime;
			}

			if (propagateInConnectionPlace && vertex->getPlace()->getScore() > 0)
			{
				const Hub* p(vertex->getPlace());
				assert (p != 0);

				if (vertex->isAddress())
				{
					const Addresses& ads(AddressablePlace::GetPlace(p)->getAddresses());
					for (Addresses::const_iterator ita(ads.begin()); ita != ads.end(); ++ita)
					{
						DateTime bestTimeAtAddress(bestTime);
						if (_accessDirection == DEPARTURE_TO_ARRIVAL)
						{
							int transferDelay(p->getTransferDelay(vertex, *ita));
							if (transferDelay == Hub::FORBIDDEN_TRANSFER_DELAY)
								continue;
							bestTimeAtAddress += transferDelay;
						}
						else
						{
							int transferDelay(p->getTransferDelay(*ita, vertex));
							if (transferDelay == Hub::FORBIDDEN_TRANSFER_DELAY)
								continue;
							bestTimeAtAddress -= transferDelay;
						}
						insert (*ita, bestTimeAtAddress, false);
					}
				}
				else
				{
					const PublicTransportStopZoneConnectionPlace* cp(static_cast<const PublicTransportStopZoneConnectionPlace*>(p));
					const PhysicalStops& ps(cp->getPhysicalStops());
					for (PhysicalStops::const_iterator itp(ps.begin()); itp != ps.end(); ++itp)
					{
						DateTime bestTimeAtStop(bestTime);
						if (_accessDirection == DEPARTURE_TO_ARRIVAL)
						{
							int transferDelay(p->getTransferDelay(vertex, itp->second));
							if (transferDelay == Hub::FORBIDDEN_TRANSFER_DELAY)
								continue;
							bestTimeAtStop += transferDelay;
						}
						else
						{
							int transferDelay(p->getTransferDelay(itp->second, vertex));
							if (transferDelay == Hub::FORBIDDEN_TRANSFER_DELAY)
								continue;
							bestTimeAtStop -= transferDelay;
						}
						insert (itp->second, bestTimeAtStop, false);
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

		bool BestVertexReachesMap::isUseless(
			const Vertex* vertex,
			const DateTime& dateTime
		) const {
			TimeMap::const_iterator itc(_bestTimeMap.find (vertex));
			if (itc != _bestTimeMap.end ())
				return (dateTime.*_comparison)(itc->second);
			return false;
		}

		bool BestVertexReachesMap::mustBeCleared(
			const Vertex* vertex,
			const DateTime& dateTime,
			const DateTime& bestEndTime
		) const {
			if ((dateTime.*_strictWeakCTimeComparison)(bestEndTime))
				return true;
			TimeMap::const_iterator itc(_bestTimeMap.find (vertex));
			if (itc != _bestTimeMap.end ())
				return (dateTime.*_cleanUpUselessComparison)(itc->second);
			return false;

		}


	}
}
