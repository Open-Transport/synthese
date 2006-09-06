#include "PhysicalStop.h"



namespace synthese
{
namespace env 
{


PhysicalStop::PhysicalStop (const uid& id,
			    const std::string& name,
			    const AddressablePlace* place,
			    double x, double y)
    : synthese::util::Registrable<uid,PhysicalStop> (id)
    , Vertex (place, x, y)
    , _name (name)
{

}



PhysicalStop::~PhysicalStop()
{

}


const std::string& 
PhysicalStop::getName () const
{
    return _name;
}



void 
PhysicalStop::setName (const std::string& name)
{
    _name = name;
}



bool 
PhysicalStop::isAddress () const
{
    return false;
}




const uid& 
PhysicalStop::getId () const
{
    return synthese::util::Registrable<uid,PhysicalStop>::getId ();
}



}
}
