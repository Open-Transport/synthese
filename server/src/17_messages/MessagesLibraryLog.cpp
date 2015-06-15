
/** MessagesLibraryLog class implementation.
	@file MessagesLibraryLog.cpp

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

#include "MessagesLibraryLog.h"
#include "Alarm.h"
#include "ScenarioTemplate.h"
#include "ScenarioTemplateTableSync.h"
#include "AlarmTableSync.h"
#include "TextTemplate.h"
#include "TextTemplateTableSync.h"
#include "MessagesLibraryRight.h"
#include "Request.h"
#include "SentScenario.h"
#include "ScenarioFolder.h"
#include "ScenarioFolderTableSync.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace dblog;
	using namespace util;
	using namespace messages;
	using namespace server;
	using namespace security;

	namespace util
	{
		template<> const std::string FactorableTemplate<DBLog, MessagesLibraryLog>::FACTORY_KEY = "messageslibrary";
	}

	namespace messages
	{
		DBLog::ColumnsVector MessagesLibraryLog::getColumnNames() const
		{
			DBLog::ColumnsVector v;
			v.push_back("Message");
			v.push_back("Action");
			return v;
		}



		std::string MessagesLibraryLog::getObjectName(
			RegistryKeyType id,
			const server::Request& searchRequest
		) const	{
			try
			{
				RegistryTableType tableId = decodeTableId(id);
				Env env;
				if (tableId == ScenarioTemplateTableSync::TABLE.ID)
				{
					boost::shared_ptr<const ScenarioTemplate> scenario(ScenarioTemplateTableSync::Get(id, env, FIELDS_ONLY_LOAD_LEVEL));
					return scenario->getName();
				}
				else if (tableId == AlarmTableSync::TABLE.ID)
				{
					boost::shared_ptr<const Alarm> alarm(AlarmTableSync::Get(id, env, FIELDS_ONLY_LOAD_LEVEL));
					return alarm->getShortMessage();
				}
				else if (tableId == TextTemplateTableSync::TABLE.ID)
				{
					boost::shared_ptr<const TextTemplate> text(TextTemplateTableSync::Get(id, env, FIELDS_ONLY_LOAD_LEVEL));
					return text->getName();
				}
				else if (tableId == ScenarioFolderTableSync::TABLE.ID)
				{
					boost::shared_ptr<const ScenarioFolder> folder(ScenarioFolderTableSync::Get(id, env));
					return folder->getFullName();
				}
			}
			catch (...)
			{
			}
			return DBLog::getObjectName(id,searchRequest);
		}

		void MessagesLibraryLog::addUpdateEntry(
			const ScenarioTemplate* scenario
			, const std::string& text
			, const security::User* user
		){
			DBLogEntry::Content content;
			content.push_back(string());
			content.push_back(text);

			_addEntry(FACTORY_KEY, DBLogEntry::DB_LOG_INFO, content, user, 0);
		}

		void MessagesLibraryLog::addUpdateEntry(
			const Alarm* alarm
			, const std::string& text
			, const security::User* user
		){
			if(!alarm->getScenario()) return;

			DBLogEntry::Content content;
			content.push_back(lexical_cast<string>(alarm->getKey()));
			content.push_back(text);

			_addEntry(FACTORY_KEY, DBLogEntry::DB_LOG_INFO, content, user, alarm->getScenario()->getKey());
		}

		std::string MessagesLibraryLog::getName() const
		{
			return "Bibliothèque de messages";
		}

		void MessagesLibraryLog::addDeleteEntry(
			const ScenarioTemplate* scenario
			, const security::User* user
		){
			DBLogEntry::Content content;
			content.push_back(string());
			content.push_back("Suppression du scénario " + scenario->getName());

			_addEntry(FACTORY_KEY, DBLogEntry::DB_LOG_INFO, content, user, scenario->getKey());
		}

		void MessagesLibraryLog::addCreateEntry(
			const ScenarioTemplate& scenario,
			const ScenarioTemplate& scenarioTemplate,
			const security::User* user
		){
			DBLogEntry::Content content;
			content.push_back(string());
			stringstream text;
			text
				<< "Création du scénario " << scenario.getName()
				<< " par copie du scénario " << scenarioTemplate.getName();
			content.push_back(text.str());

			_addEntry(FACTORY_KEY, DBLogEntry::DB_LOG_INFO, content, user, scenario.getKey());
		}

		void MessagesLibraryLog::addCreateEntry(
			const ScenarioTemplate& scenario,
			const security::User* user
		){
			DBLogEntry::Content content;
			content.push_back(string());
			stringstream text;
			text << "Création du scénario " << scenario.getName();
			content.push_back(text.str());

			_addEntry(FACTORY_KEY, DBLogEntry::DB_LOG_INFO, content, user, scenario.getKey());
		}


		void MessagesLibraryLog::AddTemplateInstanciationEntry(
			const SentScenario& scenario
			, const security::User* user
		){
			DBLogEntry::Content content;
			content.push_back(string());
			stringstream text;
			text << "Déclenchement du scénario (id=" << scenario.getKey() << ")";
			content.push_back(text.str());

			_addEntry(
				FACTORY_KEY,
				DBLogEntry::DB_LOG_INFO,
				content,
				user,
				scenario.getTemplate() ? scenario.getTemplate()->getKey() : 0
			);
		}


		void MessagesLibraryLog::AddDeleteEntry( const Alarm* alarm , const security::User* user )
		{
			if(!alarm->getScenario()) return;

			DBLog::ColumnsVector content;
			content.push_back(lexical_cast<string>(alarm->getKey()));
			stringstream text;
			text
				<< "Suppression du message " << alarm->getShortMessage()
				<< " du scénario " << alarm->getScenario()->getName();
			content.push_back(text.str());
			_addEntry(FACTORY_KEY, DBLogEntry::DB_LOG_INFO, content, user, alarm->getScenario()->getKey());
		}



		void MessagesLibraryLog::AddDeleteEntry( const ScenarioFolder& folder , const security::User* user )
		{
			DBLog::ColumnsVector content;
			content.push_back("Répertoire de scénarios");
			content.push_back("Suppression de "+ folder.getFullName());
			_addEntry(FACTORY_KEY, DBLogEntry::DB_LOG_INFO, content, user, folder.getKey());
		}


		void MessagesLibraryLog::AddTemplateDeleteEntry( const TextTemplate& text , const security::User* user )
		{
			DBLog::ColumnsVector content;
			content.push_back("Modèle de texte " + string((text.getAlarmLevel() == ALARM_LEVEL_WARNING) ? "prioritaire" : "complémentaire"));
			content.push_back("Suppression");
			_addEntry(FACTORY_KEY, DBLogEntry::DB_LOG_INFO, content, user, text.getKey());
		}



		void MessagesLibraryLog::AddTemplateUpdateEntry( const TextTemplate& text , const std::string& changes , const security::User* user )
		{
			DBLog::ColumnsVector content;
			content.push_back("Modèle de texte " + string((text.getAlarmLevel() == ALARM_LEVEL_WARNING) ? "prioritaire" : "complémentaire"));
			content.push_back("Modification : " + changes);
			_addEntry(FACTORY_KEY, DBLogEntry::DB_LOG_INFO, content, user, text.getKey());
		}



		void MessagesLibraryLog::AddTemplateCreationEntry( const TextTemplate& text , const security::User* user )
		{
			DBLog::ColumnsVector content;
			content.push_back("Modèle de texte " + string((text.getAlarmLevel() == ALARM_LEVEL_WARNING) ? "prioritaire" : "complémentaire"));
			content.push_back("Création");
			_addEntry(FACTORY_KEY, DBLogEntry::DB_LOG_INFO, content, user, text.getKey());
		}



		void MessagesLibraryLog::AddTemplateFolderUpdateEntry( const TextTemplate& text , const std::string& changes , const security::User* user )
		{
			DBLog::ColumnsVector content;
			content.push_back("Répertoire de modèles de textes" + text.getName());
			content.push_back("Modification : " + changes);
			_addEntry(FACTORY_KEY, DBLogEntry::DB_LOG_INFO, content, user, text.getKey());
		}



		void MessagesLibraryLog::AddCreateEntry( const ScenarioFolder& folder , const security::User* user )
		{
			DBLog::ColumnsVector content;
			content.push_back("Répertoire de scénarios");
			content.push_back("Création");
			_addEntry(FACTORY_KEY, DBLogEntry::DB_LOG_INFO, content, user, folder.getKey());
		}
	}
}
