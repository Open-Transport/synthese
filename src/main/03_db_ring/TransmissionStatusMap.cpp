#include "03_db_ring/TransmissionStatusMap.h"



namespace synthese
{

namespace dbring
{


    TransmissionStatus
    TransmissionStatusMap::getTransmissionStatus (const NodeId& nodeId) const
    {
	boost::mutex::scoped_lock mapLock ( _mapMutex);
	std::map<NodeId, TransmissionStatus>::const_iterator it = _map.find (nodeId);
	if (it == _map.end ()) return UNKNOWN;
	return it->second;
    }


    void
    TransmissionStatusMap::setTransmissionStatus (const NodeId& nodeId, const TransmissionStatus& transmissionStatus)
    {
	boost::mutex::scoped_lock mapLock ( _mapMutex);
	std::map<NodeId, TransmissionStatus>::const_iterator it = _map.find (nodeId);
	if (it == _map.end ()) 
	{
	    _map.insert (std::make_pair (nodeId, transmissionStatus));
	}
	else
	{
	    _map[nodeId] = transmissionStatus;
	}
    }

/*
    bool 
    TransmissionStatusMap::hasAllFailed () const 
    {
	for (std::map<NodeId, TransmissionStatus>::const_iterator it = _map.begin (); 
	     it != _map.end (); ++it)
	{
	    if (it->second != FAILED) return false; 
	} 
	return true;
    }
*/
    
}
}



