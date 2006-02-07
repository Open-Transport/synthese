#include "Vertex.h"
#include "Edge.h"
#include "Zone.h"
#include "Geometry.h"

#include <limits>
#include <iostream>


namespace synmap
{



Vertex::Vertex(double x, double y)
: XYPoint (x, y)
{
}



Vertex::Vertex(const Vertex& ref)
: XYPoint (ref)
{
}


void 
Vertex::addEdge (const Edge* edge)
{
  if (edge->getFrom() == this) {
    _outEdges.push_back(edge);
    _outVertices.push_back(edge->getTo());
  } else if (edge->getTo() == this) {
    _inEdges.push_back(edge);
    _outVertices.push_back(edge->getTo());
  }

}


bool 
Vertex::isInside (const Zone* zone) const
{
	if (zone->hasVertex(this)) return true;
	
	// If this is on polygon border, the function returns true.
	
	// Calculates the number of intersections between edges of polygon and
	// a half line starting from point.
	// If even number of points, this is outside, otherwise it is inside.	
	
	// First take affine parameters of an horizontal line going through
	// this.
	XYPoint infinite (std::numeric_limits<int>::max (), getY());
	int nbIntersections = 0;
	const std::vector<const Vertex*>& vertices = zone->getVertices ();
	for (unsigned int i=0; i<vertices.size (); ++i) {
		const XYPoint* p0 = vertices[i];
		const XYPoint* p1 = (i == vertices.size () - 1) ? vertices[0] : vertices[i+1];
		
		try {
			XYPoint intsc = calculateIntersection (*p0, *p1, *this, infinite);
			if (intsc.getX() <= getX()) ++nbIntersections;
		} catch (...) {
			// In case there is no intersection...
		}
	}
	
	return nbIntersections % 2 == 1;
	
	
}



std::set<const Vertex*> 
Vertex::findCloseNeighbors (double distance) const
{
  std::set<const Vertex*> result;
  result.insert (this);
  findCloseNeighbors (distance, result);
  result.erase (this);
  return result;
}


void
Vertex::findCloseNeighbors (double distance, 
			      std::set<const Vertex*>& result) const {
  
  const std::vector<const Edge*>& outEdges = getOutgoingEdges ();
  for (int i=0; i<outEdges.size (); ++i) {
    const Edge* outEdge = outEdges[i];
    if (result.find (outEdge->getTo ()) != result.end ()) continue;

    if (outEdge->getLength () < distance) {
      
      result.insert (outEdge->getTo ());
      outEdge->getTo ()->findCloseNeighbors (distance - outEdge->getLength (),
					      result);
    }
    
  }
}



}
	
