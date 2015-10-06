
//////////////////////////////////////////////////////////////////////////
///	ForumMessageTableSync class implementation.
///	@file ForumMessageTableSync.cpp
///	@author Hugues Romain
///	@date 2010
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "ForumMessageTableSync.hpp"

#include "PtimeField.hpp"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "ForumTopicTableSync.hpp"
#include "UserTableSync.h"

#include <sstream>

using namespace std;
using namespace boost;
using namespace boost::logic;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace forum;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,ForumMessageTableSync>::FACTORY_KEY("52.20 Messages");
	}

	namespace forum
	{
		const string ForumMessageTableSync::COL_TOPIC_ID("topic_id");
		const string ForumMessageTableSync::COL_CONTENT("content");
		const string ForumMessageTableSync::COL_USER_EMAIL("user_email");
		const string ForumMessageTableSync::COL_USER_NAME("user_name");
		const string ForumMessageTableSync::COL_USER_ID("user_id");
		const string ForumMessageTableSync::COL_DATE("date");
		const string ForumMessageTableSync::COL_PUBLISHED("published");
		const string ForumMessageTableSync::COL_IP("ip");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<ForumMessageTableSync>::TABLE(
			"t068_forum_messages"
		);



		template<> const Field DBTableSyncTemplate<ForumMessageTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(ForumMessageTableSync::COL_TOPIC_ID, SQL_INTEGER),
			Field(ForumMessageTableSync::COL_CONTENT, SQL_TEXT),
			Field(ForumMessageTableSync::COL_USER_EMAIL, SQL_TEXT),
			Field(ForumMessageTableSync::COL_USER_NAME, SQL_TEXT),
			Field(ForumMessageTableSync::COL_USER_ID, SQL_INTEGER),
			Field(ForumMessageTableSync::COL_DATE, SQL_DATETIME),
			Field(ForumMessageTableSync::COL_PUBLISHED, SQL_BOOLEAN),
			Field(ForumMessageTableSync::COL_IP, SQL_TEXT),
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<ForumMessageTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(
					ForumMessageTableSync::COL_TOPIC_ID.c_str(),
					ForumMessageTableSync::COL_DATE.c_str(),
			"")	);
			r.push_back(
				DBTableSync::Index(
					ForumMessageTableSync::COL_USER_ID.c_str(),
					ForumMessageTableSync::COL_DATE.c_str(),
			"")	);
			return r;
		}



		template<>
		void OldLoadSavePolicy<ForumMessageTableSync,ForumMessage>::Load(
			ForumMessage* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			if(linkLevel >= UP_LINKS_LOAD_LEVEL)
			{
				RegistryKeyType id(rows->getLongLong(ForumMessageTableSync::COL_TOPIC_ID));
				if(id > 0)
				{
					try
					{
						object->setTopic(ForumTopicTableSync::GetEditable(id, env, linkLevel).get());
					}
					catch(ObjectNotFoundException<ForumTopic>&)
					{
						Log::GetInstance().warn("No such topic "+ lexical_cast<string>(id) +" in ForumMessage "+ lexical_cast<string>(object->getKey()));
					}
				}
			}

			object->setContent(rows->getText(ForumMessageTableSync::COL_CONTENT));
			object->setUserEMail(rows->getText(ForumMessageTableSync::COL_USER_EMAIL));
			object->setUserName(rows->getText(ForumMessageTableSync::COL_USER_NAME));

			if(linkLevel >= UP_LINKS_LOAD_LEVEL)
			{
				RegistryKeyType userId(rows->getLongLong(ForumMessageTableSync::COL_USER_ID));
				if(userId > 0)
				{
					try
					{
						object->setUser(UserTableSync::GetEditable(userId, env, linkLevel).get());
					}
					catch(ObjectNotFoundException<User>&)
					{
						Log::GetInstance().warn("No such user "+ lexical_cast<string>(userId) +" in ForumMessage "+ lexical_cast<string>(object->getKey()));
					}
				}
			}

			object->setDate(rows->getDateTime(ForumMessageTableSync::COL_DATE));
			object->setPublished(rows->getBool(ForumMessageTableSync::COL_PUBLISHED));
			object->setIP(rows->getText(ForumMessageTableSync::COL_IP));
		}



		template<>
		void OldLoadSavePolicy<ForumMessageTableSync,ForumMessage>::Save(
			ForumMessage* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<ForumMessageTableSync> query(*object);
			query.addField(object->getTopic() ? object->getTopic()->getKey() : RegistryKeyType(0));
			query.addField(object->getContent());
			query.addField(object->getUserEMail());
			query.addField(object->getUserName());
			query.addField(object->getUser() ? object->getUser()->getKey() : RegistryKeyType(0));
			query.addFrameworkField<PtimeField>(object->getDate());
			query.addField(object->getPublished());
			query.addField(object->getIP());
			query.execute(transaction);
		}



		template<>
		void OldLoadSavePolicy<ForumMessageTableSync,ForumMessage>::Unlink(
			ForumMessage* obj
		){
		}



		template<> bool DBTableSyncTemplate<ForumMessageTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO test if the user has sufficient right level
			return true;
		}



		template<> void DBTableSyncTemplate<ForumMessageTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<ForumMessageTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<ForumMessageTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}



	namespace forum
	{
		ForumMessageTableSync::SearchResult ForumMessageTableSync::Search(
			util::Env& env,
			boost::optional<util::RegistryKeyType> topicId,
			boost::optional<std::string> userName,
			boost::optional<util::RegistryKeyType> userId,
			boost::logic::tribool published,
			size_t first /*= 0*/,
			optional<size_t> number /*= boost::optional<std::size_t>()*/,
			bool orderByDate,
			bool raisingOrder,
			util::LinkLevel linkLevel
		){
			SelectQuery<ForumMessageTableSync> query;
			if(topicId)
			{
			 	query.addWhereField(COL_TOPIC_ID, *topicId);
			}
			if(userName)
			{
				query.addWhereField(COL_USER_NAME, *userName, ComposedExpression::OP_LIKE);
			}
			if(userId)
			{
				query.addWhereField(COL_USER_ID, *userId);
			}
			if(!indeterminate(published))
			{
				query.addWhereField(COL_PUBLISHED, published);
			}
			if(orderByDate)
			{
				query.addOrderField(COL_DATE, raisingOrder);
			}
			if (number)
			{
				query.setNumber(*number + 1);
			}
			if (first > 0)
			{
				query.setFirst(first);
			}

			return LoadFromQuery(query, env, linkLevel);
		}
	}
}
