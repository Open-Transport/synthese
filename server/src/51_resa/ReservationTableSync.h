
/** ReservationTableSync class header.
	@file ReservationTableSync.h

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

#ifndef SYNTHESE_ReservationTableSync_H__
#define SYNTHESE_ReservationTableSync_H__


#include <vector>
#include <string>
#include <iostream>

#include "DBDirectTableSyncTemplate.hpp"
#include "ConditionalSynchronizationPolicy.hpp"
#include "Reservation.h"

namespace synthese
{
	namespace resa
	{
		/** Reservation table synchronizer.
			@ingroup m51LS refLS

			@warning The load method does not update the transaction attribute. To do it, load the transaction first and load each reservation which belongs to it.
		*/
		class ReservationTableSync:
			public db::DBDirectTableSyncTemplate<
				ReservationTableSync,
				Reservation,
				db::ConditionalSynchronizationPolicy
			>
		{
		public:
			/** Reservation search.
				The returned reservations includes their corresponding transaction as shared pointer.
				@param transaction Transaction
				@param first First Reservation object to answer
				@param number Number of Reservation objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return Found Reservation objects.
				@author Hugues Romain
				@date 2006
			*/
			static SearchResult Search(
				util::Env& env,
				const util::RegistryKeyType transactionId
				, int first = 0
				, boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);



			/** Reservation search.
				The returned reservations includes their corresponding transaction as shared pointer.
				@param minArrivalDate min arrival date
				@param maxDepartureDate max departure date
				@return Found Reservation objects.
				@author Hugues Romain
				@date 2011
				@warning the reservation transactions could not be entirely loaded.
			*/
			static SearchResult Search(
				util::Env& env,
				boost::posix_time::ptime minArrivalDate,
				boost::posix_time::ptime maxDepartureDate,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);




			/** Search of reservations by line and date.
				The returned reservations includes their corresponding transaction as shared pointer.
				@param commercialLineId Commercial line ID
				@param day Day of departure of the service at its origin (not necessarily the day of the departure of the customer)
				@param first First Reservation object to answer
				@param number Number of Reservation objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return Found Reservation objects.
				@author Hugues Romain
				@date 2009
			*/
			static SearchResult Search(
				util::Env& env,
				util::RegistryKeyType commercialLineId,
				const boost::gregorian::date& minDate,
				const boost::gregorian::date& maxDate,
				boost::optional<std::string> serviceNumber = boost::optional<std::string>(),
				boost::logic::tribool cancelled = false,
				bool orderByService = true,
				bool raisingOrder = true,
				int first = 0,
				boost::optional<size_t> number = boost::optional<size_t>(),
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL,
				boost::optional<util::RegistryKeyType> serviceId = boost::optional<util::RegistryKeyType>()
			);



			static SearchResult SearchByService(
				util::Env& env,
				util::RegistryKeyType serviceId,
				boost::optional<boost::posix_time::ptime> departureTime = boost::optional<boost::posix_time::ptime>(),
				boost::optional<boost::posix_time::ptime> arrivalTime = boost::optional<boost::posix_time::ptime>(),
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);
			
			virtual std::string whereClauseDefault(
			) const;

			virtual bool allowList( const server::Session* session ) const;
		};
	}
}

#endif // SYNTHESE_ReservationTableSync_H__
