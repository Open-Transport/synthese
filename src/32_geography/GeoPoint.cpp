
/** GeoPoint class implementation.
	@file GeoPoint.cpp

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


#include "GeoPoint.h"
#include "GeographyModule.h"
#include "CoordinatesSystem.hpp"

#include <proj_api.h>
#include <geos/algorithm/Angle.h>

using namespace std;
using namespace geos::algorithm;
using namespace geos::geom;

namespace synthese
{
	namespace geography
	{
		const string GeoPoint::_WGS84_CODE("EPSG:4326");

		GeoPoint::GeoPoint(
			double longitude,
			double latitude,
			double ellipsoidHeight
		):	_longitude (longitude),
			_latitude (latitude),
			_ellipsoidHeight(ellipsoidHeight),
			_coordinatesSystem(GeographyModule::GetInstanceCoordinatesSystem()),
			Coordinate(Angle::toRadians(longitude), Angle::toRadians(latitude))
		{
			// projection
			pj_transform(
				CoordinatesSystem::GetCoordinatesSystem(_WGS84_CODE).getProjObject(),
				_coordinatesSystem.getProjObject(),
				1, 1,
				&x, &y, NULL
			);
		}



		GeoPoint::GeoPoint(
			const GeoPoint& point,
			const CoordinatesSystem& coordinatesSystem
		):	Coordinate(Angle::toRadians(point._longitude), Angle::toRadians(point._latitude)),
			_latitude(point._latitude),
			_longitude(point._longitude),
			_ellipsoidHeight(point._ellipsoidHeight),
			_coordinatesSystem(coordinatesSystem)
		{
			// projection
			pj_transform(
				CoordinatesSystem::GetCoordinatesSystem(_WGS84_CODE).getProjObject(),
				_coordinatesSystem.getProjObject(),
				1, 1,
				&x, &y, NULL
			);
		}


		GeoPoint::GeoPoint(
			const geos::geom::Coordinate& coordinate,
			const CoordinatesSystem& coordinatesSystem
		):	_latitude(coordinate.y),
			_longitude(coordinate.x),
			_ellipsoidHeight(0),
			_coordinatesSystem(GeographyModule::GetInstanceCoordinatesSystem())
		{
			//transformation into wgs84
			pj_transform(
				coordinatesSystem.getProjObject(),
				CoordinatesSystem::GetCoordinatesSystem(_WGS84_CODE).getProjObject(),
				1, 1,
				&_longitude, &_latitude, NULL
			);
			
			//projection
			x = _longitude;
			y = _latitude;
			pj_transform(
				CoordinatesSystem::GetCoordinatesSystem(_WGS84_CODE).getProjObject(),
				_coordinatesSystem.getProjObject(),
				1, 1,
				&x, &y, NULL
			);

			// Storage into degrees
			_latitude = Angle::toDegrees(_latitude);
			_longitude = Angle::toDegrees(_longitude);
		}



		GeoPoint::GeoPoint(
			const geos::geom::Coordinate& coordinate
		):	Coordinate(coordinate),
			_longitude(coordinate.x),
			_latitude(coordinate.y),
			_ellipsoidHeight(0),
			_coordinatesSystem(GeographyModule::GetInstanceCoordinatesSystem())
		{
			//transformation into wgs84
			pj_transform(
				_coordinatesSystem.getProjObject(),
				CoordinatesSystem::GetCoordinatesSystem(_WGS84_CODE).getProjObject(),
				1, 1,
				&_longitude, &_latitude, NULL
			);

			// Storage into degrees
			_latitude = Angle::toDegrees(_latitude);
			_longitude = Angle::toDegrees(_longitude);
		}



		GeoPoint::GeoPoint():
			_longitude(0),
			_latitude(0),
			_coordinatesSystem(GeographyModule::GetInstanceCoordinatesSystem())
		{
			setNull();
		}



		void GeoPoint::operator=( const GeoPoint& point )
		{
			_longitude = point._longitude;
			_latitude = point._latitude;
			x = point.x;
			y = point.y;
		}



		bool
		operator== ( const GeoPoint& op1, const GeoPoint& op2 )
		{
			return 
			(op1.getLatitude () == op2.getLatitude ()) && 
			(op1.getLongitude () == op2.getLongitude ()) && 
			(op1.getEllipsoidHeight () == op2.getEllipsoidHeight ());
		    
		}



		bool
		operator!= ( const GeoPoint& op1, const GeoPoint& op2 )
		{
			return !(op1 == op2);
		}



		std::ostream&
		operator<< ( std::ostream& os, const GeoPoint& op )
		{
			os << "(" << op.getLatitude () << "," << op.getLongitude () << "," << op.getEllipsoidHeight () << ")";
			return os;
		}



	}
}

