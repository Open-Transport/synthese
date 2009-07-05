
/** TimetableRowTableSync class header.
	@file TimetableRowTableSync.h
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

#ifndef SYNTHESE_TimetableRowTableSync_H__
#define SYNTHESE_TimetableRowTableSync_H__


#include <vector>
#include <string>
#include <iostream>

#include "SQLiteNoSyncTableSyncTemplate.h"

namespace synthese
{
	namespace timetables
	{
		class TimetableRow;

		/** TimetablesRow table synchronizer.
			@ingroup m55LS refLS
		*/
		class TimetableRowTableSync:
			public db::SQLiteNoSyncTableSyncTemplate<TimetableRowTableSync,TimetableRow>
		{
		public:
			static const std::string COL_TIMETABLE_ID;
			static const std::string COL_RANK;
			static const std::string COL_PLACE_ID;
			static const std::string COL_IS_DEPARTURE;
			static const std::string COL_IS_ARRIVAL;
			static const std::string COL_IS_COMPULSORY;
			
			

			/** TimetablesRow search.
				@param env Environment to populate
				@param timetableId ID of the timetable which the rows must belong
				@param first First TimetablesRow object to answer
				@param number Number of TimetablesRow objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return Found TimetablesRow objects.
				@author Hugues Romain
				@date 2006
			*/
			static SearchResult Search(
				util::Env& env,
				uid timetableId = UNKNOWN_VALUE
				, bool orderByTimetable = true
				, bool raisingOrder = true
				, int first = 0
				, boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				util::LinkLevel linkLevel = util::FIELDS_ONLY_LOAD_LEVEL
			);



			static void Shift(
				uid timetableId
				, int rank
				, int delta
				);

			static int GetMaxRank(uid timetableId);

		};
	}
}

#endif // SYNTHESE_TimetableRowTableSync_H__
