#include "Rectangle.h"

#include "15_env/Point.h"

using synthese::env::Point;


namespace synthese
{
namespace map
{


    Rectangle::Rectangle(double x, double y, double width, double height) 
	: _x (x), _y (y), _width (width), _height (height)
    {
	
    }


    Rectangle::Rectangle(const Rectangle& ref)
	: _x (ref._x), _y (ref._y), _width (ref._width), _height (ref._height)
    {
    }	

    Rectangle::~Rectangle()
    {
	
    }

    bool
    Rectangle::contains (const Point& p) const
    {
	return (p.getX() >= _x) && (p.getX() < _x+_width) 
	    && (p.getY() >= _y) && (p.getY() < _y+_height);
    }


		
    Rectangle& 
    Rectangle::operator= (const Rectangle& rhs) 
    {
	if (*this == rhs) return *this; // Protect from self-assignment !
	_x = rhs._x;
	_y = rhs._y;
	_width = rhs._width;
	_height = rhs._height;	
	return (*this);
    }


    bool 
    Rectangle::operator== (const Rectangle& rhs) const 
    {
	return ((_x == rhs._x) && (_y == rhs._y) 
		&& (_width == rhs._width) && (_height == rhs._height));
    }


}
}


