#include "HandicappedCompliance.h"



namespace synthese
{
namespace env
{



HandicappedCompliance::HandicappedCompliance (const uid& id, 
				const boost::logic::tribool& compliant,
				const int& capacity)
    : synthese::util::Registrable<uid,HandicappedCompliance> (id)
    , Compliance (compliant, capacity)
{
    
}


HandicappedCompliance::~HandicappedCompliance()
{
}



}
}
