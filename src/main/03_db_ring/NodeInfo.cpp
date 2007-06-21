#include "03_db_ring/NodeInfo.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/date_time/posix_time/time_parsers.hpp>




using namespace boost::posix_time;



namespace synthese
{

namespace dbring
{



NodeInfo::NodeInfo ()
    : _nodeId (-1)
    , _ringId (-1)
    , _host ("")
    , _port (-1)
    , _authority (false)
    , _state (OUTRING)
    , _clock (-1)
    , _lastPendingTimestamp (min_date_time)
    , _lastAcknowledgedTimestamp (min_date_time)
{

}
 



NodeInfo::NodeInfo (const NodeId& nodeId, 
		    const RingId& ringId,
		    const std::string& host,
		    int port,
		    bool authority,
		    const NodeState& state,
		    const TokenClock& clock,
		    const boost::posix_time::ptime& lastPendingTimestamp,
		    const boost::posix_time::ptime& lastAcknowledgedTimestamp)
    : _nodeId (nodeId)
    , _ringId (ringId)
    , _host (host)
    , _port (port)
    , _authority (authority)
    , _state (state)
    , _clock (clock)
    , _lastPendingTimestamp (lastPendingTimestamp)
    , _lastAcknowledgedTimestamp (lastAcknowledgedTimestamp)
{

}





NodeInfo::~NodeInfo ()
{
    
}




std::ostream& 
operator<< ( std::ostream& os, const NodeState& op )
{
    if (op == OUTRING) os << "OUTRING"; 
    else if (op == ENTRING) os << "ENTRING"; 
    else if (op == INSRING) os << "INSRING"; 
    else os << "UNKNOWN"; 	
    return os;
}


std::istream& 
operator>> ( std::istream& is, NodeState& op )
{
    std::string st;
    is >> st;
    if (st == "ENTRING") op = ENTRING; 
    else if (st == "INSRING") op = INSRING; 
    else op = OUTRING; 	
    return is;
}




std::ostream& 
operator<< ( std::ostream& os, const NodeInfo& op )
{
    os << op._nodeId << " " << op._ringId << " " 
       << op._host << " " << op._port << " " 
       << op._authority << " "
       << op._state << " " << op._clock << " "
       << to_iso_string (op._lastPendingTimestamp) << " "
       << to_iso_string (op._lastAcknowledgedTimestamp); 
    return os;
}


std::istream& 
operator>> ( std::istream& is, NodeInfo& op )
{
    is >> op._nodeId;
    is >> op._ringId;
    is >> op._host;
    is >> op._port;
    is >> op._authority;
    is >> op._state;
    is >> op._clock;
    std::string tmp;
    is >> tmp;
    op._lastPendingTimestamp = from_iso_string (tmp);
    is >> tmp;
    op._lastAcknowledgedTimestamp = from_iso_string (tmp);
    return is;
}



bool 
operator== (const NodeInfo& op1, const NodeInfo& op2)
{
    // Note : the clock must not be taken into account when comparing
    // node infos !
    return 
	(op1.getNodeId () == op2.getNodeId ()) &&
	(op1.getRingId () == op2.getRingId ()) &&
	(op1.getHost () == op2.getHost ()) &&
	(op1.getPort () == op2.getPort ()) &&
	(op1.isAuthority () == op2.isAuthority ()) &&
	(op1.getState () == op2.getState ()) &&
	(op1.getLastPendingTimestamp () == op2.getLastPendingTimestamp ()) &&
	(op1.getLastAcknowledgedTimestamp () == op2.getLastAcknowledgedTimestamp ());

}


bool 
operator!= (const NodeInfo& op1, const NodeInfo& op2)
{
    // Note : the clock must not be taken into account when comparing
    // node infos !
    return !(op1 == op2); 
}



}
}
