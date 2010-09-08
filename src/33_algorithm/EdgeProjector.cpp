
/** EdgeProjector class implementation.
	@file EdgeProjector.cpp

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

#include "EdgeProjector.hpp"
#include "PathGroup.h"
#include "Path.h"
#include "Edge.h"
#include "Vertex.h"
#include "Log.h"

#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"

#include <geos/geom/Geometry.h>
#include <geos/geom/Point.h>
#include <geos/geom/LineString.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/linearref/LinearLocation.h>
#include <geos/linearref/LengthIndexedLine.h>

using namespace std;
using namespace geos::geom;
using namespace geos::linearref;

namespace synthese
{
	using namespace graph;

	namespace algorithm
	{
		EdgeProjector::EdgeProjector(
			const EdgeProjector::From& from,
			double distanceLimit
		):	_from(from),
			_distanceLimit(distanceLimit)
		{}



		EdgeProjector::~EdgeProjector() {
		}


		EdgeProjector::PathNearby EdgeProjector::projectEdge(
			const Coordinate& pt
		) const {
		   PathsNearby candidates = getPathsByDistance(pt);
		   if(candidates.empty())
		   {
			   throw NotFoundException();
		   }
		   EdgeProjector::PathNearby bestRoadNearby = candidates.begin()->second;
		   return bestRoadNearby;

		}



		EdgeProjector::PathsNearby EdgeProjector::getPathsByDistance(
			const Coordinate& pt
		) const {
			PathsNearby ret;
			if(pt.isNull())
			{
				return ret;
			}

			const GeometryFactory *gf = GeometryFactory::getDefaultInstance();
			Point* ptGeom(gf->createPoint(pt));

			typedef map<PathGroup*, EdgeProjector::PathsNearby::iterator> ListByPathGroup;
			ListByPathGroup listByPathGroup;

			BOOST_FOREACH(Edge* edge, _from)
			{
				boost::shared_ptr<LineString> edgeGeom = edge->getGeometry();
				if(!edgeGeom.get())
				{
					continue;
				}

				// optimization: first compute distance to road bbox
				double bboxdistance = edgeGeom->getEnvelope()->distance(ptGeom);
				if(bboxdistance >= _distanceLimit)
				{
					continue;
				}

				// real distance
				double distance = edgeGeom->distance(ptGeom);
				if(distance >= _distanceLimit)
				{
					continue;
				}

				// tests if this pathgroup has already been found at a shorter distance
				ListByPathGroup::iterator it(listByPathGroup.find(edge->getParentPath()->getPathGroup()));
				if(it != listByPathGroup.end() && it->second->first < distance)
				{
					continue;
				}
				ret.erase(it->second);
				listByPathGroup.erase(it);

				// projection
				LengthIndexedLine lil(static_cast<const Geometry*>(edgeGeom.get()));
				const Coordinate *ptCoords = ptGeom->getCoordinate();
				Coordinate coord(*ptCoords);
				double index = lil.project(coord);
				Coordinate projectedCoords = lil.extractPoint(index);
				listByPathGroup.insert(
					make_pair(
						edge->getParentPath()->getPathGroup(),
						ret.insert(std::make_pair(
							distance,
							boost::make_tuple(
									projectedCoords,
									edge,
									index
				)	)	)	)	);
			}
			return ret;
		}
}	}
