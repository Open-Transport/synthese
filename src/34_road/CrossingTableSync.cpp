
/** CrossingTableSync class implementation.
	@file CrossingTableSync.cpp

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

#include "CrossingTableSync.hpp"
#include "RoadTableSync.h"
#include "Crossing.h"
#include "DBModule.h"
#include "LinkException.h"
#include "DataSource.h"
#include "DataSourceTableSync.h"
#include "SelectQuery.hpp"
#include "ReplaceQuery.h"

#include <sstream>
#include <geos/geom/Point.h>

using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace road;
	using namespace impex;
	using namespace graph;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync, CrossingTableSync>::FACTORY_KEY("34.20.01 Crossings");
		template<> const string FactorableTemplate<Fetcher<Vertex>, CrossingTableSync>::FACTORY_KEY("43");
	}

	namespace road
	{
		const std::string CrossingTableSync::COL_CODE_BY_SOURCE ("code_by_source");
		const std::string CrossingTableSync::COL_SOURCE_ID ("source_id");
		const std::string CrossingTableSync::COL_GEOMETRY("geometry");
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<CrossingTableSync>::TABLE(
			"t043_crossings"
		);

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<CrossingTableSync>::_FIELDS[] =
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(CrossingTableSync::COL_CODE_BY_SOURCE, SQL_TEXT),
			SQLiteTableSync::Field(CrossingTableSync::COL_SOURCE_ID, SQL_INTEGER),
			SQLiteTableSync::Field(CrossingTableSync::COL_GEOMETRY, SQL_GEOM_POINT),
			SQLiteTableSync::Field()
		};

		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<CrossingTableSync>::_INDEXES[] =
		{
			SQLiteTableSync::Index()
		};

		template<> void SQLiteDirectTableSyncTemplate<CrossingTableSync, Crossing>::Load(
			Crossing* object,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){

			// Geometry
			shared_ptr<Point> point(
				static_pointer_cast<Point, Geometry>(
					rows->getGeometry(CrossingTableSync::COL_GEOMETRY)
			)	);
			if(point.get())
			{
				object->setGeometry(point);
			}

			// Code by source
			object->setCodeBySource(rows->getText(CrossingTableSync::COL_CODE_BY_SOURCE));
		
			if (linkLevel >= UP_LINKS_LOAD_LEVEL)
			{
				// Columns reading
				RegistryKeyType sourceId = rows->getLongLong(CrossingTableSync::COL_SOURCE_ID);

				// Links from the object
				try
				{
					if(sourceId > 0)
					{
						object->setDataSource(DataSourceTableSync::Get(sourceId, env, linkLevel).get());
					}
				}
				catch (ObjectNotFoundException<DataSource>& e)
				{
					throw LinkException<DataSourceTableSync>(rows, CrossingTableSync::COL_SOURCE_ID, e);
				}
			}
		}



		template<> void SQLiteDirectTableSyncTemplate<CrossingTableSync, Crossing>::Unlink(
			Crossing* obj
		){
		}



		template<> void SQLiteDirectTableSyncTemplate<CrossingTableSync, Crossing>::Save(
			Crossing* object,
			optional<SQLiteTransaction&> transaction
		){
			ReplaceQuery<CrossingTableSync> query(*object);
			query.addField(object->getCodeBySource());
			query.addField(object->getDataSource() ? lexical_cast<string>(object->getDataSource()->getKey()) : 0);
			query.addField(static_pointer_cast<Geometry,Point>(object->getGeometry()));
			query.execute(transaction);
		}
	}

	namespace road
	{
		CrossingTableSync::CrossingTableSync()
			: SQLiteRegistryTableSyncTemplate<CrossingTableSync, Crossing>()
		{
		}

		CrossingTableSync::~CrossingTableSync()
		{

		}

		CrossingTableSync::SearchResult CrossingTableSync::Search(
			Env& env,
			int first /*= 0*/,
			boost::optional<std::size_t> number /*= 0*/,
			LinkLevel linkLevel
		){
			SelectQuery<CrossingTableSync> query;
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
	}
}
