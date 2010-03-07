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


		//////////////////////////////////////////////////////////////////
		/// WGS84 point generation from LambertII.
		/// @param point Lambert II point to convert
		/// @return WGS84 converted point
		/// @ingroup m32
		///
		/// http://www.ign.fr/telechargement/MPro/geodesie/CIRCE/transfo.pdf
		/// http://www.ign.fr/telechargement/MPro/geodesie/CIRCE/NTG_80.pdf
		/// http://www.ign.fr/telechargement/MPro/geodesie/CIRCE/NTG_71.pdf
		GeoPoint WGS84FromLambert(
			const geometry::Point2D& point,
			const LambertOrigin orig = LAMBERT_IIe
		);
	}
}

#endif
