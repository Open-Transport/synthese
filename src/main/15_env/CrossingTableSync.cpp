#include "CrossingTableSync.h"

#include "01_util/Conversion.h"

#include "15_env/City.h"
#include "15_env/Crossing.h"
#include "15_env/EnvModule.h"

#include <assert.h>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace env;
	using namespace util;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<Crossing>::TABLE_NAME = "t043_crossings";
		template<> const int SQLiteTableSyncTemplate<Crossing>::TABLE_ID = 43;
		template<> const bool SQLiteTableSyncTemplate<Crossing>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<Crossing>::load(Crossing* crossing, const db::SQLiteResultSPtr& rows )
		{
			uid id (rows->getLongLong (TABLE_COL_ID));
			uid cityId (rows->getLongLong (CrossingTableSync::TABLE_COL_CITYID));
			
			crossing->setKey(id);
			crossing->setCity(EnvModule::getCities().get(cityId).get());
		}
	}


	namespace env
	{
		const std::string CrossingTableSync::TABLE_COL_CITYID = "city_id";

		CrossingTableSync::CrossingTableSync ()
		: SQLiteTableSyncTemplate<Crossing> (true, false, db::TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn (TABLE_COL_ID, "INTEGER", true);
			addTableColumn (TABLE_COL_CITYID, "INTEGER", false);

			addTableIndex(TABLE_COL_CITYID);
		}



		CrossingTableSync::~CrossingTableSync ()
		{

		}

		    
		void 
			CrossingTableSync::rowsAdded (synthese::db::SQLiteQueueThreadExec* sqlite, 
			synthese::db::SQLiteSync* sync,
			const synthese::db::SQLiteResultSPtr& rows, bool isFirstSync)
		{
			while (rows->next ())
			{
			    uid id (rows->getLongLong (TABLE_COL_ID));
			    
			    if (EnvModule::getConnectionPlaces ().contains (id)) return;
			    
			    uid cityId (rows->getLongLong (TABLE_COL_CITYID));

			    shared_ptr<const City> city = EnvModule::getCities ().get (cityId);
			    
			    shared_ptr<Crossing> crossing(new synthese::env::Crossing (id, city.get()));

			    // Add crossing to connection place registry but not in city lexical matcher...
			    EnvModule::getConnectionPlaces ().add (crossing);
			}
		}



		void 
			CrossingTableSync::rowsUpdated (synthese::db::SQLiteQueueThreadExec* sqlite, 
			synthese::db::SQLiteSync* sync,
			const synthese::db::SQLiteResultSPtr& rows)
		{
			while (rows->next ())
			{
				shared_ptr<ConnectionPlace> crossing = EnvModule::getConnectionPlaces ().getUpdateable(
				    rows->getLongLong (TABLE_COL_ID));
				
				load(dynamic_cast<Crossing*>(crossing.get()), rows);
			}
		}




		void 
			CrossingTableSync::rowsRemoved (synthese::db::SQLiteQueueThreadExec* sqlite, 
			synthese::db::SQLiteSync* sync,
			const synthese::db::SQLiteResultSPtr& rows)
		{
			while (rows->next ())
			{
			    uid id = rows->getLongLong (TABLE_COL_ID);
			    
			    EnvModule::getConnectionPlaces ().remove (id);
			}
		}
	    

	}
}
