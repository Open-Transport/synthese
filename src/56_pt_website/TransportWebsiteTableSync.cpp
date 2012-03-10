
/** TransportWebsiteTableSync class implementation.
	@file TransportWebsiteTableSync.cpp

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

#include "TransportWebsiteTableSync.h"

#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "WebPageTableSync.h"
#include "ObjectSiteLinkTableSync.h"
#include "RollingStockFilterTableSync.h"
#include "TransportWebsiteRight.h"

#include <sstream>
#include <boost/tokenizer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost;
using namespace std;
using namespace boost::gregorian;
using namespace boost::posix_time;

namespace synthese
{
	using namespace pt;
	using namespace util;
	using namespace db;
	using namespace pt_website;
	using namespace cms;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,TransportWebsiteTableSync>::FACTORY_KEY("56.01 PT Services configurations");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<TransportWebsiteTableSync>::TABLE(
			"t086_pt_services_configurations"
		);

		template<> const Field DBTableSyncTemplate<TransportWebsiteTableSync>::_FIELDS[] = { Field() }; // Defined by the record

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<TransportWebsiteTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}


		template<> void DBDirectTableSyncTemplate<TransportWebsiteTableSync,TransportWebsite>::Load(
			TransportWebsite* site,
			const DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			if(linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				DBModule::LoadObjects(site->getLinkedObjectsIds(*rows), env, linkLevel);
			}
			site->loadFromRecord(*rows, env);
			if(linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				site->link(env, linkLevel == ALGORITHMS_OPTIMIZATION_LOAD_LEVEL);
			}
		}



		template<> void DBDirectTableSyncTemplate<TransportWebsiteTableSync,TransportWebsite>::Unlink(
			TransportWebsite* obj
		){
			obj->unlink();
		}


		template<> void DBDirectTableSyncTemplate<TransportWebsiteTableSync,TransportWebsite>::Save(
			TransportWebsite* site,
			optional<DBTransaction&> transaction
		){
			// Query
			ReplaceQuery<TransportWebsiteTableSync> query(*site);
			ParametersMap map;
			site->toParametersMap(map);
			query.setValues(map);
			query.execute(transaction);
		}



		template<> bool DBTableSyncTemplate<TransportWebsiteTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportWebsiteRight>(DELETE_RIGHT);
		}



		template<> void DBTableSyncTemplate<TransportWebsiteTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
			Env env;
			ObjectSiteLinkTableSync::SearchResult links(ObjectSiteLinkTableSync::Search(env, id));
			BOOST_FOREACH(const ObjectSiteLinkTableSync::SearchResult::value_type& link, links)
			{
				ObjectSiteLinkTableSync::Remove(NULL, link->getKey(), transaction, false);
			}
			RollingStockFilterTableSync::SearchResult filters(RollingStockFilterTableSync::Search(env, id));
			BOOST_FOREACH(const RollingStockFilterTableSync::SearchResult::value_type& filter, filters)
			{
				RollingStockFilterTableSync::Remove(NULL, filter->getKey(), transaction, false);
			}
		}



		template<> void DBTableSyncTemplate<TransportWebsiteTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<TransportWebsiteTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO log the removal
		}
	}

	namespace pt_website
	{
		TransportWebsiteTableSync::SearchResult TransportWebsiteTableSync::Search(
			Env& env,
			std::string name
			, int first /*= 0*/,
			boost::optional<std::size_t> number
			, bool orderByName
			, bool raisingOrder,
			LinkLevel linkLevel
		){
			SelectQuery<TransportWebsiteTableSync> query;
			if (!name.empty())
			{
				query.addWhereField(ObjectFieldDefinition<Name>::FIELD.name, name, ComposedExpression::OP_LIKE);
			}
			if (orderByName)
			{
				query.addOrderField(ObjectFieldDefinition<Name>::FIELD.name, raisingOrder);
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
