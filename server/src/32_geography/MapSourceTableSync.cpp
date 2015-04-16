
//////////////////////////////////////////////////////////////////////////
///	MapSourceTableSync class implementation.
///	@file MapSourceTableSync.cpp
///	@author Hugues Romain
///	@date 2011
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include <sstream>

#include "MapSourceTableSync.hpp"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "CoordinatesSystem.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace geography;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,MapSourceTableSync>::FACTORY_KEY("32.30 Map sources");
	}

	namespace geography
	{
		const string MapSourceTableSync::COL_NAME("name");
		const string MapSourceTableSync::COL_URL("url");
		const string MapSourceTableSync::COL_SRID("srid");
		const string MapSourceTableSync::COL_TYPE("type");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<MapSourceTableSync>::TABLE(
			"t075_map_sources"
		);



		template<> const Field DBTableSyncTemplate<MapSourceTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(MapSourceTableSync::COL_NAME, SQL_TEXT),
			Field(MapSourceTableSync::COL_URL, SQL_TEXT),
			Field(MapSourceTableSync::COL_SRID, SQL_INTEGER),
			Field(MapSourceTableSync::COL_TYPE, SQL_INTEGER),
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<MapSourceTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		template<> void OldLoadSavePolicy<MapSourceTableSync,MapSource>::Load(
			MapSource* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			// Name
			object->setName(rows->getText(MapSourceTableSync::COL_NAME));

			// URL
			object->setURL(rows->getText(MapSourceTableSync::COL_URL));

			// SRID
			CoordinatesSystem::SRID srid(rows->getInt(MapSourceTableSync::COL_SRID));
			try
			{
				object->setCoordinatesSystem(
					CoordinatesSystem::GetCoordinatesSystem(
						srid
				)	);
			}
			catch(CoordinatesSystem::CoordinatesSystemNotFoundException&)
			{
				Log::GetInstance().warn("Inconsistent SRID value : "+ lexical_cast<string>(srid) +" in map source "+ lexical_cast<string>(object->getKey()));
			}

			// Type
			int typeNum(rows->getInt(MapSourceTableSync::COL_TYPE));
			if(typeNum > MapSource::MAX_TYPE_INT || typeNum < 0)
			{
				Log::GetInstance().warn("Inconsistent type value : "+ lexical_cast<string>(typeNum) +" in map source "+ lexical_cast<string>(object->getKey()));
			}
			else
			{
				object->setType(static_cast<MapSource::Type>(typeNum));
			}
		}



		template<> void OldLoadSavePolicy<MapSourceTableSync,MapSource>::Save(
			MapSource* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<MapSourceTableSync> query(*object);
			query.addField(object->getName());
			query.addField(object->getURL());
			try
			{
				query.addField(static_cast<int>(object->getCoordinatesSystem().getSRID()));
			}
			catch(MapSource::NoCoordinatesSystemException&)
			{
				query.addField(string());
			}
			query.addField(static_cast<int>(object->getType()));
			query.execute(transaction);
		}



		template<> void OldLoadSavePolicy<MapSourceTableSync,MapSource>::Unlink(
			MapSource* obj
		){
		}



		template<> bool DBTableSyncTemplate<MapSourceTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return true;
		}



		template<> void DBTableSyncTemplate<MapSourceTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<MapSourceTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<MapSourceTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
		}

	}



	namespace geography
	{
		MapSourceTableSync::SearchResult MapSourceTableSync::Search(
			util::Env& env,
			optional<string> name,
			size_t first /*= 0*/,
			optional<size_t> number /*= boost::optional<std::size_t>()*/,
			bool orderByName,
			bool raisingOrder,
			util::LinkLevel linkLevel
		){
			SelectQuery<MapSourceTableSync> query;
			if(name)
			{
				query.addWhereField(COL_NAME, *name, ComposedExpression::OP_LIKE);
			}
			if(orderByName)
			{
			 	query.addOrderField(COL_NAME, raisingOrder);
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
