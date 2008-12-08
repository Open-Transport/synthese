
/** ContinuousServiceTableSync class header.
	@file ContinuousServiceTableSync.h

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

#ifndef SYNTHESE_ContinuousServiceTableSync_H__
#define SYNTHESE_ContinuousServiceTableSync_H__

#include "ContinuousService.h"

#include <vector>
#include <string>
#include <iostream>

#include "SQLiteRegistryTableSyncTemplate.h"

namespace synthese
{
	namespace env
	{
		/** ContinuousService table synchronizer.
			@ingroup m35LS refLS
		*/
		class ContinuousServiceTableSync : public db::SQLiteRegistryTableSyncTemplate<ContinuousServiceTableSync,ContinuousService>
		{
		public:
			static const std::string COL_SERVICENUMBER;
			static const std::string COL_SCHEDULES;
			static const std::string COL_PATHID;
			static const std::string COL_RANGE;
			static const std::string COL_MAXWAITINGTIME;
			static const std::string COL_BIKECOMPLIANCEID;
			static const std::string COL_HANDICAPPEDCOMPLIANCEID;
			static const std::string COL_PEDESTRIANCOMPLIANCEID;
			
			ContinuousServiceTableSync();


			/** ContinuousService search.
				@param lineId Line
				@param first First ContinuousService object to answer
				@param number Number of ContinuousService objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return vector<ContinuousService*> Founded ContinuousService objects.
				@author Hugues Romain
				@date 2006
			*/
			static void Search(
				util::Env& env,
				uid lineId = UNKNOWN_VALUE
				, int first = 0
				, int number = 0
				, bool orderByDepartureTime = true
				, bool raisingOrder = true,
				util::LinkLevel linkLevel = util::FIELDS_ONLY_LOAD_LEVEL
			);



			/** The schedules indexes of each linestop are updated after the whole first sync.
				@param sqlite SQLite thread
				@param sync SQLite synchonizer
				@author Hugues Romain
				@date 2007				
			*/
			void afterFirstSync (db::SQLite* sqlite, 
				db::SQLiteSync* sync);

		};
	}
}

#endif // SYNTHESE_ContinuousServiceTableSync_H__
