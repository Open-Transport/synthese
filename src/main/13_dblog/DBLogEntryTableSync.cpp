
/** DBLogEntryTableSync class implementation.
	@file DBLogEntryTableSync.cpp

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

#include <sstream>

#include <boost/tokenizer.hpp>

#include "01_util/Conversion.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLite.h"
#include "02_db/SQLiteException.h"

#include "04_time/DateTime.h"

#include "12_security/User.h"
#include "12_security/UserTableSync.h"

#include "DBLogEntry.h"
#include "DBLogEntryTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace dblog;
	using namespace time;
	using namespace security;

	template<> const string util::FactorableTemplate<SQLiteTableSync,DBLogEntryTableSync>::FACTORY_KEY("13.01 DB Log entries");

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<DBLogEntryTableSync>::TABLE_NAME = "t045_log_entries";
		template<> const int SQLiteTableSyncTemplate<DBLogEntryTableSync>::TABLE_ID = 45;
		template<> const bool SQLiteTableSyncTemplate<DBLogEntryTableSync>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteDirectTableSyncTemplate<DBLogEntryTableSync,DBLogEntry>::load(DBLogEntry* object, const db::SQLiteResultSPtr& rows )
		{
			object->setKey(rows->getLongLong (TABLE_COL_ID));
			object->setLogKey(rows->getText ( DBLogEntryTableSync::COL_LOG_KEY));
			object->setDate(DateTime::FromSQLTimestamp(rows->getText ( DBLogEntryTableSync::COL_DATE)));
			object->setLevel((DBLogEntry::Level) rows->getInt ( DBLogEntryTableSync::COL_LEVEL));
			object->setObjectId(rows->getLongLong ( DBLogEntryTableSync::COL_OBJECT_ID));

			// Content column : parse all contents separated by | 
			DBLogEntry::Content v;
			typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
			string content = rows->getText ( DBLogEntryTableSync::COL_CONTENT);
			if (content.size() && content.at(0) == *DBLogEntryTableSync::CONTENT_SEPARATOR.c_str())
				v.push_back(string());
			boost::char_separator<char> sep (DBLogEntryTableSync::CONTENT_SEPARATOR.c_str());
			tokenizer columns (content, sep);
			for (tokenizer::iterator it = columns.begin(); it != columns.end (); ++it)
				v.push_back(*it);
			object->setContent(v);
		}

		template<> void SQLiteDirectTableSyncTemplate<DBLogEntryTableSync, DBLogEntry>::_link(DBLogEntry* obj, const SQLiteResultSPtr& rows, GetSource temporary)
		{
			// User ID
			uid userId(rows->getLongLong ( DBLogEntryTableSync::COL_USER_ID));

			if (userId > 0)
			{
				try
				{
					obj->setUser(UserTableSync::Get(userId, obj, true, temporary));
				}
				catch (User::ObjectNotFoundException& e)
				{					
					/// @todo See if an exception should be thrown
				}
			}


		}

		template<> void SQLiteDirectTableSyncTemplate<DBLogEntryTableSync, DBLogEntry>::_unlink(DBLogEntry* obj)
		{
			obj->setUser(NULL);
		}

		template<> void SQLiteDirectTableSyncTemplate<DBLogEntryTableSync,DBLogEntry>::save(DBLogEntry* object)
		{
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;

			if (object->getKey() <= 0)
				object->setKey(getId());

			query
				<< " REPLACE INTO " << TABLE_NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				<< "," << Conversion::ToSQLiteString(object->getLogKey())
				<< "," << object->getDate().toSQLString()
				<< "," << (object->getUser() ? Conversion::ToString(object->getUser()->getKey()) : "0")
				<< "," << Conversion::ToString((int) object->getLevel())
				<< ",'";

			DBLogEntry::Content c = object->getContent();
			for (DBLogEntry::Content::const_iterator it = c.begin(); it != c.end(); ++it)
			{
				if (it != c.begin())
					query << DBLogEntryTableSync::CONTENT_SEPARATOR;
				query << Conversion::ToSQLiteString(*it, false);
			}

			query
				<< "'"
				<< "," << Conversion::ToString(object->getObjectId())
				<< ")";

			sqlite->execUpdate(query.str());
		}

	}

	namespace dblog
	{
		const string DBLogEntryTableSync::CONTENT_SEPARATOR("|");
		const std::string DBLogEntryTableSync::COL_LOG_KEY = "log_key";
		const std::string DBLogEntryTableSync::COL_DATE = "date";
		const std::string DBLogEntryTableSync::COL_USER_ID = "user_id";
		const std::string DBLogEntryTableSync::COL_LEVEL = "level";
		const std::string DBLogEntryTableSync::COL_CONTENT = "content";
		const std::string DBLogEntryTableSync::COL_OBJECT_ID = "object_id";

		DBLogEntryTableSync::DBLogEntryTableSync()
			: SQLiteNoSyncTableSyncTemplate<DBLogEntryTableSync,DBLogEntry>()
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(COL_LOG_KEY, "TEXT");
			addTableColumn(COL_DATE, "TIMESTAMP");
			addTableColumn(COL_USER_ID, "INTEGER");
			addTableColumn(COL_LEVEL, "INTEGER");
			addTableColumn(COL_CONTENT, "TEXT");
			addTableColumn(COL_OBJECT_ID, "INTEGER");

			vector<string> m1;
			m1.push_back(COL_LOG_KEY);
			m1.push_back(COL_OBJECT_ID);
			m1.push_back(COL_DATE);
			addTableIndex(m1);
		}

		std::vector<shared_ptr<DBLogEntry> > DBLogEntryTableSync::search(
			const std::string& logKey
			, const time::DateTime& startDate
			, const time::DateTime& endDate
			, const shared_ptr<const User> user
			, DBLogEntry::Level level
			, uid objectId
			, const std::string& text
			, int first
			, int number
			, bool orderByDate
			, bool orderByUser
			, bool orderByLevel
			, bool raisingOrder
		){
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE "
					<< COL_LOG_KEY << "=" << Conversion::ToSQLiteString(logKey);
			if (!startDate.isUnknown())
				query << " AND " << COL_DATE << ">=" << startDate.toSQLString();
			if (!endDate.isUnknown())
				query << " AND " << COL_DATE << "<=" << endDate.toSQLString();
			if (user.get())
				query << " AND " << COL_USER_ID << "=" << user->getKey();
			if (level != DBLogEntry::DB_LOG_UNKNOWN)
				query << " AND " << COL_LEVEL << "=" << Conversion::ToString((int) level);
			if (!text.empty())
				query << " AND " << COL_CONTENT << " LIKE '%" << Conversion::ToSQLiteString(text, false) << "%'";
			if (objectId != UNKNOWN_VALUE)
				query << " AND " << COL_OBJECT_ID << "=" << Conversion::ToString(objectId);
			if (orderByDate)
				query << " ORDER BY " << COL_DATE << (raisingOrder ? " ASC" : " DESC");
			if (orderByUser)
				query << " ORDER BY " << COL_USER_ID << (raisingOrder ? " ASC" : " DESC") << "," << COL_DATE << (raisingOrder ? " ASC" : " DESC");
			if (orderByLevel)
				query << " ORDER BY " << COL_LEVEL << (raisingOrder ? " ASC" : " DESC") << "," << COL_DATE << (raisingOrder ? " ASC" : " DESC");
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteResultSPtr rows = sqlite->execQuery(query.str());
				vector<shared_ptr<DBLogEntry> > objects;
				while (rows->next ())
				{
					shared_ptr<DBLogEntry> object(new DBLogEntry());
					load(object.get(), rows);
					link(object.get(), rows, GET_AUTO);
					objects.push_back(object);
				}
				return objects;
			}
			catch(SQLiteException& e)
			{
				throw Exception(e.getMessage());
			}
		}
	}
}
