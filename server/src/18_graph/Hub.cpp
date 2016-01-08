
/** Hub class implementation.
	@file Hub.cpp

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

#include "Hub.h"

#include "Edge.h"
#include "TransferPlaceCheck.hpp"
#include "Vertex.h"
#include "VertexAccessMap.h"

using namespace std;

namespace synthese
{
	using namespace algorithm;

	namespace graph
	{
		bool Hub::isUsefulTransfer(
			GraphIdType graphId
		) const	{
			if( graphId >= _usefulnessTransferCache.size() ||
				indeterminate(_usefulnessTransferCache[graphId])
			){
				// Raise the size of the vector if necessary
				if(graphId >= _usefulnessTransferCache.size())
				{
					_usefulnessTransferCache.resize(graphId+1);
				}

				// Check the forced possible transfer attribute
				if(!isConnectionPossible())
				{
					_usefulnessTransferCache[graphId] = false;
				}
				else
				{
					TransferPlaceCheck tpc(
						*this,
						graphId
					);
					_usefulnessTransferCache[graphId] = tpc();
				}

				// Reinitialization of the edge pointers
				_linkEdgesAccordingToTransferUsefulness(graphId);
			}
			return _usefulnessTransferCache[graphId];
		}



		void Hub::clearUsefulForTransfer(
			GraphIdType graphId
		) const	{
			// Raise the size of the vector if necessary
			if(graphId >= _usefulnessTransferCache.size())
			{
				_usefulnessTransferCache.resize(graphId+1);
			}

			// Reinitialization at undefined value
			_usefulnessTransferCache[graphId] = boost::logic::indeterminate;

			// Reinitialization of the edge pointers
			_linkEdgesAccordingToTransferUsefulness(graphId);
		}



		void Hub::clearAndPropagateUsefulTransfer(
			GraphIdType graphId
		) const	{
			// Local clear
			clearUsefulForTransfer(graphId);

			// Neighborhood
			set<const Hub*> hubs;
			BOOST_FOREACH(const Vertex* incomingVertex, getVertices(graphId))
			{
				BOOST_FOREACH(const Vertex::Edges::value_type& itIncomingEdge, incomingVertex->getArrivalEdges())
				{
					// previous hub before the current one for the incoming path
					const Hub* previousIncomingHub(
						itIncomingEdge.second->getPreviousDepartureForFineSteppingOnly() &&
						itIncomingEdge.second->getPreviousDepartureForFineSteppingOnly()->getFromVertex() ?
						itIncomingEdge.second->getPreviousDepartureForFineSteppingOnly()->getFromVertex()->getHub() :
						NULL
					);
					if(previousIncomingHub)
					{
						hubs.insert(previousIncomingHub);
					}
			}	}
			BOOST_FOREACH(const Hub* hub, hubs)
			{
				hub->clearUsefulForTransfer(graphId);
			}
		}



		void Hub::_linkEdgesAccordingToTransferUsefulness(
			GraphIdType graphId
		) const	{

			// The value to take into account of
			bool transferUsefulness(
				(	_usefulnessTransferCache[graphId] || indeterminate(_usefulnessTransferCache[graphId])) &&
				isConnectionPossible()
			);

			BOOST_FOREACH(const Vertex* incomingVertex, getVertices(graphId))
			{
				BOOST_FOREACH(const Vertex::Edges::value_type& itIncomingEdge, incomingVertex->getArrivalEdges())
				{
					Edge& incomingEdge(
						const_cast<Edge&>(
							*itIncomingEdge.second
					)	);

					for(Edge* edge(incomingEdge.getPrevious());
						edge && edge->getFollowingArrivalForFineSteppingOnly() == &incomingEdge && edge != edge->getPrevious();
						edge = edge->getPrevious()
					){
						if(transferUsefulness)
						{
							edge->setFollowingConnectionArrival(&incomingEdge);
						}
						else
						{
							edge->setFollowingConnectionArrival(incomingEdge.getFollowingConnectionArrival());
						}
					}
			}	}
		}
}	}

