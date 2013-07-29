
/** TimetableTableSync class header.
	@file TimetableTableSync.h
	@author Hugues Romain
	@date 2008

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

#ifndef SYNTHESE_TimetableTableSync_H__
#define SYNTHESE_TimetableTableSync_H__

#include "DBDirectTableSyncTemplate.hpp"
#include "OldLoadSavePolicy.hpp"

#include <vector>
#include <string>
#include <iostream>

namespace synthese
{
	namespace timetables
	{
		class Timetable;

		/** Timetable table synchronizer.
			@ingroup m55LS refLS
		*/
		class TimetableTableSync:
			public db::DBDirectTableSyncTemplate<
				TimetableTableSync,
				Timetable,
				db::FullSynchronizationPolicy,
				db::OldLoadSavePolicy
			>
		{
		public:
			static const std::string COL_BOOK_ID;
			static const std::string COL_RANK;
			static const std::string COL_TITLE;
			static const std::string COL_CALENDAR_ID;
			static const std::string COL_FORMAT;
			static const std::string COL_AUTHORIZED_LINES;
			static const std::string COL_AUTHORIZED_PHYSICAL_STOPS;
			static const std::string COL_TRANSFER_TIMETABLE_BEFORE;
			static const std::string COL_TRANSFER_TIMETABLE_AFTER;
			static const std::string COL_IGNORE_EMPTY_ROWS;
			static const std::string COL_COMPRESSION;

			TimetableTableSync() {}


			/** Timetable search.
				(other search parameters)
				@param first First Timetable object to answer
				@param number Number of Timetable objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@author Hugues Romain
				@date 2006
			*/
			static SearchResult Search(
				util::Env& env,
				boost::optional<util::RegistryKeyType> bookId = boost::optional<util::RegistryKeyType>()
				, bool orderByParent = true
				, bool orderByTitle = false
				, bool raisingOrder = true
				, int first = 0
				, int number = 0,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);


			static void Shift(
				util::RegistryKeyType bookId
				, int rank
				, int delta
			);

			static boost::optional<size_t> GetMaxRank(util::RegistryKeyType bookId);
		};
}	}

#endif // SYNTHESE_TimetableTableSync_H__
