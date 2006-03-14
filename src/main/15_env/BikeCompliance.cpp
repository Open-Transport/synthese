#include "BikeCompliance.h"



namespace synthese
{
namespace env
{



BikeCompliance::BikeCompliance (const int& id, 
				const boost::logic::tribool& compliant,
				const int& capacity)
    : Registrable<int,BikeCompliance> (id)
    , _compliant (compliant)
    , _capacity (capacity)
{

}


BikeCompliance::~BikeCompliance()
{
}


int 
BikeCompliance::getCapacity () const
{
    return _capacity;
}


boost::logic::tribool 
BikeCompliance::isCompliant () const
{
    return _compliant;
}




}
}
