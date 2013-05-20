
/** MessageAlternativeTableSync class implementation.
	@file MessageAlternativeTableSync.cpp

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

#include "MessageAlternativeTableSync.hpp"

#include "AlarmTableSync.h"
#include "DBResult.hpp"
#include "SelectQuery.hpp"

#include <boost/foreach.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace db;
	using namespace messages;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,MessageAlternativeTableSync>::FACTORY_KEY("17.40 Message Alternatives");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<MessageAlternativeTableSync>::TABLE(
			"t102_message_alternatives"
		);

		template<> const Field DBTableSyncTemplate<MessageAlternativeTableSync>::_FIELDS[] = { Field() }; // Defined by the record

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<MessageAlternativeTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		template<> bool DBTableSyncTemplate<MessageAlternativeTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO Check user rights
			return true;
		}



		template<> void DBTableSyncTemplate<MessageAlternativeTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<MessageAlternativeTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<MessageAlternativeTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace messages
	{

		MessageAlternativeTableSync::SearchResult MessageAlternativeTableSync::Search(
			Env& env,
			optional<RegistryKeyType> alarmId,
			optional<RegistryKeyType> typeId,
			int first /*= 0*/,
			optional<size_t> number,
			bool raisingOrder,
			LinkLevel linkLevel
		){
			SelectQuery<MessageAlternativeTableSync> query;
			if (alarmId)
			{
				query.addWhereField(SimpleObjectFieldDefinition<Alarm>::FIELD.name, *alarmId, ComposedExpression::OP_EQ);
			}
			if (typeId)
			{
				query.addWhereField(SimpleObjectFieldDefinition<MessageType>::FIELD.name, *typeId, ComposedExpression::OP_EQ);
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



		void MessageAlternativeTableSync::CopyAlternatives(
			RegistryKeyType sourceId,
			Alarm& dest,
			optional<DBTransaction&> transaction
		){
			Env lenv;
			SearchResult alternatives(
				Search(lenv, sourceId)
			);
			BOOST_FOREACH(const boost::shared_ptr<MessageAlternative>& alternative, alternatives)
			{
				MessageAlternative ma;
				ma.set<Alarm>(dest);
				ma.set<MessageType>(alternative->get<MessageType>());
				ma.set<Content>(alternative->get<Content>());
				Save(&ma, transaction);
			}
		}
}	}
