
#include <sqlite/sqlite3.h>
#include <assert.h>

#include "01_util/Conversion.h"
// #include "01_util/Factory.h"

#include "02_db/SQLiteQueueThreadExec.h"
#include "02_db/SQLiteTableSync.h"
#include "02_db/SQLiteException.h"
#include "02_db/SQLiteSync.h"

using namespace std;

namespace synthese
{
	using namespace util;

	namespace db
	{


		SQLiteSync::SQLiteSync (const std::string& idColumnName)
		: _idColumnName (idColumnName)
		, _isRegistered (false)
		{

		}



		SQLiteSync::~SQLiteSync ()
		{
		}

		 

		void 
		SQLiteSync::addTableSynchronizer (const string& rank, SQLiteTableSync* synchronizer)
		{
			boost::recursive_mutex::scoped_lock lock (_tableSynchronizersMutex);

			// assert (synchronizer->getTableFormat ().empty () == false);
			_tableSynchronizers.insert (std::make_pair (synchronizer->getTableName (), synchronizer));
			_rankedTableSynchronizers.insert(make_pair(rank, synchronizer));
		}



		bool 
		SQLiteSync::hasTableSynchronizer (const std::string& tableName) const
		{
			boost::recursive_mutex::scoped_lock lock (_tableSynchronizersMutex);
			return _tableSynchronizers.find (tableName) != _tableSynchronizers.end ();
		}


		   
		SQLiteTableSync* 
		SQLiteSync::getTableSynchronizer (const std::string& tableName) const
		{
			boost::recursive_mutex::scoped_lock lock (_tableSynchronizersMutex);
			if (hasTableSynchronizer (tableName) == false)
			{
			throw SQLiteException ("No synchronizer for table '" + tableName + "'");
			}
			return _tableSynchronizers.find (tableName)->second;
		}

	    
	    
	    std::map<std::string, SQLiteTableSync* >
	    SQLiteSync::getTableSynchronizers () const
	    {
		boost::recursive_mutex::scoped_lock lock (_tableSynchronizersMutex);
		return _tableSynchronizers;
	    }
	    


		void 
		SQLiteSync::registerCallback (const SQLiteQueueThreadExec* emitter)
		{
			boost::recursive_mutex::scoped_lock lock (_tableSynchronizersMutex);

			_isRegistered = true;
			
			// Call the init sequence on all synchronizers.
			for (std::map<std::string, SQLiteTableSync*>::const_iterator it = 
				 _rankedTableSynchronizers.begin (); 
			     it != _rankedTableSynchronizers.end (); ++it)
			{
			    it->second->firstSync (emitter, this);
			}

		}
		 


		   
		void 
		SQLiteSync::eventCallback (const SQLiteQueueThreadExec* emitter,
					const SQLiteEvent& event)
		{
			boost::recursive_mutex::scoped_lock lock (_tableSynchronizersMutex);

			for (std::map<std::string, SQLiteTableSync* >::const_iterator it 
				 = _tableSynchronizers.begin ();
			     it != _tableSynchronizers.end (); ++it)
			{
			    SQLiteTableSync* tableSync = it->second;
			    if (tableSync->getTableName () != event.tbName) continue;
			    
			    if (event.opType == SQLITE_INSERT) 
			    {
				// Query for the modified row
				SQLiteResult result = emitter->execQuery ("SELECT * FROM " + event.tbName + " WHERE " 
									  + _idColumnName + "=" + Conversion::ToString (event.rowId));
				
				tableSync->rowsAdded (emitter, this, result);
			    }
			    else if (event.opType == SQLITE_UPDATE) 
			    {
				// Query for the modified row
				SQLiteResult result = emitter->execQuery ("SELECT * FROM " + event.tbName + " WHERE " 
									  + _idColumnName + "=" + Conversion::ToString (event.rowId));
				
				tableSync->rowsUpdated (emitter, this, result);
			    }
			    else if (event.opType == SQLITE_DELETE) 
			    {
				// Query for the modified row
				SQLiteResult result;
				std::vector<std::string> columns;
				std::vector<std::string> values;
				columns.push_back (_idColumnName);
				values.push_back (Conversion::ToString (event.rowId));
				result.addRow (values, columns);
				
				tableSync->rowsRemoved (emitter, this, result);
			    }
			}
			
		}
	    
	}
}

