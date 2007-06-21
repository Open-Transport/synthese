#include "03_db_ring/UpdateLog.h"

#include <boost/date_time/posix_time/posix_time.hpp>

#include <iostream>


using namespace boost::posix_time;


namespace synthese
{
namespace dbring
{




UpdateLog::UpdateLog ()
    : _updateMutex (new boost::recursive_mutex ())

{

}
 


UpdateLog::~UpdateLog ()
{

}




bool 
UpdateLog::hasUpdateRecord (const uid& key) const
{
    boost::recursive_mutex::scoped_lock updateLock (*_updateMutex);
    return _map.find (key) != _map.end ();
}



const UpdateRecordSPtr& 
UpdateLog::getUpdateRecord (const uid& key) const
{
    boost::recursive_mutex::scoped_lock updateLock (*_updateMutex);
    assert (hasUpdateRecord (key));
    return _map.find (key)->second;
}



UpdateRecordSPtr& 
UpdateLog::getUpdateRecord (const uid& key)
{
    boost::recursive_mutex::scoped_lock updateLock (*_updateMutex);
    assert (hasUpdateRecord (key));
    return _map.find (key)->second;
}


    
void 
UpdateLog::setUpdateRecord (const UpdateRecordSPtr& updateRecord)
{
    boost::recursive_mutex::scoped_lock updateLock (*_updateMutex);

    UpdateRecordMap::iterator it = _map.find (updateRecord->getKey ());
    if (it == _map.end ())
    {
	_map.insert (std::make_pair (updateRecord->getKey (), updateRecord));
	_set.insert (updateRecord);
    }
    else
    {
	it->second = updateRecord;
    }

}


void 
UpdateLog::flush ()
{
    boost::recursive_mutex::scoped_lock updateLock (*_updateMutex);
    _map.clear ();
    _set.clear ();
}




std::ostream& 
operator<< ( std::ostream& os, const UpdateLog& op )
{
    boost::recursive_mutex::scoped_lock updateLock (*(op._updateMutex));
    
    for (UpdateRecordMap::const_iterator it = op._map.begin ();
	 it != op._map.end (); ++it)
    {
	// TODO : replace qith a special char
	os << (*(it->second)) << "#"; 
    }
    
    return os;
}


std::istream& 
operator>> ( std::istream& is, UpdateLog& op )
{
    boost::recursive_mutex::scoped_lock updateLock (*(op._updateMutex));
    static char buf[1024*256]; // 256 KBytes max.

    std::string lut;
    while (is.getline (buf, 1024*256, '#'))
    {
	std::stringstream input (buf);
	UpdateRecord* updateRecord = new UpdateRecord ();
	input >> (*updateRecord);
	op.setUpdateRecord (UpdateRecordSPtr(updateRecord));
    }

    return is;

}





}
}

