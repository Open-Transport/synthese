
/** User class header.
	@file User.h

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

#ifndef SYNTHESE_SECURITY_USER_H
#define SYNTHESE_SECURITY_USER_H

#include <string>
#include <boost/date_time/gregorian/gregorian.hpp>

#include "ImportableTemplate.hpp"
#include "Registrable.h"

#include "Registry.h"

namespace synthese
{
	class Language;

	namespace security
	{
		class Profile;

		/** Utilisateur.
			@ingroup m12

			Un utilisateur correspond à un identifiant de connexion rattaché à une personne.
			Les données suivantes lui sont reliées :
				- un nom d'utilisateur
				- un mot de passe
				- des coordonnées (mail, etc)
				- un @ref synthese::security::Profile "profil"
				- des paramètres de personnalisation (demandes favorites...)

		*/
		class User:
			public virtual util::Registrable,
			public impex::ImportableTemplate<User>
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<User>	Registry;

			static const std::string DATA_NAME;
			static const std::string DATA_SURNAME;
			static const std::string DATA_LOGIN;

		private:
			const Profile*	_profile;
			std::string		_login;
			std::string		_password;
			std::string		_passwordHash;
			std::string		_name;
			std::string		_surname;
			std::string		_address;
			std::string		_postCode;
			std::string		_cityText;
			util::RegistryKeyType		_cityId;
			std::string		_country;
			std::string		_email;
			std::string		_phone;
			boost::gregorian::date		_creationDate;
			util::RegistryKeyType		_creatorId;
			bool			_isConnectionAllowed;
			boost::gregorian::date	_birthDate;
			const Language*	_language;

		public:
			User(util::RegistryKeyType id = 0);

			//! \name Setters
			//@{
				void setProfile(const Profile* profile);
				void setLogin(const std::string& login);

				void setPassword(const std::string& password);
				void setPasswordHash(const std::string& passwordHash);
				void setName(const std::string& name);
				void setSurname(const std::string& surname);
				void setAddress(const std::string& address);
				void setPostCode(const std::string& code);
				void setCityText(const std::string& city);
				void setCityId(util::RegistryKeyType cityId) { _cityId = cityId; }
				void setCountry(const std::string& country);
				void setEMail(const std::string& email);
				void setPhone(const std::string& phone);
				void setCreationDate(const boost::gregorian::date& date);
				void setCreatorId(util::RegistryKeyType creatorId) { _creatorId = creatorId; }
				void setConnectionAllowed(bool value);
				void setBirthDate(const boost::gregorian::date& date);
				void setRandomPassword();
				void setLanguage(const Language* value){ _language = value; }
			//@}

			//! \name Getters
			//@{
				const Profile*		getProfile() const;
				const std::string&	getLogin() const;

				/** Password getter.
					The password is only available if setPassword() was called.
					When loaded from the database, only the password hash is available.
				*/
				const std::string&	getPassword()			const;
				const std::string&	getPasswordHash()		const;
				virtual std::string	getName()				const;
				const std::string&	getSurname()			const;
				const std::string&	getAddress()			const;
				const std::string&	getPostCode()			const;
				const std::string&	getCityText()			const;
				util::RegistryKeyType	getCityId()				const { return _cityId; }
				const std::string&	getCountry()			const;
				const std::string&	getEMail()				const;
				const std::string&	getPhone()				const;
				const boost::gregorian::date&	getCreationDate()		const;
				util::RegistryKeyType	getCreatorId()		const { return _creatorId; }
				bool				getConnectionAllowed()	const;
				const boost::gregorian::date&	getBirthDate()			const;
				const Language*		getLanguage() const { return _language; }
			//@}

			//! \name Services
			//@{
				void verifyPassword(const std::string& password) const;
				std::string	getFullName()	const;
				virtual void toParametersMap(
					util::ParametersMap& map,
					bool withAdditionalParameters,
					boost::logic::tribool withFiles = boost::logic::indeterminate,
					std::string prefix = std::string()
				) const;

				/* In order to prevent clear password available in memory,
				 * this function reset private _password value accessible after
				 * setPassword(). */
				void resetTempPassword();
			//@}
		};
	}
}

#endif
