#ifndef PHYSICALSTOP_H_
#define PHYSICALSTOP_H_

#include "Location.h"
#include "Referrant.h"


namespace synmap
{

class Vertex;
class Itinerary;


class PhysicalStop : public Location, public Referrant
{
	
private:

  const int _logicalPlaceId;
  const int _rank;
	
public:

	PhysicalStop(Topography* topography, 
		     int logicalPlaceId,
		     int rank,
		     const Vertex* vertex);
				 
	virtual ~PhysicalStop();


	int getLogicalPlaceId () const { return _logicalPlaceId; }
	int getRank () const { return _rank; }

	
	friend class Topography;
};

}

#endif /*PHYSICALSTOP_H_*/
