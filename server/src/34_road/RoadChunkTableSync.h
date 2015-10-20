
/** RoadChunkTableSync class header.
	@file RoadChunkTableSync.h

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

#ifndef SYNTHESE_RoadChunkTableSync_H__
#define SYNTHESE_RoadChunkTableSync_H__

#include "RoadChunk.h"

#include "Object.hpp"
#include "DBDirectTableSyncTemplate.hpp"
#include "EdgeProjector.hpp"

#include <vector>
#include <string>
#include <iostream>

namespace geos
{
	namespace geom
	{
		class Point;
	}
}

namespace synthese
{
	namespace algorithm
	{
	}

	namespace road
	{
		class Address;

		//////////////////////////////////////////////////////////////////////////
		/// 34.10 Road chunk table.
		///	@ingroup m35LS refLS
		/// @author Marc Jambert, Hugues Romain
		/// @date 2006
		//////////////////////////////////////////////////////////////////////////
		/// Table name : t014_road_chunks
		///	Corresponding class : RoadChunk
		///
		/// <h2>Fields</h2>
		///	<ul>
		///		<li>rank_in_path</li>
		///		<li>via_points : optional. Defines the geometry of the chunk. Each point is separated
		///		by coma. Longitude and latitude are stored and are separated by :.</li>
		///		<li>metric_offset</li>
		///		<li>left_start_house_number / left_end_house_number / right_start_house_number / right_end_house_number :
		///		optional house number bounds at each side of the chunk.</li>
		///		<li>left_house_numbering_policy / right_house_numbering_policy : These fields are loaded only if the up
		///		links are loaded, because of the need to know the parent road to determinate if the chunk is at
		///		the left or right side of the road
		///	</ul>
		///
		/// <h2>Up links</h2>
		///	<dl>
		///		<dt>address_id</dt><dd>id of the @ref road::CrossingTableSync "crossing" where the chunk begins</dd>
		///		<dt>road_id</dt><dd>id of the @ref road::RoadTableSync "road" which the chunk belongs to</dd>
		///	</dl>
		class RoadChunkTableSync:
				public db::DBDirectTableSyncTemplate<
					RoadChunkTableSync,
					RoadChunk
				>
		{
		public:

			virtual bool allowList( const server::Session* session ) const;

			//////////////////////////////////////////////////////////////////////////
			/// Road chunks search.
			///	@param env Environment to populate when loading objects
			///	@param first First road chunk object to answer
			///	@param number Number of road chunk objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
			/// @param linkLevel level of link
			///	@return Found road chunks objects.
			///	@author Hugues Romain
			///	@date 2006
			static SearchResult Search(
				util::Env& env,
				boost::optional<util::RegistryKeyType> roadId = boost::optional<util::RegistryKeyType>(),
				int first = 0,
				boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);



			//////////////////////////////////////////////////////////////////////////
			/// Projection of a point on a road chunk.
			//////////////////////////////////////////////////////////////////////////
			/// @param point the point to project on the nearest roadchunk
			/// @param maxDistance maximal distance between the point and the road chunk
			/// @retval address the projected point
			/// @pre point must be a valid geometry
			/// @author Hugues Romain
			static void ProjectAddress(
				const geos::geom::Point& point,
				double maxDistance,
				Address& address,
				algorithm::EdgeProjector<boost::shared_ptr<road::RoadChunk> >::CompatibleUserClassesRequired requiredUserClasses = algorithm::EdgeProjector<boost::shared_ptr<road::RoadChunk> >::CompatibleUserClassesRequired()
			);
		};
}	}

#endif // SYNTHESE_RoadChunkTableSync_H__
