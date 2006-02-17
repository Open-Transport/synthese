#include "Zone.h"
#include <assert.h>


namespace synmap
{

Zone::Zone(Topography* topography, 
		   int key,
		   const std::vector<const Vertex*>& vertices)
: Referrant (topography, key)
, _vertices (vertices)
{
	// At least a triangle.
	assert (vertices.size () >= 3);
}



Zone::~Zone()
{
}


bool 
Zone::hasVertex (const Vertex* vertex) const
{
	for (unsigned int i=0; i<_vertices.size(); ++i) {
		if (_vertices[i] == vertex) return true;
	}	
	return false;
}






}
