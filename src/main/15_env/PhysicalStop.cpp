#include "PhysicalStop.h"



namespace synthese
{
namespace env 
{


PhysicalStop::PhysicalStop (const uid& id,
			    const std::string& name,
			    int rankInConnectionPlace,
			    const ConnectionPlace* connectionPlace,
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







}
}
