
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

#include "Conversion.h"

#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"
#include "DBModule.h"

#include "DateTime.h"

#include "User.h"
#include "UserTableSync.h"

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

	namespace dblog
	{
		const string DBLogEntryTableSync::CONTENT_SEPARATOR("|");
		const std::string DBLogEntryTableSync::COL_LOG_KEY = "log_key";
		const std::string DBLogEntryTableSync::COL_DATE = "date";
		const std::string DBLogEntryTableSync::COL_USER_ID = "user_id";
		const std::string DBLogEntryTableSync::COL_LEVEL = "level";
		const std::string DBLogEntryTableSync::COL_CONTENT = "content";
		const std::string DBLogEntryTableSync::COL_OBJECT_ID = "object_id";
		const std::string DBLogEntryTableSync::COL_OBJECT2_ID = "object2_id";
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<DBLogEntryTableSync>::TABLE(
			"t045_log_entries", true
		);

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<DBLogEntryTableSync>::_FIELDS[] =
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(DBLogEntryTableSync::COL_LOG_KEY, SQL_TEXT),
			SQLiteTableSync::Field(DBLogEntryTableSync::COL_DATE, SQL_TIMESTAMP),
			SQLiteTableSync::Field(DBLogEntryTableSync::COL_USER_ID, SQL_INTEGER),
			SQLiteTableSync::Field(DBLogEntryTableSync::COL_LEVEL, SQL_INTEGER),
			SQLiteTableSync::Field(DBLogEntryTableSync::COL_CONTENT, SQL_TEXT),
			SQLiteTableSync::Field(DBLogEntryTableSync::COL_OBJECT_ID, SQL_INTEGER),
			SQLiteTableSync::Field(DBLogEntryTableSync::COL_OBJECT2_ID, SQL_INTEGER),
			SQLiteTableSync::Field()

		};
		
		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<DBLogEntryTableSync>::_INDEXES[] = 
		{
			SQLiteTableSync::Index(
				DBLogEntryTableSync::COL_LOG_KEY.c_str(),
				DBLogEntryTableSync::COL_OBJECT_ID.c_str(),
				DBLogEntryTableSync::COL_DATE.c_str(),
				""
			),
			SQLiteTableSync::Index(
				DBLogEntryTableSync::COL_LOG_KEY.c_str(),
				DBLogEntryTableSync::COL_OBJECT2_ID.c_str(),
				DBLogEntryTableSync::COL_DATE.c_str(),
				""
			),
			SQLiteTableSync::Index(
				DBLogEntryTableSync::COL_LOG_KEY.c_str(),
				DBLogEntryTableSync::COL_DATE.c_str(),
				""
			),
			SQLiteTableSync::Index()
		};

		template<> void SQLiteDirectTableSyncTemplate<DBLogEntryTableSync,DBLogEntry>::Load(
			DBLogEntry* object,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			object->setLogKey(rows->getText ( DBLogEntryTableSync::COL_LOG_KEY));
			object->setDate(DateTime::FromSQLTimestamp(rows->getText ( DBLogEntryTableSync::COL_DATE)));
			object->setLevel((DBLogEntry::Level) rows->getInt ( DBLogEntryTableSync::COL_LEVEL));
			object->setObjectId(rows->getLongLong(DBLogEntryTableSync::COL_OBJECT_ID));
			object->setObjectId2(rows->getLongLong(DBLogEntryTableSync::COL_OBJECT2_ID));

			// Content column : parse all contents separated by | 
			DBLogEntry::Content v;
			typedef tokenizer<char_separator<char> > tokenizer;
			string content = rows->getText ( DBLogEntryTableSync::COL_CONTENT);
			char_separator<char> sep (DBLogEntryTableSync::CONTENT_SEPARATOR.c_str(), "", keep_empty_tokens);
			
			tokenizer columns (content, sep);
			for (tokenizer::iterator it = columns.begin(); it != columns.end (); ++it)
				v.push_back(*it);
			object->setContent(v);

			uid userId(rows->getLongLong ( DBLogEntryTableSync::COL_USER_ID));
			object->setUserId(userId);
		}

		template<> void SQLiteDirectTableSyncTemplate<DBLogEntryTableSync, DBLogEntry>::Unlink(
			DBLogEntry* obj
		){
		}

		template<> void SQLiteDirectTableSyncTemplate<DBLogEntryTableSync,DBLogEntry>::Save(
			DBLogEntry* object,
			optional<SQLiteTransaction&> transaction
		){
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;

			if (object->getKey() <= 0)
				object->setKey(getId());

			query
				<< " REPLACE INTO " << TABLE.NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				<< "," << Conversion::ToSQLiteString(object->getLogKey())
				<< "," << object->getDate().toSQLString()
				<< "," << Conversion::ToString(object->getUserId())
				<< "," << Conversion::ToString(static_cast<int>(object->getLevel()))
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
				<< "," << object->getObjectId()
				<< "," << object->getObjectId2()
				<< ")";

			sqlite->execUpdate(query.str(), transaction);
		}

	}

	namespace dblog
	{
		DBLogEntryTableSync::SearchResult DBLogEntryTableSync::Search(
			Env& env,
			const std::string& logKey
			, const time::DateTime& startDate
			, const time::DateTime& endDate
			, uid userId
			, DBLogEntry::Level level
			, uid objectId
			, uid objectId2
			, const std::string& text
			, int first
			, boost::optional<std::size_t> number
			, bool orderByDate
			, bool orderByUser
			, bool orderByLevel
			, bool raisingOrder,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE "
					<< COL_LOG_KEY << "=" << Conversion::ToSQLiteString(logKey);
			if (!startDate.isUnknown())
				query << " AND " << COL_DATE << ">=" << startDate.toSQLString();
			if (!endDate.isUnknown())
				query << " AND " << COL_DATE << "<=" << endDate.toSQLString();
			if (userId != UNKNOWN_VALUE)
				query << " AND " << COL_USER_ID << "=" << userId;
			if (level != DBLogEntry::DB_LOG_UNKNOWN)
				query << " AND " << COL_LEVEL << "=" << Conversion::ToString((int) level);
			if (!text.empty())
				query << " AND " << COL_CONTENT << " LIKE '%" << Conversion::ToSQLiteString(text, false) << "%'";
			if (objectId != UNKNOWN_VALUE)
				query << " AND " << COL_OBJECT_ID << "=" << Conversion::ToString(objectId);
			if (objectId2 != UNKNOWN_VALUE)
				query << " AND " << COL_OBJECT2_ID << "=" << Conversion::ToString(objectId2);
			if (orderByDate)
				query << " ORDER BY " << COL_DATE << (raisingOrder ? " ASC" : " DESC");
			if (orderByUser)
				query << " ORDER BY " << COL_USER_ID << (raisingOrder ? " ASC" : " DESC") << "," << COL_DATE << (raisingOrder ? " ASC" : " DESC");
			if (orderByLevel)
				query << " ORDER BY " << COL_LEVEL << (raisingOrder ? " ASC" : " DESC") << "," << COL_DATE << (raisingOrder ? " ASC" : " DESC");
			if (number)
				query << " LIMIT " << (*number + 1);
			if (first > 0)
				query << " OFFSET " << first;

			return LoadFromQuery(query.str(), env, linkLevel);
		}



		void DBLogEntryTableSync::Purge( const std::string& logKey, const time::DateTime& endDate )
		{
			stringstream query;
			query <<
				"DELETE FROM " << TABLE.NAME <<
				" WHERE " << COL_DATE << "<=" << endDate.toSQLString() <<
				" AND " << COL_LOG_KEY << "=" << Conversion::ToSQLiteString(logKey)
			;
			DBModule::GetSQLite()->execQuery(query.str());
		}
	}
}
