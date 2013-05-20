
/** DBLogEntryTableSync class implementation.
	@file DBLogEntryTableSync.cpp

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

#include "DBLogEntryTableSync.h"

#include "DeleteQuery.hpp"
#include "DBLog.h"
#include "DBLogRight.h"
#include "Profile.h"
#include "PtimeField.hpp"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "Session.h"
#include "UserTableSync.h"

#include <sstream>
#include <boost/tokenizer.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace dblog;
	using namespace security;

	template<> const string util::FactorableTemplate<DBTableSync,DBLogEntryTableSync>::FACTORY_KEY("13.01 DB Log entries");

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
		template<> const DBTableSync::Format DBTableSyncTemplate<DBLogEntryTableSync>::TABLE(
			"t045_log_entries", true
		);

		template<> const Field DBTableSyncTemplate<DBLogEntryTableSync>::_FIELDS[] =
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(DBLogEntryTableSync::COL_LOG_KEY, SQL_TEXT),
			Field(DBLogEntryTableSync::COL_DATE, SQL_DATETIME),
			Field(DBLogEntryTableSync::COL_USER_ID, SQL_INTEGER),
			Field(DBLogEntryTableSync::COL_LEVEL, SQL_INTEGER),
			Field(DBLogEntryTableSync::COL_CONTENT, SQL_TEXT),
			Field(DBLogEntryTableSync::COL_OBJECT_ID, SQL_INTEGER),
			Field(DBLogEntryTableSync::COL_OBJECT2_ID, SQL_INTEGER),
			Field()

		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<DBLogEntryTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(
					DBLogEntryTableSync::COL_LOG_KEY.c_str(),
					DBLogEntryTableSync::COL_OBJECT_ID.c_str(),
					DBLogEntryTableSync::COL_DATE.c_str(),
					""
			)	);
			r.push_back(
				DBTableSync::Index(
					DBLogEntryTableSync::COL_LOG_KEY.c_str(),
					DBLogEntryTableSync::COL_OBJECT2_ID.c_str(),
					DBLogEntryTableSync::COL_DATE.c_str(),
					""
			)	);
			r.push_back(
				DBTableSync::Index(
					DBLogEntryTableSync::COL_LOG_KEY.c_str(),
					DBLogEntryTableSync::COL_DATE.c_str(),
					""
			)	);
			return r;
		}

		template<>
		void OldLoadSavePolicy<DBLogEntryTableSync,DBLogEntry>::Load(
			DBLogEntry* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			object->setLogKey(rows->getText ( DBLogEntryTableSync::COL_LOG_KEY));
			object->setDate(rows->getDateTime( DBLogEntryTableSync::COL_DATE));
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

			object->setUserId(rows->getLongLong ( DBLogEntryTableSync::COL_USER_ID));
		}



		template<>
		void OldLoadSavePolicy<DBLogEntryTableSync, DBLogEntry>::Unlink(
			DBLogEntry* obj
		){
		}



		template<>
		void OldLoadSavePolicy<DBLogEntryTableSync,DBLogEntry>::Save(
			DBLogEntry* object,
			optional<DBTransaction&> transaction
		){
			// Preparation
			stringstream content;
			DBLogEntry::Content c = object->getContent();
			for (DBLogEntry::Content::const_iterator it = c.begin(); it != c.end(); ++it)
			{
				if (it != c.begin())
					content << DBLogEntryTableSync::CONTENT_SEPARATOR;
				content << *it;
			}

			// Query
			ReplaceQuery<DBLogEntryTableSync> query(*object);
			query.addField(object->getLogKey());
			query.addFrameworkField<PtimeField>(object->getDate());
			query.addField(object->getUserId());
			query.addField(static_cast<int>(object->getLevel()));
			query.addField(content.str());
			query.addField(object->getObjectId());
			query.addField(object->getObjectId2());
			query.execute(transaction);
		}

		template<> bool DBTableSyncTemplate<DBLogEntryTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			Env env;
			boost::shared_ptr<DBLog> dbLog;
			try
			{
				boost::shared_ptr<const DBLogEntry> entry(DBLogEntryTableSync::Get(object_id, env));
				dbLog.reset(Factory<DBLog>::create(entry->getLogKey()));
			}
			catch(ObjectNotFoundException<DBLogEntry>&)
			{
				return false;
			}
			catch(FactoryException<DBLog>&)
			{
				return false;
			}

			return
				session &&
				session->hasProfile() &&
				session->getUser()->getProfile()->isAuthorized<DBLogRight>(DELETE_RIGHT) &&
				dbLog->isAuthorized(*session->getUser()->getProfile(), DELETE_RIGHT)
			;
		}



		template<> void DBTableSyncTemplate<DBLogEntryTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<DBLogEntryTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<DBLogEntryTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
		}
	}

	namespace dblog
	{
		DBLogEntryTableSync::SearchResult DBLogEntryTableSync::Search(
			Env& env,
			const std::string& logKey
			, const ptime& startDate
			, const ptime& endDate
			, optional<RegistryKeyType> userId
			, DBLogEntry::Level level
			, optional<RegistryKeyType> objectId
			, optional<RegistryKeyType> objectId2
			, const std::string& text
			, int first
			, boost::optional<std::size_t> number
			, bool orderByDate
			, bool orderByUser
			, bool orderByLevel
			, bool raisingOrder,
			LinkLevel linkLevel
		){
			SelectQuery<DBLogEntryTableSync> query;
			query.addWhereField(COL_LOG_KEY, logKey);
			if (!startDate.is_not_a_date_time())
			{
				query.addWhereField(COL_DATE, startDate, ComposedExpression::OP_SUPEQ);
			}
			if (!endDate.is_not_a_date_time())
			{
				query.addWhereField(COL_DATE, endDate, ComposedExpression::OP_INFEQ);
			}
			if(userId)
			{
				query.addWhereField(COL_USER_ID, *userId);
			}
			if (level != DBLogEntry::DB_LOG_UNKNOWN)
			{
				query.addWhereField(COL_LEVEL, static_cast<int>(level));
			}
			if (!text.empty())
			{
				query.addWhereField(COL_CONTENT, "%" + text + "%", ComposedExpression::OP_LIKE);
			}
			if(objectId)
			{
				query.addWhereField(COL_OBJECT_ID, *objectId);
			}
			if(objectId2)
			{
				query.addWhereField(COL_OBJECT2_ID, *objectId2);
			}
			if (orderByDate)
			{
				query.addOrderField(COL_DATE, raisingOrder);
			}
			else if (orderByUser)
			{
				query.addOrderField(COL_USER_ID, raisingOrder);
				query.addOrderField(COL_DATE, raisingOrder);
			}
			else if (orderByLevel)
			{
				query.addOrderField(COL_LEVEL, raisingOrder);
				query.addOrderField(COL_DATE, raisingOrder);
			}
			if (number)
			{
				query.setNumber(*number + 1);
				query.setFirst(first);
			}

			return LoadFromQuery(query, env, linkLevel);
		}



		void DBLogEntryTableSync::Purge( const std::string& logKey, const ptime& endDate )
		{
			DeleteQuery<DBLogEntryTableSync> query;
			query.addWhereField(COL_DATE, endDate, ComposedExpression::OP_INFEQ);
			query.addWhereField(COL_LOG_KEY, logKey);
			query.execute();
		}
	}
}
