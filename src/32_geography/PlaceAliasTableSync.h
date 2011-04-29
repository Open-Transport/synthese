
/** PlaceAliasTableSync class header.
	@file PlaceAliasTableSync.h

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

#ifndef SYNTHESE_PlaceAliasTableSync_H__
#define SYNTHESE_PlaceAliasTableSync_H__

#include "PlaceAlias.h"

#include "DBRegistryTableSyncTemplate.hpp"

#include <vector>
#include <string>
#include <iostream>

namespace synthese
{
	namespace geography
	{
		/** PlaceAlias table synchronizer.
			@ingroup m32LS refLS
		*/
		class PlaceAliasTableSync:
			public db::DBRegistryTableSyncTemplate<PlaceAliasTableSync,PlaceAlias>
		{
		public:
			static const std::string COL_NAME;
			static const std::string COL_ALIASEDPLACEID;
			static const std::string COL_CITYID;
			static const std::string COL_ISCITYMAINCONNECTION;

			PlaceAliasTableSync();


			/** PlaceAlias search.
				(other search parameters)
				@param first First PlaceAlias object to answer
				@param number Number of PlaceAlias objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return vector<PlaceAlias> Founded PlaceAlias objects.
				@author Hugues Romain
				@date 2006
			*/
			static void Search(
				util::Env& env,
				int first = 0, int number = 0,
				util::LinkLevel linkLevel = util::FIELDS_ONLY_LOAD_LEVEL
			);
		};
	}
}

#endif // SYNTHESE_PlaceAliasTableSync_H__
