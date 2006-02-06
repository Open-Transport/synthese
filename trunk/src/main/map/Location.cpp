#include "Location.h"



namespace synmap
{

Location::Location(Topography* topography, 
		   int key,
		   const Vertex* vertex)
: Referrant (topography, key)
, _vertex (vertex)
{
	
}



Location::~Location()
{
}



}
