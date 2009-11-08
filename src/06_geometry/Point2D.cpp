
/** Point2D class implementation.
	@file Point2D.cpp

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

#include "Point2D.h"
#include <cmath>

namespace synthese
{
	namespace geometry
	{



		Point2D::Point2D (double x, double y)
		{
			setXY(x, y);
		}

		void Point2D::setXY( double x /*= UNKNOWN_VALUE*/, double y /*= UNKNOWN_VALUE*/ )
		{
			if (x <= 0 || y <= 0)
			{
				_x = UNKNOWN_VALUE;
				_y = UNKNOWN_VALUE;
				return;
			}
			_x = x;
			_y = y;
		}

		bool Point2D::isUnknown() const
		{
			return _x == UNKNOWN_VALUE;
		}

		double Point2D::getDistanceTo( const Point2D& p ) const
		{
			return sqrt ((p.getX() - getX()) * (p.getX() - getX()) + 
				(p.getY() - getY()) * (p.getY() - getY()));
		}



		    

		bool
		operator== ( const Point2D& op1, const Point2D& op2 )
		{
			return (op1.getX () == op2.getX ()) && 
			(op1.getY () == op2.getY ());
		    
		}


		bool
		operator!= ( const Point2D& op1, const Point2D& op2 )
		{
			return !(op1 == op2);
		}




		std::ostream&
		operator<< ( std::ostream& os, const Point2D& op )
		{
			os << "(" << op.getX () << "," << op.getY () << ")";
			return os;
		}



	}
}

