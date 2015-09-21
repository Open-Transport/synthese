
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

#include "AccessParameters.h"
#include "Crossing.h"
#include "Road.h"
#include "RoadChunkEdge.hpp"
#include "RoadModule.h"
#include "VertexAccessMap.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace geos::geom;

namespace synthese
{
	using namespace geography;
	using namespace graph;
	using namespace road;
	using namespace util;

	namespace road
	{
		Address::Address (
			RoadChunk& roadChunk,
			double metricOffset,
			optional<HouseNumber> houseNumber
		):	WithGeometry<Point>(
				roadChunk.getGeometry().get() ?
				roadChunk.getPointFromOffset(metricOffset) :
				boost::shared_ptr<geos::geom::Point>()
			),
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
			// RULE-109
			if(whatToSearch.find(RoadModule::GRAPH_ID) != whatToSearch.end())
			{
				// Chunk linked with the house
				{
					double distance(_metricOffset);
					result.insert(
						_roadChunk->getFromCrossing(),
						VertexAccess(
							seconds(static_cast<long>(distance / accessParameters.getApproachSpeed())),
							distance
					)	);
				}

				// Reverse chunk
				if(_roadChunk->getForwardEdge().getNext())
				{
					assert(static_cast<RoadChunkEdge*>(_roadChunk->getForwardEdge().getNext()));

					double distance(_roadChunk->getForwardEdge().getEndMetricOffset() - _roadChunk->getMetricOffset() - _metricOffset);
					result.insert(
						_roadChunk->getForwardEdge().getNext()->getFromVertex(),
						VertexAccess(
							seconds(static_cast<long>(distance / accessParameters.getApproachSpeed())),
							distance
					)	);
				}
			}
		}
}	}
