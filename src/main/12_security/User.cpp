
#include "12_security/User.h"
#include "12_security/UserException.h"

namespace synthese
{
	using namespace util;

	namespace security
	{
		User::User(uid id)
			: Registrable<uid, User>(id)
			, _cityId(0)
			, _isConnectionAllowed(true)
			, _profile(NULL)
		{

		}

		void User::setProfile( const Profile* profile )
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
	}
}
