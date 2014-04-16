
/** EdgeProjector class header.
	@file EdgeProjector.hpp

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

#ifndef EDGE_PROJECTOR_H_
#define EDGE_PROJECTOR_H_

#include "Exception.h"
#include "DBModule.h"
#include "AccessParameters.h"
#include "RoadChunkEdge.hpp"

#include <map>
#include <vector>
#include <boost/tuple/tuple.hpp>
#include <geos/geom/Geometry.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/Point.h>
#include <geos/geom/LineString.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/linearref/LinearLocation.h>
#include <geos/linearref/LengthIndexedLine.h>

namespace synthese
{
	namespace algorithm
	{

		//////////////////////////////////////////////////////////////////////////
		/// Projects a point to the nearest edge(s). T should be pointer or shared pointer
		/// @author Thomas Bonfort, Hugues Romain
		/// @date 2010
		/// @since 3.2.0
		template<class T>
		class EdgeProjector
		{
		public:
			typedef boost::tuple<
				geos::geom::Coordinate, //! coordinates of the projection of the point on the line
				T, //!corresponding road
				double //! metric offset of the projected point
			> PathNearby;

			typedef std::multimap<
				double,//!distance of the point from the road
				PathNearby
			> PathsNearby;

			typedef std::vector<T> From;
			typedef std::set<graph::UserClassCode> CompatibleUserClassesRequired;

			class NotFoundException:
			   public synthese::Exception
			{
			public:
			   NotFoundException():
				Exception("No object was found to project the point")
				{
				}
			};

		private:
			From _from;
			double _distanceLimit;
			const CompatibleUserClassesRequired _requiredUserClasses;

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			///	@param from paths to attempt to project points
			///	@param distance_limit max distance between point and path to be relevant
			/// @author Thomas Bonfort
			/// @date 2010
			/// @since 3.2.0
			EdgeProjector(
				const From& from,
				double distance_limit=30,
				const CompatibleUserClassesRequired requiredUserClasses = CompatibleUserClassesRequired()
			):	_from(from),
				_distanceLimit(distance_limit),
				_requiredUserClasses(requiredUserClasses)
			{}

			virtual ~EdgeProjector() {}



			//////////////////////////////////////////////////////////////////////////
			/// Gets the best projection.
			/// @param pt point to project
			/// @return the nearest point on the graph
			PathNearby projectEdge(
				const geos::geom::Coordinate& pt
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets all relevant projections.
			/// @param pt point to project
			/// @return a projected point per path of the graph
			/// Only one edge per path group is returned
			PathsNearby getPathsByDistance(
				const geos::geom::Coordinate& pt
			) const;
		};



		template<class T>
		typename EdgeProjector<T>::PathNearby EdgeProjector<T>::projectEdge(
			const geos::geom::Coordinate& pt
		) const	{
			if(pt.isNull())
			{
				throw NotFoundException();
			}

			const geos::geom::GeometryFactory& gf(CoordinatesSystem::GetDefaultGeometryFactory());
			boost::shared_ptr<geos::geom::Point> ptGeom(gf.createPoint(pt));

			boost::shared_ptr<geos::geom::LineString> bestEdgeGeom;
			T bestEdge;
			double bestDistance;

			BOOST_FOREACH(T edge, _from)
			{
				boost::shared_ptr<geos::geom::LineString> edgeGeom = edge->getRealGeometry();
				if(!edgeGeom.get() || edgeGeom->isEmpty())
				{
					continue;
				}

				bool compatibleWithUserClasses(true);
				BOOST_FOREACH(graph::UserClassCode userClassCode, _requiredUserClasses)
				{
					if(	!edge->getForwardEdge().isCompatibleWith(graph::AccessParameters(userClassCode)) &&
						!edge->getReverseEdge().isCompatibleWith(graph::AccessParameters(userClassCode)))
					{
						compatibleWithUserClasses = false;
						break;
					}
				}

				if(!compatibleWithUserClasses)
				{
					continue;
				}

				// real distance
				double distance = edgeGeom->distance(ptGeom.get());
				if(	distance >= _distanceLimit ||
					(bestEdgeGeom.get() && bestDistance < distance)
				){
					continue;
				}

				bestEdgeGeom = edgeGeom;
				bestEdge = edge;
				bestDistance = distance;
			}

			if(!bestEdgeGeom.get())
			{
				throw NotFoundException();
			}

			// projection
			geos::linearref::LengthIndexedLine lil(static_cast<const geos::geom::Geometry*>(bestEdgeGeom.get()));
			const geos::geom::Coordinate *ptCoords = ptGeom->getCoordinate();
			geos::geom::Coordinate coord(*ptCoords);
			double index = lil.project(coord);
			geos::geom::Coordinate projectedCoords = lil.extractPoint(index);
			return boost::make_tuple(
				projectedCoords,
				bestEdge,
				index
			);
		}



		template<class T>
		typename EdgeProjector<T>::PathsNearby EdgeProjector<T>::getPathsByDistance(
			const geos::geom::Coordinate& pt
		) const	{
			PathsNearby ret;
			if(pt.isNull())
			{
				return ret;
			}

			const geos::geom::GeometryFactory& gf(CoordinatesSystem::GetDefaultGeometryFactory());
			boost::shared_ptr<geos::geom::Point> ptGeom(gf.createPoint(pt));

			BOOST_FOREACH(T edge, _from)
			{
				boost::shared_ptr<geos::geom::LineString> edgeGeom = edge->getGeometry();
				if(!edgeGeom.get() || edgeGeom->isEmpty())
				{
					continue;
				}

				// real distance
				double distance = edgeGeom->distance(ptGeom.get());
				if(distance >= _distanceLimit)
				{
					continue;
				}

				// projection
				geos::linearref::LengthIndexedLine lil(static_cast<const geos::geom::Geometry*>(edgeGeom.get()));
				const geos::geom::Coordinate *ptCoords = ptGeom->getCoordinate();
				geos::geom::Coordinate coord(*ptCoords);
				double index = lil.project(coord);
				geos::geom::Coordinate projectedCoords = lil.extractPoint(index);
				ret.insert(
					std::make_pair(
						distance,
						boost::make_tuple(
							projectedCoords,
							edge,
							index
				)	)	);
			}
			return ret;
		}
}	}

#endif /* LINESTOPPROJECTOR_H_ */
