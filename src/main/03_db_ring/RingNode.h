#ifndef SYNTHESE_DBRING_RINGNODE_H
#define SYNTHESE_DBRING_RINGNODE_H

#include "03_db_ring/SendTokenThreadExec.h"
#include "03_db_ring/TransmissionStatusMap.h"

#include <boost/thread/recursive_mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>



namespace synthese
{



namespace dbring
{


/** Db ring node class.
    This class associates a physical node with a specific ring.
    It holds all node data relative to the associated ring.

@ingroup m03
*/
class RingNode  
{
private:

    static const boost::posix_time::time_duration RECV_TOKEN_TIMEOUT;
    static const int SEND_TOKEN_MAX_NB_TRIES;
    
    const TokenSPtr _data;  // State token (the one to be sent)
    TokenSPtr _token;   // Received token
    TransmissionStatusMap _transmissionStatusMap;

    boost::posix_time::ptime _timer;
 

protected:
    

public:
    
    RingNode (const NodeInfo& nodeInfo, 
	      UpdateLogSPtr& updateLog);

    ~RingNode ();

    /** Ring node initialzation. Called by Node initialization.
     */
    void initialize ();

    bool hasInfo (const NodeId& nodeId) const;
    NodeInfo getInfo (const NodeId& nodeId) const;
    NodeInfo getInfo () const;
    void setInfo (const NodeId& nodeId, const NodeInfo& nodeInfo);

    /* Returns whether or not this ring node is allowed
       to gain write access to db (only regarding this ring, not globally).
       This is true only if :
         - authority node is connected to ring
	 - token clock is equal to authority clock (node is up to date).
    */
    bool canWrite () const;

    void execUpdate (const std::string& sql, const boost::posix_time::ptime& timestamp);
    
    boost::posix_time::ptime getLastPendingTimestamp () const;
    void setLastPendingTimestamp (const boost::posix_time::ptime& lastPendingTimestamp);

    boost::posix_time::ptime getLastAcknowledgedTimestamp () const;
    void setLastAcknowledgedTimestamp (const boost::posix_time::ptime& lastAcknowledgedTimestamp);

    void setModified (bool modified);
    
    void loop (const TokenSPtr& token);

    void dump ();

 protected:

    virtual bool timedOut () const;
    
    virtual void sendToken ();

 private:

    void resetTimer ();


};


typedef boost::shared_ptr<RingNode> RingNodeSPtr;




}

}
#endif

