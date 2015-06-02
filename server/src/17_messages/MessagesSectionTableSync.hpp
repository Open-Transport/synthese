
/** MessagesSectionTableSync class header.
	@file MessagesSectionTableSync.hpp

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

#ifndef SYNTHESE_messages_MessagesSectionTableSync_hpp__
#define SYNTHESE_messages_MessagesSectionTableSync_hpp__

#include "MessagesSection.hpp"

#include "DBDirectTableSyncTemplate.hpp"

namespace synthese
{
	namespace messages
	{
		//////////////////////////////////////////////////////////////////////////
		/// 56.10 Table : Message section
		///	@ingroup m17LS refLS
		/// @author Hugues Romain
		/// @date 2013
		/// @since 3.7.0
		//////////////////////////////////////////////////////////////////////////
		class MessagesSectionTableSync:
			public db::DBDirectTableSyncTemplate<MessagesSectionTableSync, MessagesSection>
		{
		public:
			/** Message sections search.
			*
			* The size of the vector is less or equal to number, then all
			* sections are returned despite of the number limit. If the size is
			* greater than number (actually equal to number + 1) then there are
			* others sections to show.
			* Test it to know if the situation needs a "click for more" button.
			*
			* @param first First message section to answer
			* @param number Number of Message sections to answer (0 = all). Test after return to know if more results are available.
			* @return Found message sections, from first, limited to number.
			* @author Hugues Romain
			* @date 2013
			*/
			static SearchResult Search(
				util::Env& env,
				int first = 0,
				boost::optional<size_t> number = boost::optional<size_t>(),
				bool raisingOrder = true,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);
		};
}	}

#endif // SYNTHESE_messages_MessagesSectionTableSync_hpp__
