
/** TransportNetworkTableSync class implementation.
	@file TransportNetworkTableSync.cpp

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

#include "TransportNetworkTableSync.h"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"

#include <boost/logic/tribool.hpp>
#include <assert.h>

using boost::logic::tribool;
using namespace boost;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace pt;

	template<> const string util::FactorableTemplate<SQLiteTableSync,TransportNetworkTableSync>::FACTORY_KEY(
		"35.20.02 Network transport"
	);

	namespace pt
	{
		const string TransportNetworkTableSync::COL_NAME("name");
		const string TransportNetworkTableSync::COL_CREATOR_ID("creator_id");
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<TransportNetworkTableSync>::TABLE(
			"t022_transport_networks"
		);

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<TransportNetworkTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(TransportNetworkTableSync::COL_NAME, SQL_TEXT),
			SQLiteTableSync::Field(TransportNetworkTableSync::COL_CREATOR_ID, SQL_TEXT),
			SQLiteTableSync::Field()
		};

		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<TransportNetworkTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index(TransportNetworkTableSync::COL_CREATOR_ID.c_str(), ""),
			SQLiteTableSync::Index(TransportNetworkTableSync::COL_NAME.c_str(), ""),
			SQLiteTableSync::Index()
		};

		template<> void SQLiteDirectTableSyncTemplate<TransportNetworkTableSync,TransportNetwork>::Load(
			TransportNetwork* object,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			std::string name (rows->getText (TransportNetworkTableSync::COL_NAME));
			std::string creatorId(rows->getText (TransportNetworkTableSync::COL_CREATOR_ID));

			object->setName(name);
			object->setCreatorId(creatorId);
		}

		template<> void SQLiteDirectTableSyncTemplate<TransportNetworkTableSync,TransportNetwork>::Save(
			TransportNetwork* object,
			optional<SQLiteTransaction&> transaction
		){
			ReplaceQuery<TransportNetworkTableSync> query(*object);
			query.addField(object->getName());
			query.addField(object->getCreatorId());
			query.execute(transaction);
		}



		template<> void SQLiteDirectTableSyncTemplate<TransportNetworkTableSync,TransportNetwork>::Unlink(
			TransportNetwork* object
		){
		}
	}

	namespace pt
	{
	    TransportNetworkTableSync::SearchResult TransportNetworkTableSync::Search(
			Env& env,
			string name,
			string creatorId,
			int first, /*= 0*/
			boost::optional<std::size_t> number,
			bool orderByName,
			bool raisingOrder,
			LinkLevel linkLevel
		){
			SelectQuery<TransportNetworkTableSync> query;
			if (!name.empty())
			{
				query.addWhereField(COL_NAME, name, ComposedExpression::OP_LIKE);
			}
			if (!creatorId.empty())
			{
				query.addWhereField(COL_CREATOR_ID, creatorId, ComposedExpression::OP_LIKE);
			}
			if (orderByName)
			{
				query.addOrderField(COL_NAME, raisingOrder);
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
