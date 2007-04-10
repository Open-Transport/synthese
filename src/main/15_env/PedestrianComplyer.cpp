#include "PedestrianComplyer.h"



namespace synthese
{
namespace env
{



PedestrianComplyer::PedestrianComplyer (const PedestrianComplyer* parent, 
			    const PedestrianCompliance* pedestrianCompliance)
    : _parent (parent)
    , _pedestrianCompliance (pedestrianCompliance)
{
}


PedestrianComplyer::~PedestrianComplyer ()
{
}





const PedestrianCompliance* 
PedestrianComplyer::getPedestrianCompliance () const
{
    if (_pedestrianCompliance != 0) return _pedestrianCompliance;
    return (_parent != 0) ? _parent->getPedestrianCompliance () : 0;
}



void 
PedestrianComplyer::setPedestrianCompliance (const PedestrianCompliance* pedestrianCompliance)
{
    _pedestrianCompliance = pedestrianCompliance;
}

void PedestrianComplyer::setParent( PedestrianComplyer* parent )
{
	_parent = parent;
}



}
}

