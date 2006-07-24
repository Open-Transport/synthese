#include "HandicappedCompliance.h"



namespace synthese
{
namespace env
{



HandicappedCompliance::HandicappedCompliance (const uid& id, 
				const boost::logic::tribool& compliant,
				const int& capacity)
    : Compliance (id, compliant, capacity)
{
    
}


HandicappedCompliance::~HandicappedCompliance()
{
}



}
}
