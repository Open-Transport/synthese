
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

#include "02_db/LinkException.h"

#include "Conversion.h"
#include "CityTableSync.h"

#include <assert.h>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace env;
	using namespace util;

	template<> const string util::FactorableTemplate<SQLiteTableSync,CrossingTableSync>::FACTORY_KEY("15.40.02 Crossings");

	namespace env
	{
		const std::string CrossingTableSync::TABLE_COL_CITYID = "city_id";
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<CrossingTableSync>::TABLE(
			CrossingTableSync::CreateFormat(
				"t043_crossings",
				SQLiteTableFormat::CreateFields(
					SQLiteTableSync::Field(CrossingTableSync::TABLE_COL_CITYID, SQL_INTEGER),
					SQLiteTableSync::Field()
				), SQLiteTableFormat::CreateIndexes(
					SQLiteTableSync::Index(CrossingTableSync::TABLE_COL_CITYID),
					SQLiteTableSync::Index()
		)	)	);


		template<> void SQLiteDirectTableSyncTemplate<CrossingTableSync,Crossing>::Load(
			Crossing* obj,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel)
		{
			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				uid cityId (rows->getLongLong (CrossingTableSync::TABLE_COL_CITYID));
				try
				{
					obj->setCity(CityTableSync::Get(cityId, env, linkLevel).get());
				}
				catch(ObjectNotFoundException<City>& e)
				{
					throw LinkException<CrossingTableSync>(obj->getKey(), CrossingTableSync::TABLE_COL_CITYID, e);
				}
			}
		}

		template<> void SQLiteDirectTableSyncTemplate<CrossingTableSync,Crossing>::Unlink(
			Crossing* obj
		){
			obj->setCity(NULL);
		}

		template<> void SQLiteDirectTableSyncTemplate<CrossingTableSync,Crossing>::Save(Crossing* obj)
		{

		}
	}


	namespace env
	{
		CrossingTableSync::CrossingTableSync ()
		: SQLiteRegistryTableSyncTemplate<CrossingTableSync,Crossing> ()
		{
		}



		CrossingTableSync::~CrossingTableSync ()
		{

		}
	}
}
