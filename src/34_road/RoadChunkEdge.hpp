
/** RoadChunkEdge class header.
	@file RoadChunkEdge.hpp

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

#ifndef SYNTHESE_road_RoadChunkEdge_hpp__
#define SYNTHESE_road_RoadChunkEdge_hpp__

#include "Edge.h"

namespace synthese
{
	namespace road
	{
		class Crossing;
		class RoadChunk;
		class RoadPath;

		/** RoadChunkEdge class.
			@ingroup m34
		*/
		class RoadChunkEdge:
			public graph::Edge
		{
			friend class RoadChunk;

		private:
			const bool _reverse;
			RoadChunk* _roadChunk;

		public:
			RoadChunkEdge(
				RoadChunk& roadChunk,
				bool reverse
			);

			RoadChunk* getRoadChunk() const { return _roadChunk; }

			virtual bool isDepartureAllowed() const;
			virtual bool isArrivalAllowed() const;

			void link();
			void unlink();

			virtual boost::shared_ptr<geos::geom::LineString> getRealGeometry() const;

			virtual double getCarSpeed() const;
		};
}	}

#endif // SYNTHESE_road_RoadChunkEdge_hpp__

