#include "33_route_planner/RoutePlanner.h"


#include <boost/test/auto_unit_test.hpp>

using namespace synthese::routeplanner;
using namespace synthese::util;


BOOST_AUTO_TEST_CASE (testFake)
{
	RoutePlanner r(
		EnvModule::getConnectionPlaces().get(1970324837184594)
		, EnvModule::getConnectionPlaces().get(1970324837184595)
		, AccessParameters()
		, PlanningOrder()
		, DateTime(Date(), Hour(2,30))
		, DateTime(Date(), Hour(23,59))
		, UNKNOWN_VALUE
		);
	Journeys jv = r.computeJourneySheetDepartureArrival();
}


