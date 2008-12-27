
/** CityTableSync class implementation.
	@file CityTableSync.cpp

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

#include "CityTableSync.h"
#include "Conversion.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "EnvModule.h"

#include <sqlite3.h>
#include <assert.h>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace env;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,CityTableSync>::FACTORY_KEY("15.20.01 Cities");
	}

	namespace env
	{
		const string CityTableSync::TABLE_COL_NAME("name");
		const string CityTableSync::TABLE_COL_CODE("code");
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<CityTableSync>::TABLE(
			CityTableSync::CreateFormat(
				"t006_cities",
				SQLiteTableFormat::CreateFields(
					SQLiteTableSync::Field(CityTableSync::TABLE_COL_NAME, SQL_TEXT),
					SQLiteTableSync::Field(CityTableSync::TABLE_COL_CODE, SQL_TEXT),
					SQLiteTableSync::Field()
				), SQLiteTableFormat::CreateIndexes(
					SQLiteTableSync::Index(CityTableSync::TABLE_COL_NAME),
					SQLiteTableSync::Index(CityTableSync::TABLE_COL_CODE),
					SQLiteTableSync::Index()
				)
		)	);

		template<> void SQLiteDirectTableSyncTemplate<CityTableSync,City>::Load(
			City* object,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
		    object->setName(rows->getText ( CityTableSync::TABLE_COL_NAME));
		    object->setCode(rows->getText ( CityTableSync::TABLE_COL_CODE));

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				EnvModule::AddToCitiesMatchers(object);
			}
		}


		template<> void SQLiteDirectTableSyncTemplate<CityTableSync,City>::Unlink(City* obj)
		{
			EnvModule::RemoveFromCitiesMatchers(obj);
		}

		template<> void SQLiteDirectTableSyncTemplate<CityTableSync,City>::Save(City* object)
		{
			/// @todo Implement it
		}
	}

	namespace env
	{
		CityTableSync::CityTableSync ()
			: SQLiteAutoRegisterTableSyncTemplate<CityTableSync,City> ()
		{
		}


		CityTableSync::~CityTableSync ()
		{

		}
	}
}
