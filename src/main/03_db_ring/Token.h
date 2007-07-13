#ifndef SYNTHESE_DBRING_TOKEN_H
#define SYNTHESE_DBRING_TOKEN_H

#include "03_db_ring/NodeInfo.h"
#include "03_db_ring/UpdateLog.h"

#include <map>
#include <vector>
#include <string>
#include <iostream>

#include <boost/thread/recursive_mutex.hpp>
#include <boost/shared_ptr.hpp>


namespace synthese
{



namespace dbring
{

    typedef std::map<NodeId, NodeInfo> NodeInfoMap;


/** Token class.

@ingroup m03
*/

class Token;

typedef boost::shared_ptr<Token> TokenSPtr;


class Token
{
 private:

    NodeId _emitterNodeId;
    NodeId _authorityNodeId;
    RingId _emitterRingId;
    NodeInfoMap _infos;
    bool _modified;

    UpdateLogSPtr _updateLog;

    boost::shared_ptr<boost::recursive_mutex> _infosMutex; 
    boost::shared_ptr<boost::recursive_mutex> _modifiedMutex; 
    boost::shared_ptr<boost::recursive_mutex> _authorityMutex; 

    Token (const Token&);

 public:

    Token ();

    Token (const NodeId& emitterNodeId,
	   const RingId& emitterRingId,
	   UpdateLogSPtr& updateLog
	);

    ~Token ();

    /* Gets emitter node id.
     */
    NodeId getEmitterNodeId () const;

    NodeId getAuthorityNodeId () const;

    /* Gets emitter node's ring id.
     */
    RingId getEmitterRingId () const;

    /* Returns true if node info map contains at least two entries
     *  (self + another one).
     */
    bool hasRecipient () const; 

    bool hasInfo (const NodeId& nodeId) const;
    NodeInfo getInfo (const NodeId& nodeId) const;
    NodeInfo getInfo () const;

    NodeState getAuthorityState () const;

    void saveInfo (const NodeId& nodeId, NodeInfo info);
    void setInfo (const NodeId& nodeId, const NodeInfo& info);

    void setState (const NodeId& nodeId, const NodeState& state);
    void setState (const NodeState& state);

    bool isModified () const;
    void setModified (bool modified);
    
    std::vector<NodeId> 
	getNodesAfter (const NodeId& id) const;

    void merge (const TokenSPtr& token);

    void setClock (const TokenClock& clock);

    boost::posix_time::ptime getLastPendingTimestamp (const NodeId& nodeId) const;
    boost::posix_time::ptime getLastPendingTimestamp () const;
    void setLastPendingTimestamp (const NodeId& nodeId, const boost::posix_time::ptime& lastPendingTimestamp);
    void setLastPendingTimestamp (const boost::posix_time::ptime& lastPendingTimestamp);

    boost::posix_time::ptime getLastAcknowledgedTimestamp (const NodeId& nodeId) const;
    boost::posix_time::ptime getLastAcknowledgedTimestamp () const;
    void setLastAcknowledgedTimestamp (const NodeId& nodeId, const boost::posix_time::ptime& lastAcknowledgedTimestamp);
    void setLastAcknowledgedTimestamp (const boost::posix_time::ptime& lastAcknowledgedTimestamp);

    void prepareUpdateLog (const boost::posix_time::ptime& fromTimestamp);
    UpdateLogSPtr getUpdateLog () const { return _updateLog; }


 private:

    void setClock (const NodeId& nodeId, const TokenClock& clock);

    friend std::ostream& operator<< ( std::ostream& os, const Token& op );
    friend std::istream& operator>> ( std::istream& is, Token& op );

};



std::ostream& operator<< ( std::ostream& os, const Token& op );
std::istream& operator>> ( std::istream& is, Token& op );

//std::ostream& operator<< ( std::ostream& os, const NodeInfoMap& op );
//std::istream& operator>> ( std::istream& is, NodeInfoMap& op );


}

}
#endif

