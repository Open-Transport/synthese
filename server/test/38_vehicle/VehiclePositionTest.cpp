
/** Vehicle position test
    @file VehiclePositionTest.cpp

    This file belongs to the SYNTHESE project (public transportation specialized software)
    Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "Vehicle.hpp"
#include "VehiclePosition.hpp"
#include "VehicleInformationsService.hpp"
#include "VehicleModule.hpp"
#include "VehicleModuleRegister.cpp"
#include "WithGeometry.hpp"
#include "CoordinatesSystem.hpp"
#include "DynamicRequest.h"
#include "HTTPRequest.hpp"

#include <boost/shared_ptr.hpp>
#include <geos/geom/Geometry.h>

#include <iostream>

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/test/output_test_stream.hpp>


using namespace synthese;
using namespace synthese::util;
using namespace synthese::vehicle;
using namespace synthese::server;
using namespace std;


BOOST_AUTO_TEST_CASE (CoordinateSystemConversion)
{

	// Initialise the coordinate systems for this test

	CoordinatesSystem::AddCoordinatesSystem(
		4326,
		"WGS 84",
		"+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs"
	);

	CoordinatesSystem::AddCoordinatesSystem(
		21781,
		"CH1903 / LV03",
		"+proj=somerc +lat_0=46.95240555555556 +lon_0=7.439583333333333 +x_0=600000 +y_0=200000 +ellps=bessel +towgs84=674.374,15.056,405.346,0,0,0,0 +units=m +no_defs"
	);

	// Assign default SRID

	CoordinatesSystem::SetStorageCoordinatesSystem(
		CoordinatesSystem::GetCoordinatesSystem(4326)
	);
	CoordinatesSystem::SetDefaultCoordinatesSystems(4326);

	// Object creation

	ParametersMap pm;

	boost::shared_ptr<geos::geom::Point> point(
		CoordinatesSystem::GetCoordinatesSystem(4326).createPoint(
			5.005698,
			52.176040
	)	);

	VehicleModule::GetCurrentVehiclePosition().setGeometry(point);

	// Register the module
	synthese::vehicle::moduleRegister();

	// No conversion without a given SRID
	VehicleModule::GetCurrentVehiclePosition().toParametersMap(pm,false);
	BOOST_CHECK_CLOSE( pm.get<double>("x"), 5.005698, 0.00001 );
	BOOST_CHECK_CLOSE( pm.get<double>("y"), 52.176040, 0.00001 );

	// Convert to Swiss coordinate
	pm.clear();
	pm.insert(Vehicle::PARAMETER_SRID, 21781);
	VehicleModule::GetCurrentVehiclePosition().toParametersMap(pm,true);
	BOOST_CHECK_CLOSE( pm.get<double>("x"), 432905.773503, 0.001 );
	BOOST_CHECK_CLOSE( pm.get<double>("y"), 784537.385333, 0.001 );

	// HTTP request objects

	HTTPRequest req;
	stringstream s;

	pm.clear();
	Function * vhi = util::Factory<Function>::create("vehicle_informations");
	vhi->_setFromParametersMap(pm);
	pm = vhi->run(s,Request());
	BOOST_CHECK_CLOSE( pm.get<double>("x"), 5.005698, 0.00001 );
	BOOST_CHECK_CLOSE( pm.get<double>("y"), 52.176040, 0.00001 );

	// VehicleInformation service with a srid
	pm.clear();
	pm.insert(Vehicle::PARAMETER_SRID, 21781);
	vhi->_setFromParametersMap(pm);
	pm = vhi->run(s,Request());
	BOOST_CHECK_CLOSE( pm.get<double>("x"), 432905.773503, 0.001 );
	BOOST_CHECK_CLOSE( pm.get<double>("y"), 784537.385333, 0.001 );

}
