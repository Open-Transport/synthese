
/** RollingStockFilterTableSync class implementation.
	@file RollingStockFilterTableSync.cpp
	@author Hugues
	@date 2010

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

#include "RollingStockFilterTableSync.h"

#include "DBException.hpp"
#include "DBModule.h"
#include "DBResult.hpp"
#include "Profile.h"
#include "PTServiceConfigTableSync.hpp"
#include "ReplaceQuery.h"
#include "RollingStockTableSync.hpp"
#include "Session.h"
#include "TransportWebsiteRight.h"
#include "User.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace pt_website;
	using namespace vehicle;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,RollingStockFilterTableSync>::FACTORY_KEY("36.5 Rolling Stock Filter");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<RollingStockFilterTableSync>::TABLE(
			"t062_rolling_stock_filters"
		);

		template<> const Field DBTableSyncTemplate<RollingStockFilterTableSync>::_FIELDS[] =
		{
			Field()
		};

		template<> DBTableSync::Indexes DBTableSyncTemplate<RollingStockFilterTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		template<> bool DBTableSyncTemplate<RollingStockFilterTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportWebsiteRight>(DELETE_RIGHT);
		}



		template<> void DBTableSyncTemplate<RollingStockFilterTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<RollingStockFilterTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<RollingStockFilterTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO log the removal
		}
	}



	namespace pt_website
	{
		RollingStockFilterTableSync::SearchResult RollingStockFilterTableSync::Search(
			Env& env,
			boost::optional<util::RegistryKeyType> siteId,
			int first /*= 0*/,
			boost::optional<std::size_t> number,
			LinkLevel linkLevel
		){
			SelectQuery<RollingStockFilterTableSync> query;
			if(siteId)
			{
				query.addWhereField(ServiceConfig::FIELD.name, *siteId);
			}
			if (number)
			{
				query.setNumber(*number + 1);
				if (first > 0)
					query.setFirst(first);
			}

			return LoadFromQuery(query, env, linkLevel);
		}

		bool RollingStockFilterTableSync::allowList(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::READ);
		}
	}
}
