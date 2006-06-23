#include "Fare.h"

namespace synthese
{
namespace env
{



Fare::Fare (const int& id, 
	    const std::string& name,
	    const FareType& type)
    : synthese::util::Registrable<int,Fare> (id)
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








}
}














