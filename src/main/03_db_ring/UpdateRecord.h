#ifndef SYNTHESE_DBRING_UPDATERECORD_H
#define SYNTHESE_DBRING_UPDATERECORD_H

#include "03_db_ring/NodeInfo.h"
#include "01_util/UId.h"


#include <string>

#include <iostream>
#include <boost/date_time/posix_time/posix_time_types.hpp>



namespace synthese
{



namespace dbring
{

    typedef enum {
        PENDING = 0,       //!< Pending.
        ACKNOWLEDGED = 1,  //!< Record has been acknowledged by ring authority. 
        FAILED = 2         //!< SQL update execution failed.
    } RecordState ;



class UpdateRecord
{

private:

    uid _key;
    boost::posix_time::ptime _timestamp;   //!< Original emission time (sort criterium).
    NodeId _emitterNodeId;                 //!< Original emitter node id.
    RecordState _state;                    //!< Record enforcement state.
    std::string _sql;                      //!< Update SQL query string.


public:

    UpdateRecord ();
 
    UpdateRecord (const uid& key,
		  const boost::posix_time::ptime& timestamp, 
		  const NodeId& emitterNodeId,
		  const RecordState& state,
		  const std::string& sql);

    ~UpdateRecord ();

    const uid& getKey () const { return _key; }
    void setKey (const uid& key) { _key = key; }

    const boost::posix_time::ptime& getTimestamp () const { return _timestamp; }
    void setTimestamp (const boost::posix_time::ptime& timestamp) { _timestamp = timestamp; }

    const NodeId& getEmitterNodeId () const { return _emitterNodeId; }
    void setEmitterNodeId (const NodeId& emitterNodeId) { _emitterNodeId = emitterNodeId; }

    const RecordState& getState () const { return _state; }
    void setState (const RecordState& state) { _state = state; }

    const std::string& getSQL () const { return _sql; }
    void setSQL (const std::string& sql) { _sql = sql; }
	      
private:

    friend std::ostream& operator<< ( std::ostream& os, const UpdateRecord& op );
    friend std::istream& operator>> ( std::istream& is, UpdateRecord& op );


};



std::ostream& operator<< ( std::ostream& os, const UpdateRecord& op );
std::istream& operator>> ( std::istream& is, UpdateRecord& op );


bool operator== (const UpdateRecord& op1, const UpdateRecord& op2);
bool operator!= (const UpdateRecord& op1, const UpdateRecord& op2);


}
}


#endif
