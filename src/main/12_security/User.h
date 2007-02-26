
#ifndef SYNTHESE_SECURITY_USER_H
#define SYNTHESE_SECURITY_USER_H

#include <string>

#include "01_util/Registrable.h"
#include "01_util/UId.h"

#include "02_db/SQLiteTableSyncTemplate.h"

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
		class User : public util::Registrable<uid, User>
		{
		private:
			const Profile*	_profile;
			std::string		_login;
			std::string		_password;
			std::string		_name;
			std::string		_surname;
			std::string		_address;
			std::string		_postCode;
			std::string		_cityText;
			uid				_cityId;
			std::string		_country;
			std::string		_email;
			std::string		_phone;
			bool			_isConnectionAllowed;

			friend class db::SQLiteTableSyncTemplate<User>;
			
		public:
			User(uid id=0);

			//! \name Setters
			//@{
				void setProfile(const Profile* profile);
				void setLogin(const std::string& login);
				
				/** Password setter.
					@todo handle encryption
				*/
				void setPassword(const std::string& password);
				void setName(const std::string& name);
				void setSurname(const std::string& surname);
				void setAddress(const std::string& address);
				void setPostCode(const std::string& code);
				void setCityText(const std::string& city);
				void setCountry(const std::string& country);
				void setEMail(const std::string& email);
				void setPhone(const std::string& phone);
				void setConnectionAllowed(bool value);
			//@}

			//! \name Getters
			//@{
				const Profile* getProfile() const;
				const std::string& getLogin() const;

				/** Password getter.
					@todo handle encryption (create a getEncryptedPassword instead)
				*/
				const std::string&	getPassword() const;
				const std::string&	getName() const;
				const std::string&	getSurname() const;
				const std::string&	getAddress() const;
				const std::string&	getPostCode() const;
				const std::string&	getCityText() const;
				const std::string&	getCountry() const;
				const std::string&	getEMail() const;
				const std::string&	getPhone() const;
				bool				getConnectionAllowed() const;
			//@}

			//! \name Services
			//@{
				void verifyPassword(const std::string& password) const;
			//@}
		};
	}
}

#endif


