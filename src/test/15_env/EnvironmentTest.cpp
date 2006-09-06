#include "EnvironmentTest.h"

#include "15_env/Environment.h"

#include "15_env/Axis.h"
#include "15_env/Calendar.h"
#include "15_env/Line.h"

#include "04_time/Hour.h"
#include "04_time/Schedule.h"

#include <iostream>


using synthese::time::Date;
using synthese::time::Hour;
using synthese::time::Schedule;



namespace synthese
{
namespace env
{


void 
EnvironmentTest::setUp () 
{

}


void 
EnvironmentTest::tearDown () 
{

} 


void 
EnvironmentTest::testEnvironmentConstruction0 ()
{
/*
    Environment env (0);

    // Cities
    {
	env.getCities ().add (new City (0, "city0")); 
    }

    // Axes
    {
	env.getAxes ().add (new Axis (0, "axis0")); 
	env.getAxes ().add (new Axis (1, "axis1")); 
    }

    // Lines
    {
	env.getLines ().add (new Line (0, "line0", env.getAxes().get (0))); 
	env.getLines ().add (new Line (1, "line1", env.getAxes().get (0))); 
    }

    // Roads
    {
	env.getRoads ().add (new Road (0, "road1", env.getCities ().get (0), Road::ROAD_TYPE_STREET));
	env.getRoads ().add (new Road (1, "road2", env.getCities ().get (0), Road::ROAD_TYPE_STREET));
	env.getRoads ().add (new Road (2, "road3", env.getCities ().get (0), Road::ROAD_TYPE_STREET));
	env.getRoads ().add (new Road (3, "road4", env.getCities ().get (0), Road::ROAD_TYPE_STREET));
    }

    // Connection places
    {
	env.getConnectionPlaces ().add (new ConnectionPlace (0, "cp1", env.getCities ().get (0)));
	env.getConnectionPlaces ().add (new ConnectionPlace (1, "cp2", env.getCities ().get (0)));
	env.getConnectionPlaces ().add (new ConnectionPlace (2, "cp3", env.getCities ().get (0)));
    }


    // Addresses
    { 
	int uid = 0;

	// road1
	env.getAddresses ().add (new Address (uid++, env.getRoads ().get (0), 5.0));
	env.getAddresses ().add (new Address (uid++, env.getConnectionPlaces ().get (0), 0, env.getRoads ().get (0), 25.0));
	env.getAddresses ().add (new Address (uid++, env.getConnectionPlaces ().get (2), 0, env.getRoads ().get (0), 90.0));
	env.getAddresses ().add (new Address (uid++, env.getRoads ().get (0), 150.0));
	
	// road2
	env.getAddresses ().add (new Address (uid++, env.getRoads ().get (1), 10.0));
	env.getAddresses ().add (new Address (uid++, env.getConnectionPlaces ().get (2), 1, env.getRoads ().get (1), 55.0));
	env.getAddresses ().add (new Address (uid++, env.getRoads ().get (1), 90.0));
	env.getAddresses ().add (new Address (uid++, env.getRoads ().get (1), 165.0));

	//road3
	env.getAddresses ().add (new Address (uid++, env.getRoads ().get (2), 0.0));
	env.getAddresses ().add (new Address (uid++, env.getConnectionPlaces ().get (2), 2, env.getRoads ().get (2), 30.0));
	env.getAddresses ().add (new Address (uid++, env.getRoads ().get (2), 55.0));
	env.getAddresses ().add (new Address (uid++, env.getConnectionPlaces ().get (1), 0, env.getRoads ().get (2), 70.0));
	env.getAddresses ().add (new Address (uid++, env.getRoads ().get (2), 105.0));

	//road4
	env.getAddresses ().add (new Address (uid++, env.getRoads ().get (3), 5.0));
	env.getAddresses ().add (new Address (uid++, env.getRoads ().get (3), 25.0));
	env.getAddresses ().add (new Address (uid++, env.getConnectionPlaces ().get (1), 1, env.getRoads ().get (3), 75.0));
	env.getAddresses ().add (new Address (uid++, env.getConnectionPlaces ().get (0), 1, env.getRoads ().get (3), 115.0));
	env.getAddresses ().add (new Address (uid++, env.getRoads ().get (3), 195.0));

    }


    

    // Road chunks



    // Services
    {
	// line0
	env.getScheduledServices ().add (new ScheduledService (0, "0700", env.getLines().get (0), 
					     Schedule (Hour (7, 0), 0))); 
	env.getScheduledServices ().add (new ScheduledService (1, "0800", env.getLines().get (0), 
					     Schedule (Hour (7, 0), 0))); 
	env.getScheduledServices ().add (new ScheduledService (2, "1500", env.getLines().get (0), 
					     Schedule (Hour (7, 0), 0))); 
	
	// line1
	env.getScheduledServices ().add (new ScheduledService (3, "0710", env.getLines().get (0), 
					     Schedule (Hour (7, 10), 0))); 
	env.getScheduledServices ().add (new ScheduledService (4, "0810", env.getLines().get (0), 
					     Schedule (Hour (8, 10), 0))); 
	env.getScheduledServices ().add (new ScheduledService (5, "1510", env.getLines().get (0), 
					     Schedule (Hour (15, 10), 0))); 


	// road0 (continuous service)
        // ...
	env.getContinuousServices ().add (new ContinuousService (0, "0700", env.getRoads ().get (0),
								 Schedule (Hour (7, 0), 0), 720, 10)); 

	// road1 (continuous service)
        // ...

	// road2 (continuous service)
        // ...

	// road3 (continuous service)
        // ...
	
    }
    
*/

}



}
}

