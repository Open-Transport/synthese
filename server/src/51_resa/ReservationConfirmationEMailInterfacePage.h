
/** ReservationConfirmationEMailInterfacePage class header.
	@file ReservationConfirmationEMailInterfacePage.h
	@author Hugues
	@date 2009

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

#ifndef SYNTHESE_ReservationConfirmationEMailInterfacePage_H__
#define SYNTHESE_ReservationConfirmationEMailInterfacePage_H__

#include "InterfacePage.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace server
	{
		class Request;
	}

	namespace resa
	{
		class ReservationTransaction;

		/** ReservationConfirmationEMailInterfacePage Interface Page Class.
			@ingroup m51Pages refPages
			@author Hugues
			@date 2009

			@code reservation_confirmation_email_content @endcode

			Parameters :
				- 0 : journey detail
				- 1 : ID of the reservation transaction
				- 2 : ID of the customer
				- 3 : date of cancellation dead line
				- 4 : time of cancellation dead line
				- 5 : departure stop
				- 6 : arrival stop
				- 7 : travel date
				- 8 : customer name
				- 9 : customer phone

			Object : ReservationTransaction
		*/
		class ReservationConfirmationEMailInterfacePage
			: public util::FactorableTemplate<interfaces::InterfacePage, ReservationConfirmationEMailInterfacePage>
		{
		public:
			ReservationConfirmationEMailInterfacePage();



			/** Overloaded display method for specific parameter conversion.
				This function converts the parameters into a single ParametersVector object.
				@param stream Stream to write on
				@param resa Reservation transaction to confirm
				@param variables Execution variables
				@param request Source request
			*/
			void display(
				std::ostream& stream,
				const ReservationTransaction& resa,
				interfaces::VariablesMap& variables,
				const server::Request* request = NULL
			) const;
		};
	}
}

#endif // SYNTHESE_ReservationConfirmationEMailInterfacePage_H__
