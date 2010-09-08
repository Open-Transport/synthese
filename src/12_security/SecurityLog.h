
/** SecurityLog class header.
	@file SecurityLog.h

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

#ifndef SYNTHESE_SECURITY_LOG
#define SYNTHESE_SECURITY_LOG

#include <string>

#include "DBLogTemplate.h"

namespace synthese
{
	namespace security
	{
		class User;
		class Profile;

		/** Journal de s�curit�.
			@ingroup m12Logs refLogs

			Enregistre tous les �v�nements li�s � la s�curit� et � l'administration des utilisateurs.

			Les entr�es du journal de s�curit� sont :
				- actions d'administration sur les utilisateurs
				- actions d'administration sur les profils
				- connexions d'utilisateurs

			Les colonnes additionnelles du journal de s�curit� sont :
				- Action : d�crit l'action effectu�e (ex : cr�ation de profil)
				- UID objet concern� : 
					- lien vers l'utilisateur trait� pour les actions d'administration sur les utilisateurs (diff�rent de l'utilisateur connect�)
					- lien vers le profil trait� pour les actions d'administration sur les profils
					- rien pour les entr�es concernant les connexions d'utilisateurs

			@ingroup m12Logs refLogs
		*/
		class SecurityLog:
			public dblog::DBLogTemplate<SecurityLog>
		{
			typedef enum {
				LOGIN_ENTRY = 10,
				USER_ADMIN_ENTRY = 20,
				PROFILE_ADMIN_ENTRY = 30
			} _EntryType;

		public:
			std::string getName() const;
			DBLog::ColumnsVector getColumnNames() const;
			DBLog::ColumnsVector parse(
				const dblog::DBLogEntry& entry,
				const server::Request& searchRequest
			) const;

			virtual std::string getObjectName(
				util::RegistryKeyType id,
				const server::Request& request
			) const;

			static void addUserLogin(const User* user);
			static void addUserAdmin(
				const User* user
				, const User* subject
				, const std::string& text
			);
			static void addProfileAdmin(
				const User* user
				, const Profile* subject
				, const std::string& text
			);
		};
	}
}

#endif

