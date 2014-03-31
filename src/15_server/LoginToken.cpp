
/** LoginToken class implementation.
	@file 51_resa/LoginToken.cpp

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

#include "LoginToken.hpp"
#include "User.h"
#include "ServerModule.h"

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/remove_whitespace.hpp>

using namespace std;
using namespace boost;
using namespace boost::archive::iterators;

namespace synthese
{
	using namespace security;
	using namespace util;


	namespace server
	{
		const string LoginToken::DATA_LOGIN = "login";
		const string LoginToken::DATA_PASSWORD = "password";
		const string LoginToken::DATA_RESERVATION_TRANSACTION = "reservation_transaction";

		typedef transform_width< binary_from_base64<remove_whitespace<string::const_iterator> >, 8, 6 > it_binary_t;
		typedef insert_linebreaks<base64_from_binary<transform_width<string::const_iterator,6,8> >, 72 > it_base64_t;

		
		LoginToken::LoginToken(
			std::string login,
			std::string hashPassword,
			util::RegistryKeyType reservationTransaction
		):	_login(login),
			_password(hashPassword),
			_reservationTransaction(reservationTransaction)
		{
		}


		
		bool LoginToken::Decode(std::string encoded)
		{
			// Decode
			unsigned int paddChars = count(encoded.begin(), encoded.end(), '=');
			std::replace(encoded.begin(),encoded.end(),'=','A');
			std::string decoded(it_binary_t(encoded.begin()), it_binary_t(encoded.end()));
			decoded.erase(decoded.end()-paddChars,decoded.end());

			std::vector<std::string> parameters;
			boost::split(parameters, decoded, boost::is_any_of(","));

			if (parameters.size() == 3)
			{
				LoginToken::setLogin(parameters[0]);
				LoginToken::setReservationTransaction(lexical_cast<RegistryKeyType>(parameters[1]));
				LoginToken::setPassword(parameters[2]);

				return true;
			}
			else
				return false;
		}

		

		std::string LoginToken::toString() const
		{
			std::string token = _login + "," + lexical_cast<string>(_reservationTransaction) + "," + _password;

			// Encode
			unsigned int writePaddChars = (3-token.length()%3)%3;
			string encoded(it_base64_t(token.begin()),it_base64_t(token.end()));
			encoded.append(writePaddChars,'=');

			return encoded;
		}



		void LoginToken::toParametersMap(util::ParametersMap& pm) const	{
			pm.insert(DATA_LOGIN, _login);
			pm.insert(DATA_PASSWORD, _password);
			pm.insert(DATA_RESERVATION_TRANSACTION, _reservationTransaction);
		}

		
		
		void LoginToken::setLogin(const std::string& login)
		{
			_login = login;
		}



		void LoginToken::setPassword(const std::string& password)
		{
			_password = password;
		}



		void LoginToken::setReservationTransaction(const util::RegistryKeyType& reservationTransaction)
		{
			_reservationTransaction = reservationTransaction;
		}
	}
}
