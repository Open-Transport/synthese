
/** ReservationUserUpdateAction class header.
	@file ReservationUserUpdateAction.h
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

#ifndef SYNTHESE_ReservationUserUpdateAction_H__
#define SYNTHESE_ReservationUserUpdateAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

#include <boost/optional.hpp>

namespace synthese
{
	namespace security
	{
		class User;
	}

	namespace resa
	{
		/** ReservationUserUpdateAction action class.
			@ingroup m51Actions refActions

			@bug If two new logins are requested for the same couple surname / name at the
			same time, it is possible that the two users become the same login
		*/
		class ReservationUserUpdateAction:
			public util::FactorableTemplate<server::Action, ReservationUserUpdateAction>
		{
		public:
			static const std::string PARAMETER_USER_ID;
			static const std::string PARAMETER_LOGIN;
			static const std::string PARAMETER_SURNAME;
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_ADDRESS;
			static const std::string PARAMETER_POSTAL_CODE;
			static const std::string PARAMETER_CITY;
			static const std::string PARAMETER_PHONE;
			static const std::string PARAMETER_EMAIL;
			static const std::string PARAMETER_AUTHORIZED_LOGIN;
			static const std::string PARAMETER_AUTORESA_ACTIVATED;

		private:
			boost::shared_ptr<security::User>	_user;
			std::string				_login;
			std::string				_surname;
			std::string				_name;
			std::string				_address;
			std::string				_postalCode;
			std::string				_city;
			std::string				_phone;
			std::string				_email;
			boost::optional<bool>	_authorizedLogin;
			boost::optional<bool>	_autoResaActivated;

		protected:
			/** Conversion from attributes to generic parameter maps.
				@return Generated parameters map
			*/
			util::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				Removes the used parameters from the map.
				@param map Parameters map to interpret
				@exception ActionException Occurs when some parameters are missing or incorrect.
			*/
			void _setFromParametersMap(const util::ParametersMap& map);

		public:
			/** Action to run, defined by each subclass.
			*/
			void run(server::Request& request);

			virtual bool isAuthorized(const server::Session* session) const;

			void setUser(boost::shared_ptr<const security::User> value);
		};
	}
}

#endif // SYNTHESE_ReservationUserUpdateAction_H__
