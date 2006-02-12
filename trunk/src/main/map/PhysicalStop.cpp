#include "PhysicalStop.h"
#include "Itinerary.h"

namespace synmap
{

PhysicalStop::PhysicalStop(Topography* topography, 
			   int logicalPlaceId,
			   int rank,
			   const Vertex* vertex)
  : Location (vertex)
  , Referrant (topography, logicalPlaceId * 1000 + rank)
  , _logicalPlaceId (logicalPlaceId)
  , _rank (rank)
{
  
}



PhysicalStop::~PhysicalStop()
{
}








}
