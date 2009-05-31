
/** ReservationsListInterfacePage class header.
	@file ReservationsListInterfacePage.h
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

#ifndef SYNTHESE_ReservationsListInterfacePage_H__
#define SYNTHESE_ReservationsListInterfacePage_H__

#include "InterfacePage.h"
#include "FactorableTemplate.h"

#include <boost/shared_ptr.hpp>

namespace synthese
{
	namespace server
	{
		class Request;
	}
	namespace env
	{
		class CommercialLine;
	}

	namespace security
	{
		class User;
	}

	namespace time
	{
		class Date;
	}

	namespace resa
	{
		/** ReservationsListInterfacePage Interface Page Class.
			@ingroup m31Pages refPages

			Parameters :
				- 0 : Line ID to search
				- 1 : Customer ID to search
				- 2 : Reservation name to search
				- 3 : date
				- 4 : Display canceled reservations
				- 5 : Line name
		*/
		class ReservationsListInterfacePage
			: public util::FactorableTemplate<interfaces::InterfacePage, ReservationsListInterfacePage>
		{
		public:
			static const std::string DATA_LINE_ID;
			static const std::string DATA_CUSTOMER_ID;
			static const std::string DATA_RESERVATION_NAME;
			static const std::string DATA_DATE;
			static const std::string DATA_WITH_CANCELLED_RESERVATIONS;
			static const std::string DATA_LINE_NAME;

			/** Overloaded display method for specific parameter conversion.
				This function converts the parameters into a single ParametersVector object.
				@param stream Stream to write on
				@param line Commercial line on which the reservations belong
				@param user Customer for which reservations are done
				@param userName Name of customer for which reservations are done
				@param date Date of the search
				@param variables Execution variables
				@param request Source request
			*/
			void display(
				std::ostream& stream
				, boost::shared_ptr<const env::CommercialLine> line
				, boost::shared_ptr<const security::User> user
				, const std::string& userName
				, const time::Date& date
				, bool withCancelledReservations
				, interfaces::VariablesMap& variables
				, const server::Request* request = NULL
			) const;
			
			ReservationsListInterfacePage();
		};
	}
}

#endif // SYNTHESE_ReservationsListInterfacePage_H__
