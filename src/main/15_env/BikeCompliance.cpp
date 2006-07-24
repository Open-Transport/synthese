#include "BikeCompliance.h"



namespace synthese
{
namespace env
{



BikeCompliance::BikeCompliance (const uid& id, 
				const boost::logic::tribool& compliant,
				const int& capacity)
    : Compliance (id, compliant, capacity)
{

}


BikeCompliance::~BikeCompliance()
{
}




}
}
