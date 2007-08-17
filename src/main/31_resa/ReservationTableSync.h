
/** ReservationTableSync class header.
	@file ReservationTableSync.h

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

#ifndef SYNTHESE_ReservationTableSync_H__
#define SYNTHESE_ReservationTableSync_H__


#include <vector>
#include <string>
#include <iostream>

#include "02_db/SQLiteTableSyncTemplate.h"

namespace synthese
{
	namespace resa
	{
		class Reservation;

		/** Reservation table synchronizer.
			@ingroup m31LS refLS
		*/
		class ReservationTableSync : public db::SQLiteTableSyncTemplate<Reservation>
		{
		public:
			static const std::string COL_LINE_ID;
			static const std::string COL_LINE_CODE;
			static const std::string COL_SERVICE_CODE;
			static const std::string COL_DEPARTURE_PLACE_ID;
			static const std::string COL_DEPARTURE_PLACE_NAME;
			static const std::string COL_DEPARTURE_TIME;
			static const std::string COL_ARRIVAL_PLACE_ID;
			static const std::string COL_ARRIVAL_PLACE_NAME;
			static const std::string COL_ARRIVAL_TIME;
			static const std::string COL_RESERVATION_RULE_ID;

			ReservationTableSync();
			~ReservationTableSync();


			/** Reservation search.
				(other search parameters)
				@param first First Reservation object to answer
				@param number Number of Reservation objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return vector<Reservation*> Founded Reservation objects.
				@author Hugues Romain
				@date 2006
			*/
			static std::vector<boost::shared_ptr<Reservation> > search(
				// other search parameters ,
				int first = 0, int number = 0);


		protected:

			/** Action to do on Reservation creation.
				This method loads a new object in ram.
			*/
			void rowsAdded (db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResultSPtr& rows, bool isFirstSync = false);

			/** Action to do on Reservation creation.
				This method updates the corresponding object in ram.
			*/
			void rowsUpdated (db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResultSPtr& rows);

			/** Action to do on Reservation deletion.
				This method deletes the corresponding object in ram and runs 
				all necessary cleaning actions.
			*/
			void rowsRemoved (db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResultSPtr& rows);

		};
	}
}

#endif // SYNTHESE_ReservationTableSync_H__
