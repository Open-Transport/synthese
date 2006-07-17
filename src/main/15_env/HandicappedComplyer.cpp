#include "HandicappedComplyer.h"



namespace synthese
{
namespace env
{



HandicappedComplyer::HandicappedComplyer (const HandicappedComplyer* parent, 
			    HandicappedCompliance* handicappedCompliance)
    : _parent (parent)
    , _handicappedCompliance (handicappedCompliance)
{
}


HandicappedComplyer::~HandicappedComplyer ()
{
}





const HandicappedCompliance* 
HandicappedComplyer::getHandicappedCompliance () const
{
    if (_handicappedCompliance != 0) return _handicappedCompliance;
    return (_parent != 0) ? _parent->getHandicappedCompliance () : 0;
}



void 
HandicappedComplyer::setHandicappedCompliance (HandicappedCompliance* handicappedCompliance)
{
    _handicappedCompliance = handicappedCompliance;
}




}
}
