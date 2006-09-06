#include "Road.h"

#include "Address.h"
#include "RoadChunk.h"


namespace synthese
{
namespace env
{


Road::Road (const uid& id,
	    const std::string& name,
	    const City* city,
	    const RoadType& type)

    : synthese::util::Registrable<uid,Road> (id)
    , AddressablePlace (name, city)
    , _type (type)
{

}



Road::~Road()
{
}


bool 
Road::hasReservationRule () const
{
    return false;
}



const ReservationRule* 
Road::getReservationRule () const
{
    return 0;
}



const Axis* 
Road::getAxis () const
{
    return 0;
}





const Road::RoadType& 
Road::getType () const
{
    return _type;
}


void 
Road::setType (const RoadType& type)
{
    _type = type;
}



const Address* 
Road::findClosestAddressBefore (double metricOffset) const
{
    const Address* address = 0;
    for (std::vector<const Address*>::const_iterator it = getAddresses ().begin ();
	 it != getAddresses ().end (); ++it)
    {
	if ( ((*it)->getMetricOffset () <= metricOffset) &&
	     ( (address == 0) || ((*it)->getMetricOffset () > address->getMetricOffset ()) ) )
	{
	    address = (*it);
	}
    }
    return address;
}


const Address* 
Road::findClosestAddressAfter (double metricOffset) const
{
    const Address* address = 0;
    for (std::vector<const Address*>::const_iterator it = getAddresses ().begin ();
	 it != getAddresses ().end (); ++it)
    {
	if ( ((*it)->getMetricOffset () >= metricOffset) &&
	     ( (address == 0) || ((*it)->getMetricOffset () < address->getMetricOffset ()) ) )
	{
	    address = (*it);
	}
    }
    return address;
}



bool 
Road::isRoad () const
{
    return true;
}



}
}
