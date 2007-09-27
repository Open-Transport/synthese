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
        INPROGRESS = 0, 
        SUCCESS = 1,    
        FAILURE = 2,    
        UNKNOWN = 3
    } TransmissionStatus ;
    


class TransmissionStatusMap
{

private:
    
    mutable boost::mutex _mapMutex; 
    std::map<NodeId, TransmissionStatus> _map;

public:

    TransmissionStatusMap () {};
    ~TransmissionStatusMap () {};

    TransmissionStatus getTransmissionStatus (const NodeId& nodeId) const;
    void setTransmissionStatus (const NodeId& nodeId, const TransmissionStatus& transmissionStatus);
    // bool hasAllFailed () const;


private:

    // friend std::ostream& operator<< ( std::ostream& os, const UpdateLog& op );
    // friend std::istream& operator>> ( std::istream& is, UpdateLog& op );

};


}
}


#endif
