
/** ProjectionTest class implementation.
	@file ProjectionTest.cpp

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

#pragma GCC diagmostic ignored "-Werror=sign-compare"
#include <boost/test/auto_unit_test.hpp>
#pragma GCC diagnostic pop
#include <boost/filesystem.hpp>
#include <geos/geom/Point.h>

#include "GeographyModule.h"
#include "CoordinatesSystem.hpp"
#include "DBModule.h"
#include "101_sqlite/SQLiteDB.h"

using namespace synthese::geography;
using namespace geos::geom;
using namespace std;
using namespace boost;
using namespace synthese;
using namespace synthese::db;
using namespace synthese::server;


BOOST_AUTO_TEST_CASE (testFromLambertIIe)
{
	boost::filesystem::path _dbPath = boost::filesystem::complete("test_db.db", boost::filesystem::initial_path());
	boost::filesystem::remove(_dbPath);
	string _connectionString = "sqlite://debug=1,path=" + _dbPath.string();
	SQLiteDB::integrate();
	DBModule::SetConnectionString(_connectionString );
	ModuleClassTemplate<DBModule>::PreInit();
	ModuleClassTemplate<DBModule>::Init();

	// Toulouse LambertIIe
	boost::shared_ptr<Point> gp1(
		CoordinatesSystem::GetCoordinatesSystem(27572).createPoint(
			527674.0,
			1845128.0
	)	);
	boost::shared_ptr<Point> gp1wgs(
		CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(*gp1)
	);
	BOOST_CHECK_CLOSE(1.44199101, gp1wgs->getX(), 0.00001);
	BOOST_CHECK_CLOSE(43.60435799, gp1wgs->getY(), 0.00001);

	// Brest
	boost::shared_ptr<Point> gp2(
		CoordinatesSystem::GetCoordinatesSystem(27572).createPoint(
			95151.0,
			2398703.0
	)	);
	boost::shared_ptr<Point> gp2wgs(
		CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(*gp2)
	);
	BOOST_CHECK_CLOSE(-4.48683053, gp2wgs->getX(), 0.00001);
	BOOST_CHECK_CLOSE(48.38984464, gp2wgs->getY(), 0.00001);

	// Strasbourg
	boost::shared_ptr<Point> gp3(
		CoordinatesSystem::GetCoordinatesSystem(27572).createPoint(
			999096.6,
			2412064.0
	)	);
	boost::shared_ptr<Point> gp3wgs(
		CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(*gp3)
	);
	BOOST_CHECK_CLOSE(7.74806, gp3wgs->getX(), 0.00001);
	BOOST_CHECK_CLOSE(48.5836, gp3wgs->getY(), 0.00001);
}

BOOST_AUTO_TEST_CASE (testToLambertIIe)
{
	GeographyModule::PreInit();

	// Toulouse LambertIIe
	boost::shared_ptr<Point> gp1(
		CoordinatesSystem::GetCoordinatesSystem(4326).createPoint(
			1.44199101,
			43.60435799
	)	);
	boost::shared_ptr<Point> gp1wgs(
		CoordinatesSystem::GetCoordinatesSystem(27572).convertPoint(*gp1)
	);
	BOOST_CHECK_CLOSE(527674.0, gp1wgs->getX(), 1.0);
	BOOST_CHECK_CLOSE(1845128.0, gp1wgs->getY(), 1.0);

	// Brest LambertIIe
	boost::shared_ptr<Point> gp2(
		CoordinatesSystem::GetCoordinatesSystem(4326).createPoint(
			-4.48683053,
			48.38984464
	)	);
	boost::shared_ptr<Point> gp2wgs(
		CoordinatesSystem::GetCoordinatesSystem(27572).convertPoint(*gp2)
	);
	BOOST_CHECK_CLOSE(95151.0, gp2wgs->getX(), 1.0);
	BOOST_CHECK_CLOSE(2398703.0, gp2wgs->getY(), 1.0);

	// Strasbourg
	boost::shared_ptr<Point> gp3(
		CoordinatesSystem::GetCoordinatesSystem(4326).createPoint(
			7.74806,
			48.5836
	)	);
	boost::shared_ptr<Point> gp3wgs(
		CoordinatesSystem::GetCoordinatesSystem(27572).convertPoint(*gp3)
	);
	BOOST_CHECK_CLOSE(999096.6, gp3wgs->getX(), 1.0);
	BOOST_CHECK_CLOSE(2412064.0, gp3wgs->getY(), 1.0);

	ModuleClassTemplate<DBModule>::End();

}
