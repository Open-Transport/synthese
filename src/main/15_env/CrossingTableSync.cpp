
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


#include "CrossingTableSync.h"

#include "01_util/Conversion.h"

#include "15_env/CityTableSync.h"

#include <assert.h>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace env;
	using namespace util;

	template<> const string util::FactorableTemplate<SQLiteTableSync,CrossingTableSync>::FACTORY_KEY("15.40.02 Crossings");

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<CrossingTableSync,Crossing>::TABLE_NAME = "t043_crossings";
		template<> const int SQLiteTableSyncTemplate<CrossingTableSync,Crossing>::TABLE_ID = 43;
		template<> const bool SQLiteTableSyncTemplate<CrossingTableSync,Crossing>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<CrossingTableSync,Crossing>::load(Crossing* crossing, const db::SQLiteResultSPtr& rows )
		{
			uid id (rows->getLongLong (TABLE_COL_ID));
			crossing->setKey(id);
		}

		template<> void SQLiteTableSyncTemplate<CrossingTableSync,Crossing>::_link(Crossing* obj, const SQLiteResultSPtr& rows, GetSource temporary)
		{
			uid cityId (rows->getLongLong (CrossingTableSync::TABLE_COL_CITYID));
			obj->setCity(City::Get(cityId).get());

		}

		template<> void SQLiteTableSyncTemplate<CrossingTableSync,Crossing>::_unlink(Crossing* obj)
		{


			obj->setCity(NULL);
		}

		template<> void SQLiteTableSyncTemplate<CrossingTableSync,Crossing>::save(Crossing* obj)
		{

		}
	}


	namespace env
	{
		const std::string CrossingTableSync::TABLE_COL_CITYID = "city_id";

		CrossingTableSync::CrossingTableSync ()
		: SQLiteRegistryTableSyncTemplate<CrossingTableSync,Crossing> ()
		{
			addTableColumn (TABLE_COL_ID, "INTEGER", true);
			addTableColumn (TABLE_COL_CITYID, "INTEGER", false);

			addTableIndex(TABLE_COL_CITYID);
		}



		CrossingTableSync::~CrossingTableSync ()
		{

		}

/*		    
		void 
			CrossingTableSync::rowsAdded (synthese::db::SQLite* sqlite, 
			synthese::db::SQLiteSync* sync,
			const synthese::db::SQLiteResultSPtr& rows, bool isFirstSync)
		{
			while (rows->next ())
			{
			    uid id (rows->getLongLong (TABLE_COL_ID));
			    
				if (Crossing::Contains (id)) return;
			    
			    uid cityId (rows->getLongLong (TABLE_COL_CITYID));

			    shared_ptr<const City> city = City::Get (cityId);
			    
			    Crossing* crossing(new Crossing (id, city.get()));

			    // Add crossing to connection place registry but not in city lexical matcher...
			    crossing->store();
			}
		}



		void 
			CrossingTableSync::rowsUpdated (synthese::db::SQLite* sqlite, 
			synthese::db::SQLiteSync* sync,
			const synthese::db::SQLiteResultSPtr& rows)
		{
			while (rows->next ())
			{
				shared_ptr<Crossing> crossing = Crossing::GetUpdateable(
				    rows->getLongLong (TABLE_COL_ID)
				);
				
				load(crossing.get(), rows);
			}
		}




		void 
			CrossingTableSync::rowsRemoved (synthese::db::SQLite* sqlite, 
			synthese::db::SQLiteSync* sync,
			const synthese::db::SQLiteResultSPtr& rows)
		{
			while (rows->next ())
			{
			    uid id = rows->getLongLong (TABLE_COL_ID);
			    
			    Crossing::Remove (id);
			}
		}
	    
*/
	}
}
