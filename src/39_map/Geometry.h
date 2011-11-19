
/** Geometry class header.
	@file Geometry.h

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

#ifndef SYNTHESE_CARTO_GEOMETRY_H
#define SYNTHESE_CARTO_GEOMETRY_H

#include <vector>
#include <geos/geom/Coordinate.h>

namespace synthese
{
namespace map
{




    /** Calculates the oriented angle formed by vectors -ba> and -bc>.
     * @return The angle in radians on [-PI..PI] domain.
     */
    double calculateAngle (const geos::geom::Coordinate& a,
			   const geos::geom::Coordinate& b,
			   const geos::geom::Coordinate& c);


    /** Calculates the euclidian distance between points a and b.
     * @return The calculated distance.
     */
    double calculateDistance (const geos::geom::Coordinate& a,
			      const geos::geom::Coordinate& b);


    /** Returns symetric of a regarding b
     */
    geos::geom::Coordinate calculateSymetric (const geos::geom::Coordinate& a,
					    const geos::geom::Coordinate& b);


    std::pair<double, double>
	calculateAffineEquation (const geos::geom::Coordinate& p0,
				 const geos::geom::Coordinate& p1);




    /** Calculates the intersection between two affines defined by
     * y = a1.x + b1
     * y = a2.x + b2
     *
     * @param a1
     * @param b1
     * @param a2
     * @param b2
     * @return The intersection point.
     */
    geos::geom::Coordinate calculateIntersection (double a1,
						double b1,
						double a2,
						double b2);


    /** Calculates the intersection between two affines.
     * The first affine is defined by two points  : p0 and p1.
     * The second affine is defined by two points : p2 and p3.
     *
     @param p0
     @param p1
     @param p2
     @param p3
     @return The intersection point.
    */
    geos::geom::Coordinate calculateIntersection (
	const geos::geom::Coordinate& p0,
	const geos::geom::Coordinate& p1,
	const geos::geom::Coordinate& p2,
	const geos::geom::Coordinate& p3);



}
}

#endif



