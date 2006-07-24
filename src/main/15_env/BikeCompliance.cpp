#include "BikeCompliance.h"



namespace synthese
{
namespace env
{



BikeCompliance::BikeCompliance (const uid& id, 
				const boost::logic::tribool& compliant,
				const int& capacity)
    : synthese::util::Registrable<uid,BikeCompliance> (id)
    , Compliance (compliant, capacity)
{
    
}


BikeCompliance::~BikeCompliance()
{
}




}
}
