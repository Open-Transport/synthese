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
CalendarTest::testCalendarSimpleMarking ()
{
    Calendar calendar;
    
    CPPUNIT_ASSERT_EQUAL (0, calendar.getNbMarkedDates ());
	CPPUNIT_ASSERT_EQUAL (Date::UNKNOWN_DATE, calendar.getFirstMarkedDate ());
    CPPUNIT_ASSERT_EQUAL (Date::UNKNOWN_DATE, calendar.getLastMarkedDate ());

    Date d02042006 (2, 4, 2006);
    Date d14072006 (14, 7, 2006);
    Date d16072006 (16, 7, 2006);
    Date d17072006 (17, 7, 2006);
    Date d18072006 (18, 7, 2006);
	Date d06082006 (6, 8, 2006);

    calendar.mark (d16072006);
    CPPUNIT_ASSERT (calendar.isMarked (d16072006));
    CPPUNIT_ASSERT_EQUAL (1, calendar.getNbMarkedDates ());
    CPPUNIT_ASSERT_EQUAL (d16072006, calendar.getFirstMarkedDate ());
    CPPUNIT_ASSERT_EQUAL (d16072006, calendar.getLastMarkedDate ());
    
    calendar.mark (d18072006);
    CPPUNIT_ASSERT (calendar.isMarked (d16072006));
    CPPUNIT_ASSERT (calendar.isMarked (d17072006) == false);
    CPPUNIT_ASSERT (calendar.isMarked (d18072006));
    CPPUNIT_ASSERT_EQUAL (2, calendar.getNbMarkedDates ());
    CPPUNIT_ASSERT_EQUAL (d16072006, calendar.getFirstMarkedDate ());
    CPPUNIT_ASSERT_EQUAL (d18072006, calendar.getLastMarkedDate ());

    calendar.mark (d14072006);
    calendar.mark (d17072006);
    CPPUNIT_ASSERT (calendar.isMarked (d14072006));
    CPPUNIT_ASSERT (calendar.isMarked (d16072006));
    CPPUNIT_ASSERT (calendar.isMarked (d17072006));
    CPPUNIT_ASSERT (calendar.isMarked (d18072006));
    CPPUNIT_ASSERT_EQUAL (4, calendar.getNbMarkedDates ());
    CPPUNIT_ASSERT_EQUAL (d14072006, calendar.getFirstMarkedDate ());
    CPPUNIT_ASSERT_EQUAL (d18072006, calendar.getLastMarkedDate ());

    calendar.mark (d02042006);
    CPPUNIT_ASSERT (calendar.isMarked (d02042006));
    CPPUNIT_ASSERT (calendar.isMarked (d14072006));
    CPPUNIT_ASSERT (calendar.isMarked (d16072006));
    CPPUNIT_ASSERT (calendar.isMarked (d17072006));
    CPPUNIT_ASSERT (calendar.isMarked (d18072006));
    CPPUNIT_ASSERT_EQUAL (5, calendar.getNbMarkedDates ());
    CPPUNIT_ASSERT_EQUAL (d02042006, calendar.getFirstMarkedDate ());
    CPPUNIT_ASSERT_EQUAL (d18072006, calendar.getLastMarkedDate ());

    calendar.mark (d14072006, false);
    calendar.mark (d16072006, false);
    CPPUNIT_ASSERT (calendar.isMarked (d02042006));
    CPPUNIT_ASSERT (calendar.isMarked (d17072006));
    CPPUNIT_ASSERT (calendar.isMarked (d18072006));
    CPPUNIT_ASSERT_EQUAL (3, calendar.getNbMarkedDates ());
    CPPUNIT_ASSERT_EQUAL (d02042006, calendar.getFirstMarkedDate ());
    CPPUNIT_ASSERT_EQUAL (d18072006, calendar.getLastMarkedDate ());

    calendar.mark (d02042006, false);
    CPPUNIT_ASSERT (calendar.isMarked (d17072006));
    CPPUNIT_ASSERT (calendar.isMarked (d18072006));
    CPPUNIT_ASSERT_EQUAL (2, calendar.getNbMarkedDates ());
    CPPUNIT_ASSERT_EQUAL (d17072006, calendar.getFirstMarkedDate ());
    CPPUNIT_ASSERT_EQUAL (d18072006, calendar.getLastMarkedDate ());

    calendar.mark (d06082006);
    calendar.mark (d18072006, false);
    CPPUNIT_ASSERT (calendar.isMarked (d17072006));
    CPPUNIT_ASSERT (calendar.isMarked (d06082006));
    CPPUNIT_ASSERT_EQUAL (2, calendar.getNbMarkedDates ());
    CPPUNIT_ASSERT_EQUAL (d17072006, calendar.getFirstMarkedDate ());
    CPPUNIT_ASSERT_EQUAL (d06082006, calendar.getLastMarkedDate ());

    calendar.mark (d06082006, false);
    CPPUNIT_ASSERT (calendar.isMarked (d17072006));
    CPPUNIT_ASSERT_EQUAL (1, calendar.getNbMarkedDates ());
    CPPUNIT_ASSERT_EQUAL (d17072006, calendar.getFirstMarkedDate ());
    CPPUNIT_ASSERT_EQUAL (d17072006, calendar.getLastMarkedDate ());

    calendar.mark (d17072006, false);
    CPPUNIT_ASSERT_EQUAL (0, calendar.getNbMarkedDates ());
	CPPUNIT_ASSERT_EQUAL (Date::UNKNOWN_DATE, calendar.getFirstMarkedDate ());
    CPPUNIT_ASSERT_EQUAL (Date::UNKNOWN_DATE, calendar.getLastMarkedDate ());

}



void 
CalendarTest::testCalendarLogicalAnd ()
{
    Calendar calendar1;
    calendar1.mark (Date (2, 4, 2006));
    calendar1.mark (Date (4, 4, 2006));
    calendar1.mark (Date (5, 4, 2006));

    Calendar calendar2;
    calendar2.mark (Date (1, 4, 2006));
    calendar2.mark (Date (4, 4, 2006));
    calendar2.mark (Date (5, 4, 2006));
    calendar2.mark (Date (6, 4, 2006));

	Calendar calendar1and2 = calendar1 & calendar2;
    CPPUNIT_ASSERT_EQUAL (2, calendar1and2.getNbMarkedDates ());
    CPPUNIT_ASSERT (calendar1and2.isMarked (Date (4, 4, 2006)));
    CPPUNIT_ASSERT (calendar1and2.isMarked (Date (5, 4, 2006)));
    CPPUNIT_ASSERT_EQUAL (Date (4, 4, 2006), calendar1and2.getFirstMarkedDate ());
    CPPUNIT_ASSERT_EQUAL (Date (5, 4, 2006), calendar1and2.getLastMarkedDate ());

	Calendar calendar2and1 = calendar2 & calendar1;
    CPPUNIT_ASSERT_EQUAL (2, calendar2and1.getNbMarkedDates ());
    CPPUNIT_ASSERT (calendar2and1.isMarked (Date (4, 4, 2006)));
    CPPUNIT_ASSERT (calendar2and1.isMarked (Date (5, 4, 2006)));
    CPPUNIT_ASSERT_EQUAL (Date (4, 4, 2006), calendar2and1.getFirstMarkedDate ());
    CPPUNIT_ASSERT_EQUAL (Date (5, 4, 2006), calendar2and1.getLastMarkedDate ());

}


void 
CalendarTest::testCalendarLogicalOr ()
{
    Calendar calendar1;
    calendar1.mark (Date (2, 4, 2006));
    calendar1.mark (Date (4, 4, 2006));
    calendar1.mark (Date (5, 4, 2006));

    Calendar calendar2;
    calendar2.mark (Date (1, 4, 2006));
    calendar2.mark (Date (4, 4, 2006));
    calendar2.mark (Date (5, 4, 2006));
    calendar2.mark (Date (6, 4, 2006));

	Calendar calendar1or2 = calendar1 | calendar2;
    CPPUNIT_ASSERT_EQUAL (5, calendar1or2.getNbMarkedDates ());
    CPPUNIT_ASSERT (calendar1or2.isMarked (Date (1, 4, 2006)));
    CPPUNIT_ASSERT (calendar1or2.isMarked (Date (2, 4, 2006)));
    CPPUNIT_ASSERT (calendar1or2.isMarked (Date (4, 4, 2006)));
    CPPUNIT_ASSERT (calendar1or2.isMarked (Date (5, 4, 2006)));
    CPPUNIT_ASSERT (calendar1or2.isMarked (Date (6, 4, 2006)));
    CPPUNIT_ASSERT_EQUAL (Date (1, 4, 2006), calendar1or2.getFirstMarkedDate ());
    CPPUNIT_ASSERT_EQUAL (Date (6, 4, 2006), calendar1or2.getLastMarkedDate ());

	Calendar calendar2or1 = calendar2 | calendar1;
    CPPUNIT_ASSERT_EQUAL (5, calendar2or1.getNbMarkedDates ());
    CPPUNIT_ASSERT (calendar2or1.isMarked (Date (1, 4, 2006)));
    CPPUNIT_ASSERT (calendar2or1.isMarked (Date (2, 4, 2006)));
    CPPUNIT_ASSERT (calendar2or1.isMarked (Date (4, 4, 2006)));
    CPPUNIT_ASSERT (calendar2or1.isMarked (Date (5, 4, 2006)));
    CPPUNIT_ASSERT (calendar2or1.isMarked (Date (6, 4, 2006)));
    CPPUNIT_ASSERT_EQUAL (Date (1, 4, 2006), calendar2or1.getFirstMarkedDate ());
    CPPUNIT_ASSERT_EQUAL (Date (6, 4, 2006), calendar2or1.getLastMarkedDate ());



}




}
}


