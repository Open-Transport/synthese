
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
		//////////////////////////////////////////////////////////////////
		/// WGS84 point generation from LambertIIetendu.
		/// @param point Lambert IIetendu point to convert
		/// @return WGS84 converted point
		/// @ingroup m32
		///
		/// http://www.ign.fr/telechargement/MPro/geodesie/CIRCE/transfo.pdf
		/// http://www.ign.fr/telechargement/MPro/geodesie/CIRCE/NTG_80.pdf
		/// This method uses proj library
		GeoPoint WGS84FromLambert(
			const geometry::Point2D& point
		);

		const geometry::Point2D LambertFromWGS84(
			const GeoPoint& point
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
