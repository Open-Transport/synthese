
/** TimetableRowGroupTableSync class implementation.
	@file TimetableRowGroupTableSync.cpp

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

#include "TimetableRowGroupTableSync.hpp"

#include "DBResult.hpp"
#include "RankUpdateQuery.hpp"
#include "SelectQuery.hpp"
#include "SQLSingleOperatorExpression.hpp"
#include "StopArea.hpp"

#include <boost/foreach.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace db;
	using namespace timetables;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,TimetableRowGroupTableSync>::FACTORY_KEY("55.10 Timetable row groups");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<TimetableRowGroupTableSync>::TABLE(
			"t090_timetable_rowgroups"
		);

		template<> const Field DBTableSyncTemplate<TimetableRowGroupTableSync>::_FIELDS[] = { Field() }; // Defined by the record

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<TimetableRowGroupTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(DBTableSync::Index(SimpleObjectFieldDefinition<Timetable>::FIELD.name.c_str(), ""));
			return r;
		}



		template<> bool DBTableSyncTemplate<TimetableRowGroupTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			Env env;
			boost::shared_ptr<const TimetableRowGroupTableSync::ObjectType> object(TimetableRowGroupTableSync::Get(object_id, env));
			return object->allowDelete(session);
		}



		template<> void DBTableSyncTemplate<TimetableRowGroupTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
			Env env;
			boost::shared_ptr<const TimetableRowGroupTableSync::ObjectType> object(TimetableRowGroupTableSync::Get(id, env));
			object->beforeDelete(transaction);
		}



		template<> void DBTableSyncTemplate<TimetableRowGroupTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
			Env env;
			boost::shared_ptr<const TimetableRowGroupTableSync::ObjectType> object(TimetableRowGroupTableSync::Get(id, env));
			object->afterDelete(transaction);
		}



		template<> void DBTableSyncTemplate<TimetableRowGroupTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace timetables
	{
		TimetableRowGroupTableSync::SearchResult TimetableRowGroupTableSync::Search(
			util::Env& env,
			boost::optional<util::RegistryKeyType> timetableId,
			boost::optional<size_t> rank,
			int first,
			boost::optional<std::size_t> number,
			bool orderByRank,
			bool raisingOrder,
			util::LinkLevel linkLevel
		){
			SelectQuery<TimetableRowGroupTableSync> query;
			if (timetableId)
			{
				query.addWhereField(SimpleObjectFieldDefinition<Timetable>::FIELD.name, *timetableId);
			}
			if(rank)
			{
				query.addWhereField(SimpleObjectFieldDefinition<Rank>::FIELD.name, *rank);
			}
			if(orderByRank)
			{
				query.addOrderField(SimpleObjectFieldDefinition<Rank>::FIELD.name, raisingOrder);
			}
			if (number)
			{
				query.setNumber(*number + 1);
				if (first > 0)
					query.setFirst(first);
			}

			return LoadFromQuery(query, env, linkLevel);
		}
}	}
