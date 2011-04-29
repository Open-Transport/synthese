
/** MessagesLog class implementation.
	@file MessagesLog.cpp

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

#include "MessagesLog.h"
#include "SentAlarm.h"
#include "SentScenario.h"
#include "AlarmTableSync.h"
#include "ScenarioTableSync.h"
#include "Env.h"
#include "MessagesRight.h"
#include "Request.h"
#include "ScenarioTemplate.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace dblog;
	using namespace util;
	using namespace messages;
	using namespace server;
	using namespace security;

	namespace util
	{
		template<> const std::string FactorableTemplate<DBLog, MessagesLog>::FACTORY_KEY("messages");
	}

	namespace messages
	{
		DBLog::ColumnsVector MessagesLog::getColumnNames() const
		{
			DBLog::ColumnsVector v;
			v.push_back("Message");
			v.push_back("Action");
			return v;
		}



		void MessagesLog::AddNewScenarioMessageEntry(
			const Alarm& alarm,
			const Scenario& scenario,
			const security::User* user
		){
			DBLog::ColumnsVector content;
			content.push_back(lexical_cast<string>(alarm.getKey()));
			content.push_back("Ajout de message");
			_addEntry(FACTORY_KEY, DBLogEntry::DB_LOG_INFO, content, user, scenario.getKey());
		}



		void MessagesLog::AddNewSentScenarioEntry(
			const ScenarioTemplate& scenarioTemplate,
			const SentScenario& sentScenario,
			const security::User* user
		){
			DBLog::ColumnsVector content;
			content.push_back(string());
			content.push_back("Création d'après le modèle "+ scenarioTemplate.getName());
			_addEntry(FACTORY_KEY, DBLogEntry::DB_LOG_INFO, content, user, sentScenario.getKey());
		}



		void MessagesLog::AddNewSentScenarioEntry(
			const SentScenario& scenarioTemplate,
			const SentScenario& sentScenario,
			const security::User* user
		){
			DBLog::ColumnsVector content;
			content.push_back(string());
			content.push_back(
				"Création par copie de "+ scenarioTemplate.getName() +
				" (modèle "+ scenarioTemplate.getTemplate()->getName() +")"
			);
			_addEntry(FACTORY_KEY, DBLogEntry::DB_LOG_INFO, content, user, sentScenario.getKey());
		}

		void MessagesLog::AddNewSentScenarioEntry( const SentScenario& sentScenario, const security::User& user )
		{
			DBLog::ColumnsVector content;
			content.push_back(string());
			content.push_back("Création");
			_addEntry(FACTORY_KEY, DBLogEntry::DB_LOG_INFO, content, &user, sentScenario.getKey());
		}

		void MessagesLog::addUpdateEntry(
			const SentScenario* scenario
			, const std::string& text
			, const security::User* user
		){
			DBLog::ColumnsVector content;
			content.push_back(string());
			content.push_back(text);
			_addEntry(FACTORY_KEY, DBLogEntry::DB_LOG_INFO, content, user, scenario->getKey());
		}

		void MessagesLog::addUpdateEntry(
			const SentAlarm* alarm
			, const std::string& text
			, const security::User* user
		){
			if(!alarm->getScenario()) return;

			DBLog::ColumnsVector content;
			content.push_back(lexical_cast<string>(alarm->getKey()));
			content.push_back(text);
			_addEntry(FACTORY_KEY, DBLogEntry::DB_LOG_INFO, content, user, alarm->getScenario()->getKey());
		}

		std::string MessagesLog::getObjectName(
			RegistryKeyType id,
			const server::Request& searchRequest
		) const	{
			Env env;
			int tableId = decodeTableId(id);

			try
			{
				if (tableId == AlarmTableSync::TABLE.ID)
				{
					shared_ptr<const Alarm> alarm(AlarmTableSync::Get(id, env, FIELDS_ONLY_LOAD_LEVEL));
					return alarm->getShortMessage();
				}
				else if (tableId == ScenarioTableSync::TABLE.ID)
				{
					shared_ptr<const Scenario> scenario(ScenarioTableSync::Get(id, env, FIELDS_ONLY_LOAD_LEVEL));
					return scenario->getName();
				}
			}
			catch(...)
			{
			}

			return DBLog::getObjectName(id, searchRequest);
		}

		std::string MessagesLog::getName() const
		{
			return "Diffusion de messages";
		}

		void MessagesLog::AddDeleteEntry( const SentAlarm* alarm , const security::User* user )
		{
			DBLog::ColumnsVector content;
			content.push_back(lexical_cast<string>(alarm->getKey()));
			stringstream text;
			text
				<< "Suppression du message " << alarm->getShortMessage()
				<< " du scénario " << alarm->getScenario()->getName();
			content.push_back(text.str());
			_addEntry(FACTORY_KEY, DBLogEntry::DB_LOG_INFO, content, user, alarm->getScenario()->getKey());
		}



		void MessagesLog::AddDeleteEntry( const SentScenario& scenario, const security::User& user )
		{
			DBLog::ColumnsVector content;
			content.push_back(lexical_cast<string>(scenario.getKey()));
			stringstream text;
			text << "Suppression du scénario " << scenario.getName();
			content.push_back(text.str());
			_addEntry(FACTORY_KEY, DBLogEntry::DB_LOG_INFO, content, &user, scenario.getKey());
		}
}	}
