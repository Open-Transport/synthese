#include "PhysicalStop.h"
#include "Itinerary.h"

namespace synmap
{

PhysicalStop::PhysicalStop(Topography* topography, 
						   int key,
						   const Itinerary* itinerary, 
						   int position, 
						   const Vertex* vertex)
: Referrant (topography, key)
, _itinerary (itinerary)
, _position (position)
, _vertex (vertex)
{
	
}



PhysicalStop::~PhysicalStop()
{
}





const PhysicalStop* 
PhysicalStop::getPreceeding () const
{
	if (_position == 0) return 0;
	return _itinerary->getPhysicalStop (_position - 1);
}


const PhysicalStop* 
PhysicalStop::getFollowing () const
{
	if (_position == _itinerary->getNbPhysicalStops() - 1) return 0;
	return _itinerary->getPhysicalStop (_position + 1);
}


}
