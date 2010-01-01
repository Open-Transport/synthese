
/** Point2D class header.
	@file Point2D.h

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


#ifndef SYNTHESE_GEOMETRY_POINT2D_H
#define SYNTHESE_GEOMETRY_POINT2D_H


#include <iostream>

#include "01_util/Constants.h"

namespace synthese
{
namespace geometry
{

/** Two dimensions point.
	@ingroup m06
*/
class Point2D
{
private:

    double _x;
    double _y;

public:

    Point2D (double x = UNKNOWN_VALUE, double y = UNKNOWN_VALUE);
    virtual ~Point2D () {}


    //! @name Getters
    //@{
    double getX () const {
        return _x;
    }
    double getY () const {
        return _y;
    }
    //@}

    //! @name Queries
    //@{
    bool isUnknown() const;
    double getDistanceTo(const Point2D& op) const;
    //@}

    //! @name Setters
    //@{
    void setXY (double x = UNKNOWN_VALUE, double y = UNKNOWN_VALUE);
    //@}
};


bool operator== ( const Point2D& op1, const Point2D& op2 );
bool operator!= ( const Point2D& op1, const Point2D& op2 );

std::ostream& operator<< ( std::ostream& os, const Point2D& op );
}
}

#endif
