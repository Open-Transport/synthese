
/** MainRoadPart class header.
	@file MainRoadPart.hpp

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

#ifndef SYNTHESE_road_MainRoadPart_hpp__
#define SYNTHESE_road_MainRoadPart_hpp__

#include "Road.h"
#include "Registry.h"

namespace synthese
{
	namespace road
	{
		class ReverseRoadPart;
		class MainRoadChunk;

		/** MainRoadPart class.
			@ingroup m34
		*/
		class MainRoadPart:
			public Road
		{
			boost::shared_ptr<ReverseRoadPart> _reverseRoad;

		public:
			/// Chosen registry class.
			typedef util::Registry<MainRoadPart>	Registry;

			MainRoadPart(
				util::RegistryKeyType key = 0,
				RoadType type = ROAD_TYPE_UNKNOWN
			);

			bool isReversed() const {return false;};

			//! @name Getters
			//@{
				ReverseRoadPart* getReverseRoad() const { return _reverseRoad.get(); }
			//@}

			//! @name Update methods.
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Inserts a road chunk at the beginning of the road
				/// @param chunk the chunk to add
				/// @param length length of the chunk
				/// @param rankShift
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.1.18
				/// @pre the road must contain at least one chunk
				/// @pre the rank and the metric offset of the chunk to insert must be less
				/// than the rank of the first edge raised by the rankShift.
				/// @pre same condition on the metric offset
				void insertRoadChunk(
					MainRoadChunk& chunk,
					double length,
					std::size_t rankShift
				);



				//////////////////////////////////////////////////////////////////////////
				/// Adds a road chunk in the road at the space specified by the rank in path attribute.
				/// @param chunk the chunk to add
				/// @author Hugues Romain
				void addRoadChunk(
					MainRoadChunk& chunk
				);



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
				void merge(
					MainRoadPart& other
				);


				//////////////////////////////////////////////////////////////////////////
				/// Links the road to a road place.
				/// @param value the road place to link to the current road
				/// The following links are created :
				/// <ul>
				///	<li>the road to the road place</li>
				///	<li>the road is added to the road place</li>
				///	<li>the reverse road to the road place (if exists)</li>
				///	<li>the reverse road is added to the road place (if exists)</li>
				/// </ul>
				/// If the road was already linked to an other place, then the corresponding links
				/// are broken before.
				void setRoadPlace(RoadPlace& value);
			//@}
		};
	}
}

#endif // SYNTHESE_road_MainRoadPart_hpp__
