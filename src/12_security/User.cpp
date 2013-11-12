
/** User class implementation.
	@file User.cpp

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

#include "User.h"

#include "MD5Wrapper.h"
#include "Profile.h"
#include "Registry.h"
#include "StringUtils.hpp"
#include "UserException.h"

#include <sstream>
#include <vector>
#include <boost/random/uniform_int.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace util;

	namespace util
	{
		template<> const string Registry<security::User>::KEY("User");
	}


	namespace {
		const int SALT_LENGTH = 32;
	}


	namespace security
	{
		const string User::DATA_LOGIN = "login";
		const string User::DATA_NAME = "name";
		const string User::DATA_SURNAME = "surname";



		User::User(
			util::RegistryKeyType id
		):	Registrable(id),
			_profile(NULL),
			_cityId(0),
			_isConnectionAllowed(true),
			_birthDate(not_a_date_time),
			_language(NULL)
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

			// Hashing the passwords with MD5 is not the best security wise.
			// See https://extranet.rcsmobility.com/issues/15854 for discussions.
			MD5Wrapper md5;
			string salt(StringUtils::GenerateRandomString(SALT_LENGTH));
			string md5Hash = md5.getHashFromString(salt + password);
			string hash = salt + md5Hash;

			setPasswordHash(hash);
		}

		void User::resetTempPassword()
		{
			_password = "";
		}

		void User::setPasswordHash( const std::string& passwordHash )
		{
			_passwordHash = passwordHash;
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

		const std::string& User::getPasswordHash() const
		{
			return _passwordHash;
		}

		std::string User::getName() const
		{
			return _name;
		}

		const std::string& User::getSurname() const
		{
			return _surname;
		}

		void User::verifyPassword( const std::string& password ) const
		{
			// Backward compatibility: password used to be stored unhashed.
			if(_passwordHash == password)
				return;

			const int MD5_LENGTH = 32;
			if(_passwordHash.length() != SALT_LENGTH + MD5_LENGTH)
				throw UserException("Bad password");
			string salt = _passwordHash.substr(0, SALT_LENGTH);

			MD5Wrapper md5;
			string expectedMd5Hash = md5.getHashFromString(salt + password);
			string md5Hash(_passwordHash.substr(SALT_LENGTH, MD5_LENGTH));

			if (md5Hash != expectedMd5Hash)
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

		void User::setCreationDate( const boost::gregorian::date& creationDate )
		{
			_creationDate = creationDate;
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

		const boost::gregorian::date& User::getCreationDate() const
		{
			return _creationDate;
		}

		void User::setConnectionAllowed( bool value )
		{
			_isConnectionAllowed = value;
		}

		bool User::getConnectionAllowed() const
		{
			return _isConnectionAllowed;
		}

		const date& User::getBirthDate() const
		{
			return _birthDate;
		}

		void User::setBirthDate( const date& date )
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



		void User::setRandomPassword()
		{
			setPassword(StringUtils::GenerateRandomString(8));
		}



		void User::toParametersMap(
			util::ParametersMap& pm,
			bool withAdditionalParameters,
			boost::logic::tribool withFiles,
			std::string prefix
		) const	{
			pm.insert("id", getKey());
			pm.insert(DATA_NAME, _name);
			pm.insert(DATA_SURNAME, _surname);
			pm.insert(DATA_LOGIN, _login);
			pm.insert("phone", _phone);
			pm.insert("email", _email);
			pm.insert("address", _address);
			pm.insert("postcode", _postCode);
			pm.insert("cityText", _cityText);
			pm.insert("country", _country);
			pm.insert("is_connection_allowed", _isConnectionAllowed);
			if(_profile)
			{
				pm.insert("profile_id", _profile->getKey());
			}
			pm.insert("creation_date", _creationDate);
			if(_creatorId)
			{
				pm.insert("creator_id", _creatorId);
			}
		}
	}
}
