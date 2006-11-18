
#ifndef SYNTHESE_SECURITY_USER_H
#define SYNTHESE_SECURITY_USER_H

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
			Profile*		_profile;
			std::string		_login;
			std::string		_password;
			std::string		_name;
			std::string		_surname;
			

		public:
		};
	}
}