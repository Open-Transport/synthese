#ifndef LOCATION_H_
#define LOCATION_H_

#include "Referrant.h"


namespace synmap
{

class Vertex;


class Location 
{
	
private:

	const Vertex* _vertex;

 protected:

	Location(const Vertex* vertex);

	
public:

	virtual ~Location();


	const Vertex* getVertex () const { return _vertex; }
	
	friend class Topography;
};

}

#endif /*LOCATION_H_*/
