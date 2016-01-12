
/** BestVertexReachesMap class implementation.
	@file BestVertexReachesMap.cpp

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
			const boost::shared_ptr<RoutePlanningIntermediateJourney>& journeysptr,
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

			// TODO : this assert is wrong sometimes (due to other bugs) and cause a crash
			// So, when others bugs will be resolved, maybe the following if could be removed replaced by the assert ?
			// assert(duration.total_seconds() >= 0);

			if(duration.total_seconds() < 0)
			{
				return true;
			}

			TimeMap::value_type& vertexItem(_bestTimeMap[vertex->getIndex()]);

			if(	vertexItem.empty()
			){
				if(propagateInConnectionPlace)
				{
					_insertAndPropagateInConnectionPlace(vertexItem, transferNumber, duration, journeysptr, *vertex);
				}
				else
				{
					_insert(vertexItem, transferNumber, duration, journeysptr);
				}
				return false;
			}

			BOOST_FOREACH(const TimeMap::value_type::value_type& item, vertexItem)
			{
				if(item.first < transferNumber)
				{
					if(item.second.first <= duration)
						return true; // Useless if more transfers and longer
				}
				else if(item.first == transferNumber)
				{
					if(	item.second.first < duration ||
						(item.second.first == duration && (strict || *item.second.second > *journeysptr))
					){
						return true; // Useless if same transfers number and longer or same time but not better
					}
					else
					{
						if(propagateInConnectionPlace)
						{
							_insertAndPropagateInConnectionPlace(vertexItem, transferNumber, duration, journeysptr, *vertex);
						}
						else
						{
							_insert(vertexItem, transferNumber, duration, journeysptr);
						}
						return false; // Useful
					}
				}
				else
				{
					if(propagateInConnectionPlace)
					{
						_insertAndPropagateInConnectionPlace(vertexItem, transferNumber, duration, journeysptr, *vertex);
					}
					else
					{
						_insert(vertexItem, transferNumber, duration, journeysptr);
					}
					if(item.second.first >= duration)
					{
						if(propagateInConnectionPlace)
						{
							_removeDurationsForMoreTransfers(vertexItem, item.first);
						}
						return false; // Useful
					}
				}
			}
			if(propagateInConnectionPlace)
			{
				_insertAndPropagateInConnectionPlace(vertexItem, transferNumber, duration, journeysptr, *vertex);
			}
			else
			{
				_insert(vertexItem, transferNumber, duration, journeysptr);
			}
			return false; // Useful
		}



		void BestVertexReachesMap::_insert(
			TimeMap::value_type& vertexItem,
			size_t transfers,
			boost::posix_time::time_duration duration,
			const boost::shared_ptr<RoutePlanningIntermediateJourney>& journey
		){
			TimeMap::value_type::iterator it(vertexItem.find(transfers));
			if (it == vertexItem.end())
			{
				vertexItem.insert(make_pair(transfers, make_pair(duration, journey)));
			}
			else
			{
				it->second = make_pair(duration, journey);
			}
		}



		void BestVertexReachesMap::_insertAndPropagateInConnectionPlace(
			TimeMap::value_type& vertexItem,
			size_t transfers,
			boost::posix_time::time_duration duration,
			const boost::shared_ptr<RoutePlanningIntermediateJourney>& journey,
			const graph::Vertex& vertex
		){
			_insert(vertexItem, transfers, duration, journey);

			if(!vertex.getHub()->isUsefulTransfer(vertex.getGraphType())) return;

			const Hub* p(vertex.getHub());
			assert (p != 0);

			if (vertex.getGraphType() == PTModule::GRAPH_ID) /// @todo Move this section into PT Module
			{
				const StopArea* cp(static_cast<const StopArea*>(p));
				const StopArea::PhysicalStops& ps(cp->getPhysicalStops());
				for (StopArea::PhysicalStops::const_iterator itp(ps.begin()); itp != ps.end(); ++itp)
				{
					posix_time::time_duration bestTimeAtStop(duration);
					if (_accessDirection == DEPARTURE_TO_ARRIVAL)
					{
						if (!p->isConnectionAllowed(vertex, *itp->second)) continue;
						bestTimeAtStop += p->getTransferDelay(vertex, *itp->second);
					}
					else
					{
						if (!p->isConnectionAllowed(*itp->second, vertex)) continue;
						bestTimeAtStop += p->getTransferDelay(*itp->second, vertex);
					}
					_insert(_bestTimeMap[itp->second->getIndex()], transfers+1, bestTimeAtStop, journey);
				}
			}
		}



		void BestVertexReachesMap::_removeDurationsForMoreTransfers(
			TimeMap::value_type& vertexItem,
			std::size_t transfers
		){
			vector<TimeMap::value_type::iterator> toDelete;
			for(TimeMap::value_type::iterator it(vertexItem.find(transfers)); it != vertexItem.end(); ++it)
			{
				toDelete.push_back(it);
			}
			BOOST_FOREACH(TimeMap::value_type::iterator it, toDelete)
			{
				vertexItem.erase(it);
			}
		}



		BestVertexReachesMap::BestVertexReachesMap(
			PlanningPhase accessDirection,
			const graph::VertexAccessMap& vam,
			const graph::VertexAccessMap& destinationVam,
			std::size_t vertexNumber
		):	_bestTimeMap(vertexNumber),
			_accessDirection(accessDirection)
		{
			for (VertexAccessMap::VamMap::const_iterator it(vam.getMap().begin()); it != vam.getMap().end(); ++it)
			{
				if(destinationVam.getMap().find(it->first) == destinationVam.getMap().end())
				{
					_insert(
						_bestTimeMap[it->first->getIndex()],
						0,
						it->second.approachTime,
						boost::shared_ptr<RoutePlanningIntermediateJourney>(new RoutePlanningIntermediateJourney(accessDirection))
					);
				}
			}
		}
}	}
