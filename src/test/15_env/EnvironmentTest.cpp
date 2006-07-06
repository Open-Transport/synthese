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

    // Calendars
    {
	Calendar* calendar;
	Date firstDay2006 (1, 1, 2006);
	Date lastDay2006 (31, 12, 2006);
	
	// Every day of year
	calendar = new Calendar (0, 2006, 2006, "calendar0");
	calendar->setMark (firstDay2006, lastDay2006);
	env.getCalendars ().add (calendar);
    }

    // Lines
    {
	env.getLines ().add (new Line (0, "line0", env.getAxes().get (0), 2000, 2010)); 
	env.getLines ().add (new Line (1, "line1", env.getAxes().get (0), 2000, 2010)); 
    }

    // Addresses
    

    // Road chunks


    // Roads


    // Services
    {
	// line0
	env.getScheduledServices ().add (new ScheduledService (0, "0700", env.getLines().get (0), 
					     env.getCalendars ().get (0), Schedule (Hour (7, 0), 0))); 
	env.getScheduledServices ().add (new ScheduledService (1, "0800", env.getLines().get (0), 
					     env.getCalendars ().get (0), Schedule (Hour (7, 0), 0))); 
	env.getScheduledServices ().add (new ScheduledService (2, "1500", env.getLines().get (0), 
					     env.getCalendars ().get (0), Schedule (Hour (7, 0), 0))); 
	
	// line1
	env.getScheduledServices ().add (new ScheduledService (3, "0710", env.getLines().get (0), 
					     env.getCalendars ().get (0), Schedule (Hour (7, 0), 0))); 
	env.getScheduledServices ().add (new ScheduledService (4, "0810", env.getLines().get (0), 
					     env.getCalendars ().get (0), Schedule (Hour (7, 0), 0))); 
	env.getScheduledServices ().add (new ScheduledService (5, "1510", env.getLines().get (0), 
					     env.getCalendars ().get (0), Schedule (Hour (7, 0), 0))); 

	// road0 (continuous service)
        // ...

	// road1 (continuous service)
        // ...

	// road2 (continuous service)
        // ...

	// road3 (continuous service)
        // ...
	
    }
    
    
    

    

}



}
}

