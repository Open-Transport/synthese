
/** MessagesLibraryLog class header.
	@file MessagesLibraryLog.h

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

#ifndef SYNTHESE_MESSAGES_LIBRARY_LOG
#define SYNTHESE_MESSAGES_LIBRARY_LOG

#include "DBLogTemplate.h"

namespace synthese
{
	namespace messages
	{
		class SentScenario;
		class AlarmTemplate;
		class ScenarioTemplate;
		class TextTemplate;
		class ScenarioFolder;

		/** Journal concernant la bibliothèque de messages.

			Les colonnes additionnelles du journal de sécurité sont :
				- Action : décrit l'action effectuée (ex : création de modèle de message prioritaire)
				- UID objet concerné : lien vers le message

			@ingroup m17Logs refLogs
		*/
		class MessagesLibraryLog:
			public dblog::DBLogTemplate<MessagesLibraryLog>
		{
		public:
			std::string getName() const;
			DBLog::ColumnsVector getColumnNames() const;



			static void	addUpdateEntry(
				const AlarmTemplate* alarm
				, const std::string& text
				, const security::User* user
			);
			static void	addUpdateEntry(
				const ScenarioTemplate* scenario
				, const std::string& text
				, const security::User* user
			);
			static void addDeleteEntry(
				const ScenarioTemplate* scenario
				, const security::User* user
			);

			static void AddTemplateDeleteEntry(
				const TextTemplate& text
				, const security::User* user
			);
			static void AddTemplateUpdateEntry(
				const TextTemplate& text
				, const std::string& changes
				, const security::User* user
			);
			static void AddTemplateCreationEntry(
				const TextTemplate& text
				, const security::User* user
			);
			static void AddTemplateFolderUpdateEntry(
				const TextTemplate& text
				, const std::string& changes
				, const security::User* user
			);


			/** Adds an Alarm deletion entry.
				@param alarm Pointer to the deleted alarm : must be linked with the scenario
				@param user User who has launched the deletion
				@author Hugues Romain
				@date 2008
			*/
			static void	AddDeleteEntry(
				const AlarmTemplate* alarm
				, const security::User* user
			);

			static void	AddDeleteEntry(
				const ScenarioFolder& folder
				, const security::User* user
			);

			static void addCreateEntry(
				const ScenarioTemplate& scenario
				, const ScenarioTemplate& scenarioTemplate
				, const security::User* user
			);

			static void addCreateEntry(
				const ScenarioTemplate& scenario
				, const security::User* user
			);

			static void AddCreateEntry(
				const ScenarioFolder& folder
				, const security::User* user
			);

			static void AddTemplateInstanciationEntry(
				const SentScenario& scenario
				, const security::User* user
			);

			virtual std::string getObjectName(
				util::RegistryKeyType id,
				const server::Request& searchRequest
			) const;
		};
	}
}

#endif

