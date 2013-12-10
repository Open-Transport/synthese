
/** LoginToken class header.
	@file @15_server/LoginToken.h

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

#ifndef SYNTHESE_LoginToken_h__
#define SYNTHESE_LoginToken_h__

#include "Registrable.h"
#include "Registry.h"

#include <string>

namespace synthese
{
	namespace util
	{
		class ParametersMap;
	}

	namespace server
	{

		/** LoginToken
			@ingroup m15

			Un token d'authentification peut être généré afin de permettre à un utilisateur d'accéder à une
			annulation de réservation en se loggant automatiquement.
			Les données suivantes lui sont reliées :
				- un nom d'utilisateur
				- un hash de mot de passe
				- un id de transaction
		*/
		class LoginToken
		{
		public:
			static const std::string DATA_LOGIN;
			static const std::string DATA_PASSWORD;
			static const std::string DATA_RESERVATION_TRANSACTION;

		private:
			std::string				_login;
			std::string				_password;
			util::RegistryKeyType	_reservationTransaction;

		protected:
			void toParametersMap(util::ParametersMap& pm) const;

		public:
			LoginToken(
				std::string login = "",
				std::string passwordHash = "",
				util::RegistryKeyType reservationTransaction = 0);

			//! \name Setters
			//@{
				void setLogin(const std::string& login);
				void setPassword(const std::string& password);
				void setReservationTransaction(const util::RegistryKeyType& reservationTransaction);
			//@}

			//! \name Getters
			//@{
				const std::string& getLogin() const { return _login; }
				const std::string& getPassword() const { return _password; }
				const util::RegistryKeyType& getReservationTransaction() const { return _reservationTransaction; }
			//@}

				bool Decode(std::string encoded);
				std::string toString() const;
		};
	}
}

#endif
