#ifndef SYNTHESE_DBRING_NODE_H
#define SYNTHESE_DBRING_NODE_H

#include "01_util/threads/ThreadExec.h"
#include "01_util/concurrent/SynchronizedQueue.h"

#include "02_db/SQLite.h"

#include "03_db_ring/RingNode.h"
#include "03_db_ring/UpdateLog.h"

#include <boost/thread/recursive_mutex.hpp>
#include <boost/shared_ptr.hpp>

#include <map>
#include <set>
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
    RingNodes _ringNodes;  //!< Local tokens (one per ring). Info that this node knows locally.
    std::set<int> _listenPorts; //!< Ports this node listen to.

    UpdateLogSPtr _updateLog;

    util::SynchronizedQueue<TokenSPtr> _receivedTokens;  //!< Token reception queue.

    long _lastUpdateIndex;
    
    boost::shared_ptr<boost::recursive_mutex> _ringNodesMutex; 
    boost::shared_ptr<boost::recursive_mutex> _updateLogMutex; 

	sqlite3* getHandle(void) const;

protected:


 public:

    Node (const NodeId& id);

    ~Node ();

public:

    sqlite3* getHandle ();

    const NodeId& getId () const;

    bool hasInfo (const NodeId& nodeId, const RingId& ringId) const;
    NodeInfo getInfo (const NodeId& nodeId, const RingId& ringId) const;

    bool hasInfo (const RingId& ringId) const;
    NodeInfo getInfo (const RingId& ringId) const;

    /* Returns whether or not this node is allowed
       to gain global write access to db. 
       If this node is authority, it must have write access on all
       rings on which it is connected.
    */
    bool canWrite () const;

    db::SQLiteResultSPtr execQuery (const db::SQLiteStatementSPtr& statement, bool lazy = false);
    db::SQLiteResultSPtr execQuery (const db::SQLData& sql, bool lazy = false) ;

    db::SQLiteStatementSPtr compileStatement(const db::SQLData&);

    void execUpdate (const db::SQLiteStatementSPtr& statement) ;
    void execUpdate (const db::SQLData& sql); 

    void initialize ();
    void loop ();
    void finalize ();

    boost::posix_time::ptime getLastPendingTimestamp () const;
    boost::posix_time::ptime getLastAcknowledgedTimestamp () const;

    void flushUpdates ();

    void dump ();

private:

    bool isMasterAuthority () const;
    
    uid encodeUpdateKey (NodeId nodeId, long updateId);

    void filterUpdateLog (const TokenSPtr& token);
    void mergeUpdateLog (const TokenSPtr& token);
    void saveUpdateRecord (const UpdateRecordSPtr& urp);


    void setNodeInfoCallback (const NodeInfo& nodeInfo);
    void setUpdateRecordCallback (const UpdateRecordSPtr& updateRecord);
    
    friend class NodeInfoTableSync;
    friend class UpdateRecordTableSync;

};




}

}
#endif

