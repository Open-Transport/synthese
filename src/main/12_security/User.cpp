
#include "User.h"

namespace synthese
{
	namespace security
	{


		User::User( uid id )
			: _id(id)
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
	}
}