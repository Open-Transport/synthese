#ifndef SYNTHESE_DBRING_NODEINFO_H
#define SYNTHESE_DBRING_NODEINFO_H


#include <string>
#include <map>

#include <iostream>
#include <boost/date_time/posix_time/posix_time_types.hpp>




namespace synthese
{



namespace dbring
{

    typedef int NodeId;
    typedef int RingId;



class NodeInfo
{

private:
    
    NodeId _nodeId;
    RingId _ringId;
    std::string _host;    //!< Node IP address, or DNS name.
    int _port;            //!< Port to be used for replication messages.

public:

    NodeInfo ();
 
    NodeInfo (const NodeId& nodeId, 
	      const RingId& ringId,
	      const std::string& host,
	      int port);

    ~NodeInfo ();

    const NodeId& getNodeId () const { return _nodeId; }
    void setNodeId (const NodeId& nodeId) { _nodeId = nodeId; }

    const RingId& getRingId () const { return _ringId; }
    void setRingId (const RingId& ringId) { _ringId = ringId; }
    
    const std::string& getHost () const { return _host; }
    void setHost (const std::string& host) { _host = host; }

    int getPort () const { return _port; }
    void setPort (int port) { _port = port; }

    
private:

    friend std::ostream& operator<< ( std::ostream& os, const NodeInfo& op );
    friend std::istream& operator>> ( std::istream& is, NodeInfo& op );


};


    typedef std::map<NodeId, NodeInfo> NodeInfoMap;



std::ostream& operator<< ( std::ostream& os, const NodeInfo& op );
std::istream& operator>> ( std::istream& is, NodeInfo& op );

bool operator== (const NodeInfo& op1, const NodeInfo& op2);
bool operator!= (const NodeInfo& op1, const NodeInfo& op2);


}
}


#endif
