#ifndef SYNTHESE_DBRING_NODEINFOTABLESYNC_H
#define SYNTHESE_DBRING_NODEINFOTABLESYNC_H


#include "02_db/SQLiteTableSyncTemplate.h"
#include "03_db_ring/Token.h"


namespace synthese
{

namespace dbring
{




class NodeInfoTableSync : public db::SQLiteTableSyncTemplate<NodeInfo>
{


public:

    static const std::string TABLE_COL_NODEID;
    static const std::string TABLE_COL_RINGID;
    static const std::string TABLE_COL_HOST;
    static const std::string TABLE_COL_PORT;
    static const std::string TABLE_COL_AUTH;
    static const std::string TABLE_COL_STATE;
    static const std::string TABLE_COL_CLOCK;
    static const std::string TABLE_COL_LASTPENDINGTIMESTAMP;
    static const std::string TABLE_COL_LASTACKNOWLEDGEDTIMESTAMP;

    NodeInfoTableSync ();
    ~NodeInfoTableSync ();


    virtual void rowsAdded (db::SQLiteQueueThreadExec* sqlite, 
			    db::SQLiteSync* sync,
			    const db::SQLiteResult& rows, bool isFirstSync = false);

    virtual void rowsUpdated (db::SQLiteQueueThreadExec* sqlite, 
			      db::SQLiteSync* sync, 
			      const db::SQLiteResult& rows);

    virtual void rowsRemoved (db::SQLiteQueueThreadExec* sqlite, 
			      db::SQLiteSync* sync,
			      const db::SQLiteResult& rows);


};



}
}



#endif



