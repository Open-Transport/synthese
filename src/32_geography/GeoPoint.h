
/** GeoPoint class header.
	@file GeoPoint.h

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

#ifndef SYNTHESE_GEOGRAPHY_GEOPOINT_H
#define SYNTHESE_GEOGRAPHY_GEOPOINT_H

#include <iostream>
#include <geos/geom/Coordinate.h>

namespace synthese
{
	namespace db
	{
		class CoordinatesSystem;
	}

	namespace geography
	{
		/** Geographical point class.
			Geodetic point, based on WGS84 System (used by GPSs), usually expressed as 
			latitude, longitude, and ellipsoid height (height above WGS84 reference ellipsoid -
			which is GRS80).
		    

			Check :
			http://www.mentorsoftwareinc.com/resource/glossary.htm
			http://en.wikipedia.org/wiki/WGS84
			http://en.wikipedia.org/wiki/GRS_80
		    
			@ingroup m32
		*/
		class GeoPoint:
			public geos::geom::Coordinate
		{
		private:

			double _longitude;        //!< Longitude in degrees.
			double _latitude;         //!< Latitude in degrees.
			double _ellipsoidHeight;  //!< Height above ellipsoid in meters.
			const db::CoordinatesSystem& _coordinatesSystem;

		public:

			GeoPoint();

			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			GeoPoint(
				double longitude,
				double latitude,
				double ellipsoidHeight = 0
			);

			//////////////////////////////////////////////////////////////////////////
			/// Constructor by conversion of a projected point.
			/// @param coordinate projected point to import
			/// @param coordinatesSystem projection used to create the point to import
			GeoPoint(
				const geos::geom::Coordinate& coordinate,
				const db::CoordinatesSystem& coordinatesSystem
			);

			//////////////////////////////////////////////////////////////////////////
			/// Constructor by conversion of a projected point in the instance
			/// coordinates system.
			/// @param coordinate projected point to import
			GeoPoint(
				const geos::geom::Coordinate& coordinate
			);

			//////////////////////////////////////////////////////////////////////////
			/// Constructor by conversion of a GeoPoint object into an other
			///	coordinates system.
			/// @param point point to convert
			/// @param coordinatesSystem coordinates system to use
			GeoPoint(
				const GeoPoint& point,
				const db::CoordinatesSystem& coordinatesSystem
			);


			//////////////////////////////////////////////////////////////////////////
			/// Copy of an other point. The two points must be in the same coordinates
			/// systems.
			/// @param point point to convert
			void operator=(
				const GeoPoint& point
			);

			~GeoPoint () {}


			//! @name Getters
			//@{
				/// Latitude (phi).
				double getLatitude () const { return _latitude; }

				/// Longitude (lambda).
				double getLongitude () const { return _longitude; }

				/// Height above GRS_80 ellipsoid (he).
				///	This is NOT the commonly known concept of altitude 
				///	(even if it could be a very rough approximation +-100m) !  
				double getEllipsoidHeight () const { return _ellipsoidHeight; }

				const db::CoordinatesSystem& getCoordinatesSystem() const { return _coordinatesSystem; }
			//@}

			//! @name Setters
			//@{
				void setEllipsoidHeight (double ellipsoidHeight) { _ellipsoidHeight = ellipsoidHeight; }
			//@}
		};


		bool operator== ( const GeoPoint& op1, const GeoPoint& op2 );
		bool operator!= ( const GeoPoint& op1, const GeoPoint& op2 );

		std::ostream& operator<< ( std::ostream& os, const GeoPoint& op );

	}
}

#endif
