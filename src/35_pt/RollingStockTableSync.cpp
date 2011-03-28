
/** RollingStockTableSync class implementation.
	@file RollingStockTableSync.cpp
	@author Hugues Romain
	@date 2007

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

#include "RollingStockTableSync.h"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace pt;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,RollingStockTableSync>::FACTORY_KEY("35.10.07 Rolling Stock");
	}

	namespace pt
	{
		const string RollingStockTableSync::COL_NAME("name");
		const string RollingStockTableSync::COL_ARTICLE("article");
		const string RollingStockTableSync::COL_INDICATOR("indicator_label");
		const string RollingStockTableSync::COL_TRIDENT("trident_key");
		const string RollingStockTableSync::COL_IS_TRIDENT_REFERENCE("is_trident_reference");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<RollingStockTableSync>::TABLE(
			"t049_rolling_stock"
		);

		template<> const DBTableSync::Field DBTableSyncTemplate<RollingStockTableSync>::_FIELDS[]=
		{
			DBTableSync::Field(TABLE_COL_ID, SQL_INTEGER),
			DBTableSync::Field(RollingStockTableSync::COL_NAME, SQL_TEXT),
			DBTableSync::Field(RollingStockTableSync::COL_ARTICLE, SQL_TEXT),
			DBTableSync::Field(RollingStockTableSync::COL_INDICATOR, SQL_TEXT),
			DBTableSync::Field(RollingStockTableSync::COL_TRIDENT, SQL_TEXT),
			DBTableSync::Field(RollingStockTableSync::COL_IS_TRIDENT_REFERENCE, SQL_INTEGER),
			DBTableSync::Field()
		};
		
		template<> const DBTableSync::Index DBTableSyncTemplate<RollingStockTableSync>::_INDEXES[]=
		{
			DBTableSync::Index(RollingStockTableSync::COL_TRIDENT.c_str(), RollingStockTableSync::COL_IS_TRIDENT_REFERENCE.c_str(), ""),
			DBTableSync::Index()
		};


		template<> void DBDirectTableSyncTemplate<RollingStockTableSync,RollingStock>::Load(
			RollingStock* object
			, const db::DBResultSPtr& rows
			, Env& env,
			LinkLevel linkLevel
		){
			// Properties
			object->setName(rows->getText(RollingStockTableSync::COL_NAME));
			object->setArticle(rows->getText(RollingStockTableSync::COL_ARTICLE));
			object->setIndicator(rows->getText(RollingStockTableSync::COL_INDICATOR));
			object->setTridentKey(rows->getText(RollingStockTableSync::COL_TRIDENT));
			object->setIsTridentKeyReference(rows->getBool(RollingStockTableSync::COL_IS_TRIDENT_REFERENCE));
		}



		template<> void DBDirectTableSyncTemplate<RollingStockTableSync,RollingStock>::Save(
			RollingStock* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<RollingStockTableSync> query(*object);
			query.addField(object->getName());
			query.addField(object->getArticle());
			query.addField(object->getIndicator());
			query.addField(object->getTridentKey());
			query.addField(object->getIsTridentKeyReference());
			query.execute(transaction);
		}



		template<> void DBDirectTableSyncTemplate<RollingStockTableSync,RollingStock>::Unlink(
			RollingStock* obj
		){
		}
	}
	
	
	
	namespace pt
	{
		RollingStockTableSync::RollingStockTableSync()
			: DBRegistryTableSyncTemplate<RollingStockTableSync,RollingStock>()
		{
		}



		RollingStockTableSync::SearchResult RollingStockTableSync::Search(
			Env& env,
			optional<string> tridentKey,
			bool tridentReference,
			bool orderByName,
			bool raisingOrder,
			int first /*= 0*/,
			int number /*= 0*/,
			LinkLevel linkLevel
		){
			SelectQuery<RollingStockTableSync> query;
			if(tridentKey)
			{
				query.addWhereField(COL_TRIDENT, *tridentKey);
			}
			if(tridentReference)
			{
				query.addWhereField(COL_IS_TRIDENT_REFERENCE, 1);
			}
			if(orderByName)
			{
				query.addOrderField(COL_NAME, raisingOrder);
			}
			query.setNumber(number);
			query.setFirst(first);
		
			return LoadFromQuery(query, env, linkLevel);
		}



		RollingStockTableSync::Labels RollingStockTableSync::GetLabels(
			std::string unknownLabel
		){
			Labels result;
			if(!unknownLabel.empty())
			{
				result.push_back(make_pair(optional<RegistryKeyType>(),unknownLabel));
			}
			SearchResult modes(Search(Env::GetOfficialEnv()));
			BOOST_FOREACH(SearchResult::value_type& mode, modes)
			{
				result.push_back(make_pair(mode->getKey(), mode->getName()));
			}
			return result;
		}
	}
}
