#ifndef SYNTHESE_DBRING_NODE_H
#define SYNTHESE_DBRING_NODE_H

#include "01_util/threads/ThreadExec.h"

#include "02_db/SQLiteHandle.h"

#include "03_db_ring/RingNode.h"
#include "03_db_ring/UpdateLog.h"

#include <boost/thread/recursive_mutex.hpp>
#include <boost/shared_ptr.hpp>

#include <map>
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
class Node : public db::SQLiteHandle, public util::ThreadExec 
{
private:
    
    const NodeId _id;
    RingNodes _ringNodes;  //!< Local tokens (one per ring). Info that this node knows locally.
    std::map<int, tcp::TcpService*> _tcpServices;
    UpdateLogSPtr _updateLog;

    long _lastUpdateIndex;
    
    boost::shared_ptr<boost::recursive_mutex> _ringNodesMutex; 
    boost::shared_ptr<boost::recursive_mutex> _updateLogMutex; 


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

    db::SQLiteResultSPtr execQuery (const db::SQLiteStatementSPtr& statement, bool lazy = true);
    db::SQLiteResultSPtr execQuery (const db::SQLData& sql, bool lazy = true) ;

    void execUpdate (const db::SQLiteStatementSPtr& statement, bool asynchronous = false) ;
    void execUpdate (const db::SQLData& sql, bool asynchronous = false); 

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

