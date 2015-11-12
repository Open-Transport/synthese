
/** RoadTableSync class header.
	@file RoadTableSync.h

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

#ifndef SYNTHESE_RoadTableSync_H__
#define SYNTHESE_RoadTableSync_H__

#include "Road.h"
#include "DBDirectTableSyncTemplate.hpp"

namespace synthese
{
	namespace road
	{
		//////////////////////////////////////////////////////////////////////////
		/// Road part table synchronizer.
		///	@ingroup m34LS refLS
		/// @author Marc Jambert, Hugues Romain
		/// Fields :
		///		- road_type : see RoadType
		///		-
		///		-
		///		-
		///		- road_place_id : named road which the road part belongs to
		///		- right_side : (1|0) : true = the main Road object is on right side,
		///			false = the main Road object is on left side
		class RoadTableSync:
			public db::DBDirectTableSyncTemplate<RoadTableSync, Road>
		{
		public:



			//////////////////////////////////////////////////////////////////////////
			/// Road path search.
			///	@param env Environment to populate when loading objects
			///	@param roadPlaceId id of the road place the returned objects must belong to
			///	@param cityId id of the city the returned objects must belong to
			///	@param first First Road object to answer
			///	@param number Number of Road objects to return (undefined = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
			/// @param linkLevel level of link
			///	@return Found Road objects.
			///	@author Hugues Romain
			///	@date 2006
			static SearchResult Search(
				util::Env& env,
				boost::optional<util::RegistryKeyType> roadPlaceId = boost::optional<util::RegistryKeyType>(),
				boost::optional<util::RegistryKeyType> cityId = boost::optional<util::RegistryKeyType>(),
//				boost::optional<util::RegistryKeyType> startingNodeId = boost::optional<util::RegistryKeyType>(),
//				boost::optional<util::RegistryKeyType> endingNodeId = boost::optional<util::RegistryKeyType>(),
				int first = 0,
				boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);

			virtual bool allowList( const server::Session* session ) const;
			
		};
	}
}

#endif // SYNTHESE_RoadTableSync_H__
