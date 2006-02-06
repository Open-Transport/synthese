#include "PhysicalStop.h"
#include "Itinerary.h"

namespace synmap
{

PhysicalStop::PhysicalStop(Topography* topography, 
			   int key,
			   int position, 
			   const Vertex* vertex)
: Location (topography, key, vertex)
, _position (position)
{
	
}



PhysicalStop::~PhysicalStop()
{
}








}
