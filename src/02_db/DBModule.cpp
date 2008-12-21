
/** DBModule class implementation.
	@file DBModule.cpp

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

#include "DBModule.h"
#include "SQLiteSync.h"
#include "SQLiteTableSync.h"
#include "SQLiteHandle.h"
#include "SQLiteException.h"
#include "Conversion.h"
#include "Log.h"
#include "Factory.h"
#include "TcpService.h"

#include <iostream>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;

	template<> const string util::FactorableTemplate<db::DbModuleClass, db::DBModule>::FACTORY_KEY("02_db");
	
	namespace db
	{
		map<string,string>	DBModule::_tableSyncMap;

	    SQLiteHandle* DBModule::_sqlite = 0;
		DBModule::SubClassMap DBModule::_subClassMap;


	    void DBModule::preInit ()
	    {
	    }



	    void DBModule::initialize()
	    {
			_sqlite = new SQLiteHandle (GetDatabasePath ());

			Log::GetInstance ().info ("Using lib SQLite version " + SQLite::GetLibVersion ());
			
			bool autorespawn (true);

			SQLiteSync* syncHook = new SQLiteSync ();
			_sqlite->registerUpdateHook(syncHook);

			_tableSyncMap.clear();
			for(Factory<SQLiteTableSync>::Iterator it(Factory<SQLiteTableSync>::begin()); it != Factory<SQLiteTableSync>::end(); ++it)
			{
				_tableSyncMap[it->getFormat().NAME] = it.getKey();
			}
	    }
	    

		boost::shared_ptr<SQLiteTableSync> DBModule::GetTableSync(const std::string& tableName)
		{
			map<string,string>::const_iterator it(_tableSyncMap.find(tableName));
			if (it == _tableSyncMap.end())
			{
				throw SQLiteException("Table not found in database");
			}
			return shared_ptr<SQLiteTableSync>(Factory<SQLiteTableSync>::create(it->second));
		}
	    

	    SQLite* DBModule::GetSQLite ()
	    {
			return _sqlite;
	    }


	    void 
		DBModule::ParameterCallback (const string& name, 
						 const string& value)
	    {
	    }

		void DBModule::AddSubClass( uid id, const string& subclass)
		{
			_subClassMap[id] = subclass;
		}

		string DBModule::GetSubClass(uid id )
		{
			SubClassMap::const_iterator it(_subClassMap.find(id));
			return (it == _subClassMap.end()) ? string() : it->second;
		}

		string DBModule::getName() const
		{
			return "Base de données SQLite";
		}

	}
}


