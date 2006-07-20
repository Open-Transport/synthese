#include "Place.h"


namespace synthese
{
namespace env
{



Place::Place (const std::string& name,
	      const City* city)
    : _name (name)
    , _city (city)
{
}


Place::~Place ()
{

}



const std::string& 
Place::getName () const
{
    return _name;
}



void 
Place::setName (const std::string& name)
{
    _name = name;
}




const std::string& 
Place::getOfficialName () const
{
    return getName ();
}



const City* 
Place::getCity () const
{
    return _city;
}
    



}
}

