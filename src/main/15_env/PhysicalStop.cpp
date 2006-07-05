#include "PhysicalStop.h"
#include "LogicalStop.h"



namespace synthese
{
namespace env 
{


PhysicalStop::PhysicalStop (const uid& id,
			    const std::string& name,
			    int rankInLogicalStop,
			    const LogicalStop* logicalStop,
			    double x, double y)
    : synthese::util::Registrable<uid,PhysicalStop> (id)
    , Vertex (logicalStop, rankInLogicalStop, x, y)
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
