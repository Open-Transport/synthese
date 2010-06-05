
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

#include "14_geography/Projection.h"
#include "06_geometry/Point2D.h"

#include <boost/test/auto_unit_test.hpp>
#include <boost/math/special_functions/round.hpp>

using namespace boost::math;


using namespace synthese::geography;
using namespace synthese::geometry;



BOOST_AUTO_TEST_CASE (testFromLambertIIe)
{
    {
	// Toulouse LambertIIe
	Point2D p2d (527674.0, 1845128.0);
	GeoPoint gp = WGS84FromLambert(p2d);
	
	BOOST_CHECK_EQUAL (43604357973336, static_cast<long long int>(1000000000000*gp.getLatitude()));
	BOOST_CHECK_EQUAL (14419910165203, static_cast<long long int>(10000000000000*gp.getLongitude()));
    }
    {
	// Brest LambertIIe
	Point2D p2d (95151.0, 2398703.0);
	GeoPoint gp = WGS84FromLambert(p2d);

	std::cerr << gp << std::endl;
	BOOST_CHECK_EQUAL (48.389844624560034, gp.getLatitude ());
	BOOST_CHECK_EQUAL (-448683051106722, static_cast<long long int>(100000000000000*gp.getLongitude()));
    }
}

BOOST_AUTO_TEST_CASE (testToLambertIIe)
{
	{
		// Toulouse LambertIIe
		GeoPoint gp(43.60435797, 1.44199102, 0);
		Point2D p2d(LambertFromWGS84(gp));

		BOOST_CHECK_EQUAL (527674, round(p2d.getX()));
		BOOST_CHECK_EQUAL (1845127, round(p2d.getY()));
	}
	{
		// Brest LambertIIe
		GeoPoint gp(48.3898446, -4.486830511, 0);
		Point2D p2d(LambertFromWGS84(gp));

		BOOST_CHECK_EQUAL (95151, round(p2d.getX()));
		BOOST_CHECK_EQUAL (2398703, round(p2d.getY()));
	}
}

BOOST_AUTO_TEST_CASE (testBijectiveLambertIIe)
{
	GeoPoint gp(43.60435797, 1.44199102, 0);
	GeoPoint gp2(WGS84FromLambert(LambertFromWGS84(gp)));
	BOOST_CHECK_EQUAL(round(100000 * gp.getLatitude()), round(100000 * gp2.getLatitude()));
	BOOST_CHECK_EQUAL(round(100000 * gp.getLongitude()), round(100000 * gp2.getLongitude()));

	Point2D p(95151.0, 2398703.0);
	Point2D p2(LambertFromWGS84(WGS84FromLambert(p)));
	BOOST_CHECK_EQUAL(p.getX(), round(p2.getX()));
	BOOST_CHECK_EQUAL(p.getY(), round(p2.getY()));
}
