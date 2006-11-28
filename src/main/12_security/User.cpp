
#include "12_security/User.h"
#include "12_security/UserException.h"

namespace synthese
{
	using namespace util;

	namespace security
	{
		User::User(uid id)
			: Registrable<uid, User>(id)
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
	}
}