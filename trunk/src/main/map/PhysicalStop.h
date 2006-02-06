#ifndef PHYSICALSTOP_H_
#define PHYSICALSTOP_H_

#include "Location.h"


namespace synmap
{

class Vertex;
class Itinerary;


class PhysicalStop : public Location
{
	
private:

	const int _position;
	
public:

	PhysicalStop(Topography* topography, 
		     int key,
		     int position, 
		     const Vertex* vertex);
				 
	virtual ~PhysicalStop();



	
	int getPosition () const { return _position; }
	
	
	friend class Topography;
};

}

#endif /*PHYSICALSTOP_H_*/
