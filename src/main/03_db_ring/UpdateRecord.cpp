#include "03_db_ring/UpdateRecord.h"

#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/date_time/posix_time/time_parsers.hpp>


using namespace synthese::util;
using namespace boost::posix_time;


namespace synthese
{

namespace dbring
{




UpdateRecord::UpdateRecord ()
    : _timestamp (min_date_time)
    , _emitterNodeId (-1)
    , _state (PENDING)
    , _sql ("")
{

}
 



UpdateRecord::UpdateRecord (const uid& key,
			    const boost::posix_time::ptime& timestamp, 
			    const NodeId& emitterNodeId,
			    const RecordState& state,
			    const std::string& sql)
    : _key (key)
    , _timestamp (timestamp)
    , _emitterNodeId (emitterNodeId)
    , _state (state)
    , _sql (sql)
{

}





UpdateRecord::~UpdateRecord ()
{
    
}



std::ostream& 
operator<< ( std::ostream& os, const RecordState& op )
{
    if (op == PENDING)           os << "PENDING"; 
    else if (op == ACKNOWLEDGED) os << "ACKNOWD"; 
    else if (op == FAILED)       os << " FAILED"; 
    else os << "UNKNOWN"; 	
    return os;
}



std::istream& 
operator>> ( std::istream& is, RecordState& op )
{
    std::string st;
    is >> st;
    if (st == "PENDING") op = PENDING; 
    else if (st == "ACKNOWD") op = ACKNOWLEDGED; 
    else op = FAILED; 	
    return is;
}




std::ostream& 
operator<< ( std::ostream& os, const UpdateRecord& op )
{
    os << op._key << " " << to_iso_string (op._timestamp) << " " << op._emitterNodeId << " " 
       << " " << op._state << " " 
       << std::skipws << op._sql;
 
    return os;
}


std::istream& 
operator>> ( std::istream& is, UpdateRecord& op )
{
    static char c;
    is >> op._key;
    std::string ts;
    is >> ts;
    op._timestamp = from_iso_string (ts);
    is >> op._emitterNodeId;
    is >> op._state;
    is >> std::ws;
    while (is.get (c)) op._sql += c;
    return is;
}




bool 
operator== (const UpdateRecord& op1, const UpdateRecord& op2)
{
    return 
	(op1.getKey () == op2.getKey ()) &&
	(op1.getTimestamp () == op2.getTimestamp ()) &&
	(op1.getEmitterNodeId () == op2.getEmitterNodeId ()) &&
	(op1.getState () == op2.getState ()) &&
	(op1.getSQL () == op2.getSQL ());

}


bool 
operator!= (const UpdateRecord& op1, const UpdateRecord& op2)
{
    return !(op1 == op2); 
}


}
}
