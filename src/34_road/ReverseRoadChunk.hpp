
/** ReverseRoadChunk class header.
	@file ReverseRoadChunk.hpp

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

#ifndef SYNTHESE_road_ReverseRoadChunk_hpp__
#define SYNTHESE_road_ReverseRoadChunk_hpp__

#include "RoadChunk.h"
#include "Registry.h"

namespace synthese
{
	namespace road
	{
		class ReverseRoadPart;

		//////////////////////////////////////////////////////////////////////////
		/// Road chunk generated by inversion of a main road chunk.
		///	@ingroup m34
		///	@author Hugues Romain
		///	@date 2010
		///	@since 3.2.0
		class ReverseRoadChunk:
			public RoadChunk
		{
		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			/// @param id identifier (default 0)
			/// @param fromCrossing crossing where the chunk begins (default NULL)
			/// @param rankInRoad rank of the chunk in the road path (default unknown)
			/// @param road road which the chunk belongs to (default NULL)
			/// @param metricOffset distance between the the chunk beginning and the road beginning (default unknown)
			ReverseRoadChunk(
				util::RegistryKeyType id = 0,
				Crossing* fromCrossing = NULL,
				int rankInRoad = UNKNOWN_VALUE,
				ReverseRoadPart* road = NULL,
				double metricOffset = UNKNOWN_VALUE
			);


			/// Chosen registry class.
			typedef util::Registry<ReverseRoadChunk>	Registry;

		private:
		};
	}
}

#endif // SYNTHESE_road_ReverseRoadChunk_hpp__
