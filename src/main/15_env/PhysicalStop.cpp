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








}
}
