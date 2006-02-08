#include "PhysicalStop.h"
#include "Itinerary.h"

namespace synmap
{

PhysicalStop::PhysicalStop(Topography* topography, 
			   int key,
			   const Vertex* vertex)
  : Location (vertex)
  , Referrant (topography, key)
{
  
}



PhysicalStop::~PhysicalStop()
{
}








}
