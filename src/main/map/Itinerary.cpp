#include "Itinerary.h"

#include "Topography.h"
#include "PhysicalStop.h"
#include "Location.h"

#include <assert.h>

namespace synmap
{

Itinerary::Itinerary(Topography* topography, 
		     int key,
		     const std::vector<const Location*>& steps)
: Referrant (topography, key)
, _steps (steps)
, _edges (createEdges (topography, steps))
{
  for (std::vector<const Location*>::const_iterator iter = _steps.begin ();
       iter != _steps.end (); ++iter) 
    {
      // Physical stops must have been inserted in topography "a priori".
      const PhysicalStop* pstop = dynamic_cast<const PhysicalStop*> (*iter);
      if (pstop != 0) {
	_physicalStops.push_back (pstop);
      }
      _vertices.push_back ((*iter)->getVertex ());
    }
  

}



Itinerary::~Itinerary()
{
}




const std::vector<const Edge*> 
Itinerary::createEdges (Topography* topography, const std::vector<const Location*>& steps)
{
  std::vector<const Edge*> edges;
  for (unsigned int i=0; i< steps.size () - 1; ++i) {
    // Instantiating an itinerary does not create edges
    // It should have been previously created (roads).
    // assert (topography->hasEdge (steps[i], steps[i+1]));
    // edges.push_back (topography->getEdge (steps[i], steps[i+1]));
    
    edges.push_back (topography->newEdge (steps[i]->getVertex(), steps[i+1]->getVertex()));
        
  }
  return edges;
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


const Location* 
Itinerary::getStep (int index) const
{
	return _steps.at (index);
}
	
const Location* 
Itinerary::getFirstStep () const
{
    return _steps.at (0);
}


const Location* 
Itinerary::getLastStep () const
{
    return _steps.at (_steps.size () -1);
}


}

