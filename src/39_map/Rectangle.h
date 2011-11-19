
/** Rectangle class header.
	@file Rectangle.h

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

#ifndef SYNTHESE_CARTO_RECTANGLE_H
#define SYNTHESE_CARTO_RECTANGLE_H

namespace geos
{
	namespace geom
	{
		class Coordinate;
	}
}

namespace synthese
{


namespace map
{



/** @ingroup m39 */
class Rectangle
{
private:
	double _x;
	double _y;
	double _width;
	double _height;

public:
	Rectangle(double x, double y, double width, double height);
	Rectangle(const Rectangle& ref);

	virtual ~Rectangle();

	double getX () const { return _x; }
	double getY () const { return _y; }
	double getWidth () const { return _width; }
	double getHeight () const { return _height; }

	bool contains (const geos::geom::Coordinate& p) const;

	Rectangle& operator= (const Rectangle& rhs);
	bool operator== (const Rectangle& rhs) const;

};

}
}

#endif

