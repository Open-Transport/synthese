#include "BikeComplyer.h"



namespace synthese
{
namespace env
{



BikeComplyer::BikeComplyer (const BikeComplyer* parent, 
			    BikeCompliance* bikeCompliance)
    : _parent (parent)
    , _bikeCompliance (bikeCompliance)
{
}


BikeComplyer::~BikeComplyer ()
{
}





const BikeCompliance* 
BikeComplyer::getBikeCompliance () const
{
    if (_bikeCompliance != 0) return _bikeCompliance;
    return (_parent != 0) ? _parent->getBikeCompliance () : 0;
}



void 
BikeComplyer::setBikeCompliance (BikeCompliance* bikeCompliance)
{
    _bikeCompliance = bikeCompliance;
}




}
}
