
/** DBLog class implementation.
	@file DBLog.cpp

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

#include "DBLog.h"
#include "DBLogEntryTableSync.h"
#include "User.h"

#include <boost/lexical_cast.hpp>

using namespace boost;
using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	namespace dblog
	{

		RegistryKeyType DBLog::_addEntry(
			const std::string& logKey
			, Level level
			, const DBLogEntry::Content& content
			, const security::User* user /*= NULL*/
			, util::RegistryKeyType objectId,
			util::RegistryKeyType objectId2
		){
			DBLogEntry e;
			e.setLevel(level);
			e.setUserId(user ? user->getKey() : 0);
			e.setLogKey(logKey);
			e.setContent(content);
			e.setObjectId(objectId);
			e.setObjectId2(objectId2);
			DBLogEntryTableSync::Save(&e);

			return e.getKey();
		}



		/** Reads the last entry of a log.
			@param logKey key of the DBLog to write
			@param objectId id of the referring object (can be undefined)
			@return The last log entry of the specified log, referring the specified object if any
		*/
		boost::shared_ptr<DBLogEntry> DBLog::_getLastEntry(
			const std::string& logKey,
			boost::optional<util::RegistryKeyType> objectId
		){
			Env env;
			DBLogEntryTableSync::SearchResult entries(
				DBLogEntryTableSync::Search(
					env,
					logKey,
					ptime(not_a_date_time),
					ptime(not_a_date_time),
					optional<RegistryKeyType>(),
					DB_LOG_UNKNOWN,
					objectId,
					optional<RegistryKeyType>(),
					string(),
					0, 1,
					true, false, false,false
			)	);
			return
				entries.empty() ?
				boost::shared_ptr<DBLogEntry>() :
				entries.front()
			;
		}



		DBLog::ColumnsVector DBLog::parse(
			const DBLogEntry& entry,
			const Request& searchRequest
		) const	{
			return static_cast<ColumnsVector>(entry.getContent());
		}



		std::string DBLog::getObjectName(
			RegistryKeyType id,
			const server::Request& searchRequest
		) const	{
			return lexical_cast<string>(id);
		}



		RegistryKeyType DBLog::AddSimpleEntry(
			const std::string& logKey,
			Level level,
			const std::string& content,
			const security::User* user,
			util::RegistryKeyType objectId /*= 0 */,
			RegistryKeyType objectId2
		){
			boost::shared_ptr<DBLog> dbLog(Factory<DBLog>::create(logKey));
			int cols(dbLog->getColumnNames().size());
			DBLogEntry::Content c;
			c.push_back(content);
			for(int i(1); i<cols; ++i)
			{
				c.push_back(string());
			}
			return _addEntry(logKey, level, c, user, objectId, objectId2);
		}

		std::string DBLog::getObjectColumnName() const
		{
			return string("Objet");
		}

		std::string DBLog::getObject2ColumnName() const
		{
			return string();
		}



		std::string DBLog::getObject2Name(
			util::RegistryKeyType id,
			const server::Request& searchRequest
		) const	{
			return lexical_cast<string>(id);
		}
	}
}
