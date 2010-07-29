
/** Projection tools implementation.
	@file Projection.cpp

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


#include "Projection.h"

#include "Point2D.h"
#include "Point3D.h"
#include "Angle.h"

#include <cmath>
#include <proj_api.h>


namespace synthese
{

	using namespace geometry;

	namespace geography
	{
		const projPJ PROJ_LAMBERTIIE(pj_init_plus("+proj=lcc +towgs84=-168.0000,-60.0000,320.0000 +a=6378249.2000 +rf=293.4660210000000 +pm=2.337229167 +lat_0=46.800000000 +lon_0=0.000000000 +k_0=0.99987742 +lat_1=46.800000000 +x_0=600000.000 +y_0=2200000.000 +units=m +no_defs <>"));
		const projPJ PROJ_WGS84(pj_init_plus("+proj=longlat +towgs84=0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.000000 +a=6378137.0000 +rf=298.2572221010000 +units=m +no_defs <>"));

		GeoPoint WGS84FromLambert(
			const geometry::Point2D& p
		){
			//transformation
			double x(p.getX());
			double y(p.getY());
			pj_transform(PROJ_LAMBERTIIE, PROJ_WGS84, 1, 1, &x, &y, NULL);
			return GeoPoint(ToDegrees(y), ToDegrees(x), 0);
		}



		GeoPoint WGS84FromCH1903( const geometry::Point2D& point )
		{
			// Converts militar to civil and  to unit = 1000km
			// Axiliary values (% Bern)
			double y_aux = (point.getX() - 600000)/1000000;
			double x_aux = (point.getY() - 200000)/1000000;

			// Process lat
			double lat = 16.9023892
				+  3.238272 * x_aux
				-  0.270978 * y_aux * y_aux
				-  0.002528 * x_aux * x_aux
				-  0.0447   * y_aux * y_aux * x_aux
				-  0.0140   * x_aux * x_aux * x_aux;

			// Unit 10000" to 1 " and converts seconds to degrees (dec)
			lat = lat * 100/36;

			// Process long
			double lng = 2.6779094
				+ 4.728982 * y_aux
				+ 0.791484 * y_aux * x_aux
				+ 0.1306   * y_aux * x_aux * x_aux
				- 0.0436   * y_aux * y_aux * y_aux;

			// Unit 10000" to 1 " and converts seconds to degrees (dec)
			lng = lng * 100/36;

			return GeoPoint (lat, lng, 0);
		}



		geometry::Point2D CH1903FromWGS84( GeoPoint point )
		{
/*			// Converts degrees dec to sex
			lat = DecToSexAngle(lat);
			lng = DecToSexAngle(lng);

			// Converts degrees to seconds (sex)
			lat = SexAngleToSeconds(lat);
			lng = SexAngleToSeconds(lng);

			// Axiliary values (% Bern)
			double lat_aux = (lat - 169028.66)/10000;
			double lng_aux = (lng - 26782.5)/10000;

			// Process Y
			double y = 600072.37 
				+ 211455.93 * lng_aux 
				-  10938.51 * lng_aux * lat_aux
				-      0.36 * lng_aux * Math.Pow(lat_aux, 2)
				-     44.54 * Math.Pow(lng_aux, 3);

			return y;

			// Converts degrees dec to sex
			lat = DecToSexAngle(lat);
			lng = DecToSexAngle(lng);

			// Converts degrees to seconds (sex)
			lat = SexAngleToSeconds(lat);
			lng = SexAngleToSeconds(lng);

			// Axiliary values (% Bern)
			double lat_aux = (lat - 169028.66)/10000;
			double lng_aux = (lng - 26782.5)/10000;

			// Process X
			double x = 200147.07
				+ 308807.95 * lat_aux 
				+   3745.25 * Math.Pow(lng_aux, 2)
				+     76.63 * Math.Pow(lat_aux, 2)
				-    194.56 * Math.Pow(lng_aux, 2) * lat_aux
				+    119.79 * Math.Pow(lat_aux, 3);

			return x;
*/
			return Point2D();
		}



		const geometry::Point2D LambertFromWGS84( const GeoPoint& point)
		{
			//transformation
			double x(ToRadians(point.getLongitude()));
			double y(ToRadians(point.getLatitude()));
			pj_transform(PROJ_WGS84, PROJ_LAMBERTIIE, 1, 1, &x, &y, NULL);
			return Point2D(x,y);
		}
	}
}
