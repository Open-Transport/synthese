#include "Vertex.h"




namespace synthese
{
namespace env
{



Vertex::Vertex (const ConnectionPlace* connectionPlace,
		int rankInConnectionPlace,
		double x,
		double y) 
    : Point (x, y) // By default geolocation is unknown.
    , _connectionPlace (connectionPlace)
    , _rankInConnectionPlace (rankInConnectionPlace)
{
    
}



Vertex::~Vertex ()
{
}



const ConnectionPlace* 
Vertex::getConnectionPlace () const
{
    return _connectionPlace;
}



int 
Vertex::getRankInConnectionPlace () const
{
    return _rankInConnectionPlace;
}



    
const std::set<const Edge*>& 
Vertex::getDepartureEdges () const
{
    return _departureEdges;
}



const std::set<const Edge*>& 
Vertex::getArrivalEdges () const
{
    return _arrivalEdges;
}




 

void 
Vertex::addDepartureEdge ( const Edge* edge )
{
    _departureEdges.insert (edge);    
}



void 
Vertex::addArrivalEdge ( const Edge* edge )
{
    _arrivalEdges.insert (edge);    
}






}
}
