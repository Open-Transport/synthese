
/** DriverActivityTableSync class implementation.
	@file DriverActivityTableSync.cpp

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

#include "DriverActivityTableSync.hpp"

#include "ReplaceQuery.h"
#include "SelectQuery.hpp"

#include <sstream>
#include <boost/tokenizer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost;
using namespace std;
using namespace boost::gregorian;
using namespace boost::posix_time;

namespace synthese
{
	using namespace db;
	using namespace pt;
	using namespace pt_operation;
	using namespace security;
	using namespace util;


	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,DriverActivityTableSync>::FACTORY_KEY("37.00 Driver activities");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<DriverActivityTableSync>::TABLE(
			"t087_driver_activites"
		);

		template<> const Field DBTableSyncTemplate<DriverActivityTableSync>::_FIELDS[] = { Field() }; // Defined by the record

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<DriverActivityTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		};


		template<> void DBDirectTableSyncTemplate<DriverActivityTableSync,DriverActivity>::Load(
			DriverActivity* object,
			const DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			if(linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				DBModule::LoadObjects(object->getLinkedObjectsIds(*rows), env, linkLevel);
			}
			object->loadFromRecord(*rows, env);
			if(linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				object->link(env, linkLevel == ALGORITHMS_OPTIMIZATION_LOAD_LEVEL);
			}
		}



		template<> void DBDirectTableSyncTemplate<DriverActivityTableSync,DriverActivity>::Unlink(
			DriverActivity* obj
		){
			obj->unlink();
		}


		template<> void DBDirectTableSyncTemplate<DriverActivityTableSync,DriverActivity>::Save(
			DriverActivity* object,
			optional<DBTransaction&> transaction
		){
			DBModule::GetDB()->replaceStmt(*object, transaction);
		}



		template<> bool DBTableSyncTemplate<DriverActivityTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return true; // TODO
		}



		template<> void DBTableSyncTemplate<DriverActivityTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<DriverActivityTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<DriverActivityTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO log the removal
		}
	}

	namespace pt_operation
	{
		DriverActivityTableSync::SearchResult DriverActivityTableSync::Search(
			util::Env& env,
			size_t first /*= 0*/,
			boost::optional<std::size_t> number /*= boost::optional<std::size_t>()*/,
			bool orderByTime /*= true*/,
			bool raisingOrder /*= true*/,
			util::LinkLevel linkLevel /*= util::UP_LINKS_LOAD_LEVEL */
		){
			SelectQuery<DriverActivityTableSync> query;
			if (orderByTime)
			{
//				query.addOrderField(SimpleObjectFieldDefinition<Name>::FIELDS[0].name, raisingOrder);
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
