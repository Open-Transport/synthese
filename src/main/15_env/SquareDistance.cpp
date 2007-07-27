#include "SquareDistance.h"

#include "Point.h"


namespace synthese
{
namespace env
{



SquareDistance::SquareDistance( long int squareDistance )
    : _squareDistance (squareDistance)
{
}



SquareDistance::SquareDistance( const Point& Point1, 
				const Point& Point2 )
{
    setFromPoints( Point1, Point2 );
}



long int 
SquareDistance::getSquareDistance () const
{
    return _squareDistance;
}




long int 
SquareDistance::getDistance() const
{
    return (long int) sqrt( ( double ) _squareDistance );
}




void 
SquareDistance::setSquareDistance ( long int squareDistance )
{
    _squareDistance = squareDistance;
}




void SquareDistance::setDistance ( long int distance )
{
    _squareDistance = (long int) ( distance * distance );
}



int SquareDistance::operator < ( const SquareDistance& op ) const
{
    return abs(op.getSquareDistance () - getSquareDistance ()) > 4;
}



int SquareDistance::operator == ( const SquareDistance& op ) const
{
    return abs(op.getSquareDistance () - getSquareDistance ()) <= 4;
}




void SquareDistance::setFromPoints( const Point& point1, const Point& point2 )
{
    if ( point1.isUnknown () || point2.isUnknown () )
        setSquareDistance( UNKNOWN_VALUE );
    else
        setSquareDistance( ( point1._xKm - point2._xKm ) * 
			   ( point1._xKm - point2._xKm ) + 
			   ( point1._yKm - point2._yKm ) * 
			   ( point1._yKm - point2._yKm ) );
}



bool 
SquareDistance::isUnknown () const
{
    return _squareDistance == UNKNOWN_VALUE;
}


}
}


