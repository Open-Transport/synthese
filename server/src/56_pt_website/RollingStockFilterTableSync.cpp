
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


	namespace pt_website
	{
		const std::string RollingStockFilterTableSync::COL_SITE_ID("site_id");
		const std::string RollingStockFilterTableSync::COL_RANK("rank");
		const std::string RollingStockFilterTableSync::COL_NAME("name");
		const std::string RollingStockFilterTableSync::COL_AUTHORIZED_ONLY("authorized_only");
		const std::string RollingStockFilterTableSync::COL_ROLLING_STOCK_IDS("rolling_stock_ids");

	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<RollingStockFilterTableSync>::TABLE(
			"t062_rolling_stock_filters"
		);

		template<> const Field DBTableSyncTemplate<RollingStockFilterTableSync>::_FIELDS[] =
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(RollingStockFilterTableSync::COL_SITE_ID, SQL_INTEGER),
			Field(RollingStockFilterTableSync::COL_RANK, SQL_INTEGER),
			Field(RollingStockFilterTableSync::COL_NAME, SQL_TEXT),
			Field(RollingStockFilterTableSync::COL_AUTHORIZED_ONLY, SQL_INTEGER),
			Field(RollingStockFilterTableSync::COL_ROLLING_STOCK_IDS, SQL_TEXT),
			Field()
		};

		template<> DBTableSync::Indexes DBTableSyncTemplate<RollingStockFilterTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		template<> void OldLoadSavePolicy<RollingStockFilterTableSync,RollingStockFilter>::Load(
			RollingStockFilter* object
			, const db::DBResultSPtr& rows
			, util::Env& environment, util::LinkLevel linkLevel /* = util::UP_LINKS_LOAD_LEVEL */
		){
			// Columns reading
			RegistryKeyType id(rows->getLongLong(TABLE_COL_ID));

			// Properties
			object->setKey(id);
			object->setRank(rows->getInt(RollingStockFilterTableSync::COL_RANK));
			object->setName(rows->getText(RollingStockFilterTableSync::COL_NAME));
			object->setAuthorizedOnly(rows->getBool(RollingStockFilterTableSync::COL_AUTHORIZED_ONLY));

			if(linkLevel >= UP_LINKS_LOAD_LEVEL)
			{
				try
				{
					boost::shared_ptr<PTServiceConfig> site(
						PTServiceConfigTableSync::GetEditable(
							rows->getLongLong(RollingStockFilterTableSync::COL_SITE_ID),
							environment,
							linkLevel
					)	);
					object->setSite(site.get());
					site->addRollingStockFilter(*object);
				}
				catch (ObjectNotFoundException<PTServiceConfig> e)
				{
					Log::GetInstance().warn("Website not found in rolling stock filter "+ lexical_cast<string>(object->getKey()));
				}

				vector< string > parsed_ids;
				const string rollingStocksString(rows->getText(RollingStockFilterTableSync::COL_ROLLING_STOCK_IDS));
				split(parsed_ids, rollingStocksString, is_any_of(","));
				object->cleanRollingStocks();
				BOOST_FOREACH(const string& id, parsed_ids)
				{
					if(id.empty())
					{
						continue;
					}
					try
					{
						object->addRollingStock(
							RollingStockTableSync::Get(
								lexical_cast<RegistryKeyType>(id),
								environment,
								linkLevel
							).get()
						);
					}
					catch(ObjectNotFoundException<RollingStock> e)
					{
						Log::GetInstance().warn("Rolling stock not found in rolling stock filter "+ lexical_cast<string>(object->getKey()));
					}
				}

			}
		}



		template<> void OldLoadSavePolicy<RollingStockFilterTableSync,RollingStockFilter>::Save(
			RollingStockFilter* object
			, boost::optional<DBTransaction&> transaction /* = boost::optional<DBTransaction&> */
		){
			ReplaceQuery<RollingStockFilterTableSync> query(*object);
			query.addField(object->getSite() ? object->getSite()->getKey() : RegistryKeyType(0));
			query.addField(object->getRank());
			query.addField(object->getName());
			query.addField(object->getAuthorizedOnly());

			bool first(true);
			stringstream rollingStocks;
			BOOST_FOREACH(const RollingStock* item, object->getList())
			{
				if(!first) rollingStocks << ",";
				rollingStocks << item->getKey();
				first = false;
			}
			query.addField(rollingStocks.str());

			query.execute(transaction);
		}



		template<> void OldLoadSavePolicy<RollingStockFilterTableSync,RollingStockFilter>::Unlink(
			RollingStockFilter* obj
		){
			if(obj->getSite())
			{
				const_cast<PTServiceConfig*>(obj->getSite())->removeRollingStockFilter(*obj);
			}
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
				query.addWhereField(COL_SITE_ID, *siteId);
			}
			if (number)
			{
				query.setNumber(*number + 1);
				if (first > 0)
					query.setFirst(first);
			}

			return LoadFromQuery(query, env, linkLevel);
		}
	}
}
