
/** TimetableRowGroupTableSync class header.
	@file TimetableRowGroupTableSync.hpp

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

#ifndef SYNTHESE_TimetableRowGroupTableSync_H__
#define SYNTHESE_TimetableRowGroupTableSync_H__

#include "TimetableRowGroup.hpp"

#include <string>
#include <iostream>

#include "DBDirectTableSyncTemplate.hpp"

namespace synthese
{
	namespace timetables
	{
		//////////////////////////////////////////////////////////////////////////
		/// 36.10 Table : Timetable row.
		///	@ingroup m56LS refLS
		/// @author Hugues Romain
		/// @date 2010
		/// @since 3.1.15
		//////////////////////////////////////////////////////////////////////////
		class TimetableRowGroupTableSync:
			public db::DBDirectTableSyncTemplate<TimetableRowGroupTableSync, TimetableRowGroup>
		{
		public:
			//////////////////////////////////////////////////////////////////////////
			/// Web page search.
			/// @param env Environment to use and populate by the search
			/// @param first First page to answer
			/// @param number Number pages to answer. Undefined value seems unlimited size.
			/// @param orderByRank the results are sorted by rank
			/// @param raisingOrder the results are ordered ascendantly (true) or descendantly (false)
			/// @param linkLevel level of link to apply when loading objects
			/// @return Found web pages.
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.15
			static SearchResult Search(
				util::Env& env,
				boost::optional<util::RegistryKeyType> timetableId = boost::optional<util::RegistryKeyType>(),
				boost::optional<size_t> rank = boost::optional<size_t>(),
				int first = 0,
				boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				bool orderByRank = true,
				bool raisingOrder = true,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);
		};
	}
}

#endif // SYNTHESE_SiteTableSync_H__
