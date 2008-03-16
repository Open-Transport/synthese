
/** DrawableLineComparator class header.
	@file DrawableLineComparator.h

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

#ifndef SYNTHESE_CARTO_DRAWABLELINECOMPARATOR_H
#define SYNTHESE_CARTO_DRAWABLELINECOMPARATOR_H

#include "06_geometry/Point2D.h"

#include <vector>

namespace synthese
{
namespace map
{

class DrawableLine;



class DrawableLineComparator
{
private:	

    const DrawableLine* _reference;
    const geometry::Point2D _referencePoint;
    
    geometry::Point2D _point;
    
    
    int firstIndexOf (const geometry::Point2D& point, 
      const std::vector<geometry::Point2D>& points) const;
	
public:

    DrawableLineComparator(const DrawableLine* reference, 
			   const geometry::Point2D& referencePoint, 
			   const geometry::Point2D& point);
    
    ~DrawableLineComparator();
	
    void setPoint (const geometry::Point2D& point);


    std::pair<double, double>
	calculateStartAngles (const std::vector<geometry::Point2D>& points1, int index1,
			      const std::vector<geometry::Point2D>& points2, int index2) const;

    std::pair<double, double>
	calculateEndAngles (const std::vector<geometry::Point2D>& points1, int index1,
			    const std::vector<geometry::Point2D>& points2, int index2) const;


	
    double calculateStartAngleAtIndex (
	const std::vector<geometry::Point2D>& points, 
	int index) const;
    
    double calculateEndAngleAtIndex (
	const std::vector<geometry::Point2D>& points, 
	int index) const;
	
    int operator() (const DrawableLine* bl1, const DrawableLine* bl2) const;
	
};

}
}

#endif

