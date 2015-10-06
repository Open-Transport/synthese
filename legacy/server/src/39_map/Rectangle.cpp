
/** Rectangle class implementation.
	@file Rectangle.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "Rectangle.h"

#include <geos/geom/Coordinate.h>

using namespace geos::geom;

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
    Rectangle::contains (const Coordinate& p) const
    {
	return (p.x >= _x) && (p.x < _x+_width)
	    && (p.y >= _y) && (p.y < _y+_height);
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


