
/** WebPageTableSync class header.
	@file SiteTableSync.h

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

#ifndef SYNTHESE_WebPageTableSync_H__
#define SYNTHESE_WebPageTableSync_H__

#include "Webpage.h"

#include <string>
#include <iostream>

#include "SQLiteRegistryTableSyncTemplate.h"

namespace synthese
{
	namespace cms
	{
		//////////////////////////////////////////////////////////////////////////
		/// 36.10 Table : Web page.
		///	@ingroup m56LS refLS
		/// @author Hugues Romain
		/// @date 2010
		/// @since 3.1.15
		//////////////////////////////////////////////////////////////////////////
		class WebPageTableSync:
			public db::SQLiteRegistryTableSyncTemplate<WebPageTableSync,Webpage>
		{
		public:
			static const std::string COL_TITLE;
			static const std::string COL_SITE_ID;
			static const std::string COL_UP_ID;
			static const std::string COL_RANK;
			static const std::string COL_CONTENT1;
			static const std::string COL_START_TIME;
			static const std::string COL_END_TIME;
			static const std::string COL_MIME_TYPE;
			static const std::string COL_ABSTRACT;
			static const std::string COL_IMAGE;
			static const std::string COL_LINKS;
			static const std::string COL_DO_NOT_USE_TEMPLATE;
			static const std::string COL_HAS_FORUM;
			static const std::string COL_SMART_URL_PATH;
			static const std::string COL_SMART_URL_DEFAULT_PARAMETER_NAME;
		


			//////////////////////////////////////////////////////////////////////////
			/// Web page search.
			/// @param env Environment to use and populate by the search
			/// @param siteId Id of the site the returned pages must belong
			/// @param parentId Id of the parent page of the returned pages
			/// @param rank Rank of the page
			/// @param first First page to answer
			/// @param number Number pages to answer. Undefined value seems unlimited size.
			/// @param orderByRank the results are sorted by rank
			/// @param orderByTitle the results are sorted alphabetically by title
			/// @param raisingOrder the results are ordered ascendantly (true) or descendantly (false)
			/// @param linkLevel level of link to apply when loading objects
			/// @return Found web pages.
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.15
			static SearchResult Search(
				util::Env& env,
				boost::optional<util::RegistryKeyType> siteId = boost::optional<util::RegistryKeyType>(),
				boost::optional<util::RegistryKeyType> parentId = boost::optional<util::RegistryKeyType>(),
				boost::optional<std::size_t> rank = boost::optional<std::size_t>(),
				int first = 0,
				boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				bool orderByRank = true,
				bool orderByTitle = false,
				bool raisingOrder = true,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);


			typedef std::vector<std::pair<boost::optional<util::RegistryKeyType>, std::string> > SiteWebPagesList;


			//////////////////////////////////////////////////////////////////////////
			/// Builds the list of the pages of a website, formatted to be used in a web form.
			/// @param siteId id of the website
			/// @param rootLabel text to write for the root page link. If empty, the root page is not put on the list.
			/// @param prefix text prefix (used by the recursion)
			/// @param upId id of the top parent page (used by the recursion)
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.15
			static SiteWebPagesList GetPagesList(
				util::RegistryKeyType siteId,
				const std::string& rootLabel,
				std::string prefix = std::string(),
				util::RegistryKeyType upId = 0
			);


			//////////////////////////////////////////////////////////////////////////
			/// Increase or decrease each rank of a siblings collection after a specific rank.
			/// @param siteId ID of the site which must belong the pages to update
			/// @param parentId ID of the parent page to determinate the siblings collection.
			/// @param rank First rank where the modification must be done
			/// @param add true if the rank must be increased, false if the rank must be decreased
			/// @author Hugues Romain
			/// @date 2010
			static void ShiftRank(
				util::RegistryKeyType siteId,
				util::RegistryKeyType parentId,
				std::size_t rank,
				bool add
			);
		};
	}
}

#endif // SYNTHESE_SiteTableSync_H__
