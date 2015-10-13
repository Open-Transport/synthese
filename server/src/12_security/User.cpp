
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

#include "Language.hpp"
#include "MD5Wrapper.h"
#include "Profile.h"
#include "Registry.h"
#include "SecurityRight.h"
#include "Session.h"
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
	using namespace security;
	using namespace util;

	CLASS_DEFINITION(User, "t026_users", 26)
	FIELD_DEFINITION_OF_OBJECT(User, "user_id", "user_ids")

	FIELD_DEFINITION_OF_TYPE(SurName, "surname", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(Login, "login", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(Password, "password", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(UserProfile, "profile_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(Address, "address", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(PostCode, "post_code", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(CityText, "city_text", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(CityId, "city_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(Country, "country", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(Email, "email", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(Phone, "phone", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(CreationDate, "creation_date", SQL_DATETIME)
	FIELD_DEFINITION_OF_TYPE(CreatorId, "creator_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(Auth, "auth", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(BirthDate, "birth_date", SQL_DATETIME)
	FIELD_DEFINITION_OF_TYPE(security::Language, "language", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(DataSourceLinksWithoutUnderscore, "data_source_links", SQL_TEXT)


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
			Object<User, UserSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_DEFAULT_CONSTRUCTOR(SurName),
					FIELD_DEFAULT_CONSTRUCTOR(Login),
					FIELD_DEFAULT_CONSTRUCTOR(Password),
					FIELD_DEFAULT_CONSTRUCTOR(UserProfile),
					FIELD_DEFAULT_CONSTRUCTOR(Address),
					FIELD_DEFAULT_CONSTRUCTOR(PostCode),
					FIELD_DEFAULT_CONSTRUCTOR(CityText),
					FIELD_VALUE_CONSTRUCTOR(CityId, 0),
					FIELD_DEFAULT_CONSTRUCTOR(Country),
					FIELD_DEFAULT_CONSTRUCTOR(Email),
					FIELD_DEFAULT_CONSTRUCTOR(Phone),
					FIELD_DEFAULT_CONSTRUCTOR(CreationDate),
					FIELD_VALUE_CONSTRUCTOR(CreatorId, 0),
					FIELD_VALUE_CONSTRUCTOR(Auth, true),
					FIELD_VALUE_CONSTRUCTOR(BirthDate, not_a_date_time),
					FIELD_DEFAULT_CONSTRUCTOR(security::Language),
					FIELD_DEFAULT_CONSTRUCTOR(DataSourceLinksWithoutUnderscore)
			)	)
		{
		}



		void User::setProfile(const Profile* profile )
		{
			set<UserProfile>(profile
				? boost::optional<Profile&>(*const_cast<Profile*>(profile))
				: boost::none);
		}

		void User::setLogin( const std::string& login )
		{
			set<Login>(login);
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
			set<Password>(passwordHash);
		}

		void User::setName( const std::string& name )
		{
			set<Name>(name);
		}

		void User::setSurname( const std::string& surname )
		{
			set<SurName>(surname);
		}

		const Profile* User::getProfile() const
		{
			if (get<UserProfile>())
			{
				return get<UserProfile>().get_ptr();
			}
			return NULL;
		}

		const std::string& User::getLogin() const
		{
			return get<Login>();
		}

		const std::string& User::getPassword() const
		{
			return _password;
		}

		const std::string& User::getPasswordHash() const
		{
			return get<Password>();
		}

		std::string User::getName() const
		{
			return get<Name>();
		}

		const std::string& User::getSurname() const
		{
			return get<SurName>();
		}

		void User::verifyPassword( const std::string& password ) const
		{
			// Backward compatibility: password used to be stored unhashed.
			if(get<Password>() == password)
				return;

			const int MD5_LENGTH = 32;
			string passwordHash = get<Password>();
			if(passwordHash.length() != SALT_LENGTH + MD5_LENGTH)
				throw UserException("Bad password");
			string salt = passwordHash.substr(0, SALT_LENGTH);

			MD5Wrapper md5;
			string expectedMd5Hash = md5.getHashFromString(salt + password);
			string md5Hash(passwordHash.substr(SALT_LENGTH, MD5_LENGTH));

			if (md5Hash != expectedMd5Hash)
				throw UserException("Bad password");
		}

		void User::setAddress( const std::string& address )
		{
			set<Address>(address);
		}


		void User::setPostCode( const std::string& code )
		{
			set<PostCode>(code);
		}

		void User::setCityText( const std::string& city )
		{
			set<CityText>(city);
		}

		void User::setCountry( const std::string& country )
		{
			set<Country>(country);
		}

		void User::setEMail( const std::string& email )
		{
			set<Email>(email);
		}

		void User::setPhone( const std::string& phone )
		{
			set<Phone>(phone);
		}

		void User::setCreationDate( const boost::gregorian::date& creationDate )
		{
			set<CreationDate>(creationDate);
		}

		const std::string& User::getAddress() const
		{
			return get<Address>();
		}

		const std::string& User::getPostCode() const
		{
			return get<PostCode>();
		}

		const std::string& User::getCityText() const
		{
			return get<CityText>();
		}

		const std::string& User::getCountry() const
		{
			return get<Country>();
		}

		const std::string& User::getEMail() const
		{
			return get<Email>();
		}

		const std::string& User::getPhone() const
		{
			return get<Phone>();
		}

		const boost::gregorian::date& User::getCreationDate() const
		{
			return get<CreationDate>();
		}

		void User::setConnectionAllowed( bool value )
		{
			set<Auth>(value);
		}

		bool User::getConnectionAllowed() const
		{
			return get<Auth>();
		}

		const date& User::getBirthDate() const
		{
			return get<BirthDate>();
		}

		void User::setBirthDate( const date& date )
		{
			set<BirthDate>(date);
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
			pm.insert(DATA_NAME, get<Name>());
			pm.insert(DATA_SURNAME, get<SurName>());
			pm.insert(DATA_LOGIN, get<Login>());
			pm.insert("phone", get<Phone>());
			pm.insert("email", get<Email>());
			pm.insert("address", get<Address>());
			pm.insert("postcode", get<PostCode>());
			pm.insert("cityText", get<CityText>());
			pm.insert("country", get<Country>());
			pm.insert("is_connection_allowed", get<Auth>());
			if(get<UserProfile>())
			{
				pm.insert("profile_id", get<UserProfile>()->getKey());
			}
			pm.insert("creation_date", get<CreationDate>());
			if(get<CreatorId>())
			{
				pm.insert("creator_id", get<CreatorId>());
			}
		}

		void User::setLanguage(const synthese::Language* value)
		{
			_language = value;
		}

		void User::link(util::Env& env, bool withAlgorithmOptimizations)
		{
			// Language
			string langStr(get<security::Language>());
			if(!langStr.empty()) try
			{
				setLanguage(&synthese::Language::GetLanguageFromIso639_2Code(langStr));
			}
			catch(synthese::Language::LanguageNotFoundException& e)
			{
				Log::GetInstance().warn("Language error in user "+ lexical_cast<string>(getKey()), e);
			}
		}

		bool User::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<SecurityRight>(security::WRITE);
		}

		bool User::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<SecurityRight>(security::WRITE);
		}

		bool User::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<SecurityRight>(security::DELETE_RIGHT);
		}
	}
}
