#include "PedestrianCompliance.h"



namespace synthese
{
namespace env
{



PedestrianCompliance::PedestrianCompliance (const uid& id, 
				const boost::logic::tribool& compliant,
				const int& capacity)
    : synthese::util::Registrable<uid,PedestrianCompliance> (id)
    , Compliance (compliant, capacity)
{

}


PedestrianCompliance::~PedestrianCompliance()
{
}




}
}
