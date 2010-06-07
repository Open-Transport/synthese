
/** ProjectionTest class implementation.
	@file ProjectionTest.cpp

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

#include <boost/test/auto_unit_test.hpp>

#include "Projection.h"
#include "Point2D.h"

using namespace synthese::geography;
using namespace synthese::geometry;


BOOST_AUTO_TEST_CASE (testFromLambertIIe)
{
    {
		// Toulouse LambertIIe
		Point2D p2d (527674.0, 1845128.0);
		GeoPoint gp = WGS84FromLambert(p2d);
		
		BOOST_CHECK_CLOSE(43.60435799, gp.getLatitude(), 0.00001);
		BOOST_CHECK_CLOSE(1.44199101, gp.getLongitude(), 0.00001);
    }
    {
		// Brest
		Point2D p2d (95151.0, 2398703.0);
		GeoPoint gp = WGS84FromLambert(p2d);

		BOOST_CHECK_CLOSE(48.38984464, gp.getLatitude(), 0.00001);
		BOOST_CHECK_CLOSE(-4.48683053, gp.getLongitude(), 0.00001);
    }
	{
		// Strasbourg
		Point2D p2d (999096.6, 2412064.0);
		GeoPoint gp = WGS84FromLambert(p2d);

		BOOST_CHECK_CLOSE(48.5836, gp.getLatitude(), 0.00001);
		BOOST_CHECK_CLOSE(7.74806, gp.getLongitude(), 0.00001);
	}
}

BOOST_AUTO_TEST_CASE (testToLambertIIe)
{
	{
		// Toulouse LambertIIe
		GeoPoint gp(43.60435799, 1.44199101, 0);
		Point2D p2d(LambertFromWGS84(gp));

		BOOST_CHECK_CLOSE(527674.0, p2d.getX(), 1.0);
		BOOST_CHECK_CLOSE(1845128.0, p2d.getY(), 1.0);
	}
	{
		// Brest LambertIIe
		GeoPoint gp(48.38984464, -4.48683053, 0);
		Point2D p2d(LambertFromWGS84(gp));

		BOOST_CHECK_CLOSE(95151.0, p2d.getX(), 1.0);
		BOOST_CHECK_CLOSE(2398703.0, p2d.getY(), 1.0);
	}
	{
		// Strasbourg
		GeoPoint gp(48.5836, 7.74806, 0);
		Point2D p2d(LambertFromWGS84(gp));

		BOOST_CHECK_CLOSE(999096.6, p2d.getX(), 1.0);
		BOOST_CHECK_CLOSE(2412064.0, p2d.getY(), 1.0);
	}
}
