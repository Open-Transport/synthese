
/** TransferPlaceCheck class implementation.
	@file TransferHubCheck.cpp

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

#include "TransferPlaceCheck.hpp"

#include "Edge.h"
#include "Hub.h"
#include "Vertex.h"
#include "VertexAccessMap.h"

namespace synthese
{
	using namespace graph;

	namespace algorithm
	{
		TransferPlaceCheck::TransferPlaceCheck(
			const Hub& checkedPlace,
			GraphIdType graph
		):	_checkedPlace(checkedPlace),
			_graph(graph)
		{}



		bool TransferPlaceCheck::operator()() const
		{
			const Hub::Vertices vertices(_checkedPlace.getVertices(_graph));

			// for each incoming route at x = i :
			BOOST_FOREACH(const Vertex* incomingVertex, vertices)
			{
				const Hub& currentHub(*incomingVertex->getHub());

				BOOST_FOREACH(const Vertex::Edges::value_type& itIncomingEdge, incomingVertex->getArrivalEdges())
				{
					// previous hub before the current one for the incoming path
					const Hub* previousIncomingHub(
						itIncomingEdge.second->getPreviousDepartureForFineSteppingOnly() ?
						itIncomingEdge.second->getPreviousDepartureForFineSteppingOnly()->getFromVertex()->getHub() :
						NULL
					);

					// Ignoring services calling at the hub twice
					if(previousIncomingHub == &currentHub)
					{
						previousIncomingHub = NULL;
					}

					// The path must come from an other hub
					if(!previousIncomingHub)
					{
						continue;
					}

					// Next hub after the current one for the incoming path
					const Hub* nextIncomingHub(
						itIncomingEdge.second->getFollowingArrivalForFineSteppingOnly() ?
						itIncomingEdge.second->getFollowingArrivalForFineSteppingOnly()->getFromVertex()->getHub() :
						NULL
					);

					// Ignoring services calling at the hub twice
					if(nextIncomingHub == &currentHub)
					{
						nextIncomingHub = NULL;
					}

					// Loop on the vertices of the current hub
					BOOST_FOREACH(const Vertex* outgoingVertex, vertices)
					{
						BOOST_FOREACH(const Vertex::Edges::value_type& itOutgoingEdge, outgoingVertex->getDepartureEdges())
						{
							// Jump over the incoming path
							if(itOutgoingEdge.first == itIncomingEdge.first)
							{
								continue;
							}

							// Next hub after the current one for the outgoing path
							const Hub* nextOutgoingHub(
								itOutgoingEdge.second->getFollowingArrivalForFineSteppingOnly() ?
								itOutgoingEdge.second->getFollowingArrivalForFineSteppingOnly()->getFromVertex()->getHub() :
								NULL
							);

							// Ignoring services calling at the hub twice
							if(nextOutgoingHub == &currentHub)
							{
								nextOutgoingHub = NULL;
							}

							// The path must go to an other hub
							if(!nextOutgoingHub)
							{
								continue;
							}

							// Check of the transfer cases
							if(	nextOutgoingHub != nextIncomingHub &&
								nextOutgoingHub != previousIncomingHub
							){
								return true;
							}
						}
					}
				}
			}

			// No transfer case : the stop is not a transfer stop
			return false;
		}
}	}

