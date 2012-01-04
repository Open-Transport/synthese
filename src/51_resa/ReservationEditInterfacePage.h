
/** ReservationEditInterfacePage class header.
	@file ReservationEditInterfacePage.h
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

#ifndef SYNTHESE_ReservationEditInterfacePage_H__
#define SYNTHESE_ReservationEditInterfacePage_H__

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
	}

	namespace resa
	{
		/** ReservationEditInterfacePage Interface Page Class.
			@ingroup m51Pages refPages
			@author Hugues
			@date 2009

			@code reservation_edit @endcode

			Parameters :
				- 0 : ID of the reservation transaction
				- 1 : journey detail
				- 2 : ID of the customer
				- 3 : date of cancellation dead line
				- 4 : time of cancellation dead line
				- 5 : departure stop
				- 6 : arrival stop
				- 7 : travel date
				- 8 : customer name
				- 9 : customer phone
				- 10 : status text
				- 11 : can be cancelled
				- 12 : session ID
				- 13 : cancellation date
				- 14 : cancellation time

			Parameters 1 to 13 are sent only if a valid user is logged in.

			Object : ReservationTransaction
		*/
		class ReservationEditInterfacePage
			: public util::FactorableTemplate<interfaces::InterfacePage, ReservationEditInterfacePage>
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
				const resa::ReservationTransaction& object,
				interfaces::VariablesMap& variables,
				const server::Request* request = NULL
			) const;

			ReservationEditInterfacePage();
		};
	}
}

#endif // SYNTHESE_ReservationEditInterfacePage_H__
