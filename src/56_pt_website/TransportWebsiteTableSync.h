
/** TransportWebsiteTableSync class header.
	@file TransportWebsiteTableSync.h

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

#ifndef SYNTHESE_SiteTableSync_H__
#define SYNTHESE_SiteTableSync_H__

#include "SQLiteRegistryTableSyncTemplate.h"
#include "TransportWebsite.h"
#include "FetcherTemplate.h"

#include <string>
#include <iostream>

namespace synthese
{
	namespace transportwebsite
	{
		//////////////////////////////////////////////////////////////////////////
		/// 56.10 Table : Transport web site.
		///	@ingroup m56LS refLS
		/// @author Hugues Romain
		//////////////////////////////////////////////////////////////////////////
		/// Table name : t025_sites
		class TransportWebsiteTableSync:
			public db::SQLiteRegistryTableSyncTemplate<TransportWebsiteTableSync,TransportWebsite>,
			public db::FetcherTemplate<cms::Website, TransportWebsiteTableSync>
		{
		public:
			static const std::string TABLE_COL_NAME;
			static const std::string TABLE_COL_START_DATE;
			static const std::string TABLE_COL_END_DATE;
			static const std::string TABLE_COL_ONLINE_BOOKING;
			static const std::string TABLE_COL_USE_OLD_DATA;
			static const std::string COL_MAX_CONNECTIONS;
			static const std::string COL_USE_DATES_RANGE;
			static const std::string COL_PERIODS;
			static const std::string COL_DISPLAY_ROAD_APPROACH_DETAILS;
			static const std::string COL_CLIENT_URL;
			static const std::string COL_DEFAULT_PAGE_TEMPLATE_ID;



			/** TransportWebsite search.
				(other search parameters)
				@param first First TransportWebsite object to answer
				@param number Number of TransportWebsite objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return Found TransportWebsite objects.
				@author Hugues Romain
				@date 2006
			*/
			static SearchResult Search(
				util::Env& env,
				std::string name = std::string()
				, int first = 0
				, boost::optional<std::size_t> number = boost::optional<std::size_t>()
				, bool orderByName = true
				, bool raisingOrder = true,
				util::LinkLevel linkLevel = util::FIELDS_ONLY_LOAD_LEVEL
			);
		};
	}
}

#endif // SYNTHESE_SiteTableSync_H__
