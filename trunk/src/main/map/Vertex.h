#ifndef VERTEX_H_
#define VERTEX_H_

#include "XYPoint.h"
#include <vector>

namespace synmap
{

class Edge;
class Zone;


class Vertex : public synmap::XYPoint 
{
private:

	std::vector<const Edge*> _inEdges;	 //!< There is an inEdge which has this as target.
	std::vector<const Edge*> _outEdges;	 //!< There is an outEdge which has this as origin.
	
	std::vector<const Vertex*> _inVertices;	  //!< There is an inEdge from an inVertice to this
	std::vector<const Vertex*> _outVertices;  //!< There is an outEdge from this to an outVertice

	Vertex(double x, double y);
	Vertex(const Vertex& ref);
	
public:

	
	double getX () const { return XYPoint::getX (); }
	double getY () const { return XYPoint::getY (); }
	
	const std::vector<const Edge*>& getIncomingEdges () const { return _inEdges; }
	const std::vector<const Edge*>& getOutgoingEdges () const { return _outEdges; }
	
	bool isInside (const Zone* zone) const;
	
	
private:

	void addEdge (const Edge* edge);
	
	friend class Topography;
};

}

#endif /*VERTEX_H_*/
