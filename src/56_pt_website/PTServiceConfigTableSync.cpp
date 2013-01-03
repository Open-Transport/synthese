
/** PTServiceConfigTableSync class implementation.
	@file PTServiceConfigTableSync.cpp

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

#include "PTServiceConfigTableSync.hpp"

#include "ObjectSiteLinkTableSync.h"
#include "Profile.h"
#include "ReplaceQuery.h"
#include "RollingStockFilterTableSync.h"
#include "SelectQuery.hpp"
#include "Session.h"
#include "TransportWebsiteRight.h"
#include "User.h"
#include "WebPageTableSync.h"

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
		template<> const string FactorableTemplate<DBTableSync,PTServiceConfigTableSync>::FACTORY_KEY("56.01 PT Services configurations");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<PTServiceConfigTableSync>::TABLE(
			"t086_pt_services_configurations"
		);

		template<> const Field DBTableSyncTemplate<PTServiceConfigTableSync>::_FIELDS[] = { Field() }; // Defined by the record

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<PTServiceConfigTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		template<> bool DBTableSyncTemplate<PTServiceConfigTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportWebsiteRight>(DELETE_RIGHT);
		}



		template<> void DBTableSyncTemplate<PTServiceConfigTableSync>::BeforeDelete(
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



		template<> void DBTableSyncTemplate<PTServiceConfigTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<PTServiceConfigTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO log the removal
		}
	}

	namespace pt_website
	{
		PTServiceConfigTableSync::SearchResult PTServiceConfigTableSync::Search(
			Env& env,
			std::string name
			, int first /*= 0*/,
			boost::optional<std::size_t> number
			, bool orderByName
			, bool raisingOrder,
			LinkLevel linkLevel
		){
			SelectQuery<PTServiceConfigTableSync> query;
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
