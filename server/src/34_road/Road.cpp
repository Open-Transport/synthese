
/** Road class implementation.
	@file Road.cpp

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

#include "Road.h"

#include "RoadChunk.h"
#include "RoadChunkEdge.hpp"
#include "RoadPath.hpp"

using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace road;
	using namespace util;
	using namespace graph;

	CLASS_DEFINITION(Road, "t015_roads", 15)
	FIELD_DEFINITION_OF_OBJECT(Road, "road_id", "road_ids")

	FIELD_DEFINITION_OF_TYPE(RoadTypeField, "road_type", SQL_INTEGER)

	namespace road
	{
		Road::Road (
			RegistryKeyType id,
			RoadType type
		):	Registrable(id),
			Object<Road, RoadSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_VALUE_CONSTRUCTOR(RoadTypeField, type),
					FIELD_DEFAULT_CONSTRUCTOR(RoadPlace::Vector)
			)	),
			_forwardPath(new RoadPath(*this)),
			_reversePath(new RoadPath(*this))
		{}



		Road::~Road()
		{
			// Edges null pointer
			BOOST_FOREACH(Edge* edge, getForwardPath().getEdges())
			{
				edge->setParentPath(NULL);
			}
			BOOST_FOREACH(Edge* edge, getReversePath().getEdges())
			{
				edge->setParentPath(NULL);
			}

			unlink();
		}



		void Road::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			if(!get<RoadPlace::Vector>().empty())
			{
				BOOST_FOREACH(RoadPlace* roadPlace, get<RoadPlace::Vector>())
				{
					assert(roadPlace != NULL);
					_forwardPath->_pathGroup = roadPlace;
					roadPlace->addRoad(*_forwardPath);
					_reversePath->_pathGroup = roadPlace;
					roadPlace->addRoad(*_reversePath);

					roadPlace->addRoad(*this);
				}
			}
			else
			{
				_forwardPath->_pathGroup = NULL;
				_reversePath->_pathGroup = NULL;
			}
		}

		void Road::unlink()
		{
			BOOST_FOREACH(RoadPlace* roadPlace, get<RoadPlace::Vector>())
			{
				assert(roadPlace != NULL);
				roadPlace->removeRoad(*_forwardPath);
				roadPlace->removeRoad(*_reversePath);

				roadPlace->removeRoad(*this);
			}
		}

		bool Road::allowUpdate(const server::Session* session) const
		{
			return true;
		}

		bool Road::allowCreate(const server::Session* session) const
		{
			return true;
		}

		bool Road::allowDelete(const server::Session* session) const
		{
			return true;
		}


		void Road::insertRoadChunk( RoadChunk& chunk, double length, size_t rankShift )
		{
			assert(!getForwardPath()._edges.empty());
			assert(chunk.getMetricOffset() <= (*getForwardPath()._edges.begin())->getMetricOffset() + length);
			assert(chunk.getMetricOffset() <= (*getForwardPath()._edges.begin())->getRankInPath() + rankShift);

			// Shifting the whole road to right
			std::vector<RoadChunk*> roadChunks;
			BOOST_FOREACH(Edge* edge, getForwardPath()._edges)
			{
				roadChunks.push_back(static_cast<RoadChunkEdge*>(edge)->getRoadChunk());
			}

			BOOST_FOREACH(RoadChunk* roadChunk, roadChunks)
			{
				roadChunk->unlink();
				roadChunk->setRankInPath(roadChunk->getRankInPath() + rankShift);
				roadChunk->setMetricOffset(roadChunk->getMetricOffset() + length);
			}

			Env fakeEnv;
			BOOST_FOREACH(RoadChunk* roadChunk, roadChunks)
			{
				roadChunk->link(fakeEnv);
			}
			chunk.link(fakeEnv);
		}



		//////////////////////////////////////////////////////////////////////////
		/// Merges two roads.
		/// @param other the road to add at the end of the current object
		/// Actions :
		///  - verify if the two roads can be merged (the second one must begin
		///    where the current one ends, and the two roads must belong to the
		///    same RoadPlace)
		///  - shift the metric offset in the second road
		///  - change the pointers
		///  - delete the second road in the road place
		/// The other road must be removed from the registry externally
		void Road::merge( Road& other )
		{
			if(	other.get<RoadPlace::Vector>().empty() ||
				get<RoadPlace::Vector>().empty() ||
				other.get<RoadPlace::Vector>() != get<RoadPlace::Vector>() ||
				other.getForwardPath()._edges.empty() ||
				getForwardPath()._edges.empty() ||
				other.getForwardPath().getEdge(0)->getFromVertex() != getForwardPath().getLastEdge()->getFromVertex() ||
				&other == this
			){
				throw Exception("The two roads cannot be merged");
			}

			Env fakeEnv; // for the link method (will not be used)
			RoadChunk* lastChunkOfFirstPart(static_cast<RoadChunkEdge*>(getForwardPath().getLastEdge())->getRoadChunk());
			RoadChunk* firstChunkOfSecondPart(static_cast<const RoadChunkEdge*>(other.getForwardPath().getEdge(0))->getRoadChunk());
			double length(lastChunkOfFirstPart->getMetricOffset() - firstChunkOfSecondPart->getMetricOffset());
			size_t rankShift(lastChunkOfFirstPart->getRankInPath() - firstChunkOfSecondPart->getRankInPath());

			// Remove the last road chunk
			lastChunkOfFirstPart->unlink();
			lastChunkOfFirstPart->setRoad(NULL);

			// Shift the ranks and metric offsets of the second part and attach to the first one
			std::vector<RoadChunk*> secondPartChunks;
			BOOST_FOREACH(Edge* edge, other.getForwardPath()._edges)
			{
				secondPartChunks.push_back(static_cast<RoadChunkEdge*>(edge)->getRoadChunk());
			}
			BOOST_FOREACH(RoadChunk* roadChunk, secondPartChunks)
			{
				roadChunk->unlink();
				roadChunk->setRankInPath(roadChunk->getRankInPath() + rankShift);
				roadChunk->setMetricOffset(roadChunk->getMetricOffset() + length);
				roadChunk->setRoad(this);
			}
			BOOST_FOREACH(RoadChunk* roadChunk, secondPartChunks)
			{
				roadChunk->link(fakeEnv);
			}

			// Detach the second road part from the road place
			other.unlink();
			other.get<RoadPlace::Vector>().clear();
		}

		boost::optional<RoadPlace&>
		Road::getAnyRoadPlace() const
		{
			if (get<RoadPlace::Vector>().empty())
			{
				return boost::none;
			}
			else
			{
				return boost::optional<RoadPlace&>(*get<RoadPlace::Vector>().front());
			}
		}

}	}
