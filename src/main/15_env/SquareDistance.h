
/** SquareDistance class header.
	@file SquareDistance.h

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

#ifndef SYNTHESE_ENV_SQUAREDISTANCE_H
#define SYNTHESE_ENV_SQUAREDISTANCE_H

#include <cmath>

#include "01_util/Constants.h"



namespace synthese
{
namespace env
{

    class Point;


/** Square of distance between two points, approximated to 2kms.
@ingroup m15

This class is used to store squre distances between point coordinates in kilometers.
The approximation is aimed at fastening intensive calculation that is needed by recursive algorithm.

Classical arithmetic operators take into account this inaccuracy :
  - equality is true even if the result may be true.
  - strict inequality operators return true if the result is gauranteed to be true.
 
This way, a partial order is defined :
  - an unknown distance is equal to UNKNOWN_VALUE
  - two line stops belonging to the same group have a zero distance between them
  - if two line stops have their isobarycenters distant of less than \f$ \sqrt{2} \f$ km, they have a neglictable square distance.
  - if two line stops have their isobarycenters distant of more than \f$ \sqrt{2} \f$ km, a square distance can be consistently compared with another square distance.
  
*/
class SquareDistance
{
	long int _squareDistance; //!< Square of distance in km

    public:
        SquareDistance( long int value = UNKNOWN_VALUE );
        SquareDistance( const Point&, const Point& );

	//! @name Getters/Setters
	//@{
        long int getSquareDistance () const;
        void setSquareDistance ( long int squareDistance );
	//@}


	//! @name Query methods.
	//@{
        long int getDistance () const;
        bool isUnknown () const;
	//@}

	//! @name Update methods.
	//@{
        void setDistance (long int distance);


		/** Updates this square distance with the square distance between two points.
			@param point1 First point
			@param point2 Second point
	 
			If one point has unknow coordinates, the resulting square distance is
			unknown as well.
		*/
        void setFromPoints ( const Point& point1, const Point& point2 );
	//@}



	/** Inferior or equal operator.
	  @param op Square distance to compare.
	  @return true if the square distance to compare is likely to be 
	  superior or equal to this square distance taking into account inaccuracy.
	*/
	int operator <= ( const SquareDistance& op ) const;


	/** Strictly inferior operator.
	  @param op Square distance to compare.
	  @return true if the square distance is guaranteed to be superior  
	  to this square distance taking into account inaccuracy.
	*/
	int operator < ( const SquareDistance& op ) const;


	/** Equality operator.
	  @param op Square distance to compare.
	  @return true if both square distances are likely to be equal 
	  taking into account inaccuracy.
	*/
	int operator == ( const SquareDistance& op ) const;
};


}
}

#endif

