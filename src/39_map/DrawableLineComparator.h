
/** DrawableLineComparator class header.
	@file DrawableLineComparator.h

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

#ifndef SYNTHESE_CARTO_DRAWABLELINECOMPARATOR_H
#define SYNTHESE_CARTO_DRAWABLELINECOMPARATOR_H

#include <geos/geom/Coordinate.h>
#include <vector>

namespace synthese
{
namespace map
{

class DrawableLine;


/** @ingroup m39 */
class DrawableLineComparator
{
private:

    const DrawableLine* _reference;
    const geos::geom::Coordinate _referencePoint;

    geos::geom::Coordinate _point;


    int firstIndexOf (const geos::geom::Coordinate& point,
      const std::vector<geos::geom::Coordinate>& points) const;

public:

    DrawableLineComparator(const DrawableLine* reference,
			   const geos::geom::Coordinate& referencePoint,
			   const geos::geom::Coordinate& point);

    ~DrawableLineComparator();

    void setPoint (const geos::geom::Coordinate& point);


    std::pair<double, double>
	calculateStartAngles (const std::vector<geos::geom::Coordinate>& points1, int index1,
			      const std::vector<geos::geom::Coordinate>& points2, int index2) const;

    std::pair<double, double>
	calculateEndAngles (const std::vector<geos::geom::Coordinate>& points1, int index1,
			    const std::vector<geos::geom::Coordinate>& points2, int index2) const;



    double calculateStartAngleAtIndex (
	const std::vector<geos::geom::Coordinate>& points,
	int index) const;

    double calculateEndAngleAtIndex (
	const std::vector<geos::geom::Coordinate>& points,
	int index) const;

    int operator() (const DrawableLine* bl1, const DrawableLine* bl2) const;

};

}
}

#endif

