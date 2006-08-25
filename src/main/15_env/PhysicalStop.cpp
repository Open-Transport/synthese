#include "PhysicalStop.h"



namespace synthese
{
namespace env 
{


PhysicalStop::PhysicalStop (const uid& id,
			    const std::string& name,
			    const ConnectionPlace* connectionPlace,
			    int rankInConnectionPlace,
			    double x, double y)
    : synthese::util::Registrable<uid,PhysicalStop> (id)
    , Vertex (connectionPlace, rankInConnectionPlace, x, y)
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







}
}
