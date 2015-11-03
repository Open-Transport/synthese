
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

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<DBLogEntryTableSync>::TABLE(
			"t045_log_entries", true
		);

		template<> const Field DBTableSyncTemplate<DBLogEntryTableSync>::_FIELDS[] =
		{
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<DBLogEntryTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(
					LogKey::FIELD.name.c_str(),
					ObjectId::FIELD.name.c_str(),
					LogDate::FIELD.name.c_str(),
					""
			)	);
			r.push_back(
				DBTableSync::Index(
					LogKey::FIELD.name.c_str(),
					Object2Id::FIELD.name.c_str(),
					LogDate::FIELD.name.c_str(),
					""
			)	);
			r.push_back(
				DBTableSync::Index(
					LogKey::FIELD.name.c_str(),
					LogDate::FIELD.name.c_str(),
					""
			)	);
			return r;
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
			, Level level
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
			query.addWhereField(LogKey::FIELD.name, logKey);
			if (!startDate.is_not_a_date_time())
			{
				query.addWhereField(LogDate::FIELD.name, startDate, ComposedExpression::OP_SUPEQ);
			}
			if (!endDate.is_not_a_date_time())
			{
				query.addWhereField(LogDate::FIELD.name, endDate, ComposedExpression::OP_INFEQ);
			}
			if(userId)
			{
				query.addWhereField(LogUser::FIELD.name, *userId);
			}
			if (level != DB_LOG_UNKNOWN)
			{
				query.addWhereField(LogLevel::FIELD.name, static_cast<int>(level));
			}
			if (!text.empty())
			{
				query.addWhereField(LogContent::FIELD.name, "%" + text + "%", ComposedExpression::OP_LIKE);
			}
			if(objectId)
			{
				query.addWhereField(ObjectId::FIELD.name, *objectId);
			}
			if(objectId2)
			{
				query.addWhereField(Object2Id::FIELD.name, *objectId2);
			}
			if (orderByDate)
			{
				query.addOrderField(LogDate::FIELD.name, raisingOrder);
			}
			else if (orderByUser)
			{
				query.addOrderField(LogUser::FIELD.name, raisingOrder);
				query.addOrderField(LogDate::FIELD.name, raisingOrder);
			}
			else if (orderByLevel)
			{
				query.addOrderField(LogLevel::FIELD.name, raisingOrder);
				query.addOrderField(LogLevel::FIELD.name, raisingOrder);
			}
			if (number)
			{
				query.setNumber(*number + 1);
				query.setFirst(first);
			}

			return LoadFromQuery(query, env, linkLevel);
		}



		DBLogEntryTableSync::SearchResult DBLogEntryTableSync::SearchByUser(
			Env& env,
			util::RegistryKeyType objectId,
			LinkLevel linkLevel
		){
			SelectQuery<DBLogEntryTableSync> query;

			if (objectId)
				query.addWhereField(ObjectId::FIELD.name, objectId);
			return LoadFromQuery(query, env, linkLevel);
		}



		void DBLogEntryTableSync::Purge( const std::string& logKey, const ptime& endDate )
		{
			DeleteQuery<DBLogEntryTableSync> query;
			query.addWhereField(LogDate::FIELD.name, endDate, ComposedExpression::OP_INFEQ);
			query.addWhereField(LogKey::FIELD.name, logKey);
			query.execute();
		}

		bool DBLogEntryTableSync::allowList(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::READ);
		}
	}
}
