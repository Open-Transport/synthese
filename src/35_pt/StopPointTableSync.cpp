
/** StopPointTableSync class implementation.
    @file StopPointTableSync.cpp

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

#include "StopPointTableSync.hpp"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "StopAreaTableSync.hpp"
#include "CoordinatesSystem.hpp"

#include <geos/geom/Coordinate.h>

using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
    using namespace db;
    using namespace util;
	using namespace pt;
	using namespace geography;

	template<> const string util::FactorableTemplate<SQLiteTableSync,StopPointTableSync>::FACTORY_KEY("15.55.01 Physical stops");
	template<> const string FactorableTemplate<Fetcher<graph::Vertex>, StopPointTableSync>::FACTORY_KEY("12");

	namespace pt
	{
		const string StopPointTableSync::COL_NAME = "name";
		const string StopPointTableSync::COL_PLACEID = "place_id";
		const string StopPointTableSync::COL_X = "x";
		const string StopPointTableSync::COL_Y = "y";
		const string StopPointTableSync::COL_OPERATOR_CODE("operator_code");
	}

    namespace db
    {
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<StopPointTableSync>::TABLE(
			"t012_physical_stops"
		);

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<StopPointTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(StopPointTableSync::COL_NAME, SQL_TEXT),
			SQLiteTableSync::Field(StopPointTableSync::COL_PLACEID, SQL_INTEGER, false),
			SQLiteTableSync::Field(StopPointTableSync::COL_X, SQL_DOUBLE),
			SQLiteTableSync::Field(StopPointTableSync::COL_Y, SQL_DOUBLE),
			SQLiteTableSync::Field(StopPointTableSync::COL_OPERATOR_CODE, SQL_TEXT),
			SQLiteTableSync::Field()
		};

		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<StopPointTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index(StopPointTableSync::COL_PLACEID.c_str(), ""),
			SQLiteTableSync::Index(StopPointTableSync::COL_OPERATOR_CODE.c_str(), ""),
			SQLiteTableSync::Index()
		};


		/** Does not update the place */
		template<> void SQLiteDirectTableSyncTemplate<StopPointTableSync,StopPoint>::Load(
			StopPoint* object,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			object->setName(rows->getText ( StopPointTableSync::COL_NAME));
			if(rows->getDouble(StopPointTableSync::COL_X) <= 0 || rows->getDouble(StopPointTableSync::COL_Y) <= 0)
			{
				object->setNull();
			}
			else
			{
				*object = GeoPoint(
					Coordinate(
					rows->getDouble(StopPointTableSync::COL_X),
					rows->getDouble(StopPointTableSync::COL_Y)
					), CoordinatesSystem::GetCoordinatesSystem("EPSG:27572")
				);
			}
			object->setCodeBySource(rows->getText ( StopPointTableSync::COL_OPERATOR_CODE));
			object->setHub(NULL);

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				StopArea* place = StopAreaTableSync::GetEditable(rows->getLongLong (StopPointTableSync::COL_PLACEID), env, linkLevel).get();
				object->setHub(place);

				place->addPhysicalStop(*object);
			}
		}



		template<> void SQLiteDirectTableSyncTemplate<StopPointTableSync,StopPoint>::Unlink(
			StopPoint* obj
		){
//			StopArea* place = const_cast<StopArea*>(obj->getConnectionPlace());
/// @todo	place->removePhysicalStop(obj);

			obj->setHub(NULL);
		}



		template<> void SQLiteDirectTableSyncTemplate<StopPointTableSync,StopPoint>::Save(
			StopPoint* object,
			optional<SQLiteTransaction&> transaction
		){
			ReplaceQuery<StopPointTableSync> query(*object);
			query.addField(object->getName());
			query.addField(dynamic_cast<const StopArea*>(object->getHub()) ? dynamic_cast<const StopArea*>(object->getHub())->getKey() : RegistryKeyType(0));
			query.addField(object->x);
			query.addField(object->y);
			query.addField(object->getCodeBySource());
			query.execute(transaction);
		}
    }

    namespace pt
    {
		StopPointTableSync::SearchResult StopPointTableSync::Search(
			Env& env, 
			optional<RegistryKeyType> placeId,
			optional<string> operatorCode,
			int first /*= 0 */,
			boost::optional<std::size_t> number  /*= 0 */,
			LinkLevel linkLevel
		){
			SelectQuery<StopPointTableSync> query;
			if(operatorCode)
			{
				query.addWhereField(COL_OPERATOR_CODE, *operatorCode, ComposedExpression::OP_LIKE);
			}
			if(placeId)
			{
				query.addWhereField(COL_PLACEID, *placeId);
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
