
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

#include "17_messages/MessagesLog.h"
#include "17_messages/SingleSentAlarm.h"
#include "17_messages/ScenarioSentAlarm.h"
#include "17_messages/SentScenario.h"
#include "17_messages/AlarmTableSync.h"
#include "17_messages/ScenarioTableSync.h"

#include "01_util/Conversion.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace dblog;
	using namespace util;
	using namespace messages;

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

		void MessagesLog::addUpdateEntry(
			const SingleSentAlarm* alarm
			, const std::string& text
			, const security::User* user
		){
			DBLog::ColumnsVector content;
			content.push_back(string());
			content.push_back(text);
			_addEntry(FACTORY_KEY, DBLogEntry::DB_LOG_INFO, content, user, alarm->getKey());
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
			const ScenarioSentAlarm* alarm
			, const std::string& text
			, const security::User* user
		){
			DBLog::ColumnsVector content;
			content.push_back(Conversion::ToString(alarm->getKey()));
			content.push_back(text);
			_addEntry(FACTORY_KEY, DBLogEntry::DB_LOG_INFO, content, user, alarm->getScenario()->getKey());
		}

		std::string MessagesLog::getObjectName( uid id ) const
		{
			int tableId = decodeTableId(id);

			if (tableId == AlarmTableSync::TABLE_ID)
			{
				shared_ptr<const Alarm> alarm(AlarmTableSync::Get(id));
				return alarm->getShortMessage();
			}
			else if (tableId == ScenarioTableSync::TABLE_ID)
			{
				shared_ptr<const Scenario> scenario(ScenarioTableSync::Get(id));
				return scenario->getName();
			}

			return std::string();
		}

		std::string MessagesLog::getName() const
		{
			return "Diffusion de messages";
		}
	}
}
