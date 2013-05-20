
/** MainRoadPart class implementation.
	@file MainRoadPart.cpp

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

#include "MainRoadPart.hpp"
#include "ReverseRoadPart.hpp"
#include "RoadPlace.h"
#include "MainRoadChunk.hpp"
#include "ReverseRoadChunk.hpp"

#include <geos/geom/LineString.h>

using namespace std;
using namespace geos::geom;
using namespace boost;

namespace synthese
{

	namespace util
	{
		template<> const string Registry<road::MainRoadPart>::KEY("MainRoadPart");
	}

	using namespace graph;

	namespace road
	{
		MainRoadPart::MainRoadPart(
			util::RegistryKeyType key /*= 0*/,
			RoadType type /*= ROAD_TYPE_UNKNOWN */
		):	Registrable(key),
			Road(key, type)
		{
			_reverseRoad.reset(new ReverseRoadPart(*this));
		}



		void MainRoadPart::setRoadPlace( RoadPlace& value )
		{
			_setRoadPlace(value);

			// Links the reverse road too
			_reverseRoad->_setRoadPlace(value);
		}



		void MainRoadPart::merge( MainRoadPart& other )
		{
			Path::merge(static_cast<Path&>(other));
			other._reverseRoad->merge(*_reverseRoad);
			swap(other._reverseRoad, _reverseRoad);
		}



		void MainRoadPart::addRoadChunk( MainRoadChunk& chunk )
		{
			if(getEdges().empty())
			{
				ReverseRoadChunk* reverseChunk(
					new ReverseRoadChunk(
						0,
						chunk.getFromCrossing(),
						0,
						_reverseRoad.get(),
						-chunk.getMetricOffset(),
						&chunk
				)	);
				boost::shared_ptr<LineString> geometry(chunk.getGeometry());
				if(geometry.get())
				{
					reverseChunk->setGeometry(
						boost::shared_ptr<LineString>(
							dynamic_cast<LineString*>(chunk.getGeometry()->reverse())
					)	);
				}
				_reverseRoad->addEdge(*reverseChunk);
				chunk.setReverseRoadChunk(reverseChunk);
			}
			else
			{
				const Edge& lastEdge(**(getEdges().end() - 1));
				if(chunk.getRankInPath() > lastEdge.getRankInPath())
				{
					ReverseRoadChunk* reverseChunk(
						new ReverseRoadChunk(
							0,
							chunk.getFromCrossing(),
							0,
							_reverseRoad.get(),
							-chunk.getMetricOffset(),
							&chunk
					)	);
					_reverseRoad->_insertRoadChunk(
						*reverseChunk,
						0,
						chunk.getRankInPath() - lastEdge.getRankInPath()
					);
					chunk.setReverseRoadChunk(reverseChunk);
				}
				else
				{
					ReverseRoadChunk* reverseChunk(
						new ReverseRoadChunk(
							0,
							chunk.getFromCrossing(),
							lastEdge.getRankInPath() - chunk.getRankInPath(),
							_reverseRoad.get(),
							-chunk.getMetricOffset(),
							&chunk
					)	);
					_reverseRoad->addEdge(
						*reverseChunk
					);
					chunk.setReverseRoadChunk(reverseChunk);
				}
			}

			addEdge(static_cast<Edge&>(chunk));
		}



		void MainRoadPart::insertRoadChunk(
			MainRoadChunk& chunk,
			double length,
			std::size_t rankShift
		){
			_insertRoadChunk(chunk, length, rankShift);

			// Shifting the whole reverse road to left
			for(Edges::const_iterator it(_reverseRoad->_edges.begin()); it != _reverseRoad->_edges.end(); ++it)
			{
				(*it)->setMetricOffset((*it)->getMetricOffset() - length);
			}

			// Insertion of the chunk
			const Edge& lastEdge(**(_reverseRoad->getEdges().end() - 1));

			ReverseRoadChunk* reverseChunk(
				new ReverseRoadChunk(
					0,
					chunk.getFromCrossing(),
					lastEdge.getRankInPath() + rankShift,
					_reverseRoad.get(),
					0,
					&chunk
			)	);
			_reverseRoad->addEdge(
				*reverseChunk
			);
			chunk.setReverseRoadChunk(reverseChunk);
		}

		void MainRoadPart::setType (const RoadType& type)
		{
			Road::setType(type);
			if(_reverseRoad.get())
			{
				_reverseRoad->setType(type);
			}
		}
}	}
