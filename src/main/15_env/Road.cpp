#include "Road.h"

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





const Road::RoadType& 
Road::getType () const
{
    return _type;
}







}
}
