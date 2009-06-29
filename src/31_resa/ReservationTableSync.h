
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

#include "SQLiteNoSyncTableSyncTemplate.h"
#include "Reservation.h"

namespace synthese
{
	namespace time
	{
		class Date;
	}

	namespace resa
	{
		/** Reservation table synchronizer.
			@ingroup m31LS refLS

			@warning The load method does not update the transaction attribute. To do it, load the transaction first and load each reservation which belongs to it.
		*/
		class ReservationTableSync : public db::SQLiteNoSyncTableSyncTemplate<ReservationTableSync,Reservation>
		{
		public:
			static const std::string COL_TRANSACTION_ID;
			static const std::string COL_LINE_ID;
			static const std::string COL_LINE_CODE;
			static const std::string COL_SERVICE_ID;
			static const std::string COL_SERVICE_CODE;
			static const std::string COL_ORIGIN_DATE_TIME;
			static const std::string COL_DEPARTURE_PLACE_ID;
			static const std::string COL_DEPARTURE_PLACE_NAME;
			static const std::string COL_DEPARTURE_TIME;
			static const std::string COL_ARRIVAL_PLACE_ID;
			static const std::string COL_ARRIVAL_PLACE_NAME;
			static const std::string COL_ARRIVAL_TIME;
			static const std::string COL_RESERVATION_RULE_ID;
			static const std::string COL_RESERVATION_DEAD_LINE;

			ReservationTableSync();
			~ReservationTableSync();


			/** Reservation search.
				The returned reservations includes their corresponding transaction as shared pointer.
				@param transaction Transaction
				@param first First Reservation object to answer
				@param number Number of Reservation objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return vector<Reservation*> Founded Reservation objects.
				@author Hugues Romain
				@date 2006
			*/
			static void Search(
				util::Env& env,
				const util::RegistryKeyType transactionId
				, int first = 0
				, int number = 0,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);


			/** Search of reservations by line and date.
				The returned reservations includes their corresponding transaction as shared pointer.
				@param commercialLineId Commercial line ID
				@param day Day of departure of the service at its origin (not necessarily the day of the departure of the customer)
				@param first First Reservation object to answer
				@param number Number of Reservation objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return vector<Reservation*> Founded Reservation objects.
				@author Hugues Romain
				@date 2009
			*/
			static void Search(
				util::Env& env,
				const util::RegistryKeyType commercialLineId,
				const time::Date& day,
				const boost::optional<util::RegistryKeyType> serviceId = boost::optional<util::RegistryKeyType>(),
				bool orderByService = true,
				bool raisingOrder = true, 
				int first = 0,
				int number = 0,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);
		};
	}
}

#endif // SYNTHESE_ReservationTableSync_H__
