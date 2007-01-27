
/** Point class header.
	@file Point.h

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

#ifndef SYNTHESE_ENV_POINT_H
#define SYNTHESE_ENV_POINT_H

#include "01_util/Constants.h"

namespace synthese
{
	namespace env
	{

		/** Lambert II geographical point.
			@ingroup m15 

			Lambert II geographical coordinates are stored as two integers : 
			one for kilometers part, the other for the meter part.

			( \f$ x=x_{km}*1000+x_m \f$ et \f$ y=y_{km}*1000+y_m \f$)
			 
			The goal of this split is to fasten some calculation which do not need metric accuracy.

		*/
		class Point
		{
				friend class SquareDistance;

				double _xM;  //!< Accurate longitude in meters
				double _yM;  //!< Accurate latitude in meters

				short int _xKm; //!< Longitude in kilometers
				short int _yKm; //!< Latitude in kilometers

			public:

				Point(double x = UNKNOWN_VALUE,
					double y = UNKNOWN_VALUE);

				virtual ~Point();

			//! @name Query methods.
			//@{
				bool isUnknown () const;
			//@}

			//! @name Getters/Setters
			//@{
				double getX () const;
				double getY () const;

				/** Longitude setter
				@param x New longitude in meters
			*/
				void setX ( double x);

				/** Latitude setter.
				@param y New latitude in meters
			*/
				void setY ( double y);
			//@}


			/** Calculates the euclidian distance between this point and another point.
			* @return The calculated distance.
			*/
			double distanceTo (const synthese::env::Point& p) const; 


				/** Coordinates comparison operator.
			@param op Point to test
			@return true if meters coordinates are identical, 
			false otherwise or if one coordinate is unknown.
			*/
				bool operator == ( const Point& op ) const;

				/** Coordinates comparison operator.
			@param op Point to test
			@return false if meters coordinates are identical, 
			true otherwise or if one coordinate is unknown.
			*/
				bool operator != ( const Point& op ) const;


		};
	}
}

#endif
