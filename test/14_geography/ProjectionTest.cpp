
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

#include "GeoPoint.h"
#include "GeographyModule.h"
#include "CoordinatesSystem.hpp"

using namespace synthese::geography;
using namespace geos::geom;
using namespace std;

BOOST_AUTO_TEST_CASE (testFromLambertIIe)
{
	GeographyModule::PreInit();
	GeographyModule::ChangeInstanceCoordinatesSystem(string(), "EPSG:27572");

	// Toulouse LambertIIe
	GeoPoint gp1(
		Coordinate(527674.0, 1845128.0),
		CoordinatesSystem::GetCoordinatesSystem("EPSG:27572")
	);
	BOOST_CHECK_CLOSE(43.60435799, gp1.getLatitude(), 0.00001);
	BOOST_CHECK_CLOSE(1.44199101, gp1.getLongitude(), 0.00001);

	// Brest
	GeoPoint gp2(
		Coordinate(95151.0, 2398703.0),
		CoordinatesSystem::GetCoordinatesSystem("EPSG:27572")
	);
	BOOST_CHECK_CLOSE(48.38984464, gp2.getLatitude(), 0.00001);
	BOOST_CHECK_CLOSE(-4.48683053, gp2.getLongitude(), 0.00001);

	// Strasbourg
	GeoPoint gp3(
		Coordinate(999096.6, 2412064.0),
		CoordinatesSystem::GetCoordinatesSystem("EPSG:27572")
	);
	BOOST_CHECK_CLOSE(48.5836, gp3.getLatitude(), 0.00001);
	BOOST_CHECK_CLOSE(7.74806, gp3.getLongitude(), 0.00001);
}

BOOST_AUTO_TEST_CASE (testToLambertIIe)
{
	// Toulouse LambertIIe
	GeoPoint gp1(1.44199101, 43.60435799);
	BOOST_CHECK_CLOSE(527674.0, gp1.x, 1.0);
	BOOST_CHECK_CLOSE(1845128.0, gp1.y, 1.0);

	// Brest LambertIIe
	GeoPoint gp2(-4.48683053, 48.38984464);
	BOOST_CHECK_CLOSE(95151.0, gp2.x, 1.0);
	BOOST_CHECK_CLOSE(2398703.0, gp2.y, 1.0);

	// Strasbourg
	GeoPoint gp3(7.74806, 48.5836);
	BOOST_CHECK_CLOSE(999096.6, gp3.x, 1.0);
	BOOST_CHECK_CLOSE(2412064.0, gp3.y, 1.0);
}
