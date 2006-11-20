
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
		
			Un utilisateur correspond � un identifiant de connexion rattach� � une personne. 
			Les donn�es suivantes lui sont reli�es :
				- un nom d'utilisateur
				- un mot de passe
				- des coordonn�es (mail, etc)
				- un @ref synthese::security::Profile "profil"
				- des param�tres de personnalisation (demandes favorites...)

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
