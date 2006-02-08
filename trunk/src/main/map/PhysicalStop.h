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

	
public:

	PhysicalStop(Topography* topography, 
		     int key,
		     const Vertex* vertex);
				 
	virtual ~PhysicalStop();


	
	friend class Topography;
};

}

#endif /*PHYSICALSTOP_H_*/
