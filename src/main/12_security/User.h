
#ifndef SYNTHESE_SECURITY_USER_H
#define SYNTHESE_SECURITY_USER_H

#include <string>

#include "01_util/UId.h"

namespace synthese
{
	namespace security
	{
		class Profile;

		/** Utilisateur.
			@ingroup m05
		
			Un utilisateur correspond à un identifiant de connexion rattaché à une personne. 
			Les données suivantes lui sont reliées :
				- un nom d'utilisateur
				- un mot de passe
				- des coordonnées (mail, etc)
				- un @ref synthese::security::Profile "profil"
				- des paramètres de personnalisation (demandes favorites...)

		*/
		class User
		{
		private:
			const uid		_id;
			const Profile*	_profile;
			std::string		_login;
			std::string		_password;
			std::string		_name;
			std::string		_surname;
			
		public:
			User(uid id);
			void setProfile(const Profile* profile);
			void setLogin(const std::string& login);
			
			/** Password setter.
				@todo handle encription
			*/
			void setPassword(const std::string& password);
			void setName(const std::string& name);
			void setSurname(const std::string& surname);
		};
	}
}

#endif
