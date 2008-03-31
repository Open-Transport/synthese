
/** BookReservationAction class header.
	@file BookReservationAction.h
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

#ifndef SYNTHESE_BookReservationAction_H__
#define SYNTHESE_BookReservationAction_H__

#include "30_server/Action.h"

#include "15_env/Journey.h"

#include "01_util/FactorableTemplate.h"

#include <boost/shared_ptr.hpp>
#include <string>

namespace synthese
{
	namespace security
	{
		class User;
	}

	namespace resa
	{
		/** BookReservationAction action class.
			@ingroup m31Actions refActions

			@todo Move this action into route planner and rename it BookJourneyFromRoutePlannerAction

			The reservation is made upon a journey. Each journey leg allowing reservation
			is booked simultaneously.

			Two types of user can do a reservation :
			 - an operator (ResaRight public WRITE right) : can type a customer name or a customer id, must not type his password
			 - a customer (ResaRight private WRITE right) : can only modify the contact phone number and must retype his password even if it is already logged in

			The datetime must be precisely identical with the start time of the founded journey. If not, the reservation is cancelled and the route planning must be launched again.
			This case can occur if a hot update has been done while the customer asked for his reservation.
		*/
		class BookReservationAction
			: public util::FactorableTemplate<server::Action, BookReservationAction>
		{
		public:
			static const std::string PARAMETER_SITE;
			
			// Accessibility
			static const std::string PARAMETER_ACCESSIBILITY;
			static const std::string PARAMETER_DISABLED_CUSTOMER;
			static const std::string PARAMETER_DRT_ONLY;

			// Journey information
			static const std::string PARAMETER_ORIGIN_CITY;
			static const std::string PARAMETER_ORIGIN_PLACE;
			static const std::string PARAMETER_DESTINATION_CITY;
			static const std::string PARAMETER_DESTINATION_PLACE;
			static const std::string PARAMETER_DATE_TIME;

			// Customer information
			static const std::string PARAMETER_CREATE_CUSTOMER;
			static const std::string PARAMETER_CUSTOMER_ID;
			static const std::string PARAMETER_CUSTOMER_NAME;
			static const std::string PARAMETER_CUSTOMER_SURNAME;
			static const std::string PARAMETER_CUSTOMER_PHONE;
			static const std::string PARAMETER_CUSTOMER_EMAIL;

			// Requester information
			static const std::string PARAMETER_PASSWORD;

			// Reservation information
			static const std::string PARAMETER_SEATS_NUMBER;

		private:
			env::Journey						_journey;
			boost::shared_ptr<security::User>	_customer;
			bool								_createCustomer;
			bool								_disabledCustomer;
			bool								_drtOnly;
			int									_seatsNumber;

		protected:
			/** Conversion from attributes to generic parameter maps.
				@return Generated parameters map
			*/
			server::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				Removes the used parameters from the map.
				@param map Parameters map to interpret
				@exception ActionException Occurs when some parameters are missing or incorrect.
			*/
			void _setFromParametersMap(const server::ParametersMap& map);

		public:
			/** Action to run, defined by each subclass.
			*/
			void run();

			BookReservationAction();
			
			void setJourney(const env::Journey& journey);
		};
	}
}

#endif // SYNTHESE_BookReservationAction_H__
