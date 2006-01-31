#ifndef PHYSICALSTOP_H_
#define PHYSICALSTOP_H_

#include "Referrant.h"


namespace synmap
{

class Vertex;
class Itinerary;


class PhysicalStop : public Referrant
{
	
private:

	const Itinerary* _itinerary;
	const int _position;
	const Vertex* _vertex;

	
public:

	PhysicalStop(Topography* topography, 
				 int key,
				 const Itinerary* itinerary, 
				 int position, 
				 const Vertex* vertex);
				 
	virtual ~PhysicalStop();



	const Vertex* getVertex () const { return _vertex; }
	
	const Itinerary* getItinerary () const { return _itinerary; }
	
	int getPosition () const { return _position; }
	
	const PhysicalStop* getPreceeding () const;
	const PhysicalStop* getFollowing () const;

	
	friend class Topography;
};

}

#endif /*PHYSICALSTOP_H_*/
