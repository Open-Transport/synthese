
//////////////////////////////////////////////////////////////////////////
///	ForumTopicTableSync class implementation.
///	@file ForumTopicTableSync.cpp
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

#include "ForumTopicTableSync.hpp"

#include "PtimeField.hpp"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
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
		template<> const string FactorableTemplate<DBTableSync,ForumTopicTableSync>::FACTORY_KEY("52.10 Topics");
	}

	namespace forum
	{
		const string ForumTopicTableSync::COL_NAME("name");
		const string ForumTopicTableSync::COL_NODE_ID("node_id");
		const string ForumTopicTableSync::COL_NODE_EXTENSION("node_extension");
		const string ForumTopicTableSync::COL_USER_EMAIL("user_email");
		const string ForumTopicTableSync::COL_USER_NAME("user_name");
		const string ForumTopicTableSync::COL_USER_ID("user_id");
		const string ForumTopicTableSync::COL_DATE("date");
		const string ForumTopicTableSync::COL_PUBLISHED("published");
		const string ForumTopicTableSync::COL_CLOSED("closed");
		const string ForumTopicTableSync::COL_IP("ip");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<ForumTopicTableSync>::TABLE(
			"t067_forum_topics"
		);



		template<> const Field DBTableSyncTemplate<ForumTopicTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(ForumTopicTableSync::COL_NAME, SQL_TEXT),
			Field(ForumTopicTableSync::COL_NODE_ID, SQL_INTEGER),
			Field(ForumTopicTableSync::COL_NODE_EXTENSION, SQL_TEXT),
			Field(ForumTopicTableSync::COL_USER_EMAIL, SQL_TEXT),
			Field(ForumTopicTableSync::COL_USER_NAME, SQL_TEXT),
			Field(ForumTopicTableSync::COL_USER_ID, SQL_INTEGER),
			Field(ForumTopicTableSync::COL_DATE, SQL_DATETIME),
			Field(ForumTopicTableSync::COL_PUBLISHED, SQL_BOOLEAN),
			Field(ForumTopicTableSync::COL_CLOSED, SQL_BOOLEAN),
			Field(ForumTopicTableSync::COL_IP, SQL_TEXT),
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<ForumTopicTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(
					ForumTopicTableSync::COL_NODE_ID.c_str(),
					ForumTopicTableSync::COL_NODE_EXTENSION.c_str(),
					ForumTopicTableSync::COL_DATE.c_str(),
			"")	);
			r.push_back(
				DBTableSync::Index(
					ForumTopicTableSync::COL_USER_ID.c_str(),
					ForumTopicTableSync::COL_DATE.c_str(),
			"")	);
			return r;
		}



		template<>
		void OldLoadSavePolicy<ForumTopicTableSync,ForumTopic>::Load(
			ForumTopic* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			object->setName(rows->getText(ForumTopicTableSync::COL_NAME));
			object->setNode(rows->getLongLong(ForumTopicTableSync::COL_NODE_ID));
			object->setNodeExtension(rows->getText(ForumTopicTableSync::COL_NODE_EXTENSION));
			object->setUserEMail(rows->getText(ForumTopicTableSync::COL_USER_EMAIL));
			object->setUserName(rows->getText(ForumTopicTableSync::COL_USER_NAME));

			if(linkLevel >= UP_LINKS_LOAD_LEVEL)
			{
				RegistryKeyType userId(rows->getLongLong(ForumTopicTableSync::COL_USER_ID));
				if(userId > 0)
				{
					try
					{
						object->setUser(UserTableSync::GetEditable(userId, env, linkLevel).get());
					}
					catch(ObjectNotFoundException<User>&)
					{
						Log::GetInstance().warn("No such user "+ lexical_cast<string>(userId) +" in ForumTopic "+ lexical_cast<string>(object->getKey()));
					}
				}
			}

			object->setDate(rows->getDateTime(ForumTopicTableSync::COL_DATE));
			object->setPublished(rows->getBool(ForumTopicTableSync::COL_PUBLISHED));
			object->setClosed(rows->getBool(ForumTopicTableSync::COL_CLOSED));
			object->setIP(rows->getText(ForumTopicTableSync::COL_IP));
		}



		template<>
		void OldLoadSavePolicy<ForumTopicTableSync,ForumTopic>::Save(
			ForumTopic* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<ForumTopicTableSync> query(*object);
			query.addField(object->getName());
			query.addField(object->getNode());
			query.addField(object->getNodeExtension());
			query.addField(object->getUserEMail());
			query.addField(object->getUserName());
			query.addField(object->getUser() ? object->getUser()->getKey() : RegistryKeyType(0));
			query.addFrameworkField<PtimeField>(object->getDate());
			query.addField(object->getPublished());
			query.addField(object->getClosed());
			query.addField(object->getIP());
			query.execute(transaction);
		}



		template<>
		void OldLoadSavePolicy<ForumTopicTableSync,ForumTopic>::Unlink(
			ForumTopic* obj
		){
		}



		template<> bool DBTableSyncTemplate<ForumTopicTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO test if the user has sufficient right level
			return true;
		}



		template<> void DBTableSyncTemplate<ForumTopicTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
			//TODO delete all messages of the topic
		}



		template<> void DBTableSyncTemplate<ForumTopicTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<ForumTopicTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}



	namespace forum
	{
		ForumTopicTableSync::SearchResult ForumTopicTableSync::Search(
			util::Env& env,
			boost::optional<util::RegistryKeyType> nodeId,
			boost::optional<std::string> nodeExtension,
			boost::optional<std::string> userName,
			boost::optional<util::RegistryKeyType> userId,
			boost::logic::tribool published,
			size_t first /*= 0*/,
			optional<size_t> number /*= boost::optional<std::size_t>()*/,
			bool orderByDate,
			bool raisingOrder,
			util::LinkLevel linkLevel
		){
			SelectQuery<ForumTopicTableSync> query;
			if(nodeId)
			{
			 	query.addWhereField(COL_NODE_ID, *nodeId);
			}
			if(nodeExtension)
			{
				query.addWhereField(COL_NODE_EXTENSION, *nodeExtension);
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
