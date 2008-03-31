#include "14_geography/Projection.h"
#include "06_geometry/Point2D.h"

#include <boost/test/auto_unit_test.hpp>

using namespace synthese::geography;
using namespace synthese::geometry;


BOOST_AUTO_TEST_CASE (testFromLambertIIe)
{
    {
	// Toulouse LambertIIe
	Point2D p2d (527674.0, 1845128.0);
	GeoPoint gp = FromLambertIIe (p2d);
	
	BOOST_CHECK_EQUAL (43.604357973336143, gp.getLatitude ());
	BOOST_CHECK_EQUAL (1.44199101652035731, gp.getLongitude ());
	BOOST_CHECK_EQUAL (144.23713738757579, gp.getEllipsoidHeight ());
    }
    {
	// Brest LambertIIe
	Point2D p2d (95151.0, 2398703.0);
	GeoPoint gp = FromLambertIIe (p2d);

	std::cerr << gp << std::endl;
	BOOST_CHECK_EQUAL (48.389844624560034, gp.getLatitude ());
	BOOST_CHECK_EQUAL (-4.48683051106722, gp.getLongitude ());
	BOOST_CHECK_EQUAL (148.25864991357594, gp.getEllipsoidHeight ());
    }

}


