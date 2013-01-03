
/** VDVClientTableSync class implementation.
	@file VDVClientTableSync.cpp

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

#include "VDVClientTableSync.hpp"

#include "DBResult.hpp"
#include "SelectQuery.hpp"

#include <boost/foreach.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace db;
	using namespace data_exchange;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,VDVClientTableSync>::FACTORY_KEY("36.01 VDVClients");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<VDVClientTableSync>::TABLE(
			"t099_vdv_clients"
		);

		template<> const Field DBTableSyncTemplate<VDVClientTableSync>::_FIELDS[] = { Field() }; // Defined by the record

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<VDVClientTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		template<> bool DBTableSyncTemplate<VDVClientTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO Check user rights
			return true;
		}



		template<> void DBTableSyncTemplate<VDVClientTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<VDVClientTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<VDVClientTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace data_exchange
	{

		VDVClientTableSync::SearchResult VDVClientTableSync::Search(
			Env& env,
			std::string name
			, int first /*= 0*/,
			boost::optional<std::size_t> number
			, bool orderByName
			, bool raisingOrder,
			LinkLevel linkLevel
		){
			SelectQuery<VDVClientTableSync> query;
			if (!name.empty())
			{
				query.addWhereField(SimpleObjectFieldDefinition<Name>::FIELD.name, name, ComposedExpression::OP_LIKE);
			}
			if (orderByName)
			{
				query.addOrderField(SimpleObjectFieldDefinition<Name>::FIELD.name, raisingOrder);
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
}	}
