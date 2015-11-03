/** NotificationLog implementation.
	@file NotificationLog.cpp
	@author Yves Martin
	@date 2015

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2015 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include <Alarm.h>
#include <AlarmTableSync.h>
#include <DBDirectTableSyncTemplate.hpp>
#include <DBLogEntry.h>
#include <DBTableSync.hpp>
#include <DBTableSyncTemplate.hpp>
#include <Env.h>
#include <FactorableTemplate.h>
#include <NotificationEvent.hpp>
#include <NotificationLog.hpp>
#include <NotificationProvider.hpp>
#include <NotificationProviderTableSync.hpp>
#include <Object.hpp>
#include <Registrable.h>
#include <SentScenario.h>
#include <ScenarioTemplate.h>
#include <Scenario.h>
#include <SentScenarioTableSync.h>
#include <ScenarioTemplateTableSync.h>
#include <StringField.hpp>
#include <UtilTypes.h>

#include <boost/lexical_cast.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

#include <sstream>
#include <vector>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace dblog;

	namespace util
	{
		template<> const std::string FactorableTemplate<DBLog, messages::NotificationLog>::FACTORY_KEY("messagesnotification");
	}

	namespace messages
	{
		string NotificationLog::getName() const
		{
				return "Notification de messages";
		}



		DBLog::ColumnsVector NotificationLog::getColumnNames() const
		{
			DBLog::ColumnsVector v;
			v.push_back("Message");
			v.push_back("Action");
			return v;
		}


		std::string NotificationLog::getObjectColumnName() const
		{
			return "Evènement";
		}



		std::string NotificationLog::getObjectName(
			RegistryKeyType id,
			const server::Request& searchRequest
		) const	{
			try
			{
				RegistryTableType tableId = decodeTableId(id);
				Env env;
				if (tableId == SentScenarioTableSync::TABLE.ID)
				{
					boost::shared_ptr<const SentScenario> scenario(SentScenarioTableSync::Get(id, env, FIELDS_ONLY_LOAD_LEVEL));
					return scenario->getName();
				}
				else if (tableId == ScenarioTemplateTableSync::TABLE.ID)
				{
					boost::shared_ptr<const ScenarioTemplate> scenario(ScenarioTemplateTableSync::Get(id, env, FIELDS_ONLY_LOAD_LEVEL));
					return scenario->getName();
				}
				else if (tableId == AlarmTableSync::TABLE.ID)
				{
					boost::shared_ptr<const Alarm> alarm(AlarmTableSync::Get(id, env, FIELDS_ONLY_LOAD_LEVEL));
					return alarm->getShortMessage();
				}
				else if (tableId == NotificationProviderTableSync::TABLE.ID)
				{
					boost::shared_ptr<const NotificationProvider> provider(NotificationProviderTableSync::Get(id, env, FIELDS_ONLY_LOAD_LEVEL));
					return provider->getName();
				}
			}
			catch (...)
			{
			}
			return DBLog::getObjectName(id,searchRequest);
		}



		std::string NotificationLog::getObject2ColumnName() const
		{
			return "Fournisseur";
		}




		std::string NotificationLog::getObject2Name(
			RegistryKeyType id,
			const server::Request& searchRequest
		) const	{
			try
			{
				RegistryTableType tableId = decodeTableId(id);
				Env env;
				if (tableId == NotificationProviderTableSync::TABLE.ID)
				{
					boost::shared_ptr<const NotificationProvider> provider(NotificationProviderTableSync::Get(id, env, FIELDS_ONLY_LOAD_LEVEL));
					return provider->getName();
				}
			}
			catch (...)
			{
			}
			return DBLog::getObjectName(id,searchRequest);
		}



		void NotificationLog::AddNotificationEventEntry(
			const boost::shared_ptr<NotificationEvent>& event
		) {
			if (!event) return;
			Alarm& alarm = event->get<Alarm>().get();
			NotificationProvider& provider = event->get<NotificationProvider>().get();

			Level level = DB_LOG_INFO;
			DBLog::ColumnsVector content;
			content.push_back(lexical_cast<string>(alarm.getKey()));

			stringstream text;
			text << "Notification vers '" << provider.getName();

			if (event->get<Status>() == FAILED)
			{
				text << "' en échec pour le message '";
				level = DB_LOG_ERROR;
			}
			else if (event->get<Status>() == IN_PROGRESS)
			{
				text << "' en reprise pour le message '";
				level = DB_LOG_WARNING;
			}
			else if (event->get<Status>() == SUCCESS)
			{
				text << "' réussi pour le message '";
			}
			text << alarm.getShortMessage() << "'";
			content.push_back(text.str());
			_addEntry(FACTORY_KEY, level, content, NULL,
					(alarm.getScenario() ? alarm.getScenario()->getKey() : 0),
					provider.getKey()
			);
		}

		void NotificationLog::AddNotificationProviderFailure(
			const NotificationProvider* provider,
			const std::string details,
			const Alarm* alarm /* = NULL */
		) {
			DBLog::ColumnsVector content;
			if (alarm)
			{
				content.push_back(lexical_cast<string>(alarm->getKey()));
			} else {
				content.push_back(string());
			}
			stringstream text;
			text << "Echec vers '" << provider->getName();
			if (alarm) {
				text << "' pour le message '" << alarm->getShortMessage();
			}
			text << "': " << details;
			content.push_back(text.str());

			_addEntry(FACTORY_KEY, DB_LOG_ERROR, content, NULL,
					(alarm ? alarm->getScenario()->getKey() : 0),
					provider->getKey()
			);
		}

	} /* namespace messages */
} /* namespace synthese */
