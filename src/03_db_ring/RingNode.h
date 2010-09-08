#ifndef SYNTHESE_DBRING_RINGNODE_H
#define SYNTHESE_DBRING_RINGNODE_H

#include "03_db_ring/TransmissionStatusMap.h"
#include "03_db_ring/NodeInfo.h"

#include "01_util/threads/ThreadExec.h"

#include <boost/thread/recursive_mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <vector>


namespace synthese
{


namespace tcp
{
    class TcpServerSocket;
}




namespace dbring
{


/** Db ring node class.
    This class associates a physical node with a specific ring.
    It holds all node data relative to the associated ring.

@ingroup m101
*/
    class RingNode : public util::ThreadExec
{
private:

    static const boost::posix_time::time_duration UNREACHABLE_NODES_RETRY_TIMEOUT;

    const NodeId _nodeId;
    const RingId _ringId;

    NodeInfoMap _infos;

    TransmissionStatusMap _clientStatusMap;
    TransmissionStatusMap _serverStatusMap;

    boost::posix_time::ptime _unreachableNodesRetryTimer;

    boost::shared_ptr<boost::recursive_mutex> _infosMutex; 
    boost::shared_ptr<boost::recursive_mutex> _lastAcknowledgedMutex; 


protected:
    

public:
    
    RingNode (const NodeInfo& nodeInfo);

    ~RingNode ();

    /** Ring node initialzation. Called by Node initialization.
     */
    void initialize ();

    bool hasInfo (const NodeId& nodeId) const;
    NodeInfo getInfo (const NodeId& nodeId) const;
    NodeInfo getInfo () const;
    void setInfo (const NodeInfo& nodeInfo);

    
    void serverLoop (NodeInfo clientNodeInfo, int port, tcp::TcpServerSocket* serverSocket);
    void clientLoop ();

    void loop ();

 protected:

    
 private:

    std::vector<NodeId> getNodesAfter (const NodeId& id) const;

    bool unreachableNodesRetryTimedOut () const;
    void resetUnreachableNodesRetryTimer ();


};


typedef boost::shared_ptr<RingNode> RingNodeSPtr;




}

}
#endif

