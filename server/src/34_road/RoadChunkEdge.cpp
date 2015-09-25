
/** RoadChunkEdge class implementation.
	@file RoadChunkEdge.cpp

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

#include "RoadChunkEdge.hpp"

#include "Crossing.h"
#include "RoadChunk.h"
#include "RoadModule.h"
#include "RoadPath.hpp"

#include <geos/geom/LineString.h>

namespace synthese
{
	using namespace graph;

	namespace road
	{
		RoadChunkEdge::RoadChunkEdge(
			RoadChunk& roadChunk,
			bool reverse
		):	Edge(
				NULL,
				0,
				roadChunk.getFromCrossing(),
				0
			),
			_reverse(reverse),
			_roadChunk(&roadChunk)
		{}




		bool RoadChunkEdge::isDepartureAllowed() const
		{
			return true;
		}



		bool RoadChunkEdge::isArrivalAllowed() const
		{
			return true;
		}



		void RoadChunkEdge::link()
		{
			// Links from stop to the linestop
			if(_fromVertex)
			{
				_fromVertex->addArrivalEdge(static_cast<Edge*>(this));
				_fromVertex->addDepartureEdge(static_cast<Edge*>(this));

				// Useful transfer calculation
				if(getHub())
				{
					getHub()->clearAndPropagateUsefulTransfer(RoadModule::GRAPH_ID);
				}
			}

			// Insertion in the path
			if(_parentPath)
			{
				_parentPath->addEdge(*this);

				BOOST_FOREACH(const Path::ServiceCollections::value_type& itCollection, _parentPath->getServiceCollections())
				{
					markServiceIndexUpdateNeeded(*itCollection, false);
				}
			}
		}



		void RoadChunkEdge::unlink()
		{
			if(_fromVertex)
			{
				// Links from stop to the linestop
				_fromVertex->removeArrivalEdge(static_cast<Edge*>(this));
				_fromVertex->removeDepartureEdge(static_cast<Edge*>(this));

				// Useful transfer calculation
				if(getHub())
				{
					getHub()->clearAndPropagateUsefulTransfer(RoadModule::GRAPH_ID);
				}
			}

			if(getParentPath())
			{
				BOOST_FOREACH(const Path::ServiceCollections::value_type& itCollection, getParentPath()->getServiceCollections())
				{
					markServiceIndexUpdateNeeded(*itCollection, false);
				}

				_parentPath->removeEdge(*this);
			}
		}



		boost::shared_ptr<geos::geom::LineString> RoadChunkEdge::getRealGeometry() const
		{
			boost::shared_ptr<geos::geom::LineString> tmpGeom;
			if(_reverse)
			{
				if(!this->getNext())
				{
					return boost::shared_ptr<geos::geom::LineString>();
				}

				tmpGeom = static_cast<const RoadChunkEdge*>(this->getNext())->_roadChunk->getGeometry();
				if (tmpGeom)
				{
					tmpGeom.reset(dynamic_cast<geos::geom::LineString*>(tmpGeom->reverse()));
				}
			}
			else
			{
				tmpGeom = _roadChunk->getGeometry();
			}

			if(tmpGeom && !tmpGeom->isEmpty())
			{
				return tmpGeom;
			}
			else if (getFromVertex())
			{
				return graph::Edge::getRealGeometry();
			}
			else
			{
				return boost::shared_ptr<geos::geom::LineString>();
			}
		}



		double RoadChunkEdge::getCarSpeed() const
		{
			return _roadChunk->getCarSpeed();
		}
}	}

