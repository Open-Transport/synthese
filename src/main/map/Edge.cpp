#include "Edge.h"
#include "Vertex.h"




namespace synmap
{

Edge::Edge(const Vertex* from, const Vertex* to)
: _from (from), _to (to), _length (from->distanceTo (*to))
{
}




Edge::~Edge()
{
}

}


