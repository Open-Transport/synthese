
/** VinciUpdateCustomerAction class header.
	@file VinciUpdateCustomerAction.h

	This file belongs to the VINCI BIKE RENTAL SYNTHESE module
	Copyright (C) 2006 Vinci Park 
	Contact : Raphaël Murat - Vinci Park <rmurat@vincipark.com>

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

#ifndef SYNTHESE_UpdateCustomerAction_H__
#define SYNTHESE_UpdateCustomerAction_H__

#include "04_time/Date.h"

#include "30_server/Action.h"

namespace synthese
{
	namespace security
	{
		class User;
	}

	namespace vinci
	{
		/** Update of the contract with the customer.
			@ingroup m71
		*/
		class VinciUpdateCustomerAction : public server::Action
		{
		public:
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_SURNAME;
			static const std::string PARAMETER_ADDRESS;
			static const std::string PARAMETER_POST_CODE;
			static const std::string PARAMETER_CITY;
			static const std::string PARAMETER_COUNTRY;
			static const std::string PARAMETER_EMAIL;
			static const std::string PARAMETER_PHONE;
			static const std::string PARAMETER_BIRTH_DATE;
			static const std::string PARAMETER_PASSPORT;

		private:
			VinciContract*	_contract;
			security::User*	_user;
			std::string		_name;
			std::string		_surname;
			std::string		_address;
			std::string		_postCode;
			std::string		_city;
			std::string		_country;
			std::string		_email;
			std::string		_phone;
			time::Date		_birthDate;
			std::string		_passport;

		protected:
			/** Conversion from attributes to generic parameter maps.
			*/
			server::Request::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
			Removes the used parameters from the map.
			*/
			void setFromParametersMap(server::Request::ParametersMap& map);

		public:

			/** Action to run, defined by each subclass.
			*/
			void run();
		};
	}
}

#endif // SYNTHESE_UpdateCustomerAction_H__

