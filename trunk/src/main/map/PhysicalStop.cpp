#include "PhysicalStop.h"
#include "Itinerary.h"
#include "Address.h"
#include "Topography.h"
#include "server/cEnvironnement.h"

#include <assert.h>

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
    , _address (0)
{
  
}



PhysicalStop::~PhysicalStop()
{
  delete _address;
}


// bidouille infame
void 
PhysicalStop::setMetricOffset (const Road* road, double metricOffset) {
  // urk urk
  const LogicalPlace* logicalPlace = ((cEnvironnement*) getTopography ()
    ->getEnvironment ())->getLogicalPlace (_logicalPlaceId);

  // et hop un peu de rigueur ne fait jamais de mal...
  assert (_address == 0);
  
  // TODO : verifier -1 correspond a rang inconnu
  _address = new Address ((LogicalPlace*) logicalPlace, -1, (Road*) road, metricOffset);

}
	

// bidouille encore plus infame
Address* 
PhysicalStop::getAddress () const {
  return _address;
}







}
