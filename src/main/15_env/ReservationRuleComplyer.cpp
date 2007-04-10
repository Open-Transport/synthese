#include "ReservationRuleComplyer.h"



namespace synthese
{
namespace env
{



ReservationRuleComplyer::ReservationRuleComplyer (const ReservationRuleComplyer* parent, 
						  const ReservationRule* reservationRule)
    : _parent (parent)
    , _reservationRule (reservationRule)
{
}


ReservationRuleComplyer::~ReservationRuleComplyer ()
{
}




const ReservationRule* 
ReservationRuleComplyer::getReservationRule () const
{
    if (_reservationRule != 0) return _reservationRule;
    return (_parent != 0) ? _parent->getReservationRule () : 0;
}



void 
ReservationRuleComplyer::setReservationRule (const ReservationRule* reservationRule)
{
    _reservationRule = reservationRule;
}

void ReservationRuleComplyer::setParent( const ReservationRuleComplyer* parent )
{
	_parent = parent;
}




}
}

