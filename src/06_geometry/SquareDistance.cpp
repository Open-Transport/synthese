
/** SquareDistance class implementation.
	@file SquareDistance.cpp

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

#include "SquareDistance.h"

#include "06_geometry/Point2D.h"

#include <cmath>

using namespace std;

namespace synthese
{
	namespace geometry
	{



		SquareDistance::SquareDistance( long int squareDistance )
			: _squareDistance (squareDistance)
		{
		}



		SquareDistance::SquareDistance( const Point2D& Point1, 
						const Point2D& Point2 )
		{
			setFromPoints( Point1, Point2 );
		}



		long int 
		SquareDistance::getSquareDistance () const
		{
			return _squareDistance;
		}




		long int SquareDistance::getDistance() const
		{
			return static_cast<long int>(sqrt(static_cast<double>(_squareDistance)));
		}




		void 
		SquareDistance::setSquareDistance ( long int squareDistance )
		{
			_squareDistance = squareDistance;
		}




		void SquareDistance::setFromDistance ( long int distance )
		{
			_squareDistance = static_cast<long int>( distance * distance );
		}



		int SquareDistance::operator < ( const SquareDistance& op ) const
		{
			return abs(static_cast<float>(op.getSquareDistance () - getSquareDistance ())) > 4;
		}



		int SquareDistance::operator == ( const SquareDistance& op ) const
		{
			return abs(static_cast<float>(op.getSquareDistance () - getSquareDistance ())) <= 4;
		}




		void SquareDistance::setFromPoints( const Point2D& point1, const Point2D& point2 )
		{
			if ( point1.isUnknown () || point2.isUnknown () )
				setSquareDistance( UNKNOWN_VALUE );
			else
			{
				int deltaX((point2.getX() - point1.getX()) / 1000);
				int deltaY((point2.getY() - point1.getY()) / 1000);
				setSquareDistance(deltaX * deltaX + deltaY * deltaY);
			}
		}



		bool 
		SquareDistance::isUnknown () const
		{
			return _squareDistance == UNKNOWN_VALUE;
		}
	}
}
