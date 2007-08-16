#ifndef SYNTHESE_ENVLSSQL_CROSSINGTABLESYNC_H
#define SYNTHESE_ENVLSSQL_CROSSINGTABLESYNC_H

#include "02_db/SQLiteTableSyncTemplate.h"

#include <string>
#include <iostream>

namespace synthese
{
	namespace env
	{
		class Crossing;

		/** Crossing SQLite table synchronizer.
		    The memory registry for this table is connection places registry.
		    Crossing are just a special kind of connection place (road-road) that we want 
		    to keep in a separate table.

		    @ingroup m15
		    
		    Connection places table :
		    - on insert : 
		    - on update : 
		    - on delete : X
		*/
		class CrossingTableSync : public db::SQLiteTableSyncTemplate<Crossing>
		{
		public:
			static const std::string TABLE_COL_CITYID;

			CrossingTableSync ();
			~CrossingTableSync ();

			void rowsAdded (synthese::db::SQLiteQueueThreadExec* sqlite, 
				synthese::db::SQLiteSync* sync,
				const synthese::db::SQLiteResultSPtr& rows, bool isFirstSync = false);

			void rowsUpdated (synthese::db::SQLiteQueueThreadExec* sqlite, 
				synthese::db::SQLiteSync* sync,
				const synthese::db::SQLiteResultSPtr& rows);

			void rowsRemoved (synthese::db::SQLiteQueueThreadExec* sqlite, 
				synthese::db::SQLiteSync* sync,
				const synthese::db::SQLiteResultSPtr& rows);
		};
	}
}
#endif
