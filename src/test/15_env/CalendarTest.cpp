#include "CalendarTest.h"

#include "15_env/Calendar.h"
#include "04_time/Date.h"

#include <iostream>


using synthese::time::Date;


namespace synthese
{
namespace env
{


void 
CalendarTest::setUp () 
{

}


void 
CalendarTest::tearDown () 
{

} 


void 
CalendarTest::testCalendarConstruction0 ()
{
    int year = 2006;
    Calendar calendar (0, year, year, "calendar0");
    
    // Check no day is selected for the year 2006
    for (int m=1; m<=12; ++m)
    {
	for (int d=1; d<=31; ++d)
	{
	    Date date (d, m, year);
	    CPPUNIT_ASSERT (calendar.isMarked (date) == false);
	}
    }

    // Check marking a day out of year range does nothing
    Date date (16, 7, 2008);
    calendar.setMark (date);
    
    // Check no day is selected for the year 2006
    for (int m=1; m<=12; ++m)
    {
	for (int d=1; d<=31; ++d)
	{
	    Date date (d, m, year);
	    CPPUNIT_ASSERT (calendar.isMarked (date) == false);
	}
    }
    
    // Check marking a proper day
    Date markedDate (16, 7, 2006);
    calendar.setMark (markedDate);
    CPPUNIT_ASSERT (calendar.isMarked (markedDate));
	    
    // Check no day but the 16/07/2006 is marked
    for (int m=1; m<=12; ++m)
    {
	for (int d=1; d<=31; ++d)
	{
	    Date date (d, m, year);
	    CPPUNIT_ASSERT (calendar.isMarked (date) == (markedDate == date));
	}
    }
    
    // Mark an interval of dates
    Date startMarkedDate (2,  2, 2006);
    Date endMarkedDate   (6, 2, 2006);
    calendar.setMark (startMarkedDate, endMarkedDate);
    
    CPPUNIT_ASSERT (calendar.isMarked (Date (1, 2, 2006)) == false);
    CPPUNIT_ASSERT (calendar.isMarked (Date (2, 2, 2006)));
    CPPUNIT_ASSERT (calendar.isMarked (Date (3, 2, 2006)));
    CPPUNIT_ASSERT (calendar.isMarked (Date (4, 2, 2006)));
    CPPUNIT_ASSERT (calendar.isMarked (Date (5, 2, 2006)));
    CPPUNIT_ASSERT (calendar.isMarked (Date (6, 2, 2006)));
    CPPUNIT_ASSERT (calendar.isMarked (Date (7, 2, 2006)) == false);
    

}



}
}

