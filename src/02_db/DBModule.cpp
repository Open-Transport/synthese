
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

#include <iostream>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace db;
	using namespace server;

	template<> const string util::FactorableTemplate<ModuleClass,DBModule>::FACTORY_KEY("02_db");
	
	namespace db
	{
		filesystem::path DBModule::_DatabasePath;

		map<string,string>	DBModule::_tableSyncMap;

		SQLiteHandle* DBModule::_sqlite = 0;
		DBModule::SubClassMap DBModule::_subClassMap;
	}

	namespace server
	{
		template<> const string ModuleClassTemplate<DBModule>::NAME("Base de données SQLite");
		
		
		
		template<> void ModuleClassTemplate<DBModule>::PreInit ()
		{
		}



		template<> void ModuleClassTemplate<DBModule>::Init()
		{
			DBModule::_sqlite = new SQLiteHandle(DBModule::GetDatabasePath ());

			Log::GetInstance ().info ("Using lib SQLite version " + SQLite::GetLibVersion ());
			
			bool autorespawn (true);

			SQLiteSync* syncHook = new SQLiteSync ();
			DBModule::_sqlite->registerUpdateHook(syncHook);

			DBModule::_tableSyncMap.clear();
			vector<shared_ptr<SQLiteTableSync> > tableSyncs(Factory<SQLiteTableSync>::GetNewCollection());
			BOOST_FOREACH(const shared_ptr<SQLiteTableSync>& sync, tableSyncs)
			{
				DBModule::_tableSyncMap[sync->getFormat().NAME] = sync->getFactoryKey();
			}
	    }
	
	
	
		template<> void ModuleClassTemplate<DBModule>::End()
		{
		}
	}
	
	namespace db
	{
	    const boost::filesystem::path& DBModule::GetDatabasePath ()
	    {
			return _DatabasePath;
	    }

		
		
	    void DBModule::SetDatabasePath(
	    	const boost::filesystem::path& databasePath
	    ){
			_DatabasePath = databasePath;
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

	}
}
