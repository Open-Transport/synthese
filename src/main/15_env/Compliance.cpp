#include "Compliance.h"



namespace synthese
{
namespace env
{



Compliance::Compliance (const boost::logic::tribool& compliant,
			const int& capacity)
    : _compliant (compliant)
    , _capacity (capacity)
{

}


Compliance::~Compliance()
{
}


int 
Compliance::getCapacity () const
{
    return _capacity;
}


boost::logic::tribool 
Compliance::isCompliant () const
{
    return _compliant;
}




}
}
