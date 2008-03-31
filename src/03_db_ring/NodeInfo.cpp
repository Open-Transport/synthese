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
{

}
 



NodeInfo::NodeInfo (const NodeId& nodeId, 
		    const RingId& ringId,
		    const std::string& host,
		    int port)
    : _nodeId (nodeId)
    , _ringId (ringId)
    , _host (host)
    , _port (port)
{

}





NodeInfo::~NodeInfo ()
{
    
}





std::ostream& 
operator<< ( std::ostream& os, const NodeInfo& op )
{
    os << op._nodeId << " " << op._ringId << " " 
       << op._host << " " << op._port;
    return os;
}




std::istream& 
operator>> ( std::istream& is, NodeInfo& op )
{
    is >> op._nodeId;
    is >> op._ringId;
    is >> op._host;
    is >> op._port;

    return is;
}



bool 
operator== (const NodeInfo& op1, const NodeInfo& op2)
{
    return 
	(op1.getNodeId () == op2.getNodeId ()) &&
	(op1.getRingId () == op2.getRingId ()) &&
	(op1.getHost () == op2.getHost ()) &&
	(op1.getPort () == op2.getPort ());
}

    
bool 
operator!= (const NodeInfo& op1, const NodeInfo& op2)
{
    return !(op1 == op2); 
}



}
}
