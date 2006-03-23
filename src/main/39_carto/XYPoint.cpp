#include "XYPoint.h"

#include <cmath>


namespace synthese
{
namespace carto
{



XYPoint::XYPoint(double x, double y)
: _x(x), _y(y)
{
}



XYPoint::XYPoint(const XYPoint& ref)
: _x(ref._x), _y(ref._y)
{
}



XYPoint::~XYPoint()
{
}



XYPoint& 
XYPoint::operator= (const XYPoint& rhs)
{
    if (*this == rhs) return *this; // Protect from self-assignment !
	this->_x = rhs._x;
	this->_y = rhs._y;
	return *this;
}


	
bool 
XYPoint::operator== (const XYPoint& rhs) const
{
	return (_x == rhs._x) && (_y == rhs._y);
}



double
XYPoint::distanceTo (const XYPoint& p) const 
{
	return sqrt((p._x - _x)*(p._x - _x) + 
		(p._y - _y)*(p._y - _y));
}







}
}

