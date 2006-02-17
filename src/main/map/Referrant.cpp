#include "Referrant.h"

namespace synmap
{

Referrant::Referrant(Topography* topography, int key)
: _topography (topography)
, _key (key)
{
}

Referrant::~Referrant()
{
}



bool
Referrant::hasProperty (const std::string& name) const
{
	Properties::const_iterator iter = _properties.find (name);
	return (iter != _properties.end());	
}



const std::string& 
Referrant::getProperty (const std::string& name) const
{
	Properties::const_iterator iter = _properties.find (name);
	if (iter == _properties.end()) {
		throw (std::string ("No property ") + name + std::string (" defined"));
	} 
	return iter->second;	
}


void 
Referrant::setProperty (const std::string& name, const std::string& value)
{
	_properties[name] = value;
}





}
