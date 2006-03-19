#include "PedestrianCompliance.h"



namespace synthese
{
namespace env
{



PedestrianCompliance::PedestrianCompliance (const int& id, 
				const boost::logic::tribool& compliant,
				const int& capacity)
    : Compliance (id, compliant, capacity)
{

}


PedestrianCompliance::~PedestrianCompliance()
{
}




}
}
