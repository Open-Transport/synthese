
/** SQLiteSync class implementation.
	@file SQLiteSync.cpp

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

#include <sqlite3.h>
#include <assert.h>
#include <boost/foreach.hpp>

#include "Conversion.h"
#include "Log.h"
#include "Factory.h"
#include "02_db/Constants.h"
#include "SQLiteHandle.h"
#include "SQLiteTableSync.h"
#include "SQLiteCachedResult.h"
#include "SQLiteException.h"
#include "SQLiteSync.h"
#include "DBModule.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;

	namespace db
	{


		SQLiteSync::SQLiteSync ()
		: _isRegistered (false)
		{

		}



		SQLiteSync::~SQLiteSync ()
		{
		}
	    


		void 
		SQLiteSync::registerCallback (SQLiteHandle* emitter)
		{
			boost::recursive_mutex::scoped_lock lock (_tableSynchronizersMutex);

			// Call the update schema step on all synchronizers.
			for(Factory<SQLiteTableSync>::Iterator it(Factory<SQLiteTableSync>::begin());
				it != Factory<SQLiteTableSync>::end();
				++it
			){
				Log::GetInstance().info("Updating schema for table " + it.getKey());
			    try 
			    {
					it->updateSchema (emitter);
			    }
			    catch (std::exception& e)
			    {
				Log::GetInstance().error ("Error during schema update of " + it.getKey() + ".", e);
			    }
			}

			_isRegistered = true;


			for(Factory<SQLiteTableSync>::Iterator it(Factory<SQLiteTableSync>::begin());
				it != Factory<SQLiteTableSync>::end();
				++it
			){
				it->initAutoIncrement ();
			}
			
			// Call the first sync step on all synchronizers.
			for(Factory<SQLiteTableSync>::Iterator it(Factory<SQLiteTableSync>::begin());
				it != Factory<SQLiteTableSync>::end();
				++it
			){
			    Log::GetInstance().info("Loading table " + it.getKey());
			    try 
			    {
					it->firstSync (emitter, this);
			    }
			    catch (std::exception& e)
			    {
					Log::GetInstance().error ("Unattended error during first sync of " + it.getKey() + 
							  ". In-memory data might be inconsistent.", e);
			    }
			}
			
		}
		 


		void SQLiteSync::eventCallback(
			SQLiteHandle* emitter,
			const SQLiteEvent& event
		){
			boost::recursive_mutex::scoped_lock lock (_tableSynchronizersMutex);
			if (_isRegistered == false) return;

			shared_ptr<SQLiteTableSync> tableSync(DBModule::GetTableSync(event.tbName));

		    if (event.opType == SQLITE_INSERT) 
		    {
				tableSync->rowsAdded (emitter, this, tableSync->getRowById (emitter, event.rowId));
		    }
		    else if (event.opType == SQLITE_UPDATE) 
		    {
				// Query for the modified row
				tableSync->rowsUpdated (emitter, this, tableSync->getRowById (emitter, event.rowId));
		    }
		    else if (event.opType == SQLITE_DELETE) 
		    {
				std::vector<std::string> columnNames;
				columnNames.push_back (TABLE_COL_ID);
				SQLiteCachedResult* cachedResult = new SQLiteCachedResult (columnNames);
				
				SQLiteResultRow values;
				values.push_back (new SQLiteValue (Conversion::ToString (event.rowId)));
								cachedResult->addRow (values);
				
				tableSync->rowsRemoved (emitter, this, SQLiteResultSPtr (cachedResult));
		    }
		}
	}
}
