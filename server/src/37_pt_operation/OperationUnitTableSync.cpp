
/** OperationUnitTableSync class implementation.
	@file OperationUnitTableSync.cpp

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

#include "OperationUnitTableSync.hpp"

#include "CommercialLine.h"
#include "DBResult.hpp"
#include "SelectQuery.hpp"

#include <boost/foreach.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace db;
	using namespace pt_operation;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,OperationUnitTableSync>::FACTORY_KEY("37.10 Operation units");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<OperationUnitTableSync>::TABLE(
			"t116_operation_units"
		);

		template<> const Field DBTableSyncTemplate<OperationUnitTableSync>::_FIELDS[] = { Field() }; // Defined by the record

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<OperationUnitTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			return r;
		}



		template<> bool DBTableSyncTemplate<OperationUnitTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			Env env;
			boost::shared_ptr<const OperationUnitTableSync::ObjectType> object(OperationUnitTableSync::Get(object_id, env));
			return object->allowDelete(session);
		}



		template<> void DBTableSyncTemplate<OperationUnitTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
			Env env;
			boost::shared_ptr<const OperationUnitTableSync::ObjectType> object(OperationUnitTableSync::Get(id, env));
			object->beforeDelete(transaction);
		}



		template<> void DBTableSyncTemplate<OperationUnitTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
			Env env;
			boost::shared_ptr<const OperationUnitTableSync::ObjectType> object(OperationUnitTableSync::Get(id, env));
			object->afterDelete(transaction);
		}



		template<> void DBTableSyncTemplate<OperationUnitTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace pt_operation
	{
		OperationUnitTableSync::SearchResult OperationUnitTableSync::Search(
			util::Env& env,
			int first,
			boost::optional<std::size_t> number,
			bool raisingOrder,
			util::LinkLevel linkLevel
		){
			SelectQuery<OperationUnitTableSync> query;
			if (number)
			{
				query.setNumber(*number + 1);
				if (first > 0)
					query.setFirst(first);
			}

			return LoadFromQuery(query, env, linkLevel);
		}
}	}
