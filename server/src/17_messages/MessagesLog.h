
/** MessagesLog class header.
	@file MessagesLog.h

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

#ifndef SYNTHESE_MESSAGES_LOG
#define SYNTHESE_MESSAGES_LOG

#include "DBLogTemplate.h"

namespace synthese
{
	namespace messages
	{
		class Alarm;
		class Alarm;
		class ScenarioTemplate;
		class SentScenario;
		class Scenario;

		/** Journal des envois de messages.

			Les entrées du journal de messages sont :
				- envois de messages
				- envois selon scénarios de diffusion

			Les colonnes additionnelles du journal de sécurité sont :
				- Action : décrit l'action effectuée (ex : création de profil)
				- UID objet concerné :
					- lien vers le message si envoi de message unique
					- lien vers le scénario si envoi selon un scénario de diffusion

			@ingroup m17Logs refLogs
		*/
		class MessagesLog:
			public dblog::DBLogTemplate<MessagesLog>
		{
		public:
			std::string getName() const;
			DBLog::ColumnsVector getColumnNames() const;



			static void AddNewScenarioMessageEntry(
				const Alarm& alarm,
				const Scenario& scenario,
				const security::User* user
			);

			static void AddNewSentScenarioEntry(
				const ScenarioTemplate& scenarioTemplate,
				const SentScenario& sentScenario,
				const security::User* user
			);


			static void AddNewSentScenarioEntry(
				const SentScenario& scenarioTemplate,
				const SentScenario& sentScenario,
				const security::User* user
			);


			static void AddNewSentScenarioEntry(
				const SentScenario& sentScenario,
				const security::User& user
			);


			static void	addUpdateEntry(
				const SentScenario* scenario
				, const std::string& text
				, const security::User* user
				);
			static void	addUpdateEntry(
				const Alarm* alarm
				, const std::string& text
				, const security::User* user
				);



			/** Adds an Alarm deletion entry.
				@param alarm Pointer to the deleted alarm : must be linked with the scenario
				@param user User who has launched the deletion
				@author Hugues Romain
				@date 2008
			*/
			static void	AddDeleteEntry(
				const Alarm* alarm,
				const security::User* user
			);

			static void	AddDeleteEntry(
				const SentScenario& scenario,
				const security::User& user
			);


			virtual std::string getObjectName(
				util::RegistryKeyType id,
				const server::Request& searchRequest
			) const;
		};
	}
}

#endif

