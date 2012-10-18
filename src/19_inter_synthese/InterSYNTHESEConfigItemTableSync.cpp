
/** InterSYNTHESEConfigItemTableSync class implementation.
	@file InterSYNTHESEConfigItemTableSync.cpp

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

#include "InterSYNTHESEConfigItemTableSync.hpp"

#include "DBResult.hpp"
#include "SelectQuery.hpp"
#include "Webpage.h"

#include <boost/foreach.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace db;
	using namespace inter_synthese;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,InterSYNTHESEConfigItemTableSync>::FACTORY_KEY("19.01 InterSYNTHESEConfigItems");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<InterSYNTHESEConfigItemTableSync>::TABLE(
			"t095_inter_synthese_config_items"
		);

		template<> const Field DBTableSyncTemplate<InterSYNTHESEConfigItemTableSync>::_FIELDS[] = { Field() }; // Defined by the record

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<InterSYNTHESEConfigItemTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		template<> void DBDirectTableSyncTemplate<InterSYNTHESEConfigItemTableSync,InterSYNTHESEConfigItem>::Load(
			InterSYNTHESEConfigItem* website,
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



		template<> void DBDirectTableSyncTemplate<InterSYNTHESEConfigItemTableSync,InterSYNTHESEConfigItem>::Unlink(
			InterSYNTHESEConfigItem* obj
		){
			obj->unlink();
		}



		template<> void DBDirectTableSyncTemplate<InterSYNTHESEConfigItemTableSync,InterSYNTHESEConfigItem>::Save(
			InterSYNTHESEConfigItem* object,
			optional<DBTransaction&> transaction
		){
			DBModule::GetDB()->replaceStmt(*object, transaction);
		}



		template<> bool DBTableSyncTemplate<InterSYNTHESEConfigItemTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO Check user rights
			return true;
		}



		template<> void DBTableSyncTemplate<InterSYNTHESEConfigItemTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<InterSYNTHESEConfigItemTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<InterSYNTHESEConfigItemTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace inter_synthese
	{

		InterSYNTHESEConfigItemTableSync::SearchResult InterSYNTHESEConfigItemTableSync::Search(
			Env& env,
			std::string name
			, int first /*= 0*/,
			boost::optional<std::size_t> number
			, bool orderByName
			, bool raisingOrder,
			LinkLevel linkLevel
		){
			SelectQuery<InterSYNTHESEConfigItemTableSync> query;
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
