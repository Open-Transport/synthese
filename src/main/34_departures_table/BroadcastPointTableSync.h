
/** BroadcastPointTableSync class header.
	@file BroadcastPointTableSync.h

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

#ifndef SYNTHESE_BroadcastPointTableSync_H__
#define SYNTHESE_BroadcastPointTableSync_H__

#include "02_db/SQLiteTableSyncTemplate.h"

#include "01_util/Constants.h"
#include "01_util/UId.h"

#include <vector>
#include <string>
#include <iostream>

namespace synthese
{
	namespace env
	{
		class ConnectionPlace;
	}

	namespace departurestable
	{
		class BroadcastPoint;

		/** BroadcastPoint table synchronizer.
			@ingroup m34
		*/
		class BroadcastPointTableSync : public db::SQLiteTableSyncTemplate<BroadcastPoint>
		{
		public:
			static const std::string TABLE_COL_NAME;
			static const std::string TABLE_COL_PLACE_ID;
			static const std::string TABLE_COL_PHYSICAL_STOP_ID;

			BroadcastPointTableSync();


			/** BroadcastPoint search.
				(other search parameters)
				@param first First user to answer
				@param number Number of users to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return vector<BroadcastPoint*> Founded currencies.
				@author Hugues Romain
				@date 2006
			*/
			static std::vector<boost::shared_ptr<BroadcastPoint> > search(
				uid placeId = UNKNOWN_VALUE
				, int number = UNKNOWN_VALUE
				, int first=0
				, bool orderByName = true
				, bool raisingOrder = true
				);


		protected:

			/** Action to do on BroadcastPoint creation.
				This method loads a new object in ram.
			*/
			void rowsAdded (const db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows, bool isFirstSync = false);

			/** Action to do on BroadcastPoint creation.
				This method updates the corresponding object in ram.
			*/
			void rowsUpdated (const db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

			/** Action to do on BroadcastPoint deletion.
				This method deletes the corresponding object in ram and runs 
				all necessary cleaning actions.
			*/
			void rowsRemoved (const db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

		};
	}
}

#endif // SYNTHESE_BroadcastPointTableSync_H__
