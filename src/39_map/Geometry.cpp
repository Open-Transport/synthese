
/** Geometry class implementation.
	@file Geometry.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#include <cmath>

using namespace std;

namespace synthese
{
	using namespace geometry;
	
namespace map
{


double 
calculateAngle (const Point2D& a, const Point2D& b, const Point2D& c)
{
    double angle = atan2 (c.getY()-b.getY(), c.getX()-b.getX()) - 
	atan2 (a.getY()-b.getY(), a.getX()-b.getX());

    if (angle > M_PI) angle = angle - 2*M_PI;
    if (angle < -M_PI) angle = angle + 2*M_PI;
    
    return angle;	
}



double 
calculateDistance (const Point2D& a, const Point2D& b) 
{
    return sqrt((b.getX()-a.getX())*(b.getX()-a.getX()) + 
		(b.getY()-a.getY())*(b.getY()-a.getY()));
}



double 
toDegrees (double angle)
{
    return 180.0 * angle / M_PI;
}



Point2D  
calculateSymetric (const Point2D& a, const Point2D& b)
{
    double deltax = b.getX() - a.getX();
    double deltay = b.getY() - a.getY();
    return Point2D (b.getX() + deltax, b.getY() + deltay);
} 




Point2D 
calculateIntersection (double a1, double b1, double a2, double b2)
{
    // Intersection of :
    // y = a1.x + b1
    // y = a2.x + b2
    
    if (a1 == a2) throw "No intersection";
    
    return Point2D ( (b2 - b1) / (a1 -a2),
		     (b2*a1 - a2*b1) / (a1 - a2) );
}




std::pair<double, double>
calculateAffineEquation (const Point2D& p0, const Point2D& p1) {
    // Given 2 points return a and b so that y = a.x + b
    // goes through p0 and p1
    double u0 = p1.getX() - p0.getX ();
    double v0 = p1.getY() - p0.getY ();

    double a = v0 / u0;
    double b = p0.getY () - a * p0.getX ();
	
    return std::pair<double, double> (a,b);	
}





Point2D 
calculateIntersection (const Point2D& p0, const Point2D& p1, 
		       const Point2D& p2, const Point2D& p3)
{
    double u0 = p1.getX() - p0.getX ();
    double v0 = p1.getY() - p0.getY ();
    
    double u1 = p3.getX() - p2.getX ();
    double v1 = p3.getY() - p2.getY ();
    
    double a1 = v0 / u0;
    double b1 = p0.getY () - a1 * p0.getX ();
    
    double a2 = v1 / u1;
    double b2 = p2.getY () - a2 * p2.getX ();
    
    return calculateIntersection (a1, b1, a2, b2);
}






}
}









