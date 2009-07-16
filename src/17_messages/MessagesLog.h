
/** MessagesLog class header.
	@file MessagesLog.h

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

#ifndef SYNTHESE_MESSAGES_LOG
#define SYNTHESE_MESSAGES_LOG

#include "DBLogTemplate.h"

namespace synthese
{
	namespace messages
	{
		class Alarm;
		class SentAlarm;
		class ScenarioTemplate;
		class SentScenario;
		class SentAlarm;
		class Scenario;

		/** Journal des envois de messages.

			Les entr�es du journal de messages sont :
				- envois de messages
				- envois selon sc�narios de diffusion
				
			Les colonnes additionnelles du journal de s�curit� sont :
				- Action : d�crit l'action effectu�e (ex : cr�ation de profil)
				- UID objet concern� : 
					- lien vers le message si envoi de message unique
					- lien vers le sc�nario si envoi selon un sc�nario de diffusion
			
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
				const SentAlarm* alarm
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
				const SentAlarm* alarm
				, const security::User* user
			);


			std::string getObjectName(uid id) const;
		};
	}
}

#endif

