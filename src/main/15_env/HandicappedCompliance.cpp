#include "HandicappedCompliance.h"



namespace synthese
{
namespace env
{



HandicappedCompliance::HandicappedCompliance (const int& id, 
				const boost::logic::tribool& compliant,
				const int& capacity)
    : Registrable<int,HandicappedCompliance> (id)
    , _compliant (compliant)
    , _capacity (capacity)
{

}


HandicappedCompliance::~HandicappedCompliance()
{
}


int 
HandicappedCompliance::getCapacity () const
{
    return _capacity;
}


boost::logic::tribool 
HandicappedCompliance::isCompliant () const
{
    return _compliant;
}




}
}
