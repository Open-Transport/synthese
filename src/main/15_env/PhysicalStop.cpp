#include "PhysicalStop.h"
#include "LogicalStop.h"



namespace synthese
{
namespace env 
{


PhysicalStop::PhysicalStop (int id,
			    const std::string& name,
			    int rankInLogicalStop,
			    const LogicalStop* logicalStop,
			    double x, double y)
    : Registrable<int,PhysicalStop> (id)
    , Vertex (logicalStop, rankInLogicalStop, x, y)
    , _name (name)
{

}



PhysicalStop::~PhysicalStop()
{

}








}
}
