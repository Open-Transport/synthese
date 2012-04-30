
/** PublicPlaceTableSync class header.
	@file PublicPlaceTableSync.h

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

#ifndef SYNTHESE_PublicPlaceTableSync_H__
#define SYNTHESE_PublicPlaceTableSync_H__

#include "PublicPlace.h"

#include "DBRegistryTableSyncTemplate.hpp"
#include "FetcherTemplate.h"

#include <vector>
#include <string>
#include <iostream>

namespace synthese
{
	namespace road
	{
		/** PublicPlace table synchronizer.
			@ingroup m34LS refLS
		*/
		class PublicPlaceTableSync:
			public db::DBRegistryTableSyncTemplate<PublicPlaceTableSync, PublicPlace>,
			public db::FetcherTemplate<geography::NamedPlace, PublicPlaceTableSync>
		{
		public:
			static const std::string COL_NAME;
			static const std::string COL_CITYID;
			static const std::string COL_DATASOURCE_LINKS;



			/** PublicPlace search.
				(other search parameters)
				@param first First PublicPlace object to answer
				@param number Number of PublicPlace objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return Found PublicPlace objects.
				@author Hugues Romain
				@date 2006
			*/
			static SearchResult Search(
				util::Env& env,
				boost::optional<util::RegistryKeyType> cityId = boost::optional<util::RegistryKeyType>(),
				boost::optional<std::string> name = boost::optional<std::string>(),
				int first = 0,
				boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);

		};
	}
}

#endif // SYNTHESE_PublicPlaceTableSync_H__
