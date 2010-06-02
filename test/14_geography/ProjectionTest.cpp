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
	GeoPoint gp = WGS84FromLambert(p2d);
	
	BOOST_CHECK_EQUAL (43.604357973336143, gp.getLatitude ());
	BOOST_CHECK_EQUAL (1.44199101652035731, gp.getLongitude ());
	BOOST_CHECK_EQUAL (144.23713738757579, gp.getEllipsoidHeight ());
    }
    {
	// Brest LambertIIe
	Point2D p2d (95151.0, 2398703.0);
	GeoPoint gp = WGS84FromLambert(p2d);

	std::cerr << gp << std::endl;
	BOOST_CHECK_EQUAL (48.389844624560034, gp.getLatitude ());
	BOOST_CHECK_EQUAL (-4.48683051106722, gp.getLongitude ());
	BOOST_CHECK_EQUAL (148.25864991357594, gp.getEllipsoidHeight ());
    }
}

BOOST_AUTO_TEST_CASE (testToLambertIIe)
{
	// ALG0001
	{
		double L1(ALG0001(0.872664626,0.08199188998));
		double L2(ALG0001(-0.3,0.08199188998));
		double L3(ALG0001(0.19998903370,0.08199188998));

		BOOST_CHECK_EQUAL (1.00552653649, L1);
		BOOST_CHECK_EQUAL (-0.30261690063, L2);
		BOOST_CHECK_EQUAL (0.200000000009, L3);
	}

	// ALG0003
	{
		Point2D p(ALG0003(0.145512099,0.872664626,0.824832568,600000,5657656.674,11603796.9767,0.760405966,0.04079234433));

		BOOST_CHECK_EQUAL (1029705.0818, ALG0003(0.145512099,0.872664626,0.824832568,600000,5657656.674,11603796.9767,0.760405966,0.04079234433).getX());
		BOOST_CHECK_EQUAL (272723.851, p.getY());
	}

	{
		// Toulouse LambertIIe
		GeoPoint gp(43.604357973336143, 1.44199101652035731, 144.23713738757579);
		Point2D p2d(LambertFromWGS84(gp));

		BOOST_CHECK_EQUAL (527674.0, p2d.getX());
		BOOST_CHECK_EQUAL (1845128.0, p2d.getY());
	}
	{
		// Brest LambertIIe
		GeoPoint gp(48.389844624560034, -4.48683051106722, 148.25864991357594);
		Point2D p2d(LambertFromWGS84(gp));

		BOOST_CHECK_EQUAL (95151.0, p2d.getX());
		BOOST_CHECK_EQUAL (2398703.0, p2d.getY());
	}
}

