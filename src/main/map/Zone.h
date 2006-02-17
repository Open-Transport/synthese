#ifndef ZONE_H_
#define ZONE_H_

#include "Referrant.h"
#include <vector>


namespace synmap
{

class Vertex;


class Zone : public Referrant
{
private:
	const std::vector<const Vertex*> _vertices;

	
public:

	Zone(Topography* topography, 
    	 int key,
		 const std::vector<const Vertex*>& vertices);
		 
	virtual ~Zone();


	bool hasVertex (const Vertex* vertex) const;
	const std::vector<const Vertex*>& getVertices () const { return _vertices; } 
	
};

}

#endif /*ZONE_H_*/
