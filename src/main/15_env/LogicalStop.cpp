#include "LogicalStop.h"



namespace synthese
{
namespace env
{



LogicalStop::LogicalStop (const int& id,
			  const std::string& name,
			  const City* city)
    : synthese::util::Registrable<int,LogicalStop> (id)
    , ConnectionPlace (name, city)
{

}



LogicalStop::~LogicalStop ()
{
}




const std::vector<const PhysicalStop*>& 
LogicalStop::getPhysicalStops () const
{
    return _physicalStops;
}



void 
LogicalStop::addPhysicalStop (const PhysicalStop* physicalStop)
{
    _physicalStops.push_back (physicalStop);
}






}
}

