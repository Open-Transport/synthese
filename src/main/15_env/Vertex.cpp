#include "Vertex.h"




namespace synthese
{
namespace env
{



Vertex::Vertex (const ConnectionPlace* connectionPlace,
		int rankInConnectionPlace) 
    : Point () // By default geolocation is unknown.
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







}
}
