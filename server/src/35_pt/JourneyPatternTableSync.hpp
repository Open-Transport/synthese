
/** JourneyPatternTableSync class header.
	@file JourneyPatternTableSync.hpp

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

#ifndef SYNTHESE_LineTableSync_H__
#define SYNTHESE_LineTableSync_H__


#include <vector>
#include <string>
#include <iostream>

#include "JourneyPattern.hpp"

#include "DBDirectTableSyncTemplate.hpp"

namespace synthese
{
	namespace pt
	{
		class JourneyPattern;

		/** JourneyPattern table synchronizer.
			@ingroup m35LS refLS

		Lines table :
			- on insert :
			- on update :
			- on delete : X
		*/
		class JourneyPatternTableSync:
			public db::DBDirectTableSyncTemplate<
				JourneyPatternTableSync,
				JourneyPattern,
				db::FullSynchronizationPolicy
			>
		{
		public:
			/** JourneyPattern search.
				@param commercialLineId Id of commercial line which must belong the results (undefined = all lines)
				@param first First JourneyPattern object to answer
				@param number Number of JourneyPattern objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return Found JourneyPattern objects.
				@author Hugues Romain
				@date 2006
			*/
			static SearchResult Search(
				util::Env& env,
				boost::optional<util::RegistryKeyType> commercialLineId = boost::optional<util::RegistryKeyType>(),
				int first = 0,
				boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				bool orderByName = true,
				bool raisingOrder = true,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL,
				boost::optional<bool> wayback = boost::optional<bool>()
			);

			virtual bool allowList( const server::Session* session ) const;
		};
}	}

#endif // SYNTHESE_LineTableSync_H__
