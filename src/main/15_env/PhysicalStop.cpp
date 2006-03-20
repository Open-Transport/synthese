#include "PhysicalStop.h"
#include "LogicalStop.h"



namespace synthese
{
namespace env 
{


PhysicalStop::PhysicalStop (const std::string& name,
			    int rankInLogicalStop,
			    const LogicalStop* logicalStop)
    : Vertex (logicalStop, rankInLogicalStop)
    , _name (name)
{

}



PhysicalStop::~PhysicalStop()
{

}



    
const std::set<const LineStop*>& 
PhysicalStop::getDepartureLineStops () const
{
    return _departureLineStops;
}



const std::set<const LineStop*>& 
PhysicalStop::getArrivalLineStops () const
{
    return _arrivalLineStops;
}




 

void 
PhysicalStop::addDepartureLineStop ( const LineStop* lineStop )
{
    _departureLineStops.insert (lineStop);    
}



void 
PhysicalStop::addArrivalLineStop ( const LineStop* lineStop )
{
    _arrivalLineStops.insert (lineStop);    
}







}
}
