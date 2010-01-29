
/** RollingStockFilterTableSync class implementation.
	@file RollingStockFilterTableSync.cpp
	@author Hugues
	@date 2010

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#include <sstream>

#include "RollingStockFilterTableSync.h"
#include "RollingStockFilter.h"
#include "RollingStockTableSync.h"
#include "RollingStock.h"
#include "Site.h"
#include "SiteTableSync.h"
#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace transportwebsite;
	using namespace env;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,RollingStockFilterTableSync>::FACTORY_KEY("36.5 Rolling Stock Filter");
	}


	namespace transportwebsite
	{
		const std::string RollingStockFilterTableSync::COL_SITE_ID("site_id");
		const std::string RollingStockFilterTableSync::COL_RANK("rank");
		const std::string RollingStockFilterTableSync::COL_NAME("name");
		const std::string RollingStockFilterTableSync::COL_AUTHORIZED_ONLY("authorized_only");
		const std::string RollingStockFilterTableSync::COL_ROLLING_STOCK_IDS("rolling_stock_ids");

	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<RollingStockFilterTableSync>::TABLE(
			"t062_rolling_stock_filters"
		);

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<RollingStockFilterTableSync>::_FIELDS[] =
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(RollingStockFilterTableSync::COL_SITE_ID, SQL_INTEGER),
			SQLiteTableSync::Field(RollingStockFilterTableSync::COL_RANK, SQL_INTEGER),
			SQLiteTableSync::Field(RollingStockFilterTableSync::COL_NAME, SQL_TEXT),
			SQLiteTableSync::Field(RollingStockFilterTableSync::COL_AUTHORIZED_ONLY, SQL_INTEGER),
			SQLiteTableSync::Field(RollingStockFilterTableSync::COL_ROLLING_STOCK_IDS, SQL_TEXT),
			SQLiteTableSync::Field()
		};

		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<RollingStockFilterTableSync>::_INDEXES[] =
		{
			SQLiteTableSync::Index()
		};



		template<> void SQLiteDirectTableSyncTemplate<RollingStockFilterTableSync,RollingStockFilter>::Load(
			RollingStockFilter* object
			, const db::SQLiteResultSPtr& rows
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
					shared_ptr<Site> site(
						SiteTableSync::GetEditable(
							rows->getLongLong(RollingStockFilterTableSync::COL_SITE_ID),
							environment,
							linkLevel
					)	);
					object->setSite(site.get());
					site->addRollingStockFilter(*object);
				}
				catch (ObjectNotFoundException<Site> e)
				{
					Log::GetInstance().warn("Site not found in rolling stock filter "+ lexical_cast<string>(object->getKey()));
				}

				vector< string > parsed_ids;
				split(
					parsed_ids,
					rows->getText(RollingStockFilterTableSync::COL_ROLLING_STOCK_IDS),
					is_any_of(",")
				);
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



		template<> void SQLiteDirectTableSyncTemplate<RollingStockFilterTableSync,RollingStockFilter>::Save(
			RollingStockFilter* object
			, boost::optional<SQLiteTransaction&> transaction /* = boost::optional<SQLiteTransaction&> */
		){
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());
               
			 query <<
				" REPLACE INTO " << TABLE.NAME << " VALUES(" <<
				object->getKey() << "," <<
				(object->getSite() ? object->getSite()->getKey() : RegistryKeyType(0)) << "," <<
				object->getRank() << "," <<
				Conversion::ToSQLiteString(object->getName()) << "," <<
				object->getAuthorizedOnly() << "," <<
				"\"";
			 bool first(true);
			 BOOST_FOREACH(const RollingStock* item, object->getList())
			 {
				 if(!first) query << ",";
				 query << item->getKey();
				 first = false;
			 }
			query << "\"" <<
			")";
			
			 DBModule::GetSQLite()->execUpdate(query.str(), transaction);
		}



		template<> void SQLiteDirectTableSyncTemplate<RollingStockFilterTableSync,RollingStockFilter>::Unlink(
			RollingStockFilter* obj
		){
			if(obj->getSite())
			{
				const_cast<Site*>(obj->getSite())->removeRollingStockFilter(*obj);
			}
		}
	}
	
	
	
	namespace transportwebsite
	{
		vector<shared_ptr<RollingStockFilter> > RollingStockFilterTableSync::Search(
			Env& env,
			int first /*= 0*/,
			boost::optional<std::size_t> number,
			LinkLevel linkLevel
		){
			SQLite* sqlite = DBModule::GetSQLite();

			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE 1 ";
			/// @todo Fill Where criteria
			// if (!name.empty())
			// 	query << " AND " << COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(name, false) << "%'";
				;
			//if (orderByName)
			//	query << " ORDER BY " << COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (number)
			{
				query << " LIMIT " << (*number + 1);
				if (first > 0)
					query << " OFFSET " << first;
			}

			return LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
