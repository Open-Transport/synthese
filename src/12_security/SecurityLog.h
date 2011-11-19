
/** SecurityLog class header.
	@file SecurityLog.h

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

		/** Journal de sécurité.
			@ingroup m12Logs refLogs

			Enregistre tous les événements liés à la sécurité et à l'administration des utilisateurs.

			Les entrées du journal de sécurité sont :
				- actions d'administration sur les utilisateurs
				- actions d'administration sur les profils
				- connexions d'utilisateurs

			Les colonnes additionnelles du journal de sécurité sont :
				- Action : décrit l'action effectuée (ex : création de profil)
				- UID objet concerné :
					- lien vers l'utilisateur traité pour les actions d'administration sur les utilisateurs (différent de l'utilisateur connecté)
					- lien vers le profil traité pour les actions d'administration sur les profils
					- rien pour les entrées concernant les connexions d'utilisateurs

			@ingroup m12Logs refLogs
		*/
		class SecurityLog:
			public dblog::DBLogTemplate<SecurityLog>
		{
			typedef enum {
				LOGIN_ENTRY = 10,
				USER_ADMIN_ENTRY = 20,
				PROFILE_ADMIN_ENTRY = 30,
				QUIT_ENTRY = 40
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

			static void AddQuitEntry(
				const User& user
			);
		};
	}
}

#endif

