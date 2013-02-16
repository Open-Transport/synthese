
/** WebsiteConfigTableSync class header.
	@file WebsiteConfigTableSync.hpp

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

#ifndef SYNTHESE_WebsiteConfigTableSync_H__
#define SYNTHESE_WebsiteConfigTableSync_H__

#include "WebsiteConfig.hpp"

#include <string>
#include <iostream>

#include "DBDirectTableSyncTemplate.hpp"

namespace synthese
{
	namespace cms
	{
		//////////////////////////////////////////////////////////////////////////
		/// 36.10 Table : Website configuration.
		///	@ingroup m36LS refLS
		/// @author Hugues Romain
		/// @date 2013
		/// @since 3.6.0
		//////////////////////////////////////////////////////////////////////////
		class WebsiteConfigTableSync:
			public db::DBDirectTableSyncTemplate<WebsiteConfigTableSync, WebsiteConfig>
		{
		public:
			//////////////////////////////////////////////////////////////////////////
			/// Website configuration search.
			/// @param env Environment to use and populate by the search
			/// @param siteId Id of the site the returned pages must belong
			/// @param first First page to answer
			/// @param number Number pages to answer. Undefined value seems unlimited size.
			/// @return Found website configurations.
			/// @author Hugues Romain
			/// @date 2013
			/// @since 3.6.0
			static SearchResult Search(
				util::Env& env,
				boost::optional<util::RegistryKeyType> siteId = boost::optional<util::RegistryKeyType>(),
				int first = 0,
				boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);
		};
}	}

#endif
