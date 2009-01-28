
/** ReservationTransactionTableSync class header.
	@file ReservationTransactionTableSync.h
	@author Hugues Romain
	@date 2007

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

#ifndef SYNTHESE_ReservationTransactionTableSync_H__
#define SYNTHESE_ReservationTransactionTableSync_H__


#include <vector>
#include <string>
#include <iostream>

#include "02_db/SQLiteNoSyncTableSyncTemplate.h"

namespace synthese
{
	namespace graph
	{
		class Service;
	}

	namespace time
	{
		class Date;
		class DateTime;
	}

	namespace resa
	{
		class ReservationTransaction;

		/** ReservationTransaction table synchronizer.
			@ingroup m31LS refLS
		*/
		class ReservationTransactionTableSync : public db::SQLiteNoSyncTableSyncTemplate<ReservationTransactionTableSync,ReservationTransaction>
		{
		public:
			static const std::string COL_LAST_RESERVATION_ID;
			static const std::string COL_SEATS;
			static const std::string COL_BOOKING_TIME;
			static const std::string COL_CANCELLATION_TIME;
			static const std::string COL_CUSTOMER_ID;
			static const std::string COL_CUSTOMER_NAME;
			static const std::string COL_CUSTOMER_PHONE;
			static const std::string COL_BOOKING_USER_ID;
			static const std::string COL_CANCEL_USER_ID;
			static const std::string COL_CUSTOMER_EMAIL;
			
			ReservationTransactionTableSync();
			~ReservationTransactionTableSync();


			/** ReservationTransaction search.
				The returned reservations transactions includes their child reservations as shared pointer.
				@param service Booked service
				@param originDate Date of departure of the service, according to the first schedule
				@param withCancelled true = return also the canceled reservations, false = hide it
				@param first First ReservationTransaction object to answer
				@param number Number of ReservationTransaction objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return vector<ReservationTransaction> Founded ReservationTransaction objects.
				@author Hugues Romain
				@date 2006
			*/
			static void Search(
				util::Env& env,
				const util::RegistryKeyType serviceId
				, const time::Date& originDate
				, bool withCancelled
				, int first = 0
				, int number = 0,
				util::LinkLevel linkLevel = util::FIELDS_ONLY_LOAD_LEVEL
			);


			static void Search(
				util::Env& env,
				uid userId
				, const time::DateTime& minDate
				, const time::DateTime& maxDate
				, bool withCancelled = false
				, int first = 0
				, int number = 0,
				util::LinkLevel linkLevel = util::FIELDS_ONLY_LOAD_LEVEL
			);
		};
	}
}

#endif // SYNTHESE_ReservationTransactionTableSync_H__
