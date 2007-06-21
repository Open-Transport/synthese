#ifndef SYNTHESE_DBRING_NODEINFO_H
#define SYNTHESE_DBRING_NODEINFO_H


#include <string>

#include <iostream>
#include <boost/date_time/posix_time/posix_time_types.hpp>




namespace synthese
{



namespace dbring
{

    typedef int NodeId;
    typedef int RingId;

    typedef signed long long int TokenClock;


    typedef enum {
        OUTRING = 0,    //!< Node is out of ring.
        ENTRING = 1,    //!< Node is entering ring.
        INSRING = 2    //!< Node is inside ring.
    } NodeState ;



class NodeInfo
{

private:
    
    NodeId _nodeId;
    RingId _ringId;
    std::string _host;    //!< Node IP address, or DNS name.
    int _port;            //!< Port to be used for replication messages.
    bool _authority;      //!< Is this node authority on a given ring ?
    NodeState _state;     //!< Node state.
    TokenClock _clock;    //!< Last received token clock
    boost::posix_time::ptime _lastPendingTimestamp;   // Last time the ring node was updated with a pending record.
    boost::posix_time::ptime _lastAcknowledgedTimestamp;   // Last time the ring node was updated with an ack record.


public:

    NodeInfo ();
 
    NodeInfo (const NodeId& nodeId, 
	      const RingId& ringId,
	      const std::string& host,
	      int port,
	      bool authority = false,
	      const NodeState& state = OUTRING,
	      const TokenClock& clock = -1,
	      const boost::posix_time::ptime& lastPendingTimestamp = boost::posix_time::min_date_time,
	      const boost::posix_time::ptime& lastAcknowledgedTimestamp = boost::posix_time::min_date_time);

    ~NodeInfo ();

    const NodeId& getNodeId () const { return _nodeId; }
    void setNodeId (const NodeId& nodeId) { _nodeId = nodeId; }

    const RingId& getRingId () const { return _ringId; }
    void setRingId (const RingId& ringId) { _ringId = ringId; }
    
    const std::string& getHost () const { return _host; }
    void setHost (const std::string& host) { _host = host; }

    int getPort () const { return _port; }
    void setPort (int port) { _port = port; }

    bool isAuthority () const { return _authority; }
    void setAuthority (bool authority) { _authority = authority; }

    const NodeState& getState () const { return _state; }
    void setState (const NodeState& state) { _state = state; }

    const TokenClock& getClock () const { return _clock; }
    void setClock (const TokenClock& clock) { _clock = clock; }
	      
    const boost::posix_time::ptime& getLastPendingTimestamp () const { return _lastPendingTimestamp; }
    void setLastPendingTimestamp (const boost::posix_time::ptime& lastPendingTimestamp) { _lastPendingTimestamp = lastPendingTimestamp; }
    
    const boost::posix_time::ptime& getLastAcknowledgedTimestamp () const { return _lastAcknowledgedTimestamp; }
    void setLastAcknowledgedTimestamp (const boost::posix_time::ptime& lastAcknowledgedTimestamp) { _lastAcknowledgedTimestamp = lastAcknowledgedTimestamp; }
    
private:

    friend std::ostream& operator<< ( std::ostream& os, const NodeInfo& op );
    friend std::istream& operator>> ( std::istream& is, NodeInfo& op );


};



std::ostream& operator<< ( std::ostream& os, const NodeInfo& op );
std::istream& operator>> ( std::istream& is, NodeInfo& op );

bool operator== (const NodeInfo& op1, const NodeInfo& op2);
bool operator!= (const NodeInfo& op1, const NodeInfo& op2);


}
}


#endif
