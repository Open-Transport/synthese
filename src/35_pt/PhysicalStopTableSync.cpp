
/** PhysicalStopTableSync class implementation.
    @file PhysicalStopTableSync.cpp

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

#include "PhysicalStopTableSync.h"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "ConnectionPlaceTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
    using namespace db;
    using namespace pt;
    using namespace util;
	using namespace pt;

	template<> const string util::FactorableTemplate<SQLiteTableSync,PhysicalStopTableSync>::FACTORY_KEY("15.55.01 Physical stops");
	template<> const string FactorableTemplate<Fetcher<graph::Vertex>, PhysicalStopTableSync>::FACTORY_KEY("12");

	namespace pt
	{
		const string PhysicalStopTableSync::COL_NAME = "name";
		const string PhysicalStopTableSync::COL_PLACEID = "place_id";
		const string PhysicalStopTableSync::COL_X = "x";
		const string PhysicalStopTableSync::COL_Y = "y";
		const string PhysicalStopTableSync::COL_OPERATOR_CODE("operator_code");
	}

    namespace db
    {
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<PhysicalStopTableSync>::TABLE(
			"t012_physical_stops"
		);

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<PhysicalStopTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(PhysicalStopTableSync::COL_NAME, SQL_TEXT),
			SQLiteTableSync::Field(PhysicalStopTableSync::COL_PLACEID, SQL_INTEGER, false),
			SQLiteTableSync::Field(PhysicalStopTableSync::COL_X, SQL_DOUBLE),
			SQLiteTableSync::Field(PhysicalStopTableSync::COL_Y, SQL_DOUBLE),
			SQLiteTableSync::Field(PhysicalStopTableSync::COL_OPERATOR_CODE, SQL_TEXT),
			SQLiteTableSync::Field()
		};

		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<PhysicalStopTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index(PhysicalStopTableSync::COL_PLACEID.c_str(), ""),
			SQLiteTableSync::Index(PhysicalStopTableSync::COL_OPERATOR_CODE.c_str(), ""),
			SQLiteTableSync::Index()
		};


		/** Does not update the place */
		template<> void SQLiteDirectTableSyncTemplate<PhysicalStopTableSync,PhysicalStop>::Load(
			PhysicalStop* object,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			object->setName(rows->getText ( PhysicalStopTableSync::COL_NAME));
			object->setXY (rows->getDouble ( PhysicalStopTableSync::COL_X), rows->getDouble ( PhysicalStopTableSync::COL_Y));
			object->setCodeBySource(rows->getText ( PhysicalStopTableSync::COL_OPERATOR_CODE));
			object->setHub(NULL);

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				StopArea* place = ConnectionPlaceTableSync::GetEditable(rows->getLongLong (PhysicalStopTableSync::COL_PLACEID), env, linkLevel).get();
				object->setHub(place);

				place->addPhysicalStop(*object);
			}
		}



		template<> void SQLiteDirectTableSyncTemplate<PhysicalStopTableSync,PhysicalStop>::Unlink(
			PhysicalStop* obj
		){
//			StopArea* place = const_cast<StopArea*>(obj->getConnectionPlace());
/// @todo	place->removePhysicalStop(obj);

			obj->setHub(NULL);
		}



		template<> void SQLiteDirectTableSyncTemplate<PhysicalStopTableSync,PhysicalStop>::Save(
			PhysicalStop* object,
			optional<SQLiteTransaction&> transaction
		){
			ReplaceQuery<PhysicalStopTableSync> query(*object);
			query.addField(object->getName());
			query.addField(dynamic_cast<const StopArea*>(object->getHub()) ? dynamic_cast<const StopArea*>(object->getHub())->getKey() : RegistryKeyType(0));
			query.addField(object->getX());
			query.addField(object->getY());
			query.addField(object->getCodeBySource());
			query.execute(transaction);
		}
    }

    namespace pt
    {
		PhysicalStopTableSync::SearchResult PhysicalStopTableSync::Search(
			Env& env, 
			optional<RegistryKeyType> placeId,
			optional<string> operatorCode,
			int first /*= 0 */,
			boost::optional<std::size_t> number  /*= 0 */,
			LinkLevel linkLevel
		){
			SelectQuery<PhysicalStopTableSync> query;
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
