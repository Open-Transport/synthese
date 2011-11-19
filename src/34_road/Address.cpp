
/** Address class implementation.
	@file Address.cpp

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

#include "Address.h"
#include "Road.h"
#include "AccessParameters.h"
#include "RoadModule.h"
#include "VertexAccessMap.h"
#include "ReverseRoadChunk.hpp"

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace geos::geom;

namespace synthese
{
	using namespace geography;
	using namespace graph;

	namespace road
	{
		Address::Address (
			MainRoadChunk& roadChunk,
			double metricOffset,
			optional<MainRoadChunk::HouseNumber> houseNumber
		):	WithGeometry<Point>(roadChunk.getPointFromOffset(metricOffset)),
			_roadChunk(&roadChunk),
			_metricOffset(metricOffset),
			_houseNumber(houseNumber)
		{
		}



		Address::Address()
		:	_roadChunk(NULL),
			_metricOffset(0)
		{
		}


		Address::~Address()
		{
		}



		void Address::getVertexAccessMap(
			graph::VertexAccessMap& result,
			const graph::AccessParameters& accessParameters,
			const Place::GraphTypes& whatToSearch
		) const	{
			if(whatToSearch.find(RoadModule::GRAPH_ID) != whatToSearch.end())
			{
				// Chunk linked with the house
				result.insert(
					_roadChunk->getFromVertex(),
					VertexAccess(
						seconds(static_cast<long>((_metricOffset - _roadChunk->getMetricOffset()) / accessParameters.getApproachSpeed())),
						_metricOffset
				)	);

				// Reverse chunk
				if(_roadChunk->getNext())
				{
					assert(static_cast<MainRoadChunk*>(_roadChunk->getNext())->getReverseRoadChunk());

					double distance(_roadChunk->getEndMetricOffset() - _metricOffset);
					result.insert(
						static_cast<MainRoadChunk*>(_roadChunk->getNext())->getReverseRoadChunk()->getFromVertex(),
						VertexAccess(
							seconds(static_cast<long>(distance / accessParameters.getApproachSpeed())),
							distance
					)	);
				}
			}
		}
}	}
