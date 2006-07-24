#include "PedestrianCompliance.h"



namespace synthese
{
namespace env
{



PedestrianCompliance::PedestrianCompliance (const uid& id, 
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
