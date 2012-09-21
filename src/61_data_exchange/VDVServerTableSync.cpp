
/** VDVServerTableSync class implementation.
	@file VDVServerTableSync.cpp

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

#include "VDVServerTableSync.hpp"

#include "DBResult.hpp"
#include "ReplaceQuery.h"
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
		template<> const string FactorableTemplate<DBTableSync,VDVServerTableSync>::FACTORY_KEY("36.01 VDVServers");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<VDVServerTableSync>::TABLE(
			"t097_vdv_servers"
		);

		template<> const Field DBTableSyncTemplate<VDVServerTableSync>::_FIELDS[] = { Field() }; // Defined by the record

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<VDVServerTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		template<> void DBDirectTableSyncTemplate<VDVServerTableSync,VDVServer>::Load(
			VDVServer* website,
			const DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			if(linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				DBModule::LoadObjects(website->getLinkedObjectsIds(*rows), env, linkLevel);
			}
			website->loadFromRecord(*rows, env);
			if(linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				website->link(env, linkLevel == ALGORITHMS_OPTIMIZATION_LOAD_LEVEL);
			}
		}



		template<> void DBDirectTableSyncTemplate<VDVServerTableSync,VDVServer>::Unlink(
			VDVServer* obj
		){
			obj->unlink();
		}



		template<> void DBDirectTableSyncTemplate<VDVServerTableSync,VDVServer>::Save(
			VDVServer* website,
			optional<DBTransaction&> transaction
		){
			// Query
			ReplaceQuery<VDVServerTableSync> query(*website);
			ParametersMap map(ParametersMap::FORMAT_SQL);
			website->toParametersMap(map);
			query.setValues(map);
			query.execute(transaction);
		}



		template<> bool DBTableSyncTemplate<VDVServerTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO Check user rights
			return true;
		}



		template<> void DBTableSyncTemplate<VDVServerTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<VDVServerTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<VDVServerTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace data_exchange
	{

		VDVServerTableSync::SearchResult VDVServerTableSync::Search(
			Env& env,
			std::string name
			, int first /*= 0*/,
			boost::optional<std::size_t> number
			, bool orderByName
			, bool raisingOrder,
			LinkLevel linkLevel
		){
			SelectQuery<VDVServerTableSync> query;
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
