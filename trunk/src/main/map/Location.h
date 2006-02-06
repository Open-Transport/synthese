#ifndef LOCATION_H_
#define LOCATION_H_

#include "Referrant.h"


namespace synmap
{

class Vertex;


class Location : public Referrant
{
	
private:

	const Vertex* _vertex;

	
public:

	Location(Topography* topography, 
		 int key,
		 const Vertex* vertex);
				 
	virtual ~Location();

	const Vertex* getVertex () const { return _vertex; }
	
	friend class Topography;
};

}

#endif /*LOCATION_H_*/
