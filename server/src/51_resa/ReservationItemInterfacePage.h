
/** ReservationItemInterfacePage class header.
	@file ReservationItemInterfacePage.h
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

#ifndef SYNTHESE_ReservationItemInterfacePage_H__
#define SYNTHESE_ReservationItemInterfacePage_H__

#include "11_interfaces/InterfacePage.h"

#include "01_util/FactorableTemplate.h"

namespace synthese
{
	namespace server
	{
		class Request;
	}

	namespace resa
	{
		class ReservationTransaction;

		/** ReservationItemInterfacePage Interface Page Class.
			@ingroup m51Pages refPages
			@author Hugues Romain
			@date 2008

			@code reservation_transaction @endcode

			Parameters :
				- 0 : Departure place name
				- 1 : Departure time
				- 2 : Arrival place name
				- 3 : Arrival time
				- 4 : Seats
				- 5 : URL to reservation cancellation (empty = non cancellable)
				- 6 : Status canceled (1 = not canceled, 0 = canceled)

			Object : ReservationTransaction
		*/
		class ReservationItemInterfacePage
			: public util::FactorableTemplate<interfaces::InterfacePage, ReservationItemInterfacePage>
		{
		public:
			static const std::string DATA_DEPARTURE_PLACE_NAME;
			static const std::string DATA_DEPARTURE_TIME;
			static const std::string DATA_ARRIVAL_PLACE_NAME;
			static const std::string DATA_ARRIVAL_TIME;
			static const std::string DATA_SEATS;
			static const std::string DATA_URL_TO_CANCELLATION;
			static const std::string DATA_STATUS_CANCELLED;

			/** Overloaded display method for specific parameter conversion.
				This function converts the parameters into a single ParametersVector object.
				@param stream Stream to write on
				@param transaction Reservation transaction
				@param variables Execution variables
				@param request Source request
			*/
			void display(
				std::ostream& stream
				, const ReservationTransaction& transaction
				, interfaces::VariablesMap& variables
				, const server::Request* request = NULL
			) const;

			ReservationItemInterfacePage();
		};
	}
}

#endif // SYNTHESE_ReservationItemInterfacePage_H__
