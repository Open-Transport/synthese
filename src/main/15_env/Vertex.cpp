#include "Vertex.h"

#include "Edge.h"
#include "ConnectionPlace.h"




namespace synthese
{
namespace env
{



Vertex::Vertex (const AddressablePlace* place,
		double x,
		double y) 
    : Point (x, y) // By default geolocation is unknown.
    , _addressablePlace (place)
{
    
}



Vertex::~Vertex ()
{
}



const ConnectionPlace* 
Vertex::getConnectionPlace () const
{
    return dynamic_cast<const ConnectionPlace*> (_addressablePlace);
}



const AddressablePlace* 
Vertex::getPlace () const
{
    return _addressablePlace;
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

