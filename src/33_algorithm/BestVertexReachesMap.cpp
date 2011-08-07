
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

#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "Edge.h"
#include "ServicePointer.h"
#include "RoutePlanningIntermediateJourney.hpp"
#include "RoadModule.h"
#include "PTModule.h"
#include "VertexAccessMap.h"
#include <assert.h>

using namespace std;
using namespace boost;
using namespace boost::posix_time;


namespace synthese
{
	using namespace pt;
	using namespace graph;
	using namespace road;
	using namespace pt;


	namespace algorithm
	{


//
// 		bool BestVertexReachesMap::mustBeCleared(
// 			const Journey& testJourney,
// 			const time::ptime& ptime,
// 			const ptime& bestEndTime
// 		) const {
// 			if ((ptime.*_strictWeakCTimeComparison)(bestEndTime))
// 				return true;
// 			TimeMap::const_iterator itc(_bestTimeMap.find(testJourney.getEndEdge()->getFromVertex());
// 			if (itc != _bestTimeMap.end ())
// 				return (ptime.*_cleanUpUselessComparison)(itc->second);
// 			return false;
//
// 		}



		bool BestVertexReachesMap::isUseLess(
			boost::shared_ptr<RoutePlanningIntermediateJourney> journeysptr,
			const ptime& originDateTime,
			bool propagateInConnectionPlace,
			bool strict
		){
			const RoutePlanningIntermediateJourney& journey(*journeysptr);
			const Vertex* const vertex(journey.getEndEdge().getFromVertex());
			const size_t transferNumber(journey.size());
			const posix_time::time_duration duration(
				_accessDirection == DEPARTURE_TO_ARRIVAL ?
				journey.getEndTime() - originDateTime :
				originDateTime - journey.getEndTime()
			);
			assert(duration.total_seconds() >= 0);

			TimeMap::const_iterator itc(_bestTimeMap.find(vertex));

			if (itc == _bestTimeMap.end ())
			{
				if(propagateInConnectionPlace)
				{
					_insertAndPropagateInConnectionPlace(vertex, transferNumber, duration, journeysptr);
				}
				else
				{
					_insert(vertex, transferNumber, duration, journeysptr);
				}
				return false;
			}

			BOOST_FOREACH(const TimeMap::mapped_type::value_type& item, itc->second)
			{
				if(item.first < transferNumber)
				{
					if(item.second.first <= duration)
						return true;
				}
				else if(item.first == transferNumber)
				{
					if(	item.second.first < duration ||
						item.second.first == duration && (strict || *item.second.second > *journeysptr)
					){
						return true;
					}
					else
					{
						if(propagateInConnectionPlace)
						{
							_insertAndPropagateInConnectionPlace(vertex, transferNumber, duration, journeysptr);
						}
						else
						{
							_insert(vertex, transferNumber, duration, journeysptr);
						}
						return false;
					}
				}
				else
				{
					if(propagateInConnectionPlace)
					{
						_insertAndPropagateInConnectionPlace(vertex, transferNumber, duration, journeysptr);
					}
					else
					{
						_insert(vertex, transferNumber, duration, journeysptr);
					}
					if(item.second.first >= duration)
					{
						if(propagateInConnectionPlace)
						{
							_removeDurationsForMoreTransfers(vertex, item.first);
						}
						return false;
					}
				}
			}
			if(propagateInConnectionPlace)
			{
				_insertAndPropagateInConnectionPlace(vertex, transferNumber, duration, journeysptr);
			}
			else
			{
				_insert(vertex, transferNumber, duration, journeysptr);
			}
			return false;
		}



		void BestVertexReachesMap::_insert(
			const TimeMap::key_type& vertex,
			const TimeMap::mapped_type::key_type& transfers,
			boost::posix_time::time_duration duration,
			boost::shared_ptr<RoutePlanningIntermediateJourney> journey
		){
			TimeMap::iterator itc = _bestTimeMap.find (vertex);

			if (itc == _bestTimeMap.end ())
			{
				itc = _bestTimeMap.insert(make_pair(vertex, TimeMap::mapped_type())).first;
			}

			TimeMap::mapped_type::iterator it(itc->second.find(transfers));
			if (it == itc->second.end())
			{
				itc->second.insert(make_pair(transfers, make_pair(duration, journey)));
			}
			else
			{
				it->second = make_pair(duration, journey);
			}
		}



		void BestVertexReachesMap::_insertAndPropagateInConnectionPlace(
			const TimeMap::key_type& vertex,
			const TimeMap::mapped_type::key_type& transfers,
			boost::posix_time::time_duration duration,
			boost::shared_ptr<RoutePlanningIntermediateJourney> journey
		){
			_insert(vertex, transfers, duration, journey);

			if(!vertex->getHub()->isConnectionPossible()) return;

			const Hub* p(vertex->getHub());
			assert (p != 0);

			if (vertex->getGraphType() == PTModule::GRAPH_ID) /// @todo Move this section into PT Module
			{
				const StopArea* cp(static_cast<const StopArea*>(p));
				const StopArea::PhysicalStops& ps(cp->getPhysicalStops());
				for (StopArea::PhysicalStops::const_iterator itp(ps.begin()); itp != ps.end(); ++itp)
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
					_insert(itp->second, transfers+1, bestTimeAtStop, journey);
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
			PlanningPhase accessDirection,
			const graph::VertexAccessMap& vam,
			const graph::VertexAccessMap& destinationVam
		):	_accessDirection(accessDirection)
		{
			for (VertexAccessMap::VamMap::const_iterator it(vam.getMap().begin()); it != vam.getMap().end(); ++it)
			{
				if(destinationVam.getMap().find(it->first) == destinationVam.getMap().end())
				{
					_insert(
						it->first,
						0,
						it->second.approachTime,
						shared_ptr<RoutePlanningIntermediateJourney>(new RoutePlanningIntermediateJourney(accessDirection))
					);
				}
			}
		}
	}
}
