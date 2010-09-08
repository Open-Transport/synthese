
/** EdgeProjector class header.
	@file EdgeProjector.hpp

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

#ifndef EDGE_PROJECTOR_H_
#define EDGE_PROJECTOR_H_

#include <map>
#include <vector>
#include <boost/tuple/tuple.hpp>
#include <geos/geom/Coordinate.h>
#include "Exception.h"

namespace synthese
{
	namespace graph
	{
		class Edge;
	}

	namespace algorithm
	{

		//////////////////////////////////////////////////////////////////////////
		/// Projects a point to the nearest edge(s).
		/// @author Thomas Bonfort
		/// @date 2010
		/// @since 3.2.0
		class EdgeProjector
		{
		public:
			typedef boost::tuple<
				geos::geom::Coordinate, //! coordinates of the projection of the point on the line
				graph::Edge*, //!corresponding road
			  double //! metric offset of the projected point
			> PathNearby;

			typedef std::multimap<
				double,//!distance of the point from the road
				PathNearby
			> PathsNearby;

			typedef std::vector<graph::Edge*> From;

			class NotFoundException:
			   public util::Exception
			{
			public:
			   NotFoundException():
				util::Exception("No road was found to project the point")
				{
				}
			};

		private:
			From _from;
			double _distanceLimit;

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
				double distance_limit=30
			);

			virtual ~EdgeProjector();



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
}	}

#endif /* LINESTOPPROJECTOR_H_ */
