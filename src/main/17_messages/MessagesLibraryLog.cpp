
/** MessagesLibraryLog class implementation.
	@file MessagesLibraryLog.cpp

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

#include "17_messages/MessagesLibraryLog.h"
#include "17_messages/AlarmTemplate.h"
#include "17_messages/ScenarioTemplate.h"
#include "17_messages/ScenarioTableSync.h"

#include "01_util/Conversion.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace dblog;
	using namespace util;
	using namespace messages;

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

		std::string MessagesLibraryLog::getObjectName( uid id ) const
		{
			try
			{
				shared_ptr<Scenario> scenario = ScenarioTableSync::getScenario(id);
				return scenario->getName();
			}
			catch (...)
			{
				return Conversion::ToString(id);
			}			
		}

		void MessagesLibraryLog::addUpdateEntry( boost::shared_ptr<const ScenarioTemplate> scenario , const std::string& text , boost::shared_ptr<const security::User> user )
		{
			DBLogEntry::Content content;
			content.push_back(string());
			content.push_back(text);
			
			_addEntry(FACTORY_KEY, DBLogEntry::DB_LOG_INFO, content, user, scenario->getKey());
		}

		void MessagesLibraryLog::addUpdateEntry( boost::shared_ptr<const AlarmTemplate> alarm , const std::string& text , boost::shared_ptr<const security::User> user )
		{
			DBLogEntry::Content content;
			content.push_back(Conversion::ToString(alarm->getKey()));
			content.push_back(text);

			_addEntry(FACTORY_KEY, DBLogEntry::DB_LOG_INFO, content, user, alarm->getScenarioId());
		}

		std::string MessagesLibraryLog::getName() const
		{
			return "Administration bibliothèque de messages";
		}
	}
}
