
/** User class header.
	@file User.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#include <boost/shared_ptr.hpp>

#include "01_util/Registrable.h"
#include "01_util/UId.h"

#include "02_db/SQLiteTableSyncTemplate.h"

#include "04_time/Date.h"

namespace synthese
{
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
		class User : public util::Registrable<uid, User>
		{
		private:
			boost::shared_ptr<const Profile>	_profile;
			std::string							_login;
			std::string							_password;
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
			time::Date		_birthDate;

			friend class db::SQLiteTableSyncTemplate<User>;
			
		public:
			User(uid id=0);

			//! \name Setters
			//@{
				void setProfile(boost::shared_ptr<const Profile> profile);
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
				void setBirthDate(const time::Date& date);
			//@}

			//! \name Getters
			//@{
				boost::shared_ptr<const Profile>	getProfile() const;
				const std::string&					getLogin() const;

				/** Password getter.
					@todo handle encryption (create a getEncryptedPassword instead)
				*/
				const std::string&	getPassword()			const;
				const std::string&	getName()				const;
				const std::string&	getSurname()			const;
				const std::string&	getAddress()			const;
				const std::string&	getPostCode()			const;
				const std::string&	getCityText()			const;
				const std::string&	getCountry()			const;
				const std::string&	getEMail()				const;
				const std::string&	getPhone()				const;
				bool				getConnectionAllowed()	const;
				const time::Date&	getBirthDate()			const;
			//@}

			//! \name Services
			//@{
				void verifyPassword(const std::string& password) const;
			//@}
		};
	}
}

#endif


