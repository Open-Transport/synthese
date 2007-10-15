#ifndef SYNTHESE_DBRING_NODE_H
#define SYNTHESE_DBRING_NODE_H

#include "01_util/threads/ThreadExec.h"

#include "02_db/SQLite.h"

#include "03_db_ring/RingNode.h"
#include "03_db_ring/UpdateRecord.h"

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/shared_ptr.hpp>

#include <map>
#include <set>
#include <vector>
#include <string>
#include <iostream>


namespace synthese
{


namespace tcp
{
    class TcpService;
}


namespace dbring
{
    
    typedef std::map<RingId, RingNodeSPtr> RingNodes;
    


/** Db ring node class.

@ingroup m03
*/
class Node : public db::SQLite, public util::ThreadExec 
{
private:
    
    const NodeId _id;
    bool _isAuthority;

    RingNodes _ringNodes;

    std::set<uid> _pendingUpdateRecords;

    int _lastUpdateIndex;

    boost::posix_time::ptime _lastAcknowledgedTimestamp;   //!< Last acknowledged update record timestamp.

    boost::shared_ptr<boost::recursive_mutex> _ringNodesMutex; 
    boost::shared_ptr<boost::recursive_mutex> _lastAcknowledgedTimestampMutex; 
    boost::shared_ptr<boost::recursive_mutex> _pendingUpdateRecordsMutex; 

	sqlite3* getHandle(void) const;

protected:


 public:

    Node (const NodeId& id, bool isAuthority);

    ~Node ();

public:

    const NodeId& getId () const;

    bool canWrite () const;

    db::SQLiteResultSPtr execQuery (const db::SQLiteStatementSPtr& statement, bool lazy = false);
    db::SQLiteResultSPtr execQuery (const db::SQLData& sql, bool lazy = false) ;

    db::SQLiteStatementSPtr compileStatement(const db::SQLData&);

    void execUpdate (const db::SQLiteStatementSPtr& statement) ;
    void execUpdate (const db::SQLData& sql); 

    void initialize ();
    void loop ();
    void finalize ();

    boost::posix_time::ptime getLastAcknowledgedTimestamp () const;

    std::set<uid> getPendingUpdateRecords () const;

    void saveNodeInfo (NodeInfo info);
    void saveUpdateRecord (const UpdateRecordSPtr& urp);


private:


    bool isAuthority () const;
    
    uid encodeUpdateKey (NodeId nodeId, long updateId);

    void setNodeInfoCallback (const NodeInfo& nodeInfo);
    void setUpdateRecordCallback (const UpdateRecordSPtr& updateRecord);

    friend class NodeInfoTableSync;
    friend class UpdateRecordTableSync;

};




}

}
#endif

