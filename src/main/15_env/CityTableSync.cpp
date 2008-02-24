
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

#include "01_util/Conversion.h"

#include "02_db/SQLiteResult.h"
#include "02_db/SQLite.h"

#include "15_env/EnvModule.h"

#include <sqlite3.h>
#include <assert.h>


using synthese::util::Conversion;
using synthese::db::SQLiteResult;
using boost::shared_ptr;

namespace synthese
{
	using namespace db;
	using namespace env;

	namespace util
	{
		template<> const std::string FactorableTemplate<SQLiteTableSync,CityTableSync>::FACTORY_KEY("15.20.01 Cities");
	}

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<CityTableSync>::TABLE_NAME = "t006_cities";
		template<> const int SQLiteTableSyncTemplate<CityTableSync>::TABLE_ID = 6;
		template<> const bool SQLiteTableSyncTemplate<CityTableSync>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteDirectTableSyncTemplate<CityTableSync,City>::load(City* object, const db::SQLiteResultSPtr& rows)
		{
		    object->setKey(rows->getLongLong (TABLE_COL_ID));
		    object->setName(rows->getText ( CityTableSync::TABLE_COL_NAME));
		    object->setCode(rows->getText ( CityTableSync::TABLE_COL_CODE));  
		}

		template<> void SQLiteDirectTableSyncTemplate<CityTableSync,City>::_link(City* obj, const SQLiteResultSPtr& rows, GetSource temporary)
		{
			if (temporary == GET_REGISTRY)
			{
				EnvModule::AddToCitiesMatchers(obj);
			}
		}

		template<> void SQLiteDirectTableSyncTemplate<CityTableSync,City>::_unlink(City* obj)
		{
			EnvModule::RemoveFromCitiesMatchers(obj);
		}

		template<> void SQLiteDirectTableSyncTemplate<CityTableSync,City>::save(City* object)
		{
			/// @todo Implement it
		}
	}

	namespace env
	{
		const std::string CityTableSync::TABLE_COL_NAME = "name";
		const std::string CityTableSync::TABLE_COL_CODE = "code";
		
		CityTableSync::CityTableSync ()
			: SQLiteAutoRegisterTableSyncTemplate<CityTableSync,City> ()
		{
			addTableColumn (TABLE_COL_ID, "INTEGER");
			addTableColumn (TABLE_COL_NAME, "TEXT");
			addTableColumn (TABLE_COL_CODE, "TEXT");

			addTableIndex(TABLE_COL_NAME);
			addTableIndex(TABLE_COL_CODE);
		}


		CityTableSync::~CityTableSync ()
		{

		}
	}
}
