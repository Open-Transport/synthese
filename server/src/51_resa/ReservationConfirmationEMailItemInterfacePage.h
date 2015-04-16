
/** ReservationConfirmationEMailItemInterfacePage class header.
	@file ReservationConfirmationEMailItemInterfacePage.h
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

#ifndef SYNTHESE_ReservationConfirmationEMailItemInterfacePage_H__
#define SYNTHESE_ReservationConfirmationEMailItemInterfacePage_H__

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
		class Reservation;

		/** ReservationConfirmationEMailItemInterfacePage Interface Page Class.
			@ingroup m51Pages refPages
			@author Hugues
			@date 2009

			@code reservation_confirmation_email_item @endcode

			Parameters :
				- 0 : departure time
				- 1 : departure place
				- 2 : arrival time
				- 3 : arrival place
				- 4 : line

			Object : Reservation
		*/
		class ReservationConfirmationEMailItemInterfacePage
			: public util::FactorableTemplate<interfaces::InterfacePage, ReservationConfirmationEMailItemInterfacePage>
		{
		public:
			/** Overloaded display method for specific parameter conversion.
				This function converts the parameters into a single ParametersVector object.
				@param stream Stream to write on
				@param ...
				@param variables Execution variables
				@param request Source request
			*/
			void display(
				std::ostream& stream,
				const Reservation& resa,
				interfaces::VariablesMap& variables,
				const server::Request* request = NULL
			) const;

			ReservationConfirmationEMailItemInterfacePage();
		};
	}
}

#endif // SYNTHESE_ReservationConfirmationEMailItemInterfacePage_H__
