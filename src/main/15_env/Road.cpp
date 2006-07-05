#include "Road.h"

#include "RoadChunk.h"


namespace synthese
{
namespace env
{


Road::Road (const uid& id,
	    const std::string& name,
	    const City* city,
	    const RoadType& type,
	    const std::vector<const RoadChunk*>& chunks)
    : synthese::util::Registrable<uid,Road> (id)
    , AddressablePlace (name, city)
    , _type (type)
    , _chunks (chunks)
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



int 
Road::getEdgesCount () const
{
    return _chunks.size ();
}


const Edge* 
Road::getEdge (int index) const
{
    return _chunks.at (index);
}



const Road::RoadType& 
Road::getType () const
{
    return _type;
}






}
}
