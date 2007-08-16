
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
#include "02_db/SQLiteQueueThreadExec.h"

#include "15_env/City.h"
#include "15_env/EnvModule.h"

#include <sqlite/sqlite3.h>
#include <assert.h>


using synthese::util::Conversion;
using synthese::db::SQLiteResult;
using boost::shared_ptr;

namespace synthese
{
	using namespace db;
	using namespace env;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<City>::TABLE_NAME = "t006_cities";
		template<> const int SQLiteTableSyncTemplate<City>::TABLE_ID = 6;
		template<> const bool SQLiteTableSyncTemplate<City>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<City>::load(City* object, const db::SQLiteResultSPtr& rows )
		{
		    object->setKey(rows->getLongLong (TABLE_COL_ID));
		    object->setName(rows->getText ( CityTableSync::TABLE_COL_NAME));
		    object->setCode(rows->getText ( CityTableSync::TABLE_COL_CODE));  
		}


		template<> void SQLiteTableSyncTemplate<City>::save(City* object)
		{
			/// @todo Implement
			/*			SQLiteHandle* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() > 0)
			{
			query
			<< "UPDATE " << TABLE_NAME << " SET "
			<< DisplayTypeTableSync::TABLE_COL_NAME << "=" << Conversion::ToSQLiteString(object->getName())
			<< "," << DisplayTypeTableSync::TABLE_COL_INTERFACE_ID << "="  << Conversion::ToString(object->getInterface()->getKey())
			<< "," << DisplayTypeTableSync::TABLE_COL_ROWS_NUMBER << "="  << Conversion::ToString(object->getRowNumber())
			<< " WHERE " << TABLE_COL_ID << "=" << Conversion::ToString(object->getKey());
			}
			else
			{
			object->setKey(getId());
			query
			<< " INSERT INTO " << TABLE_NAME << " VALUES("
			<< Conversion::ToString(object->getKey())
			<< "," << Conversion::ToSQLiteString(object->getName())
			<< "," << Conversion::ToString(object->getInterface()->getKey())
			<< "," << Conversion::ToString(object->getRowNumber())
			<< ")";
			}
			sqlite->execUpdate(query.str());
			*/		
		}
	}

	namespace env
	{
		const std::string CityTableSync::TABLE_COL_NAME = "name";
		const std::string CityTableSync::TABLE_COL_CODE = "code";
		
		CityTableSync::CityTableSync ()
			: SQLiteTableSyncTemplate<City> (true, false, db::TRIGGERS_ENABLED_CLAUSE)
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

		    
		void 
			CityTableSync::rowsAdded (synthese::db::SQLiteQueueThreadExec* sqlite, 
			synthese::db::SQLiteSync* sync,
			const synthese::db::SQLiteResultSPtr& rows, bool isFirstSync)
		{
		    while (rows->next ())
		    {
			uid id = rows->getLongLong (TABLE_COL_ID);
			
			if (EnvModule::getCities ().contains (id)) return;
			
			shared_ptr<City> city(new City);
			load (city.get (), rows);
			
			EnvModule::getCities ().add (city);
			EnvModule::getCitiesMatcher ().add (city->getName (), city->getKey ());
		    }
		}


		void 
			CityTableSync::rowsUpdated (synthese::db::SQLiteQueueThreadExec* sqlite, 
			synthese::db::SQLiteSync* sync,
			const synthese::db::SQLiteResultSPtr& rows)
		{
			while (rows->next ())
			{
			    uid id = rows->getLongLong (TABLE_COL_ID);
			    shared_ptr<City> city = EnvModule::getCities ().getUpdateable (id);
			    
			    EnvModule::getCitiesMatcher ().remove (city->getName ());
			    
			    load(city.get(), rows);
			    
			     EnvModule::getCitiesMatcher ().add (city->getName (), city->getKey ());
			}
		}



		void 
			CityTableSync::rowsRemoved (synthese::db::SQLiteQueueThreadExec* sqlite, 
			synthese::db::SQLiteSync* sync,
			const synthese::db::SQLiteResultSPtr& rows)
		{
			while (rows->next ())
			{
			    uid id =rows->getLongLong (TABLE_COL_ID);
			    
			    EnvModule::getCitiesMatcher ().remove (EnvModule::getCities ().get (id)->getName ());
			    EnvModule::getCities ().remove (id);
			}
		}

	}
}
