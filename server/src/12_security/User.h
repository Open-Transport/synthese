
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

#include "Object.hpp"

#include "DataSourceLinksField.hpp"
#include "DateField.hpp"
#include "ImportableTemplate.hpp"

#include "Registry.h"
#include "StringField.hpp"

namespace synthese
{
	class Language;

	namespace security
	{
		class Profile;

		FIELD_STRING(SurName)
		FIELD_STRING(Login)
		FIELD_STRING(Password)
		FIELD_POINTER(UserProfile, Profile)
		FIELD_STRING(Address)
		FIELD_STRING(PostCode)
		FIELD_STRING(CityText)
		FIELD_ID(CityId)
		FIELD_STRING(Country)
		FIELD_STRING(Email)
		FIELD_STRING(Phone)
		FIELD_DATE(CreationDate)
		FIELD_ID(CreatorId)
		FIELD_BOOL(Auth)
		FIELD_DATE(BirthDate)
		FIELD_STRING(Language)
		FIELD_DATASOURCE_LINKS(DataSourceLinksWithoutUnderscore)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(SurName),
			FIELD(Login),
			FIELD(Password),
			FIELD(UserProfile),
			FIELD(Address),
			FIELD(PostCode),
			FIELD(CityText),
			FIELD(CityId),
			FIELD(Country),
			FIELD(Email),
			FIELD(Phone),
			FIELD(CreationDate),
			FIELD(CreatorId),
			FIELD(Auth),
			FIELD(BirthDate),
			FIELD(security::Language),
			FIELD(DataSourceLinksWithoutUnderscore)
		> UserSchema;

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
			public impex::ImportableTemplate<User>,
			public virtual Object<User,UserSchema>
		{
		public:
			static const std::string DATA_NAME;
			static const std::string DATA_SURNAME;
			static const std::string DATA_LOGIN;

		private:
			const synthese::Language*	_language;
			std::string		_password;

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
				void setCityId(util::RegistryKeyType cityId) { set<CityId>(cityId); }
				void setCountry(const std::string& country);
				void setEMail(const std::string& email);
				void setPhone(const std::string& phone);
				void setCreationDate(const boost::gregorian::date& date);
				void setCreatorId(util::RegistryKeyType creatorId) { set<CreatorId>(creatorId); }
				void setConnectionAllowed(bool value);
				void setBirthDate(const boost::gregorian::date& date);
				void setRandomPassword();
				void setLanguage(const synthese::Language* value);
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
				util::RegistryKeyType	getCityId()			const { return get<CityId>(); }
				const std::string&	getCountry()			const;
				const std::string&	getEMail()				const;
				const std::string&	getPhone()				const;
				const boost::gregorian::date&	getCreationDate()		const;
				util::RegistryKeyType	getCreatorId()		const { return get<CreatorId>(); }
				bool				getConnectionAllowed()	const;
				const boost::gregorian::date&	getBirthDate()			const;
				const synthese::Language*		getLanguage() const { return _language; }
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

			//! @name Modifiers
			//@{
				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
			//@}

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
	}
}

#endif
