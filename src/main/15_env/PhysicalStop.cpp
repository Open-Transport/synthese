#include "PhysicalStop.h"


namespace synthese
{
namespace env 
{


PhysicalStop::PhysicalStop (int rank, const std::string& name, const LogicalPlace* logicalPlace)
    : Gateway (rank, name, logicalPlace)
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
