
/** WebsiteTableSync class implementation.
	@file WebsiteTableSync.cpp

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

#include "WebsiteTableSync.hpp"

#include "DBResult.hpp"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "Webpage.h"

#include <boost/foreach.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace db;
	using namespace cms;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,WebsiteTableSync>::FACTORY_KEY("36.01 Websites");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<WebsiteTableSync>::TABLE(
			"t025_sites"
		);

		template<> const Field DBTableSyncTemplate<WebsiteTableSync>::_FIELDS[] = { Field() }; // Defined by the record

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<WebsiteTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		template<> void DBDirectTableSyncTemplate<WebsiteTableSync,Website>::Load(
			Website* website,
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



		template<> void DBDirectTableSyncTemplate<WebsiteTableSync,Website>::Unlink(
			Website* obj
		){
			obj->unlink();
		}



		template<> void DBDirectTableSyncTemplate<WebsiteTableSync,Website>::Save(
			Website* website,
			optional<DBTransaction&> transaction
		){
			DBModule::GetDB()->replaceStmt(*website, transaction);
		}



		template<> bool DBTableSyncTemplate<WebsiteTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO Check user rights
			return true;
		}



		template<> void DBTableSyncTemplate<WebsiteTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<WebsiteTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<WebsiteTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace cms
	{

		WebsiteTableSync::SearchResult WebsiteTableSync::Search(
			Env& env,
			std::string name
			, int first /*= 0*/,
			boost::optional<std::size_t> number
			, bool orderByName
			, bool raisingOrder,
			LinkLevel linkLevel
		){
			SelectQuery<WebsiteTableSync> query;
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
