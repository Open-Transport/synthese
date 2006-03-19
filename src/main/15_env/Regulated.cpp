#include "Regulated.h"



namespace synthese
{
namespace env
{



Regulated::Regulated (const Regulated* parent, 
		      ReservationRule* reservationRule,
		      PedestrianCompliance* pedestrianCompliance,
		      HandicappedCompliance* handicappedCompliance,
		      BikeCompliance* bikeCompliance)
    : _parent (parent)
    , _reservationRule (reservationRule)
    , _pedestrianCompliance (pedestrianCompliance)
    , _handicappedCompliance (handicappedCompliance)
    , _bikeCompliance (bikeCompliance)
{
}


Regulated::~Regulated ()
{
}




const ReservationRule* 
Regulated::getReservationRule () const
{
    if (_reservationRule != 0) return _reservationRule;
    return (_parent != 0) ? _parent->getReservationRule () : 0;
}



void 
Regulated::setReservationRule (ReservationRule* reservationRule)
{
    _reservationRule = reservationRule;
}




const PedestrianCompliance* 
Regulated::getPedestrianCompliance () const
{
    if (_pedestrianCompliance != 0) return _pedestrianCompliance;
    return (_parent != 0) ? _parent->getPedestrianCompliance () : 0;
}


void 
Regulated::setPedestrianCompliance (PedestrianCompliance* pedestrianCompliance)
{
    _pedestrianCompliance = pedestrianCompliance;
}





const HandicappedCompliance* 
Regulated::getHandicappedCompliance () const
{
    if (_handicappedCompliance != 0) return _handicappedCompliance;
    return (_parent != 0) ? _parent->getHandicappedCompliance () : 0;
}



void 
Regulated::setHandicappedCompliance (HandicappedCompliance* handicappedCompliance)
{
    _handicappedCompliance = handicappedCompliance;
}





const BikeCompliance* 
Regulated::getBikeCompliance () const
{
    if (_bikeCompliance != 0) return _bikeCompliance;
    return (_parent != 0) ? _parent->getBikeCompliance () : 0;
}



void 
Regulated::setBikeCompliance (BikeCompliance* bikeCompliance)
{
    _bikeCompliance = bikeCompliance;
}




}
}
