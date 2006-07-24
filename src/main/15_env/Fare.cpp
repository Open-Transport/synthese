#include "Fare.h"

namespace synthese
{
namespace env
{



Fare::Fare (const uid& id, 
	    const std::string& name,
	    const FareType& type)
    : synthese::util::Registrable<uid,Fare> (id)
    , _name (name)
    , _type (type)
{
}



Fare::~Fare ()
{
}


    
    
const std::string& 
Fare::getName () const
{
    return _name;
}




const Fare::FareType& 
Fare::getType () const
{
    return _type;
}


void 
Fare::setName (const std::string& name)
{
    _name = name;
}



void 
Fare::setType (const FareType& fareType)
{
    _type = type;
}






}
}














