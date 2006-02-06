#include "Road.h"

namespace synmap
{

Road::Road(Topography* topography,
	   int key,
	   const std::string& name,
	   const std::string& discriminant)
  : Referrant (topography, key)
  , _name (name)
  , _discriminant (discriminant)
{
}



Road::~Road()
{
}



}
