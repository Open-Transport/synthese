#include "Point.h"

#include <cmath>


namespace synthese
{
namespace env
{



Point::Point(double x, double y)
    : _xM (x)
    , _yM (y)
{
    setX ( x );
    setY ( y );
}



Point::~Point()
{
}



double
Point::getX () const
{
    return _xM;
}



double
Point::getY () const
{
    return _yM;
}



bool 
Point::isUnknown () const
{
    return _xKm == UNKNOWN_VALUE || _yKm == UNKNOWN_VALUE;
}



void 
Point::setX( double xM )
{
    if ( xM == UNKNOWN_VALUE )
        _xKm = UNKNOWN_VALUE;
    else
    {
        _xKm = ( short int ) ( xM / 1000 );
        _xM = xM;
    }
}



void 
Point::setY( double yM )
{
    if ( yM == UNKNOWN_VALUE )
        _yKm = UNKNOWN_VALUE;
    else
    {
        _yKm = ( short int ) ( yM / 1000 );
        _yM = yM;
    }
}




double 
Point::distanceTo (const synthese::env::Point& p) const
{
    return sqrt ((p.getX() - getX()) * (p.getX() - getX()) + 
		 (p.getY() - getY()) * (p.getY() - getY()));
}



bool 
Point::operator == ( const Point& op ) const
{
    if ( op.isUnknown () || isUnknown () ) return false;
    return op._xM == _xM && op._yM == _yM;
}


bool 
Point::operator != ( const Point& op ) const
{
    return ! operator==(op);
}






}
}


