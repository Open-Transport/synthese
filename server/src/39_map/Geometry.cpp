
/** Geometry class implementation.
	@file Geometry.cpp

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

#include "Geometry.h"

using namespace geos::geom;
using namespace std;

namespace synthese
{
namespace map
{


double
calculateAngle (const Coordinate& a, const Coordinate& b, const Coordinate& c)
{
    double angle = atan2 (c.y-b.y, c.x-b.x) -
	atan2 (a.y-b.y, a.x-b.x);

    if (angle > M_PI) angle = angle - 2*M_PI;
    if (angle < -M_PI) angle = angle + 2*M_PI;

    return angle;
}



double
calculateDistance (const Coordinate& a, const Coordinate& b)
{
    return sqrt((b.x-a.x)*(b.x-a.x) +
		(b.y-a.y)*(b.y-a.y));
}



Coordinate
calculateSymetric (const Coordinate& a, const Coordinate& b)
{
    double deltax = b.x - a.x;
    double deltay = b.y - a.y;
    return Coordinate (b.x + deltax, b.x + deltay);
}




Coordinate
calculateIntersection (double a1, double b1, double a2, double b2)
{
    // Intersection of :
    // y = a1.x + b1
    // y = a2.x + b2

    if (a1 == a2) throw "No intersection";

    return Coordinate ( (b2 - b1) / (a1 -a2),
		     (b2*a1 - a2*b1) / (a1 - a2) );
}




std::pair<double, double>
calculateAffineEquation (const Coordinate& p0, const Coordinate& p1) {
    // Given 2 points return a and b so that y = a.x + b
    // goes through p0 and p1
    double u0 = p1.x - p0.x;
    double v0 = p1.y - p0.y;

    double a = v0 / u0;
    double b = p0.y - a * p0.x;

    return std::pair<double, double> (a,b);
}





Coordinate
calculateIntersection (const Coordinate& p0, const Coordinate& p1,
		       const Coordinate& p2, const Coordinate& p3)
{
    double u0 = p1.x - p0.x;
    double v0 = p1.y - p0.y;

    double u1 = p3.x - p2.x;
    double v1 = p3.y - p2.y;

    double a1 = v0 / u0;
    double b1 = p0.y - a1 * p0.x;

    double a2 = v1 / u1;
    double b2 = p2.y - a2 * p2.x;

    return calculateIntersection (a1, b1, a2, b2);
}
}
}
