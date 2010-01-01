
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
#include "Journey.h"
#include "RoadModule.h"
#include "PTModule.h"
#include "VertexAccessMap.h"

#include <assert.h>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace env;
	using namespace graph;
	using namespace road;
	using namespace pt;
	

	namespace algorithm
	{


// 
// 		bool BestVertexReachesMap::mustBeCleared(
// 			const Journey& testJourney,
// 			const time::DateTime& dateTime,
// 			const DateTime& bestEndTime
// 		) const {
// 			if ((dateTime.*_strictWeakCTimeComparison)(bestEndTime))
// 				return true;
// 			TimeMap::const_iterator itc(_bestTimeMap.find(testJourney.getEndEdge()->getFromVertex());
// 			if (itc != _bestTimeMap.end ())
// 				return (dateTime.*_cleanUpUselessComparison)(itc->second);
// 			return false;
// 
// 		}



		bool BestVertexReachesMap::isUseLess(
			const TimeMap::key_type& vertex,
			const TimeMap::mapped_type::key_type& transferNumber,
			const TimeMap::mapped_type::mapped_type& duration,
			bool propagateInConnectionPlace
		){
			TimeMap::const_iterator itc(_bestTimeMap.find(vertex));

			if (itc == _bestTimeMap.end ())
			{
				if(propagateInConnectionPlace)
				{
					_insertAndPropagateInConnectionPlace(vertex, transferNumber, duration);
				}
				else
				{
					_insert(vertex, transferNumber, duration);
				}
				return false;
			}

			BOOST_FOREACH(const TimeMap::mapped_type::value_type& item, itc->second)
			{
				if(item.first < transferNumber)
				{
					if(item.second < duration)
						return true;
				}
				else if(item.first == transferNumber)
				{
					if(item.second < duration)
					{
						return true;
					}
					else
					{
						if(propagateInConnectionPlace)
						{
							_insertAndPropagateInConnectionPlace(vertex, transferNumber, duration);
						}
						else
						{
							_insert(vertex, transferNumber, duration);
						}
						return false;
					}
				}
				else
				{
					if(propagateInConnectionPlace)
					{
						_insertAndPropagateInConnectionPlace(vertex, transferNumber, duration);
					}
					else
					{
						_insert(vertex, transferNumber, duration);
					}
					if(item.second >= duration)
					{
						if(propagateInConnectionPlace)
						{
							_removeDurationsForMoreTransfers(vertex, item.first);
						}
						return true;
					}
				}
			}
			if(propagateInConnectionPlace)
			{
				_insertAndPropagateInConnectionPlace(vertex, transferNumber, duration);
			}
			else
			{
				_insert(vertex, transferNumber, duration);
			}
			return false;
		}



		void BestVertexReachesMap::_insert(
			const TimeMap::key_type& vertex,
			const TimeMap::mapped_type::key_type& transfers,
			const TimeMap::mapped_type::mapped_type& duration
		){
			TimeMap::iterator itc = _bestTimeMap.find (vertex);

			if (itc == _bestTimeMap.end ()) 
			{
				itc = _bestTimeMap.insert(make_pair(vertex, TimeMap::mapped_type())).first;
			}

			TimeMap::mapped_type::iterator it(itc->second.find(transfers));
			if (it == itc->second.end())
			{
				itc->second.insert(make_pair(transfers, duration));
			}
			else
			{
				it->second = duration;
			}
		}



		void BestVertexReachesMap::_insertAndPropagateInConnectionPlace(
			const TimeMap::key_type& vertex,
			const TimeMap::mapped_type::key_type& transfers,
			const TimeMap::mapped_type::mapped_type& duration
		){
			_insert(vertex, transfers, duration);
			
			if(!vertex->getHub()->isConnectionPossible()) return;

			const Hub* p(vertex->getHub());
			assert (p != 0);

			if (vertex->getGraphType() == RoadModule::GRAPH_ID)
			{
				if(dynamic_cast<const AddressablePlace*>(p))
				{
					const AddressablePlace::Addresses& ads(AddressablePlace::GetPlace(p)->getAddresses());
					for(AddressablePlace::Addresses::const_iterator ita(ads.begin()); ita != ads.end(); ++ita)
					{
						posix_time::time_duration bestTimeAtAddress(duration);
						if (_accessDirection == DEPARTURE_TO_ARRIVAL)
						{
							if (!p->isConnectionAllowed(*vertex, **ita)) continue;
							bestTimeAtAddress += p->getTransferDelay(*vertex, **ita);
						}
						else
						{
							if (!p->isConnectionAllowed(**ita, *vertex)) continue;
							bestTimeAtAddress += p->getTransferDelay(**ita, *vertex);
						}
						_insert (*ita, transfers+1, bestTimeAtAddress);
					}
				}
			}
			else
			{
				const PublicTransportStopZoneConnectionPlace* cp(static_cast<const PublicTransportStopZoneConnectionPlace*>(p));
				const PublicTransportStopZoneConnectionPlace::PhysicalStops& ps(cp->getPhysicalStops());
				for (PublicTransportStopZoneConnectionPlace::PhysicalStops::const_iterator itp(ps.begin()); itp != ps.end(); ++itp)
				{
					posix_time::time_duration bestTimeAtStop(duration);
					if (_accessDirection == DEPARTURE_TO_ARRIVAL)
					{
						if (!p->isConnectionAllowed(*vertex, *itp->second)) continue;
						bestTimeAtStop += p->getTransferDelay(*vertex, *itp->second);
					}
					else
					{
						if (!p->isConnectionAllowed(*itp->second,*vertex)) continue;
						bestTimeAtStop += p->getTransferDelay(*itp->second, *vertex);
					}
					_insert(itp->second, transfers+1, bestTimeAtStop);
				}
			}
		}



		void BestVertexReachesMap::_removeDurationsForMoreTransfers(
			const TimeMap::key_type& vertex,
			const TimeMap::mapped_type::key_type& transfers
		){
			TimeMap::iterator itc = _bestTimeMap.find(vertex);
			vector<TimeMap::mapped_type::iterator> toDelete;
			for(TimeMap::mapped_type::iterator it(itc->second.find(transfers)); it != itc->second.end(); ++it)
			{
				toDelete.push_back(it);
			}
			BOOST_FOREACH(TimeMap::mapped_type::iterator it, toDelete)
			{
				itc->second.erase(it);
			}
		}



		BestVertexReachesMap::BestVertexReachesMap(
			graph::AccessDirection accessDirection,
			const graph::VertexAccessMap& vam
		):	_accessDirection(accessDirection)
		{
			for (VertexAccessMap::VamMap::const_iterator it(vam.getMap().begin()); it != vam.getMap().end(); ++it)
			{
				_insert(
					it->first,
					0,
					it->second.approachTime
				);
			}
		}
	}
}
