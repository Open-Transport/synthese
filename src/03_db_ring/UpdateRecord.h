#ifndef SYNTHESE_DBRING_UPDATERECORD_H
#define SYNTHESE_DBRING_UPDATERECORD_H

#include "03_db_ring/NodeInfo.h"
#include "01_util/Registrable.h"
#include "01_util/UId.h"


#include <string>
#include <set>

#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>



namespace synthese
{



namespace dbring
{
	/** @addtogroup m101
		@{
	*/

    typedef enum {
        PENDING = 0,       //!< Pending.
        ACKNOWLEDGED = 1,  //!< Record has been acknowledged by ring authority. 
        FAILED = 2         //!< SQL update execution failed.
    } RecordState ;



    class UpdateRecord : public util::Registrable<uid,UpdateRecord>
{

private:

    uid _key;
    boost::posix_time::ptime _timestamp;   //!< Original emission time (sort criterium).
    NodeId _emitterNodeId;                 //!< Original emitter node id.
    RecordState _state;                    //!< Record enforcement state.
    std::string _compressedSQL;            //!< Update SQL query string (zlib compressed).


public:

    UpdateRecord ();
 
    UpdateRecord (const uid& key,
		  const boost::posix_time::ptime& timestamp, 
		  const NodeId& emitterNodeId,
		  const RecordState& state,
		  const std::string& compressedSQL);


    ~UpdateRecord ();

    const uid& getKey () const { return _key; }
    void setKey (const uid& key) { _key = key; }

    const boost::posix_time::ptime& getTimestamp () const { return _timestamp; }
    void setTimestamp (const boost::posix_time::ptime& timestamp) { _timestamp = timestamp; }

    const NodeId& getEmitterNodeId () const { return _emitterNodeId; }
    void setEmitterNodeId (const NodeId& emitterNodeId) { _emitterNodeId = emitterNodeId; }

    const RecordState& getState () const { return _state; }
    void setState (const RecordState& state) { _state = state; }

    bool hasCompressedSQL () const;
    const std::string& getCompressedSQL () const;
    void setCompressedSQL (const std::string& compressedSQL);

    // TODO : remove this (unneeded)
    // bool getLinked () const { return true; }
    //void setLinked (bool linked) {}

	      
private:

    friend std::ostream& operator<< ( std::ostream& os, const UpdateRecord& op );
    friend std::istream& operator>> ( std::istream& is, UpdateRecord& op );


};

    typedef boost::shared_ptr<UpdateRecord> UpdateRecordSPtr;


    struct updateRecordCmp
    {
	bool operator()(const UpdateRecordSPtr& op1, const UpdateRecordSPtr& op2) const
	{
	    return op1->getTimestamp () < op2->getTimestamp ();
	}
    };

    
    typedef std::set<UpdateRecordSPtr, updateRecordCmp> UpdateRecordSet;



std::ostream& operator<< ( std::ostream& os, const UpdateRecordSet& op );
std::istream& operator>> ( std::istream& is, UpdateRecordSet& op );


std::ostream& operator<< ( std::ostream& os, const UpdateRecord& op );
std::istream& operator>> ( std::istream& is, UpdateRecord& op );


bool operator== (const UpdateRecord& op1, const UpdateRecord& op2);
bool operator!= (const UpdateRecord& op1, const UpdateRecord& op2);

		/** @} */

}
}


#endif
