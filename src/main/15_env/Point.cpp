#include "Point.h"


namespace synthese
{
namespace env
{



Point::Point(long int x, long int y)
    : _xM (x)
    , _yM (y)
{
    setX ( x );
    setY ( y );
}



Point::~Point()
{
}



long int 
Point::getX () const
{
    return _xM;
}



long int Point::getY () const
{
    return _yM;
}



bool Point::isUnknown () const
{
    return _xKm == UNKNOWN_VALUE || _yKm == UNKNOWN_VALUE;
}



void Point::setX( const long int _xKmM )
{
    if ( _xKmM == UNKNOWN_VALUE )
        _xKm = UNKNOWN_VALUE;
    else
    {
        _xKm = ( short int ) ( _xKmM / 1000 );
        _xM = ( short int ) ( _xKmM % 1000 );
    }
}



void Point::setY( const long int _yKmM )
{
    if ( _yKmM == UNKNOWN_VALUE )
        _yKm = UNKNOWN_VALUE;
    else
    {
        _yKm = ( short int ) ( _yKmM / 1000 );
        _yM = ( short int ) ( _yKmM % 1000 );
    }
}



bool 
Point::operator== ( const Point& op ) const
{
    if ( op.isUnknown () || isUnknown () ) return false;
    return op._xKm == _xKm && op._yKm == _yKm;
}



}
}

