
/** User class implementation.
	@file User.cpp

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

#include <sstream>
#include <stdlib.h>
#include <vector>
#include <time.h>

#include "Registry.h"
#include "User.h"
#include "UserException.h"


using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace time;

	namespace util
	{
		template<> const string Registry<security::User>::KEY("User");
	}


	namespace security
	{
		User::User(util::RegistryKeyType id)
			: Registrable(id)
			, _cityId(0)
			, _isConnectionAllowed(true)
			, _birthDate(TIME_UNKNOWN),
			_profile(NULL)
		{
		}

		void User::setProfile(const Profile* profile )
		{
			_profile = profile;
		}

		void User::setLogin( const std::string& login )
		{
			_login = login;
		}

		void User::setPassword( const std::string& password )
		{
			_password = password;
		}

		void User::setName( const std::string& name )
		{
			_name = name;
		}

		void User::setSurname( const std::string& surname )
		{
			_surname = surname;
		}

		const Profile* User::getProfile() const
		{
			return _profile;
		}

		const std::string& User::getLogin() const
		{
			return _login;
		}

		const std::string& User::getPassword() const
		{
			return _password;
		}

		const std::string& User::getName() const
		{
			return _name;
		}

		const std::string& User::getSurname() const
		{
			return _surname;
		}

		void User::verifyPassword( const std::string& password ) const
		{
			if (_password != password)
				throw UserException("Bad password");
		}

		void User::setAddress( const std::string& address )
		{
			_address = address;
		}

		
		void User::setPostCode( const std::string& code )
		{
			_postCode = code;
		}

		void User::setCityText( const std::string& city )
		{
			_cityText = city;
		}

		void User::setCountry( const std::string& country )
		{
			_country = country;
		}

		void User::setEMail( const std::string& email )
		{
			_email = email;
		}

		void User::setPhone( const std::string& phone )
		{
			_phone = phone;
		}

		const std::string& User::getAddress() const
		{
			return _address;
		}

		const std::string& User::getPostCode() const
		{
			return _postCode;
		}

		const std::string& User::getCityText() const
		{
			return _cityText;
		}

		const std::string& User::getCountry() const
		{
			return _country;
		}

		const std::string& User::getEMail() const
		{
			return _email;
		}

		const std::string& User::getPhone() const
		{
			return _phone;
		}

		void User::setConnectionAllowed( bool value )
		{
			_isConnectionAllowed = value;
		}

		bool User::getConnectionAllowed() const
		{
			return _isConnectionAllowed;
		}

		const time::Date& User::getBirthDate() const
		{
			return _birthDate;
		}

		void User::setBirthDate( const time::Date& date )
		{
			_birthDate = date;
		}

		std::string User::getFullName() const
		{
			stringstream s;
			if (!getSurname().empty())
				s << getSurname() << " ";
			s << getName();
			return s.str();
		}

		void User::setCityId( uid cityId )
		{
			_cityId = cityId;
		}

		uid User::getCityId() const
		{
			return _cityId;
		}



		void User::setRandomPassword()
		{
			vector<char> characters;
			for(char c='a'; c<='z'; ++c)
			{
				characters.push_back(c);
			}
			for(char c='A'; c<='Z'; ++c)
			{
				characters.push_back(c);
			}
			for(char c='0'; c<='9'; ++c)
			{
				characters.push_back(c);
			}

			string password;
			srand( (unsigned) ::time( NULL ) );

			for(int i=0; i<8; ++i)
			{
				password += characters[(rand() * 62) / (RAND_MAX + 1)];
			}

			setPassword(password);
		}
	}
}
