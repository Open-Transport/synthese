#ifndef SYNTHESE_DBRING_TRANSMISSIONSTATUSMAP_H
#define SYNTHESE_DBRING_TRANSMISSIONSTATUSMAP_H

#include "03_db_ring/NodeInfo.h"

#include <map>

#include <iostream>

#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>




namespace synthese
{



namespace dbring
{


    typedef enum {
	READY = 0,
        INPROGRESS = 1, 
        FAILURE = 2    
    } TransmissionStatus ;
    


class TransmissionStatusMap
{

private:
    
    mutable boost::mutex _mapMutex; 
    std::map<NodeId, TransmissionStatus> _map;

public:

    TransmissionStatusMap () {};
    ~TransmissionStatusMap () {};

    /* Resets all transmission statuses.
       Actually, this method clears the map (the default transmission status being returned is UNKNOWN)
    */
    void reset ();
    
    TransmissionStatus getTransmissionStatus (const NodeId& nodeId) const;
    void setTransmissionStatus (const NodeId& nodeId, const TransmissionStatus& transmissionStatus);
    

private:

};


}
}


#endif
