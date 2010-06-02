
/** Projection tools header.
	@file Projection.h

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

#ifndef SYNTHESE_GEOGRAPHY_PROJECTION_H
#define SYNTHESE_GEOGRAPHY_PROJECTION_H

#include "GeoPoint.h"

#include <iostream>


namespace synthese
{
	namespace geometry
	{
		class Point2D;
	}


	namespace geography
	{
		/// @ingroup m09
		typedef enum
		{
			LAMBERT_93,
			LAMBERT_II,
			LAMBERT_IIe
		} LambertOrigin;

		typedef struct {
			double e; 
			double n; 
			double C; 
			double lambdac; 
			double Xs; 
			double Ys; 
		} LambertParams;

		typedef struct {
			double lambda; // Longitude
			double phi;    // Latitude
			double he;     // Height above ellipsoid
		} LatLonHe;

		typedef struct {
			double x; 
			double y; 
			double z; 
		} XYZ;

		double ALG0001 (double phi, double e);
		double ALG0002 (double L, double e, double epsilon);
		geometry::Point2D ALG0003(double lambda, double phi, double e, double Xs, double Ys, double c, double n, double lambdac);
		LatLonHe ALG0004(double X, double Y, double n, double c, double Xs, double Ys, double lambdac, double e, double epsilon);
		XYZ	ALG0009 (double lambda, double phi, double he, double a, double e);
		LatLonHe ALG0012 (double X,double Y,double Z,double a,double e,double epsilon);
		XYZ	ALG0013 (double Tx,double Ty,double Tz,double D,double Rx,double Ry,double Rz, const XYZ& U);
		LambertParams ALG0019 (double lambda0,double phi0,double k0,double X0,double Y0,double a,double e);
		double ALG0021(double phi,double a,double e);
		LambertParams ALG0054 (double lambda0,double phi0,double X0,double Y0,double phi1,double phi2,double a,double e);



		//////////////////////////////////////////////////////////////////
		/// WGS84 point generation from LambertII.
		/// @param point Lambert II point to convert
		/// @return WGS84 converted point
		/// @ingroup m32
		///
		/// http://www.ign.fr/telechargement/MPro/geodesie/CIRCE/transfo.pdf
		/// http://www.ign.fr/telechargement/MPro/geodesie/CIRCE/NTG_80.pdf
		/// <a href="include/14_geography/NTG_71.pdf">Algorithm documentation</a>
		GeoPoint WGS84FromLambert(
			const geometry::Point2D& point,
			const LambertOrigin orig = LAMBERT_IIe
		);

		const geometry::Point2D LambertFromWGS84(
			const GeoPoint& point,
			const LambertOrigin orig = LAMBERT_IIe
		);


		//////////////////////////////////////////////////////////////////////////
		/// WGS84 point generation from Swiss MN03 projection (CH-1903)
		/// @return WGS84 converted point
		/// @ingroup m32
		/// @date 2010
		/// @since 3.1.18
		/// @author Hugues Romain
		//////////////////////////////////////////////////////////////////////////
		/// http://www.swisstopo.admin.ch/internet/swisstopo/en/home/products/software/products/skripts.html
		/// <a href="include/14_geography/ch1903wgs84en.pdf">Algorithm documentation (EN)</a>
		/// <a href="include/14_geography/ch1903wgs84fr.pdf">Algorithm documentation (FR)</a>
		GeoPoint WGS84FromCH1903(
			const geometry::Point2D& point
		);


		geometry::Point2D CH1903FromWGS84(
			GeoPoint point
		);
	}
}

#endif
