#ifndef SYNTHESE_DBRING_UPDATERECORDTABLESYNC_H
#define SYNTHESE_DBRING_UPDATERECORDTABLESYNC_H


#include "02_db/SQLiteTableSyncTemplate.h"
#include "03_db_ring/UpdateLog.h"
#include "03_db_ring/UpdateRecord.h"

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace synthese
{

namespace dbring
{




class UpdateRecordTableSync : public db::SQLiteTableSyncTemplate<UpdateRecord>
{


public:

    static const std::string TABLE_COL_TIMESTAMP;
    static const std::string TABLE_COL_EMITTERNODEID;
    static const std::string TABLE_COL_STATE;
    static const std::string TABLE_COL_SQL;

    UpdateRecordTableSync ();
    ~UpdateRecordTableSync ();


    virtual void rowsAdded (db::SQLite* sqlite, 
			    db::SQLiteSync* sync,
			    const db::SQLiteResultSPtr& rows, bool isFirstSync = false);

    virtual void rowsUpdated (db::SQLite* sqlite, 
			      db::SQLiteSync* sync,
			      const db::SQLiteResultSPtr& rows);

    virtual void rowsRemoved (db::SQLite* sqlite, 
			      db::SQLiteSync* sync,
			      const db::SQLiteResultSPtr& rows);

    static void loadAllAfterTimestamp (UpdateLogSPtr dest, 
				       const boost::posix_time::ptime& timestamp,
				       bool inclusive = false);

    static boost::posix_time::ptime getLastPendingTimestamp ();

    static long getLastUpdateIndex (NodeId nodeId);

private:

};



}
}



#endif



