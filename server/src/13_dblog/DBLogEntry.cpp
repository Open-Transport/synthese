
/** DBLogEntry class implementation.
	@file DBLogEntry.cpp

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

#include "DBLogEntry.h"

#include "Profile.h"
#include "Registry.h"
#include "Session.h"
#include "User.h"

#include <sstream>

using namespace boost;
using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace dblog;
	using namespace util;

	CLASS_DEFINITION(DBLogEntry, "t045_log_entries", 45)
	FIELD_DEFINITION_OF_OBJECT(DBLogEntry, "log_entry_id", "log_entry_ids")

	FIELD_DEFINITION_OF_TYPE(LogKey, "log_key", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(LogDate, "date", SQL_DATE)
	FIELD_DEFINITION_OF_TYPE(LogUser, "user_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(LogLevel, "level", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(LogContent, "content", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(ObjectId, "object_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(Object2Id, "object2_id", SQL_INTEGER)

	namespace dblog
	{
		const string DBLogEntry::CONTENT_SEPARATOR("|");

		void DBLogEntry::setDate( const boost::posix_time::ptime& date )
		{
			set<LogDate>(date);
		}

		void DBLogEntry::setUserId(util::RegistryKeyType value)
		{
			set<LogUser>(value);
		}

		void DBLogEntry::setLogKey( const std::string& key )
		{
			set<LogKey>(key);
		}
		void DBLogEntry::setContent( const Content& content )
		{
			_content = content;
			string strContent("");
			for (DBLogEntry::Content::const_iterator it = content.begin(); it != content.end(); ++it)
			{
				if (it != content.begin())
					strContent += DBLogEntry::CONTENT_SEPARATOR;
				strContent += *it;
			}
			set<LogContent>(strContent);
		}

		boost::posix_time::ptime DBLogEntry::getDate() const
		{
			return get<LogDate>();
		}

		util::RegistryKeyType DBLogEntry::getUserId() const
		{
			return get<LogUser>();
		}

		const DBLogEntry::Content& DBLogEntry::getContent() const
		{
			return _content;
		}

		const std::string& DBLogEntry::getLogKey() const
		{
			return get<LogKey>();
		}

		Level DBLogEntry::getLevel() const
		{
			return get<LogLevel>();
		}

		void DBLogEntry::setLevel( Level level )
		{
			set<LogLevel>(level);
		}

		util::RegistryKeyType DBLogEntry::getObjectId() const
		{
			return get<ObjectId>();
		}

		void DBLogEntry::setObjectId(util::RegistryKeyType id )
		{
			set<ObjectId>(id);
		}

		string DBLogEntry::getStringContent() const
		{
			stringstream s;
			for (Content::const_iterator it = _content.begin(); it != _content.end(); ++it)
			{
				s << *it << " ";
			}
			return s.str();
		}

		DBLogEntry::DBLogEntry(RegistryKeyType key)
		:	Registrable(key),
			Object<DBLogEntry, DBLogEntrySchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, key),
					FIELD_DEFAULT_CONSTRUCTOR(LogKey),
					FIELD_VALUE_CONSTRUCTOR(LogDate, second_clock::local_time()),
					FIELD_DEFAULT_CONSTRUCTOR(LogUser),
					FIELD_DEFAULT_CONSTRUCTOR(LogLevel),
					FIELD_DEFAULT_CONSTRUCTOR(LogContent),
					FIELD_DEFAULT_CONSTRUCTOR(ObjectId),
					FIELD_DEFAULT_CONSTRUCTOR(Object2Id)
			)	)
		{

		}

		void DBLogEntry::setObjectId2( util::RegistryKeyType id )
		{
			set<Object2Id>(id);
		}

		util::RegistryKeyType DBLogEntry::getObjectId2() const
		{
			return get<Object2Id>();
		}

		void DBLogEntry::link(util::Env& env, bool withAlgorithmOptimizations)
		{
			// Content column : parse all contents separated by |
			DBLogEntry::Content v;
			typedef tokenizer<char_separator<char> > tokenizer;
			string content = get<LogContent>();
			char_separator<char> sep (DBLogEntry::CONTENT_SEPARATOR.c_str(), "", keep_empty_tokens);

			tokenizer columns (content, sep);
			for (tokenizer::iterator it = columns.begin(); it != columns.end (); ++it)
				v.push_back(*it);
			_content = v;
		}

		bool DBLogEntry::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool DBLogEntry::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool DBLogEntry::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::DELETE_RIGHT);
		}
	}
}
