#include "Itinerary.h"

#include "Topography.h"
#include "PhysicalStop.h"

#include <assert.h>

namespace synmap
{

Itinerary::Itinerary(Topography* topography, 
			         int key,
					 const std::vector<const Vertex*>& vertices, 
					 const std::vector<bool>& physicalStop)
: Referrant (topography, key)
, _vertices (vertices)
, _edges (createEdges (topography, vertices))
, _physicalStops (createPhysicalStops (topography, this, vertices, physicalStop))
, _isPhysicalStop (physicalStop)
{
}



Itinerary::~Itinerary()
{
}




const std::vector<const Edge*> 
Itinerary::createEdges (Topography* topography, const std::vector<const Vertex*>& vertices)
{
	std::vector<const Edge*> edges;
	for (unsigned int i=0; i< vertices.size () - 1; ++i) {
        // Instantiating an itinerary does not create edges
        // It should have been previously created (roads).
        // assert (topography->hasEdge (vertices[i], vertices[i+1]));
		// edges.push_back (topography->getEdge (vertices[i], vertices[i+1]));
        
        edges.push_back (topography->newEdge (vertices[i], vertices[i+1]));
        
	}
	return edges;
}


const std::vector<const PhysicalStop*> 
Itinerary::createPhysicalStops (Topography* topography, 
								const Itinerary* itinerary, 
								const std::vector<const Vertex*>& vertices,
								const std::vector<bool>& physicalStop)
{
	std::vector<const PhysicalStop*> physicalStops;
	int pos (0);
	for (unsigned int i=0; i< vertices.size (); ++i) {
		if (physicalStop[i]) {
            // TODO : pb here
			physicalStops.push_back (topography->newPhysicalStop (0, itinerary, pos++, vertices[i]));
		}
	}	
	return physicalStops;
}


bool 
Itinerary::isPhysicalStop (int vertexIndex)
{
	return _isPhysicalStop[vertexIndex];
}
	
	
bool 
Itinerary::hasVertex (const Vertex* vertex) const
{
	for (std::vector<const Vertex*>::const_iterator iter = _vertices.begin ();
		 iter != _vertices.end (); ++iter) 
	{
		if (*iter == vertex) return true;	 	
	}
	return false;
}
	
	
	
bool 
Itinerary::hasEdge (const Edge* edge) const
{
	for (std::vector<const Edge*>::const_iterator iter = _edges.begin ();
		 iter != _edges.end (); ++iter) 
	{
		if (*iter == edge) return true;	 	
	}
	return false;
}



int 
Itinerary::firstIndexOf (const Edge* edge) const
{
	for (unsigned int i=0; i<_edges.size (); ++i)
	{
		if (_edges[i] == edge) return i;	 	
	}
	return -1;
}


int 
Itinerary::firstIndexOf (const Vertex* vertex) const
{
	for (unsigned int i=0; i<_vertices.size (); ++i)
	{
		if (_vertices[i] == vertex) return i;	 	
	}
	return -1;
}


const Vertex* 
Itinerary::getVertex (int index) const
{
	return _vertices.at (index);
}
	
const Vertex* 
Itinerary::getFirstVertex () const
{
    return _vertices.at (0);
}


const Vertex* 
Itinerary::getLastVertex () const
{
    return _vertices.at (_vertices.size () -1);
}


}

