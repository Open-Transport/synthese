
/** ObjectSiteLinkTableSync class header.
	@file ObjectSiteLinkTableSync.h
	@author Hugues Romain
	@date 2008

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

#ifndef SYNTHESE_ObjectSiteLinkTableSync_H__
#define SYNTHESE_ObjectSiteLinkTableSync_H__


#include <vector>
#include <string>
#include <iostream>

#include "SQLiteRegistryTableSyncTemplate.h"

namespace synthese
{
	namespace transportwebsite
	{
		class ObjectSiteLink;

		/** ObjectSiteLink table synchronizer.
			@ingroup m36LS refLS
		*/
		class ObjectSiteLinkTableSync:
			public db::SQLiteRegistryTableSyncTemplate<ObjectSiteLinkTableSync,ObjectSiteLink>
		{
		public:
			static const std::string COL_OBJECT_ID;
			static const std::string COL_SITE_ID;


			/** ObjectSiteLink search.
				(other search parameters)
				@param first First ObjectSiteLink object to answer
				@param number Number of ObjectSiteLink objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@author Hugues Romain
				@date 2006
			*/
			static SearchResult Search(
				util::Env& env,
				boost::optional<util::RegistryKeyType> siteId = boost::optional<util::RegistryKeyType>(),
				boost::optional<util::RegistryKeyType> objectId = boost::optional<util::RegistryKeyType>(),
				boost::optional<int> objectTableId = boost::optional<int>(),
				int first = 0,
				boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				util::LinkLevel linkLevel = util::FIELDS_ONLY_LOAD_LEVEL
			);

			/** Remove a link between a site and an object specified by their id.
				@param siteId ID of the site
				@param objectId ID of the object. UNKNOWN_VALUE = Removes all links of the alarm
				@author Hugues Romain
				@date 2007				
			*/
			static void Remove(uid siteId, uid objectId = UNKNOWN_VALUE);
		};
	}
}

#endif // SYNTHESE_ObjectSiteLinkTableSync_H__
