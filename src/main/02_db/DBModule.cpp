
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

#include "02_db/DBModule.h"
#include "02_db/SQLiteSync.h"
#include "02_db/SQLiteTableSync.h"
#include "02_db/SQLiteHandle.h"

#include "01_util/Conversion.h"
#include "01_util/Log.h"
#include "01_util/Factory.h"

#include "00_tcp/TcpService.h"


#include <iostream>

namespace synthese
{
	using namespace util;

	template<> const std::string util::FactorableTemplate<db::DbModuleClass, db::DBModule>::FACTORY_KEY("02_db");
	
	namespace db
	{

	    SQLiteHandle* DBModule::_sqlite = 0;


	    void DBModule::preInit ()
	    {
	    }



	    void DBModule::initialize()
	    {
		
		_sqlite = new SQLiteHandle (GetDatabasePath ());
		
		bool autorespawn (true);

		SQLiteSync* syncHook = new SQLiteSync ();
		
		// Register all table syncs
		for (Factory<SQLiteTableSync>::Iterator it = 
			 Factory<SQLiteTableSync>::begin(); 
		     it != Factory<SQLiteTableSync>::end(); 
		     ++it)
		{
		    syncHook->addTableSynchronizer(it.getKey(), *it);
		}
		
		_sqlite->registerUpdateHook (syncHook);

	    }
	    
	    

	    SQLite*
	    DBModule::GetSQLite ()
	    {
		return _sqlite;
	    }


	    void 
		DBModule::ParameterCallback (const std::string& name, 
						 const std::string& value)
	    {
	    }

	}
}


