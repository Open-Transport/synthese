#include "03_db_ring/UpdateLog.h"
#include "00_tcp/Constants.h"


#include <boost/date_time/posix_time/posix_time.hpp>

#include <iostream>


using namespace boost::posix_time;

using namespace synthese::tcp;


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



boost::posix_time::ptime 
UpdateLog::getUpdateLogBeginTimestamp () const
{
    assert (_set.size () > 0);
    return (*_set.begin ())->getTimestamp ();
}



std::ostream& 
operator<< ( std::ostream& os, const UpdateLog& op )
{
    boost::recursive_mutex::scoped_lock updateLock (*(op._updateMutex));
    
    for (UpdateRecordMap::const_iterator it = op._map.begin ();
	 it != op._map.end (); ++it)
    {
	os << (*(it->second)) << ETX; 
    }
    
    return os;
}


std::istream& 
operator>> ( std::istream& is, UpdateLog& op )
{
    boost::recursive_mutex::scoped_lock updateLock (*(op._updateMutex));
    static const int maxlen (1024*1024*64); 
    static char buf[maxlen]; 

    std::string lut;
    while (is.getline (buf, maxlen, ETX))
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

