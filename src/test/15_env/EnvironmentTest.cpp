#include "EnvironmentTest.h"

#include "15_env/Environment.h"

#include "15_env/Axis.h"
#include "15_env/Calendar.h"
#include "15_env/Line.h"

#include <iostream>


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

    // Axes
    env.getAxes ().add (new Axis (0, "axis0")); 
    env.getAxes ().add (new Axis (1, "axis1")); 

    // Calendars
    
    // env.getCalendars ().add ();

    // Services
    
    
    // Lines
    env.getLines ().add (new Line (0, "line0", env.getAxes().get (0), 2000, 2010)); 

    

}



}
}

